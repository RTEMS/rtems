/**
 * @file
 *
 * @ingroup mpc55xx_dspi
 *
 * @brief Header file for the LibI2C bus driver for the Deserial Serial Peripheral Interface (DSPI).
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

/** 
 * @defgroup mpc55xx_dspi Deserial Serial Peripheral Interface (DSPI)
 * 
 * @ingroup mpc55xx
 */

#ifndef LIBCPU_POWERPC_MPC55XX_DSPI_H
#define LIBCPU_POWERPC_MPC55XX_DSPI_H

#include <rtems/libi2c.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct DSPI_tag;

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
	int master;

	/**
	 * @brief Data for the Push Register.
	 */
	union DSPI_PUSHR_tag push_data;

	/**
	 * @brief eDMA channel for transmission.
	 *
	 * The channel is fixed to particular DSPI.
	 */
	int edma_channel_transmit;

	/**
	 * @brief eDMA channel to generate the push data.
	 *
	 * You can choose any available channel.
	 */
	int edma_channel_push;

	/**
	 * @brief eDMA channel for receiving.
	 *
	 * The channel is fixed to particular DSPI.
	 */
	int edma_channel_receive;

	/**
	 * @brief Semaphore ID for a transmit update.
	 */
	rtems_id edma_channel_transmit_update;

	/**
	 * @brief Semaphore ID for a receive update.
	 */
	rtems_id edma_channel_receive_update;

	/**
	 * @brief Transmit error status.
	 */
	uint32_t edma_channel_transmit_error;

	/**
	 * @brief Receive error status.
	 */
	uint32_t edma_channel_receive_error;

	/**
	 * @brief Idle character transmitted in read only mode.
	 */
	uint32_t idle_char;
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
