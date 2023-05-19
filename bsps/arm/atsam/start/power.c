/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2016 embedded brains GmbH & Co. KG
 *
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
