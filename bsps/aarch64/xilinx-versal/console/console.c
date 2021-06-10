/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsAArch64XilinxVersal
 *
 * @brief This source file contains this BSP's console configuration.
 */

/*
 * Copyright (C) 2021 Gedare Bloom <gedare@rtems.org>
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
#include <dev/serial/versal-uart.h>

#include <bspopts.h>

static versal_uart_context versal_uart_instances[2] = {
  {
    .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER( "Versal UART 0" ),
    .regs = (volatile struct versal_uart *) 0xff000000,
    .irq = VERSAL_IRQ_UART_0
  }, {
    .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER( "Versal UART 1" ),
    .regs = (volatile struct versal_uart *) 0xff010000,
    .irq = VERSAL_IRQ_UART_1
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

  for (i = 0; i < RTEMS_ARRAY_SIZE(versal_uart_instances); ++i) {
    char uart[] = "/dev/ttySX";

    uart[sizeof(uart) - 2] = (char) ('0' + i);
    rtems_termios_device_install(
      &uart[0],
      &versal_uart_handler,
      NULL,
      &versal_uart_instances[i].base
    );

    if (i == BSP_CONSOLE_MINOR) {
      link(&uart[0], CONSOLE_DEVICE_NAME);
    }
  }

  return RTEMS_SUCCESSFUL;
}

void versal_debug_console_flush(void)
{
  versal_uart_reset_tx_flush(&versal_uart_instances[BSP_CONSOLE_MINOR].base);
}

static void versal_debug_console_out(char c)
{
  rtems_termios_device_context *base =
    &versal_uart_instances[BSP_CONSOLE_MINOR].base;

  versal_uart_write_polled(base, c);
}

static void versal_debug_console_init(void)
{
  rtems_termios_device_context *base =
    &versal_uart_instances[BSP_CONSOLE_MINOR].base;

  (void) versal_uart_initialize(base);
  BSP_output_char = versal_debug_console_out;
}

static void versal_debug_console_early_init(char c)
{
  rtems_termios_device_context *base =
    &versal_uart_instances[BSP_CONSOLE_MINOR].base;

  (void) versal_uart_initialize(base);
  BSP_output_char = versal_debug_console_out;
  versal_debug_console_out(c);
}

static int versal_debug_console_in(void)
{
  rtems_termios_device_context *base =
    &versal_uart_instances[BSP_CONSOLE_MINOR].base;

  return versal_uart_read_polled(base);
}

BSP_output_char_function_type BSP_output_char = versal_debug_console_early_init;

BSP_polling_getchar_function_type BSP_poll_char = versal_debug_console_in;

RTEMS_SYSINIT_ITEM(
  versal_debug_console_init,
  RTEMS_SYSINIT_BSP_START,
  RTEMS_SYSINIT_ORDER_LAST_BUT_5
);
