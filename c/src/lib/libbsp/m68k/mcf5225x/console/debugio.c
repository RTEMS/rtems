/*
 *  Multi UART console serial I/O.
 *
 *  TO DO: Add DMA input/output
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <stdio.h>
#include <fcntl.h>
#include <rtems/libio.h>
#include <rtems/termiostypes.h>
#include <termios.h>
#include <bsp.h>
#include <malloc.h>
#include <rtems/mw_uid.h>

#include <rtems/bspIo.h>

static void _BSP_null_char(char c)
{
  rtems_interrupt_level level=UART0_IRQ_LEVEL;

  if (c == '\n')
        _BSP_null_char('\r');

  rtems_interrupt_disable(level);
  while ((MCF_UART_USR(CONSOLE_PORT) & MCF_UART_USR_TXRDY) == 0)
    continue;
  MCF_UART_UTB(CONSOLE_PORT) = c;
  while ((MCF_UART_USR(CONSOLE_PORT) & MCF_UART_USR_TXRDY) == 0)
    continue;
  rtems_interrupt_enable(level);
}

BSP_output_char_function_type     BSP_output_char = _BSP_null_char;
BSP_polling_getchar_function_type BSP_poll_char = NULL;
