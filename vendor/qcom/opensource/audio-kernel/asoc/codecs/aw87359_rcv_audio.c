/*
 * aw87359_rcv_audio.c   aw87359_rcv pa module
 *
 * Copyright (c) 2019 AWINIC Technology CO., LTD
 *
 *  Author: Joseph <zhangzetao@awinic.com.cn>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#include <linux/i2c.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <linux/firmware.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/dma-mapping.h>
#include <linux/gameport.h>
#include <linux/moduleparam.h>
#include <linux/mutex.h>
//#include <linux/wakelock.h>
#include <linux/timer.h>
#include <linux/workqueue.h>
#include <linux/hrtimer.h>
#include <linux/aw87359_rcv_audio.h>
#include <linux/module.h>

/*****************************************************************
* aw87359_rcv marco
******************************************************************/
#define AW87359_RCV_I2C_NAME        "aw87359_rcv_pa"
#define AW87359_RCV_DRIVER_VERSION  "V1.3.2"

#define USE_PINCTRL_FOR_RESET
/*************************************************************************
 * aw87359_rcv variable
 ************************************************************************/
 
//unsigned char aw87359_rcv_audio_dspk(void);
//unsigned char aw87359_rcv_audio_abrcv(void);
//unsigned char aw87359_rcv_audio_off(void);
extern unsigned char (*awinic_rcv_audio_spk_pf) (void);
extern unsigned char (*awinic_rcv_audio_rcv_pf) (void);
extern unsigned char (*awinic_rcv_audio_spk_off_pf) (void);
extern int (*awinic_set_rcv_voltage_pf) (int val);

struct aw87359_rcv *aw87359_rcv;
struct aw87359_rcv_container *aw87359_rcv_dspk_cnt;
struct aw87359_rcv_container *aw87359_rcv_drcv_cnt;
struct aw87359_rcv_container *aw87359_rcv_abspk_cnt;
struct aw87359_rcv_container *aw87359_rcv_abrcv_cnt;

static char *aw87359_rcv_dspk_name = "aw87359_rcv_dspk.bin";
static char *aw87359_rcv_drcv_name = "aw87359_rcv_drcv.bin";
static char *aw87359_rcv_abspk_name = "aw87359_rcv_abspk.bin";
static char *aw87359_rcv_abrcv_name = "aw87359_rcv_abrcv.bin";

unsigned int dspk_load_cont;
unsigned int drcv_load_cont;
unsigned int abspk_load_cont;
unsigned int abrcv_load_cont;

#ifdef USE_PINCTRL_FOR_RESET
static struct pinctrl *pinctrl;
static struct pinctrl_state *pins_reset_on;
static struct pinctrl_state *pins_reset_off;
#endif

//#ifdef VENDOR_EDIT
// Ming.Liu@PSW.MM.AudioDriver.feature.1446118, 2020/07/26, Add for PA_BOOST_VOLTAGE
//#ifdef OPPO_AUDIO_PA_BOOST_VOLTAGE
static int curr_rcv_voltage = 0;
//#endif /* OPPO_AUDIO_PA_BOOST_VOLTAGE */
//#endif /* VENDOR_EDIT */
/**********************************************************
* i2c write and read
**********************************************************/
static int aw87359_rcv_i2c_write(struct aw87359_rcv *aw87359_rcv,
	unsigned char reg_addr, unsigned char reg_data)
{
	int ret = -1;
	unsigned char cnt = 0;

	while (cnt < AW_I2C_RETRIES) {
		ret = i2c_smbus_write_byte_data(aw87359_rcv->i2c_client,
			reg_addr, reg_data);
		if (ret < 0) {
			pr_err("%s: i2c_write cnt=%d error=%d\n",
				__func__, cnt, ret);
		} else {
			break;
		}
		cnt++;
		mdelay(AW_I2C_RETRY_DELAY);
	}

	return ret;
}

static int aw87359_rcv_i2c_read(struct aw87359_rcv *aw87359_rcv,
		 unsigned char reg_addr, unsigned char *reg_data)
{
	 int ret = -1;
	 unsigned char cnt = 0;

	while (cnt < AW_I2C_RETRIES) {
		ret = i2c_smbus_read_byte_data(aw87359_rcv->i2c_client,
						reg_addr);
		if (ret < 0) {
			pr_err("%s: i2c_read cnt=%d error=%d\n",
				__func__, cnt, ret);
		} else {
			*reg_data = ret;
			break;
		}
		cnt++;
		mdelay(AW_I2C_RETRY_DELAY);
	}

	return ret;
}

/************************************************************************
* aw87359_rcv hardware control
************************************************************************/
unsigned int aw87359_rcv_hw_on(struct aw87359_rcv *aw87359_rcv)
{
#ifdef USE_PINCTRL_FOR_RESET
	int ret;
#endif
	pr_info("%s enter\n", __func__);

#ifdef USE_PINCTRL_FOR_RESET
		ret = pinctrl_select_state(pinctrl,pins_reset_off);
		if (ret) {
			dev_err(&aw87359_rcv->i2c_client->dev,
				"%s: pinctrl_select_state failed\n", __func__);
		}
		mdelay(2);
		ret = pinctrl_select_state(pinctrl,pins_reset_on);
		if (ret) {
			dev_err(&aw87359_rcv->i2c_client->dev,
				"%s: pinctrl_select_state failed\n", __func__);
		}
		mdelay(2);
		aw87359_rcv->hwen_flag = 1;
#else
	
		if (aw87359_rcv && gpio_is_valid(aw87359_rcv->reset_gpio)) {
			gpio_set_value_cansleep(aw87359_rcv->reset_gpio, 0);
			mdelay(2);
			gpio_set_value_cansleep(aw87359_rcv->reset_gpio, 1);
			mdelay(2);
			aw87359_rcv->hwen_flag = 1;
		} else {
			dev_err(&aw87359_rcv->i2c_client->dev, "%s: failed\n", __func__);
		}
#endif
	return 0;
}

unsigned int aw87359_rcv_hw_off(struct aw87359_rcv *aw87359_rcv)
{
#ifdef USE_PINCTRL_FOR_RESET
		int ret;
#endif
		pr_info("%s enter\n", __func__);
#ifdef USE_PINCTRL_FOR_RESET
		ret = pinctrl_select_state(pinctrl,pins_reset_off);
		if (ret) {
			dev_err(&aw87359_rcv->i2c_client->dev,
				"%s: pinctrl_select_state failed\n", __func__);
		}
		mdelay(2);
		aw87359_rcv->hwen_flag = 0;
#else
		if (aw87359_rcv && gpio_is_valid(aw87359_rcv->reset_gpio)) {
			gpio_set_value_cansleep(aw87359_rcv->reset_gpio, 0);
			mdelay(2);
			aw87359_rcv->hwen_flag = 0;
		} else {
			dev_err(&aw87359_rcv->i2c_client->dev, "%s: failed\n", __func__);
		}
#endif
		return 0;

}


/*******************************************************************************
* aw87359_rcv control interface
******************************************************************************/
unsigned char aw87359_rcv_audio_dspk(void)
{
	unsigned int i;
	unsigned int length;

//#ifdef VENDOR_EDIT
// Ming.Liu@PSW.MM.AudioDriver.feature.1446118, 2020/07/26, Add for PA_BOOST_VOLTAGE
//#ifdef OPPO_AUDIO_PA_BOOST_VOLTAGE
	unsigned char value = 0;
//#endif /* OPPO_AUDIO_PA_BOOST_VOLTAGE */
//#endif /* VENDOR_EDIT */

	pr_info("%s enter\n", __func__);

	if (aw87359_rcv == NULL)
		return 2;

	if (!aw87359_rcv->hwen_flag)
		aw87359_rcv_hw_on(aw87359_rcv);

	length = sizeof(aw87359_rcv_dspk_cfg_default)/sizeof(char);
	if (aw87359_rcv->dspk_cfg_update_flag == 0) { /*update default data*/
		for (i = 0; i < length; i = i+2) {
			aw87359_rcv_i2c_write(aw87359_rcv,
			aw87359_rcv_dspk_cfg_default[i],
			aw87359_rcv_dspk_cfg_default[i+1]);
		}
	}

	if (aw87359_rcv->dspk_cfg_update_flag == 1) {  /*update firmware data*/
		for (i = 0; i < aw87359_rcv_dspk_cnt->len; i = i+2) {
			aw87359_rcv_i2c_write(aw87359_rcv,
					aw87359_rcv_dspk_cnt->data[i],
					aw87359_rcv_dspk_cnt->data[i+1]);

		}
	}
//#ifdef VENDOR_EDIT
// Ming.Liu@PSW.MM.AudioDriver.feature.1446118, 2020/07/26, Add for PA_BOOST_VOLTAGE
//#ifdef OPPO_AUDIO_PA_BOOST_VOLTAGE
	if (curr_rcv_voltage != 0) {
		switch (curr_rcv_voltage) {
		// 7V
		case 70:
			value = 0x04;
			aw87359_rcv_i2c_write(aw87359_rcv, aw87359_rcv_REG_CPOVP, value);
			break;
		// 7.5V
		case 75:
			value = 0x06;
			aw87359_rcv_i2c_write(aw87359_rcv, aw87359_rcv_REG_CPOVP, value);
			break;
		// 8V
		default:
			break;
		}

	}
//#endif /* OPPO_AUDIO_PA_BOOST_VOLTAGE */
//#endif /* VENDOR_EDIT */

	return 0;
}

unsigned char aw87359_rcv_audio_drcv(void)
{
	unsigned int i;
	unsigned int length;

	pr_info("%s enter\n", __func__);

	if (aw87359_rcv == NULL)
		return 2;
	if (!aw87359_rcv->hwen_flag)
		aw87359_rcv_hw_on(aw87359_rcv);

	length = sizeof(aw87359_rcv_drcv_cfg_default)/sizeof(char);
	if (aw87359_rcv->drcv_cfg_update_flag == 0) { /*send default data*/
		for (i = 0; i < length; i = i+2) {
			aw87359_rcv_i2c_write(aw87359_rcv,
				aw87359_rcv_drcv_cfg_default[i],
				aw87359_rcv_drcv_cfg_default[i+1]);
		}
	}

	if (aw87359_rcv->drcv_cfg_update_flag == 1) {  /*send firmware data*/
		for (i = 0; i < aw87359_rcv_drcv_cnt->len; i = i+2) {
			aw87359_rcv_i2c_write(aw87359_rcv,
					aw87359_rcv_drcv_cnt->data[i],
					aw87359_rcv_drcv_cnt->data[i+1]);

		}
	}

	return 0;
}

unsigned char aw87359_rcv_audio_abspk(void)
{
	unsigned int i;
	unsigned int length;

	pr_info("%s enter\n", __func__);

	if (aw87359_rcv == NULL)
		return 2;

	if (!aw87359_rcv->hwen_flag)
		aw87359_rcv_hw_on(aw87359_rcv);


	length = sizeof(aw87359_rcv_abspk_cfg_default)/sizeof(char);
	if (aw87359_rcv->abspk_cfg_update_flag == 0) { /*send default data*/
		for (i = 0; i < length; i = i+2) {
			aw87359_rcv_i2c_write(aw87359_rcv,
				aw87359_rcv_abspk_cfg_default[i],
				aw87359_rcv_abspk_cfg_default[i+1]);
		}
	}

	if (aw87359_rcv->abspk_cfg_update_flag == 1) {  /*send firmware data*/
		for (i = 0; i < aw87359_rcv_abspk_cnt->len; i = i+2) {
			aw87359_rcv_i2c_write(aw87359_rcv,
					aw87359_rcv_abspk_cnt->data[i],
					aw87359_rcv_abspk_cnt->data[i+1]);

		}
	}

	return 0;
}

unsigned char aw87359_rcv_audio_abrcv(void)
{
	unsigned int i;
	unsigned int length;

	pr_info("%s enter\n", __func__);

	if (aw87359_rcv == NULL)
		return 2;

	if (!aw87359_rcv->hwen_flag)
		aw87359_rcv_hw_on(aw87359_rcv);


	length = sizeof(aw87359_rcv_abrcv_cfg_default)/sizeof(char);
	if (aw87359_rcv->abrcv_cfg_update_flag == 0) { /*send default data*/
		for (i = 0; i < length; i = i+2) {
			aw87359_rcv_i2c_write(aw87359_rcv,
					aw87359_rcv_abrcv_cfg_default[i],
					aw87359_rcv_abrcv_cfg_default[i+1]);
		}
	}

	if (aw87359_rcv->abrcv_cfg_update_flag == 1) {  /*send firmware data*/
		for (i = 0; i < aw87359_rcv_abrcv_cnt->len; i = i+2) {
			aw87359_rcv_i2c_write(aw87359_rcv,
					aw87359_rcv_abrcv_cnt->data[i],
					aw87359_rcv_abrcv_cnt->data[i+1]);

		}
	}

	return 0;
}

unsigned char aw87359_rcv_audio_off(void)
{
	if (aw87359_rcv == NULL)
		return 2;

	if (aw87359_rcv->hwen_flag)
		aw87359_rcv_i2c_write(aw87359_rcv, 0x01, 0x00);   /*CHIP Disable*/

	aw87359_rcv_hw_off(aw87359_rcv);
	return 0;
}

//#ifdef VENDOR_EDIT
// Ming.Liu@PSW.MM.AudioDriver.feature.1446118, 2020/07/26, Add for PA_BOOST_VOLTAGE
//#ifdef OPPO_AUDIO_PA_BOOST_VOLTAGE
int aw87359_set_rcv_voltage(int level)
{
	unsigned char value = 0;
	int i = 0;

	if (aw87359_rcv == NULL) {
		pr_err("%s, %d, aw87359_rcv == NULL", __func__, __LINE__);

		return -1;
	}

	if (!aw87359_rcv->hwen_flag) {
		curr_rcv_voltage = level;
		pr_debug("%s, %d, aw87359_rcv is off! curr_rcv_voltage = %d", __func__, __LINE__, curr_rcv_voltage);

		return -1;
	}

	switch (level) {
	// 7V
	case 70:
		value = 0x02;
		break;
	// 7.5V
	case 75:
		value = 0x04;
		break;
	// Default
	default:
		if (aw87359_rcv->dspk_cfg_update_flag == 0) {
			for (i = 0; i < sizeof(aw87359_rcv_dspk_cfg_default) / sizeof(char); i = i + 2) {
				if (aw87359_rcv_REG_CPOVP == aw87359_rcv_dspk_cfg_default[i]) {
					value = aw87359_rcv_dspk_cfg_default[i + 1];
					break;
				}
			}
		}

		if (aw87359_rcv->dspk_cfg_update_flag == 1) {
			for (i = 0; i < aw87359_rcv_dspk_cnt->len; i = i + 2) {
				if (aw87359_rcv_REG_CPOVP == aw87359_rcv_dspk_cnt->data[i]) {
					value = aw87359_rcv_dspk_cnt->data[i + 1];
					break;
				}
			}
		}
	}

	if (aw87359_rcv->hwen_flag) {
		aw87359_rcv_i2c_write(aw87359_rcv, aw87359_rcv_REG_CPOVP, value);
	}

	curr_rcv_voltage = level;

    pr_debug("%s, %d, level = %d, value = %#x", __func__, __LINE__, level, value);	

	return 0;
}
//#endif /* OPPO_AUDIO_PA_BOOST_VOLTAGE */
//#endif /* VENDOR_EDIT */
/****************************************************************************
* aw87359 firmware cfg update
***************************************************************************/
static void aw87359_rcv_abrcv_cfg_loaded(const struct firmware *cont,
			void *context)
{
	int i = 0;
	int ram_timer_val = 2000;

	pr_info("%s enter\n", __func__);

	abrcv_load_cont++;
	if (!cont) {
		pr_err("%s: failed to read %s\n", __func__,
		aw87359_rcv_abrcv_name);
		release_firmware(cont);
		if (abrcv_load_cont <= 2) {
			schedule_delayed_work(&aw87359_rcv->ram_work,
					msecs_to_jiffies(ram_timer_val));
			pr_info("%s:restart hrtimer to load firmware\n",
			__func__);
		}
		return;
	}

	pr_info("%s: loaded %s - size: %zu\n", __func__, aw87359_rcv_abrcv_name,
					cont ? cont->size : 0);

	for (i = 0; i < cont->size; i = i+2) {
		pr_info("%s: addr:0x%04x, data:0x%02x\n",
		__func__, *(cont->data+i), *(cont->data+i+1));
	}

	/* aw87359 ram update */
	aw87359_rcv_abrcv_cnt = kzalloc(cont->size+sizeof(int), GFP_KERNEL);
	if (!aw87359_rcv_abrcv_cnt) {
		release_firmware(cont);
		pr_err("%s: Error allocating memory\n", __func__);
		return;
	}
	aw87359_rcv_abrcv_cnt->len = cont->size;
	memcpy(aw87359_rcv_abrcv_cnt->data, cont->data, cont->size);
	release_firmware(cont);
	aw87359_rcv->abrcv_cfg_update_flag = 1;

	pr_info("%s: fw update complete\n", __func__);
}

static int aw87359_rcv_abrcv_update(struct aw87359_rcv *aw87359_rcv)
{
	pr_info("%s enter\n", __func__);
	return request_firmware_nowait(THIS_MODULE,
					FW_ACTION_HOTPLUG,
					aw87359_rcv_abrcv_name,
					&aw87359_rcv->i2c_client->dev,
					GFP_KERNEL,
					aw87359_rcv,
					aw87359_rcv_abrcv_cfg_loaded);
}

static void aw87359_rcv_abspk_cfg_loaded(const struct firmware *cont,
			void *context)
{
	int i = 0;
	int ram_timer_val = 2000;

	pr_info("%s enter\n", __func__);

	abspk_load_cont++;
	if (!cont) {
		pr_err("%s: failed to read %s\n", __func__,
				aw87359_rcv_abspk_name);
		release_firmware(cont);
		if (abspk_load_cont <= 2) {
			schedule_delayed_work(&aw87359_rcv->ram_work,
					msecs_to_jiffies(ram_timer_val));
			pr_info("%s:restart hrtimer to load firmware\n",
			__func__);
		}
		return;
	}

	pr_info("%s: loaded %s - size: %zu\n", __func__, aw87359_rcv_abspk_name,
					cont ? cont->size : 0);

	for (i = 0; i < cont->size; i = i+2) {
		pr_info("%s: addr:0x%04x, data:0x%02x\n",
		__func__, *(cont->data+i), *(cont->data+i+1));
	}

	/* aw87359 ram update */
	aw87359_rcv_abspk_cnt = kzalloc(cont->size+sizeof(int), GFP_KERNEL);
	if (!aw87359_rcv_abspk_cnt) {
		release_firmware(cont);
		pr_err("%s: Error allocating memory\n", __func__);
		return;
	}
	aw87359_rcv_abspk_cnt->len = cont->size;
	memcpy(aw87359_rcv_abspk_cnt->data, cont->data, cont->size);
	release_firmware(cont);
	aw87359_rcv->abspk_cfg_update_flag = 1;

	pr_info("%s: fw update complete\n", __func__);
}

static int aw87359_rcv_abspk_update(struct aw87359_rcv *aw87359_rcv)
{
	pr_info("%s enter\n", __func__);
	return request_firmware_nowait(THIS_MODULE,
					FW_ACTION_HOTPLUG,
					aw87359_rcv_abspk_name,
					&aw87359_rcv->i2c_client->dev,
					GFP_KERNEL,
					aw87359_rcv,
					aw87359_rcv_abspk_cfg_loaded);
}

static void aw87359_rcv_drcv_cfg_loaded(const struct firmware *cont, void *context)
{
	int i = 0;
	int ram_timer_val = 2000;

	pr_info("%s enter\n", __func__);

	drcv_load_cont++;
	if (!cont) {
		pr_err("%s: failed to read %s\n", __func__, aw87359_rcv_drcv_name);
		release_firmware(cont);
		if (drcv_load_cont <= 2) {
			schedule_delayed_work(&aw87359_rcv->ram_work,
					msecs_to_jiffies(ram_timer_val));
			pr_info("%s:restart hrtimer to load firmware\n",
				__func__);
		}
		return;
	}

	pr_info("%s: loaded %s - size: %zu\n", __func__, aw87359_rcv_drcv_name,
					cont ? cont->size : 0);

	for (i = 0; i < cont->size; i = i+2) {
		pr_info("%s: addr:0x%04x, data:0x%02x\n",
		__func__, *(cont->data+i), *(cont->data+i+1));
	}

	if (aw87359_rcv_drcv_cnt != NULL)
		aw87359_rcv_drcv_cnt = NULL;

	/* aw87359 ram update */
	aw87359_rcv_drcv_cnt = kzalloc(cont->size+sizeof(int), GFP_KERNEL);
	if (!aw87359_rcv_drcv_cnt) {
		release_firmware(cont);
		pr_err("%s: Error allocating memory\n", __func__);
		return;
	}
	aw87359_rcv_drcv_cnt->len = cont->size;
	memcpy(aw87359_rcv_drcv_cnt->data, cont->data, cont->size);
	release_firmware(cont);
	aw87359_rcv->drcv_cfg_update_flag = 1;

	pr_info("%s: fw update complete\n", __func__);
}

static int aw87359_rcv_drcv_update(struct aw87359_rcv *aw87359_rcv)
{
	pr_info("%s enter\n", __func__);
	return request_firmware_nowait(THIS_MODULE,
					FW_ACTION_HOTPLUG,
					aw87359_rcv_drcv_name,
					&aw87359_rcv->i2c_client->dev,
					GFP_KERNEL,
					aw87359_rcv,
					aw87359_rcv_drcv_cfg_loaded);
}

static void aw87359_rcv_dspk_cfg_loaded(const struct firmware *cont, void *context)
{
	int i = 0;
	int ram_timer_val = 2000;

	pr_info("%s enter\n", __func__);
	dspk_load_cont++;
	if (!cont) {
		pr_err("%s: failed to read %s\n", __func__, aw87359_rcv_dspk_name);
		release_firmware(cont);
		if (dspk_load_cont <= 2) {
			schedule_delayed_work(&aw87359_rcv->ram_work,
					msecs_to_jiffies(ram_timer_val));
			pr_info("%s:restart hrtimer to load firmware\n",
			__func__);
		}
	return;
	}

	pr_info("%s: loaded %s - size: %zu\n", __func__, aw87359_rcv_dspk_name,
				cont ? cont->size : 0);

	for (i = 0; i < cont->size; i = i+2) {
		pr_info("%s: addr:0x%02x, data:0x%02x\n",
				__func__, *(cont->data+i), *(cont->data+i+1));
	}

	if (aw87359_rcv_dspk_cnt != NULL)
		aw87359_rcv_dspk_cnt = NULL;

	/* aw87359 ram update */
	aw87359_rcv_dspk_cnt = kzalloc(cont->size+sizeof(int), GFP_KERNEL);
	if (!aw87359_rcv_dspk_cnt) {
		release_firmware(cont);
		pr_err("%s: Error allocating memory\n", __func__);
		return;
	}
	aw87359_rcv_dspk_cnt->len = cont->size;
	memcpy(aw87359_rcv_dspk_cnt->data, cont->data, cont->size);
	release_firmware(cont);
	aw87359_rcv->dspk_cfg_update_flag = 1;

	pr_info("%s: fw update complete\n", __func__);
}



#ifdef AWINIC_CFG_UPDATE_DELAY
static int aw87359_rcv_dspk_update(struct aw87359_rcv *aw87359_rcv)
{
	pr_info("%s enter\n", __func__);

	return request_firmware_nowait(THIS_MODULE,
					FW_ACTION_HOTPLUG,
					aw87359_rcv_dspk_name,
					&aw87359_rcv->i2c_client->dev,
					GFP_KERNEL,
					aw87359_rcv,
					aw87359_rcv_dspk_cfg_loaded);
}

static void aw87359_rcv_cfg_work_routine(struct work_struct *work)
{
	pr_info("%s enter\n", __func__);
	if (aw87359_rcv->dspk_cfg_update_flag == 0)
		aw87359_rcv_dspk_update(aw87359_rcv);
	if (aw87359_rcv->drcv_cfg_update_flag == 0)
		aw87359_rcv_drcv_update(aw87359_rcv);
	if (aw87359_rcv->abspk_cfg_update_flag == 0)
		aw87359_rcv_abspk_update(aw87359_rcv);
	if (aw87359_rcv->abrcv_cfg_update_flag == 0)
		aw87359_rcv_abrcv_update(aw87359_rcv);
}
#endif

static int aw87359_rcv_cfg_init(struct aw87359_rcv *aw87359_rcv)
{
	int ret = -1;
#ifdef AWINIC_CFG_UPDATE_DELAY
	int cfg_timer_val = 5000;

	INIT_DELAYED_WORK(&aw87359_rcv->ram_work, aw87359_rcv_cfg_work_routine);
	schedule_delayed_work(&aw87359_rcv->ram_work,
		msecs_to_jiffies(cfg_timer_val));
	ret = 0;
#else
	ret = request_firmware_nowait(THIS_MODULE,
						  FW_ACTION_HOTPLUG,
						  aw87359_rcv_dspk_name,
						  &aw87359_rcv->i2c_client->dev,
						  GFP_KERNEL,
						  aw87359_rcv, aw87359_rcv_dspk_cfg_loaded);
		if (ret) {
			pr_err("%s: request_firmware_nowait failed with read %s.\n",
				   __func__, aw87359_rcv_dspk_name);
		}
#endif
	return ret;
}

/****************************************************************************
* aw87359 attribute
*****************************************************************************/
static ssize_t aw87359_rcv_get_reg(struct device *dev,
		   struct device_attribute *attr, char *buf)
{
	ssize_t len = 0;
	unsigned int i = 0;
	unsigned char reg_val = 0;

	for (i = 0; i < AW87359_rcv_REG_MAX; i++) {
		if (!(aw87359_rcv_reg_access[i]&REG_RD_ACCESS))
		aw87359_rcv_i2c_read(aw87359_rcv, i, &reg_val);
		len += snprintf(buf+len, PAGE_SIZE-len,
				"reg:0x%02x=0x%02x\n", i, reg_val);
	}

	return len;
}

static ssize_t aw87359_rcv_set_reg(struct device *dev,
		 struct device_attribute *attr, const char *buf, size_t len)
{
	unsigned int databuf[2] = {0, 0};

	if (sscanf(buf, "%x %x", &databuf[0], &databuf[1]) == 2)
		aw87359_rcv_i2c_write(aw87359_rcv, databuf[0], databuf[1]);

	return len;
}


static ssize_t aw87359_rcv_get_hwen(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	ssize_t len = 0;

	len += snprintf(buf+len, PAGE_SIZE-len, "hwen: %d\n",
			aw87359_rcv->hwen_flag);

	return len;
}

static ssize_t aw87359_rcv_set_hwen(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t len)
{
	ssize_t ret;
	unsigned int state;

	ret = kstrtouint(buf, 10, &state);
	if (ret)
		goto out_strtoint;
	if (state == 0) {		/*OFF*/
		aw87359_rcv_hw_off(aw87359_rcv);
	} else {			/*ON*/
		aw87359_rcv_hw_on(aw87359_rcv);
	}

	return len;

out_strtoint:
	dev_err(&aw87359_rcv->i2c_client->dev, "%s: fail to change str to int\n",
		__func__);
	return ret;
}

static ssize_t aw87359_rcv_get_update(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	ssize_t len = 0;

	return len;
}

static ssize_t aw87359_rcv_set_update(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t len)
{
	ssize_t ret;
	unsigned int state;
	int cfg_timer_val = 10;

	ret = kstrtouint(buf, 10, &state);
	if (ret)
		goto out_strtoint;
	if (state == 0) {
	} else {
		aw87359_rcv->dspk_cfg_update_flag = 1;
		aw87359_rcv->drcv_cfg_update_flag = 1;
		aw87359_rcv->abspk_cfg_update_flag = 1;
		aw87359_rcv->abrcv_cfg_update_flag = 1;
		schedule_delayed_work(&aw87359_rcv->ram_work,
				msecs_to_jiffies(cfg_timer_val));
	}

	return len;

out_strtoint:
	dev_err(&aw87359_rcv->i2c_client->dev, "%s: fail to change str to int\n",
		__func__);
	return ret;
}

static ssize_t aw87359_rcv_get_mode(struct device *cd,
		struct device_attribute *attr, char *buf)
{
	ssize_t len = 0;

	len += snprintf(buf+len, PAGE_SIZE-len, "0: off mode\n");
	len += snprintf(buf+len, PAGE_SIZE-len, "1: dspk mode\n");
	len += snprintf(buf+len, PAGE_SIZE-len, "2: drcv mode\n");
	len += snprintf(buf+len, PAGE_SIZE-len, "3: abspk mode\n");
	len += snprintf(buf+len, PAGE_SIZE-len, "4: abrcv mode\n");

	return len;
}

static ssize_t aw87359_rcv_set_mode(struct device *cd,
		struct device_attribute *attr, const char *buf, size_t len)
{
	ssize_t ret;
	unsigned int state;

	ret = kstrtouint(buf, 10, &state);
	if (ret)
		goto out_strtoint;
	if (state == 0)
		aw87359_rcv_audio_off();
	else if (state == 1)
		aw87359_rcv_audio_dspk();
	else if (state == 2)
		aw87359_rcv_audio_drcv();
	else if (state == 3)
		aw87359_rcv_audio_abspk();
	else if (state == 4)
		aw87359_rcv_audio_abrcv();
	else
		aw87359_rcv_audio_off();

	return len;

out_strtoint:
	dev_err(&aw87359_rcv->i2c_client->dev, "%s: fail to change str to int\n",
		__func__);
	return ret;
}

static DEVICE_ATTR(reg, 0660, aw87359_rcv_get_reg, aw87359_rcv_set_reg);
static DEVICE_ATTR(hwen, 0660, aw87359_rcv_get_hwen, aw87359_rcv_set_hwen);
static DEVICE_ATTR(update, 0660, aw87359_rcv_get_update, aw87359_rcv_set_update);
static DEVICE_ATTR(mode, 0660, aw87359_rcv_get_mode, aw87359_rcv_set_mode);

static struct attribute *aw87359_rcv_attributes[] = {
	&dev_attr_reg.attr,
	&dev_attr_hwen.attr,
	&dev_attr_update.attr,
	&dev_attr_mode.attr,
	NULL
};

static struct attribute_group aw87359_rcv_attribute_group = {
	.attrs = aw87359_rcv_attributes
};
#ifndef USE_PINCTRL_FOR_RESET
/*****************************************************
* device tree
*****************************************************/
static int aw87359_rcv_parse_dt(struct device *dev, struct device_node *np)
{
	aw87359_rcv->reset_gpio = of_get_named_gpio(np, "reset-gpio", 0);
	if (aw87359_rcv->reset_gpio < 0) {
		dev_err(dev, "%s: no reset gpio provided .\n", __func__);
		return -1;
	}
	dev_info(dev, "%s: reset gpio provided ok\n", __func__);
	
	return 0;
}
	
int aw87359_rcv_hw_reset(struct aw87359_rcv *aw87359_rcv)
{
	pr_info("%s enter\n", __func__);
	
	if (aw87359_rcv && gpio_is_valid(aw87359_rcv->reset_gpio)) {
		gpio_set_value_cansleep(aw87359_rcv->reset_gpio, 0);
		mdelay(2);
		gpio_set_value_cansleep(aw87359_rcv->reset_gpio, 1);
		mdelay(2);
		aw87359_rcv->hwen_flag = 1;
	} else {
		aw87359_rcv->hwen_flag = 0;
		dev_err(&aw87359_rcv->i2c_client->dev, "%s:  failed\n", __func__);
	}
	return 0;
}
#endif
/*****************************************************
* check chip id
*****************************************************/
int aw87359_rcv_read_chipid(struct aw87359_rcv *aw87359_rcv)
{
	unsigned int cnt = 0;
	int ret = -1;
	unsigned char reg_val = 0;

	while (cnt < AW_READ_CHIPID_RETRIES) {
		ret = aw87359_rcv_i2c_read(aw87359_rcv, aw87359_rcv_REG_CHIPID, &reg_val);
		if (reg_val == AW87359_RCV_CHIPID) {
			pr_info("%s: This Chip is aw87359_RCV chipid=0x%x\n",
					__func__, reg_val);
			return 0;
		}
		cnt++;

		mdelay(AW_READ_CHIPID_RETRY_DELAY);
	}
	pr_info("%s: aw87359_rcv chipid=0x%x error\n", __func__, reg_val);
	return -EINVAL;
}



/****************************************************************************
* aw87359_rcv i2c driver
*****************************************************************************/
static int
aw87359_rcv_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
#ifndef USE_PINCTRL_FOR_RESET
	struct device_node *np = client->dev.of_node;
#endif

	int ret = -1;

	pr_info("%s Enter\n", __func__);

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		dev_err(&client->dev, "%s: check_functionality failed\n",
			__func__);
		ret = -ENODEV;
		goto exit_check_functionality_failed;
	}

	aw87359_rcv = devm_kzalloc(&client->dev,
				sizeof(struct aw87359_rcv),
				GFP_KERNEL);
	if (aw87359_rcv == NULL) {
		ret = -ENOMEM;
		goto exit_devm_kzalloc_failed;
	}

	aw87359_rcv->i2c_client = client;
	i2c_set_clientdata(client, aw87359_rcv);
	
	/* aw87359_rcv rst */
#ifdef USE_PINCTRL_FOR_RESET
	pinctrl = devm_pinctrl_get(&client->dev);
	if (IS_ERR_OR_NULL(pinctrl)) {
			dev_err(&client->dev,
			"%s: get pinctrl failed\n", __func__);
		}

	pins_reset_on = pinctrl_lookup_state(pinctrl, "rcv_reset_on");
	if (IS_ERR_OR_NULL(pins_reset_on)) {
		dev_err(&client->dev, "%s: loockup failed\n", __func__);
	}

	pins_reset_off = pinctrl_lookup_state(pinctrl, "rcv_reset_off");
	if (IS_ERR_OR_NULL(pins_reset_off)) {
			dev_err(&client->dev,
				"%s: loockup failed\n", __func__);
	}

	ret = pinctrl_select_state(pinctrl,pins_reset_off);
	if (ret) {
		dev_err(&client->dev,
			"%s: pinctrl_select_state failed\n", __func__);
	}
	mdelay(2);
	ret = pinctrl_select_state(pinctrl,pins_reset_on);
	if (ret) {
		dev_err(&client->dev,
			"%s: pinctrl_select_state failed\n", __func__);
	}
	mdelay(5);

	aw87359_rcv->hwen_flag = 1;
#else
	if (np) {
		ret = aw87359_rcv_parse_dt(&client->dev, np);
		if (ret) {
			dev_err(&client->dev,
				"%s: failed to parse device tree node\n",
				__func__);
			goto exit_gpio_get_failed;
		}
	} else {
		aw87359_rcv->reset_gpio = -1;
	}

	if (gpio_is_valid(aw87359_rcv->reset_gpio)) {
		ret =
		    devm_gpio_request_one(&client->dev,
					  aw87359_rcv->reset_gpio,
					  GPIOF_OUT_INIT_LOW, "aw87359_rcv_rst");
		if (ret) {
			dev_err(&client->dev,
				"%s: rst request failed\n", __func__);
			goto exit_gpio_request_failed;
		}
	}

	/* hardware reset */
	aw87359_rcv_hw_reset(aw87359_rcv);
#endif
	/*real i2c address*/
	if(aw87359_rcv->i2c_client->addr == 0x78)
		{
			aw87359_rcv->i2c_client->addr = 0x58;
		}
	else if(aw87359_rcv->i2c_client->addr == 0x79)
		{
			aw87359_rcv->i2c_client->addr = 0x59;
		}
	/* aw87359_rcv chip id */
	ret = aw87359_rcv_read_chipid(aw87359_rcv);
	if (ret < 0) {
		dev_err(&client->dev, "%s: aw87359_rcv_read_chipid failed ret=%d\n",
			__func__, ret);
		goto exit_i2c_check_id_failed;
	}
	awinic_rcv_audio_rcv_pf = &aw87359_rcv_audio_drcv;
	awinic_rcv_audio_spk_pf = &aw87359_rcv_audio_dspk;
	awinic_rcv_audio_spk_off_pf = &aw87359_rcv_audio_off;
	awinic_set_rcv_voltage_pf = &aw87359_set_rcv_voltage;

	ret = sysfs_create_group(&client->dev.kobj, &aw87359_rcv_attribute_group);
	if (ret < 0) {
		dev_info(&client->dev, "%s error creating sysfs attr files\n",
			__func__);
	}

	/* aw87359_rcv cfg update */
	dspk_load_cont = 0;
	drcv_load_cont = 0;
	abspk_load_cont = 0;
	abrcv_load_cont = 0;
	aw87359_rcv->dspk_cfg_update_flag = 0;
	aw87359_rcv->drcv_cfg_update_flag = 0;
	aw87359_rcv->abspk_cfg_update_flag = 0;
	aw87359_rcv->abrcv_cfg_update_flag = 0;
	aw87359_rcv_cfg_init(aw87359_rcv);

	/* aw87359 hardware off */
	aw87359_rcv_hw_off(aw87359_rcv);

	return 0;

exit_i2c_check_id_failed:
 #ifndef USE_PINCTRL_FOR_RESET
	devm_gpio_free(&client->dev, aw87359_rcv->reset_gpio);
exit_gpio_request_failed:
exit_gpio_get_failed:
	devm_kfree(&client->dev, aw87359_rcv);
#endif
	aw87359_rcv = NULL;
exit_devm_kzalloc_failed:
exit_check_functionality_failed:
	return ret;
}

static int aw87359_rcv_i2c_remove(struct i2c_client *client)
{
	struct aw87359_rcv *aw87359_rcv = i2c_get_clientdata(client);

	if (gpio_is_valid(aw87359_rcv->reset_gpio))
		devm_gpio_free(&client->dev, aw87359_rcv->reset_gpio);

	return 0;
}

static const struct i2c_device_id aw87359_rcv_i2c_id[] = {
	{ AW87359_RCV_I2C_NAME, 0 },
	{ }
};


static const struct of_device_id extpa_of_match[] = {
	{.compatible = "awinic,aw87359_rcv_pa"},
	{},
};


static struct i2c_driver aw87359_rcv_i2c_driver = {
	.driver = {
		.owner = THIS_MODULE,
		.name = AW87359_RCV_I2C_NAME,
		.of_match_table = extpa_of_match,
	},
	.probe = aw87359_rcv_i2c_probe,
	.remove = aw87359_rcv_i2c_remove,
	.id_table	= aw87359_rcv_i2c_id,
};

static int __init aw87359_rcv_pa_init(void)
{
	int ret;

	pr_info("%s enter\n", __func__);
	pr_info("%s: driver version: %s\n", __func__, AW87359_RCV_DRIVER_VERSION);

	ret = i2c_add_driver(&aw87359_rcv_i2c_driver);
	if (ret) {
		pr_info("****[%s] Unable to register driver (%d)\n",
				__func__, ret);
		return ret;
	}
	return 0;
}

static void __exit aw87359_rcv_pa_exit(void)
{
	pr_info("%s enter\n", __func__);
	i2c_del_driver(&aw87359_rcv_i2c_driver);
}


module_init(aw87359_rcv_pa_init);
module_exit(aw87359_rcv_pa_exit);

MODULE_AUTHOR("<zhangzetao@awinic.com.cn>");
MODULE_DESCRIPTION("awinic aw87359_rcv pa driver");
MODULE_LICENSE("GPL v2");
