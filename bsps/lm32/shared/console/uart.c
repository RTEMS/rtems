/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  Uart driver for Lattice Mico32 (lm32) UART
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
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
 *
 *  Jukka Pietarinen <jukka.pietarinen@mrf.fi>, 2008,
 *  Micro-Research Finland Oy
 */

#include "../include/system_conf.h"
#include "uart.h"
#include <bsp.h>
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

int BSP_uart_polled_read( void )
{
  /* Wait until there is a byte in RBR */
  while (!(uartread(LM32_UART_LSR) & LM32_UART_LSR_DR));
  return (int) uartread(LM32_UART_RBR);
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
