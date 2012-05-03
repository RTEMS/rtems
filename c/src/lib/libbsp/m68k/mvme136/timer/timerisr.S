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
 */

#include <rtems/asm.h>

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
