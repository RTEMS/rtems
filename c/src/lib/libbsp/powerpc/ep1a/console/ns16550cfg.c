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
        unsigned char reg;
        unsigned char pad[7];
} uartReg;

unsigned8 Read_ns16550_register(
  unsigned32  ulCtrlPort,
  unsigned8   ucRegNum
)
{
  struct uart_reg *p = (struct uart_reg *)ulCtrlPort;
  unsigned8  ucData;
  ucData = p[ucRegNum].reg;
  asm volatile("sync");
  return ucData;
}

void  Write_ns16550_register(
  unsigned32  ulCtrlPort,
  unsigned8   ucRegNum,
  unsigned8   ucData
)
{
  struct uart_reg *p = (struct uart_reg *)ulCtrlPort;
  volatile int i;
  p[ucRegNum].reg = ucData; 
  asm volatile("sync");
  for (i=0;i<0x08ff;i++);
}
