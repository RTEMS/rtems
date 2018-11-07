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
#include <bsp/arm-pl050.h>
#include <bsp/console.h>
#include <bsp/console-termios.h>
#include <bsp/irq.h>

static arm_pl050_context pl050_context = {
  .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER("PL050"),
  .regs = (volatile pl050 *) 0x10007000,
  .irq = RVPBXA9_IRQ_KMI1,
  .initial_baud = 115200
};

const console_device console_device_table[] = {
  {
    .device_file = "/dev/ttyS0",
    .probe = rvpbx_pl011_probe,
    .handler = &arm_pl011_fns,
    .context = &rvpbx_pl011_context.base
  }, {
    .device_file = SERIAL_MOUSE_DEVICE_PS2,
    .probe = console_device_probe_default,
    .handler = &arm_pl050_fns,
    .context = &pl050_context.base
  }
};

const size_t console_device_count = RTEMS_ARRAY_SIZE(console_device_table);
