/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Pragnesh Patel <pragnesh.patel@sifive.com>
 * Copyright (c) 2019 Sachin Ghadi <sachin.ghadi@sifive.com>
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

#include <bsp/riscv.h>
#include <bsp/fe310-uart.h>

#include <assert.h>

int fe310_uart_read(rtems_termios_device_context *base)
{
  fe310_uart_context * ctx = (fe310_uart_context*) base;

  if ((ctx->regs->rxdata & TXRXREADY) != 0) {
    return -1;
  } else {
    return ctx->regs->rxdata;
  }
}

static void fe310_uart_write (
  rtems_termios_device_context  *base,
  const char                    *buf,
  size_t                        n
)
{
  fe310_uart_context * ctx = (fe310_uart_context*) base;
  size_t i;

  ctx->regs->div = riscv_get_core_frequency() / 115200 - 1;
  ctx->regs->txctrl |= 1;
  ctx->regs->rxctrl |= 1;

  for (i = 0; i < n; ++i) {
    while ((ctx->regs->txdata & TXRXREADY) != 0) {
      /* Wait */
    }

    ctx->regs->txdata = buf[i];
  }
}

void fe310_console_putchar(rtems_termios_device_context *context, char c)
{
  fe310_uart_write(context, &c, 1);
}

static bool fe310_uart_first_open (
  rtems_termios_tty             *tty,
  rtems_termios_device_context  *base,
  struct termios                *term,
  rtems_libio_open_close_args_t *args
)
{
  fe310_uart_context * ctx;
  rtems_status_code sc;

  /* Configure GPIO to be UART */

  sc = rtems_termios_set_initial_baud(tty, B115200);
  if ( sc != RTEMS_SUCCESSFUL ) {
    return false;
  }

  /* Set up a baud rate and enable tx and rx */
  ctx = (fe310_uart_context *) base;
  (ctx->regs)->div = riscv_get_core_frequency() / 115200 - 1;
  (ctx->regs)->txctrl |= 1;
  (ctx->regs)->rxctrl |= 1;
  return true;
};

const rtems_termios_device_handler fe310_uart_handler = {
  .first_open = fe310_uart_first_open,
  .write = fe310_uart_write,
  .poll_read = fe310_uart_read,
  .mode = TERMIOS_POLLED
};
