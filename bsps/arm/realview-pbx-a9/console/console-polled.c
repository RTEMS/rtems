/*
 * Copyright (c) 2013, 2018 embedded brains GmbH.  All rights reserved.
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

#include <bsp/console.h>
#include <bsp.h>
#include <bsp/irq.h>

#include <rtems/bspIo.h>

arm_pl011_context rvpbx_pl011_context = {
  .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER("PL011"),
  .regs = (volatile pl011 *) 0x10009000,
  .irq = RVPBXA9_IRQ_UART_0,
  .initial_baud = 115200
};

static void output_char(char c)
{
  arm_pl011_write_polled(&rvpbx_pl011_context.base, c);
}

bool rvpbx_pl011_probe(rtems_termios_device_context *base)
{
  BSP_output_char = output_char;

  return arm_pl011_probe(base);
}

static void output_char_init(char c)
{
  rvpbx_pl011_probe(&rvpbx_pl011_context.base);
  output_char(c);
}

BSP_output_char_function_type BSP_output_char = output_char_init;

BSP_polling_getchar_function_type BSP_poll_char = NULL;
