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
 *  http://www.rtems.com/license/LICENSE.
 */



#include <rtems/asm.h>

/* LEON specific power-down function */

        .align 4
        PUBLIC(bsp_idle_thread)
SYM(bsp_idle_thread):
pwdloop:  mov   %g0, %asr19
          ba    pwdloop
          nop
          retl
          nop

