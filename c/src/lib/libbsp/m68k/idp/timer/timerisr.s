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
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#include "asm.h"

BEGIN_CODE

.set TSR,        0x00c0106B         	 | base address of PIT register "TSR"

        PUBLIC (timerisr)
SYM (timerisr):
        movb    #1,TSR				 	       | acknowledge interrupt
        addql   #1, SYM (Ttimer_val)   | increment timer value
        rte

END_CODE
END
