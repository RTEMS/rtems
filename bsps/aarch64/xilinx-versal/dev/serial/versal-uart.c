/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2022 Chris Johns <chris@contemporary.software>
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

#include <dev/serial/versal-uart.h>
#include <dev/serial/versal-uart-regs.h>
#include <bsp/irq.h>

#include <bspopts.h>

static uint32_t versal_uart_intr_all(void)
{
  return VERSAL_UARTI_OEI |
    VERSAL_UARTI_BEI |
    VERSAL_UARTI_PEI |
    VERSAL_UARTI_FEI |
    VERSAL_UARTI_RTI |
    VERSAL_UARTI_TXI |
    VERSAL_UARTI_RXI |
    VERSAL_UARTI_DSRMI |
    VERSAL_UARTI_DCDMI |
    VERSAL_UARTI_CTSMI |
    VERSAL_UARTI_RIMI;
}

#ifdef VERSAL_CONSOLE_USE_INTERRUPTS
static void versal_uart_intr_clear(volatile versal_uart *regs, uint32_t ints)
{
  regs->uarticr = ints;
}

static void versal_uart_intr_clearall(volatile versal_uart *regs)
{
  versal_uart_intr_clear(regs, versal_uart_intr_all());
}

static void versal_uart_intr_enable(volatile versal_uart *regs, uint32_t ints)
{
  regs->uartimsc |= ints;
}
#endif

static void versal_uart_intr_disable(volatile versal_uart *regs, uint32_t ints)
{
  regs->uartimsc &= ~ints;
}

static void versal_uart_intr_disableall(volatile versal_uart *regs)
{
  versal_uart_intr_disable(regs, versal_uart_intr_all());
}

#ifdef VERSAL_CONSOLE_USE_INTERRUPTS
static bool versal_uart_flags_clear(volatile versal_uart *regs, uint32_t flags)
{
  return (regs->uartfr & flags) == 0;
}

static void versal_uart_interrupt(void *arg)
{
  rtems_termios_tty *tty = arg;
  versal_uart_context *ctx = rtems_termios_get_device_context(tty);
  volatile versal_uart *regs = ctx->regs;
  uint32_t uartmis = regs->uartmis;

  versal_uart_intr_clear(regs, uartmis);

  if ((uartmis & (VERSAL_UARTI_RTI | VERSAL_UARTI_RXI)) != 0) {
    char buf[32];
    int c = 0;
    while (c < sizeof(buf) &&
           versal_uart_flags_clear(regs, VERSAL_UARTFR_RXFE)) {
      buf[c++] = (char) VERSAL_UARTDR_DATA_GET(regs->uartdr);
    }
    rtems_termios_enqueue_raw_characters(tty, buf, c);
  }

  if (ctx->transmitting) {
    int sent = ctx->tx_queued;
    ctx->transmitting = false;
    ctx->tx_queued = 0;
    versal_uart_intr_disable(regs, VERSAL_UARTI_TXI);
    rtems_termios_dequeue_characters(tty, sent);
  }
}
#endif

static bool versal_uart_first_open(
  rtems_termios_tty *tty,
  rtems_termios_device_context *base,
  struct termios *term,
  rtems_libio_open_close_args_t *args
)
{
#ifdef VERSAL_CONSOLE_USE_INTERRUPTS
  versal_uart_context *ctx = (versal_uart_context *) base;
  volatile versal_uart *regs = ctx->regs;
  rtems_status_code sc;

  ctx->transmitting = false;
  ctx->tx_queued = 0;
  ctx->first_send = true;
#endif

  rtems_termios_set_initial_baud(tty, VERSAL_UART_DEFAULT_BAUD);
  versal_uart_initialize(base);

#ifdef VERSAL_CONSOLE_USE_INTERRUPTS
  regs->uartifls = VERSAL_UARTIFLS_RXIFLSEL(2) | VERSAL_UARTIFLS_TXIFLSEL(2);
  regs->uartlcr_h |= VERSAL_UARTLCR_H_FEN;
  versal_uart_intr_disableall(regs);
  sc = rtems_interrupt_handler_install(
    ctx->irq,
    "UART",
    RTEMS_INTERRUPT_SHARED,
    versal_uart_interrupt,
    tty
  );
  if (sc != RTEMS_SUCCESSFUL) {
    return false;
  }
  versal_uart_intr_clearall(regs);
  versal_uart_intr_enable(regs, VERSAL_UARTI_RTI | VERSAL_UARTI_RXI);
#endif

  return true;
}

#ifdef VERSAL_CONSOLE_USE_INTERRUPTS
static void versal_uart_last_close(
  rtems_termios_tty *tty,
  rtems_termios_device_context *base,
  rtems_libio_open_close_args_t *args
)
{
  versal_uart_context *ctx = (versal_uart_context *) base;
  rtems_interrupt_handler_remove(ctx->irq, versal_uart_interrupt, tty);
}
#endif

static void versal_uart_write_support(
  rtems_termios_device_context *base,
  const char *buf,
  size_t len
)
{
#ifdef VERSAL_CONSOLE_USE_INTERRUPTS
  versal_uart_context *ctx = (versal_uart_context *) base;
  volatile versal_uart *regs = ctx->regs;

  if (len > 0) {
    size_t len_remaining = len;
    const char *p = &buf[0];
    versal_uart_intr_enable(regs, VERSAL_UARTI_TXI);
    /*
     * The PL011 IP in the Versal needs preloading the TX FIFO with
     * exactly 17 characters for the first TX interrupt to be
     * generated.
     */
    if (ctx->first_send) {
      ctx->first_send = false;
      for (int i = 0; i < 17; ++i) {
        regs->uartdr = VERSAL_UARTDR_DATA('\r');
      }
    }
    while (versal_uart_flags_clear(regs, VERSAL_UARTFR_TXFF) &&
           len_remaining > 0) {
      regs->uartdr = VERSAL_UARTDR_DATA(*p++);
      --len_remaining;
    }
    ctx->tx_queued = len - len_remaining;
    ctx->transmitting = true;
  }
#else
  ssize_t i;
  for (i = 0; i < len; ++i) {
    versal_uart_write_polled(base, buf[i]);
  }
#endif
}

static bool versal_uart_set_attributes(
  rtems_termios_device_context *context,
  const struct termios *term
)
{
  versal_uart_context *ctx = (versal_uart_context *) context;
  volatile versal_uart *regs = ctx->regs;
  int32_t baud;
  uint32_t ibauddiv = 0;
  uint32_t fbauddiv = 0;
  uint32_t mode = 0;
  int rc;

  /*
   * Determine the baud rate
   */
  baud = rtems_termios_baud_to_number(term->c_ospeed);

  if (baud > 0) {
    uint32_t maxerr = 3;

    rc = versal_cal_baud_rate(
        VERSAL_UART_DEFAULT_BAUD,
        maxerr,
        &ibauddiv,
        &fbauddiv
    );
    if (rc != 0) {
      return rc;
    }
  }

  /*
   * Configure the mode register
   */
  mode = regs->uartlcr_h & VERSAL_UARTLCR_H_FEN;

  /*
   * Parity
   */
  if ((term->c_cflag & PARENB) != 0) {
    mode |= VERSAL_UARTLCR_H_PEN;
    if ((term->c_cflag & PARODD) == 0) {
      mode |= VERSAL_UARTLCR_H_EPS;
    }
  }

  /*
   * Character Size
   */
  switch (term->c_cflag & CSIZE)
  {
  case CS5:
    mode = VERSAL_UARTLCR_H_WLEN_SET(mode, VERSAL_UARTLCR_H_WLEN_5);
    break;
  case CS6:
    mode = VERSAL_UARTLCR_H_WLEN_SET(mode, VERSAL_UARTLCR_H_WLEN_6);
    break;
  case CS7:
    mode = VERSAL_UARTLCR_H_WLEN_SET(mode, VERSAL_UARTLCR_H_WLEN_7);
    break;
  case CS8:
  default:
    mode = VERSAL_UARTLCR_H_WLEN_SET(mode, VERSAL_UARTLCR_H_WLEN_8);
    break;
  }

  /*
   * Stop Bits
   */
  if (term->c_cflag & CSTOPB) {
    /* 2 stop bits */
    mode |= VERSAL_UARTLCR_H_STP2;
  }

  versal_uart_intr_disableall(regs);

  /*
   * Wait for any data in the TXFIFO to be sent then wait while the
   * transmiter is active.
   */
  while ((regs->uartfr & VERSAL_UARTFR_TXFE) == 0 ||
         (regs->uartfr & VERSAL_UARTFR_BUSY) != 0) {
    /* Wait */
  }

  regs->uartcr = VERSAL_UARTCR_UARTEN;
  /* Ignore baud rate of B0. There are no modem control lines to de-assert */
  if (baud > 0) {
    regs->uartibrd = VERSAL_UARTIBRD_BAUD_DIVINT(ibauddiv);
    regs->uartfbrd = VERSAL_UARTFBRD_BAUD_DIVFRAC(fbauddiv);
  }
  regs->uartlcr_h = mode;

  /* Control: receive, transmit, uart enable, no CTS, no RTS, no loopback */
  regs->uartcr = VERSAL_UARTCR_RXE
    | VERSAL_UARTCR_TXE
    | VERSAL_UARTCR_UARTEN;

#ifdef VERSAL_CONSOLE_USE_INTERRUPTS
  versal_uart_intr_clearall(regs);
  versal_uart_intr_enable(regs, VERSAL_UARTI_RTI | VERSAL_UARTI_RXI);
#endif

  return true;
}

const rtems_termios_device_handler versal_uart_handler = {
  .first_open = versal_uart_first_open,
  .set_attributes = versal_uart_set_attributes,
  .write = versal_uart_write_support,
#ifdef VERSAL_CONSOLE_USE_INTERRUPTS
  .last_close = versal_uart_last_close,
  .mode = TERMIOS_IRQ_DRIVEN
#else
  .poll_read = versal_uart_read_polled,
  .mode = TERMIOS_POLLED
#endif
};
