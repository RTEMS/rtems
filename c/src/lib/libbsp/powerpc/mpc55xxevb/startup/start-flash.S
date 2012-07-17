/**
 * @file
 *
 * @ingroup mpc55xx_asm
 *
 * @brief Flash configuration.
 */

/*
 * Copyright (c) 2008-2011 embedded brains GmbH.  All rights reserved.
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

#include <libcpu/powerpc-utility.h>
#include <mpc55xx/reg-defs.h>

	.section	".bsp_start_text", "ax"

/* Optimized flash configurations (Table 13-15 [MPC5567 Microcontroller Reference Manual]) */
.equ FLASH_SETTINGS_RESET, 0xff00
.equ FLASH_SETTINGS_82, FLASH_BUICR_CPU_PREFTCH | FLASH_BUICR_APC_1 | FLASH_BUICR_RWSC_1 | FLASH_BUICR_WWSC_1 | FLASH_BUICR_DPFEN_3 | FLASH_BUICR_IPFEN_3 | FLASH_BUICR_PFLIM_6 | FLASH_BUICR_BFEN
.equ FLASH_SETTINGS_102, FLASH_BUICR_CPU_PREFTCH | FLASH_BUICR_APC_1 | FLASH_BUICR_RWSC_2 | FLASH_BUICR_WWSC_1 | FLASH_BUICR_DPFEN_3 | FLASH_BUICR_IPFEN_3 | FLASH_BUICR_PFLIM_6 | FLASH_BUICR_BFEN
.equ FLASH_SETTINGS_132, FLASH_BUICR_CPU_PREFTCH | FLASH_BUICR_APC_2 | FLASH_BUICR_RWSC_3 | FLASH_BUICR_WWSC_1 | FLASH_BUICR_DPFEN_3 | FLASH_BUICR_IPFEN_3 | FLASH_BUICR_PFLIM_6 | FLASH_BUICR_BFEN
.equ FLASH_SETTINGS_264, 0x01716B15

/**
 * @fn void mpc55xx_start_flash()
 * @brief Optimized flash configuration.
 * @warning Code will be copied and executed on the stack.
 */
GLOBAL_FUNCTION mpc55xx_start_flash
#if !defined(MPC55XX_NEEDS_LOW_LEVEL_INIT) || MPC55XX_CHIP_TYPE / 10 == 564
	blr
#else
	.equ	stack_size, 20
	.equ	lr_offset, 28

	/* Reserve stack frame */
	stwu	r1, -stack_size(r1)
	mflr	r0
	stw	r0, lr_offset(r1)

	/* Flash settings dependent on system clock */
	bl	mpc55xx_get_system_clock
	LWI	r4, 82000000
	cmpw	r3, r4
	ble	clock_82
	LWI	r4, 102000000
	cmpw	r3, r4
	ble	clock_102
	LWI	r4, 132000000
	cmpw	r3, r4
	ble	clock_132
	LWI	r4, 264000000
	cmpw	r3, r4
	ble	clock_264
	LWI	r3, FLASH_SETTINGS_RESET
	b	settings_done
clock_82:
	LWI	r3, FLASH_SETTINGS_82
	b	settings_done
clock_102:
	LWI	r3, FLASH_SETTINGS_102
	b	settings_done
clock_132:
	LWI	r3, FLASH_SETTINGS_132
	b settings_done
clock_264:
	LWI	r3, FLASH_SETTINGS_264
	b	settings_done
settings_done:

	/* Copy store code on the stack */
	LA	r4, store_start
	lwz	r6, 0(r4)
	lwz	r7, 4(r4)
	lwz	r8, 8(r4)
	stw	r6, 8(r1)
	stw	r7, 12(r1)
	stw	r8, 16(r1)

	/* Execute store code */
	LA	r4, FLASH_BIUCR
	addi	r5, r1, 8
	mtctr	r5
	bctrl

	/* Return */
	lwz	r0, lr_offset(r1)
	addi	r1, r1, stack_size
	mtlr	r0
	blr

/*
 * Store flash settings
 */

store_start:

	stw	r3, 0(r4)
	isync
	blr

#endif
