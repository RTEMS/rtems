/*
 * Copyright © 2013 Eugeniy Meshcheryakov <eugen@debian.org>
 *
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
 * http://www.rtems.org/license/LICENSE.
 */

#include <bspopts.h>
#include <bsp/uart.h>
#include <libchip/sersupp.h>
#include <bsp/syscon.h>
#include <bsp/lm3s69xx.h>
#include <rtems/irq-extension.h>
#include <assert.h>

#define LM3S69XX_UART_FIFO_DEPTH 16

static volatile lm3s69xx_uart *get_uart_regs(int minor)
{
  console_tbl *ct = Console_Port_Tbl [minor];

  return (lm3s69xx_uart *) ct->ulCtrlPort1;
}

static unsigned int get_uart_number(int minor)
{
  console_tbl *ct = Console_Port_Tbl [minor];

  return (unsigned int)ct->pDeviceParams;
}

/*
 * Returns both integer and fractional parts as one number.
 */
static uint32_t get_baud_div(uint32_t baud)
{
  uint32_t clock4 = LM3S69XX_SYSTEM_CLOCK * 4;
  return (clock4 + baud - 1) / baud;
}

static void irq_handler(void *arg)
{
  int minor = (int)arg;
  console_data *cd = &Console_Port_Data [minor];
  volatile lm3s69xx_uart *uart = get_uart_regs(minor);

  do {
    char buf[LM3S69XX_UART_FIFO_DEPTH];
    int i = 0;
    uint32_t status = uart->fr;

    while (((status & UARTFR_RXFE) == 0) && (i < LM3S69XX_UART_FIFO_DEPTH)) {
      uint32_t d = uart->dr;

      if ((d & UARTDR_ERROR_MSK) == 0) {
        buf[i] = UARTDR_DATA_GET(d);
        i++;
      }

      status = uart->fr;
    }

    if (i > 0)
      rtems_termios_enqueue_raw_characters(cd->termios_data, buf, i);
  } while (uart->mis != 0);
}

static void initialize(int minor)
{
  const console_tbl *ct = Console_Port_Tbl[minor];
  volatile lm3s69xx_uart *uart = get_uart_regs(minor);
  unsigned int num = get_uart_number(minor);

  lm3s69xx_syscon_enable_uart_clock(num, true);

  uart->ctl = 0;

  uint32_t brd = get_baud_div(LM3S69XX_UART_BAUD);
  uart->ibrd = brd / 64;
  uart->fbrd = brd % 64;

  uart->lcrh = UARTLCRH_WLEN(0x3) | UARTLCRH_FEN;
  uart->ctl = UARTCTL_RXE | UARTCTL_TXE | UARTCTL_UARTEN;

  int rv = rtems_interrupt_handler_install(ct->ulIntVector, "UART",
      RTEMS_INTERRUPT_UNIQUE, irq_handler, (void *)minor);
  assert(rv == RTEMS_SUCCESSFUL);
}

static int first_open(int major, int minor, void *arg)
{
  rtems_libio_open_close_args_t *oc = (rtems_libio_open_close_args_t *) arg;
  struct rtems_termios_tty *tty = (struct rtems_termios_tty *) oc->iop->data1;
  console_data *cd = &Console_Port_Data [minor];
  volatile lm3s69xx_uart *uart = get_uart_regs(minor);

  cd->termios_data = tty;
  rtems_termios_set_initial_baud(tty, LM3S69XX_UART_BAUD);

  /* Drain the RX FIFO. */
  while ((uart->fr & UARTFR_RXFE) == 0)
    (void)uart->dr;

  uart->im = UARTI_RX | UARTI_RT;

  return 0;
}

static int last_close(int major, int minor, void *arg)
{
  volatile lm3s69xx_uart *uart = get_uart_regs(minor);
  uart->im = 0;

  return 0;
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

const console_fns lm3s69xx_uart_fns = {
  .deviceProbe = libchip_serial_default_probe,
  .deviceFirstOpen = first_open,
  .deviceLastClose = last_close,
  .deviceRead = NULL,
  .deviceWrite = write_support_polled,
  .deviceInitialize = initialize,
  .deviceWritePolled = write_polled,
  .deviceSetAttributes = set_attribues,
  .deviceOutputUsesInterrupts = false
};
