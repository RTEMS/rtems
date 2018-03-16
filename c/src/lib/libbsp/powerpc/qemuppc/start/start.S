#include <rtems/score/cpu.h>
#include <rtems/powerpc/powerpc.h>

#include <bspopts.h>

	.global bsp_interrupt_stack_start
	.global bsp_interrupt_stack_end
	.global _start

	.section .bsp_start_text,"awx",@progbits

_start:
	lis   %r1,bsp_interrupt_stack_start@h
	ori   %r1,%r1,bsp_interrupt_stack_start@l
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

	/* Start stack area */
	.section ".bsp_rwextra", "aw", @nobits
	.align 4
	.space 4096
bsp_interrupt_stack_start:
	.space 32768
bsp_interrupt_stack_end:
