#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/asm.h>

.text
.align 4

PUBLIC(_CPU_Context_volatile_clobber)

SYM(_CPU_Context_volatile_clobber):

.macro clobber_register reg
	addi r5, r5, -1
	add \reg, r0, r5
.endm

	clobber_register r3
	clobber_register r4
	clobber_register r6
	clobber_register r7
	clobber_register r8
	clobber_register r9
	clobber_register r10

	rtsd	r15, 8
	nop
