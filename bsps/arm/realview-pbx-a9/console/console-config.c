/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2013, 2014 embedded brains GmbH & Co. KG
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
