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

#include <rtems/bspIo.h>
#include <rtems/sysinit.h>

#include <bsp.h>
#include <bsp/zynq-uart.h>

#include <bspopts.h>

static void zynq_debug_console_out(char c)
{
  rtems_termios_device_context *base =
    &zynq_uart_instances[BSP_CONSOLE_MINOR].base;

  zynq_uart_write_polled(base, c);
}

static void zynq_debug_console_init(void)
{
  rtems_termios_device_context *base =
    &zynq_uart_instances[BSP_CONSOLE_MINOR].base;

  zynq_uart_initialize(base);
  BSP_output_char = zynq_debug_console_out;
}

static void zynq_debug_console_early_init(char c)
{
  rtems_termios_device_context *base =
    &zynq_uart_instances[BSP_CONSOLE_MINOR].base;

  zynq_uart_initialize(base);
  zynq_debug_console_out(c);
}

static int zynq_debug_console_in(void)
{
  rtems_termios_device_context *base =
    &zynq_uart_instances[BSP_CONSOLE_MINOR].base;

  return zynq_uart_read_polled(base);
}

BSP_output_char_function_type BSP_output_char = zynq_debug_console_early_init;

BSP_polling_getchar_function_type BSP_poll_char = zynq_debug_console_in;

RTEMS_SYSINIT_ITEM(
  zynq_debug_console_init,
  RTEMS_SYSINIT_BSP_START,
  RTEMS_SYSINIT_ORDER_LAST_BUT_5
);
