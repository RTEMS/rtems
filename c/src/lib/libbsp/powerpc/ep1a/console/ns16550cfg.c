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

uint8_t Read_ns16550_register(
  uint32_t  ulCtrlPort,
  uint8_t   ucRegNum
)
{
volatile struct uart_reg *p = (volatile struct uart_reg *)ulCtrlPort;
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
  volatile struct uart_reg *p = (volatile struct uart_reg *)ulCtrlPort;
  volatile int i;
  p[ucRegNum].reg = ucData; 
  asm volatile("sync");
  asm volatile("isync");
  asm volatile("eieio");
  for (i=0;i<0x08ff;i++)
    asm volatile("isync");
}
