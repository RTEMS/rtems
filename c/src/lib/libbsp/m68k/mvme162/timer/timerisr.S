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
 *  Modifications of respective RTEMS file: COPYRIGHT (c) 1994.
 *  EISCAT Scientific Association. M.Savitski
 *
 *  This material is a part of the MVME162 Board Support Package
 *  for the RTEMS executive. Its licensing policies are those of the
 *  RTEMS above.
 */

#include <rtems/asm.h>

BEGIN_CODE

.set INTR_CLEAR_REG,      0xfff40074      | interrupt clear register
.set RELOAD,              0x01000000      | clear tick 1 interrupt

        PUBLIC (Ttimer_val)
        PUBLIC (timerisr)
SYM (timerisr):
        move.l  a0, -(a7)                 | save a0
        movea.l #INTR_CLEAR_REG, a0       | a0 = addr of cmd status reg
        ori.l   #RELOAD, (a0)             | reload countdown
        addq.l  #1, SYM (Ttimer_val)      | increment timer value
        move.l  (a7)+, a0                 | restore a0
        rte

END_CODE
END
