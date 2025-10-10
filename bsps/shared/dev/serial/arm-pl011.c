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
#include <bspopts.h>

static inline int arm_pl011_read_char(volatile pl011_base *regs_base)
{
  return PL011_UARTDR_DATA_GET(regs_base->uartdr);
}

static inline void arm_pl011_write_char(
  volatile pl011_base *regs_base,
  const char ch
)
{
  regs_base->uartdr = PL011_UARTDR_DATA(ch);
}

static inline bool arm_pl011_is_rxfifo_empty(volatile pl011_base *regs_base)
{
  return (regs_base->uartfr & PL011_UARTFR_RXFE) != 0;
}

static inline bool arm_pl011_is_txfifo_full(volatile pl011_base *regs_base)
{
  return (regs_base->uartfr & PL011_UARTFR_TXFF) != 0;
}

volatile arm_pl011_uart *arm_pl011_get_regs(rtems_termios_device_context *base)
{
  return ((arm_pl011_context *)base)->regs;
}

bool arm_pl011_probe(rtems_termios_device_context *base)
{
  volatile arm_pl011_uart *regs = arm_pl011_get_regs(base);

  regs->base.uartlcr_h = PL011_UARTLCR_H_WLEN(PL011_UARTLCR_H_WLEN_8);
  regs->base.uartcr = PL011_UARTCR_RXE
    | PL011_UARTCR_TXE
    | PL011_UARTCR_UARTEN;

  return true;
}

static void arm_pl011_flush_fifos(const arm_pl011_context *context)
{
  volatile pl011_base *regs = (volatile pl011_base *) context->regs;

  regs->uartlcr_h &= ~PL011_UARTLCR_H_FEN;
  regs->uartlcr_h |= PL011_UARTLCR_H_FEN;
}

#ifdef BSP_CONSOLE_USE_INTERRUPTS
static inline void arm_pl011_clear_irq(
  volatile pl011_base *regs_base,
  const uint32_t irq_mask
)
{
  /* ICR is a write-only register */
  regs_base->uarticr = irq_mask;
}

static inline void arm_pl011_enable_irq(
  volatile pl011_base *regs_base,
  const uint32_t irq_mask
)
{
  regs_base->uartimsc |= irq_mask;
}
#endif

static inline void arm_pl011_disable_irq(
  volatile pl011_base *regs_base,
  uint32_t irq_mask
)
{
  regs_base->uartimsc &= ~irq_mask;
}

#ifdef BSP_CONSOLE_USE_INTERRUPTS
static void arm_pl011_irq_handler(void *arg)
{
  arm_pl011_context *context = (void *) arg;
  volatile arm_pl011_uart *regs =
    arm_pl011_get_regs((rtems_termios_device_context *) context);
  const uint32_t irqs = regs->base.uartmis;
  char buffer[ARM_PL011_FIFO_DEPTH];
  /* RXFIFO got data */
  uint32_t rx_irq_mask = PL011_UARTI_RTI | PL011_UARTI_RXI;
  if ((irqs & rx_irq_mask) != 0) {
    arm_pl011_clear_irq(&regs->base, rx_irq_mask);
    
    unsigned int i = 0;
    while (i < sizeof(buffer) && !arm_pl011_is_rxfifo_empty(&regs->base)) {
      buffer[i] = arm_pl011_read_char(&regs->base);
      i++;
    }

    (void) rtems_termios_enqueue_raw_characters(context->tty, buffer, i);
  }

  /*
   * Some characters got queued in the TXFIFO, so dequeue them from Termios'
   * structures.
   */
  if ((irqs & PL011_UARTI_TXI) != 0) {
    /*
     * First interrupt was raised, so no need to trigger the handler
     * through software anymore.
     */
    if (context->needs_sw_triggered_tx_irq == true) {
      context->needs_sw_triggered_tx_irq = false;
    }

    (void) rtems_termios_dequeue_characters(context->tty,
      context->tx_queued_chars);

    /*
     * Explicitly clear the transmit interrupt.  This is necessary
     * because there may not be enough bytes in the output buffer to
     * fill the FIFO greater than the transmit interrupt trigger level.
     * If FIFOs are disabled, this applies if there are 0 bytes to
     * transmit and therefore nothing to fill the Tx holding register
     * with.
     */
    arm_pl011_clear_irq(&regs->base, PL011_UARTI_TXI);
  }
}

static bool arm_pl011_first_open_irq(
  rtems_termios_device_context *base,
  arm_pl011_context *context
)
{
  rtems_status_code sc;
  volatile arm_pl011_uart *regs = arm_pl011_get_regs(base);
  /* Set FIFO trigger levels for interrupts */
  regs->base.uartifls =
    PL011_UARTIFLS_TXIFLSEL_SET(regs->base.uartifls, TXFIFO_IRQ_TRIGGER_LEVEL);
  regs->base.uartifls =
    PL011_UARTIFLS_RXIFLSEL_SET(regs->base.uartifls, RXFIFO_IRQ_TRIGGER_LEVEL);
  regs->base.uartlcr_h |= PL011_UARTLCR_H_FEN;
  arm_pl011_disable_irq(&regs->base, PL011_UARTI_MASK);
  sc = rtems_interrupt_handler_install(
    context->irq,
    "UART",
    RTEMS_INTERRUPT_SHARED,
    arm_pl011_irq_handler,
    context
  );
  if (sc != RTEMS_SUCCESSFUL) {
    return false;
  }

  /* Clear all interrupts  */
  arm_pl011_clear_irq(&regs->base, PL011_UARTI_MASK);
  arm_pl011_enable_irq(&regs->base, PL011_UARTI_RTI | PL011_UARTI_RXI);
  return RTEMS_SUCCESSFUL;
}
#endif

int arm_pl011_compute_baudrate_params(
  uint32_t *ibrd,
  uint32_t *fbrd,
  const uint32_t baudrate,
  const uint32_t clock,
  const unsigned short max_error
)
{
  uint16_t scalar;
  uint32_t computed_bauddiv, computed_baudrate, baud_error;
  uint64_t scaled_bauddiv;
  unsigned short round_off, percent_error;
  /*
   * The integer baudrate divisor, i.e. (clock / (baudrate * 16)), value
   * should lie on [1, 2^16 - 1]. To ensure this, clock and baudrate have to
   * be validated.
   */
  *ibrd = clock / 16 / baudrate;
  if (*ibrd < 1 || *ibrd > PL011_UARTIBRD_BAUD_DIVINT_MASK) {
    return 2;
  }

  /* Find the fractional part */
  scalar = 1 << (PL011_UARTFBRD_BAUD_DIVFRAC_WIDTH + 1);
  scaled_bauddiv =
    ((uint64_t) clock * scalar) / 16 / baudrate;
  round_off = scaled_bauddiv & 0x1;
  *fbrd =
    ((scaled_bauddiv >> 1) & PL011_UARTFBRD_BAUD_DIVFRAC_MASK) + round_off;

  /* Calculate the baudrate and check if the error is too large */
  computed_bauddiv =
    (*ibrd << PL011_UARTFBRD_BAUD_DIVFRAC_WIDTH) | *fbrd;
  computed_baudrate =
    ((uint64_t) clock << PL011_UARTFBRD_BAUD_DIVFRAC_WIDTH)
    / 16 / computed_bauddiv;

  baud_error = computed_baudrate - baudrate;
  if (baudrate > computed_baudrate) {
    baud_error = baudrate - computed_baudrate;
  }

  percent_error = (baud_error * 100) / baudrate;
  if (percent_error >= max_error) {
    return 1;
  }

  return 0;
}

static uint32_t arm_pl011_set_lcrh(const struct termios *term)
{
  /* enable FIFO */ 
  uint32_t lcrh = PL011_UARTLCR_H_FEN;

  /* Mode: parity */
  if ((term->c_cflag & PARENB) != 0) {
    lcrh |= PL011_UARTLCR_H_PEN;
    if ((term->c_cflag & PARODD) == 0) {
      lcrh |= PL011_UARTLCR_H_EPS;
    }
  }

  /* Mode: character size */
  switch (term->c_cflag & CSIZE) {
    case CS5:
      lcrh |= PL011_UARTLCR_H_WLEN_SET(lcrh, PL011_UARTLCR_H_WLEN_5);
      break;
    case CS6:
      lcrh |= PL011_UARTLCR_H_WLEN_SET(lcrh, PL011_UARTLCR_H_WLEN_6);
      break;
    case CS7:
      lcrh |= PL011_UARTLCR_H_WLEN_SET(lcrh, PL011_UARTLCR_H_WLEN_7);
      break;
    case CS8:
    default:
      lcrh |= PL011_UARTLCR_H_WLEN_SET(lcrh, PL011_UARTLCR_H_WLEN_8);
  }

  /* Mode: stop bits */
  if ((term->c_cflag & CSTOPB) != 0) {
    /* 2 stop bits */
    lcrh |= PL011_UARTLCR_H_STP2;
  }
  return lcrh;
}

static uint32_t arm_pl011_set_cr(const struct termios *term, volatile pl011_base *regs)
{
  uint32_t cr = regs->uartcr;
  /*
   * Control: Configure flow control
   * NOTE: Flow control is untested
   */
  cr &= ~(PL011_UARTCR_CTSEN | PL011_UARTCR_RTSEN);
  if ((term->c_cflag & CCTS_OFLOW) != 0) {
    cr |= PL011_UARTCR_CTSEN;
  }
  if ((term->c_cflag & CRTS_IFLOW) != 0) {
    cr |= PL011_UARTCR_RTSEN;
  }

  /* Control: Configure receiver */
  if ((term->c_cflag & CREAD) != 0) {
    cr |= PL011_UARTCR_RXE;
  }

  /* Control: Re-enable UART */
  cr |= PL011_UARTCR_UARTEN | PL011_UARTCR_TXE;

  return cr;
}

static bool arm_pl011_set_attributes(
  rtems_termios_device_context *base,
  const struct termios *term
)
{
  uint32_t ibrd, fbrd, lcrh, baud, cr;
  int err;
  arm_pl011_context *context = (arm_pl011_context *) base;
  volatile arm_pl011_uart *regs = arm_pl011_get_regs(base);

  /* Determine baudrate parameters */
  baud = rtems_termios_number_to_baud(term->c_ospeed);
  if (baud == B0) {
    return false;
  }

  err = arm_pl011_compute_baudrate_params(&ibrd,&fbrd,baud,context->clock,3);
  if (err != 0) {
    return false;
  }

  lcrh = arm_pl011_set_lcrh(term);

  /* Disable all interrupts */
  arm_pl011_disable_irq(&regs->base, PL011_UARTI_MASK);

  /*
   * Wait for any data in the TXFIFO to be sent then wait while the
   * transmiter is active.
   */
  while (
    (regs->base.uartfr & PL011_UARTFR_TXFE) == 0 ||
    (regs->base.uartfr & PL011_UARTFR_BUSY) != 0
  ) {
      /* Wait */
  }

  regs->base.uartcr = PL011_UARTCR_UARTEN;

  /* Set the baudrate */
  regs->base.uartibrd = ibrd;
  regs->base.uartfbrd = fbrd;

  /*
   * Commit mode configurations
   * NOTE:
   * This has to happen after IBRD and FBRD as writing to LCRH is
   * required to trigger the baudrate update.
   */
  regs->base.uartlcr_h = lcrh;

  /* Control: Disable UART */
  regs->base.uartcr &=
    ~(PL011_UARTCR_UARTEN | PL011_UARTCR_RXE | PL011_UARTCR_TXE);
  cr = arm_pl011_set_cr(term, &regs->base);
  arm_pl011_flush_fifos(context);
  /* Commit changes to control register */
  regs->base.uartcr = cr;

#ifdef BSP_CONSOLE_USE_INTERRUPTS
  /* Clear all interrupts  */
  arm_pl011_clear_irq(&regs->base, PL011_UARTI_MASK);

  /* Re-enable RX interrupts */
  if ((term->c_cflag & CREAD) != 0) {
    arm_pl011_enable_irq(&regs->base, PL011_UARTI_RTI | PL011_UARTI_RXI);
  }

  /*
   * UART is freshly enabled, TXFIFO is empty, and interrupt will be enabled,
   * so the next transmission will required software-trigger interrupt.
   */
  context->needs_sw_triggered_tx_irq = true;
#endif
  return true;
}

static bool arm_pl011_first_open(
  struct rtems_termios_tty *tty,
  rtems_termios_device_context *base,
  struct termios *term,
  rtems_libio_open_close_args_t *args
)
{
  (void) args;

  arm_pl011_context *context = (arm_pl011_context *) base;
#ifdef BSP_CONSOLE_USE_INTERRUPTS
  rtems_status_code sc;

  context->tty = tty;
#endif

  if (rtems_termios_set_initial_baud(tty, context->initial_baud) != 0) {
    return false;
  }

  if (!arm_pl011_set_attributes(base, term)) {
    return false;
  }
  
#ifdef BSP_CONSOLE_USE_INTERRUPTS
  sc = arm_pl011_first_open_irq(base, context);
  if (sc != RTEMS_SUCCESSFUL) {
    return false;
  }
#endif
  
  return true;
}

#ifdef BSP_CONSOLE_USE_INTERRUPTS
static void arm_pl011_last_close(
  rtems_termios_tty *tty,
  rtems_termios_device_context *base,
  rtems_libio_open_close_args_t *args
)
{
  (void) args;

  const arm_pl011_context *context = (void *) base;
  (void) rtems_interrupt_handler_remove(
    context->irq,
    arm_pl011_irq_handler,
    tty
  );
}
#endif

int arm_pl011_read_polled(rtems_termios_device_context *base)
{
  volatile arm_pl011_uart *regs = arm_pl011_get_regs(base);

  if (arm_pl011_is_rxfifo_empty(&regs->base)) {
    return -1;
  } else {
    return arm_pl011_read_char(&regs->base);
  }
}

void arm_pl011_write_polled(rtems_termios_device_context *base, char c)
{
  volatile arm_pl011_uart *regs = arm_pl011_get_regs(base);

  /* Wait until TXFIFO has space */
  while (arm_pl011_is_txfifo_full(&regs->base)) {
    /* Wait */
  }

  arm_pl011_write_char(&regs->base, c);
}

#ifdef BSP_CONSOLE_USE_INTERRUPTS
static void arm_pl011_write_support_interrupt(
  rtems_termios_device_context *base,
  const char *buffer,
  size_t buffer_len
)
{
  arm_pl011_context *context = (arm_pl011_context *) base;
  volatile arm_pl011_uart *regs = arm_pl011_get_regs(base);
  size_t i = 0;

  if (buffer_len > 0) {
    arm_pl011_enable_irq(&regs->base, PL011_UARTI_TXI);
    /*
     * When arm_pl011_write_support_interrupt writes the first character,
     * if txfifo is full, it will return directly. This will cause this
     * character to be lost. If txfifo is full, wait for it to be not full.
     */
    while (arm_pl011_is_txfifo_full(&regs->base)) {
      /* wait for txfifo not full */
    }
    while (!arm_pl011_is_txfifo_full(&regs->base) && i < buffer_len) {
      arm_pl011_write_char(&regs->base, buffer[i]);
      i++;
    }
    context->tx_queued_chars = i;

    if (context->needs_sw_triggered_tx_irq) {
      /*
       * The first write may not trigger the TX interrupt due to insufficient
       * characters being written. Call rtems_termios_dequeue_characters() to
       * continue writing.
       */
      (void) rtems_termios_dequeue_characters(context->tty,
                                              context->tx_queued_chars);
    }
  } else {
    /*
     * Termios will set n to zero to indicate that the transmitter is now
     * inactive. The output buffer is empty in this case. The driver may
     * disable the transmit interrupts now.
     */
    arm_pl011_disable_irq(&regs->base, PL011_UARTI_TXI);
  }
}
#else
static void arm_pl011_write_support_polled(
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
#endif

static void arm_pl011_write_buffer(
  rtems_termios_device_context *base,
  const char *buffer,
  size_t n
)
{
#ifdef BSP_CONSOLE_USE_INTERRUPTS
  arm_pl011_write_support_interrupt(base, buffer, n);
#else
  arm_pl011_write_support_polled(base, buffer, n);
#endif
}

const rtems_termios_device_handler arm_pl011_fns = {
  .first_open = arm_pl011_first_open,
  .write = arm_pl011_write_buffer,
  .set_attributes = arm_pl011_set_attributes,
  .ioctl = NULL,
#ifdef BSP_CONSOLE_USE_INTERRUPTS
  .last_close = arm_pl011_last_close,
  .poll_read = NULL,
  .mode = TERMIOS_IRQ_DRIVEN,
#else
  .last_close = NULL,
  .poll_read = arm_pl011_read_polled,
  .mode = TERMIOS_POLLED,
#endif
};
