/*
 *  boardinit.s
 *
 *  Start code for the LEON.
 *
 *  $Id$
 */

#include <asm.h>
#include <leon.h>

	.global __bsp_board_init

__bsp_board_init:

/* Check if LEON is initialised. If not, this means that we are
   running on the simulator. Initiate some of the parameters
   that are done by the boot-prom otherwise.
*/

        set     SYM(LEON_REG), %l0  	! %l0 = base address of peripherals
	ld	[%l0+LEON_REG_CACHECTRL_OFFSET], %l1 ! Check if LEON has been initialised
	andcc	%l1, 3, %g0		
	bne	2f
	nop


	flush
	set	0x1000f, %l1
	st      %l1, [%l0 + LEON_REG_CACHECTRL_OFFSET]     ! enable caches
	ld      [%l0 + LEON_REG_SIM_ROM_SIZE_OFFSET], %g1  ! load simulator rom size
	clr	%l2
	srl	%g1, 13, %g1		! calculate appropriate rom size
1:
	srl	%g1, 1, %g1
	tst	%g1
	bne,a	1b
	inc	%l2
	sll	%l2, 14, %l2
	st	%l2, [%l0 + LEON_REG_MEMCFG1_OFFSET] ! set prom size in memcfg1

	set	0, %l2
	ld      [%l0 + LEON_REG_SIM_RAM_SIZE_OFFSET], %g2 ! load simulator ram size
	srl	%g2, 13, %g1		! calculate appropriate ram size
1:
	srl	%g1, 1, %g1
	tst	%g1
	bne,a	1b
	inc	%l2
	sll	%l2, 9, %l2
	st	%l2, [%l0 + LEON_REG_MEMCFG2_OFFSET] ! set ram size in memcfg2

	set	SYM(RAM_START), %l1  ! Cannot use RAM_END due to bug in linker
	set	SYM(RAM_SIZE), %l2
	add	%l1, %l2, %sp
	set	49, %l2
	st	%l2, [%l0 + LEON_REG_SCALER_LOAD_OFFSET] ! scaler reload register
	st	%l2, [%l0 + LEON_REG_SCALERCNT_OFFSET] ! scaler counter register

2:
	retl
	nop

/* end of file */
