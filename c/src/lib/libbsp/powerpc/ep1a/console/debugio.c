/*
 *  This file contains the debug IO support.
 *
 *  COPYRIGHT (c) 1998 by Radstone Technology
 *
 *
 * THIS FILE IS PROVIDED TO YOU, THE USER, "AS IS", WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE. THE ENTIRE RISK
 * AS TO THE QUALITY AND PERFORMANCE OF ALL CODE IN THIS FILE IS WITH YOU.
 *
 * You are hereby granted permission to use, copy, modify, and distribute
 * this file, provided that this notice, plus the above copyright notice
 * and disclaimer, appears in all copies. Radstone Technology will provide
 * no support for this code.
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  debugio.c,v 1.2.4.1 2003/09/04 18:45:11 joel Exp
 */

#include <bsp.h>
#include <rtems/libio.h>
#include <stdlib.h>
#include <assert.h>
#include <termios.h>

#include <libchip/serial.h>

/*
 * Load configuration table
 */

extern console_data  Console_Port_Data[];
extern rtems_device_minor_number  Console_Port_Minor;
 
/* PAGE
 *
 *  DEBUG_puts
 *
 *  This should be safe in the event of an error.  It attempts to ensure
 *  that no TX empty interrupts occur while it is doing polled IO.  Then
 *  it restores the state of that external interrupt.
 *
 *  Input parameters:
 *    string  - pointer to debug output string
 *
 *  Output parameters:  NONE
 *
 *  Return values:      NONE
 */

void DEBUG_puts(
  char *string
)
{
  char *s;
  unsigned32  Irql;

  rtems_interrupt_disable(Irql);

  for ( s = string ; *s ; s++ ) {
    Console_Port_Tbl[Console_Port_Minor].pDeviceFns->
      deviceWritePolled(Console_Port_Minor, *s);
  }

  rtems_interrupt_enable(Irql);
}

/* PAGE
 *
 *  DEBUG_puth
 *
 *  This should be safe in the event of an error.  It attempts to ensure
 *  that no TX empty interrupts occur while it is doing polled IO.  Then
 *  it restores the state of that external interrupt.
 *
 *  Input parameters:
 *    ulHexNum - value to display
 *
 *  Output parameters:  NONE
 *
 *  Return values:      NONE
 */

void DEBUG_puth(
  unsigned32 ulHexNum
)
{
  unsigned long  i,d;
  unsigned32      Irql;
  void          (*poll)(int minor, char cChar);
  
  poll = Console_Port_Tbl[Console_Port_Minor].pDeviceFns->deviceWritePolled;

  rtems_interrupt_disable(Irql);

    (*poll)(Console_Port_Minor, '0');
    (*poll)(Console_Port_Minor, 'x');

    for ( i=32 ; i ; ) {
      i -= 4;
      d  = (ulHexNum>>i)&0xf;
      (*poll)(Console_Port_Minor, (d<=9) ? d+'0' : d+'a'-0xa);
    }
  rtems_interrupt_enable(Irql);
}

