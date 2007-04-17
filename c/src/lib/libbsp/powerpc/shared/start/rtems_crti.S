/* rtems_crti.S */
#include <rtems/asm.h>
#include <rtems/score/cpu.h>

	/* terminate the __init() function and create
	 * a new head '_init' for use by RTEMS to
	 * invoke C++ global constructors
	 * NOTE: it is essential that this snippet
	 * is hooked between ecrti and crtbegin
	 *
	 * ecrti has the following .init section:
	 * __init:
	 *    stwu r1,-16(r1)
	 *    mflr r0
	 *    stw  r0,20(r1)
	 *
	 * The reason for this is that we want to call
	 * __eabi() at an early stage but prevent __eabi()
	 * from branching to __init (C++ exception init
	 * and global CTORs). Hence we make __init a no-op
	 * and create a new entry point:
	 */
	.section ".init","ax"
	.align 2
	lwz		r0,r20(r1)
	mtlr	r0
	addi	r1,r1,16
	blr
	.globl	_init
	.type	_init,@function
_init:
	stwu	r1,-16(r1)
	mflr	r0
	stw		r0,20(r1)
