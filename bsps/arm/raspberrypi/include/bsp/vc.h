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

#ifndef LIBBSP_ARM_RASPBERRYPI_VC_H
#define LIBBSP_ARM_RASPBERRYPI_VC_H

/**
 * @defgroup raspberrypi_vc Register Definitions
 *
 * @ingroup RTEMSBSPsARMRaspberryPi
 *
 * @brief Register Definitions
 *
 * @{
 */

typedef struct {
  uint32_t width;
  uint32_t height;
} bcm2835_get_display_size_entries;

int bcm2835_mailbox_get_display_size(
  bcm2835_get_display_size_entries *_entries );

typedef enum {
  bcm2835_mailbox_pixel_order_bgr = 0,
  bcm2835_mailbox_pixel_order_rgb = 1
} bcm2835_pixel_order;

typedef enum {
  bcm2835_mailbox_alpha_mode_0_opaque       = 0,
  bcm2835_mailbox_alpha_mode_0_transparent  = 1,
  bcm2835_mailbox_alpha_mode_ignored        = 2
} bcm2835_alpha_mode;

typedef struct {
  uint32_t xres;
  uint32_t yres;
  uint32_t xvirt;
  uint32_t yvirt;
  uint32_t depth;
  bcm2835_pixel_order pixel_order;
  bcm2835_alpha_mode alpha_mode;
  uint32_t voffset_x;
  uint32_t voffset_y;
  uint32_t overscan_left;
  uint32_t overscan_right;
  uint32_t overscan_top;
  uint32_t overscan_bottom;
  uint32_t base;
  size_t size;
  uint32_t pitch;
} bcm2835_init_frame_buffer_entries;

int bcm2835_mailbox_init_frame_buffer(
  bcm2835_init_frame_buffer_entries *_entries );

typedef struct {
  uint32_t pitch;
} bcm2835_get_pitch_entries;

int bcm2835_mailbox_get_pitch( bcm2835_get_pitch_entries *_entries );

typedef struct {
  char cmdline[ 1024 ];
} bcm2835_get_cmdline_entries;

int bcm2835_mailbox_get_cmdline( bcm2835_get_cmdline_entries *_entries );

typedef enum {
  bcm2835_mailbox_power_udid_sd_card = 0x00000000u,
  bcm2835_mailbox_power_udid_uart0 = 0x00000001u,
  bcm2835_mailbox_power_udid_uart1 = 0x00000002u,
  bcm2835_mailbox_power_udid_usb_hcd = 0x00000003u,
  bcm2835_mailbox_power_udid_i2c0 = 0x00000004u,
  bcm2835_mailbox_power_udid_i2c1 = 0x00000005u,
  bcm2835_mailbox_power_udid_i2c2 = 0x00000006u,
  bcm2835_mailbox_power_udid_spi = 0x00000007u,
  bcm2835_mailbox_power_udid_ccp2tx = 0x00000008u,
} bcm2835_power_device_id;

typedef struct {
  bcm2835_power_device_id dev_id;
  uint32_t state;
} bcm2835_set_power_state_entries;

#define BCM2835_MAILBOX_SET_POWER_STATE_REQ_ON ( 1 << 0 )
#define BCM2835_MAILBOX_SET_POWER_STATE_REQ_WAIT ( 1 << 1 )
#define BCM2835_MAILBOX_POWER_STATE_ON ( 1 << 0 )
#define BCM2835_MAILBOX_POWER_STATE_NODEV ( 1 << 1 )
int bcm2835_mailbox_set_power_state( bcm2835_set_power_state_entries *_entries );

int bcm2835_mailbox_get_power_state( bcm2835_set_power_state_entries *_entries );

typedef struct {
  uint32_t base;
  size_t size;
} bcm2835_get_arm_memory_entries;

int bcm2835_mailbox_get_arm_memory( bcm2835_get_arm_memory_entries *_entries );

typedef struct {
  uint32_t base;
  size_t size;
} bcm2835_get_vc_memory_entries;

int bcm2835_mailbox_get_vc_memory( bcm2835_get_vc_memory_entries *_entries );

typedef struct {
  uint32_t fw_rev;
} bcm2835_mailbox_get_fw_rev_entries;

int bcm2835_mailbox_get_firmware_revision(
  bcm2835_mailbox_get_fw_rev_entries *_entries );

typedef struct {
  uint32_t spec;
} bcm2835_get_board_spec_entries;

int bcm2835_mailbox_get_board_model( bcm2835_get_board_spec_entries *_entries );

int bcm2835_mailbox_get_board_revision(
  bcm2835_get_board_spec_entries *_entries );

typedef struct {
  uint64_t board_serial;
} bcm2835_get_board_serial_entries;

int bcm2835_mailbox_get_board_serial(
  bcm2835_get_board_serial_entries *_entries );

typedef struct {
  uint32_t clock_id;
  uint32_t clock_rate;
} bcm2835_get_clock_rate_entries;

int bcm2835_mailbox_get_clock_rate(
  bcm2835_get_clock_rate_entries *_entries );
/** @} */

#endif /* LIBBSP_ARM_RASPBERRYPI_VC_H */
