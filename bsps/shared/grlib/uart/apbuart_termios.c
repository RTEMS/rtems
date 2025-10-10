/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modified for LEON3 BSP.
 *  COPYRIGHT (c) 2004.
 *  Gaisler Research.
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

#include <grlib/apbuart_termios.h>
#include <grlib/apbuart.h>
#include <grlib/io.h>
#include <bsp.h>

static void apbuart_isr(void *arg)
{
  rtems_termios_tty *tty = arg;
  struct apbuart_context *uart = rtems_termios_get_device_context(tty);
  unsigned int status;
  char data;

  /* Get all received characters */
  while ((status=grlib_load_32(&uart->regs->status)) & APBUART_STATUS_DR) {
    /* Data has arrived, get new data */
    data = (char)grlib_load_32(&uart->regs->data);

    /* Tell termios layer about new character */
    rtems_termios_enqueue_raw_characters(tty, &data, 1);
  }

  if (
    (status & APBUART_STATUS_TE)
      && (grlib_load_32(&uart->regs->ctrl) & APBUART_CTRL_TI) != 0
  ) {
    /* write_interrupt will get called from this function */
    rtems_termios_dequeue_characters(tty, 1);
  }
}

static void apbuart_write_support(
  rtems_termios_device_context *base,
  const char *buf,
  size_t len
)
{
  struct apbuart_context *uart = (struct apbuart_context *) base;
  int sending;
  uint32_t ctrl;

  ctrl = grlib_load_32(&uart->regs->ctrl);

  if (len > 0) {
    /* Enable TX interrupt (interrupt is edge-triggered) */
    ctrl |= APBUART_CTRL_TI;

    /* start UART TX, this will result in an interrupt when done */
    grlib_store_32(&uart->regs->data, (uint8_t)*buf);

    sending = 1;
  } else {
    /* No more to send, disable TX interrupts */
    ctrl &= ~APBUART_CTRL_TI;

    /* Tell close that we sent everything */
    sending = 0;
  }

  grlib_store_32(&uart->regs->ctrl, ctrl);
  uart->sending = sending;
}

static void apbuart_write_polled(
  rtems_termios_device_context *base,
  const char *buf,
  size_t len
)
{
  struct apbuart_context *uart = (struct apbuart_context *) base;
  size_t nwrite = 0;

  while (nwrite < len) {
    apbuart_outbyte_polled(uart->regs, *buf++);
    nwrite++;
  }
}

static int apbuart_poll_read(rtems_termios_device_context *base)
{
  struct apbuart_context *uart = (struct apbuart_context *) base;

  return apbuart_inbyte_nonblocking(uart->regs);
}

static bool apbuart_set_attributes(
  rtems_termios_device_context *base,
  const struct termios *t
)
{
  struct apbuart_context *uart = (struct apbuart_context *) base;
  rtems_interrupt_lock_context lock_context;
  unsigned int scaler;
  unsigned int ctrl;
  int baud;

  switch (t->c_cflag & CSIZE) {
    default:
    case CS5:
    case CS6:
    case CS7:
      /* Hardware doesn't support other than CS8 */
      return false;
    case CS8:
      break;
  }

  rtems_termios_device_lock_acquire(base, &lock_context);

  /* Read out current value */
  ctrl = grlib_load_32(&uart->regs->ctrl);

  switch (t->c_cflag & (PARENB|PARODD)) {
    case (PARENB|PARODD):
      /* Odd parity */
      ctrl |= APBUART_CTRL_PE|APBUART_CTRL_PS;
      break;

    case PARENB:
      /* Even parity */
      ctrl &= ~APBUART_CTRL_PS;
      ctrl |= APBUART_CTRL_PE;
      break;

    default:
    case 0:
    case PARODD:
      /* No Parity */
      ctrl &= ~(APBUART_CTRL_PS|APBUART_CTRL_PE);
  }

  if (!(t->c_cflag & CLOCAL)) {
    ctrl |= APBUART_CTRL_FL;
  } else {
    ctrl &= ~APBUART_CTRL_FL;
  }

  /* Update new settings */
  grlib_store_32(&uart->regs->ctrl, ctrl);

  rtems_termios_device_lock_release(base, &lock_context);

  /* Baud rate */
  baud = rtems_termios_baud_to_number(t->c_ospeed);
  if (baud > 0) {
    /* Calculate Baud rate generator "scaler" number */
    scaler = (((uart->freq_hz * 10) / (baud * 8)) - 5) / 10;

    /* Set new baud rate by setting scaler */
    grlib_store_32(&uart->regs->scaler, scaler);
  }

  return true;
}

static void apbuart_set_best_baud(
  const struct apbuart_context *uart,
  struct termios *term
)
{
  uint32_t baud = (uart->freq_hz * 10) /
    ((grlib_load_32(&uart->regs->scaler) * 10 + 5) * 8);

  rtems_termios_set_best_baud(term, baud);
}

static bool apbuart_first_open_polled(
  rtems_termios_tty *tty,
  rtems_termios_device_context *base,
  struct termios *term,
  rtems_libio_open_close_args_t *args
)
{
  (void) tty;
  (void) args;

  struct apbuart_context *uart = (struct apbuart_context *) base;
  uint32_t ctrl;

  apbuart_set_best_baud(uart, term);

  /* Initialize UART on opening */
  ctrl = grlib_load_32(&uart->regs->ctrl);
  ctrl |= APBUART_CTRL_RE | APBUART_CTRL_TE;
  grlib_store_32(&uart->regs->ctrl, ctrl);
  grlib_store_32(&uart->regs->status, 0);

  return true;
}

static bool apbuart_first_open_interrupt(
  rtems_termios_tty *tty,
  rtems_termios_device_context *base,
  struct termios *term,
  rtems_libio_open_close_args_t *args
)
{
  (void) args;

  struct apbuart_context *uart = (struct apbuart_context *) base;
  rtems_status_code sc;
  uint32_t ctrl;

  apbuart_set_best_baud(uart, term);

  /* Register Interrupt handler */
  sc = rtems_interrupt_handler_install(uart->irq, "console",
                                       RTEMS_INTERRUPT_SHARED,
                                       apbuart_isr,
                                       tty);
  if (sc != RTEMS_SUCCESSFUL)
    return false;

  uart->sending = 0;
  /* Enable Receiver and transmitter and Turn on RX interrupts */
  ctrl = grlib_load_32(&uart->regs->ctrl);
  ctrl |= APBUART_CTRL_RE | APBUART_CTRL_TE | APBUART_CTRL_RI;
  grlib_store_32(&uart->regs->ctrl, ctrl);
  /* Initialize UART on opening */
  ctrl |= APBUART_CTRL_RE | APBUART_CTRL_TE;
  grlib_store_32(&uart->regs->ctrl, ctrl);
  grlib_store_32(&uart->regs->status, 0);

  return true;
}

static void apbuart_last_close_interrupt(
  rtems_termios_tty *tty,
  rtems_termios_device_context *base,
  rtems_libio_open_close_args_t *args
)
{
  (void) args;

  struct apbuart_context *uart = (struct apbuart_context *) base;
  rtems_interrupt_lock_context lock_context;
  uint32_t ctrl;

  /* Turn off RX interrupts */
  rtems_termios_device_lock_acquire(base, &lock_context);
  ctrl = grlib_load_32(&uart->regs->ctrl);
  ctrl &= ~APBUART_CTRL_RI;
  grlib_store_32(&uart->regs->ctrl, ctrl);
  rtems_termios_device_lock_release(base, &lock_context);

  /**** Flush device ****/
  while (uart->sending) {
    /* Wait until all data has been sent */
  }

  /* uninstall ISR */
  rtems_interrupt_handler_remove(uart->irq, apbuart_isr, tty);
}

const rtems_termios_device_handler apbuart_handler_interrupt = {
  .first_open = apbuart_first_open_interrupt,
  .last_close = apbuart_last_close_interrupt,
  .write = apbuart_write_support,
  .set_attributes = apbuart_set_attributes,
  .mode = TERMIOS_IRQ_DRIVEN
};

const rtems_termios_device_handler apbuart_handler_polled = {
  .first_open = apbuart_first_open_polled,
  .poll_read = apbuart_poll_read,
  .write = apbuart_write_polled,
  .set_attributes = apbuart_set_attributes,
  .mode = TERMIOS_POLLED
};
