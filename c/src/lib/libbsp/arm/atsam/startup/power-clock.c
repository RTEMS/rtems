/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/power.h>
#include <bsp/irq.h>

#include <libchip/chip.h>

#include <rtems/score/armv7m.h>

void atsam_power_handler_clock_driver(
    const atsam_power_control *control,
    atsam_power_state state
)
{
	volatile ARMV7M_Systick *systick = _ARMV7M_Systick;

	(void) control;

	switch (state) {
		case ATSAM_POWER_ON:
			systick->csr = ARMV7M_SYSTICK_CSR_ENABLE |
			    ARMV7M_SYSTICK_CSR_TICKINT |
			    ARMV7M_SYSTICK_CSR_CLKSOURCE;
			break;
		case ATSAM_POWER_OFF:
			systick->csr = 0;
			break;
		default:
			break;
	}
}
