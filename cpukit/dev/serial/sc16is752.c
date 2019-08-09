/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */


#include <dev/serial/sc16is752.h>

#include <sys/param.h>

#include <assert.h>
#include <stdio.h>
#include <fcntl.h>

#include <rtems/seterr.h>

#include "sc16is752-regs.h"

static void write_reg(
  sc16is752_context *ctx,
  uint8_t addr,
  const uint8_t *data,
  size_t len
)
{
  (*ctx->write_reg)(ctx, addr, data, len);
}

static void read_reg(
  sc16is752_context *ctx,
  uint8_t addr,
  uint8_t *data,
  size_t len
)
{
  (*ctx->read_reg)(ctx, addr, data, len);
}

static void read_2_reg(
  sc16is752_context *ctx,
  uint8_t addr_0,
  uint8_t addr_1,
  uint8_t data[2]
)
{
  (*ctx->read_2_reg)(ctx, addr_0, addr_1, data);
}

static bool is_sleep_mode_enabled(sc16is752_context *ctx)
{
  return (ctx->ier & SC16IS752_IER_SLEEP_MODE) != 0;
}

static void set_sleep_mode(sc16is752_context *ctx, bool enable)
{
  if (enable) {
    ctx->ier |= SC16IS752_IER_SLEEP_MODE;
  } else {
    ctx->ier &= ~SC16IS752_IER_SLEEP_MODE;
  }

  write_reg(ctx, SC16IS752_IER, &ctx->ier, 1);
}

static void set_mcr_dll_dlh(
  sc16is752_context *ctx,
  uint8_t mcr,
  uint32_t divisor
)
{
  bool sleep_mode = is_sleep_mode_enabled(ctx);
  uint8_t dll = (uint8_t)divisor;
  uint8_t dlh = (uint8_t)(divisor >> 8);

  if (sleep_mode) {
    set_sleep_mode(ctx, false);
  }

  ctx->lcr |= SC16IS752_LCR_ENABLE_DIVISOR;
  write_reg(ctx, SC16IS752_LCR, &ctx->lcr, 1);

  write_reg(ctx, SC16IS752_MCR, &mcr, 1);
  write_reg(ctx, SC16IS752_DLH, &dlh, 1);
  write_reg(ctx, SC16IS752_DLL, &dll, 1);

  ctx->lcr &= ~SC16IS752_LCR_ENABLE_DIVISOR;
  write_reg(ctx, SC16IS752_LCR, &ctx->lcr, 1);

  if (sleep_mode) {
    set_sleep_mode(ctx, true);
  }
}

static void set_efr(sc16is752_context *ctx, uint8_t efr)
{
  uint8_t lcr = ctx->lcr;

  ctx->lcr = 0xbf;
  write_reg(ctx, SC16IS752_LCR, &ctx->lcr, 1);

  write_reg(ctx, SC16IS752_EFR, &efr, 1);

  ctx->lcr = lcr;
  write_reg(ctx, SC16IS752_LCR, &ctx->lcr, 1);
}

static void set_tlr(sc16is752_context *ctx, uint8_t tlr)
{
  uint8_t mcr;

  read_reg(ctx, SC16IS752_MCR, &mcr, 1);
  mcr |= SC16IS752_MCR_TCR_TLR;
  write_reg(ctx, SC16IS752_MCR, &mcr, 1);
  write_reg(ctx, SC16IS752_TLR, &tlr, 1);
  mcr &= ~SC16IS752_MCR_TCR_TLR;
  write_reg(ctx, SC16IS752_MCR, &mcr, 1);
}

static bool set_baud(sc16is752_context *ctx, rtems_termios_baud_t baud)
{
  uint32_t freq = ctx->input_frequency;
  uint8_t mcr;
  uint32_t divisor;

  read_reg(ctx, SC16IS752_MCR, &mcr, 1);

  divisor = freq / baud / 16;
  if (divisor > 0xFFFF){
    divisor = (freq / (4 * baud)) / 16;
    if (divisor > 0xFFFF){
      return false;
    } else {
      mcr |= SC16IS752_MCR_PRESCALE_NEEDED;
    }
  } else {
    mcr &= ~SC16IS752_MCR_PRESCALE_NEEDED;
  }

  set_mcr_dll_dlh(ctx, mcr, divisor);
  return true;
}

static bool sc16is752_set_attributes(
  rtems_termios_device_context *base,
  const struct termios *term
)
{
  sc16is752_context *ctx = (sc16is752_context *)base;
  rtems_termios_baud_t baud;

  ctx->lcr = 0;

  baud = rtems_termios_baud_to_number(term->c_ospeed);

  if (baud > 0) {
    if (!set_baud(ctx, baud)){
      return false;
    }

    ctx->efcr &= ~SC16IS752_EFCR_TX_DISABLE;

    if ((term->c_cflag & CREAD) == 0){
      ctx->efcr |= SC16IS752_EFCR_RX_DISABLE;
    } else {
      ctx->efcr &= ~SC16IS752_EFCR_RX_DISABLE;
    }
  } else {
    ctx->efcr |= SC16IS752_EFCR_RX_DISABLE | SC16IS752_EFCR_TX_DISABLE;
  }

  write_reg(ctx, SC16IS752_EFCR, &ctx->efcr, 1);

  switch (term->c_cflag & CSIZE) {
    case CS5:
      ctx->lcr |= SC16IS752_LCR_CHRL_5_BIT;
      break;
    case CS6:
      ctx->lcr |= SC16IS752_LCR_CHRL_6_BIT;
      break;
    case CS7:
      ctx->lcr |= SC16IS752_LCR_CHRL_7_BIT;
      break;
    case CS8:
      ctx->lcr |= SC16IS752_LCR_CHRL_8_BIT;
      break;
  }

  if ((term->c_cflag & PARENB) != 0){
    ctx->lcr |= SC16IS752_LCR_SET_PARITY;
    if ((term->c_cflag & PARODD) != 0) {
      ctx->lcr &= ~SC16IS752_LCR_EVEN_PARITY;
    } else {
      ctx->lcr |= SC16IS752_LCR_EVEN_PARITY;
    }
  } else {
    ctx->lcr &= ~SC16IS752_LCR_SET_PARITY;
  }

  if ((term->c_cflag & CSTOPB) != 0) {
    ctx->lcr |= SC16IS752_LCR_2_STOP_BIT;
  } else {
    ctx->lcr &= ~SC16IS752_LCR_2_STOP_BIT;
  }

  write_reg(ctx, SC16IS752_LCR, &ctx->lcr, 1);
  return true;
}

static bool sc16is752_first_open(
  rtems_termios_tty *tty,
  rtems_termios_device_context *base,
  struct termios *term,
  rtems_libio_open_close_args_t *args
)
{
  bool ok;
  uint8_t fcr;
  uint8_t efcr;

  (void)args;
  sc16is752_context *ctx = (sc16is752_context *)base;

  ctx->tty = tty;

  ok = (*ctx->first_open)(ctx);
  if (!ok) {
    return ok;
  }

  set_efr(ctx, SC16IS752_EFR_ENHANCED_FUNC_ENABLE);

  efcr = 0;

  switch (ctx->mode) {
    case SC16IS752_MODE_RS485_RTS_INV:
      efcr |= SC16IS752_EFCR_RTSINVER;
      /* Fall through */
    case SC16IS752_MODE_RS485_RTS:
      efcr |= SC16IS752_EFCR_RTSCON;
      /* Fall through */
    case SC16IS752_MODE_RS485:
      efcr |= SC16IS752_EFCR_RS485_ENABLE;
      break;
    default:
      break;
  }

  ctx->efcr = efcr;
  write_reg(ctx, SC16IS752_EFCR, &ctx->efcr, 1);

  fcr = SC16IS752_FCR_FIFO_EN
    | SC16IS752_FCR_RX_FIFO_RST
    | SC16IS752_FCR_TX_FIFO_RST
    | SC16IS752_FCR_RX_FIFO_TRG_8
    | SC16IS752_FCR_TX_FIFO_TRG_32;
  write_reg(ctx, SC16IS752_FCR, &fcr, 1);

  fcr = SC16IS752_FCR_FIFO_EN
    | SC16IS752_FCR_RX_FIFO_TRG_8
    | SC16IS752_FCR_TX_FIFO_TRG_32;
  write_reg(ctx, SC16IS752_FCR, &fcr, 1);

  set_tlr(ctx, 0);

  ctx->ier = SC16IS752_IER_RHR;
  write_reg(ctx, SC16IS752_IER, &ctx->ier, 1);

  rtems_termios_set_initial_baud(tty, 115200);
  ok = sc16is752_set_attributes(base, term);
  if (!ok) {
    return ok;
  }

  ok = (*ctx->install_irq)(ctx);
  return ok;
}

static void sc16is752_last_close(
  rtems_termios_tty *tty,
  rtems_termios_device_context *base,
  rtems_libio_open_close_args_t *args
)
{
  sc16is752_context *ctx = (sc16is752_context *)base;

  (void)tty;
  (void)args;
  (*ctx->last_close)(ctx);
}

static void sc16is752_write(
  rtems_termios_device_context *base,
  const char *buf,
  size_t len
)
{
  sc16is752_context *ctx = (sc16is752_context *)base;

  if (len > 0) {
    ctx->ier |= SC16IS752_IER_THR;
    len = MIN(len, 32);
    ctx->tx_in_progress = (uint8_t)len;
    write_reg(ctx, SC16IS752_THR, (const uint8_t *)&buf[0], len);
    write_reg(ctx, SC16IS752_IER, &ctx->ier, 1);
  } else {
    ctx->tx_in_progress = 0;
    ctx->ier &= ~SC16IS752_IER_THR;
    write_reg(ctx, SC16IS752_IER, &ctx->ier, 1);
  }
}

static void sc16is752_get_modem_bits(sc16is752_context *ctx, int *bits)
{
  *bits = 0;
  uint8_t msr;
  uint8_t mcr;

  read_reg(ctx, SC16IS752_MSR, &msr, 1);
  read_reg(ctx, SC16IS752_MCR, &mcr, 1);

  if (msr & SC16IS752_MSR_CTS) {
    *bits |= TIOCM_CTS;
  }
  if (msr & SC16IS752_MSR_DSR) {
    *bits |= TIOCM_DSR;
  }
  if (msr & SC16IS752_MSR_RI) {
    *bits |= TIOCM_RI;
  }
  if (msr & SC16IS752_MSR_CD) {
    *bits |= TIOCM_CD;
  }
  if ((mcr & SC16IS752_MCR_DTR) == 0) {
    *bits |= TIOCM_DTR;
  }
  if ((mcr & SC16IS752_MCR_RTS) == 0) {
    *bits |= TIOCM_RTS;
  }
}

static void sc16is752_set_modem_bits(
  sc16is752_context *ctx, int *bits, int set, int clear
)
{
  uint8_t mcr;

  read_reg(ctx, SC16IS752_MCR, &mcr, 1);

  if (bits != NULL) {
    if ((*bits & TIOCM_DTR) == 0) {
      mcr |= SC16IS752_MCR_DTR;
    } else {
      mcr &= ~SC16IS752_MCR_DTR;
    }

    if ((*bits & TIOCM_RTS) == 0) {
      mcr |= SC16IS752_MCR_RTS;
    } else {
      mcr &= ~SC16IS752_MCR_RTS;
    }
  }

  if ((set & TIOCM_DTR) != 0) {
    mcr &= ~SC16IS752_MCR_DTR;
  }
  if ((set & TIOCM_RTS) != 0) {
    mcr &= ~SC16IS752_MCR_RTS;
  }
  if ((clear & TIOCM_DTR) != 0) {
    mcr |= SC16IS752_MCR_DTR;
  }
  if ((clear & TIOCM_RTS) != 0) {
    mcr |= SC16IS752_MCR_RTS;
  }

  write_reg(ctx, SC16IS752_MCR, &mcr, 1);
}

static int sc16is752_ioctl(
  rtems_termios_device_context *base,
  ioctl_command_t               request,
  void                         *buffer
)
{
  sc16is752_context *ctx = (sc16is752_context *)base;
  uint8_t regval;

  switch (request) {
    case SC16IS752_SET_SLEEP_MODE:
      set_sleep_mode(ctx, *(int *)buffer != 0);
      break;
    case SC16IS752_GET_SLEEP_MODE:
      *(int *)buffer = is_sleep_mode_enabled(ctx);
      break;
    case SC16IS752_SET_IOCONTROL:
      regval = (*(uint8_t *)buffer) & ~SC16IS752_IOCONTROL_SRESET;
      write_reg(ctx, SC16IS752_IOCONTROL, &regval, 1);
      break;
    case SC16IS752_GET_IOCONTROL:
      read_reg(ctx, SC16IS752_IOCONTROL, (uint8_t *)buffer, 1);
      break;
    case SC16IS752_SET_IODIR:
      write_reg(ctx, SC16IS752_IODIR, (uint8_t *)buffer, 1);
      break;
    case SC16IS752_GET_IODIR:
      read_reg(ctx, SC16IS752_IODIR, (uint8_t *)buffer, 1);
      break;
    case SC16IS752_SET_IOSTATE:
      write_reg(ctx, SC16IS752_IOSTATE, (uint8_t *)buffer, 1);
      break;
    case SC16IS752_GET_IOSTATE:
      read_reg(ctx, SC16IS752_IOSTATE, (uint8_t *)buffer, 1);
      break;
    case SC16IS752_SET_EFCR:
      write_reg(ctx, SC16IS752_EFCR, (uint8_t *)buffer, 1);
      break;
    case SC16IS752_GET_EFCR:
      read_reg(ctx, SC16IS752_EFCR, (uint8_t *)buffer, 1);
      break;
    case TIOCMGET:
      sc16is752_get_modem_bits(ctx, (int *)buffer);
      break;
    case TIOCMSET:
      sc16is752_set_modem_bits(ctx, (int *)buffer, 0, 0);
      break;
    case TIOCMBIS:
      sc16is752_set_modem_bits(ctx, NULL, *(int *)buffer, 0);
      break;
    case TIOCMBIC:
      sc16is752_set_modem_bits(ctx, NULL, 0, *(int *)buffer);
      break;
    default:
      rtems_set_errno_and_return_minus_one(EINVAL);
  }

  return 0;
}

const rtems_termios_device_handler sc16is752_termios_handler = {
  .first_open = sc16is752_first_open,
  .last_close = sc16is752_last_close,
  .write = sc16is752_write,
  .set_attributes = sc16is752_set_attributes,
  .ioctl = sc16is752_ioctl,
  .mode = TERMIOS_IRQ_SERVER_DRIVEN
};

void sc16is752_interrupt_handler(void *arg)
{
  sc16is752_context *ctx = (sc16is752_context *)arg;
  uint8_t data[2];
  uint8_t iir;

  read_2_reg(ctx, SC16IS752_IIR, SC16IS752_RXLVL, data);
  iir = data[0];

  if ((iir & SC16IS752_IIR_TX_INTERRUPT) != 0 && ctx->tx_in_progress > 0) {
    rtems_termios_dequeue_characters(ctx->tty, ctx->tx_in_progress);
  }

  if ((iir & SC16IS752_IIR_RX_INTERRUPT) != 0) {
    uint8_t buf[SC16IS752_FIFO_DEPTH];
    uint8_t rxlvl = data[1];

    rxlvl = MIN(rxlvl, SC16IS752_FIFO_DEPTH);
    read_reg(ctx, SC16IS752_RHR, &buf[0], rxlvl);
    rtems_termios_enqueue_raw_characters(ctx->tty, (const char *)&buf[0], rxlvl);
  }
}
