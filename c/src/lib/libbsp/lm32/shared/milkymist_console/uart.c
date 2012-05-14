/*
 *  Driver for Milkymist UART
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  COPYRIGHT (c) 2010 Sebastien Bourdeauducq
 *  COPYRIGHT (c) Yann Sionneau <yann.sionneau@telecom-sudparis.eu> (GSoC 2010)
 *  Telecom SudParis
 */

#include <rtems.h>
#include <rtems/libio.h>
#include "../include/system_conf.h"
#include "uart.h"

void BSP_uart_init(int baud)
{
  MM_WRITE(MM_UART_DIV, MM_READ(MM_FREQUENCY)/baud/16);
}

void BSP_uart_polled_write(char ch)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
  while(!(MM_READ(MM_UART_STAT) & UART_STAT_THRE));
  MM_WRITE(MM_UART_RXTX, ch);
  while(!(MM_READ(MM_UART_STAT) & UART_STAT_THRE));
  rtems_interrupt_enable(level);
}

int BSP_uart_polled_read(void)
{
  char r;
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
  while(!(MM_READ(MM_UART_STAT) & UART_STAT_RX_EVT));
  r = MM_READ(MM_UART_RXTX);
  MM_WRITE(MM_UART_STAT, UART_STAT_RX_EVT);
  rtems_interrupt_enable(level);

  return r;
}
