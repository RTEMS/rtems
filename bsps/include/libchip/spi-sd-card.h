/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief SD Card LibI2C driver.
 */

/*
 * Copyright (c) 2008 embedded brains GmbH & Co. KG
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
