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
