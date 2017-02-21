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

#include <rtems/console.h>
#include <rtems/bspIo.h>

#include <bsp/irq.h>
#include <bsp/zynq-uart.h>

#include <bspopts.h>

zynq_uart_context zynq_uart_instances[2] = {
  {
    .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER( "Zynq UART 0" ),
    .regs = (volatile struct zynq_uart *) 0xe0000000,
    .irq = ZYNQ_IRQ_UART_0
  }, {
    .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER( "Zynq UART 1" ),
    .regs = (volatile struct zynq_uart *) 0xe0001000,
    .irq = ZYNQ_IRQ_UART_1
  }
};

rtems_status_code console_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  size_t i;

  rtems_termios_initialize();

  for (i = 0; i < RTEMS_ARRAY_SIZE(zynq_uart_instances); ++i) {
    char uart[] = "/dev/ttySX";

    uart[sizeof(uart) - 2] = (char) ('0' + i);
    rtems_termios_device_install(
      &uart[0],
      &zynq_uart_handler,
      NULL,
      &zynq_uart_instances[i].base
    );

    if (i == BSP_CONSOLE_MINOR) {
      link(&uart[0], CONSOLE_DEVICE_NAME);
    }
  }

  return RTEMS_SUCCESSFUL;
}
