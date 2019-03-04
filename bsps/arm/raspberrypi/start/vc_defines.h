/**
 * @file
 *
 * @ingroup raspberrypi_vc
 *
 * @brief video core support.
 *
 */

/*
 * Copyright (c) 2015 Yang Qiao
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *
 *  http://www.rtems.org/license/LICENSE
 *
 */

#ifndef LIBBSP_ARM_RASPBERRYPI_VC_DEFINES_H
#define LIBBSP_ARM_RASPBERRYPI_VC_DEFINES_H

#include <string.h>

/**
 * @defgroup raspberrypi_vc Register Definitions
 *
 * @ingroup RTEMSBSPsARMRaspberryPi
 *
 * @brief Register Definitions
 *
 * @{
 */

/**
 * @name Mailbox Property tags (ARM <-> VC)
 *
 * @{
 */

#define BCM2835_MBOX_BUF_CODE_PROCESS_REQUEST         0x00000000
#define BCM2835_MBOX_BUF_CODE_REQUEST_SUCCEED         0x80000000
#define BCM2835_MBOX_BUF_CODE_REQUEST_PARSING_ERROR   0x80000001
#define BCM2835_MBOX_TAG_VAL_LEN_REQUEST              0x00000000
#define BCM2835_MBOX_TAG_VAL_LEN_RESPONSE             0x80000000

/**
 * @brief Buffer Header
 *
 * All message buffers should start with this header, called 'hdr'.
 * Then it should be followed by a list of tags.
 * An additional empty tag should be added at the end, called 'end_tag'
 *
 */
typedef struct {
  /**
   * @brief Buffer Size
   *
   * The size of whole structure including the header(size and code),
   *   tags, end tag and padding.
   */
  uint32_t buf_size;

  /**
   * @brief Buffer Code
   *
   * The code indicate the status of operation.
   * Use function BCM2835_MAILBOX_BUFFER_IS_SUCCEED(bcm2835_mailbox_buffer*)
   * to check if the operation is succeed
   */
  uint32_t buf_code;
} bcm2835_mbox_buf_hdr;

/**
 * @brief Tag Header
 *
 * Tag is used for getting/testing/setting an property.
 * All tags should start with this header.
 * Then it should be followed by a data buffer for request and response.
 *
 */
typedef struct {
  /**
   * @brief Property Tag ID
   */
  uint32_t tag;
  /**
   * @brief The size of request qnd responce buffer
   */
  uint32_t val_buf_size;
  /**
   * @brief The size of response buffer set by videocore.
   *
   * upper bit reserved for request/response indicator
   */
  uint32_t val_len;
} bcm2835_mbox_tag_hdr;

#define BCM2835_MBOX_TAG_REPLY_IS_SET( _t_ ) \
   ( ( _t_ )->tag_hdr.val_len & 0x80000000 )

#define BCM2835_MBOX_INIT_BUF( _m_ ) { \
    memset( ( _m_ ), 0,  sizeof( *( _m_ ) ) ); \
    ( _m_ )->hdr.buf_size = (void *)&(( _m_ )->end_tag) + 4 - (void *)( _m_ ); \
    ( _m_ )->hdr.buf_code = BCM2835_MBOX_BUF_CODE_PROCESS_REQUEST; \
    ( _m_ )->end_tag = 0; \
}

#define BCM2835_MBOX_INIT_TAG( _t_, _id_ ) { \
    ( _t_ )->tag_hdr.tag = _id_; \
    ( _t_ )->tag_hdr.val_buf_size = sizeof( ( _t_ )->body ); \
    ( _t_ )->tag_hdr.val_len = sizeof( ( _t_ )->body.req ); \
}

#define BCM2835_MBOX_INIT_TAG_NO_REQ( _t_, _id_ ) { \
    ( _t_ )->tag_hdr.tag = _id_; \
    ( _t_ )->tag_hdr.val_buf_size = sizeof( ( _t_ )->body ); \
    ( _t_ )->tag_hdr.val_len = 0; \
}

/*
 * Mailbox buffers has to be aligned to 16 bytes because
 * 4 LSB bits of the BCM2835_MBOX_WRITE and BCM2835_MBOX_READ
 * registers are used to pass channel number.
 *
 * But there is another requirement for buffer allocation
 * as well when interface is called after cache is enabled.
 * The buffer should not share cache line with another variable
 * which can be updated during data exchange with VideoCore.
 * If cache is filled to satisfy another variable update
 * during VideoCore output is stored into main memory then
 * part of received data can be lost.
 *
 * Cache line length is 64 bytes for RPi2 Cortex-A7 data cache
 * so align buffers to this value.
 */
#define BCM2835_MBOX_BUF_ALIGN_ATTRIBUTE __attribute__( ( aligned( 64 ) ) )

/* Video Core */
#define BCM2835_MAILBOX_TAG_FIRMWARE_REVISION   0x00000001
typedef struct {
  bcm2835_mbox_tag_hdr tag_hdr;
  union {
    struct {
    } req;
    struct {
      uint32_t rev;
    } resp;
  } body;
} bcm2835_mbox_tag_get_fw_rev;

/* Hardware */
#define BCM2835_MAILBOX_TAG_GET_BOARD_MODEL     0x00010001
#define BCM2835_MAILBOX_TAG_GET_BOARD_VERSION   0x00010002
typedef struct {
  bcm2835_mbox_tag_hdr tag_hdr;
  union {
    struct {
    } req;
    struct {
      uint32_t spec;
    } resp;
  } body;
} bcm2835_mbox_tag_get_board_spec;

#if (BSP_IS_RPI2 == 1)
#define BCM2836_MAILBOX_BOARD_V_2_B             0x4
#else
#define BCM2835_MAILBOX_BOARD_V_B_I2C0_2        0x2
#define BCM2835_MAILBOX_BOARD_V_B_I2C0_3        0x3
#define BCM2835_MAILBOX_BOARD_V_B_I2C1_4        0x4
#define BCM2835_MAILBOX_BOARD_V_B_I2C1_5        0x5
#define BCM2835_MAILBOX_BOARD_V_B_I2C1_6        0x6
#define BCM2835_MAILBOX_BOARD_V_A_7             0x7
#define BCM2835_MAILBOX_BOARD_V_A_8             0x8
#define BCM2835_MAILBOX_BOARD_V_A_9             0x9
#define BCM2835_MAILBOX_BOARD_V_B_REV2_d        0xd
#define BCM2835_MAILBOX_BOARD_V_B_REV2_e        0xe
#define BCM2835_MAILBOX_BOARD_V_B_REV2_f        0xf
#define BCM2835_MAILBOX_BOARD_V_B_PLUS          0x10
#define BCM2835_MAILBOX_BOARD_V_CM              0x11
#define BCM2835_MAILBOX_BOARD_V_A_PLUS          0x12
#endif

#define BCM2835_MAILBOX_TAG_GET_BOARD_MAC       0x00010003
#define BCM2835_MAILBOX_TAG_GET_BOARD_SERIAL    0x00010004
typedef struct {
  bcm2835_mbox_tag_hdr tag_hdr;
  union {
    struct {
    } req;
    struct {
      uint64_t board_serial;
    } resp;
  } body;
} bcm2835_mbox_tag_get_board_serial;

#define BCM2835_MAILBOX_TAG_GET_ARM_MEMORY      0x00010005
typedef struct {
  bcm2835_mbox_tag_hdr tag_hdr;
  union {
    struct {
    } req;
    struct {
      uint32_t base;
      uint32_t size;
    } resp;
  } body;
} bcm2835_mbox_tag_get_arm_memory;

#define BCM2835_MAILBOX_TAG_GET_VC_MEMORY       0x00010006
typedef struct {
  bcm2835_mbox_tag_hdr tag_hdr;
  union {
    struct {
    } req;
    struct {
      uint32_t base;
      uint32_t size;
    } resp;
  } body;
} bcm2835_mbox_tag_get_vc_memory;

#define BCM2835_MAILBOX_TAG_GET_CLOCKS          0x00010007
typedef struct {
  bcm2835_mbox_tag_hdr tag_hdr;
  union {
    struct {
      uint32_t clock_id;
    } req;
    struct {
      uint32_t clock_id;
      uint32_t clock_rate;
    } resp;
  } body;
} bcm2835_mbox_tag_get_clock_rate;

/* Config */
#define BCM2835_MAILBOX_TAG_GET_CMD_LINE        0x00050001
typedef struct {
  bcm2835_mbox_tag_hdr tag_hdr;
  union {
    struct {
    } req;
    struct {
      uint8_t cmdline[ 1024 ];
    } resp;
  } body;
} bcm2835_mbox_tag_get_cmd_line;

/* Shared resource management */
#define BCM2835_MAILBOX_TAG_GET_DMA_CHANNELS    0x00060001

/* Power */
#define BCM2835_MAILBOX_POWER_UDID_SD_Card           0x00000000
#define BCM2835_MAILBOX_POWER_UDID_UART0             0x00000001
#define BCM2835_MAILBOX_POWER_UDID_UART1             0x00000002
#define BCM2835_MAILBOX_POWER_UDID_USB_HCD           0x00000003
#define BCM2835_MAILBOX_POWER_UDID_I2C0              0x00000004
#define BCM2835_MAILBOX_POWER_UDID_I2C1              0x00000005
#define BCM2835_MAILBOX_POWER_UDID_I2C2              0x00000006
#define BCM2835_MAILBOX_POWER_UDID_SPI               0x00000007
#define BCM2835_MAILBOX_POWER_UDID_CCP2TX            0x00000008

#define BCM2835_MAILBOX_TAG_GET_POWER_STATE     0x00020001
typedef struct {
  bcm2835_mbox_tag_hdr tag_hdr;
  union {
    struct {
      uint32_t dev_id;
    } req;
    struct {
      uint32_t dev_id;
      uint32_t state;
    } resp;
  } body;
} bcm2835_mbox_tag_get_power_state;

#define BCM2835_MAILBOX_POWER_STATE_RESP_ON       (1 << 0)
#define BCM2835_MAILBOX_POWER_STATE_RESP_NODEV    (1 << 1)

#define BCM2835_MAILBOX_TAG_GET_TIMING          0x00020002
#define BCM2835_MAILBOX_TAG_SET_POWER_STATE     0x00028001
typedef struct {
  bcm2835_mbox_tag_hdr tag_hdr;
  union {
    struct {
      uint32_t dev_id;
      uint32_t state;
    } req;
    struct {
      uint32_t dev_id;
      uint32_t state;
    } resp;
  } body;
} bcm2835_mbox_tag_power_state;

#ifndef BCM2835_MAILBOX_SET_POWER_STATE_REQ_ON
/* Value is defined as a part of public VideoCore API */
#define BCM2835_MAILBOX_SET_POWER_STATE_REQ_ON    (1 << 0)
#define BCM2835_MAILBOX_SET_POWER_STATE_REQ_WAIT  (1 << 1)
#endif

/* Clocks */
#define BCM2835_MAILBOX_UCID_CLOCK_RESERVED          0x000000000
#define BCM2835_MAILBOX_UCID_CLOCK_EMMC              0x000000001
#define BCM2835_MAILBOX_UCID_CLOCK_UART              0x000000002
#define BCM2835_MAILBOX_UCID_CLOCK_ARM               0x000000003
#define BCM2835_MAILBOX_UCID_CLOCK_CORE              0x000000004
#define BCM2835_MAILBOX_UCID_CLOCK_V3D               0x000000005
#define BCM2835_MAILBOX_UCID_CLOCK_H264              0x000000006
#define BCM2835_MAILBOX_UCID_CLOCK_ISP               0x000000007
#define BCM2835_MAILBOX_UCID_CLOCK_SDRAM             0x000000008
#define BCM2835_MAILBOX_UCID_CLOCK_PIXEL             0x000000009
#define BCM2835_MAILBOX_UCID_CLOCK_PWM               0x00000000a

#define BCM2835_MAILBOX_TAG_GET_CLOCK_STATE     0x00030001
#define BCM2835_MAILBOX_TAG_SET_CLOCK_STATE     0x00038001
#define BCM2835_MAILBOX_TAG_GET_CLOCK_RATE      0x00030002
#define BCM2835_MAILBOX_TAG_SET_CLOCK_RATE      0x00038002
#define BCM2835_MAILBOX_TAG_GET_MAX_CLOCK_RATE  0x00030004
#define BCM2835_MAILBOX_TAG_GET_MIN_CLOCK_RATE  0x00030007
#define BCM2835_MAILBOX_TAG_GET_TRUBO           0x00030009
#define BCM2835_MAILBOX_TAG_SET_TURBO           0x00038009

#define BCM2835_MAILBOX_TAG_GET_DOMAIN_STATE    0x00030030
#define BCM2835_MAILBOX_TAG_SET_DOMAIN_STATE    0x00038030

/* Voltage */
#define BCM2835_MAILBOX_VOLTAGE_RESERVED_UVID        0x000000000
#define BCM2835_MAILBOX_VOLTAGE_CORE_UVID            0x000000001
#define BCM2835_MAILBOX_VOLTAGE_SDRAM_C_UVID         0x000000002
#define BCM2835_MAILBOX_VOLTAGE_SDRAM_P_UVID         0x000000003
#define BCM2835_MAILBOX_VOLTAGE_SDRAM_I_UVID         0x000000004

#define BCM2835_MAILBOX_TAG_GET_VOLTAGE         0x00030003
#define BCM2835_MAILBOX_TAG_SET_VOLTAGE         0x00038003
#define BCM2835_MAILBOX_TAG_GET_MAX_VOLTAGE     0x00030005
#define BCM2835_MAILBOX_TAG_GET_MIN_VOLTAGE     0x00030008
#define BCM2835_MAILBOX_TAG_GET_TEMPERATURE     0x00030006
#define BCM2835_MAILBOX_TAG_GET_MAX_TEMPERATURE 0x0003000a

/* Memory */
#define BCM2835_MAILBOX_TAG_ALLOCATE_MEMORY     0x0003000c
#define BCM2835_MAILBOX_TAG_LOCK_MEMORY         0x0003000d
#define BCM2835_MAILBOX_TAG_UNLOCK_MEMORY       0x0003000e
#define BCM2835_MAILBOX_TAG_RELEASE_MEMORY      0x0003000f
#define BCM2835_MAILBOX_TAG_EXECUTE_CODE        0x00030010
#define BCM2835_MAILBOX_TAG_GET_DISPMANX_RESOURCE_MEM_HANDLE     0x00030014

#define BCM2835_MAILBOX_TAG_GET_EDID_BLOCK      0x00030020

/* Framebuffer */
#define BCM2835_MAILBOX_TAG_ALLOCATE_BUFFER     0x00040001
typedef struct {
  bcm2835_mbox_tag_hdr tag_hdr;
  union {
    struct {
      uint32_t align;
    } req;
    struct {
      uint32_t base;
      uint32_t size;
    } resp;
  } body;
} bcm2835_mbox_tag_allocate_buffer;

#define BCM2835_MAILBOX_TAG_RELEASE_BUFFER      0x00048001

#define BCM2835_MAILBOX_TAG_BLANK_SCREEN        0x00040002

#define BCM2835_MAILBOX_TAG_GET_DISPLAY_SIZE          0x00040003
#define BCM2835_MAILBOX_TAG_TEST_DISPLAY_SIZE         0x00044003
#define BCM2835_MAILBOX_TAG_SET_DISPLAY_SIZE          0x00048003
typedef struct {
  bcm2835_mbox_tag_hdr tag_hdr;
  union {
    struct {
      uint32_t width;
      uint32_t height;
    } req;
    struct {
      uint32_t width;
      uint32_t height;
    } resp;
  } body;
} bcm2835_mbox_tag_display_size;

#define BCM2835_MAILBOX_TAG_GET_VIRTUAL_SIZE     0x00040004
#define BCM2835_MAILBOX_TAG_TEST_VIRTUAL_SIZE    0x00044004
#define BCM2835_MAILBOX_TAG_SET_VIRTUAL_SIZE     0x00048004
typedef struct {
  bcm2835_mbox_tag_hdr tag_hdr;
  union {
    struct {
      uint32_t vwidth;
      uint32_t vheight;
    } req;
    struct {
      uint32_t vwidth;
      uint32_t vheight;
    } resp;
  } body;
} bcm2835_mbox_tag_virtual_size;

#define BCM2835_MAILBOX_TAG_GET_DEPTH           0x00040005
#define BCM2835_MAILBOX_TAG_TEST_DEPTH          0x00044005
#define BCM2835_MAILBOX_TAG_SET_DEPTH           0x00048005
typedef struct {
  bcm2835_mbox_tag_hdr tag_hdr;
  union {
    struct {
      uint32_t depth;
    } req;
    struct {
      uint32_t depth;
    } resp;
  } body;
} bcm2835_mbox_tag_depth;

#define BCM2835_MAILBOX_TAG_GET_PIXEL_ORDER     0x00040006
#define BCM2835_MAILBOX_TAG_TEST_PIXEL_ORDER    0x00044006
#define BCM2835_MAILBOX_TAG_SET_PIXEL_ORDER     0x00048006

#define BCM2835_MAILBOX_PIXEL_ORDER_BGR    0
#define BCM2835_MAILBOX_PIXEL_ORDER_RGB    1
typedef struct {
  bcm2835_mbox_tag_hdr tag_hdr;
  union {
    struct {
      uint32_t pixel_order;
    } req;
    struct {
      uint32_t pixel_order;
    } resp;
  } body;
} bcm2835_mbox_tag_pixel_order;

#define BCM2835_MAILBOX_TAG_GET_ALPHA_MODE      0x00040007
#define BCM2835_MAILBOX_TAG_TEST_ALPHA_MODE     0x00044007
#define BCM2835_MAILBOX_TAG_SET_ALPHA_MODE      0x00048007
typedef struct {
  bcm2835_mbox_tag_hdr tag_hdr;
  union {
    struct {
      uint32_t alpha_mode;
    } req;
    struct {
      uint32_t alpha_mode;
    } resp;
  } body;
} bcm2835_mbox_tag_alpha_mode;

#define BCM2835_MAILBOX_ALPHA_MODE_0_OPAQUE  0
#define BCM2835_MAILBOX_ALPHA_MODE_0_TRANSPARENT 1
#define BCM2835_MAILBOX_ALPHA_MODE_IGNORED   2

#define BCM2835_MAILBOX_TAG_GET_PITCH           0x00040008
typedef struct {
  bcm2835_mbox_tag_hdr tag_hdr;
  union {
    struct {
    } req;
    struct {
      uint32_t pitch;
    } resp;
  } body;
} bcm2835_mbox_tag_get_pitch;

#define BCM2835_MAILBOX_TAG_GET_VIRTUAL_OFFSET  0x00040009
#define BCM2835_MAILBOX_TAG_TEST_VIRTUAL_OFFSET 0x00044009
#define BCM2835_MAILBOX_TAG_SET_VIRTUAL_OFFSET  0x00048009
typedef struct {
  bcm2835_mbox_tag_hdr tag_hdr;
  union {
    struct {
      uint32_t voffset_x;
      uint32_t voffset_y;
    } req;
    struct {
      uint32_t voffset_x;
      uint32_t voffset_y;
    } resp;
  } body;
} bcm2835_mbox_tag_virtual_offset;

#define BCM2835_MAILBOX_TAG_GET_OVERSCAN        0x0004000a
#define BCM2835_MAILBOX_TAG_TEST_OVERSCAN       0x0004400a
#define BCM2835_MAILBOX_TAG_SET_OVERSCAN        0x0004800a
typedef struct {
  bcm2835_mbox_tag_hdr tag_hdr;
  union {
    struct {
      uint32_t overscan_top;
      uint32_t overscan_bottom;
      uint32_t overscan_left;
      uint32_t overscan_right;
    } req;
    struct {
      uint32_t overscan_top;
      uint32_t overscan_bottom;
      uint32_t overscan_left;
      uint32_t overscan_right;
    } resp;
  } body;
} bcm2835_mbox_tag_overscan;

#define BCM2835_MAILBOX_TAG_GET_PALETTE         0x0004000b
#define BCM2835_MAILBOX_TAG_TEST_PALETTE        0x0004400b
#define BCM2835_MAILBOX_TAG_SET_PALETTE         0x0004800b
#define BCM2835_MAILBOX_TAG_SET_CURSOR_INFO     0x00008011
#define BCM2835_MAILBOX_TAG_SET_CURSOR_STATE    0x00008010

/** @} */

/** @} */

#endif /* LIBBSP_ARM_RASPBERRYPI_VC_DEFINES_H */
