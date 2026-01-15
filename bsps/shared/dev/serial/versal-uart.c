/* SPDX-License-Identifier: BSD-2-Clause */

/*
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
#include <dev/serial/arm-pl011.h>
#include <bsp/irq.h>

#include <bspopts.h>

static uint32_t versal_uart_intr_all(void)
{
  return PL011_UARTI_OEI |
    PL011_UARTI_BEI |
    PL011_UARTI_PEI |
    PL011_UARTI_FEI |
    PL011_UARTI_RTI |
    PL011_UARTI_TXI |
    PL011_UARTI_RXI |
    PL011_UARTI_DSRMI |
    PL011_UARTI_DCDMI |
    PL011_UARTI_CTSMI |
    PL011_UARTI_RIMI;
}

#ifdef BSP_CONSOLE_USE_INTERRUPTS
static void versal_uart_intr_clear(volatile arm_pl011_uart *regs, uint32_t ints)
{
  regs->base.uarticr = ints;
}

static void versal_uart_intr_clearall(volatile arm_pl011_uart *regs)
{
  versal_uart_intr_clear(regs, versal_uart_intr_all());
}

static void versal_uart_intr_enable(volatile arm_pl011_uart *regs, uint32_t ints)
{
  regs->base.uartimsc |= ints;
}
#endif

static void versal_uart_intr_disable(volatile arm_pl011_uart *regs, uint32_t ints)
{
  regs->base.uartimsc &= ~ints;
}

static void versal_uart_intr_disableall(volatile arm_pl011_uart *regs)
{
  versal_uart_intr_disable(regs, versal_uart_intr_all());
}

#ifdef BSP_CONSOLE_USE_INTERRUPTS
static bool versal_uart_flags_clear(volatile arm_pl011_uart *regs, uint32_t flags)
{
  return (regs->base.uartfr & flags) == 0;
}

static void versal_uart_interrupt(void *arg)
{
  rtems_termios_tty *tty = arg;
  versal_pl011_context *ctx = rtems_termios_get_device_context(tty);
  volatile arm_pl011_uart *regs = (volatile arm_pl011_uart *) ctx->pl011_ctx.regs;
  uint32_t uartmis = regs->base.uartmis;

  versal_uart_intr_clear(regs, uartmis);

  if ((uartmis & (PL011_UARTI_RTI | PL011_UARTI_RXI)) != 0) {
    char buf[32];
    int c = 0;
    while (c < sizeof(buf) &&
           versal_uart_flags_clear(regs, PL011_UARTFR_RXFE)) {
      buf[c++] = (char) PL011_UARTDR_DATA_GET(regs->base.uartdr);
    }
    rtems_termios_enqueue_raw_characters(tty, buf, c);
  }

  if (ctx->transmitting) {
    int sent = ctx->pl011_ctx.tx_queued_chars;
    ctx->transmitting = false;
    ctx->pl011_ctx.tx_queued_chars = 0;
    versal_uart_intr_disable(regs, PL011_UARTI_TXI);
    rtems_termios_dequeue_characters(tty, sent);
  }
}
#endif

void versal_uart_reset_tx_flush(rtems_termios_device_context *base)
{
  volatile arm_pl011_uart *regs = (volatile arm_pl011_uart *) arm_pl011_get_regs(base);
  int c = 4;

  while (c-- > 0) {
    arm_pl011_write_polled(base, '\r');
  }

  while ((regs->base.uartfr & PL011_UARTFR_TXFE) == 0) {
    /* Wait for empty */
  }
  while ((regs->base.uartfr & PL011_UARTFR_BUSY) != 0) {
    /* Wait for empty */
  }
}

int versal_uart_initialize(rtems_termios_device_context *base)
{
  volatile pl011_base *regs = (volatile pl011_base *)arm_pl011_get_regs(base);
  arm_pl011_context *ctx = (arm_pl011_context *) base;
  uint32_t maxerr = 3;
  uint32_t ibauddiv = 0;
  uint32_t fbauddiv = 0;
  int rv;

  versal_uart_reset_tx_flush(base);

  rv = arm_pl011_compute_baudrate_params(
    &ibauddiv,
    &fbauddiv,
    VERSAL_UART_DEFAULT_BAUD,
    ctx->clock,
    maxerr
  );
  if (rv != 0) {
    return rv;
  }

  /* Line control: 8-bit word length, no parity, no FIFO, 1 stop bit */
  regs->uartlcr_h = PL011_UARTLCR_H_WLEN( PL011_UARTLCR_H_WLEN_8 )
    | PL011_UARTLCR_H_FEN;

  /* Control: receive, transmit, uart enable, no CTS, no RTS, no loopback */
  regs->uartcr = PL011_UARTCR_RXE
    | PL011_UARTCR_TXE
    | PL011_UARTCR_UARTEN;

  regs->uartibrd = ibauddiv;
  regs->uartfbrd = fbauddiv;

  return 0;
}

static bool versal_uart_first_open(
  rtems_termios_tty *tty,
  rtems_termios_device_context *base,
  struct termios *term,
  rtems_libio_open_close_args_t *args
)
{
  (void) term;
  (void) args;

#ifdef BSP_CONSOLE_USE_INTERRUPTS
  versal_pl011_context *ctx = (versal_pl011_context *) base;
  volatile arm_pl011_uart *regs = (volatile arm_pl011_uart *) ctx->pl011_ctx.regs;
  rtems_status_code sc;

  ctx->transmitting = false;
  ctx->pl011_ctx.tx_queued_chars = 0;
  ctx->pl011_ctx.needs_sw_triggered_tx_irq = true;
#endif

  rtems_termios_set_initial_baud(tty, VERSAL_UART_DEFAULT_BAUD);
  versal_uart_initialize(base);

#ifdef BSP_CONSOLE_USE_INTERRUPTS
  regs->base.uartifls = PL011_UARTIFLS_RXIFLSEL(2) | PL011_UARTIFLS_TXIFLSEL(2);
  regs->base.uartlcr_h |= PL011_UARTLCR_H_FEN;
  versal_uart_intr_disableall(regs);
  sc = rtems_interrupt_handler_install(
    ctx->pl011_ctx.irq,
    "UART",
    RTEMS_INTERRUPT_SHARED,
    versal_uart_interrupt,
    tty
  );
  if (sc != RTEMS_SUCCESSFUL) {
    return false;
  }
  versal_uart_intr_clearall(regs);
  versal_uart_intr_enable(regs, PL011_UARTI_RTI | PL011_UARTI_RXI);
#endif

  return true;
}

#ifdef BSP_CONSOLE_USE_INTERRUPTS
static void versal_uart_last_close(
  rtems_termios_tty *tty,
  rtems_termios_device_context *base,
  rtems_libio_open_close_args_t *args
)
{
  (void) args;

  versal_pl011_context *ctx = (versal_pl011_context *) base;
  rtems_interrupt_handler_remove(ctx->pl011_ctx.irq, versal_uart_interrupt, tty);
}
#endif

static void versal_uart_write_support(
  rtems_termios_device_context *base,
  const char *buf,
  size_t len
)
{
#ifdef BSP_CONSOLE_USE_INTERRUPTS
  versal_pl011_context *ctx = (versal_pl011_context *) base;
  volatile arm_pl011_uart *regs = (volatile arm_pl011_uart *) ctx->pl011_ctx.regs;

  if (len > 0) {
    size_t len_remaining = len;
    const char *p = &buf[0];
    versal_uart_intr_enable(regs, PL011_UARTI_TXI);
    /*
     * The PL011 IP in the Versal needs preloading the TX FIFO with
     * exactly 17 characters for the first TX interrupt to be
     * generated.
     */
    if (ctx->pl011_ctx.needs_sw_triggered_tx_irq) {
      ctx->pl011_ctx.needs_sw_triggered_tx_irq = false;
      for (int i = 0; i < 17; ++i) {
        regs->base.uartdr = PL011_UARTDR_DATA('\r');
      }
    }
    while (versal_uart_flags_clear(regs, PL011_UARTFR_TXFF) &&
           len_remaining > 0) {
      regs->base.uartdr = PL011_UARTDR_DATA(*p++);
      --len_remaining;
    }
    ctx->pl011_ctx.tx_queued_chars = len - len_remaining;
    ctx->transmitting = true;
  }
#else
  ssize_t i;
  for (i = 0; i < len; ++i) {
    arm_pl011_write_polled(base, buf[i]);
  }
#endif
}

static bool versal_uart_set_attributes(
  rtems_termios_device_context *context,
  const struct termios *term
)
{
  versal_pl011_context *ctx = (versal_pl011_context *) context;
  volatile arm_pl011_uart *regs = (volatile arm_pl011_uart *) ctx->pl011_ctx.regs;
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

    rc = arm_pl011_compute_baudrate_params(
      &ibauddiv,
      &fbauddiv,
      baud,
      ctx->pl011_ctx.clock,
      maxerr
    );
    if (rc != 0) {
      return rc;
    }
  }

  /*
   * Configure the mode register
   */
  mode = regs->base.uartlcr_h & PL011_UARTLCR_H_FEN;

  /*
   * Parity
   */
  if ((term->c_cflag & PARENB) != 0) {
    mode |= PL011_UARTLCR_H_PEN;
    if ((term->c_cflag & PARODD) == 0) {
      mode |= PL011_UARTLCR_H_EPS;
    }
  }

  /*
   * Character Size
   */
  switch (term->c_cflag & CSIZE)
  {
  case CS5:
    mode = PL011_UARTLCR_H_WLEN_SET(mode, PL011_UARTLCR_H_WLEN_5);
    break;
  case CS6:
    mode = PL011_UARTLCR_H_WLEN_SET(mode, PL011_UARTLCR_H_WLEN_6);
    break;
  case CS7:
    mode = PL011_UARTLCR_H_WLEN_SET(mode, PL011_UARTLCR_H_WLEN_7);
    break;
  case CS8:
  default:
    mode = PL011_UARTLCR_H_WLEN_SET(mode, PL011_UARTLCR_H_WLEN_8);
    break;
  }

  /*
   * Stop Bits
   */
  if (term->c_cflag & CSTOPB) {
    /* 2 stop bits */
    mode |= PL011_UARTLCR_H_STP2;
  }

  versal_uart_intr_disableall(regs);

  /*
   * Wait for any data in the TXFIFO to be sent then wait while the
   * transmiter is active.
   */
  while ((regs->base.uartfr & PL011_UARTFR_TXFE) == 0 ||
         (regs->base.uartfr & PL011_UARTFR_BUSY) != 0) {
    /* Wait */
  }

  regs->base.uartcr = PL011_UARTCR_UARTEN;
  /* Ignore baud rate of B0. There are no modem control lines to de-assert */
  if (baud > 0) {
    regs->base.uartibrd = ibauddiv;
    regs->base.uartfbrd = fbauddiv;
  }
  regs->base.uartlcr_h = mode;

  /* Control: receive, transmit, uart enable, no CTS, no RTS, no loopback */
  regs->base.uartcr = PL011_UARTCR_RXE
    | PL011_UARTCR_TXE
    | PL011_UARTCR_UARTEN;

#ifdef BSP_CONSOLE_USE_INTERRUPTS
  versal_uart_intr_clearall(regs);
  versal_uart_intr_enable(regs, PL011_UARTI_RTI | PL011_UARTI_RXI);
#endif

  return true;
}

const rtems_termios_device_handler versal_uart_handler = {
  .first_open = versal_uart_first_open,
  .set_attributes = versal_uart_set_attributes,
  .write = versal_uart_write_support,
#ifdef BSP_CONSOLE_USE_INTERRUPTS
  .last_close = versal_uart_last_close,
  .mode = TERMIOS_IRQ_DRIVEN
#else
  .poll_read = arm_pl011_read_polled,
  .mode = TERMIOS_POLLED
#endif
};
