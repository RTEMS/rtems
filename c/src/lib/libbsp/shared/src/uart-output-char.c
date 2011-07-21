/**
 * @file
 *
 * @ingroup bsp_kit
 *
 * @brief Output character implementation for standard UARTs.
 */

/*
 * Copyright (c) 2010-2011 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <rtems/bspIo.h>

#include <bsp/uart-output-char.h>

static void uart_output_raw(char c)
{
  while ((CONSOLE_LSR & CONSOLE_LSR_THRE) == 0) {
    /* Wait */
  }

  CONSOLE_THR = c;
}

static void uart_output(char c)
{
  if (c == '\n') {
    uart_output_raw('\r');
  }

  uart_output_raw(c);
}

static int uart_input(void)
{
  if ((CONSOLE_LSR & CONSOLE_LSR_RDR) != 0) {
    return CONSOLE_RBR;
  } else {
    return -1;
  }
}

BSP_output_char_function_type BSP_output_char = uart_output;

BSP_polling_getchar_function_type BSP_poll_char = uart_input;
