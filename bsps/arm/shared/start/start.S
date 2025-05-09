/**
 * @file
 *
 * @brief Boot and system start code.
 */
/*
 * Copyright (C) 2008, 2020 embedded brains GmbH & Co. KG
 * Copyright (C) 2016 Pavel Pisa <pisa@cmp.felk.cvut.cz>
 *  Czech Technical University in Prague
 *  Zikova 1903/4
 *  166 36 Praha 6
 *  Czech Republic
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <rtems/asm.h>
#include <rtems/score/percpu.h>

#include <bspopts.h>
#include <bsp/irq.h>

#include <dev/cache/arm-data-cache-loop-set-way.h>

	/* Global symbols */
	.globl	_start
	.type	_start, %function
	.globl	bsp_start_hook_0_done
	.type	bsp_start_hook_0_done, %function
	.globl	bsp_start_vector_table_begin
	.globl	bsp_start_vector_table_end
	.globl	bsp_start_vector_table_size
	.globl	bsp_vector_table_size

	.section	".bsp_start_text", "ax"

#ifdef BSP_START_ZIMAGE_HEADER
.rept	8
	mov	r0, r0
.endr
	b	.Lafter_header
	.word	0x016f2818		/* Magic numbers to help the loader */
	.word	bsp_section_start_begin	/* zImage start address */
	.word	bsp_section_data_end	/* zImage end address */
.Lafter_header:
#endif /* BSP_START_ZIMAGE_HEADER */

#if defined(ARM_MULTILIB_ARCH_V4)

#ifdef BSP_START_IN_HYP_SUPPORT
	.globl	bsp_start_hyp_vector_table_begin
#endif

	.arm

/*
 * This is the exception vector table and the pointers to the default
 * exceptions handlers.
 */

bsp_start_vector_table_begin:

	ldr	pc, .Lhandler_addr_reset
	ldr	pc, .Lhandler_addr_undef
	ldr	pc, .Lhandler_addr_swi
	ldr	pc, .Lhandler_addr_prefetch
	ldr	pc, .Lhandler_addr_abort

#ifdef BSP_START_VECTOR_RESERVED_SLOT
	BSP_START_VECTOR_RESERVED_SLOT
#else
	udf
#endif

	ldr	pc, .Lhandler_addr_irq
	ldr	pc, .Lhandler_addr_fiq

#ifdef BSP_START_VECTOR_ADDRESS_TABLE_ALIGNMENT
.balign BSP_START_VECTOR_ADDRESS_TABLE_ALIGNMENT
#endif

.Lhandler_addr_reset:

#ifdef BSP_START_RESET_VECTOR
	.word	BSP_START_RESET_VECTOR
#else
	.word	_start
#endif

.Lhandler_addr_undef:

	.word	_ARMV4_Exception_undef_default

.Lhandler_addr_swi:

	.word	_ARMV4_Exception_swi_default

.Lhandler_addr_prefetch:

	.word	_ARMV4_Exception_pref_abort_default

.Lhandler_addr_abort:

	.word	_ARMV4_Exception_data_abort_default

.Lhandler_addr_reserved:

	.word	0

.Lhandler_addr_irq:

	.word	_ARMV4_Exception_interrupt

.Lhandler_addr_fiq:

	.word	_ARMV4_Exception_fiq_default

bsp_start_vector_table_end:

#ifdef BSP_START_IN_HYP_SUPPORT
bsp_start_hyp_vector_table_begin:
	udf
	ldr	pc, .Lhandler_addr_hyp_undef
	udf
	udf
	udf
	udf
	udf
	udf

.Lhandler_addr_hyp_reset:
	.word	0

.Lhandler_addr_hyp_undef:
	.word	_ARMV4_Exception_undef_default

.Lhandler_addr_hyp_swi:
	.word	0

.Lhandler_addr_hyp_prefetch:
	.word	0

.Lhandler_addr_hyp_abort:
	.word	0

.Lhandler_addr_hyp_hyp:
	.word	0

.Lhandler_addr_hyp_irq:
	.word	0

.Lhandler_addr_hyp_fiq:
	.word	0
#endif

/* Start entry */

_start:

	/*
	 * We do not save the context since we do not return to the boot
	 * loader.  Boot loaders may pass the device tree in r2.  Do not touch
	 * r2 until bsp_fdt_copy() is called.
	 */

#ifdef BSP_START_NEEDS_REGISTER_INITIALIZATION
	mov	r0, #0
	mov	r1, #0
	mov	r2, #0
	mov	r3, #0
	mov	r4, #0
	mov	r5, #0
	mov	r6, #0
	mov	r7, #0
	mov	r8, #0
	mov	r9, #0
	mov	r10, #0
	mov	r11, #0
	mov	r12, #0
	mov	r13, #0
#endif

#if __ARM_ARCH >= 7
	/*
	 * Write to BPIALL (Branch Predictor Invalidate All) to invalidate all
	 * branch predictors.  There is no need to use BPIALLIS (Branch
	 * Predictor Invalidate All, Inner Shareable) since this code is
	 * executed on all processors used by RTEMS.
	 */
	mov	r0, #0
	mcr	p15, 0, r0, c7, c5, 6
#endif

#ifdef RTEMS_SMP
	/* Read MPIDR and get current processor index */
	mrc	p15, 0, r7, c0, c0, 5
	and	r7, #0xff

	/*
	 * Check that this is a configured processor.  If not, then there is
	 * not much that can be done since we do not have a stack available for
	 * this processor.  Just loop forever in this case.
	 */
	ldr	r1, =_SMP_Processor_configured_maximum
	ldr	r1, [r1]
	cmp	r1, r7
	bgt	.Lconfigured_processor
.Linvalid_processor_wait_for_ever:
	wfe
	b	.Linvalid_processor_wait_for_ever
.Lconfigured_processor:

        /*
         * Get current per-CPU control and store it in PL1 only Thread ID
         * Register (TPIDRPRW).
         */
	ldr	r1, =_Per_CPU_Information
	add	r1, r1, r7, asl #PER_CPU_CONTROL_SIZE_LOG2
	mcr	p15, 0, r1, c13, c0, 4

#endif

	/* Calculate interrupt stack area end for current processor */
	ldr	r1, =_ISR_Stack_size
#ifdef RTEMS_SMP
	add	r3, r7, #1
	mul	r1, r1, r3
#endif
	ldr	r0, =_ISR_Stack_area_begin
	add	r3, r1, r0

	/* Save original CPSR value */
	mrs	r4, cpsr

#ifdef BSP_START_IN_HYP_SUPPORT
	orr	r0, r4, #(ARM_PSR_I | ARM_PSR_F)
	msr	cpsr, r4

	and	r0, r4, #ARM_PSR_M_MASK
	cmp	r0, #ARM_PSR_M_HYP
	bne	.L_skip_hyp_svc_switch

	/* Boot loader starts kernel in HYP mode, switch to SVC necessary */

	ldr	r1, =bsp_stack_hyp_size
	mov	sp, r3
	sub	r3, r3, r1

	ldr	r0, =bsp_start_hyp_vector_table_begin
	mcr	p15, 4, r0, c12, c0, 0

	mov	r0, #0
	mcr	p15, 4, r0, c1, c1, 0
	mcr	p15, 4, r0, c1, c1, 2
	mcr	p15, 4, r0, c1, c1, 3
/*
 * HSCTLR.TE
 * optional start of hypervisor handlers in Thumb mode
 *	orr	r0, #(1 << 30)
 */
	mcr	p15, 4, r0, c1, c0, 0	/* HSCTLR */
	mrc	p15, 4, r0, c1, c1, 1	/* HDCR */
	and	r0, #0x1f		/* Preserve HPMN */
	mcr	p15, 4, r0, c1, c1, 1	/* HDCR */

	/* Prepare SVC mode for eret */
	mrs	r0, cpsr
	bic	r0, r0, #ARM_PSR_M_MASK
	orr	r0, r0, #ARM_PSR_M_SVC
	msr	spsr_cxsf, r0

	ldr	r0, =.L_hyp_to_svc_return
	.inst 0xe12ef300	/* msr ELR_hyp, r0 */
	mov	r0, sp
	.inst 0xe160006e	/* eret */
.L_hyp_to_svc_return:
	mov	sp, r0

.L_skip_hyp_svc_switch:
#endif /* BSP_START_IN_HYP_SUPPORT */

	/* Initialize stack pointer registers for the various modes */

	/* Enter FIQ mode and set up the FIQ stack pointer */
	mov	r0, #(ARM_PSR_M_FIQ | ARM_PSR_I | ARM_PSR_F)
	msr	cpsr, r0
	ldr	r1, =bsp_stack_fiq_size
	mov	sp, r3
	sub	r3, r3, r1

#ifdef BSP_START_NEEDS_REGISTER_INITIALIZATION
	/* These are banked FIQ mode registers */
	mov	r8, #0
	mov	r9, #0
	mov	r10, #0
	mov	r11, #0
	mov	r12, #0
#endif

	/* Enter ABT mode and set up the ABT stack pointer */
	mov	r0, #(ARM_PSR_M_ABT | ARM_PSR_I | ARM_PSR_F)
	msr	cpsr, r0
	ldr	r1, =bsp_stack_abt_size
	mov	sp, r3
	sub	r3, r3, r1

	/* Enter UND mode and set up the UND stack pointer */
	mov	r0, #(ARM_PSR_M_UND | ARM_PSR_I | ARM_PSR_F)
	msr	cpsr, r0
	ldr	r1, =bsp_stack_und_size
	mov	sp, r3
	sub	r3, r3, r1

	/* Enter IRQ mode and set up the IRQ stack pointer */
	mov	r0, #(ARM_PSR_M_IRQ | ARM_PSR_I | ARM_PSR_F)
	msr	cpsr, r0
	mov	sp, r3

	/*
	 * Enter SVC mode and set up the SVC stack pointer, reuse IRQ stack
	 * (interrupts are disabled).
	 */
	mov	r0, #(ARM_PSR_M_SVC | ARM_PSR_I | ARM_PSR_F)
	msr	cpsr, r0
	mov	sp, r3

	/* Stay in SVC mode */

	/* Copy device tree from boot loader */
#ifdef BSP_START_COPY_FDT_FROM_U_BOOT
#ifdef RTEMS_SMP
	cmp	r7, #0
	bne	1f
#endif
	mov	r0, r2
	bl	bsp_fdt_copy
1:
#endif

#ifdef ARM_MULTILIB_VFP
#ifdef ARM_MULTILIB_HAS_CPACR
	/* Read CPACR */
	mrc p15, 0, r0, c1, c0, 2

	/* Enable CP10 and CP11 */
	orr r0, r0, #(1 << 20)
	orr r0, r0, #(1 << 22)

	/*
	 * Clear ASEDIS and D32DIS.  Writes to D32DIS are ignored for VFP-D16.
	 */
	bic r0, r0, #(3 << 30)

	/* Write CPACR */
	mcr p15, 0, r0, c1, c0, 2
	isb
#endif

	/* Enable FPU */
	mov r0, #(1 << 30)
	vmsr FPEXC, r0

#ifdef BSP_START_NEEDS_REGISTER_INITIALIZATION
	mov	r0, #0
	vmov	d0, r0, r0
	vmov	d1, r0, r0
	vmov	d2, r0, r0
	vmov	d3, r0, r0
	vmov	d4, r0, r0
	vmov	d5, r0, r0
	vmov	d6, r0, r0
	vmov	d7, r0, r0
	vmov	d8, r0, r0
	vmov	d9, r0, r0
	vmov	d10, r0, r0
	vmov	d11, r0, r0
	vmov	d12, r0, r0
	vmov	d13, r0, r0
	vmov	d14, r0, r0
	vmov	d15, r0, r0
#ifdef ARM_MULTILIB_VFP_D32
	vmov	d16, r0, r0
	vmov	d17, r0, r0
	vmov	d18, r0, r0
	vmov	d19, r0, r0
	vmov	d20, r0, r0
	vmov	d21, r0, r0
	vmov	d22, r0, r0
	vmov	d23, r0, r0
	vmov	d24, r0, r0
	vmov	d25, r0, r0
	vmov	d26, r0, r0
	vmov	d27, r0, r0
	vmov	d28, r0, r0
	vmov	d29, r0, r0
	vmov	d30, r0, r0
	vmov	d31, r0, r0
#endif /* ARM_MULTILIB_VFP_D32 */
#endif /* BSP_START_NEEDS_REGISTER_INITIALIZATION */

#endif /* ARM_MULTILIB_VFP */

#if (__ARM_ARCH >= 7 && __ARM_ARCH_PROFILE == 'A') || __ARM_ARCH >= 8
	/*
	 * Set VBAR to the vector table in the start section and make sure
	 * SCTLR[M, I, A, C, V] are cleared.  Afterwards, exceptions are
	 * handled by RTEMS.
	 */
	ldr	r0, =bsp_start_vector_table_begin
	dsb
	mcr	p15, 0, r0, c12, c0, 0
	mrc	p15, 0, r0, c1, c0, 0
	bic	r1, r0, #0x2800
	bic	r1, r1, #0x7
	mcr	p15, 0, r1, c1, c0, 0
	isb

#ifdef RTEMS_SMP
	/* Skip the data cache initialization if we are a secondary processor */
	cmp	r7, #0
	bne	.Ldata_caches_initialized
#endif

	/* Check previous SCTLR[C] and initialize data caches */
	tst	r0, #0x4
	bne	.Lclean_invalidate_data_caches

	/*
	 * Invalidate the sets and ways of all data or unified cache levels
	 * using DCISW (Data Cache line Invalidate by Set/Way).
	 */
	ARM_DATA_CACHE_LOOP_SET_WAY c6
	b	.Ldata_caches_initialized

.Lclean_invalidate_data_caches:

	/*
	 * Clean and invalidate the sets and ways of all data or unified cache
	 * levels using DCCISW (Data Cache line Clean and Invalidate by
	 * Set/Way).
	 */
	ARM_DATA_CACHE_LOOP_SET_WAY c14

.Ldata_caches_initialized:

	/*
	 * Invalidate the instruction cache levels using ICIALLU (Instruction
	 * Cache Invalidate All to PoU).
	 */
	mov	r0, #0
	mcr	p15, 0, r0, c7, c5, 0

#if __ARM_ARCH_PROFILE == 'A'
	/*
	 * Invalidate the TLB using ITLBIALL (Instruction TLB Invalidate All).
	 */
	mov	r0, #0
	mcr	p15, 0, r0, c8, c7, 0
	isb
#endif
#endif /* (__ARM_ARCH >= 7 && __ARM_ARCH_PROFILE == 'A') || __ARM_ARCH >= 8 */

	/*
	 * Invoke the start hook 0.
	 *
	 * The previous code and parts of the start hook 0 may run with an
	 * address offset.  After the return from start hook 0 it is assumed
	 * that the code can run at its intended position.  Thus the link
	 * register will be loaded with the absolute address and the branch
	 * link instruction cannot be used.  In THUMB mode the branch
	 * instruction as a very limited address range of 2KiB.  Use a bx to
	 * the start hook 0 address instead corrected by the address offset.
	 */
	ldr	lr, =bsp_start_hook_0_done
	mov	r0, pc
	ldr	r1, =.Lget_absolute_pc
.Lget_absolute_pc:
	sub	r1, r0
	ldr	r7, =bsp_start_hook_0
	add	r7, r1
	bx	r7

	/* Allow bsp_start_hook_0() hooks to jump to this label */
bsp_start_hook_0_done:

	/*
	 * Initialize the exception vectors.  This includes the exceptions
	 * vectors and the pointers to the default exception handlers.
	 */

	ldr	r0, =bsp_vector_table_begin
	ldr	r1, =bsp_start_vector_table_begin
	cmp	r0, r1
	beq	.Lvector_table_copy_done
	ldmia	r1!, {r2-r9}
	stmia	r0!, {r2-r9}
	ldmia	r1!, {r2-r9}
	stmia	r0!, {r2-r9}

.Lvector_table_copy_done:

	/*
	 * This code path is only executed by the primary processor.  Set the
	 * VBAR to the normal vector table.  For secondary processors, this is
	 * done by bsp_start_hook_0().
	 */
#if (__ARM_ARCH >= 7 && __ARM_ARCH_PROFILE == 'A') || __ARM_ARCH >= 8
	ldr	r0, =bsp_vector_table_begin
	dsb
	mcr	p15, 0, r0, c12, c0, 0
	isb
#elif defined(__ARM_ARCH_6KZ__)
	mov	r1, #0
	ldr	r0, =bsp_vector_table_begin
	mcr	p15, 0, r1, c7, c10, 4      /* DataSync */
	mcr	p15, 0, r0, c12, c0, 0      /* Load VBAR */
	mcr	p15, 0, r1, c7, c5, 4       /* Flush Prefetch */
#endif

	SWITCH_FROM_ARM_TO_THUMB	r3

	/* Branch to start hook 1 */
	bl	bsp_start_hook_1

	/* Branch to boot card */
	mov	r0, #0
	bl	boot_card

#elif defined(ARM_MULTILIB_ARCH_V7M)

#include <rtems/score/armv7m.h>

	.syntax	unified

	.thumb

bsp_start_vector_table_begin:

	.word	_ISR_Stack_area_end
	.word	_start /* Reset */
	.word	_ARMV7M_Exception_default /* NMI */
	.word	_ARMV7M_Exception_default /* Hard Fault */
	.word	_ARMV7M_Exception_default /* MPU Fault */
	.word	_ARMV7M_Exception_default /* Bus Fault */
	.word	_ARMV7M_Exception_default /* Usage Fault */
	.word	_ARMV7M_Exception_default /* Reserved */
	.word	_ARMV7M_Exception_default /* Reserved */
	.word	_ARMV7M_Exception_default /* Reserved */
	.word	_ARMV7M_Exception_default /* Reserved */
	.word	_ARMV7M_Supervisor_call /* SVC */
	.word	_ARMV7M_Exception_default /* Debug Monitor */
	.word	_ARMV7M_Exception_default /* Reserved */
	.word	_ARMV7M_Pendable_service_call /* PendSV */
	.word	_ARMV7M_Clock_handler /* SysTick */
	.rept	BSP_INTERRUPT_VECTOR_COUNT
	.word	_ARMV7M_NVIC_Interrupt_dispatch /* IRQ */
	.endr

bsp_start_vector_table_end:

	.thumb_func

_start:

#if defined(ARM_MULTILIB_VFP) && defined(ARM_MULTILIB_HAS_CPACR)
	/*
	 * Enable CP10 and CP11 coprocessors for privileged and user mode in
	 * CPACR (bits 20-23).  Ensure that write to register completes.
	 */
	ldr	r0, =ARMV7M_CPACR
	ldr	r1, [r0]
	orr	r1, r1, #(0xf << 20)
	str	r1, [r0]
	dsb
	isb
#endif

	ldr	sp, =_ISR_Stack_area_end

	/*
	 * Set the Main Stack Pointer (MSP) to the ISR stack area end just in
	 * case we run using the Process Stack Pointer (PSP).
	 */
	mov	r0, sp
	msr	msp, r0

	ldr	lr, =bsp_start_hook_0_done + 1
	b	bsp_start_hook_0

	/* Allow bsp_start_hook_0() hooks to jump to this label */
bsp_start_hook_0_done:

	bl	bsp_start_hook_1
	movs	r0, #0
	bl	boot_card

#endif /* defined(ARM_MULTILIB_ARCH_V7M) */

	.set	bsp_start_vector_table_size, bsp_start_vector_table_end - bsp_start_vector_table_begin
	.set	bsp_vector_table_size, bsp_start_vector_table_size
