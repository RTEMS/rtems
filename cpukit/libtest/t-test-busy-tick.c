/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestFrameworkImpl
 *
 * @brief This source file contains the implementation of
 *   T_get_one_clock_tick_busy().
 */

/*
 * Copyright (C) 2014, 2020 embedded brains GmbH & Co. KG
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/test.h>

#include <rtems.h>

static uint_fast32_t
T_estimate_busy_loop_maximum(void)
{
	uint_fast32_t initial;
	uint_fast32_t units;

	initial = rtems_clock_get_ticks_since_boot();
	units = 0;

	while (initial == rtems_clock_get_ticks_since_boot()) {
		++units;
	}

	return units;
}

static uint_fast32_t
T_wait_for_tick_change(void)
{
	uint_fast32_t initial;
	uint_fast32_t now;

	initial = rtems_clock_get_ticks_since_boot();

	do {
		now = rtems_clock_get_ticks_since_boot();
	} while (now == initial);

	return now;
}

uint_fast32_t
T_get_one_clock_tick_busy(void)
{
	uint_fast32_t last;
	uint_fast32_t now;
	uint_fast32_t a;
	uint_fast32_t b;
	uint_fast32_t m;

	/* Choose a lower bound */
	a = 1;

	/* Estimate an upper bound */

	T_wait_for_tick_change();
	b = 2 * T_estimate_busy_loop_maximum();

	while (true) {
		last = T_wait_for_tick_change();
		T_busy(b);
		now = rtems_clock_get_ticks_since_boot();

		if (now != last) {
			break;
		}

		b *= 2;
	}

	/* Find a good value */
	do {
		m = (a + b) / 2;

		last = T_wait_for_tick_change();
		T_busy(m);
		now = rtems_clock_get_ticks_since_boot();

		if (now != last) {
			b = m;
		} else {
			a = m;
		}
	} while (b - a > 1);

	return m;
}
