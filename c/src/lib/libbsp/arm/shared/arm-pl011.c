/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <bsp/arm-pl011.h>
#include <bsp/arm-pl011-regs.h>

#include <libchip/sersupp.h>

static volatile pl011 *pl011_get_regs(int minor)
{
  const console_tbl *ct = Console_Port_Tbl != NULL ?
    Console_Port_Tbl[minor] : &Console_Configuration_Ports[minor];

  return (volatile pl011 *) ct->ulCtrlPort1;
}

static void pl011_initialize(int minor)
{
  volatile pl011 *regs = pl011_get_regs(minor);

  regs->uartlcr_h = PL011_UARTLCR_H_WLEN(PL011_UARTLCR_H_WLEN_8);
  regs->uartcr = PL011_UARTCR_RXE
    | PL011_UARTCR_TXE
    | PL011_UARTCR_UARTEN;
}

static int pl011_first_open(int major, int minor, void *arg)
{
  rtems_libio_open_close_args_t *oc = (rtems_libio_open_close_args_t *) arg;
  struct rtems_termios_tty *tty = (struct rtems_termios_tty *) oc->iop->data1;
  console_data *cd = &Console_Port_Data[minor];
  const console_tbl *ct = Console_Port_Tbl[minor];

  cd->termios_data = tty;
  rtems_termios_set_initial_baud(tty, (rtems_termios_baud_t) ct->pDeviceParams);

  return 0;
}

static int pl011_last_close(int major, int minor, void *arg)
{
  return 0;
}

static int pl011_read_polled(int minor)
{
  volatile pl011 *regs = pl011_get_regs(minor);

  if ((regs->uartfr & PL011_UARTFR_RXFE) != 0) {
    return -1;
  } else {
    return PL011_UARTDR_DATA(regs->uartdr);
  }
}

static void pl011_write_polled(int minor, char c)
{
  volatile pl011 *regs = pl011_get_regs(minor);

  while ((regs->uartfr & PL011_UARTFR_TXFF) != 0) {
    /* Wait */
  }

  regs->uartdr = PL011_UARTDR_DATA(c);
}

static ssize_t pl011_write_support_polled(
  int minor,
  const char *s,
  size_t n
)
{
  ssize_t i = 0;

  for (i = 0; i < n; ++i) {
    pl011_write_polled(minor, s[i]);
  }

  return n;
}

static int pl011_set_attribues(int minor, const struct termios *term)
{
  return -1;
}

const console_fns arm_pl011_fns = {
  .deviceProbe = libchip_serial_default_probe,
  .deviceFirstOpen = pl011_first_open,
  .deviceLastClose = pl011_last_close,
  .deviceRead = pl011_read_polled,
  .deviceWrite = pl011_write_support_polled,
  .deviceInitialize = pl011_initialize,
  .deviceWritePolled = pl011_write_polled,
  .deviceSetAttributes = pl011_set_attribues,
  .deviceOutputUsesInterrupts = false
};
