/**
 * @file
 * @ingroup zynq_uart
 * @brief UART support.
 */

/*
 * Copyright (c) 2013, 2017 embedded brains GmbH.  All rights reserved.
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

#ifndef LIBBSP_ARM_XILINX_ZYNQ_UART_H
#define LIBBSP_ARM_XILINX_ZYNQ_UART_H

#include <rtems/termiostypes.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup zynq_uart UART Support
 * @ingroup arm_zynq
 * @brief UART Support
 */

typedef struct {
  rtems_termios_device_context base;
  volatile struct zynq_uart *regs;
  bool transmitting;
  rtems_vector_number irq;
} zynq_uart_context;

const rtems_termios_device_handler zynq_uart_handler;

extern zynq_uart_context zynq_uart_instances[2];

#define ZYNQ_UART_DEFAULT_BAUD 115200

void zynq_uart_initialize(rtems_termios_device_context *base);

int zynq_uart_read_polled(rtems_termios_device_context *base);

void zynq_uart_write_polled(
  rtems_termios_device_context *base,
  char c
);

/**
  * Flush TX FIFO and wait until it is empty. Used in bsp_reset.
  */
void zynq_uart_reset_tx_flush(zynq_uart_context *ctx);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_XILINX_ZYNQ_UART_H */
