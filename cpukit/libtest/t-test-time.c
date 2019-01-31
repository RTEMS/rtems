/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2018 embedded brains GmbH
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

#undef __STRICT_ANSI__

#include <t.h>

#include <inttypes.h>
#include <stdatomic.h>
#include <stdio.h>
#include <time.h>

#ifdef __rtems__
#include <rtems/counter.h>
#include <rtems/score/timecounter.h>
#endif

#ifdef __rtems__
static T_time
round_sbt(T_time time)
{
	/*
	 * One 1ns consists of 4.30 fractions of 1/2**32.  Round up close to
	 * the middle.  This turns the conversion mapping of struct timespec to
	 * sbintime_t and back into the identity function.
	 */
	return time + 2;
}
#endif

const char *
T_time_to_string_ns(T_time time, T_time_string string)
{
	uint32_t s;
	uint32_t f;

#ifdef __rtems__
	time = round_sbt(time);
	s = (uint32_t)(time >> 32);
	f = (uint32_t)(((uint64_t)1000000000 * (uint32_t)time) >> 32);
#else
	s = (uint32_t)(time / 1000000000);
	f = (uint32_t)(time % 1000000000);
#endif

	(void)T_snprintf(string, sizeof(T_time_string),
	    "%" PRIu32 ".%09" PRIu32, s, f);
	return string;
}

const char *
T_time_to_string_us(T_time time, T_time_string string)
{
	uint32_t s;
	uint32_t f;

#ifdef __rtems__
	time = round_sbt(time);
	s = (uint32_t)(time >> 32);
	f = (uint32_t)(((uint64_t)1000000 * (uint32_t)time) >> 32);
#else
	time /= 1000;
	s = (uint32_t)(time / 1000000);
	f = (uint32_t)(time % 1000000);
#endif

	(void)T_snprintf(string, sizeof(T_time_string),
	    "%" PRIu32 ".%06" PRIu32, s, f);
	return string;
}

const char *
T_time_to_string_ms(T_time time, T_time_string string)
{
	uint32_t s;
	uint32_t f;

#ifdef __rtems__
	time = round_sbt(time);
	s = (uint32_t)(time >> 32);
	f = (uint32_t)(((uint64_t)1000 * (uint32_t)time) >> 32);
#else
	time /= 1000000;
	s = (uint32_t)(time / 1000);
	f = (uint32_t)(time % 1000);
#endif

	(void)T_snprintf(string, sizeof(T_time_string),
	    "%" PRIu32 ".%03" PRIu32, s, f);
	return string;
}

const char *
T_time_to_string_s(T_time time, T_time_string string)
{
	uint32_t s;

#ifdef __rtems__
	time = round_sbt(time);
	s = (uint32_t)(time >> 32);
#else
	s = (uint32_t)(time / 1000000000);
#endif

	(void)T_snprintf(string, sizeof(T_time_string), "%" PRIu32, s);
	return string;
}

const char *
T_ticks_to_string_ns(T_ticks ticks, T_time_string string)
{
	return T_time_to_string_ns(T_ticks_to_time(ticks), string);
}

const char *
T_ticks_to_string_us(T_ticks ticks, T_time_string string)
{
	return T_time_to_string_us(T_ticks_to_time(ticks), string);
}

const char *
T_ticks_to_string_ms(T_ticks ticks, T_time_string string)
{
	return T_time_to_string_ms(T_ticks_to_time(ticks), string);
}

const char *
T_ticks_to_string_s(T_ticks ticks, T_time_string string)
{
	return T_time_to_string_s(T_ticks_to_time(ticks), string);
}

uint64_t
T_ticks_to_time(T_ticks ticks)
{
#ifdef __rtems__
	return (uint64_t)rtems_counter_ticks_to_sbintime(ticks);
#else
	return ticks;
#endif
}

T_ticks
T_time_to_ticks(T_time time)
{
#ifdef __rtems__
	return rtems_counter_sbintime_to_ticks((sbintime_t)time);
#else
	return time;
#endif
}

T_time
T_seconds_and_nanoseconds_to_time(uint32_t s, uint32_t ns)
{
#ifdef __rtems__
	struct timespec ts;

	ts.tv_sec = s;
	ts.tv_nsec = (long)ns;
	return (T_time)tstosbt(ts);
#else
	return (T_time)s * (T_time)1000000000 + (T_time)ns;
#endif
}

void
T_time_to_seconds_and_nanoseconds(T_time time, uint32_t *s, uint32_t *ns)
{
#ifdef __rtems__
	time = round_sbt(time);
	*s = (uint32_t)(time >> 32);
	*ns = (uint32_t)(((uint64_t)1000000000 * (uint32_t)time) >> 32);
#else
	*s = (uint32_t)(time / 1000000000);
	*ns = (uint32_t)(time % 1000000000);
#endif
}

#ifndef __rtems__
T_time
T_now(void)
{
	struct timespec tp;

	(void)clock_gettime(CLOCK_MONOTONIC, &tp);
	return (T_time)tp.tv_sec * (T_time)1000000000 + (T_time)tp.tv_nsec;
}

T_ticks
T_tick(void)
{
	return T_now();
}
#endif

static atomic_uint dummy_time;

T_time
T_now_dummy(void)
{
	return atomic_fetch_add_explicit(&dummy_time, 1, memory_order_relaxed);
}

T_time
T_now_via_tick(void)
{
	return T_ticks_to_time(T_tick());
}
