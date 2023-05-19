/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCMPC55XX
 *
 * @brief Header file for the watchdog timer.
 */

/*
 * Copyright (c) 2008 embedded brains GmbH & Co. KG
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
