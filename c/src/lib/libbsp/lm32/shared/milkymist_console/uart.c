/*
 *  Uart driver for Lattice Mico32 (lm32) UART
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 *
 *  COPYRIGHT (c) Yann Sionneau <yann.sionneau@telecom-sudparis.eu> (GSoC 2010)
 *  Telecom SudParis
 */

#include "../include/system_conf.h"
#include "uart.h"
#include <rtems/libio.h>

static inline int uartread(unsigned int reg)
{
  return *((int*)(reg));
}

static inline void uartwrite(unsigned int reg, int value)
{
  *((int*)(reg)) = value;
}

void BSP_uart_init(int baud)
{

  /* Set baud rate */
  uartwrite(MM_UART_DIV, CPU_FREQUENCY/baud/16);
}

void BSP_uart_polled_write(char ch)
{
  int ip;
  /* Wait until THR is empty. */
  uartwrite(MM_UART_RXTX, ch);
  do {
    lm32_read_interrupts(ip);
  } while (! (ip & MM_IRQ_UARTTX) );
  lm32_interrupt_ack(MM_IRQ_UARTTX);
}

char BSP_uart_polled_read( void )
{
  int ip;
  /* Wait until there is a byte in RBR */
  do {
    lm32_read_interrupts(ip);
  } while(! (ip & MM_IRQ_UARTRX) );
  lm32_interrupt_ack(MM_IRQ_UARTRX);
  return (char) uartread(MM_UART_RXTX);
}

char BSP_uart_is_character_ready(char *ch)
{
  int ip;
  lm32_read_interrupts(ip);
  if (ip & MM_IRQ_UARTRX)
    {
      *ch = (char) uartread(MM_UART_RXTX);
      lm32_interrupt_ack(MM_IRQ_UARTRX);
      return true;
    }
  *ch = '0';
  return false;
}
