/*-------------------------------------------------------------------------+
| inch.c v1.1 - PC386 BSP - 1997/08/07
+--------------------------------------------------------------------------+
| (C) Copyright 1997 -
| - NavIST Group - Real-Time Distributed Systems and Industrial Automation
|
| http://pandora.ist.utl.pt
|
| Instituto Superior Tecnico * Lisboa * PORTUGAL
+--------------------------------------------------------------------------+
| Disclaimer:
|
| This file is provided "AS IS" without warranty of any kind, either
| expressed or implied.
+--------------------------------------------------------------------------+
| This code is based on:
|   inch.c,v 1.3 1995/12/19 20:07:25 joel Exp - go32 BSP
| With the following copyright notice:
| With the following copyright notice:
| **************************************************************************
| *  COPYRIGHT (c) 1989-1998.
| *  On-Line Applications Research Corporation (OAR).
| *  Copyright assigned to U.S. Government, 1994. 
| *
| *  The license and distribution terms for this file may be
| *  found in found in the file LICENSE in this distribution or at
| *  http://www.OARcorp.com/rtems/license.html.
| **************************************************************************
|
|  $Id$
+--------------------------------------------------------------------------*/

#include <bsp.h>
#include <irq.h>

/*-------------------------------------------------------------------------+
| Constants
+--------------------------------------------------------------------------*/
#define KBD_BUF_SIZE    256

static unsigned short   kbd_buffer[KBD_BUF_SIZE];
static rtems_unsigned16 kbd_first = 0;
static rtems_unsigned16 kbd_last  = 0;
static rtems_unsigned16 kbd_end   = KBD_BUF_SIZE - 1;

/*-------------------------------------------------------------------------+
|         Function: rtemsReboot
|      Description: Reboot the PC.
| Global Variables: None.
|        Arguments: None.
|          Returns: Nothing.
+--------------------------------------------------------------------------*/
void rtemsReboot(void)
{
  /* shutdown and reboot */
  outport_byte(0x64, 0xFE);      /* use keyboard controler to do the job... */
} /* rtemsReboot */



#define disable __asm__ __volatile__("cli")
#define enable  __asm__ __volatile__("sti");
/*
 * Check if a key has been pressed. This is a non-destructive
 * call, meaning, it keeps the key in the buffer.
 */
int rtems_kbpoll( void )
{
  int rc;
  disable;
  rc = ( kbd_first != kbd_last ) ? TRUE : FALSE;
  enable;
  return rc;
}

int getch( void )
{
  int c;
  while( kbd_first == kbd_last )
  {
     rtems_task_wake_after( 10 );
  }
  c = kbd_buffer[ kbd_first ];
  kbd_first = (kbd_first + 1) % KBD_BUF_SIZE;
  return c;
}

void add_to_queue( unsigned short b )
{
 unsigned int next;
 kbd_buffer[ kbd_last ] = b;
 next = (kbd_last == kbd_end) ? 0 : kbd_last + 1;
 if( next != kbd_first )
 {
    kbd_last = next;
 }
}

