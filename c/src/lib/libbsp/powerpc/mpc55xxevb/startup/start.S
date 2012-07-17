/**
 * @file
 *
 * @ingroup mpc55xx_asm
 *
 * @brief Boot and system start code.
 */

/*
 * Copyright (c) 2008-2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <bspopts.h>
#include <bsp/linker-symbols.h>
#include <libcpu/powerpc-utility.h>

#if MPC55XX_CHIP_TYPE / 10 != 551
  #define HAS_SPE
#endif

#if MPC55XX_CHIP_TYPE / 10 == 564
  #define INIT_REGISTERS_FOR_LSM
#endif

#ifdef HAS_SPE
  #define ZERO_GPR(reg) evxor	reg, reg, reg
#else
  #define ZERO_GPR(reg) xor	reg, reg, reg
#endif

	.extern	__eabi
	.extern	boot_card
	.extern	bsp_ram_start
	.extern	mpc55xx_start_config_mmu_early
	.extern	mpc55xx_start_config_mmu_early_count
	.extern	mpc55xx_start_early

	.globl	_start
	.globl	mpc55xx_start_load_section
	.globl	mpc55xx_start_mmu_apply_config

#ifdef MPC55XX_BOOTFLAGS
	.globl	mpc55xx_bootflag_0
	.globl	mpc55xx_bootflag_1
#endif

	.section	".bsp_start_text", "ax"

	/* BAM: RCHW */
	.int	0x005a0000

	/* BAM: Address of start instruction */
	.int	_start

#ifdef MPC55XX_BOOTFLAGS
	/*
	 * We skip over the next two boot flag words to the next 64-bit
	 * aligned start address. It is 64-bit aligned to play well with
	 * FLASH programming.  These boot flags can be set by debuggers
	 * and emulators to customize boot.  Currently bit0 of
	 * bootflag_0 means to "skip setting up the MMU", allowing
	 * external MMU setup in a debugger before branching to 0x10.
	 * This can be used e.g., to map FLASH into RAM.
	 */
mpc55xx_bootflag_0:
	.int	0xffffffff
mpc55xx_bootflag_1:
	.int	0xffffffff
#endif

_start:

#ifdef MPC55XX_NEEDS_LOW_LEVEL_INIT

	/* Enable SPE */
#ifdef HAS_SPE
	mfmsr	r3
	oris	r3, r3, MSR_SPE >> 16
	mtmsr	r3
	isync
#endif

	/*
	 * Initialization of core registers according to "e200z4 Power
	 * Architecture Core Reference Manual" section 2.6 "Reset Settings"
	 * table 2-16 "Reset Settings of e200 Resources".  This is necessary
	 * for lock step mode (LSM).
	 */
	ZERO_GPR(r0)
#ifdef INIT_REGISTERS_FOR_LSM
	ZERO_GPR(r1)
	ZERO_GPR(r2)
	ZERO_GPR(r4)
	ZERO_GPR(r5)
	ZERO_GPR(r6)
	ZERO_GPR(r7)
	ZERO_GPR(r8)
	ZERO_GPR(r9)
	ZERO_GPR(r10)
	ZERO_GPR(r11)
	ZERO_GPR(r12)
	ZERO_GPR(r13)
	ZERO_GPR(r14)
	ZERO_GPR(r15)
	ZERO_GPR(r16)
	ZERO_GPR(r17)
	ZERO_GPR(r18)
	ZERO_GPR(r19)
	ZERO_GPR(r20)
	ZERO_GPR(r21)
	ZERO_GPR(r22)
	ZERO_GPR(r23)
	ZERO_GPR(r24)
	ZERO_GPR(r25)
	ZERO_GPR(r26)
	ZERO_GPR(r27)
	ZERO_GPR(r28)
	ZERO_GPR(r29)
	ZERO_GPR(r30)
	ZERO_GPR(r31)
	mtcrf	0xff, r0
	mtcsrr0	r0
	mtcsrr1	r0
	mtctr	r0
	mtspr	FSL_EIS_DBCNT, r0
	mtspr	DEAR_BOOKE, r0
	mtdec	r0
	mtspr	BOOKE_DECAR, r0
	mtspr	FSL_EIS_DSRR0, r0
	mtspr	FSL_EIS_DSRR1, r0
	mtspr	BOOKE_DVC1, r0
	mtspr	BOOKE_DVC2, r0
	mtspr	BOOKE_IVPR, r0
	mtlr	r0
	mtspr	FSL_EIS_MCAR, r0
	mtmcsrr0	r0
	mtmcsrr1	r0
	mtspr	SPRG0, r0
	mtspr	SPRG1, r0
	mtspr	SPRG2, r0
	mtspr	SPRG3, r0
	mtspr	SPRG4, r0
	mtspr	SPRG5, r0
	mtspr	SPRG6, r0
	mtspr	SPRG7, r0
	mtspr	FSL_EIS_SPRG8, r0
	mtspr	FSL_EIS_SPRG9, r0
	mtsrr0	r0
	mtsrr1	r0
	mtspr	USPRG0, r0
#ifdef HAS_SPE
	evmra	r0, r0
#endif
#endif /* INIT_REGISTERS_FOR_LSM */
	mtspr	TBWL, r0
	mtspr	TBWU, r0

	/* Enable time base */
	mfspr	r3, HID0
	ori	r3, r3, 0x4000
	mtspr	HID0, r3

	/* Enable branch prediction */
	LWI	r3, FSL_EIS_BUCSR_BBFI | FSL_EIS_BUCSR_BPEN
	mtspr	FSL_EIS_BUCSR, r3

	/* MMU early initialization */
	LA	r3, mpc55xx_start_config_mmu_early
	LW	r4, mpc55xx_start_config_mmu_early_count
	bl	mpc55xx_start_mmu_apply_config

	/* Initialize intermediate stack (ECC) */

	LA	r3, bsp_ram_start
	addi	r4, r3, MPC55XX_EARLY_STACK_SIZE

zero_intermediate_stack_loop:

#ifdef HAS_SPE
	evstdd	r0, 0(r3)
	evstdd	r0, 8(r3)
	evstdd	r0, 16(r3)
	evstdd	r0, 24(r3)
#else
	stw	r0, 0(r3)
	stw	r0, 4(r3)
	stw	r0, 8(r3)
	stw	r0, 12(r3)
	stw	r0, 16(r3)
	stw	r0, 20(r3)
	stw	r0, 24(r3)
	stw	r0, 28(r3)
#endif
	addi	r3, r3, 32
	cmpw	cr7, r3, r4
	bne	cr7, zero_intermediate_stack_loop
	subi	r1, r3, 16

#endif /* MPC55XX_NEEDS_LOW_LEVEL_INIT */

	/* Next steps in C */
	bl	mpc55xx_start_early

	/* Initialize start stack */
	LA	r1, start_stack_end
	subi	r1, r1, 16
	li	r0, 0
	stw	r0, 0(r1)

	/* Load sections */
	LA	r3, bsp_section_fast_text_begin
	LA	r4, bsp_section_fast_text_load_begin
	LA	r5, bsp_section_fast_text_size
	bl	mpc55xx_start_load_section
	LA	r3, bsp_section_fast_data_begin
	LA	r4, bsp_section_fast_data_load_begin
	LA	r5, bsp_section_fast_data_size
	bl	mpc55xx_start_load_section
	LA	r3, bsp_section_data_begin
	LA	r4, bsp_section_data_load_begin
	LA	r5, bsp_section_data_size
	bl	mpc55xx_start_load_section

	/* Set up EABI and SYSV environment */
	bl	__eabi

	/* Clear command line */
	li	r3, 0

	/* Start RTEMS */
	bl	boot_card

	/* Spin around */
twiddle:

	b	twiddle

mpc55xx_start_mmu_apply_config:

	cmpwi	cr7, r4, r0
	beqlr	cr7
	mtctr	r4

mmu_init_loop:

	lwz	r4, 0(r3)
	lwz	r5, 4(r3)
	lwz	r6, 8(r3)
	lwz	r7, 12(r3)
	mtspr	FSL_EIS_MAS0, r4
	mtspr	FSL_EIS_MAS1, r5
	mtspr	FSL_EIS_MAS2, r6
	mtspr	FSL_EIS_MAS3, r7
	tlbwe
	addi	r3, r3, 16
	bdnz	mmu_init_loop
	blr

mpc55xx_start_load_section:
	cmpw	cr7, r3, r4
	beqlr	cr7
	b	memcpy

	/* Start stack area */

	.section	".bsp_rwextra", "aw", @nobits
	.align	4
	.space	4096

start_stack_end:
