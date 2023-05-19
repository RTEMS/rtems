/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2013, 2018 embedded brains GmbH & Co. KG
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
