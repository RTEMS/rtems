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
  return (ctx->ier & IER_SLEEP_MODE) != 0;
}

static void set_sleep_mode(sc16is752_context *ctx, bool enable)
{
  if (enable) {
    ctx->ier |= IER_SLEEP_MODE;
  } else {
    ctx->ier &= ~IER_SLEEP_MODE;
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

  ctx->lcr |= LCR_ENABLE_DIVISOR;
  write_reg(ctx, SC16IS752_LCR, &ctx->lcr, 1);

  write_reg(ctx, SC16IS752_MCR, &mcr, 1);
  write_reg(ctx, SC16IS752_DLH, &dlh, 1);
  write_reg(ctx, SC16IS752_DLL, &dll, 1);

  ctx->lcr &= ~LCR_ENABLE_DIVISOR;
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
      mcr |= MCR_PRESCALE_NEEDED;
    }
  } else {
    mcr &= ~MCR_PRESCALE_NEEDED;
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
  bool baud_successful;
  rtems_termios_baud_t baud;

  ctx->lcr = 0;

  baud = rtems_termios_baud_to_number(term->c_ospeed);
  baud_successful = set_baud(ctx, baud);
  if (!baud_successful){
    return false;
  }

  if ((term->c_cflag & CREAD) == 0){
    ctx->efcr |= EFCR_RX_DISABLE;
  } else {
    ctx->efcr &= ~EFCR_RX_DISABLE;
  }

  write_reg(ctx, SC16IS752_EFCR, &ctx->efcr, 1);

  switch (term->c_cflag & CSIZE) {
    case CS5:
      ctx->lcr |= LCR_CHRL_5_BIT;
      break;
    case CS6:
      ctx->lcr |= LCR_CHRL_6_BIT;
      break;
    case CS7:
      ctx->lcr |= LCR_CHRL_7_BIT;
      break;
    case CS8:
      ctx->lcr |= LCR_CHRL_8_BIT;
      break;
  }

  if ((term->c_cflag & PARENB) != 0){
    if ((term->c_cflag & PARODD) != 0) {
      ctx->lcr &= ~LCR_EVEN_PARITY;
    } else {
      ctx->lcr |= LCR_EVEN_PARITY;
    }
  } else {
    ctx->lcr &= ~LCR_SET_PARITY;
  }

  if ((term->c_cflag & CSTOPB) != 0) {
    ctx->lcr |= LCR_2_STOP_BIT;
  } else {
    ctx->lcr &= ~LCR_2_STOP_BIT;
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

  (void)args;
  sc16is752_context *ctx = (sc16is752_context *)base;

  ctx->tty = tty;

  ok = (*ctx->first_open)(ctx);
  if (!ok) {
    return ok;
  }

  if (ctx->mode == SC16IS752_MODE_RS485) {
    ctx->efcr = EFCR_RS485_ENABLE;
  } else {
    ctx->efcr = 0;
  }

  write_reg(ctx, SC16IS752_FCR, &ctx->efcr, 1);

  fcr = FCR_FIFO_EN | FCR_RX_FIFO_RST | FCR_TX_FIFO_RST
    | FCR_RX_FIFO_TRG_16 | FCR_TX_FIFO_TRG_32;
  write_reg(ctx, SC16IS752_FCR, &fcr, 1);

  ctx->ier = IER_RHR;
  write_reg(ctx, SC16IS752_IER, &ctx->ier, 1);
  set_efr(ctx, EFR_ENHANCED_FUNC_ENABLE);

  rtems_termios_set_initial_baud(tty, 115200);
  sc16is752_set_attributes(base, term);

  (*ctx->install_irq)(ctx);

  return true;
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
    ctx->ier |= IER_THR;
    len = MIN(len, 32);
    ctx->tx_in_progress = (uint8_t)len;
    write_reg(ctx, SC16IS752_THR, (const uint8_t *)&buf[0], len);
    write_reg(ctx, SC16IS752_IER, &ctx->ier, 1);
  } else {
    ctx->tx_in_progress = 0;
    ctx->ier &= ~IER_THR;
    write_reg(ctx, SC16IS752_IER, &ctx->ier, 1);
  }
}

static int sc16is752_ioctl(
  rtems_termios_device_context *base,
  ioctl_command_t               request,
  void                         *buffer
)
{
  sc16is752_context *ctx = (sc16is752_context *)base;

  switch (request) {
    case SC16IS752_SET_SLEEP_MODE:
      set_sleep_mode(ctx, *(int *)buffer != 0);
      break;
    case SC16IS752_GET_SLEEP_MODE:
      *(int *)buffer = is_sleep_mode_enabled(ctx);
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

  if ((iir & IIR_TX_INTERRUPT) != 0 && ctx->tx_in_progress > 0) {
    rtems_termios_dequeue_characters(ctx->tty, ctx->tx_in_progress);
  }

  if ((iir & IIR_RX_INTERRUPT) != 0) {
    uint8_t buf[SC16IS752_FIFO_DEPTH];
    uint8_t rxlvl = data[1];

    rxlvl = MIN(rxlvl, SC16IS752_FIFO_DEPTH);
    read_reg(ctx, SC16IS752_RHR, &buf[0], rxlvl);
    rtems_termios_enqueue_raw_characters(ctx->tty, (const char *)&buf[0], rxlvl);
  }
}
