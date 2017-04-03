/*
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modified for LEON3 BSP.
 *  COPYRIGHT (c) 2004.
 *  Gaisler Research.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp/apbuart_termios.h>
#include <bsp/apbuart.h>
#include <bsp.h>

static void apbuart_isr(void *arg)
{
  rtems_termios_tty *tty = arg;
  struct apbuart_context *uart = rtems_termios_get_device_context(tty);
  unsigned int status;
  char data;

  /* Get all received characters */
  while ((status=uart->regs->status) & APBUART_STATUS_DR) {
    /* Data has arrived, get new data */
    data = uart->regs->data;

    /* Tell termios layer about new character */
    rtems_termios_enqueue_raw_characters(tty, &data, 1);
  }

  if (
    (status & APBUART_STATUS_TE)
      && (uart->regs->ctrl & APBUART_CTRL_TI) != 0
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

  if (len > 0) {
    /* Enable TX interrupt (interrupt is edge-triggered) */
    uart->regs->ctrl |= APBUART_CTRL_TI;

    /* start UART TX, this will result in an interrupt when done */
    uart->regs->data = *buf;

    sending = 1;
  } else {
    /* No more to send, disable TX interrupts */
    uart->regs->ctrl &= ~APBUART_CTRL_TI;

    /* Tell close that we sent everything */
    sending = 0;
  }

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
    apbuart_outbyte_polled(uart->regs, *buf++, 0, 0);
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
  ctrl = uart->regs->ctrl;

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
  uart->regs->ctrl = ctrl;

  rtems_termios_device_lock_release(base, &lock_context);

  /* Baud rate */
  baud = rtems_termios_baud_to_number(t->c_ospeed);
  if (baud > 0) {
    /* Calculate Baud rate generator "scaler" number */
    scaler = (((uart->freq_hz * 10) / (baud * 8)) - 5) / 10;

    /* Set new baud rate by setting scaler */
    uart->regs->scaler = scaler;
  }

  return true;
}

static void apbuart_set_best_baud(
  const struct apbuart_context *uart,
  struct termios *term
)
{
  uint32_t baud = (uart->freq_hz * 10) / ((uart->regs->scaler * 10 + 5) * 8);

  rtems_termios_set_best_baud(term, baud);
}

static bool apbuart_first_open_polled(
  rtems_termios_tty *tty,
  rtems_termios_device_context *base,
  struct termios *term,
  rtems_libio_open_close_args_t *args
)
{
  struct apbuart_context *uart = (struct apbuart_context *) base;

  apbuart_set_best_baud(uart, term);

  /* Initialize UART on opening */
  uart->regs->ctrl |= APBUART_CTRL_RE | APBUART_CTRL_TE;
  uart->regs->status = 0;

  return true;
}

static bool apbuart_first_open_interrupt(
  rtems_termios_tty *tty,
  rtems_termios_device_context *base,
  struct termios *term,
  rtems_libio_open_close_args_t *args
)
{
  struct apbuart_context *uart = (struct apbuart_context *) base;
  rtems_status_code sc;

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
  uart->regs->ctrl |= APBUART_CTRL_RE | APBUART_CTRL_TE |
                      APBUART_CTRL_RI;
  /* Initialize UART on opening */
  uart->regs->ctrl |= APBUART_CTRL_RE | APBUART_CTRL_TE;
  uart->regs->status = 0;

  return true;
}

static void apbuart_last_close_interrupt(
  rtems_termios_tty *tty,
  rtems_termios_device_context *base,
  rtems_libio_open_close_args_t *args
)
{
  struct apbuart_context *uart = (struct apbuart_context *) base;
  rtems_interrupt_lock_context lock_context;

  /* Turn off RX interrupts */
  rtems_termios_device_lock_acquire(base, &lock_context);
  uart->regs->ctrl &= ~(APBUART_CTRL_RI);
  rtems_termios_device_lock_release(base, &lock_context);

  /**** Flush device ****/
  while (uart->sending) {
    /* Wait until all data has been sent */
  }

  /* uninstall ISR */
  rtems_interrupt_handler_remove(uart->irq, apbuart_isr, tty);
}

/*
 *  apbuart_outbyte_polled
 *
 *  This routine transmits a character using polling.
 */
void apbuart_outbyte_polled(
  struct apbuart_regs *regs,
  unsigned char ch,
  int do_cr_on_newline,
  int wait_sent
)
{
send:
  while ( (regs->status & APBUART_STATUS_TE) == 0 ) {
    /* Lower bus utilization while waiting for UART */
    __asm__ volatile ("nop"::); __asm__ volatile ("nop"::);
    __asm__ volatile ("nop"::); __asm__ volatile ("nop"::);
    __asm__ volatile ("nop"::); __asm__ volatile ("nop"::);
    __asm__ volatile ("nop"::); __asm__ volatile ("nop"::);
  }

  if ((ch == '\n') && do_cr_on_newline) {
    regs->data = (unsigned int) '\r';
    do_cr_on_newline = 0;
    goto send;
  }
  regs->data = (unsigned int) ch;

  /* Wait until the character has been sent? */
  if (wait_sent) {
    while ((regs->status & APBUART_STATUS_TE) == 0)
      ;
  }
}

/*
 *  apbuart_inbyte_nonblocking
 *
 *  This routine polls for a character.
 */
int apbuart_inbyte_nonblocking(struct apbuart_regs *regs)
{
  /* Clear errors */
  if (regs->status & APBUART_STATUS_ERR)
    regs->status = ~APBUART_STATUS_ERR;

  if ((regs->status & APBUART_STATUS_DR) == 0)
    return -1;
  else
    return (int) regs->data;
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
