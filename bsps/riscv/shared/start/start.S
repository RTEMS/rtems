/*
 * Copyright (c) 2018 embedded brains GmbH & Co. KG

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
	.option	arch, +zicsr
	.option	norelax

#ifdef ESPRESSIF_DIRECT_BOOT
/*
 * These header values indicate to the ESP32 bootrom that the application in
 * flash is to be run via direct boot and is not an application image to be
 * unpacked.
 */
	.word 0xaedb041d
	.word 0xaedb041d
#endif

TYPE_FUNC(_start)
SYM(_start):

#ifndef RISCV_USE_S_MODE
	csrr	a0, mhartid
#endif

	mv	s0, a0
	mv	s1, a1

#ifdef RISCV_USE_S_MODE
	/* start boot hartid */
	li	t0, RISCV_BOOT_HARTID
	beq	t0, a0, .Lafter_boot_hartid

	/* set up temporary stack */
	LADDR	sp, _ISR_Stack_area_end

#ifdef BSP_START_COPY_FDT_FROM_U_BOOT
	mv	a0, a1
	call	bsp_fdt_copy
#endif

.Lstart_boot_hartid:

	li	a0, RISCV_BOOT_HARTID
	LADDR	a1, _start
	li 	a2, 1
	call	sbi_hsm_hart_start

	/* Not much to do if we could not start it, try again. */
	bnez	a0, .Lstart_boot_hartid
	mv	a0, s0
#endif

.Lafter_boot_hartid:

	call RISCV_Start_on_processor

	/* only boot hartid returns here */

.Lclear_bss:

	/* Clear .bss */
	LADDR	a0, bsp_section_bss_begin
	li	a1, 0
	LADDR	a2, bsp_section_bss_size
	call	memset

#ifdef BSP_START_COPY_FDT_FROM_U_BOOT
	/*
	 * s1 contains the address of FDT if the boot hartid was the one
	 * actually booted first, otherwise it contains the privilege
	 * level passed to sbi_hsm_hart_start.
	 */
	li	t0, 3
	ble	s1, t0, .Lafter_fdt_copy
	mv	a0, s1
	call	bsp_fdt_copy
.Lafter_fdt_copy:
#endif

#if defined(RTEMS_SMP)
	/* Give go to secondary processors */
	LADDR	t0, .Lsecondary_processor_go
	fence	iorw,ow
	amoswap.w	zero, zero, 0(t0)
#endif

	li	a0, 0
	tail	boot_card

PUBLIC(RISCV_Start_on_processor)
	.section	.bsp_start_text, "wax", @progbits
	.align	2
	.option	arch, +zicsr
	.option	norelax

TYPE_FUNC(RISCV_Start_on_processor)
RISCV_Start_on_processor:

	/* Load global pointer */
	.option	push
	.option	norelax
	LADDR	gp, __global_pointer$
	.option	pop

#ifndef RTEMS_SMP
	li	t3, RISCV_BOOT_HARTID
	bne	a0, t3, .Lwfi
#endif

	/* Init FPU */
#if defined( __riscv_flen )
#if defined( RISCV_USE_S_MODE )
	li	t0, SSTATUS_FS
	csrs	sstatus, t0
#else
	li	t0, MSTATUS_FS
	csrs	mstatus, t0
#endif
	csrw	fcsr, zero
#endif

	/* Set exception handler */
	LADDR	t0, _RISCV_Exception_handler
#ifdef RISCV_USE_S_MODE
	csrw	stvec, t0
#else
	csrw	mtvec, t0
#endif

	/*
	 * Load stack pointer, setup per-cpu storage, and branch to secondary
	 * processor start if necessary.
	 */
#ifdef RTEMS_SMP
	LADDR	sp, _ISR_Stack_area_begin
	LADDR	t2, _ISR_Stack_size
	li	t3, RISCV_BOOT_HARTID
	sub	a0, a0, t3

	/*
	 * Check that this is a configured processor.  If not, then there is
	 * not much that can be done since we do not have a stack available for
	 * this processor.  Just loop forever in this case.
	 */
	lw	t3, _SMP_Processor_configured_maximum
	bgeu	a0, t3, .Lwfi

	/* Adjust stack pointer */
#ifdef __riscv_mul
	addi	t0, a0, 1
	mul	t2, t2, t0
#else
	mv	t0, a0
	mv	t1, t2

.Ladd_more:

	add	t2, t2, t1
	addi	t0, t0, -1
	bnez	t0, .Ladd_more
#endif
	add	sp, sp, t2

	LADDR	t0, _Per_CPU_Information
	slli	t1, a0, PER_CPU_CONTROL_SIZE_LOG2
	add	t3, t0, t1
#ifdef RISCV_USE_S_MODE
	csrw	sscratch, t3
#else
	csrw	mscratch, t3
#endif
	/* boot hartid returns to _start, others wait to be released */
	bnez	a0, .Lstart_on_secondary_processor
#else /* RTEMS_SMP */
	LADDR	sp, _ISR_Stack_area_end
#endif
	ret

.Lwfi:
	wfi
	j	.Lwfi

#ifdef RTEMS_SMP
.Lstart_on_secondary_processor:

	/* Wait for boot processor (mhartid == RISCV_BOOT_HARTID) to go */
	LADDR	t0, .Lsecondary_processor_go

.Lwait_for_go_again:

	lw	t1, 0(t0)
	fence	iorw, iorw
	bnez	t1, .Lwait_for_go_again

	mv	a0, t3
	call	bsp_start_on_secondary_processor

	.section	.bsp_start_data, "aw"

	.type	.Lsecondary_processor_go, @object

#if __riscv_xlen == 32
	.size	.Lsecondary_processor_go, 4
	.align	2
#elif __riscv_xlen == 64
	.size	.Lsecondary_processor_go, 8
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
