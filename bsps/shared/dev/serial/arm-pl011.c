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

#include <dev/serial/arm-pl011.h>

static volatile pl011 *pl011_get_regs(rtems_termios_device_context *base)
{
  arm_pl011_context *ctx = (arm_pl011_context *) base;

  return ctx->regs;
}


bool arm_pl011_probe(rtems_termios_device_context *base)
{
  volatile pl011 *regs = pl011_get_regs(base);

  regs->uartlcr_h = PL011_UARTLCR_H_WLEN(PL011_UARTLCR_H_WLEN_8);
  regs->uartcr = PL011_UARTCR_RXE
    | PL011_UARTCR_TXE
    | PL011_UARTCR_UARTEN;

  return true;
}

static bool pl011_first_open(
  struct rtems_termios_tty *tty,
  rtems_termios_device_context *base,
  struct termios *term,
  rtems_libio_open_close_args_t *args
)
{
  arm_pl011_context *ctx = (arm_pl011_context *) base;

  rtems_termios_set_initial_baud(tty, ctx->initial_baud);

  return true;
}

static int pl011_read_polled(rtems_termios_device_context *base)
{
  volatile pl011 *regs = pl011_get_regs(base);

  if ((regs->uartfr & PL011_UARTFR_RXFE) != 0) {
    return -1;
  } else {
    return PL011_UARTDR_DATA_GET(regs->uartdr);
  }
}

void arm_pl011_write_polled(rtems_termios_device_context *base, char c)
{
  volatile pl011 *regs = pl011_get_regs(base);

  while ((regs->uartfr & PL011_UARTFR_TXFF) != 0) {
    /* Wait */
  }

  regs->uartdr = PL011_UARTDR_DATA(c);
}

static void pl011_write_support_polled(
  rtems_termios_device_context *base,
  const char *s,
  size_t n
)
{
  size_t i;

  for (i = 0; i < n; ++i) {
    arm_pl011_write_polled(base, s[i]);
  }
}

const rtems_termios_device_handler arm_pl011_fns = {
  .first_open = pl011_first_open,
  .poll_read = pl011_read_polled,
  .write = pl011_write_support_polled,
  .mode = TERMIOS_POLLED
};
