/*  timer_isr()
 *
 *  This routine initializes the Z8536 timer on the SQSIO4 SQUALL
 *  board for the CVME961 board.  The timer is setup to provide a
 *  tick every 0x10000 / 2 milliseconds.  This is used to time
 *  executing code.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include "asm.h"

.set PORT_A,     0xc00000a8                 # port A
.set PORT_B,     0xc00000a4                 # port B
.set PORT_C,     0xc00000a0                 # port C
.set CTL_PORT,   0xc00000ac                 # control port

.set T1CSR,      0x0a                       # T1 command/status reg
.set RELOAD,     0x24                       # clr IP & IUS,allow countdown

/*
 * Duplicating this symbol is stupid but eliminates
 * toolset variation problems.
 */
        PUBLIC(timerisr)
        PUBLIC(_timerisr)
SYM (timerisr):
SYM (_timerisr):
        #ldconst   1,r4
        #modpc     0,r4,r4               # enable tracing

        ld      _Ttimer_val,r6           # r6 = test timer

        ldconst T1CSR,r4                 # r4 = T1 control status reg
        stob    r4,CTL_PORT              # select T1CSR
        ldconst RELOAD,r5                # r5 = reset value
        stob    r5,CTL_PORT              # reset countdown
        addo    1,r6,r6
        st      r6,_Ttimer_val           # increment test timer
loop_til_cleared:
        /* clrbit 4,sf0,sf0 XXX JRS */
        /* bbs    4,sf0,loop_til_cleared XXX JRS */
leaf:   ret

        .leafproc _flush_reg, flush_reg.lf
        .globl    _flush_reg, flush_reg.lf
_flush_reg:
        lda     leaf,g14                 # g14 = exit address
flush_reg.lf:
        flushreg
        mov     g14,g0                   # g0 = exit address
        ldconst 0,g14                    # set g14 for non-leaf
        bx      (g0)
