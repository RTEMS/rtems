/**
 * @file printk-support.c
 *
 * @ingroup tms570
 *
 * @brief definitions of serial line for debugging.
 */

/*
 * Copyright (c) 2014 Premysl Houdek <kom541000@gmail.com>
 *
 * Google Summer of Code 2014 at
 * Czech Technical University in Prague
 * Zikova 1903/4
 * 166 36 Praha 6
 * Czech Republic
 *
 * Based on LPC24xx and LPC1768 BSP
 * by embedded brains GmbH and others
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <rtems/bspIo.h>
#include <rtems/sysinit.h>
#include <stdint.h>
#include <bsp/tms570-sci.h>
#include <bsp/tms570-sci-driver.h>

#define TMS570_CONSOLE (&driver_context_table[0])

/**
 * @brief Puts chars into peripheral
 *
 * debug functions always use serial dev 0 peripheral
 *
 * @retval Void
 */
static void tms570_debug_console_putc(char ch)
{
  tms570_sci_context *ctx = TMS570_CONSOLE;
  volatile tms570_sci_t *regs = ctx->regs;
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
  while ( ( regs->FLR & TMS570_SCI_FLR_TXRDY ) == 0) {
    rtems_interrupt_flash(level);
  }
  regs->TD = ch;
  while ( ( regs->FLR & TMS570_SCI_FLR_TX_EMPTY ) == 0) {
    rtems_interrupt_flash(level);
  }
  rtems_interrupt_enable(level);
}

/**
 * @brief debug console output
 *
 * debug functions always use serial dev 0 peripheral
 *
 * @retval Void
 */
static void tms570_debug_console_out(char c)
{
  if ( c == '\n' ) {
    tms570_debug_console_putc('\r');
  }

  tms570_debug_console_putc(c);
}

static void tms570_debug_console_init(void)
{
  tms570_sci_context *ctx = TMS570_CONSOLE;
  struct termios term;

  tms570_sci_initialize(ctx);
  memset(&term, 0, sizeof(term));
  term.c_cflag = B115200;
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
  RTEMS_SYSINIT_ORDER_LAST
);
