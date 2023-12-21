/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMTMS570
 *
 * @brief This source file contains the definition of ::BSP_output_char and
 *   ::BSP_poll_char.
 */

/*
 * Copyright (C) 2023 embedded brains GmbH & Co. KG
 * Copyright (C) 2014 Premysl Houdek <kom541000@gmail.com>
 *
 * Google Summer of Code 2014 at
 * Czech Technical University in Prague
 * Zikova 1903/4
 * 166 36 Praha 6
 * Czech Republic
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

#include <rtems/bspIo.h>
#include <rtems/sysinit.h>
#include <stdint.h>
#include <string.h>
#include <bsp/tms570-sci-driver.h>

#define TMS570_CONSOLE (&driver_context_table[0])

/**
 * @brief Puts chars into peripheral
 *
 * debug functions always use serial dev 0 peripheral
 *
 * @retval Void
 */
static void tms570_debug_console_out(char ch)
{
  tms570_sci_context *ctx = TMS570_CONSOLE;
  volatile tms570_sci_t *regs = ctx->regs;

  while ( true ) {
    rtems_interrupt_level level;

    while ( ( regs->FLR & TMS570_SCI_FLR_TXRDY ) == 0) {
      /* Wait */
    }

    rtems_interrupt_disable( level );

    if ( ( regs->FLR & TMS570_SCI_FLR_TXRDY ) != 0) {
      regs->TD = ch;
      rtems_interrupt_enable( level );

      break;
    }

    rtems_interrupt_enable( level );
  }

  while ( ( regs->FLR & TMS570_SCI_FLR_TX_EMPTY ) == 0) {
    /* Wait */
  }
}

static void tms570_debug_console_init(void)
{
  tms570_sci_context *ctx = TMS570_CONSOLE;
  struct termios term;

  tms570_sci_initialize(ctx);
  memset(&term, 0, sizeof(term));
  term.c_ospeed = B115200;
  tms570_sci_set_attributes(&ctx->base, &term);
  BSP_output_char = tms570_debug_console_out;
}

static void tms570_debug_console_early_init(char c)
{
  tms570_debug_console_init();
  tms570_debug_console_out(c);
}

/**
 * @brief debug console input
 *
 * debug functions always use serial dev 0 peripheral
 *
 * @retval x Read char
 * @retval -1 No input character available
 */
static int tms570_debug_console_in( void )
{
  tms570_sci_context *ctx = TMS570_CONSOLE;
  volatile tms570_sci_t *regs = ctx->regs;
  rtems_interrupt_level level;
  int c;

  rtems_interrupt_disable(level);

  if ( regs->FLR & TMS570_SCI_FLR_RXRDY ) {
      c = (unsigned char) regs->RD;
  } else {
      c = -1;
  }

  rtems_interrupt_enable(level);

  return c;
}

BSP_output_char_function_type BSP_output_char =
  tms570_debug_console_early_init;

BSP_polling_getchar_function_type BSP_poll_char = tms570_debug_console_in;

RTEMS_SYSINIT_ITEM(
  tms570_debug_console_init,
  RTEMS_SYSINIT_BSP_START,
  RTEMS_SYSINIT_ORDER_LAST_BUT_5
);
