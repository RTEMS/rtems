/**
 * @file
 *
 * @ingroup mpc55xx
 *
 * @brief Header file for the watchdog timer.
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

#ifndef LIBCPU_POWERPC_MPC55XX_WATCHDOG_H
#define LIBCPU_POWERPC_MPC55XX_WATCHDOG_H

#include <stdbool.h>

#include <rtems.h>

#include <libcpu/powerpc-utility.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static inline void mpc55xx_watchdog_clear()
{
	PPC_SET_SPECIAL_PURPOSE_REGISTER( BOOKE_TSR, BOOKE_TSR_WIS);
}

static inline void mpc55xx_watchdog_enable_interrupt( bool enable)
{
	if (enable) {
		PPC_SET_SPECIAL_PURPOSE_REGISTER_BITS( BOOKE_TCR, BOOKE_TCR_WIE);
	} else {
		PPC_CLEAR_SPECIAL_PURPOSE_REGISTER_BITS( BOOKE_TCR, BOOKE_TCR_WIE);
	}
}

static inline rtems_status_code mpc55xx_watchdog_set_time_base_bit( uint32_t bit)
{
	if (bit > 63) {
		return RTEMS_INVALID_NUMBER;
	}

	PPC_SET_SPECIAL_PURPOSE_REGISTER_BITS_MASKED(
		BOOKE_TCR,
		BOOKE_TCR_WP( bit) | BOOKE_TCR_WPEXT( bit >> 2),
		BOOKE_TCR_WP_MASK | BOOKE_TCR_WPEXT_MASK
	);

	return RTEMS_SUCCESSFUL;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBCPU_POWERPC_MPC55XX_WATCHDOG_H */
