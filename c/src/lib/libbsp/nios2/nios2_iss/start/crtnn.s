/*  NIOS2 crtn.asm fix
 *
 *  COPYRIGHT (c) 2005-2006 Kolja Waschk rtemsdev/ixo.de
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

/* Can't use the original crtn.asm from Altera because it doesn't
   restore the stack pointer correctly (in 5.1b73, the stack pointer
   is further decreased by 48 instead of increased). This is named
   differently (crtnn instead crtn) to make sure it can be picked up
   using custom gcc specs instead of gcc's crtn. - kawk */

	.section .init
	ldw	ra, 44(sp)
	ldw	r23, 40(sp)
	ldw	r22, 36(sp)
	ldw	r21, 32(sp)
	ldw	r20, 28(sp)
	ldw	r19, 24(sp)
	ldw	r18, 20(sp)
	ldw	r17, 16(sp)
	ldw	r16, 12(sp)
	ldw	fp, 8(sp)
	addi sp, sp, 48
	ret
	
	.section .fini
	ldw	ra, 44(sp)
	ldw	r23, 40(sp)
	ldw	r22, 36(sp)
	ldw	r21, 32(sp)
	ldw	r20, 28(sp)
	ldw	r19, 24(sp)
	ldw	r18, 20(sp)
	ldw	r17, 16(sp)
	ldw	r16, 12(sp)
	ldw	fp, 8(sp)
	addi sp, sp, 48
	ret
	
