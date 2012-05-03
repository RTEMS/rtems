/*
 * Copyright (c) 2011 Sebastian Huber.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <bspopts.h>
#include <bsp/uart.h>
#include <libchip/sersupp.h>

static volatile lm3s69xx_uart *get_uart_regs(int minor)
{
  console_tbl *ct = Console_Port_Tbl [minor];

  return (lm3s69xx_uart *) ct->ulCtrlPort1;
}

static void initialize(int minor)
{
  volatile lm3s69xx_uart *uart = get_uart_regs(minor);

  uart->ctl = 0;
  uart->lcrh = UARTLCRH_WLEN(0x3) | UARTLCRH_FEN;
  uart->ctl = UARTCTL_RXE | UARTCTL_TXE | UARTCTL_UARTEN;
}

static int first_open(int major, int minor, void *arg)
{
  rtems_libio_open_close_args_t *oc = (rtems_libio_open_close_args_t *) arg;
  struct rtems_termios_tty *tty = (struct rtems_termios_tty *) oc->iop->data1;
  console_data *cd = &Console_Port_Data [minor];

  cd->termios_data = tty;
  rtems_termios_set_initial_baud(tty, LM3S69XX_UART_BAUD);

  return 0;
}

static int last_close(int major, int minor, void *arg)
{
  return 0;
}

static int read_polled(int minor)
{
  volatile lm3s69xx_uart *uart = get_uart_regs(minor);

  if ((uart->fr & UARTFR_RXFE) != 0) {
    return -1;
  } else {
    return UARTDR_DATA(uart->dr);
  }
}

static void write_polled(int minor, char c)
{
  volatile lm3s69xx_uart *uart = get_uart_regs(minor);

  while ((uart->fr & UARTFR_TXFF) != 0) {
    /* Wait */
  }

  uart->dr = UARTDR_DATA(c);
}

static ssize_t write_support_polled(
  int minor,
  const char *s,
  size_t n
)
{
  ssize_t i = 0;

  for (i = 0; i < n; ++i) {
    write_polled(minor, s [i]);
  }

  return n;
}

static int set_attribues(int minor, const struct termios *term)
{
  return -1;
}

console_fns lm3s69xx_uart_fns = {
  .deviceProbe = libchip_serial_default_probe,
  .deviceFirstOpen = first_open,
  .deviceLastClose = last_close,
  .deviceRead = read_polled,
  .deviceWrite = write_support_polled,
  .deviceInitialize = initialize,
  .deviceWritePolled = write_polled,
  .deviceSetAttributes = set_attribues,
  .deviceOutputUsesInterrupts = false
};
