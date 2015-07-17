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
#include <stdint.h>
#include <bsp/tms570-sci.h>
#include <bsp/tms570-sci-driver.h>


/**
 * @brief Puts chars into peripheral
 *
 * debug functions always use serial dev 0 peripheral
 *
 * @retval Void
 */
static void tms570_putc(char ch)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
  while ( ( driver_context_table[0].regs->FLR & TMS570_SCI_FLR_TXRDY ) == 0) {
    rtems_interrupt_flash(level);
  }
  driver_context_table[0].regs->TD = ch;
  rtems_interrupt_enable(level);
}

/**
 * @brief debug console output
 *
 * debug functions always use serial dev 0 peripheral
 *
 * @retval Void
 */
static void tms570_uart_output(char c)
{
  if ( c == '\n' ) {
    char r = '\r';
    tms570_putc(r);
  }
  tms570_putc(c);
}

/**
 * @brief debug console input
 *
 * debug functions always use serial dev 0 peripheral
 *
 * @retval x Read char
 * @retval -1 No input character available
 */
static int tms570_uart_input( void )
{
  if ( driver_context_table[0].regs->FLR & TMS570_SCI_FLR_RXRDY ) {
      return driver_context_table[0].regs->RD;
  } else {
      return -1;
  }
}

BSP_output_char_function_type BSP_output_char = tms570_uart_output;
BSP_polling_getchar_function_type BSP_poll_char = tms570_uart_input;
