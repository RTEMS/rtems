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
 *
 *  COPYRIGHT (c) 1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include "asm.h"

	BEGIN_CODE
	PUBLIC(start)
SYM (start):
        ! install the stack pointer
	mov.l	stack_k,r15

	! zero out bss
	mov.l	edata_k,r0
	mov.l	end_k,r1
	mov	#0,r2
start_l:
	mov.l	r2,@r0
	add	#4,r0
	cmp/ge	r0,r1
	bt	start_l

	! copy the vector table from rom to ram
	mov.l   vects_k,r7   	! vectab
	mov	#0,r2		! address of boot vector table
	mov     #0,r3		| number of bytes copied
	mov.w   vects_size,r6   ! size of entries in vectab
0:
	mov.l   @r2+,r1
	mov.l   r1,@r7
	add     #4,r7
	add     #1,r3
	cmp/hi  r6,r3
	bf      0b

	mov.l   vects_k,r8   	! update vbr to point to vectab
	ldc     r8,vbr

	! call the mainline	
	mov.l	main_k,r0
	jsr	@r0
	or	r0,r0

	! call exit
	mov	r0,r4
	mov.l	exit_k,r0
	jsr	@r0
	or	r0,r0

	END_CODE

	.align 2
stack_k:
	.long	_stack	
edata_k:
	.long	_edata
end_k:
	.long	_end
main_k:
	.long	_main
exit_k:
	.long	_exit

vects_k:
	.long	_vectab
vects_size:
	.word	255

#ifdef __ELF__
	.section .stack,"aw"
#else
	.section .stack
#endif
_stack:	.long	0xdeaddead
