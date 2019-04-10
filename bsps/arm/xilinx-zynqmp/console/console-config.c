/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2013, 2017 embedded brains GmbH
 *
 * Copyright (C) 2019 DornerWorks
 *
 * Written by Jeff Kubascik <jeff.kubascik@dornerworks.com>
 *        and Josh Whitehead <josh.whitehead@dornerworks.com>
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
#include <rtems/sysinit.h>

#include <bsp/irq.h>
#include <bsp/zynq-uart.h>

#include <bspopts.h>

static zynq_uart_context zynqmp_uart_instances[2] = {
  {
    .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER( "Zynq UART 0" ),
    .regs = (volatile struct zynq_uart *) 0xff000000,
    .irq = ZYNQMP_IRQ_UART_0
  }, {
    .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER( "Zynq UART 1" ),
    .regs = (volatile struct zynq_uart *) 0xff010000,
    .irq = ZYNQMP_IRQ_UART_1
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

  for (i = 0; i < RTEMS_ARRAY_SIZE(zynqmp_uart_instances); ++i) {
    char uart[] = "/dev/ttySX";

    uart[sizeof(uart) - 2] = (char) ('0' + i);
    rtems_termios_device_install(
      &uart[0],
      &zynq_uart_handler,
      NULL,
      &zynqmp_uart_instances[i].base
    );

    if (i == BSP_CONSOLE_MINOR) {
      link(&uart[0], CONSOLE_DEVICE_NAME);
    }
  }

  return RTEMS_SUCCESSFUL;
}

void zynqmp_debug_console_flush(void)
{
  zynq_uart_reset_tx_flush(&zynqmp_uart_instances[BSP_CONSOLE_MINOR]);
}

static void zynqmp_debug_console_out(char c)
{
  rtems_termios_device_context *base =
    &zynqmp_uart_instances[BSP_CONSOLE_MINOR].base;

  zynq_uart_write_polled(base, c);
}

static void zynqmp_debug_console_init(void)
{
  rtems_termios_device_context *base =
    &zynqmp_uart_instances[BSP_CONSOLE_MINOR].base;

  zynq_uart_initialize(base);
  BSP_output_char = zynqmp_debug_console_out;
}

static void zynqmp_debug_console_early_init(char c)
{
  rtems_termios_device_context *base =
    &zynqmp_uart_instances[BSP_CONSOLE_MINOR].base;

  zynq_uart_initialize(base);
  zynqmp_debug_console_out(c);
}

static int zynqmp_debug_console_in(void)
{
  rtems_termios_device_context *base =
    &zynqmp_uart_instances[BSP_CONSOLE_MINOR].base;

  return zynq_uart_read_polled(base);
}

BSP_output_char_function_type BSP_output_char = zynqmp_debug_console_early_init;

BSP_polling_getchar_function_type BSP_poll_char = zynqmp_debug_console_in;

RTEMS_SYSINIT_ITEM(
  zynqmp_debug_console_init,
  RTEMS_SYSINIT_BSP_START,
  RTEMS_SYSINIT_ORDER_LAST
);
