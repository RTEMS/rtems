/*  timer_isr()
 *
 *  This routine provides the ISR for the timer.   The timer is set up 
 *  to generate an interrupt at maximum intervals.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include "asm.h"

        BEGIN_CODE

        EXTERN(_Ttimer_val)

        PUBLIC(timerisr)

SYM (timerisr): 
        addl    $1,_Ttimer_val   # another tick
        push    edx
        push    eax
        movw    $0x20,dx
        mov     edx,eax
        outb    al,(dx) # touch interrupt controller
        pop     eax
        pop     edx
        iret

END_CODE
END
