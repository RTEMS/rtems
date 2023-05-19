/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup mpc55xx_dspi
 *
 * @brief Header file for the LibI2C bus driver for the Deserial Serial Peripheral Interface (DSPI).
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

/**
 * @defgroup mpc55xx_dspi Deserial Serial Peripheral Interface (DSPI)
 *
 * @ingroup RTEMSBSPsPowerPCMPC55XX
 */

#ifndef LIBCPU_POWERPC_MPC55XX_DSPI_H
#define LIBCPU_POWERPC_MPC55XX_DSPI_H

#include <rtems/libi2c.h>

#include <mpc55xx/edma.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct DSPI_tag;

typedef struct {
  edma_channel_context edma;
  rtems_id id;
} mpc55xx_dspi_edma_entry;

/**
 * @brief LibI2C bus driver entry.
 */
typedef struct {
	/**
	 * @brief Standard bus driver fields.
	 */
	rtems_libi2c_bus_t bus;

	/**
	 * @brief Index in the bus table: @ref mpc55xx_dspi_bus_table.
	 */
	unsigned table_index;

	/**
	 * @brief Bus number (available after rtems_libi2c_register_bus()).
	 *
	 * @note You must set it in the initialization code after the bus registration.
	 */
	unsigned bus_number;

	/**
	 * @brief Hardware registers.
	 */
	volatile struct DSPI_tag *regs;

	/**
	 * @brief Selects SPI master or slave mode.
	 */
	bool master;

	/**
	 * @brief Data for the Push Register.
	 */
	union DSPI_PUSHR_tag push_data;

	/**
	 * @brief eDMA entry for transmission.
	 *
	 * The channel is fixed to a particular DSPI.
	 */
	mpc55xx_dspi_edma_entry edma_transmit;

	/**
	 * @brief eDMA entry for push data generation.
	 *
	 * You can choose every available channel.
	 */
	mpc55xx_dspi_edma_entry edma_push;

	/**
	 * @brief eDMA entry for receiving.
	 *
	 * The channel is fixed to a particular DSPI.
	 */
	mpc55xx_dspi_edma_entry edma_receive;

	/**
	 * @brief Idle character transmitted in read only mode.
	 */
	uint32_t idle_char;

	/**
	 * @brief Current baud.
	 */
	uint32_t baud;
} mpc55xx_dspi_bus_entry;

/**
 * @brief Number of DSPIs.
 */
#define MPC55XX_DSPI_NUMBER 4

/**
 * @brief Table with bus driver entries.
 */
extern mpc55xx_dspi_bus_entry mpc55xx_dspi_bus_table [ /* MPC55XX_DSPI_NUMBER */ ];

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBCPU_POWERPC_MPC55XX_DSPI_H */
