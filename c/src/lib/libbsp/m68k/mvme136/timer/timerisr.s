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

.set CT1_CMD_STATUS,  0xfffb000a         | port A
.set RELOAD,          0x24               | clr IP & IUS,allow countdown

        PUBLIC (timerisr)
SYM (timerisr):
        movl    a0,a7@-                  | save a0
        movl    #CT1_CMD_STATUS,a0       | a0 = addr of cmd status reg
        movb    #RELOAD,a0@              | reload countdown
        addql   #1, SYM (Ttimer_val)     | increment timer value
        movl    a7@+,a0                  | save a0
        rte

END_CODE
END
