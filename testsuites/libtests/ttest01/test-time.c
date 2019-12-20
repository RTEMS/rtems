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

#include "t-self-test.h"

T_TEST_OUTPUT(time_to_string,
"B:time_to_string\n"
"P:0:0:UI1:test-time.c:11\n"
"P:1:0:UI1:test-time.c:12\n"
"P:2:0:UI1:test-time.c:13\n"
"P:3:0:UI1:test-time.c:14\n"
"P:4:0:UI1:test-time.c:17\n"
"P:5:0:UI1:test-time.c:18\n"
"E:time_to_string:N:6:F:0:D:0.001000\n");

T_TEST_OUTPUT(now,
"B:now\n"
"L:0.001000000\n"
"E:now:N:0:F:0:D:0.003000\n");

T_TEST_OUTPUT(tick,
"B:tick\n"
"P:0:0:UI1:test-time.c:44\n"
"E:tick:N:1:F:0:D:0.001000\n");

T_TEST_OUTPUT(time,
"B:time\n"
"P:0:0:UI1:test-time.c:55\n"
"P:1:0:UI1:test-time.c:56\n"
"P:2:0:UI1:test-time.c:57\n"
"P:3:0:UI1:test-time.c:58\n"
"P:4:0:UI1:test-time.c:61\n"
"P:5:0:UI1:test-time.c:63\n"
"P:6:0:UI1:test-time.c:64\n"
"P:7:0:UI1:test-time.c:67\n"
"P:8:0:UI1:test-time.c:69\n"
"P:9:0:UI1:test-time.c:70\n"
"P:10:0:UI1:test-time.c:73\n"
"P:11:0:UI1:test-time.c:75\n"
"P:12:0:UI1:test-time.c:76\n"
"P:13:0:UI1:test-time.c:79\n"
"P:14:0:UI1:test-time.c:81\n"
"P:15:0:UI1:test-time.c:82\n"
"P:16:0:UI1:test-time.c:85\n"
"P:17:0:UI1:test-time.c:87\n"
"P:18:0:UI1:test-time.c:88\n"
"P:19:0:UI1:test-time.c:91\n"
"P:20:0:UI1:test-time.c:93\n"
"P:21:0:UI1:test-time.c:94\n"
"P:22:0:UI1:test-time.c:97\n"
"P:23:0:UI1:test-time.c:99\n"
"P:24:0:UI1:test-time.c:100\n"
"P:25:0:UI1:test-time.c:103\n"
"P:26:0:UI1:test-time.c:105\n"
"P:27:0:UI1:test-time.c:106\n"
"P:28:0:UI1:test-time.c:109\n"
"P:29:0:UI1:test-time.c:111\n"
"P:30:0:UI1:test-time.c:112\n"
"P:31:0:UI1:test-time.c:115\n"
"P:32:0:UI1:test-time.c:117\n"
"P:33:0:UI1:test-time.c:118\n"
"P:34:0:UI1:test-time.c:121\n"
"P:35:0:UI1:test-time.c:123\n"
"P:36:0:UI1:test-time.c:124\n"
"P:37:0:UI1:test-time.c:127\n"
"P:38:0:UI1:test-time.c:129\n"
"P:39:0:UI1:test-time.c:130\n"
"E:time:N:40:F:0:D:0.001000\n");

T_TEST_OUTPUT(ticks,
"B:ticks\n"
"P:0:0:UI1:test-time.c:147\n"
"P:1:0:UI1:test-time.c:151\n"
"P:2:0:UI1:test-time.c:152\n"
"P:3:0:UI1:test-time.c:172\n"
"P:4:0:UI1:test-time.c:173\n"
"P:5:0:UI1:test-time.c:174\n"
"P:6:0:UI1:test-time.c:175\n"
"E:ticks:N:7:F:0:D:0.001000\n");

T_TEST_OUTPUT(begin_time,
"B:begin_time\n"
"L:time at test case begin 0.001000000\n"
"E:begin_time:N:0:F:0:D:0.002000\n");

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
