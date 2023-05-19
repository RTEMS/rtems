/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2016 embedded brains GmbH & Co. KG
 *
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

typedef struct {
	i2c_bus base;
	Twihs *regs;

	/* First message and number of messages that have to be processed. */
	i2c_msg *msgs;
	uint32_t msg_todo;

	/* Information about the current transfer. */
	bool stop_request;
	uint32_t current_msg_todo;
	uint8_t *current_msg_byte;

	/* Error information that can be returned to the task */
	uint32_t err;

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
