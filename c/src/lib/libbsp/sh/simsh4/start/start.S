/*
 * start.S -- Initialization code for SH4 simulator BSP
 *
 * Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 * Author: Victor V. Vengerov <vvv@oktet.ru>
 *
 * Based on work:
 *  Authors: Ralf Corsepius (corsepiu@faw.uni-ulm.de) and
 *           Bernd Becker (becker@faw.uni-ulm.de)
 *
 *  COPYRIGHT (c) 1997-1998, FAW Ulm, Germany
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  Modified to reflect Hitachi EDK SH7045F:
 *  John M. Mills (jmills@tga.com)
 *  TGA Technologies, Inc.
 *  100 Pinnacle Way, Suite 140
 *  Norcross, GA 30071 U.S.A.
 *  
 *
 *  This modified file may be copied and distributed in accordance
 *  the above-referenced license. It is provided for critique and
 *  developmental purposes without any warranty nor representation
 *  by the authors or by TGA Technologies.
 *
 *  COPYRIGHT (c) 1999-2001.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include "asm.h"
#include "rtems/score/sh4_regs.h"

	BEGIN_CODE
	PUBLIC(start)

SYM (start):
        ! install the stack pointer
	mov.l	stack_k,r15

        mov.l   initial_sr_k,r0
        ldc     r0,ssr
        ldc     r0,sr

        ! Set up VBR register
        mov.l   _vbr_base_k,r0
        ldc     r0,vbr


#ifdef	START_HW_INIT	/* from $RTEMS_BSP.cfg */
	! Initialize minimal hardware
	mov.l	hw_init_k, r0
	jsr @r0
	nop		!dead slot
#endif /* START_HW_INIT */

	! zero out bss
	mov.l	edata_k,r0
	mov.l	end_k,r1
	mov	#0,r2
0:
	mov.l	r2,@r0
	add	#4,r0
	cmp/ge	r0,r1
	bt	0b

        ! initialise fpscr for gcc
        mov.l set_fpscr_k, r1
        jsr @r1
        nop

        ! Set FPSCR register
        mov.l   initial_fpscr_k,r0
        lds     r0,fpscr

	! call the mainline	
	mov #0,r4		! argc
	mov.l main_k,r0
	jsr @r0
	mov #0,r5		! argv - can place in dead slot

	! call exit
	mov	r0,r4
	mov.l	exit_k,r0
	jsr	@r0
	or	r0,r0

        ! stop the simulator
        trapa   #2
	nop

	.global	___trap34
___trap34:
	trapa	#34
!	tst	r1,r1	! r1 is errno
!	bt	ret
!	mov.l	perrno,r2
!	mov.l	r1,@r2
!ret:
	rts
	nop
	
!	.align	2
!perrno:
!	.long	_errno


	END_CODE

	.align 2
set_fpscr_k:
    .long   ___set_fpscr
_vbr_base_k:
        .long   SYM(_vbr_base)

stack_k:
	.long	SYM(stack)	
edata_k:
	.long	SYM(edata)
end_k:
	.long	SYM(end)
main_k:
	.long	SYM(boot_card)
exit_k:
	.long	SYM(exit)

#ifdef	START_HW_INIT	/* from $RTEMS_BSP.cfg */
hw_init_k:
	.long	SYM(early_hw_init)
#endif /* START_HW_INIT */

vects_k:
	.long	SYM(vectab)
vects_size:
	.word	255

    .align 2
initial_sr_k:
        .long   SH4_SR_MD | SH4_SR_RB | SH4_SR_IMASK
initial_fpscr_k:
#ifdef __SH4__
        .long   SH4_FPSCR_DN | SH4_FPSCR_PR | SH4_FPSCR_RM
#else
        .long   SH4_FPSCR_DN | SH4_FPSCR_RM
#endif

#ifdef __ELF__
	.section .stack,"aw"
#else
	.section .stack
#endif
SYM(stack):
	.long	0xdeaddead

#ifdef __ELF__
	.section .bss,"aw"
#else
	.section .bss
#endif

	.global __sh4sim_dummy_register
__sh4sim_dummy_register:
	.long	0
