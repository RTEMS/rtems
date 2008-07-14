/**
 * @file
 *
 * @ingroup mpc55xx_asm
 *
 * @brief FMPLL setup.
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

/* Timeout for delay in clocks */
.equ FMPLL_TIMEOUT, 6000

/* Reference clock */
.equ FMPLL_REF_CLOCK, 8000000

/* Settings for FMPLL from 12 MHz up to 128 MHz with 8 MHz reference frequency */
.equ FMPLL_128_8_SYNCR_SETTING_0, (FMPLL_SYNCR_PREDIV_0 | FMPLL_SYNCR_MFD_12 | FMPLL_SYNCR_RFD_2 | FMPLL_SYNCR_LOCEN)
.equ FMPLL_128_8_SYNCR_SETTING_1, (FMPLL_SYNCR_PREDIV_0 | FMPLL_SYNCR_MFD_12 | FMPLL_SYNCR_RFD_0 | FMPLL_SYNCR_LOCEN)

.macro DO_SETTING setting
	LWI r5, FMPLL_128_8_SYNCR_SETTING_\setting
	stw r5, 0(r4)
	msync
	bl mpc55xx_fmpll_wait_for_lock
.endm

/**
 * @fn void mpc55xx_fmpll_reset_config()
 * @brief Configure FMPLL after reset.
 *
 * Sets the system clock from 12 MHz in two steps up to 128 MHz.
 */
GLOBAL_FUNCTION mpc55xx_fmpll_reset_config
	/* Save link register */
	mflr r3

	LA r4, FMPLL_SYNCR

	DO_SETTING 0
	DO_SETTING 1

	/* Enable loss-of-clock and loss-of-lock IRQs */
	lwz r5, 0(r4)
	LWI r6, FMPLL_SYNCR_LOCIRQ | FMPLL_SYNCR_LOLIRQ
	or r5, r5, r6

	/* Disable loss-of-clock and loss-of-lock resets */
	LWI r6, ~FMPLL_SYNCR_LOCRE & ~FMPLL_SYNCR_LOLRE
	and r5, r5, r6
	stw r5, 0(r4)

	/* Restore link register and return */
	mtlr r3
	blr

/**
 * @fn void mpc55xx_fmpll_wait_for_lock()
 * @brief Wait for FMPLL lock.
 * @warning If the lock cannot be obtained within some clock cycles a software
 * system reset will be initiated.
 */
GLOBAL_FUNCTION mpc55xx_fmpll_wait_for_lock
	LWI r6, FMPLL_TIMEOUT
	mtctr r6

	LWI r7, FMPLL_SYNSR_LOCK

	LA r6, FMPLL_SYNSR

fmpll_not_locked:
	bdnz fmpll_continue

	b mpc55xx_system_reset
fmpll_continue:
	lwz r8, 0(r6)
	and. r8, r8, r7
	beq fmpll_not_locked

	blr

/**
 * @fn int mpc55xx_get_system_clock()
 * @brief Returns the system clock.
 */
GLOBAL_FUNCTION mpc55xx_get_system_clock
	LA r4, FMPLL_SYNCR
	lwz r3, 0(r4)

	/* PREDIV */
	rlwinm r5, r3, 4, 29, 31

	/* MFD */
	rlwinm r6, r3, 9, 27, 31

	/* RFD */
	rlwinm r7, r3, 13, 29, 31

	/* Calculate system clock (Table 11-10 [MPC5567 Microcontroller Reference Manual]) */
	LWI r8, FMPLL_REF_CLOCK
	addi r5, r5, 1
	addi r6, r6, 4
	mullw r6, r6, r8
	sraw r6, r6, r7
	divw r3, r6, r5

	blr

/**
 * @fn void mpc55xx_system_reset()
 * @brief Software system reset.
 */
GLOBAL_FUNCTION mpc55xx_system_reset
	LA r8, SIU_SRCR
	LWI r9, SIU_SRCR_SSR
	stw r9, 0(r8)
twiddle:
	b twiddle
