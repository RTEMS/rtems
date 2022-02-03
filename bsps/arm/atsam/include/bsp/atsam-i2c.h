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

#ifndef LIBBSP_ARM_ATSAM_ATSAM_I2C_H
#define LIBBSP_ARM_ATSAM_ATSAM_I2C_H

#include <libchip/chip.h>
#include <libchip/include/pio.h>

#include <bsp.h>
#include <rtems/thread.h>
#include <dev/i2c/i2c.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define TWI_AMOUNT_PINS 2

typedef enum {
	TX_SEND_DATA,
	TX_CONT_MESSAGE_NEEDED,
	RX_SEND_DATA,
	RX_CONT_MESSAGE_NEEDED,
	TX_RX_STOP_SENT
}transfer_state;

typedef struct {
	i2c_bus base;
	Twihs *regs;

	/* First message and number of messages that have to be processed. */
	i2c_msg *msgs;
	uint32_t msg_todo;

	/* Information about the current transfer. */
	bool stop_request;
	transfer_state trans_state;
	uint32_t current_msg_todo;
	uint8_t *current_msg_byte;

	uint32_t output_clock;
	rtems_binary_semaphore sem;
	rtems_vector_number irq;
} atsam_i2c_bus;

int i2c_bus_register_atsam(
    const char *bus_path,
    Twihs *register_base,
    rtems_vector_number irq,
    const Pin pins[TWI_AMOUNT_PINS]
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_ATSAM_ATSAM_I2C_H */
