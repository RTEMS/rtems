/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2013, 2017 embedded brains GmbH & Co. KG
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

#include <dev/serial/zynq-uart.h>
#include <dev/serial/zynq-uart-regs.h>
#include <bsp/irq.h>
#include <rtems/termiostypes.h>

#include <bspopts.h>

#ifdef ZYNQ_CONSOLE_USE_INTERRUPTS
static void zynq_uart_interrupt(void *arg)
{
  rtems_termios_tty *tty = arg;
  zynq_uart_context *ctx = rtems_termios_get_device_context(tty);
  volatile zynq_uart *regs = ctx->regs;

  if ((regs->irq_sts & ZYNQ_UART_RTRIG) != 0) {
    char buf[32];
    int c = 0;
    regs->irq_sts = ZYNQ_UART_RTRIG;
    while (c < sizeof(buf) &&
           (regs->channel_sts & ZYNQ_UART_CHANNEL_STS_REMPTY) == 0) {
       buf[c++] = (char) ZYNQ_UART_TX_RX_FIFO_FIFO_GET(regs->tx_rx_fifo);
    }
    rtems_termios_enqueue_raw_characters(tty, buf, c);
  }

  if (ctx->transmitting) {
    int sent = ctx->tx_queued;
    regs->irq_dis = ZYNQ_UART_TEMPTY;
    ctx->transmitting = false;
    ctx->tx_queued = 0;
    rtems_termios_dequeue_characters(tty, sent);
  }
}
#endif

static bool zynq_uart_first_open(
  rtems_termios_tty *tty,
  rtems_termios_device_context *base,
  struct termios *term,
  rtems_libio_open_close_args_t *args
)
{
  (void) term;
  (void) args;

  zynq_uart_context *ctx = (zynq_uart_context *) base;
  volatile zynq_uart *regs = ctx->regs;
#ifdef ZYNQ_CONSOLE_USE_INTERRUPTS
  rtems_status_code sc;
#endif

  rtems_termios_set_initial_baud(tty, ZYNQ_UART_DEFAULT_BAUD);
  zynq_uart_initialize(regs);

#ifdef ZYNQ_CONSOLE_USE_INTERRUPTS
  regs->rx_fifo_trg_lvl = 1;
  regs->irq_dis = 0xffffffff;
  regs->irq_sts = 0xffffffff;
  regs->irq_en = ZYNQ_UART_RTRIG;
  sc = rtems_interrupt_handler_install(
    ctx->irq,
    "UART",
    RTEMS_INTERRUPT_SHARED,
    zynq_uart_interrupt,
    tty
  );
  if (sc != RTEMS_SUCCESSFUL) {
    return false;
  }
#endif

  return true;
}

#ifdef ZYNQ_CONSOLE_USE_INTERRUPTS
static void zynq_uart_last_close(
  rtems_termios_tty *tty,
  rtems_termios_device_context *base,
  rtems_libio_open_close_args_t *args
)
{
  (void) args;

  zynq_uart_context *ctx = (zynq_uart_context *) base;

  rtems_interrupt_handler_remove(ctx->irq, zynq_uart_interrupt, tty);
}
#endif

#ifndef ZYNQ_CONSOLE_USE_INTERRUPTS
static int zynq_uart_read_polled(rtems_termios_device_context *base)
{
  zynq_uart_context *ctx = (zynq_uart_context *) base;
  return zynq_uart_read_char_polled(ctx->regs);
}
#endif

static void zynq_uart_write_support(
  rtems_termios_device_context *base,
  const char *buf,
  size_t len
)
{
  zynq_uart_context *ctx = (zynq_uart_context *) base;
  volatile zynq_uart *regs = ctx->regs;
#ifdef ZYNQ_CONSOLE_USE_INTERRUPTS

  regs->irq_dis = ZYNQ_UART_TEMPTY;

  if (len > 0) {
    const char *p = &buf[0];
    regs->irq_sts = ZYNQ_UART_TEMPTY;
    while (((regs->channel_sts & ZYNQ_UART_CHANNEL_STS_TNFUL) == 0) &&
           len > 0) {
      regs->tx_rx_fifo = ZYNQ_UART_TX_RX_FIFO_FIFO(*p);
      ++p;
      ++ctx->tx_queued;
      --len;
    }
    ctx->transmitting = true;
    regs->irq_en = ZYNQ_UART_TEMPTY;
  }
#else
  size_t i;
  for (i = 0; i < len; ++i) {
    zynq_uart_write_char_polled(regs, buf[i]);
  }
#endif
}

static bool zynq_uart_set_attributes(
  rtems_termios_device_context *context,
  const struct termios *term
)
{
  zynq_uart_context *ctx = (zynq_uart_context *) context;
  volatile zynq_uart *regs = ctx->regs;
  uint32_t desired_baud;
  uint32_t cd;
  uint32_t bdiv;
  uint32_t mode;

  /*
   * Determine the baud
   */
  desired_baud = rtems_termios_baud_to_number(term->c_ospeed);
  mode = regs->mode & ZYNQ_UART_MODE_CLKS;

  if (desired_baud > 0) {
    uint32_t error = zynq_uart_calculate_baud(desired_baud, mode, &cd, &bdiv);
    uint32_t margin;

    if ( desired_baud >= 100 ) {
      margin = 3 * (desired_baud / 100);
    } else {
      margin = 1;
    }

    if (error > margin) {
      return false;
    }
  }

  /*
   * Configure the mode register
   */
  mode |= ZYNQ_UART_MODE_CHMODE(ZYNQ_UART_MODE_CHMODE_NORMAL);

  /*
   * Parity
   */
  mode |= ZYNQ_UART_MODE_PAR(ZYNQ_UART_MODE_PAR_NONE);
  if (term->c_cflag & PARENB) {
    if (!(term->c_cflag & PARODD)) {
      mode |= ZYNQ_UART_MODE_PAR(ZYNQ_UART_MODE_PAR_ODD);
    } else {
      mode |= ZYNQ_UART_MODE_PAR(ZYNQ_UART_MODE_PAR_EVEN);
    }
  }

  /*
   * Character Size
   */
  switch (term->c_cflag & CSIZE)
  {
  case CS5:
    return false;
  case CS6:
    mode |= ZYNQ_UART_MODE_CHRL(ZYNQ_UART_MODE_CHRL_6);
    break;
  case CS7:
    mode |= ZYNQ_UART_MODE_CHRL(ZYNQ_UART_MODE_CHRL_7);
    break;
  case CS8:
  default:
    mode |= ZYNQ_UART_MODE_CHRL(ZYNQ_UART_MODE_CHRL_8);
    break;
  }

  /*
   * Stop Bits
   */
  if (term->c_cflag & CSTOPB) {
    /* 2 stop bits */
    mode |= ZYNQ_UART_MODE_NBSTOP(ZYNQ_UART_MODE_NBSTOP_STOP_2);
  } else {
    /* 1 stop bit */
    mode |= ZYNQ_UART_MODE_NBSTOP(ZYNQ_UART_MODE_NBSTOP_STOP_1);
  }

  /*
   * Wait for any data in the TXFIFO to be sent then wait while the
   * transmiter is active.
   */
  while ((regs->channel_sts & ZYNQ_UART_CHANNEL_STS_TEMPTY) == 0 ||
         (regs->channel_sts & ZYNQ_UART_CHANNEL_STS_TACTIVE) != 0) {
    /* Wait */
  }

  regs->control = ZYNQ_UART_CONTROL_RXDIS | ZYNQ_UART_CONTROL_TXDIS;
  /* Ignore baud rate of B0. There are no modem control lines to de-assert */
  if (desired_baud > 0) {
    regs->baud_rate_gen = ZYNQ_UART_BAUD_RATE_GEN_CD(cd);
    regs->baud_rate_div = ZYNQ_UART_BAUD_RATE_DIV_BDIV(bdiv);
  }
  regs->control = ZYNQ_UART_CONTROL_RXRES | ZYNQ_UART_CONTROL_TXRES;
  regs->mode = mode;
  regs->irq_sts = 0xffffffff;
  regs->control = ZYNQ_UART_CONTROL_RXEN | ZYNQ_UART_CONTROL_TXEN;

  return true;
}

const rtems_termios_device_handler zynq_uart_handler = {
  .first_open = zynq_uart_first_open,
  .set_attributes = zynq_uart_set_attributes,
  .write = zynq_uart_write_support,
#ifdef ZYNQ_CONSOLE_USE_INTERRUPTS
  .last_close = zynq_uart_last_close,
  .mode = TERMIOS_IRQ_DRIVEN
#else
  .poll_read = zynq_uart_read_polled,
  .mode = TERMIOS_POLLED
#endif
};
