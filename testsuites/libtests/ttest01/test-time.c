#include <t.h>

T_TEST_CASE(time_to_string)
{
	T_time_string ts;
	T_time t;
	uint32_t s;
	uint32_t ns;

	t = T_seconds_and_nanoseconds_to_time(0, 123456789);
	T_eq_str(T_time_to_string_ns(t, ts), "0.123456789");
	T_eq_str(T_time_to_string_us(t, ts), "0.123456");
	T_eq_str(T_time_to_string_ms(t, ts), "0.123");
	T_eq_str(T_time_to_string_s(t, ts), "0");

	T_time_to_seconds_and_nanoseconds(t, &s, &ns);
	T_eq_u32(s, 0);
	T_eq_u32(ns, 123456789);
}

T_TEST_CASE(now)
{
	T_time_string ts;
	T_time t0;
	T_time t1;

	t0 = T_now();
	t1 = T_now();
	T_log(T_QUIET, "%s", T_time_to_string_ns(t1 - t0, ts));
}

T_TEST_CASE(tick)
{
	T_ticks t;
	uint64_t i;

	t = T_tick();
	i = 0;

	do {
		++i;
	} while (T_tick() == t);

	T_gt_u64(i, 0);
}

T_TEST_CASE(time)
{
	T_time_string ts;
	T_time t;
	uint32_t s;
	uint32_t ns;

	t = T_seconds_and_nanoseconds_to_time(1, 123456789);
	T_eq_str(T_time_to_string_ns(t, ts), "1.123456789");
	T_eq_str(T_time_to_string_us(t, ts), "1.123456");
	T_eq_str(T_time_to_string_ms(t, ts), "1.123");
	T_eq_str(T_time_to_string_s(t, ts), "1");

	t = T_seconds_and_nanoseconds_to_time(1, 0);
	T_eq_str(T_time_to_string_ns(t, ts), "1.000000000");
	T_time_to_seconds_and_nanoseconds(t, &s, &ns);
	T_eq_u32(s, 1);
	T_eq_u32(ns, 0);

	t = T_seconds_and_nanoseconds_to_time(1, 1);
	T_eq_str(T_time_to_string_ns(t, ts), "1.000000001");
	T_time_to_seconds_and_nanoseconds(t, &s, &ns);
	T_eq_u32(s, 1);
	T_eq_u32(ns, 1);

	t = T_seconds_and_nanoseconds_to_time(1, 2);
	T_eq_str(T_time_to_string_ns(t, ts), "1.000000002");
	T_time_to_seconds_and_nanoseconds(t, &s, &ns);
	T_eq_u32(s, 1);
	T_eq_u32(ns, 2);

	t = T_seconds_and_nanoseconds_to_time(1, 3);
	T_eq_str(T_time_to_string_ns(t, ts), "1.000000003");
	T_time_to_seconds_and_nanoseconds(t, &s, &ns);
	T_eq_u32(s, 1);
	T_eq_u32(ns, 3);

	t = T_seconds_and_nanoseconds_to_time(1, 4);
	T_eq_str(T_time_to_string_ns(t, ts), "1.000000004");
	T_time_to_seconds_and_nanoseconds(t, &s, &ns);
	T_eq_u32(s, 1);
	T_eq_u32(ns, 4);

	t = T_seconds_and_nanoseconds_to_time(1, 5);
	T_eq_str(T_time_to_string_ns(t, ts), "1.000000005");
	T_time_to_seconds_and_nanoseconds(t, &s, &ns);
	T_eq_u32(s, 1);
	T_eq_u32(ns, 5);

	t = T_seconds_and_nanoseconds_to_time(1, 6);
	T_eq_str(T_time_to_string_ns(t, ts), "1.000000006");
	T_time_to_seconds_and_nanoseconds(t, &s, &ns);
	T_eq_u32(s, 1);
	T_eq_u32(ns, 6);

	t = T_seconds_and_nanoseconds_to_time(1, 7);
	T_eq_str(T_time_to_string_ns(t, ts), "1.000000007");
	T_time_to_seconds_and_nanoseconds(t, &s, &ns);
	T_eq_u32(s, 1);
	T_eq_u32(ns, 7);

	t = T_seconds_and_nanoseconds_to_time(1, 8);
	T_eq_str(T_time_to_string_ns(t, ts), "1.000000008");
	T_time_to_seconds_and_nanoseconds(t, &s, &ns);
	T_eq_u32(s, 1);
	T_eq_u32(ns, 8);

	t = T_seconds_and_nanoseconds_to_time(1, 9);
	T_eq_str(T_time_to_string_ns(t, ts), "1.000000009");
	T_time_to_seconds_and_nanoseconds(t, &s, &ns);
	T_eq_u32(s, 1);
	T_eq_u32(ns, 9);

	t = T_seconds_and_nanoseconds_to_time(1, 10);
	T_eq_str(T_time_to_string_ns(t, ts), "1.000000010");
	T_time_to_seconds_and_nanoseconds(t, &s, &ns);
	T_eq_u32(s, 1);
	T_eq_u32(ns, 10);

	t = T_seconds_and_nanoseconds_to_time(1, 999999999);
	T_eq_str(T_time_to_string_ns(t, ts), "1.999999999");
	T_time_to_seconds_and_nanoseconds(t, &s, &ns);
	T_eq_u32(s, 1);
	T_eq_u32(ns, 999999999);
}

T_TEST_CASE(ticks)
{
	T_time_string ts;
	T_time t;
	T_time r;
	T_ticks k;
	uint64_t f;
	uint32_t s;
	uint32_t ns;
	uint32_t m;
	size_t n;

	t = T_seconds_and_nanoseconds_to_time(1, 0);
	f = T_time_to_ticks(t);
	T_gt_u64(f, 0);

	r = T_ticks_to_time(1);
	T_time_to_seconds_and_nanoseconds(r, &s, &ns);
	T_eq_u32(s, 0);
	T_ne_u32(ns, 0);

	n = 1;
	m = 10;

	do {
		if (ns / m == 0) {
			break;
		}

		++n;
		m *= 10;
	} while (n < 10);

	n = 10 - n;

	t = T_seconds_and_nanoseconds_to_time(1, 123456789);
	k = T_time_to_ticks(t);

	n += 2;
	T_eq_nstr(T_ticks_to_string_ns(k, ts), "1.123456789", n);
	T_eq_nstr(T_ticks_to_string_us(k, ts), "1.123456", n);
	T_eq_nstr(T_ticks_to_string_ms(k, ts), "1.123", n);
	T_eq_str(T_ticks_to_string_s(k, ts), "1");
}

T_TEST_CASE(begin_time)
{
	T_time_string ts;
	T_time t0;
	T_time t1;
	T_time d;

	t1 = T_now();
	t0 = T_case_begin_time();
	d = t1 - t0;
	T_log(T_QUIET, "time at test case begin %s",
	    T_time_to_string_ns(d, ts));
}

/*
 * The license is at the end of the file to be able to use the test code and
 * output in examples in the documentation.  This is also the reason for the
 * dual licensing.  The license for RTEMS documentation is CC-BY-SA-4.0.
 */

/*
 * SPDX-License-Identifier: BSD-2-Clause OR CC-BY-SA-4.0
 *
 * Copyright (C) 2018, 2019 embedded brains GmbH
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
 *
 * ALTERNATIVELY, this software may be distributed under the terms of the
 * Creative Commons Attribution-ShareAlike 4.0 International Public License as
 * published by Creative Commons, PO Box 1866, Mountain View, CA 94042
 * (https://creativecommons.org/licenses/by-sa/4.0/legalcode).
 */
