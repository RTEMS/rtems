/*===============================================================*\
| Project: RTEMS generic MPC83xx BSP                              |
+-----------------------------------------------------------------+
| This file has been adapted from the ep1a BSP to MPC83xx by      |
|    Thomas Doerfler <Thomas.Doerfler@embedded-brains.de>         |
|                    Copyright (c) 2007                           |
|                    Embedded Brains GmbH                         |
|                    Obere Lagerstr. 30                           |
|                    D-82178 Puchheim                             |
|                    Germany                                      |
|                    rtems@embedded-brains.de                     |
|                                                                 |
| See the other copyright notice below for the original parts.    |
+-----------------------------------------------------------------+
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
|                                                                 |
| http://www.rtems.com/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
| this file contains the DUART access routines                    |
\*===============================================================*/
/* derived from: */
/*
 *  This include file contains all console driver definations for the nc16550
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <rtems.h>
#include <bsp.h>
#include "console.h"
                                                            
typedef struct uart_reg
{
  volatile unsigned char reg;
} uartReg;

uint8_t Read_ns16550_register(
  uint32_t  ulCtrlPort,
  uint8_t   ucRegNum
)
{
  struct uart_reg *p = (struct uart_reg *)ulCtrlPort;
  uint8_t  ucData;
  ucData = p[ucRegNum].reg;  
  asm volatile("sync");
  return ucData;
}

void  Write_ns16550_register(
  uint32_t  ulCtrlPort,
  uint8_t   ucRegNum,
  uint8_t   ucData
)
{
  struct uart_reg *p = (struct uart_reg *)ulCtrlPort;
  p[ucRegNum].reg = ucData; 
  asm volatile("sync");
}
