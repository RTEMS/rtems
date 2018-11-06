/*
 * Moxie CPU functions
 *   Copyright (C) 2011 Anthony Green
 *
 *  Based on example code and other ports with this copyright:
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/asm.h>

        .text
        .align 2

        .global SYM(_CPU_Context_switch)

SYM(_CPU_Context_switch):
        sto.l   0($r0), $fp
        sto.l   4($r0), $sp
        sto.l   8($r0), $r0
        sto.l   12($r0), $r1
        sto.l   16($r0), $r2
        sto.l   20($r0), $r3
        sto.l   24($r0), $r4
        sto.l   28($r0), $r5
        sto.l   32($r0), $r6
        sto.l   36($r0), $r7
        sto.l   40($r0), $r8
        sto.l   44($r0), $r9
        sto.l   48($r0), $r10
        sto.l   52($r0), $r11
        sto.l   56($r0), $r12
        sto.l   60($r0), $r13

restore:
        ldo.l   $fp, 0($r1)
        ldo.l   $sp, 4($r1)
        ldo.l   $r0, 8($r1)
        ldo.l   $r2, 16($r1)
        ldo.l   $r3, 20($r1)
        ldo.l   $r4, 24($r1)
        ldo.l   $r5, 28($r1)
        ldo.l   $r6, 32($r1)
        ldo.l   $r7, 36($r1)
        ldo.l   $r8, 40($r1)
        ldo.l   $r9, 44($r1)
        ldo.l   $r10, 48($r1)
        ldo.l   $r11, 52($r1)
        ldo.l   $r12, 56($r1)
        ldo.l   $r13, 60($r1)
        ldo.l   $r1, 12($r1)

        ret

        .align 2

        .global SYM(_CPU_Context_restore)

SYM(_CPU_Context_restore):
        mov     $r1, $r0
        jmpa    restore


/*
        VHandler for Vectored Interrupts

        All IRQ's are vectored to routine _ISR_#vector_number
        This routine stacks er0 and loads er0 with vector number
        before transferring to here

*/
        .align 2
        .global SYM(_ISR_Handler)
        .extern SYM(_Vector_table)


SYM(_ISR_Handler):
        brk


        .align 2
        .global SYM(_CPU_Context_save_fp)

SYM(_CPU_Context_save_fp):
        brk


        .align 2
        .global SYM(_CPU_Context_restore_fp)

SYM(_CPU_Context_restore_fp):
        brk

