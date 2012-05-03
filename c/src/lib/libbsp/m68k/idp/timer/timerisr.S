/*  timer_isr()
 *
 *  This routine provides the ISR for the MC68230 timer on the Motorola
 *  IDP board.   The timer is set up to generate an interrupt at maximum
 *  intervals.
 *
 *  Code modified by Doug McBride, Colorado Space Grant College
 *  countdown should be loaded automatically
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
 */

#include <rtems/asm.h>

BEGIN_CODE

.set TSR,        0x00c0106B         	 | base address of PIT register "TSR"

        PUBLIC (timerisr)
SYM (timerisr):
        movb    #1,TSR				 	       | acknowledge interrupt
        addql   #1, SYM (Ttimer_val)   | increment timer value
        rte

END_CODE
END
