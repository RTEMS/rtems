/*-------------------------------------------------------------------------+
| timerisr.s v1.1 - PC386 BSP - 1997/08/07
+--------------------------------------------------------------------------+
| This file contains the PC386 timer interrupt handler.
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
| This code is base on:
|   timerisr.s,v 1.5 1995/12/19 20:07:45 joel Exp - go32 BSP
| With the following copyright notice:
| **************************************************************************
| *  COPYRIGHT (c) 1989-1999.
| *  On-Line Applications Research Corporation (OAR).
| *
| *  The license and distribution terms for this file may be
| *  found in the file LICENSE in this distribution or at
| *  http://www.rtems.com/license/LICENSE.
| **************************************************************************
+--------------------------------------------------------------------------*/

#include <rtems/asm.h>

BEGIN_CODE

	EXTERN(Ttimer_val)

/*-------------------------------------------------------------------------+
|         Function: rtems_isr timerisr(rtems_vector_number);
|      Description: ISR for the timer. The timer is set up to generate an
|                   interrupt at maximum intervals.
| Global Variables: None.
|        Arguments: standard - see RTEMS documentation.
|          Returns: standard return value - see RTEMS documentation.
+--------------------------------------------------------------------------*/
	PUBLIC(timerisr)
SYM (timerisr):
	incl	Ttimer_val	# another tick
	pushl	eax
	movb	$0x20, al
	outb	al, $0x20	# signal generic End Of Interrupt (EOI) to PIC
	popl	eax
	iret

END_CODE

END
