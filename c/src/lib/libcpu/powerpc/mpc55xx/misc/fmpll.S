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
#include <bspopts.h>
	
.section ".text"

/* Timeout for delay in clocks */
.equ FMPLL_TIMEOUT, 6000

.macro DO_SETTING setting
	lwz r5, \setting
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
	mflr r9

#if ((MPC55XX_CHIP_TYPE >= 5510) && (MPC55XX_CHIP_TYPE <= 5517))
	/*
	 * for MPC5510: pass in ptr to array with:
	 * off 0:  temp setting for ESYNCR2
	 * off 4: final setting for ESYNCR2
	 * off 8: final setting for ESYNCR1
	 */
	LA r4, FMPLL_ESYNCR2

	lwz r5, 0(r3)
	stw r5, 0(r4)
	msync

	lwz r5, 8(r3)
	stw r5, (FMPLL_ESYNCR1-FMPLL_ESYNCR2)(r4)	
	msync
	bl mpc55xx_fmpll_wait_for_lock

	DO_SETTING 4(r3)

	/*
	 * switch to PLL clock in SIU
	 */
	LA r4, SIU_SYSCLK
	lwz r5, 0(r4)
	LWI r6, ~SIU_SYSCLK_SYSCLKSEL_MASK
	and r5, r5, r6
	LWI r6, SIU_SYSCLK_SYSCLKSEL_PLL
	or  r5, r5, r6
	stw r5, 0(r4)
#else
	/*
	 * for MPC5566: pass in ptr to array with:
	 * off 0:  temp setting for SYNCR
	 * off 4: final setting for SYNCR
	 */

	LA r4, FMPLL_SYNCR

	DO_SETTING 0(r3)
	DO_SETTING 4(r3)

	/* Enable loss-of-clock and loss-of-lock IRQs */
	lwz r5, 0(r4)
	LWI r6, FMPLL_SYNCR_LOCIRQ | FMPLL_SYNCR_LOLIRQ
	or r5, r5, r6

	/* Disable loss-of-clock and loss-of-lock resets */
	LWI r6, ~FMPLL_SYNCR_LOCRE & ~FMPLL_SYNCR_LOLRE
	and r5, r5, r6
	stw r5, 0(r4)
#endif

	/* Restore link register and return */
	mtlr r9
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
	lwz r5, 0(r6)
	and. r5, r5, r7
	beq fmpll_not_locked

	blr

/**
 * @fn int mpc55xx_get_system_clock()
 * @brief Returns the system clock.
 */
GLOBAL_FUNCTION mpc55xx_get_system_clock
#if ((MPC55XX_CHIP_TYPE >= 5510) && (MPC55XX_CHIP_TYPE <= 5517))
	LA r4, FMPLL_ESYNCR1
	lwz r3, 0(r4)
	/* EPREDIV */
	rlwinm r5, r3,16, 28, 31

	/* MFD */
	rlwinm r6, r3,0, 24, 31

	LA r4, FMPLL_ESYNCR2
	lwz r3, 0(r4)
	/* ERFD */
	rlwinm r7, r3,0, 26, 31

	LWI r8, MPC55XX_FMPLL_REF_CLOCK
	addi r5, r5, 1
	addi r6, r6,16
	addi r7, r7, 1
	divw r3, r8, r5  /* REF_CLOCK/PREDIV */
	mullw r3, r6, r3 /* REF_CLOCK/PREDIV*MFD */
	divw r3, r3, r7  /* REF_CLOCK/PREDIV*MFD/RFD */
	
#else
	LA r4, FMPLL_SYNCR
	lwz r3, 0(r4)

	/* PREDIV */
	rlwinm r5, r3, 4, 29, 31

	/* MFD */
	rlwinm r6, r3, 9, 27, 31

	/* RFD */
	rlwinm r7, r3, 13, 29, 31
	/* Calculate system clock (Table 11-10 [MPC5567 Microcontroller Reference Manual]) */
	LWI r8, MPC55XX_FMPLL_REF_CLOCK
	addi r5, r5, 1
	addi r6, r6, 4
	mullw r6, r6, r8
	sraw r6, r6, r7
	divw r3, r6, r5
#endif

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
