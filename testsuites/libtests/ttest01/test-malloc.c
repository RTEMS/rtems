#include <t.h>

T_TEST_CASE(malloc_free)
{
	void *p;

	p = T_malloc(1);
	T_assert_not_null(p);
	T_free(p);
}

T_TEST_CASE(malloc_auto)
{
	void *p;

	p = T_malloc(1);
	T_assert_not_null(p);
}

static void
destroy(void *p)
{
	int *i;

	i = p;
	T_step_eq_int(2, *i, 1);
}

T_TEST_CASE(zalloc_auto)
{
	int *i;

	T_plan(3);
	i = T_zalloc(sizeof(*i), destroy);
	T_step_assert_not_null(0, i);
	T_step_eq_int(1, *i, 0);
	*i = 1;
}

T_TEST_CASE(malloc_huge)
{
	void *p;

	p = T_malloc(SIZE_MAX);
	T_null(p);
}

T_TEST_CASE(calloc_auto)
{
	int *i;

	i = T_calloc(1, sizeof(*i));
	T_assert_not_null(i);
	T_eq_int(*i, 0);
}

#include "t-self-test.h"

T_TEST_OUTPUT(malloc_free,
"B:malloc_free\n"
"P:0:0:UI1:test-malloc.c:8\n"
"E:malloc_free:N:1:F:0:D:0.001000\n");

T_TEST_OUTPUT(malloc_auto,
"B:malloc_auto\n"
"P:0:0:UI1:test-malloc.c:17\n"
"E:malloc_auto:N:1:F:0:D:0.001000\n");

T_TEST_OUTPUT(zalloc_auto,
"B:zalloc_auto\n"
"P:0:0:UI1:test-malloc.c:35\n"
"P:1:0:UI1:test-malloc.c:36\n"
"P:2:0:UI1:test-malloc.c:26\n"
"E:zalloc_auto:N:3:F:0:D:0.001000\n");

T_TEST_OUTPUT(malloc_huge,
"B:malloc_huge\n"
"P:0:0:UI1:test-malloc.c:45\n"
"E:malloc_huge:N:1:F:0:D:0.001000\n");

T_TEST_OUTPUT(calloc_auto,
"B:calloc_auto\n"
"P:0:0:UI1:test-malloc.c:53\n"
"P:1:0:UI1:test-malloc.c:54\n"
"E:calloc_auto:N:2:F:0:D:0.001000\n");

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
