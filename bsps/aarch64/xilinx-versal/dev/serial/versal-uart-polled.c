/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2021 Gedare Bloom <gedare@rtems.org>
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

#include <dev/serial/versal-uart.h>
#include <dev/serial/versal-uart-regs.h>

#include <bspopts.h>

static inline volatile versal_uart *versal_uart_get_regs(
  rtems_termios_device_context *base
)
{
  versal_uart_context *ctx = (versal_uart_context *) base;
  return ctx->regs;
}

/*
 * Make weak and let the user override.
 */
uint32_t versal_uart_input_clock(void) __attribute__ ((weak));

uint32_t versal_uart_input_clock(void)
{
  return VERSAL_CLOCK_UART;
}

int32_t versal_cal_baud_rate(
  uint32_t baudrate,
  uint32_t maxerror,
  uint32_t *ibdiv,
  uint32_t *fbdiv
)
{
  uint32_t calcbaudrate;
  uint32_t bauderror;
  uint32_t percenterror;
  uint32_t fbdivrnd;
  uint32_t inputclk = versal_uart_input_clock();

  /*
   * The baud rate cannot be larger than the UART clock / 16.
   */
  if ((baudrate * 16) > inputclk) {
    return -1;
  }

  /*
   * The UART clock cannot be larger than 16*65535*baudrate.
   * Could maybe use an estimate (inputclk / 2**16) to save a division.
   * This invariant gets checked below, by ensuring ibdiv < 2**16.
   */

  /*
   * The UART clock cannot be more than 5/3 times faster than the LPD_LSBUS_CLK
   *  - TODO?
   */

  /*
   * The baud rate divisor is a 16-bit integer and 6-bit fractional part.
   * It is equal to the UART clock / (16 * baudrate).
   */
  *ibdiv = inputclk / 16 / baudrate;
  if ( *ibdiv > 1<<16 ) {
    return -1;
  }

  /*
   * Find the fractional part. This can overflow with 32-bit arithmetic if
   * inputclk > 536870911, so use 64-bit. Unfortunately, this means we have
   * two 64-bit divisions here.
   */
  fbdivrnd = ((((uint64_t)inputclk / 16) << 7) / baudrate) & 0x1;
  *fbdiv = (((((uint64_t)inputclk / 16) << 6) / baudrate) & 0x3F) + fbdivrnd;

  /*
   * calculate the baudrate and check if the error is too large.
   */
  calcbaudrate = (((uint64_t)inputclk / 16) << 6) / ((*ibdiv << 6) | *fbdiv);

  if (baudrate > calcbaudrate) {
    bauderror = baudrate - calcbaudrate;
  } else {
    bauderror = calcbaudrate - baudrate;
  }

  percenterror = (bauderror * 100) / baudrate;

  if (maxerror < percenterror) {
    return -1;
  }

  return 0;
}

int versal_uart_initialize(rtems_termios_device_context *base)
{
  volatile versal_uart *regs = versal_uart_get_regs(base);
  uint32_t maxerr = 3;
  uint32_t ibauddiv = 0;
  uint32_t fbauddiv = 0;
  int rv;

  versal_uart_reset_tx_flush(base);

  rv = versal_cal_baud_rate(
      VERSAL_UART_DEFAULT_BAUD,
      maxerr,
      &ibauddiv,
      &fbauddiv
  );

  if ( rv < 0 ) {
    return rv;
  }

  /* Line control: 8-bit word length, no parity, no FIFO, 1 stop bit */
  regs->uartlcr_h = VERSAL_UARTLCR_H_WLEN( VERSAL_UARTLCR_H_WLEN_8 )
    | VERSAL_UARTLCR_H_FEN;

  /* Control: receive, transmit, uart enable, no CTS, no RTS, no loopback */
  regs->uartcr = VERSAL_UARTCR_RXE
    | VERSAL_UARTCR_TXE
    | VERSAL_UARTCR_UARTEN;

  regs->uartibrd = VERSAL_UARTIBRD_BAUD_DIVINT(ibauddiv);
  regs->uartfbrd = VERSAL_UARTFBRD_BAUD_DIVFRAC(fbauddiv);

  return 0;
}

int versal_uart_read_polled(rtems_termios_device_context *base)
{
  volatile versal_uart *regs = versal_uart_get_regs(base);

  if ((regs->uartfr & VERSAL_UARTFR_RXFE) != 0) {
    return -1;
  } else {
    return VERSAL_UARTDR_DATA_GET(regs->uartdr);
  }
}

void versal_uart_write_polled(
  rtems_termios_device_context *base,
  char c
)
{
  volatile versal_uart *regs = versal_uart_get_regs(base);

  while ((regs->uartfr & VERSAL_UARTFR_TXFF) != 0) {
    /* Wait while full */
  }

  regs->uartdr = VERSAL_UARTDR_DATA(c);
}

void versal_uart_reset_tx_flush(rtems_termios_device_context *base)
{
  volatile versal_uart *regs = versal_uart_get_regs(base);
  int c = 4;

  while (c-- > 0)
    versal_uart_write_polled(base, '\r');

  while ((regs->uartfr & VERSAL_UARTFR_TXFE) == 0) {
    /* Wait for empty */
  }
  while ((regs->uartfr & VERSAL_UARTFR_BUSY) != 0) {
    /* Wait for empty */
  }
}
