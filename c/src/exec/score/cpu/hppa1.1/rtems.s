/*  rtems.S
 *
 *  This file contains the single entry point code for
 *  the HPPA implementation of RTEMS.
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <rtems/score/hppa.h>
#include <rtems/score/cpu_asm.h>

	.SPACE $PRIVATE$
	.SUBSPA $DATA$,QUAD=1,ALIGN=8,ACCESS=31
	.SUBSPA $BSS$,QUAD=1,ALIGN=8,ACCESS=31,ZERO,SORT=82
	.SPACE $TEXT$
	.SUBSPA $LIT$,QUAD=0,ALIGN=8,ACCESS=44
	.SUBSPA $CODE$,QUAD=0,ALIGN=8,ACCESS=44,CODE_ONLY
	.SPACE $TEXT$
	.SUBSPA $CODE$

	.align 32
	.EXPORT cpu_jump_to_directive,ENTRY,PRIV_LEV=0
cpu_jump_to_directive
	.PROC
	.CALLINFO FRAME=0,NO_CALLS
	.ENTRY

# invoke user interrupt handler

# XXX: look at register usage and code
# XXX: this is not necessarily right!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# r9 = directive number

        .import   _Entry_points,data
	ldil	  L%_Entry_points,%r8
	ldo	  R%_Entry_points(%r8),%r8
        ldwx,s    %r9(%r8),%r8

        .call  ARGW0=GR
        bv,n      0(%r8)
        nop

        .EXIT
        .PROCEND

