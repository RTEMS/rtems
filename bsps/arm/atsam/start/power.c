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

void atsam_power_change_state(
    const atsam_power_control *controls,
    size_t n,
    atsam_power_state state
)
{
	size_t i;

	switch (state) {
		case ATSAM_POWER_ON:
			for (i = n; i > 0; --i) {
				const atsam_power_control *c;

				c = &controls[i - 1];
				(*c->handler)(c, state);
			}

			break;
		case ATSAM_POWER_INIT:
		case ATSAM_POWER_OFF:
			for (i = 0; i < n; ++i) {
				const atsam_power_control *c;

				c = &controls[i];
				(*c->handler)(c, state);
			}

			break;
		default:
			break;
	}
}

void atsam_power_handler_peripheral(
    const atsam_power_control *control,
    atsam_power_state state
)
{
	uint32_t id;
	uint32_t end;

	id = control->data.peripherals.first;
	end = control->data.peripherals.last + 1;

	switch (state) {
		case ATSAM_POWER_ON:
			while (id != end) {
				PMC_EnablePeripheral(id);
				++id;
			}
			break;
		case ATSAM_POWER_OFF:
			while (id != end) {
				PMC_DisablePeripheral(id);
				++id;
			}
			break;
		default:
			break;
	}
}

void atsam_power_handler_sleep_mode(const atsam_power_control *control, atsam_power_state state)
{
	(void) control;

	switch (state) {
		case ATSAM_POWER_OFF:
			/* Enable Low Power Mode in the Fast Startup Mode Register */
			PMC->PMC_FSMR &= (uint32_t)~PMC_FSMR_LPM;
			/* Do not set deep sleep, but "normal" sleep */
			SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;

			__asm__ volatile ("wfi");
			break;
		default:
			break;
	}
}
