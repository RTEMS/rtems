/*
 * $Id$
 *	
 * This code is loaded by the ROM loader at some arbitrary location.
 * Move it to high memory so that it can load the kernel at 0x0000.
 *
 */
/* A few utility functions, some copied from arch/ppc/lib/string.S */
#include <libcpu/cpu.h>
#include <rtems/score/targopts.h>
#include "asm.h"
		
	.text
	
        .globl  strlen
	.type	strlen,@function
strlen:
        addi    r4,r3,-1
1:      lbzu    r0,1(r4)
        cmpwi   0,r0,0
        bne     1b
        subf    r3,r3,r4
        blr
