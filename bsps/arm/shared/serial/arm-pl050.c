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
