/*
 * Copyright (c) 2018 embedded brains GmbH

 * Copyright (c) 2015 University of York.
 * Hesham Almatary <hesham@alumni.york.ac.uk>
 *
 * Copyright (c) 2013, The Regents of the University of California (Regents).
 * All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <rtems/asm.h>
#include <rtems/score/percpu.h>
#include <rtems/score/riscv-utility.h>
#include <bsp/linker-symbols.h>
#include <bspopts.h>

PUBLIC(_start)

	.section	.bsp_start_text, "wax", @progbits
	.align	2

TYPE_FUNC(_start)
SYM(_start):
	/* Load global pointer */
	.option	push
	.option	norelax
	LADDR	gp, __global_pointer$
	.option	pop

	/* Init FPU */
#ifdef __riscv_flen
	li	t0, MSTATUS_FS
	csrs	mstatus, t0
	csrw	fcsr, zero
#endif

	/* Set exception handler */
	LADDR	t0, _RISCV_Exception_handler
	csrw	mtvec, t0

	/* Load stack pointer and branch to secondary processor start if necessary */
#ifdef RTEMS_SMP
	LADDR	sp, _ISR_Stack_area_begin
	LADDR	t2, _ISR_Stack_size
	csrr	s0, mhartid
	LADDR	t0, _Per_CPU_Information
	slli	t1, s0, PER_CPU_CONTROL_SIZE_LOG2
	add	s1, t0, t1
	csrw	mscratch, s1
	bnez	s0, .Lstart_on_secondary_processor
	add	sp, sp, t2
#else
	LADDR	sp, _ISR_Stack_area_end
#endif

#ifdef BSP_START_COPY_FDT_FROM_U_BOOT
	mv	a0, a1
	call	bsp_fdt_copy
#endif

	/* Clear .bss */
	LADDR	a0, bsp_section_bss_begin
	li	a1, 0
	LADDR	a2, bsp_section_bss_size
	call	memset

#ifdef RTEMS_SMP
	/* Give go to secondary processors */
	LADDR	t0, .Lsecondary_processor_go
	fence	iorw,ow
	amoswap.w	zero, zero, 0(t0)
#endif

	li	a0, 0
	j	boot_card

#ifdef RTEMS_SMP

.Lstart_on_secondary_processor:

	/* Adjust stack pointer */
#ifdef __riscv_mul
	addi	t0, s0, 1
	mul	t2, t2, t0
#else
	mv	t0, s0
	mv	t3, t2

.Ladd_more:

	add	t2, t2, t3
	addi	t0, t0, -1
	bnez	t0, .Ladd_more
#endif
	add	sp, sp, t2

	/* Wait for go issued by the boot processor (mhartid == 0) */
	LADDR	t0, .Lsecondary_processor_go

.Lwait_for_go_again:

	lw	t1, 0(t0)
	fence	iorw, iorw
	bnez	t1, .Lwait_for_go_again

	mv	a0, s1
	call	bsp_start_on_secondary_processor

#if __riscv_xlen == 32
	.align	2
#elif __riscv_xlen == 64
	.align	3
#endif

.Lsecondary_processor_go:

	/*
	 * These are ebreak instructions, just in case we end up here executing
	 * code.
	 */
	.word	0x00100073
#if __riscv_xlen == 64
	.word	0x00100073
#endif

#endif /* RTEMS_SMP */
