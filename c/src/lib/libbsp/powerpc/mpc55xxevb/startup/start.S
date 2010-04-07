/**
 * @file
 *
 * @ingroup mpc55xx_asm
 *
 * @brief Boot and system start code.
 */

/*
 * Copyright (c) 2008
 * Embedded Brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * rtems@embedded-brains.de
 *
 * The license and distribution terms for this file may be found in the file
 * LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 */

/**
 * @defgroup mpc55xx_asm Assembler files
 *
 * @ingroup mpc55xx
 */

#include <libcpu/powerpc-utility.h>
#include <mpc55xx/reg-defs.h>
#include <bspopts.h>
	
.section ".entry", "ax"
PUBLIC_VAR (start)
.globl  fmpll_syncr_vals
bam_rchw:
/*
 * BAM
 */

	/* BAM: RCHW */
	.int 0x005a0000

	/* BAM: Address of start instruction */
	.int 0x8

/*
 * Enable time base
 */
start:	
	li r0, 0
	mtspr TBWU, r0
	mtspr TBWL, r0
	mfspr r2, HID0
	ori r2, r2, 0x4000
	mtspr HID0, r2

/*
 * System clock
 */

	LWI r3,fmpll_syncr_vals
	bl SYM (mpc55xx_fmpll_reset_config)

/*
 * Enable branch prediction
 */

	LWI r2, BUCSR_BBFI | BUCSR_BPEN
	mtspr BUCSR, r2

/*
 * Basics
 */

	/* Set stack start to end of ram */
	LA r1, bsp_ram_end
	addi r1, r1, -8

	/* Enable SPE */
	mfmsr r2
	oris r2, r2, 0x200
	mtmsr r2

	/* Config internal flash */
	bl SYM (mpc55xx_flash_config)

#if DATA_CACHE_ENABLE || INSTRUCTION_CACHE_ENABLE
	/* FIXME: Config cache */
	bl config_cache
#endif /* DATA_CACHE_ENABLE || INSTRUCTION_CACHE_ENABLE */

/*
 * TODO, FIXME: Enable cache in the MMU for the SRAM
 */

.equ MAS0, 624
.equ MAS1, 625
.equ MAS2, 626
.equ MAS3, 627

	LWI r3, 0x10030000
	mtspr MAS0, r3
	tlbre
	LWI r4, ~0x00000008
	mfspr r3, MAS2
	and r3, r3, r4
	mtspr MAS2, r3
	tlbwe

/*
 * TODO, FIXME: Set MMU for the external SRAM
 */

	LWI r3, 0x10020000
	mtspr MAS0, r3
	tlbre
	LWI r4, 0xfff
	mfspr r3, MAS3
	and r3, r3, r4
	LWI r4, 0x20000000
	or r3, r3, r4
	mtspr MAS3, r3
	tlbwe

/*
 * Zero RAM (needed to get proper ECC)
 */

	/* Addresses */
	LA r3, bsp_ram_start
	LA r4, bsp_ram_end

	/* Assert: Proper alignment of destination start */
	andi. r6, r3, 0x3f
	bne twiddle

	/* Assert: Proper alignment of destination end */
	andi. r6, r4, 0x3f
	bne twiddle

	/* Data size = destination end - destination start */
	subf r4, r3, r4

	/* Save time */
	mftb r24

	/* Zero */
	bl SYM (mpc55xx_zero_32)

	/* Save time and get time delta */
	mftb r25
	subf r24, r24, r25

/*
 * Copy data
 */

	/* Addresses */
	LA r3, bsp_section_text_end
	LA r4, bsp_section_data_start
	LA r5, bsp_section_data_end

	/* Assert: Proper alignment of source start */
	andi. r6, r3, 0x7
	bne twiddle

	/* Assert: Proper alignment of destination start */
	andi. r6, r4, 0x7
	bne twiddle

	/* Assert: Proper alignment of destination end */
	andi. r6, r5, 0x7
	bne twiddle

	/* Data size = destination end - destination start */
	subf r5, r4, r5

	/* Copy */
	bl SYM (mpc55xx_copy_8)

	/* Save time and get time delta */
	mftb r26
	subf r25, r25, r26

/*
 * Prepare high level initialization
 */

	/* Create NULL */
	li r0, 0

	/* Return address */
	stw r0, 4(r1)

	/* Back chain */
	stw r0, 0(r1)

	/* Read-only small data */
	LA r2, _SDA2_BASE_

	/* Read-write small data */
	LA r13, _SDA_BASE_

/*
 * Start RTEMS
 */

	/* Clear command line */
	xor r3, r3, r3

	/* Start RTEMS */
	bl SYM (boot_card)

	/* Spin around */
	b twiddle

.equ L1CSR0, 1010
.equ L1CSR0_CINV, 0x2
.equ L1CSR0_CABT, 0x4
/* FIXME: CORG??? .equ L1CSR0_SETTINGS, 0x00180011 */
.equ L1CSR0_SETTINGS, 0x00100001

/*
 * Configure cache
 */
config_cache:
	/* Start cache invalidation */
	LWI r5, L1CSR0_CINV
	mtspr L1CSR0, r5

	/* Bit masks to test and clear invalidation abortion (CABT) */
	LWI r6, L1CSR0_CABT
	not r7, r6

	/* Wait for cache invalidation to complete */
check_cache_invalidation:
	mfspr r9, L1CSR0

	/* Check if the invalidate was aborted */
	and. r10, r9, r6
	beq no_chache_invalidation_abort

	/* Clear CABT bit */
	and r10, r9, r7
	mtspr L1CSR0, r10

	/* Retry invalidation */
	b config_cache

no_chache_invalidation_abort:
	/* Check CINV bit */
	and. r10, r5, r9

	/* Wait? */
	bne check_cache_invalidation

	/* Enable cache */
	LWI r6, L1CSR0_SETTINGS
	mfspr r5, L1CSR0
	or r5, r5, r6
	msync
	isync
	mtspr L1CSR0, r5

	/* Return */
	blr

twiddle:
	b	twiddle
