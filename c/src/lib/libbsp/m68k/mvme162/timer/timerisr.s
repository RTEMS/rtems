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
 *  Modifications of respective RTEMS file: COPYRIGHT (c) 1994.
 *  EISCAT Scientific Association. M.Savitski
 *
 *  This material is a part of the MVME162 Board Support Package
 *  for the RTEMS executive. Its licensing policies are those of the
 *  RTEMS above.
 *
 *  $Id$
 */

#include "asm.h"

BEGIN_CODE

.set INTR_CLEAR_REG,      0xfff40074      | interrupt clear register
.set RELOAD,              0x01000000      | clear tick 1 interrupt

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
