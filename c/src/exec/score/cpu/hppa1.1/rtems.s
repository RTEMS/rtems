/*  rtems.S
 *
 *  This file contains the single entry point code for
 *  the HPPA implementation of RTEMS.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  rtems.S,v 1.2 1995/05/09 20:11:41 joel Exp
 */

#include <rtems/hppa.h>
#include <rtems/cpu_asm.h>

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

