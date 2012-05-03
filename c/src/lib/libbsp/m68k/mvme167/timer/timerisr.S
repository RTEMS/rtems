/*  timer_isr()
 *
 *  This ISR is used to bump a count of interval "overflow" interrupts which
 *  have occurred since the timer was started. The number of overflows is taken
 *  into account in the benchmark_timer_read() routine.
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
 *  Copyright (c) 1998, National Research Council of Canada
 */

#include <rtems/asm.h>

BEGIN_CODE

.set INTR_CLEAR_REG,    0xfff40074      | interrupt clear register
.set T1_CNTRL_REG,      0xfff40060      | tick timer 1 control register
.set CLEAR_INT,         0x01000000      | clear tick 1 interrupt
.set CLEAR_OVF,         0x00000004      | clear tick 1 overflow counter

        PUBLIC (Ttimer_val)
        PUBLIC (timerisr)
SYM (timerisr):
        move.l  a0, -(a7)               | save a0
        move.l  d0, -(a7)               | save d0
        move.w  sr, -(a7)               | save ccr
        movea.l #INTR_CLEAR_REG, a0     | a0 = addr of intr clr reg
        ori.l   #CLEAR_INT, (a0)        | clear tick timer 1 intr
        movea.l #T1_CNTRL_REG, a0       | a0 = addr of t1 cntrl reg
        move.l  (a0), d0                | read overflow counter
        lsr.l   #4, d0                  | put overflow in low order bits
        andi.l  #0xF, d0                | keep only overflow
        add.l   d0, SYM (Ttimer_val)    | increment timer value
        ori.l   #CLEAR_OVF, (a0)        | clear overflow counter
        move.w  (a7)+, sr               | restore ccr
        move.l  (a7)+, d0               | restore d0
        move.l  (a7)+, a0               | restore a0
        rte

END_CODE
END
