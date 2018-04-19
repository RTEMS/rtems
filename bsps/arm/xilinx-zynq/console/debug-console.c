/*
 * Copyright (c) 2013, 2017 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <rtems/bspIo.h>
#include <rtems/sysinit.h>

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
  RTEMS_SYSINIT_ORDER_LAST
);
