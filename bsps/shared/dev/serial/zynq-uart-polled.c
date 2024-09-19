/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSDeviceSerialZynq
 *
 * @brief This source file contains the implementation of the polled Zynq UART
 *   support.
 */

/*
 * Copyright (C) 2013, 2017 embedded brains GmbH & Co. KG
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

#include <dev/serial/zynq-uart-regs.h>

#include <bspopts.h>

#include <rtems/dev/io.h>
#include <rtems/score/assert.h>

static uint32_t zync_uart_baud_error(
  uint32_t selected_clock,
  uint32_t desired_baud,
  uint32_t cd,
  uint32_t bdiv_plus_one
)
{
  uint32_t actual_baud = selected_clock / ( cd * bdiv_plus_one );

  if ( actual_baud > desired_baud ) {
    return actual_baud - desired_baud;
  }

  return desired_baud - actual_baud;
}

uint32_t zynq_uart_calculate_baud(
  uint32_t  desired_baud,
  uint32_t  mode_clks,
  uint32_t *cd_ptr,
  uint32_t *bdiv_ptr
)
{
  uint32_t best_error = UINT32_MAX;
  uint32_t best_cd = 0x28b;
  uint32_t best_bdiv_plus_one = 16;
  uint32_t bdiv_plus_one;
  uint32_t selected_clock;

  _Assert((mode_clks & ~ZYNQ_UART_MODE_CLKS) == 0);
  selected_clock = zynq_uart_input_clock() / (1U << (3 * mode_clks));

  for (bdiv_plus_one = 5; bdiv_plus_one <= 256; ++bdiv_plus_one) {
    uint32_t cd = ( selected_clock / bdiv_plus_one ) / desired_baud;
    uint32_t error;

    if (cd == 0 ) {
      cd = 1;
    } else if ( cd > 65535 ) {
      cd = 65535;
    }

    error = zync_uart_baud_error(
      selected_clock,
      desired_baud,
      cd,
      bdiv_plus_one
    );

    /*
     * The procedure to detect a start bit uses three samples in the middle of
     * an RX-bit.  If the sample set is too small, there may be a sample in
     * another bit in case the baud setting is not accurate.  Most noise is in
     * the form of small peaks, if the sample rate is too high, then noise may
     * get detected as a bit.
     *
     * Prefer an sample set of around 16 per RX-bit.
     */
    if (error < best_error || (bdiv_plus_one <= 20 && error <= best_error)) {
      best_error = error;
      best_cd = cd;
      best_bdiv_plus_one = bdiv_plus_one;
    }
  }

  *cd_ptr = best_cd;
  *bdiv_ptr = best_bdiv_plus_one - 1;
  return best_error;
}

void zynq_uart_initialize(volatile zynq_uart *regs)
{
  uint32_t mode_clks = regs->mode & ZYNQ_UART_MODE_CLKS;
  uint32_t cd;
  uint32_t bdiv;

  zynq_uart_reset_tx_flush(regs);
  (void) zynq_uart_calculate_baud(
    ZYNQ_UART_DEFAULT_BAUD,
    mode_clks,
    &cd,
    &bdiv
  );

  regs->control = ZYNQ_UART_CONTROL_RXDIS | ZYNQ_UART_CONTROL_TXDIS;
  regs->baud_rate_gen = ZYNQ_UART_BAUD_RATE_GEN_CD(cd);
  regs->baud_rate_div = ZYNQ_UART_BAUD_RATE_DIV_BDIV(bdiv);
  /* A Tx/Rx logic reset must be issued after baud rate manipulation */
  regs->control = ZYNQ_UART_CONTROL_RXRES | ZYNQ_UART_CONTROL_TXRES;
  regs->rx_fifo_trg_lvl = ZYNQ_UART_RX_FIFO_TRG_LVL_RTRIG(0);
  regs->rx_timeout = ZYNQ_UART_RX_TIMEOUT_RTO(0);
  regs->mode = ZYNQ_UART_MODE_CHMODE(ZYNQ_UART_MODE_CHMODE_NORMAL)
    | ZYNQ_UART_MODE_PAR(ZYNQ_UART_MODE_PAR_NONE)
    | ZYNQ_UART_MODE_CHRL(ZYNQ_UART_MODE_CHRL_8)
    | mode_clks;
  regs->control = ZYNQ_UART_CONTROL_RXEN | ZYNQ_UART_CONTROL_TXEN;
}

int zynq_uart_read_char_polled(volatile zynq_uart *regs)
{
  if ((regs->channel_sts & ZYNQ_UART_CHANNEL_STS_REMPTY) != 0) {
    return -1;
  } else {
    return ZYNQ_UART_TX_RX_FIFO_FIFO_GET(regs->tx_rx_fifo);
  }
}

void zynq_uart_write_char_polled(volatile zynq_uart *regs, char c)
{
  while ((regs->channel_sts & ZYNQ_UART_CHANNEL_STS_TNFUL) != 0) {
    _IO_Relax();
  }

  regs->tx_rx_fifo = ZYNQ_UART_TX_RX_FIFO_FIFO(c);
}

void zynq_uart_reset_tx_flush(volatile zynq_uart *regs)
{
  while (
    (regs->channel_sts &
      (ZYNQ_UART_CHANNEL_STS_TEMPTY | ZYNQ_UART_CHANNEL_STS_TACTIVE)) !=
    ZYNQ_UART_CHANNEL_STS_TEMPTY) {
    _IO_Relax();
  }
}
