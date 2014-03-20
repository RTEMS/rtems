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
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
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

/* Default speed = 400kbps, default timeout = 100ms, n_ac_max is in bytes */
#define SD_CARD_N_AC_MAX_DEFAULT 5000

typedef struct {
	const char *device_name;
	rtems_device_minor_number bus;
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
	uint32_t retries;
} sd_card_driver_entry;

extern sd_card_driver_entry sd_card_driver_table [];

extern size_t sd_card_driver_table_size;

rtems_status_code sd_card_register( void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBI2C_SD_CARD_H */
