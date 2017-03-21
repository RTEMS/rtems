/**
 * @file
 *
 * @ingroup arm_lpc32xx
 *
 * @brief High speed UART driver (14-clock).
 */

/*
 * Copyright (c) 2010-2014 embedded brains GmbH.  All rights reserved.
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

#include <bsp.h>
#include <bsp/lpc32xx.h>
#include <bsp/irq.h>
#include <bsp/hsu.h>

#define HSU_FIFO_SIZE 64

#define HSU_LEVEL_RX_MASK 0xffU
#define HSU_LEVEL_TX_MASK 0xff00U
#define HSU_LEVEL_TX_SHIFT 8

#define HSU_RX_DATA_MASK 0xffU
#define HSU_RX_EMPTY (1U << 8)
#define HSU_RX_ERROR (1U << 9)
#define HSU_RX_BREAK (1U << 10)

#define HSU_IIR_TX (1U << 0)
#define HSU_IIR_RX_TRIG (1U << 1)
#define HSU_IIR_RX_TIMEOUT (1U << 2)

#define HSU_CTRL_INTR_DISABLED 0x1280fU
#define HSU_CTRL_RX_INTR_ENABLED 0x1284fU
#define HSU_CTRL_RX_AND_TX_INTR_ENABLED 0x1286fU

/* We are interested in RX timeout, RX trigger and TX trigger interrupts */
#define HSU_IIR_MASK 0x7U

bool lpc32xx_hsu_probe(rtems_termios_device_context *base)
{
  lpc32xx_hsu_context *ctx = (lpc32xx_hsu_context *) base;
  volatile lpc32xx_hsu *hsu = ctx->hsu;

  hsu->ctrl = HSU_CTRL_INTR_DISABLED;

  /* Drain FIFOs */
  while (hsu->level != 0) {
    hsu->fifo;
  }

  return true;
}

static void lpc32xx_hsu_interrupt_handler(void *arg)
{
  rtems_termios_tty *tty = arg;
  lpc32xx_hsu_context *ctx = rtems_termios_get_device_context(tty);
  volatile lpc32xx_hsu *hsu = ctx->hsu;

  /* Iterate until no more interrupts are pending */
  do {
    int rv = 0;
    int i = 0;
    char buf [HSU_FIFO_SIZE];

    /* Enqueue received characters */
    while (i < HSU_FIFO_SIZE) {
      uint32_t in = hsu->fifo;

      if ((in & HSU_RX_EMPTY) == 0) {
        if ((in & HSU_RX_BREAK) == 0) {
          buf [i] = in & HSU_RX_DATA_MASK;
          ++i;
        }
      } else {
        break;
      }
    }
    rtems_termios_enqueue_raw_characters(tty, buf, i);

    /* Dequeue transmitted characters */
    rv = rtems_termios_dequeue_characters(tty, (int) ctx->chars_in_transmission);
    if (rv == 0) {
      /* Nothing to transmit */
    }
  } while ((hsu->iir & HSU_IIR_MASK) != 0);
}

static bool lpc32xx_hsu_first_open(
  struct rtems_termios_tty *tty,
  rtems_termios_device_context *base,
  struct termios *term,
  rtems_libio_open_close_args_t *args
)
{
  lpc32xx_hsu_context *ctx = (lpc32xx_hsu_context *) base;
  volatile lpc32xx_hsu *hsu = ctx->hsu;
  rtems_status_code sc;
  bool ok;

  sc = rtems_interrupt_handler_install(
    ctx->irq,
    "HSU",
    RTEMS_INTERRUPT_UNIQUE,
    lpc32xx_hsu_interrupt_handler,
    tty
  );
  ok = sc == RTEMS_SUCCESSFUL;

  if (ok) {
    rtems_termios_set_initial_baud(tty, ctx->initial_baud);
    hsu->ctrl = HSU_CTRL_RX_INTR_ENABLED;
  }

  return ok;
}

static void lpc32xx_hsu_last_close(
  struct rtems_termios_tty *tty,
  rtems_termios_device_context *base,
  rtems_libio_open_close_args_t *args
)
{
  lpc32xx_hsu_context *ctx = (lpc32xx_hsu_context *) base;
  volatile lpc32xx_hsu *hsu = ctx->hsu;

  hsu->ctrl = HSU_CTRL_INTR_DISABLED;

  rtems_interrupt_handler_remove(
    ctx->irq,
    lpc32xx_hsu_interrupt_handler,
    tty
  );
}

static void lpc32xx_hsu_write(
  rtems_termios_device_context *base,
  const char *buf,
  size_t len
)
{
  lpc32xx_hsu_context *ctx = (lpc32xx_hsu_context *) base;
  volatile lpc32xx_hsu *hsu = ctx->hsu;
  size_t tx_level = (hsu->level & HSU_LEVEL_TX_MASK) >> HSU_LEVEL_TX_SHIFT;
  size_t tx_free = HSU_FIFO_SIZE - tx_level;
  size_t i = 0;
  size_t out = len > tx_free ? tx_free : len;

  for (i = 0; i < out; ++i) {
    hsu->fifo = buf [i];
  }

  ctx->chars_in_transmission = out;

  if (len > 0) {
    hsu->ctrl = HSU_CTRL_RX_AND_TX_INTR_ENABLED;
  } else {
    hsu->ctrl = HSU_CTRL_RX_INTR_ENABLED;
    hsu->iir = HSU_IIR_TX;
  }
}

static bool lpc32xx_hsu_set_attributes(
  rtems_termios_device_context *base,
  const struct termios *term
)
{
  lpc32xx_hsu_context *ctx = (lpc32xx_hsu_context *) base;
  volatile lpc32xx_hsu *hsu = ctx->hsu;
  int baud_flags = term->c_ospeed;

  if (baud_flags != 0) {
    int32_t baud = rtems_termios_baud_to_number(baud_flags);

    if (baud > 0) {
      uint32_t baud_divisor = 14 * (uint32_t) baud;
      uint32_t rate = LPC32XX_PERIPH_CLK / baud_divisor;
      uint32_t remainder = LPC32XX_PERIPH_CLK - rate * baud_divisor;

      if (2 * remainder >= baud_divisor) {
        ++rate;
      }

      hsu->rate = rate - 1;
    }
  }

  return true;
}

const rtems_termios_device_handler lpc32xx_hsu_fns = {
  .first_open = lpc32xx_hsu_first_open,
  .last_close = lpc32xx_hsu_last_close,
  .write = lpc32xx_hsu_write,
  .set_attributes = lpc32xx_hsu_set_attributes,
  .mode = TERMIOS_IRQ_DRIVEN
};
