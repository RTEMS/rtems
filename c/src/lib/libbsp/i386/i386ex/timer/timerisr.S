/*  timer_isr()
 *
 *  This routine provides the ISR for the Z8036 timer on the MVME136
 *  board.   The timer is set up to generate an interrupt at maximum
 *  intervals.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
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

#include "asm.h"

        BEGIN_CODE

        EXTERN (Ttimer_val)

        PUBLIC (timerisr)
SYM (timerisr):
        addl    $250, SYM (Ttimer_val)   # another 250 microseconds
	pushl	eax
	movb	0xa0,al		/* signal generic End Of Interrupt (EOI) to slave PIC */
	outb	al, $0x20
	movb	$0x20, al
	outb	al, $0x20	/* signal generic EOI to Master PIC */
	popl	eax
        iret

END_CODE
END
