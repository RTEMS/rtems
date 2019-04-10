/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2013, 2017 embedded brains GmbH
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
