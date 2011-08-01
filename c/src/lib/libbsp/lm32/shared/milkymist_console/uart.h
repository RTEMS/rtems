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

extern bool BSP_uart_txbusy;

void BSP_uart_init(int baud);
void BSP_uart_polled_write(char ch);
int BSP_uart_polled_read(void);

#endif /* _BSPUART_H */
