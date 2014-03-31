/*
 *  Idle Thread Body
 *
 *  This routine puts LEON3 in power-down mode.
 *
 *  COPYRIGHT (c) 2004.
 *  Gaisler Research.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */



#include <rtems/asm.h>

/* LEON specific power-down function */

        .align 4
        PUBLIC(bsp_idle_thread)
        PUBLIC(leon3_power_down_loop)
SYM(bsp_idle_thread):
SYM(leon3_power_down_loop):
pwdloop:  mov   %g0, %asr19
          lda   [%sp] 1, %g0            ! Needed for UT699 and GR712
          ba,a    pwdloop
           nop
