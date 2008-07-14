/**
 * @file
 *
 * @ingroup mpc55xx_asm
 *
 * @brief Flash configuration.
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

#include <libcpu/powerpc-utility.h>
#include <mpc55xx/reg-defs.h>

.section ".text"

/* Optimized flash configurations (Table 13-15 [MPC5567 Microcontroller Reference Manual]) */
.equ FLASH_SETTINGS_RESET, 0xff00
.equ FLASH_SETTINGS_82, FLASH_BUICR_CPU_PREFTCH | FLASH_BUICR_APC_1 | FLASH_BUICR_RWSC_1 | FLASH_BUICR_WWSC_1 | FLASH_BUICR_DPFEN_3 | FLASH_BUICR_IPFEN_3 | FLASH_BUICR_PFLIM_6 | FLASH_BUICR_BFEN
.equ FLASH_SETTINGS_102, FLASH_BUICR_CPU_PREFTCH | FLASH_BUICR_APC_1 | FLASH_BUICR_RWSC_2 | FLASH_BUICR_WWSC_1 | FLASH_BUICR_DPFEN_3 | FLASH_BUICR_IPFEN_3 | FLASH_BUICR_PFLIM_6 | FLASH_BUICR_BFEN
.equ FLASH_SETTINGS_132, FLASH_BUICR_CPU_PREFTCH | FLASH_BUICR_APC_2 | FLASH_BUICR_RWSC_3 | FLASH_BUICR_WWSC_1 | FLASH_BUICR_DPFEN_3 | FLASH_BUICR_IPFEN_3 | FLASH_BUICR_PFLIM_6 | FLASH_BUICR_BFEN

/**
 * @fn void mpc55xx_flash_config()
 * @brief Optimized flash configuration.
 * @warning Code will be copied and executed on the stack.  The stack pointer
 * will not be updated, since this function has to work before memory
 * initialization.
 */
GLOBAL_FUNCTION mpc55xx_flash_config
	mflr r31

	/* Flash settings dependent on system clock */
	bl mpc55xx_get_system_clock
	LWI r4, 82000000
	cmpw r3, r4
	ble clock_82
	LWI r4, 102000000
	cmpw r3, r4
	ble clock_102
	LWI r4, 132000000
	cmpw r3, r4
	ble clock_132
	LWI r30, FLASH_SETTINGS_RESET
	b settings_done
clock_82:
	LWI r30, FLASH_SETTINGS_82
	b settings_done
clock_102:
	LWI r30, FLASH_SETTINGS_102
	b settings_done
clock_132:
	LWI r30, FLASH_SETTINGS_132
	b settings_done
settings_done:

	/* Copy store code on the stack */
	LA r3, store_start
	LA r5, store_end
	subf r5, r3, r5
	subf r4, r5, r1

	/* Assert: Proper alignment of destination start */
	andi. r6, r4, 0x7
	bne twiddle

	/* Copy */
	bl mpc55xx_copy_8

	LA r6, FLASH_BIUCR

	/* Execute store code */
	mtctr r4
	bctrl

	mtlr r31
	blr

/*
 * Store flash settings
 */

	.align 3
	.set store_start, .
	stw r30, 0(r6)
	isync
	blr
	.align 3
	.set store_end, .

twiddle:
	b twiddle
