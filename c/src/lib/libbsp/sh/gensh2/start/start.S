/*
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
 *  COPYRIGHT (c) 1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems/asm.h>

	BEGIN_CODE
	PUBLIC(start)

SYM (start):
        ! install the stack pointer
	mov.l	stack_k,r15

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

	! copy the vector table from rom to ram
	mov.l   vects_k,r0   	! vectab
	mov	#0,r1		! address of boot vector table
	mov     #0,r2		! number of bytes copied
	mov.w   vects_size,r3   ! size of entries in vectab
1:
	mov.l   @r1+,r4
	mov.l   r4,@r0
	add     #4,r0
	add     #1,r2
	cmp/hi  r3,r2
	bf      1b

#ifndef STANDALONE_EVB
	! overlay monitor vectors onto RTEMS table template
	! code adapted  from Hitachi EDK7045F User Manual: "Copyvect.s"
	mova	vects_k,r0
	mov.l	@r0, r1	! Shadow vect tbl addr
	stc		vbr, r2	! Original vect tbl addr
	and		#0, r0
	mov		r0, r4	! 0 in r4 and r0

!trapa #32
	or		#0x80, r0
	mov.l	@(r0,r2), r3
	mov.l	r3, @(r0, r1)
	mov		r4, r0

!cpu addr err
	or		#0x24, r0
	mov.l	@(r0, r2), r3
	mov.l	r3, @(r0, r1)
	mov		r4, r0

!ill slot
	or		#0x18, r0
	mov.l	@(r0, r2), r3
	mov.l	r3, @(r0, r1)
	mov		r4, r0

!ill inst
	or		#0x10, r0
	mov.l	@(r0, r2), r3
	mov.l	r3, @(r0, r1)
	mov		r4, r0

!nmi
	or		#0x2c, r0
	mov.l	@(r0, r2), r3
	mov.l	r3, @(r0, r1)
	mov		r4, r0

!User brk
	or		#0x30, r0
	mov.l	@(r0, r2), r3
	mov.l	r3, @(r0, r1)
	mov		r4, r0

!sci0 err
	or		#0x80, r0
	rotl	r0
	rotl	r0
	mov.l	@(r0, r2), r3
	mov.l	r3, @(r0, r1)
	mov		r4, r0
!sci rx
	or		#0x81, r0
	rotl	r0
	rotl	r0
	mov.l	@(r0, r2), r3
	mov.l	r3, @(r0, r1)

	stc	vbr,r3		! capture copy of monitor vbr
	mov.l	vbrtemp_k,r0
	mov.l	r3, @r0
	mov.l   vects_k,r0   	! point vbr to vectab
	ldc     r0,vbr
#endif /* ! STANDALONE_EVB */

	! call the mainline
	mov #0,r4		! command line
	mov.l main_k,r0
	jsr @r0
	nop

	! call exit
	mov	r0,r4
	mov.l	exit_k,r0
	jsr	@r0
	or	r0,r0

	mov.l	vbrtemp_k,r0	! restore original vbr
	mov.l	@r0,r3
	ldc	r3, vbr
	trapa	#13		! UBR capture by monitor
	nop		!debug dead-slot target

	END_CODE

	.align 2
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
vbrtemp_k:
	.long	SYM(vbrtemp)
vects_k:
	.long	SYM(vectab)
vects_size:
	.word	255

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
SYM(vbrtemp):
	.long	0x0
