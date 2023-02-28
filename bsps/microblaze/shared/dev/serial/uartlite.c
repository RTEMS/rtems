/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsMicroblaze
 *
 * @brief MicroBlaze AXI UART Lite support
 */

/*
 * Copyright (C) 2021 On-Line Applications Research Corporation (OAR)
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

#include <bsp/irq.h>
#include <dev/serial/uartlite.h>
#include <bspopts.h>

#ifdef BSP_MICROBLAZE_FPGA_CONSOLE_INTERRUPTS
static void microblaze_uart_interrupt( void *arg )
{
  rtems_termios_tty *tty = arg;
  uart_lite_context *ctx = rtems_termios_get_device_context( tty );

  while ( !XUartLite_IsReceiveEmpty( ctx->address ) ) {
    char c = (char) XUartLite_ReadReg( ctx->address, XUL_RX_FIFO_OFFSET );
    rtems_termios_enqueue_raw_characters( tty, &c, 1 );
  }

  if ( ctx->transmitting && XUartLite_IsTransmitEmpty( ctx->address ) ) {
    size_t sent = ctx->tx_queued;
    ctx->transmitting = false;
    ctx->tx_queued = 0;
    rtems_termios_dequeue_characters( tty, sent );
  }
}
#endif

static bool uart_first_open(
  struct rtems_termios_tty *tty,
  rtems_termios_device_context *base,
  struct termios *term,
  rtems_libio_open_close_args_t *args
)
{
  uart_lite_context *ctx = (uart_lite_context *) base;
#ifdef BSP_MICROBLAZE_FPGA_CONSOLE_INTERRUPTS
  rtems_status_code sc;
#endif

  rtems_termios_set_initial_baud( tty, ctx->initial_baud );

#ifdef BSP_MICROBLAZE_FPGA_CONSOLE_INTERRUPTS
  XUartLite_EnableIntr( ctx->address );

  sc = rtems_interrupt_handler_install(
    ctx->irq,
    "UART",
    RTEMS_INTERRUPT_SHARED,
    microblaze_uart_interrupt,
    tty
  );
  if ( sc != RTEMS_SUCCESSFUL ) {
    return false;
  }

  ctx->tty = tty;
#endif

  return true;
}

static void uart_last_close(
  rtems_termios_tty *tty,
  rtems_termios_device_context *base,
  rtems_libio_open_close_args_t *args
)
{
#ifdef BSP_MICROBLAZE_FPGA_CONSOLE_INTERRUPTS
  rtems_interrupt_handler_remove( 1, microblaze_uart_interrupt, tty );
#endif
}

#ifndef BSP_MICROBLAZE_FPGA_CONSOLE_INTERRUPTS
static int uart_read_polled( rtems_termios_device_context *base )
{
  uart_lite_context *ctx = (uart_lite_context *) base;

  if ( XUartLite_IsReceiveEmpty( ctx->address ) ) {
    return -1;
  }

  return XUartLite_ReadReg( ctx->address, XUL_RX_FIFO_OFFSET );
}
#endif

static void uart_write(
  rtems_termios_device_context *base,
  const char *s,
  size_t n
)
{
  uart_lite_context *ctx = (uart_lite_context *) base;

#ifdef BSP_MICROBLAZE_FPGA_CONSOLE_INTERRUPTS
  if ( n > 0 ) {
    size_t remaining = n;
    const char *p = &s[0];

    while (!XUartLite_IsTransmitFull( ctx->address ) && remaining > 0) {
      XUartLite_SendByte( ctx->address, *p );
      p++;
      remaining--;
    }

    ctx->transmitting = true;
    ctx->tx_queued = n - remaining;
  }
#else
  size_t i = 0;

  for ( i = 0; i < n; ++i ) {
    XUartLite_SendByte( ctx->address, s[i] );
  }
#endif
}

const rtems_termios_device_handler microblaze_uart_fns = {
  .first_open = uart_first_open,
  .last_close = uart_last_close,
  .write = uart_write,
#ifdef BSP_MICROBLAZE_FPGA_CONSOLE_INTERRUPTS
  .mode = TERMIOS_IRQ_DRIVEN
#else
  .poll_read = uart_read_polled,
  .mode = TERMIOS_POLLED
#endif
};
