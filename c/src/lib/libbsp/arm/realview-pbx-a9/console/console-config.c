/*
 * Copyright (c) 2013-2014 embedded brains GmbH.  All rights reserved.
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

#include <rtems/serial_mouse.h>
#include <rtems/bspIo.h>

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/arm-pl011.h>
#include <bsp/arm-pl050.h>
#include <bsp/console-termios.h>

static arm_pl011_context pl011_context = {
  .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER("PL011"),
  .regs = (volatile pl011 *) 0x10009000,
  .irq = RVPBXA9_IRQ_UART_0,
  .initial_baud = 115200
};

static arm_pl050_context pl050_context = {
  .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER("PL050"),
  .regs = (volatile pl050 *) 0x10007000,
  .irq = RVPBXA9_IRQ_KMI1,
  .initial_baud = 115200
};

static void output_char(char c)
{
  if (c == '\n') {
    arm_pl011_write_polled(&pl011_context.base, '\r');
  }

  arm_pl011_write_polled(&pl011_context.base, c);
}

static bool pl011_probe(rtems_termios_device_context *base)
{
  BSP_output_char = output_char;

  return arm_pl011_probe(base);
}

static void output_char_init(char c)
{
  pl011_probe(&pl011_context.base);
  output_char(c);
}

BSP_output_char_function_type BSP_output_char = output_char_init;

BSP_polling_getchar_function_type BSP_poll_char = NULL;

const console_device console_device_table[] = {
  {
    .device_file = "/dev/ttyS0",
    .probe = pl011_probe,
    .handler = &arm_pl011_fns,
    .context = &pl011_context.base
  }, {
    .device_file = SERIAL_MOUSE_DEVICE_PS2,
    .probe = console_device_probe_default,
    .handler = &arm_pl050_fns,
    .context = &pl050_context.base
  }
};

const size_t console_device_count = RTEMS_ARRAY_SIZE(console_device_table);
