/**
 * @file
 *
 * @ingroup bsp_kit
 *
 * @brief Output character implementation for standard UARTs.
 */

/*
 * Copyright (c) 2010
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <rtems/bspIo.h>

#include <bsp/uart-output-char.h>

static void uart_output(char c)
{
  while ((CONSOLE_LSR & CONSOLE_LSR_THRE) == 0) {
    /* Wait */
  }

  CONSOLE_THR = c;
}

static void output(char c)
{
  if (c == '\n') {
    uart_output('\r');
  }

  uart_output(c);
}

BSP_output_char_function_type BSP_output_char = output;
