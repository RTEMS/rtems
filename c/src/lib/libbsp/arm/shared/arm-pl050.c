/*
 * Copyright (c) 2013-2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <assert.h>

#include <bsp/irq.h>
#include <bsp/arm-pl050.h>

static volatile pl050 *pl050_get_regs(rtems_termios_device_context *base)
{
  arm_pl050_context *ctx = (arm_pl050_context *) base;

  return ctx->regs;
}

static void pl050_interrupt(void *arg)
{
  rtems_termios_tty *tty = arg;
  rtems_termios_device_context *base = rtems_termios_get_device_context(tty);
  volatile pl050 *regs = pl050_get_regs(base);
  uint32_t kmiir_rx = PL050_KMIIR_KMIRXINTR;
  uint32_t kmiir_tx = (regs->kmicr & PL050_KMICR_KMITXINTREN) != 0 ?
    PL050_KMIIR_KMITXINTR : 0;
  uint32_t kmiir = regs->kmiir;

  if ((kmiir & kmiir_rx) != 0) {
    char c = (char) PL050_KMIDATA_KMIDATA_GET(regs->kmidata);

    rtems_termios_enqueue_raw_characters(tty, &c, 1);
  }

  if ((kmiir & kmiir_tx) != 0) {
    rtems_termios_dequeue_characters(tty, 1);
  }
}

static bool pl050_first_open(
  struct rtems_termios_tty *tty,
  rtems_termios_device_context *base,
  struct termios *term,
  rtems_libio_open_close_args_t *args
)
{
  arm_pl050_context *ctx = (arm_pl050_context *) base;
  volatile pl050 *regs = pl050_get_regs(base);
  rtems_status_code sc;

  rtems_termios_set_initial_baud(tty, ctx->initial_baud);

  regs->kmicr = PL050_KMICR_KMIEN | PL050_KMICR_KMIRXINTREN;

  sc = rtems_interrupt_handler_install(
    ctx->irq,
    "PL050",
    RTEMS_INTERRUPT_UNIQUE,
    pl050_interrupt,
    tty
  );
  assert(sc == RTEMS_SUCCESSFUL);

  return true;
}

static void pl050_last_close(
  struct rtems_termios_tty *tty,
  rtems_termios_device_context *base,
  rtems_libio_open_close_args_t *args
)
{
  arm_pl050_context *ctx = (arm_pl050_context *) base;
  volatile pl050 *regs = pl050_get_regs(base);
  rtems_status_code sc;

  regs->kmicr = 0;

  sc = rtems_interrupt_handler_remove(
    ctx->irq,
    pl050_interrupt,
    tty
  );
  assert(sc == RTEMS_SUCCESSFUL);
}

static void pl050_write_support(
  rtems_termios_device_context *base,
  const char *s,
  size_t n
)
{
  volatile pl050 *regs = pl050_get_regs(base);

  if (n > 0) {
    regs->kmidata = PL050_KMIDATA_KMIDATA(s[0]);
    regs->kmicr |= PL050_KMICR_KMITXINTREN;
  } else {
    regs->kmicr &= ~PL050_KMICR_KMITXINTREN;
  }
}

const rtems_termios_device_handler arm_pl050_fns = {
  .first_open = pl050_first_open,
  .last_close = pl050_last_close,
  .write = pl050_write_support,
  .mode = TERMIOS_IRQ_DRIVEN
};
