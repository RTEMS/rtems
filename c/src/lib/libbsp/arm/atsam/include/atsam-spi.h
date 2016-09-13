/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_ATSAM_ATSAM_SPI_H
#define LIBBSP_ARM_ATSAM_ATSAM_SPI_H

#include <libchip/chip.h>
#include <libchip/include/pio.h>

#include <bsp.h>
#include <dev/spi/spi.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define DEFAULT_SPI0_CS 0
#define DEFAULT_SPI_CLOCK 500000

#define REGISTER_COMMAND_LENGTH 2

typedef struct {
	spi_bus base;
	Spi *regs;
	rtems_vector_number irq;
	uint32_t board_id;
	uint32_t msg_todo;
	const spi_ioc_transfer *msgs;
	rtems_id task_id;
	sXdmad Dma;
	Spid SpiDma;
	uint32_t dma_tx_channel;
	uint32_t dma_rx_channel;
	bool rx_transfer_done;
	bool tx_transfer_done;
} atsam_spi_bus;

int spi_bus_register_atsam(
	const char *bus_path,
	Spi *register_base,
	rtems_vector_number irq,
	size_t pin_amount,
	const Pin *pins
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_ATSAM_ATSAM_SPI_H */
