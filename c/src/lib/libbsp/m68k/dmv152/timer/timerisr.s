/*  timer_isr()
 *
 *  This routine provides the ISR for the Z8536 timer on the DMV152
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

.set TIMER,           0x0c000007         | port A
.set CT1_CMD_STATUS,  0x0a               | command status register
.set RELOAD,          0x26               | clr IP & IUS,allow countdown

        PUBLIC(timerisr)
SYM (timerisr):
        movb    #CT1_CMD_STATUS,TIMER    | set pointer to cmd status reg
        movb    #RELOAD,TIMER            | reload countdown
        addql   #1, SYM (Ttimer_val)     | increment timer value
        rte

END_CODE
END
