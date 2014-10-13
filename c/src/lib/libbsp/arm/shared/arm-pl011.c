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

#include <bsp/arm-pl011.h>

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
