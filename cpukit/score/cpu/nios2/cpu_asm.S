/*
 *  $Id$
 *
 *  This file contains all assembly code for the
 *  NIOS2 implementation of RTEMS.
 *
 *  Copyright (c) 2006 Kolja Waschk (rtemsdev/ixo.de)
 *
 *  Derived from no_cpu/cpu_asm.S, copyright (c) 1989-1999,
 *    On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/asm.h>
#include <rtems/score/cpu_asm.h>

    .set noat

/* ===================================================================== */

/*
 *  void _CPU_Context_switch( run_context, heir_context )
 *  void _CPU_Context_restore( run_context, heir_context )
 *
 *  This routine performs a normal non-FP context switch.
 */

    .globl _CPU_Context_switch

_CPU_Context_switch:

    rdctl r6, status
    stw r16,  0(r4)
    stw r17,  4(r4)
    stw r18,  8(r4)
    stw r19, 12(r4)
    stw r20, 16(r4)
    stw r21, 20(r4)
    stw r22, 24(r4)
    stw r23, 28(r4)
    stw fp,  32(r4)
    stw sp,  36(r4)
    stw ra,  40(r4)
    /* r6 saved status */
    stw r6,  44(r4)

_CPU_Context_switch_restore:

    ldw r16,  0(r5)
    ldw r17,  4(r5)
    ldw r18,  8(r5)
    ldw r19, 12(r5)
    ldw r20, 16(r5)
    ldw r21, 20(r5)
    ldw r22, 24(r5)
    ldw r23, 28(r5)
    ldw fp,  32(r5)
    ldw sp,  36(r5)

    /* Disable interrupts */
    wrctl status, r0

    ldw ea,  40(r5)
    ldw at,  44(r5)
    /* FIXME: Always have interrupts enabled when we return from Context_switch */
    ori at, at, 1
    wrctl estatus, at

    eret

    .globl _CPU_Context_restore

_CPU_Context_restore:

    /* Copy first to second arg, then re-use 2nd half of Context_switch */
    mov r5, r4
    br  _CPU_Context_switch_restore
