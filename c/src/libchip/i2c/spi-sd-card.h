/**
 * @file
 *
 * @brief SD Card LibI2C driver.
 */

/*
 * Copyright (c) 2008
 * Embedded Brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * rtems@embedded-brains.de
 *
 * The license and distribution terms for this file may be found in the file
 * LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBI2C_SD_CARD_H
#define LIBI2C_SD_CARD_H

#include <stdint.h>
#include <stdbool.h>

#include <rtems/libi2c.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SD_CARD_IDLE_TOKEN 0xff

/**
 * 1 idle token before command
 * 6 bytes for the command
 * 1 to 8 bytes for response start (N_CR)
 * 1 to 2 bytes for response
 * 1 idle token after command (minimum N_RC)
 */
#define SD_CARD_COMMAND_SIZE 18

#define SD_CARD_TRANSFER_MODE_DEFAULT { .baudrate = 400000, .bits_per_char = 8, .lsb_first = FALSE, .clock_inv = FALSE, .clock_phs = FALSE, .idle_char = SD_CARD_IDLE_TOKEN }

#define SD_CARD_COMMAND_DEFAULT { \
	SD_CARD_IDLE_TOKEN, \
	0x40, 0, 0, 0, 0, 0x95, \
	SD_CARD_IDLE_TOKEN, SD_CARD_IDLE_TOKEN, \
	SD_CARD_IDLE_TOKEN, SD_CARD_IDLE_TOKEN, \
	SD_CARD_IDLE_TOKEN, SD_CARD_IDLE_TOKEN, \
	SD_CARD_IDLE_TOKEN, SD_CARD_IDLE_TOKEN, \
	SD_CARD_IDLE_TOKEN, SD_CARD_IDLE_TOKEN, \
	SD_CARD_IDLE_TOKEN \
}

#define SD_CARD_N_AC_MAX_DEFAULT 8

typedef struct {
	rtems_libi2c_drv_t driver;
	int table_index;
	rtems_device_minor_number minor;
	const char *device_name;
	const char *disk_device_name;
	rtems_libi2c_tfr_mode_t transfer_mode;
	uint8_t command [SD_CARD_COMMAND_SIZE];
	uint8_t response [SD_CARD_COMMAND_SIZE];
	int response_index;
	uint32_t n_ac_max; 
	uint32_t block_number;
	uint32_t block_size;
	uint32_t block_size_shift;
	bool busy;
	bool verbose;
	bool schedule_if_busy;
} sd_card_driver_entry;

extern const rtems_driver_address_table sd_card_driver_ops;

extern sd_card_driver_entry sd_card_driver_table [];

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBI2C_SD_CARD_H */
