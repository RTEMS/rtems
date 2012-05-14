/*
 *  Uart driver for Lattice Mico32 (lm32) UART
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Jukka Pietarinen <jukka.pietarinen@mrf.fi>, 2008,
 *  Micro-Research Finland Oy
 */

#include "../include/system_conf.h"
#include "uart.h"
#include <rtems/libio.h>

static inline int uartread(unsigned int reg)
{
  return *((int*)(UART_BASE_ADDRESS + reg));
}

static inline void uartwrite(unsigned int reg, int value)
{
  *((int*)(UART_BASE_ADDRESS + reg)) = value;
}

void BSP_uart_init(int baud)
{
  /* Disable UART interrupts */
  uartwrite(LM32_UART_IER, 0);

  /* Line control 8 bit, 1 stop, no parity */
  uartwrite(LM32_UART_LCR, LM32_UART_LCR_8BIT);

  /* Modem control, DTR = 1, RTS = 1 */
  uartwrite(LM32_UART_MCR, LM32_UART_MCR_DTR | LM32_UART_MCR_RTS);

  /* Set baud rate */
  uartwrite(LM32_UART_DIV, CPU_FREQUENCY/baud);
}

void BSP_uart_polled_write(char ch)
{
  /* Insert CR before LF */
  if (ch == '\n')
    BSP_uart_polled_write('\r');
  /* Wait until THR is empty. */
  while (!(uartread(LM32_UART_LSR) & LM32_UART_LSR_THRE));
  uartwrite(LM32_UART_RBR, ch);
}

char BSP_uart_polled_read( void )
{
  /* Wait until there is a byte in RBR */
  while (!(uartread(LM32_UART_LSR) & LM32_UART_LSR_DR));
  return (char) uartread(LM32_UART_RBR);
}

char BSP_uart_is_character_ready(char *ch)
{
  if (uartread(LM32_UART_LSR) & LM32_UART_LSR_DR)
    {
      *ch = (char) uartread(LM32_UART_RBR);
      return true;
    }
  *ch = '0';
  return false;
}
