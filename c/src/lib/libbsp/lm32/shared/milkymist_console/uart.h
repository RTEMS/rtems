/*
 *  This file contains definitions for LatticeMico32 UART
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

#ifndef _BSPUART_H
#define _BSPUART_H

void BSP_uart_init(int baud);

#define MM_UART_RXTX	(0xe0000000)

#define MM_UART_DIV     (0xe0000004)

#define MM_IRQ_UARTTX	(0x00000010) /* 4 */
#define MM_IRQ_UARTRX	(0x00000008) /* 3 */

#endif /* _BSPUART_H */
