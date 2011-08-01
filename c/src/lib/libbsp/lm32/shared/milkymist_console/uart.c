/*
 *  Driver for Milkymist UART
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 *
 *  COPYRIGHT (c) 2010 Sebastien Bourdeauducq
 *  COPYRIGHT (c) Yann Sionneau <yann.sionneau@telecom-sudparis.eu> (GSoC 2010)
 *  Telecom SudParis
 */

#include <rtems.h>
#include <rtems/libio.h>
#include "../include/system_conf.h"
#include "uart.h"

bool BSP_uart_txbusy;

void BSP_uart_init(int baud)
{
  MM_WRITE(MM_UART_DIV, CPU_FREQUENCY/baud/16);
}

void BSP_uart_polled_write(char ch)
{
  int ip;
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
  if (BSP_uart_txbusy) {
    /* wait for the end of the transmission by the IRQ-based driver */
    do {
      lm32_read_interrupts(ip);
    } while (!(ip & (1 << MM_IRQ_UARTTX)));
    lm32_interrupt_ack(1 << MM_IRQ_UARTTX);
  }
  MM_WRITE(MM_UART_RXTX, ch);
  do {
    lm32_read_interrupts(ip);
  } while (!(ip & (1 << MM_IRQ_UARTTX)));
  /* if TX was busy, do not ack the IRQ
   * so that the IRQ-based driver ISR is run */
  if (!BSP_uart_txbusy)
    lm32_interrupt_ack(1 << MM_IRQ_UARTTX);
  rtems_interrupt_enable(level);
}

int BSP_uart_polled_read(void)
{
  int ip;
  char r;
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
  do {
    lm32_read_interrupts(ip);
  } while (!(ip & (1 << MM_IRQ_UARTRX)));
  lm32_interrupt_ack(1 << MM_IRQ_UARTRX);
  r = MM_READ(MM_UART_RXTX);
  rtems_interrupt_enable(level);

  return r;
}
