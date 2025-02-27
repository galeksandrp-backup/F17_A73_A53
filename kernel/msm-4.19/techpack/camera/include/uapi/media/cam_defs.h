/* SPDX-License-Identifier: GPL-2.0-only WITH Linux-syscall-note */
/*
 * Copyright (c) 2016-2019, The Linux Foundation. All rights reserved.
 */

#ifndef __UAPI_CAM_DEFS_H__
#define __UAPI_CAM_DEFS_H__

#include <linux/videodev2.h>
#include <linux/types.h>
#include <linux/ioctl.h>


/* camera op codes */
#define CAM_COMMON_OPCODE_BASE                  0x100
#define CAM_QUERY_CAP                           (CAM_COMMON_OPCODE_BASE + 0x1)
#define CAM_ACQUIRE_DEV                         (CAM_COMMON_OPCODE_BASE + 0x2)
#define CAM_START_DEV                           (CAM_COMMON_OPCODE_BASE + 0x3)
#define CAM_STOP_DEV                            (CAM_COMMON_OPCODE_BASE + 0x4)
#define CAM_CONFIG_DEV                          (CAM_COMMON_OPCODE_BASE + 0x5)
#define CAM_RELEASE_DEV                         (CAM_COMMON_OPCODE_BASE + 0x6)
#define CAM_SD_SHUTDOWN                         (CAM_COMMON_OPCODE_BASE + 0x7)
#define CAM_FLUSH_REQ                           (CAM_COMMON_OPCODE_BASE + 0x8)
#define CAM_COMMON_OPCODE_MAX                   (CAM_COMMON_OPCODE_BASE + 0x9)

#define CAM_COMMON_OPCODE_BASE_v2           0x150
#define CAM_ACQUIRE_HW                      (CAM_COMMON_OPCODE_BASE_v2 + 0x1)
#define CAM_RELEASE_HW                      (CAM_COMMON_OPCODE_BASE_v2 + 0x2)
#define CAM_DUMP_REQ                        (CAM_COMMON_OPCODE_BASE_v2 + 0x3)

#define CAM_EXT_OPCODE_BASE                     0x200
#define CAM_CONFIG_DEV_EXTERNAL                 (CAM_EXT_OPCODE_BASE + 0x1)

/* camera handle type */
#define CAM_HANDLE_USER_POINTER                 1
#define CAM_HANDLE_MEM_HANDLE                   2

/* Generic Blob CmdBuffer header properties */
#define CAM_GENERIC_BLOB_CMDBUFFER_SIZE_MASK    0xFFFFFF00
#define CAM_GENERIC_BLOB_CMDBUFFER_SIZE_SHIFT   8
#define CAM_GENERIC_BLOB_CMDBUFFER_TYPE_MASK    0xFF
#define CAM_GENERIC_BLOB_CMDBUFFER_TYPE_SHIFT   0

/* Command Buffer Types */
#define CAM_CMD_BUF_DMI                     0x1
#define CAM_CMD_BUF_DMI16                   0x2
#define CAM_CMD_BUF_DMI32                   0x3
#define CAM_CMD_BUF_DMI64                   0x4
#define CAM_CMD_BUF_DIRECT                  0x5
#define CAM_CMD_BUF_INDIRECT                0x6
#define CAM_CMD_BUF_I2C                     0x7
#define CAM_CMD_BUF_FW                      0x8
#define CAM_CMD_BUF_GENERIC                 0x9
#define CAM_CMD_BUF_LEGACY                  0xA

/* UBWC API Version */
#define CAM_UBWC_CFG_VERSION_1              1
#define CAM_UBWC_CFG_VERSION_2              2

#define CAM_MAX_ACQ_RES    5
#define CAM_MAX_HW_SPLIT   3

#ifndef VENDOR_EDIT
#define VENDOR_EDIT
#endif

#ifdef VENDOR_EDIT  //add dpc read for imx471
#define CAM_GET_DPC_DATA                    (CAM_COMMON_OPCODE_BASE_v2 + 0x3)
#endif


/**
 * enum flush_type_t - Identifies the various flush types
 *
 * @CAM_FLUSH_TYPE_REQ:    Flush specific request
 * @CAM_FLUSH_TYPE_ALL:    Flush all requests belonging to a context
 * @CAM_FLUSH_TYPE_MAX:    Max enum to validate flush type
 *
 */
enum flush_type_t {
	CAM_FLUSH_TYPE_REQ,
	CAM_FLUSH_TYPE_ALL,
	CAM_FLUSH_TYPE_MAX
};

/**
 * struct cam_control - Structure used by ioctl control for camera
 *
 * @op_code:            This is the op code for camera control
 * @size:               Control command size
 * @handle_type:        User pointer or shared memory handle
 * @reserved:           Reserved field for 64 bit alignment
 * @handle:             Control command payload
 */
struct cam_control {
	uint32_t        op_code;
	uint32_t        size;
	uint32_t        handle_type;
	uint32_t        reserved;
	uint64_t        handle;
};

/* camera IOCTL */
#define VIDIOC_CAM_CONTROL \
	_IOWR('V', BASE_VIDIOC_PRIVATE, struct cam_control)

#ifdef VENDOR_EDIT
	/*add by hongbo.dai@Camera,20180326 for AT test*/
#define VIDIOC_CAM_FTM_POWNER_UP 0
#define VIDIOC_CAM_FTM_POWNER_DOWN 1
#endif
/**
 * struct cam_hw_version - Structure for HW version of camera devices
 *
 * @major    : Hardware version major
 * @minor    : Hardware version minor
 * @incr     : Hardware version increment
 * @reserved : Reserved for 64 bit aligngment
 */
struct cam_hw_version {
	uint32_t major;
	uint32_t minor;
	uint32_t incr;
	uint32_t reserved;
};

/**
 * struct cam_iommu_handle - Structure for IOMMU handles of camera hw devices
 *
 * @non_secure: Device Non Secure IOMMU handle
 * @secure:     Device Secure IOMMU handle
 *
 */
struct cam_iommu_handle {
	int32_t non_secure;
	int32_t secure;
};

/* camera secure mode */
#define CAM_SECURE_MODE_NON_SECURE             0
#define CAM_SECURE_MODE_SECURE                 1

/* Camera Format Type */
#define CAM_FORMAT_BASE                         0
#define CAM_FORMAT_MIPI_RAW_6                   1
#define CAM_FORMAT_MIPI_RAW_8                   2
#define CAM_FORMAT_MIPI_RAW_10                  3
#define CAM_FORMAT_MIPI_RAW_12                  4
#define CAM_FORMAT_MIPI_RAW_14                  5
#define CAM_FORMAT_MIPI_RAW_16                  6
#define CAM_FORMAT_MIPI_RAW_20                  7
#define CAM_FORMAT_QTI_RAW_8                    8
#define CAM_FORMAT_QTI_RAW_10                   9
#define CAM_FORMAT_QTI_RAW_12                   10
#define CAM_FORMAT_QTI_RAW_14                   11
#define CAM_FORMAT_PLAIN8                       12
#define CAM_FORMAT_PLAIN16_8                    13
#define CAM_FORMAT_PLAIN16_10                   14
#define CAM_FORMAT_PLAIN16_12                   15
#define CAM_FORMAT_PLAIN16_14                   16
#define CAM_FORMAT_PLAIN16_16                   17
#define CAM_FORMAT_PLAIN32_20                   18
#define CAM_FORMAT_PLAIN64                      19
#define CAM_FORMAT_PLAIN128                     20
#define CAM_FORMAT_ARGB                         21
#define CAM_FORMAT_ARGB_10                      22
#define CAM_FORMAT_ARGB_12                      23
#define CAM_FORMAT_ARGB_14                      24
#define CAM_FORMAT_DPCM_10_6_10                 25
#define CAM_FORMAT_DPCM_10_8_10                 26
#define CAM_FORMAT_DPCM_12_6_12                 27
#define CAM_FORMAT_DPCM_12_8_12                 28
#define CAM_FORMAT_DPCM_14_8_14                 29
#define CAM_FORMAT_DPCM_14_10_14                30
#define CAM_FORMAT_NV21                         31
#define CAM_FORMAT_NV12                         32
#define CAM_FORMAT_TP10                         33
#define CAM_FORMAT_YUV422                       34
#define CAM_FORMAT_PD8                          35
#define CAM_FORMAT_PD10                         36
#define CAM_FORMAT_UBWC_NV12                    37
#define CAM_FORMAT_UBWC_NV12_4R                 38
#define CAM_FORMAT_UBWC_TP10                    39
#define CAM_FORMAT_UBWC_P010                    40
#define CAM_FORMAT_PLAIN8_SWAP                  41
#define CAM_FORMAT_PLAIN8_10                    42
#define CAM_FORMAT_PLAIN8_10_SWAP               43
#define CAM_FORMAT_YV12                         44
#define CAM_FORMAT_Y_ONLY                       45
#define CAM_FORMAT_DPCM_12_10_12                46
#define CAM_FORMAT_PLAIN32                      47
#define CAM_FORMAT_ARGB_16                      48
#define CAM_FORMAT_MAX                          49

/* Pixel Patterns */
#define PIXEL_PATTERN_RGRGRG                    0x0
#define PIXEL_PATTERN_GRGRGR                    0x1
#define PIXEL_PATTERN_BGBGBG                    0x2
#define PIXEL_PATTERN_GBGBGB                    0x3
#define PIXEL_PATTERN_YCBYCR                    0x4
#define PIXEL_PATTERN_YCRYCB                    0x5
#define PIXEL_PATTERN_CBYCRY                    0x6
#define PIXEL_PATTERN_CRYCBY                    0x7

/* camera rotaion */
#define CAM_ROTATE_CW_0_DEGREE                  0
#define CAM_ROTATE_CW_90_DEGREE                 1
#define CAM_RORATE_CW_180_DEGREE                2
#define CAM_ROTATE_CW_270_DEGREE                3

/* camera Color Space */
#define CAM_COLOR_SPACE_BASE                    0
#define CAM_COLOR_SPACE_BT601_FULL              1
#define CAM_COLOR_SPACE_BT601625                2
#define CAM_COLOR_SPACE_BT601525                3
#define CAM_COLOR_SPACE_BT709                   4
#define CAM_COLOR_SPACE_DEPTH                   5
#define CAM_COLOR_SPACE_MAX                     6

/* camera buffer direction */
#define CAM_BUF_INPUT                           1
#define CAM_BUF_OUTPUT                          2
#define CAM_BUF_IN_OUT                          3

/* camera packet device Type */
#define CAM_PACKET_DEV_BASE                     0
#define CAM_PACKET_DEV_IMG_SENSOR               1
#define CAM_PACKET_DEV_ACTUATOR                 2
#define CAM_PACKET_DEV_COMPANION                3
#define CAM_PACKET_DEV_EEPOM                    4
#define CAM_PACKET_DEV_CSIPHY                   5
#define CAM_PACKET_DEV_OIS                      6
#define CAM_PACKET_DEV_FLASH                    7
#define CAM_PACKET_DEV_FD                       8
#define CAM_PACKET_DEV_JPEG_ENC                 9
#define CAM_PACKET_DEV_JPEG_DEC                 10
#define CAM_PACKET_DEV_VFE                      11
#define CAM_PACKET_DEV_CPP                      12
#define CAM_PACKET_DEV_CSID                     13
#define CAM_PACKET_DEV_ISPIF                    14
#define CAM_PACKET_DEV_IFE                      15
#define CAM_PACKET_DEV_ICP                      16
#define CAM_PACKET_DEV_LRME                     17
#define CAM_PACKET_DEV_TFE                      18
#define CAM_PACKET_DEV_OPE                      19
#define CAM_PACKET_DEV_MAX                      20

/* Register base type */
#define CAM_REG_DUMP_BASE_TYPE_ISP_LEFT         1
#define CAM_REG_DUMP_BASE_TYPE_ISP_RIGHT        2
#define CAM_REG_DUMP_BASE_TYPE_CAMNOC           3

/* Register dump read type */
#define CAM_REG_DUMP_READ_TYPE_CONT_RANGE       1
#define CAM_REG_DUMP_READ_TYPE_DMI              2

/* Max number of config writes to read from DMI */
#define CAM_REG_DUMP_DMI_CONFIG_MAX             5


/* constants */
#define CAM_PACKET_MAX_PLANES                   3

/**
 * struct cam_plane_cfg - Plane configuration info
 *
 * @width:                      Plane width in pixels
 * @height:                     Plane height in lines
 * @plane_stride:               Plane stride in pixel
 * @slice_height:               Slice height in line (not used by ISP)
 * @meta_stride:                UBWC metadata stride
 * @meta_size:                  UBWC metadata plane size
 * @meta_offset:                UBWC metadata offset
 * @packer_config:              UBWC packer config
 * @mode_config:                UBWC mode config
 * @tile_config:                UBWC tile config
 * @h_init:                     UBWC horizontal initial coordinate in pixels
 * @v_init:                     UBWC vertical initial coordinate in lines
 *
 */
struct cam_plane_cfg {
	uint32_t                width;
	uint32_t                height;
	uint32_t                plane_stride;
	uint32_t                slice_height;
	uint32_t                meta_stride;
	uint32_t                meta_size;
	uint32_t                meta_offset;
	uint32_t                packer_config;
	uint32_t                mode_config;
	uint32_t                tile_config;
	uint32_t                h_init;
	uint32_t                v_init;
};

/**
 * struct cam_ubwc_plane_cfg_v1 - UBWC Plane configuration info
 *
 * @port_type:                  Port Type
 * @meta_stride:                UBWC metadata stride
 * @meta_size:                  UBWC metadata plane size
 * @meta_offset:                UBWC metadata offset
 * @packer_config:              UBWC packer config
 * @mode_config_0:              UBWC mode config 0
 * @mode_config_1:              UBWC 3 mode config 1
 * @tile_config:                UBWC tile config
 * @h_init:                     UBWC horizontal initial coordinate in pixels
 * @v_init:                     UBWC vertical initial coordinate in lines
 *
 */
struct cam_ubwc_plane_cfg_v1 {
	uint32_t                port_type;
	uint32_t                meta_stride;
	uint32_t                meta_size;
	uint32_t                meta_offset;
	uint32_t                packer_config;
	uint32_t                mode_config_0;
	uint32_t                mode_config_1;
	uint32_t                tile_config;
	uint32_t                h_init;
	uint32_t                v_init;
};

/**
 * struct cam_ubwc_plane_cfg_v2 - UBWC Plane configuration info
 *
 * @port_type:                  Port Type
 * @meta_stride:                UBWC metadata stride
 * @meta_size:                  UBWC metadata plane size
 * @meta_offset:                UBWC metadata offset
 * @packer_config:              UBWC packer config
 * @mode_config:                UBWC mode config
 * @static ctrl:                UBWC static ctrl
 * @ctrl_2:                     UBWC ctrl 2
 * @tile_config:                UBWC tile config
 * @h_init:                     UBWC horizontal initial coordinate in pixels
 * @v_init:                     UBWC vertical initial coordinate in lines
 * @stats_ctrl_2:               UBWC stats control
 * @lossy_threshold0            UBWC lossy threshold 0
 * @lossy_threshold1            UBWC lossy threshold 1
 * @lossy_var_offset            UBWC offset variance thrshold
 *
 */
struct cam_ubwc_plane_cfg_v2 {
	uint32_t                port_type;
	uint32_t                meta_stride;
	uint32_t                meta_size;
	uint32_t                meta_offset;
	uint32_t                packer_config;
	uint32_t                mode_config_0;
	uint32_t                mode_config_1;
	uint32_t                tile_config;
	uint32_t                h_init;
	uint32_t                v_init;
	uint32_t                static_ctrl;
	uint32_t                ctrl_2;
	uint32_t                stats_ctrl_2;
	uint32_t                lossy_threshold_0;
	uint32_t                lossy_threshold_1;
	uint32_t                lossy_var_offset;
	uint32_t                bandwidth_limit;
	uint32_t                reserved[3];
};
/**
 * struct cam_cmd_buf_desc - Command buffer descriptor
 *
 * @mem_handle:                 Command buffer handle
 * @offset:                     Command start offset
 * @size:                       Size of the command buffer in bytes
 * @length:                     Used memory in command buffer in bytes
 * @type:                       Type of the command buffer
 * @meta_data:                  Data type for private command buffer
 *                              Between UMD and KMD
 *
 */
struct cam_cmd_buf_desc {
	int32_t                 mem_handle;
	uint32_t                offset;
	uint32_t                size;
	uint32_t                length;
	uint32_t                type;
	uint32_t                meta_data;
};

/**
 * struct cam_buf_io_cfg - Buffer io configuration for buffers
 *
 * @mem_handle:                 Mem_handle array for the buffers.
 * @offsets:                    Offsets for each planes in the buffer
 * @planes:                     Per plane information
 * @width:                      Main plane width in pixel
 * @height:                     Main plane height in lines
 * @format:                     Format of the buffer
 * @color_space:                Color space for the buffer
 * @color_pattern:              Color pattern in the buffer
 * @bpp:                        Bit per pixel
 * @rotation:                   Rotation information for the buffer
 * @resource_type:              Resource type associated with the buffer
 * @fence:                      Fence handle
 * @early_fence:                Fence handle for early signal
 * @aux_cmd_buf:                An auxiliary command buffer that may be
 *                              used for programming the IO
 * @direction:                  Direction of the config
 * @batch_size:                 Batch size in HFR mode
 * @subsample_pattern:          Subsample pattern. Used in HFR mode. It
 *                              should be consistent with batchSize and
 *                              CAMIF programming.
 * @subsample_period:           Subsample period. Used in HFR mode. It
 *                              should be consistent with batchSize and
 *                              CAMIF programming.
 * @framedrop_pattern:          Framedrop pattern
 * @framedrop_period:           Framedrop period
 * @flag:                       Flags for extra information
 * @direction:                  Buffer direction: input or output
 * @padding:                    Padding for the structure
 *
 */
struct cam_buf_io_cfg {
	int32_t                         mem_handle[CAM_PACKET_MAX_PLANES];
	uint32_t                        offsets[CAM_PACKET_MAX_PLANES];
	struct cam_plane_cfg            planes[CAM_PACKET_MAX_PLANES];
	uint32_t                        format;
	uint32_t                        color_space;
	uint32_t                        color_pattern;
	uint32_t                        bpp;
	uint32_t                        rotation;
	uint32_t                        resource_type;
	int32_t                         fence;
	int32_t                         early_fence;
	struct cam_cmd_buf_desc         aux_cmd_buf;
	uint32_t                        direction;
	uint32_t                        batch_size;
	uint32_t                        subsample_pattern;
	uint32_t                        subsample_period;
	uint32_t                        framedrop_pattern;
	uint32_t                        framedrop_period;
	uint32_t                        flag;
	uint32_t                        padding;
};

/**
 * struct cam_packet_header - Camera packet header
 *
 * @op_code:                    Camera packet opcode
 * @size:                       Size of the camera packet in bytes
 * @request_id:                 Request id for this camera packet
 * @flags:                      Flags for the camera packet
 * @padding:                    Padding
 *
 */
struct cam_packet_header {
	uint32_t                op_code;
	uint32_t                size;
	uint64_t                request_id;
	uint32_t                flags;
	uint32_t                padding;
};

/**
 * struct cam_patch_desc - Patch structure
 *
 * @dst_buf_hdl:                Memory handle for the dest buffer
 * @dst_offset:                 Offset byte in the dest buffer
 * @src_buf_hdl:                Memory handle for the source buffer
 * @src_offset:                 Offset byte in the source buffer
 *
 */
struct cam_patch_desc {
	int32_t                 dst_buf_hdl;
	uint32_t                dst_offset;
	int32_t                 src_buf_hdl;
	uint32_t                src_offset;
};

/**
 * struct cam_packet - Camera packet structure
 *
 * @header:                     Camera packet header
 * @cmd_buf_offset:             Command buffer start offset
 * @num_cmd_buf:                Number of the command buffer in the packet
 * @io_config_offset:           Buffer io configuration start offset
 * @num_io_configs:             Number of the buffer io configurations
 * @patch_offset:               Patch offset for the patch structure
 * @num_patches:                Number of the patch structure
 * @kmd_cmd_buf_index:          Command buffer index which contains extra
 *                              space for the KMD buffer
 * @kmd_cmd_buf_offset:         Offset from the beginning of the command
 *                              buffer for KMD usage.
 * @payload:                    Camera packet payload
 *
 */
struct cam_packet {
	struct cam_packet_header        header;
	uint32_t                        cmd_buf_offset;
	uint32_t                        num_cmd_buf;
	uint32_t                        io_configs_offset;
	uint32_t                        num_io_configs;
	uint32_t                        patch_offset;
	uint32_t                        num_patches;
	uint32_t                        kmd_cmd_buf_index;
	uint32_t                        kmd_cmd_buf_offset;
	uint64_t                        payload[1];

};

/**
 * struct cam_release_dev_cmd - Control payload for release devices
 *
 * @session_handle:             Session handle for the release
 * @dev_handle:                 Device handle for the release
 */
struct cam_release_dev_cmd {
	int32_t                 session_handle;
	int32_t                 dev_handle;
};

/**
 * struct cam_start_stop_dev_cmd - Control payload for start/stop device
 *
 * @session_handle:             Session handle for the start/stop command
 * @dev_handle:                 Device handle for the start/stop command
 *
 */
struct cam_start_stop_dev_cmd {
	int32_t                 session_handle;
	int32_t                 dev_handle;
};

/**
 * struct cam_config_dev_cmd - Command payload for configure device
 *
 * @session_handle:             Session handle for the command
 * @dev_handle:                 Device handle for the command
 * @offset:                     Offset byte in the packet handle.
 * @packet_handle:              Packet memory handle for the actual packet:
 *                              struct cam_packet.
 *
 */
struct cam_config_dev_cmd {
	int32_t                 session_handle;
	int32_t                 dev_handle;
	uint64_t                offset;
	uint64_t                packet_handle;
};

/**
 * struct cam_query_cap_cmd - Payload for query device capability
 *
 * @size:               Handle size
 * @handle_type:        User pointer or shared memory handle
 * @caps_handle:        Device specific query command payload
 *
 */
struct cam_query_cap_cmd {
	uint32_t        size;
	uint32_t        handle_type;
	uint64_t        caps_handle;
};

/**
 * struct cam_acquire_dev_cmd - Control payload for acquire devices
 *
 * @session_handle:     Session handle for the acquire command
 * @dev_handle:         Device handle to be returned
 * @handle_type:        Resource handle type:
 *                      1 = user pointer, 2 = mem handle
 * @num_resources:      Number of the resources to be acquired
 * @resources_hdl:      Resource handle that refers to the actual
 *                      resource array. Each item in this
 *                      array is device specific resource structure
 *
 */
struct cam_acquire_dev_cmd {
	int32_t         session_handle;
	int32_t         dev_handle;
	uint32_t        handle_type;
	uint32_t        num_resources;
	uint64_t        resource_hdl;
};

/*
 * In old version, while acquiring device the num_resources in
 * struct cam_acquire_dev_cmd will be a valid value. During ACQUIRE_DEV
 * KMD driver will return dev_handle as well as associate HW to handle.
 * If num_resources is set to the constant below, we are using
 * the new version and we do not acquire HW in ACQUIRE_DEV IOCTL.
 * ACQUIRE_DEV will only return handle and we should receive
 * ACQUIRE_HW IOCTL after ACQUIRE_DEV and that is when the HW
 * is associated with the dev_handle.
 *
 * (Data type): uint32_t
 */
#define CAM_API_COMPAT_CONSTANT                   0xFEFEFEFE

#define CAM_ACQUIRE_HW_STRUCT_VERSION_1           1
#define CAM_ACQUIRE_HW_STRUCT_VERSION_2           2

/**
 * struct cam_acquire_hw_cmd_v1 - Control payload for acquire HW IOCTL (Ver 1)
 *
 * @struct_version:     = CAM_ACQUIRE_HW_STRUCT_VERSION_1 for this struct
 *                      This value should be the first 32-bits in any structure
 *                      related to this IOCTL. So that if the struct needs to
 *                      change, we can first read the starting 32-bits, get the
 *                      version number and then typecast the data to struct
 *                      accordingly.
 * @reserved:           Reserved field for 64-bit alignment
 * @session_handle:     Session handle for the acquire command
 * @dev_handle:         Device handle to be returned
 * @handle_type:        Tells you how to interpret the variable resource_hdl-
 *                      1 = user pointer, 2 = mem handle
 * @data_size:          Total size of data contained in memory pointed
 *                      to by resource_hdl
 * @resource_hdl:       Resource handle that refers to the actual
 *                      resource data.
 */
struct cam_acquire_hw_cmd_v1 {
	uint32_t        struct_version;
	uint32_t        reserved;
	int32_t         session_handle;
	int32_t         dev_handle;
	uint32_t        handle_type;
	uint32_t        data_size;
	uint64_t        resource_hdl;
};

/**
 * struct cam_acquired_hw_info - Update the acquired hardware info
 *
 * @acquired_hw_id:     Acquired hardware mask
 * @acquired_hw_path:   Acquired path mask for an input
 *                      if input splits into multiple paths,
 *                      its updated per hardware
 * valid_acquired_hw:   Valid num of acquired hardware
 */
struct cam_acquired_hw_info {
	uint32_t    acquired_hw_id[CAM_MAX_ACQ_RES];
	uint32_t    acquired_hw_path[CAM_MAX_ACQ_RES][CAM_MAX_HW_SPLIT];
	uint32_t    valid_acquired_hw;
};

/**
 * struct cam_acquire_hw_cmd_v2 - Control payload for acquire HW IOCTL (Ver 2)
 *
 * @struct_version:     = CAM_ACQUIRE_HW_STRUCT_VERSION_2 for this struct
 *                      This value should be the first 32-bits in any structure
 *                      related to this IOCTL. So that if the struct needs to
 *                      change, we can first read the starting 32-bits, get the
 *                      version number and then typecast the data to struct
 *                      accordingly.
 * @reserved:           Reserved field for 64-bit alignment
 * @session_handle:     Session handle for the acquire command
 * @dev_handle:         Device handle to be returned
 * @handle_type:        Tells you how to interpret the variable resource_hdl-
 *                      1 = user pointer, 2 = mem handle
 * @data_size:          Total size of data contained in memory pointed
 *                      to by resource_hdl
 * @resource_hdl:       Resource handle that refers to the actual
 *                      resource data.
 */
struct cam_acquire_hw_cmd_v2 {
	uint32_t                    struct_version;
	uint32_t                    reserved;
	int32_t                     session_handle;
	int32_t                     dev_handle;
	uint32_t                    handle_type;
	uint32_t                    data_size;
	uint64_t                    resource_hdl;
	struct cam_acquired_hw_info hw_info;
};

#define CAM_RELEASE_HW_STRUCT_VERSION_1           1

/**
 * struct cam_release_hw_cmd_v1 - Control payload for release HW IOCTL (Ver 1)
 *
 * @struct_version:     = CAM_RELEASE_HW_STRUCT_VERSION_1 for this struct
 *                      This value should be the first 32-bits in any structure
 *                      related to this IOCTL. So that if the struct needs to
 *                      change, we can first read the starting 32-bits, get the
 *                      version number and then typecast the data to struct
 *                      accordingly.
 * @reserved:           Reserved field for 64-bit alignment
 * @session_handle:     Session handle for the release
 * @dev_handle:         Device handle for the release
 */
struct cam_release_hw_cmd_v1 {
	uint32_t                struct_version;
	uint32_t                reserved;
	int32_t                 session_handle;
	int32_t                 dev_handle;
};

/**
 * struct cam_flush_dev_cmd - Control payload for flush devices
 *
 * @version:           Version
 * @session_handle:    Session handle for the acquire command
 * @dev_handle:        Device handle to be returned
 * @flush_type:        Flush type:
 *                     0 = flush specific request
 *                     1 = flush all
 * @reserved:          Reserved for 64 bit aligngment
 * @req_id:            Request id that needs to cancel
 *
 */
struct cam_flush_dev_cmd {
	uint64_t       version;
	int32_t        session_handle;
	int32_t        dev_handle;
	uint32_t       flush_type;
	uint32_t       reserved;
	int64_t        req_id;
};

/**
 * struct cam_ubwc_config - UBWC Configuration Payload
 *
 * @api_version:         UBWC config api version
 * @num_ports:           Number of ports to be configured
 * @ubwc_plane_config:   Array of UBWC configurations per port
 *                       Size [CAM_PACKET_MAX_PLANES - 1] per port
 *                       as UBWC is supported on Y & C planes
 *                       and therefore a max size of 2 planes
 *
 */
struct cam_ubwc_config {
	uint32_t   api_version;
	uint32_t   num_ports;
	struct cam_ubwc_plane_cfg_v1
		   ubwc_plane_cfg[1][CAM_PACKET_MAX_PLANES - 1];
};

/**
 * struct cam_ubwc_config_v2 - UBWC Configuration Payload
 *
 * @api_version:         UBWC config api version
 * @num_ports:           Number of ports to be configured
 * @ubwc_plane_config:   Array of UBWC configurations per port
 *                       Size [CAM_PACKET_MAX_PLANES - 1] per port
 *                       as UBWC is supported on Y & C planes
 *                       and therefore a max size of 2 planes
 *
 */
struct cam_ubwc_config_v2 {
	uint32_t   api_version;
	uint32_t   num_ports;
	struct cam_ubwc_plane_cfg_v2
	   ubwc_plane_cfg[1][CAM_PACKET_MAX_PLANES - 1];
};

/**
 * struct cam_cmd_mem_region_info -
 *              Cmd buffer region info
 *
 * @mem_handle : Memory handle of the region
 * @offset     : Offset if any
 * @size       : Size of the region
 * @flags      : Flags if any
 */
struct cam_cmd_mem_region_info {
	int32_t   mem_handle;
	uint32_t  offset;
	uint32_t  size;
	uint32_t  flags;
};

/**
 * struct cam_cmd_mem_regions -
 *        List of multiple memory descriptors of
 *        of different regions
 *
 * @version        : Version number
 * @num_regions    : Number of regions
 * @map_info_array : Array of all the regions
 */
struct cam_cmd_mem_regions {
	uint32_t version;
	uint32_t num_regions;
	struct cam_cmd_mem_region_info map_info_array[1];
};

/**
 * struct cam_reg_write_desc - Register write descriptor
 *
 * @offset               : Register offset at which 'value' needs to written
 * @value                : Register value to write
 */
struct cam_reg_write_desc {
	uint32_t   offset;
	uint32_t   value;
};

/**
 * struct cam_reg_range_read_desc - Descriptor to provide read info
 *
 * @offset               : Register offset address to start with
 * @num_values           : Number of values to read
 */
struct cam_reg_range_read_desc {
	uint32_t   offset;
	uint32_t   num_values;
};

/**
 * struct cam_dmi_read_desc - Descriptor to provide DMI read info
 *
 * @num_pre_writes       : Number of registers to write before reading DMI data
 * @num_post_writes      : Number of registers to write after reading DMI data
 * @pre_read_config      : Registers to write before reading DMI data
 * @dmi_data_read        : DMI Register, number of values to read to dump
 *                         DMI data
 * @post_read_config     : Registers to write after reading DMI data
 */
struct cam_dmi_read_desc {
	uint32_t                         num_pre_writes;
	uint32_t                         num_post_writes;
	struct cam_reg_write_desc        pre_read_config[
						CAM_REG_DUMP_DMI_CONFIG_MAX];
	struct cam_reg_range_read_desc   dmi_data_read;
	struct cam_reg_write_desc        post_read_config[
						CAM_REG_DUMP_DMI_CONFIG_MAX];
};

/**
 * struct cam_reg_read_info - Register read info for both reg continuous read
 *                            or DMI read
 *
 * @type                 : Whether Register range read or DMI read
 * @reg_read             : Range of registers to read
 * @dmi_read             : DMI data to read
 */
struct cam_reg_read_info {
	uint32_t                                type;
	uint32_t                                reserved;
	union {
		struct cam_reg_range_read_desc  reg_read;
		struct cam_dmi_read_desc        dmi_read;
	};
};

/**
 * struct cam_reg_dump_out_buffer -Buffer info for dump data to be provided
 *
 * @req_id               : Request ID corresponding to reg dump data
 * @bytes_written        : Number of bytes written
 * @dump_data            : Register dump data
 */
struct cam_reg_dump_out_buffer {
	uint64_t   req_id;
	uint32_t   bytes_written;
	uint32_t   dump_data[1];
};

/**
 * struct cam_reg_dump_desc - Descriptor to provide dump info
 *
 * @reg_base_type        : Register base type, e.g. ISP_LEFT, ISP_RIGHT, CAMNOC
 * @dump_buffer_offset   : Offset from base of mem_handle at which Register dump
 *                         will be written for this set
 * @dump_buffer_size     : Available size in bytes for writing dump values
 * @num_read_range       : Number register range reads (Continuous + DMI)
 * @read_range           : Read range info
 */
struct cam_reg_dump_desc {
	uint32_t                   reg_base_type;
	uint32_t                   dump_buffer_offset;
	uint32_t                   dump_buffer_size;
	uint32_t                   num_read_range;
	struct cam_reg_read_info   read_range[1];
};

/**
 * struct cam_reg_dump_input_info - Info about required dump sets
 *
 * @num_dump_sets        : Number of different dump sets (base types) given
 * @dump_set_offsets     : Points to the given dump description structures
 *                         (cam_reg_dump_desc)
 */
struct cam_reg_dump_input_info {
	uint32_t                   num_dump_sets;
	uint32_t                   dump_set_offsets[1];
};

/**
 * struct cam_dump_req_cmd -
 *        Dump the information of issue req id
 *
 * @issue_req_id   : Issue Request Id
 * @offset         : Offset for the buffer
 * @buf_handle     : Buffer Handle
 * @error_type     : Error type, using it, dumping information can be extended
 * @session_handle : Session Handle
 * @link_hdl       : link handle
 * @dev_handle     : Device Handle
 */
struct cam_dump_req_cmd {
	uint64_t       issue_req_id;
	size_t         offset;
	uint32_t       buf_handle;
	uint32_t       error_type;
	int32_t        session_handle;
	int32_t        link_hdl;
	int32_t        dev_handle;
};

#endif /* __UAPI_CAM_DEFS_H__ */
