/*
 *  This file contains definitions for LatticeMico32 UART
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

#ifndef _BSPUART_H
#define _BSPUART_H

void BSP_uart_init(int baud);

/* Receive buffer register / transmit holding register */

#define LM32_UART_RBR      (0x0000)

/* Interrupt enable register */

#define LM32_UART_IER      (0x0004)
#define LM32_UART_IER_RBRI (0x0001)
#define LM32_UART_IER_THRI (0x0002)
#define LM32_UART_IER_RLSI (0x0004)
#define LM32_UART_IER_MSI  (0x0008)

/* Interrupt identification register */

#define LM32_UART_IIR      (0x0008)
#define LM32_UART_IIR_STAT (0x0001)
#define LM32_UART_IIR_ID0  (0x0002)
#define LM32_UART_IIR_ID1  (0x0004)

/* Line control register */

#define LM32_UART_LCR      (0x000C)
#define LM32_UART_LCR_WLS0 (0x0001)
#define LM32_UART_LCR_WLS1 (0x0002)
#define LM32_UART_LCR_STB  (0x0004)
#define LM32_UART_LCR_PEN  (0x0008)
#define LM32_UART_LCR_EPS  (0x0010)
#define LM32_UART_LCR_SP   (0x0020)
#define LM32_UART_LCR_SB   (0x0040)
#define LM32_UART_LCR_5BIT (0)
#define LM32_UART_LCR_6BIT (LM32_UART_LCR_WLS0)
#define LM32_UART_LCR_7BIT (LM32_UART_LCR_WLS1)
#define LM32_UART_LCR_8BIT (LM32_UART_LCR_WLS1 | LM32_UART_LCR_WLS0)

/* Modem control register */

#define LM32_UART_MCR      (0x0010)
#define LM32_UART_MCR_DTR  (0x0001)
#define LM32_UART_MCR_RTS  (0x0002)

/* Line status register */

#define LM32_UART_LSR      (0x0014)
#define LM32_UART_LSR_DR   (0x0001)
#define LM32_UART_LSR_OE   (0x0002)
#define LM32_UART_LSR_PE   (0x0004)
#define LM32_UART_LSR_FE   (0x0008)
#define LM32_UART_LSR_BI   (0x0010)
#define LM32_UART_LSR_THRE (0x0020)
#define LM32_UART_LSR_TEMT (0x0040)

/* Modem status register */

#define LM32_UART_MSR      (0x0018)
#define LM32_UART_MSR_DCTS (0x0001)
#define LM32_UART_MSR_DDSR (0x0002)
#define LM32_UART_MSR_TERI (0x0004)
#define LM32_UART_MSR_DDCD (0x0008)
#define LM32_UART_MSR_CTS  (0x0010)
#define LM32_UART_MSR_DSR  (0x0020)
#define LM32_UART_MSR_RI   (0x0040)
#define LM32_UART_MSR_DCD  (0x0000)

/* Baud-rate divisor register */

#define LM32_UART_DIV      (0x001C)

#endif /* _BSPUART_H */
