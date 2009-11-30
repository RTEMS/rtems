/*
 *  systrap.S
 *
 *  This file contains emulated system calls using software trap 0.
 *  The following calls are supported:
 *
 *    + SYS_exit	(halt)
 *    + SYS_irqdis	(disable interrupts)
 *    + SYS_irqset	(set interrupt level)
 *
 *  COPYRIGHT:
 *
 *  COPYRIGHT (c) 1995. European Space Agency.
 *
 *  This terms of the RTEMS license apply to this file.
 *
 */

#include <rtems/asm.h>
#include "syscall.h"

        .seg    "text"
        /*
         *  system call
         *
         *  On entry:
         *
         *    l0 = psr (from trap table)
         *    l1 = pc
         *    l2 = npc
         *    g1 = system call id
         */

        PUBLIC(syscall)

SYM(syscall):

	subcc   %g1, 2, %g0		! syscall 2, disable interrupts
	bne 	3f
	or    	%l0, 0x0f00, %l4	! set PIL=15
	mov   	%l4, %psr
	or	%l0, SPARC_PSR_ET_MASK, %i0	! return old psr with ET=1
	ba,a	9f
3:
	subcc   %g1, 3, %g0		! syscall 3, enable interrupts
	bne 	1f
	and	%i0, SPARC_PSR_PIL_MASK, %l4
	andn	%l0, SPARC_PSR_PIL_MASK, %l5
	or	%l5, %l4, %l4
	mov   	%l4, %psr
	ba,a	9f

1:
	ta	0			! halt
9:					! leave
	mov	0, %g1			! clear %g1
        jmpl    %l2, %g0
        rett    %l2 + 4

        PUBLIC(sparc_disable_interrupts)

SYM(sparc_disable_interrupts):

	mov	SYS_irqdis, %g1
	ta	0
	retl
	nop

        PUBLIC(sparc_enable_interrupts)

SYM(sparc_enable_interrupts):

	mov	SYS_irqen, %g1
	ta	0
	retl
	nop

/* end of file */
