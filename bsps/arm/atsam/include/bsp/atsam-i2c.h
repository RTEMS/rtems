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
	TX_SEND_STOP,
	TX_CONT_MESSAGE_NEEDED,
	RX_SEND_DATA,
	RX_SEND_STOP,
	RX_CONT_MESSAGE_NEEDED,
	TX_RX_STOP_SENT
}transfer_state;

typedef struct {
	uint8_t status;
	uint8_t *data;
	bool stop_request;
	uint32_t data_size;
	uint32_t already_transferred;
	transfer_state trans_state;
} transfer_desc;

typedef struct {
	i2c_bus base;
	i2c_msg *msgs;
	Twihs *regs;
	transfer_desc trans_desc;
	uint32_t msg_todo;
	uint32_t current_msg_todo;
	uint8_t *current_msg_byte;
	uint32_t output_clock;
	bool read;
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
