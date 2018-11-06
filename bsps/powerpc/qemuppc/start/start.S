#include <rtems/score/cpu.h>
#include <rtems/powerpc/powerpc.h>

	.global _start

	.section .bsp_start_text,"awx",@progbits

_start:
	lis   %r1,_ISR_Stack_area_end@h
	ori   %r1,%r1,_ISR_Stack_area_end@l
	/* Make sure stack is properly aligned */
	li    %r3, CPU_STACK_ALIGNMENT - 1
	andc  %r1, %r1, %r3
	/* NULL ptr to back chain */
	li    %r3, 0
	stw   %r3, 0(%r1)

	li    %r3,8192
	mtmsr %r3

	/* Read-only small data */
	lis %r2, _SDA2_BASE_@h
	ori %r2, %r2,_SDA2_BASE_@l

	/* Read-write small data */
	lis %r13, _SDA_BASE_@h
	ori %r13, %r13,_SDA_BASE_@l

        bl      cmain
	.size _start, . - _start

	.global __eabi
__eabi:
	blr
	.size __eabi, . - __eabi

	.section ".reset","ax"
_reset:
	b _start
	.size _reset, . - _reset
