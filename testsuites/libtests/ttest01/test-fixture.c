#include <rtems/test.h>

static int initial_value = 3;

static int counter;

static void
setup(void *ctx)
{
	int *c;

	T_log(T_QUIET, "setup begin");
	T_eq_ptr(ctx, &initial_value);
	T_eq_ptr(ctx, T_fixture_context());
	c = ctx;
	counter = *c;
	T_set_fixture_context(&counter);
	T_eq_ptr(&counter, T_fixture_context());
	T_log(T_QUIET, "setup end");
}

static void
stop(void *ctx)
{
	int *c;

	T_log(T_QUIET, "stop begin");
	T_eq_ptr(ctx, &counter);
	c = ctx;
	++(*c);
	T_log(T_QUIET, "stop end");
}

static void
teardown(void *ctx)
{
	int *c;

	T_log(T_QUIET, "teardown begin");
	T_eq_ptr(ctx, &counter);
	c = ctx;
	T_eq_int(*c, 4);
	T_log(T_QUIET, "teardown end");
}

static size_t
scope(void *ctx, char *buf, size_t n)
{
	(void) ctx;

	return T_str_copy(buf, "/More", n);
}

static const T_fixture fixture = {
	.setup = setup,
	.stop = stop,
	.teardown = teardown,
	.scope = scope,
	.initial_context = &initial_value
};

static int initial_value_2 = 7;

static int counter_2;

static void
setup_2(void *ctx)
{
	int *c;

	T_log(T_QUIET, "setup 2 begin");
	T_eq_ptr(ctx, &initial_value_2);
	T_eq_ptr(ctx, T_fixture_context());
	c = ctx;
	counter_2 = *c;
	T_set_fixture_context(&counter_2);
	T_eq_ptr(&counter_2, T_fixture_context());
	T_log(T_QUIET, "setup 2 end");
}

static void
stop_2(void *ctx)
{
	int *c;

	T_log(T_QUIET, "stop 2 begin");
	T_eq_ptr(ctx, &counter_2);
	c = ctx;
	++(*c);
	T_log(T_QUIET, "stop 2 end");
}

static void
teardown_2(void *ctx)
{
	int *c;

	T_log(T_QUIET, "teardown 2 begin");
	T_eq_ptr(ctx, &counter_2);
	c = ctx;
	T_eq_int(*c, 8);
	T_log(T_QUIET, "teardown 2 end");
}

static size_t
scope_2(void *ctx, char *buf, size_t n)
{
	(void) ctx;

	return T_str_copy(buf, "/AndMore", n);
}

static const T_fixture fixture_2 = {
	.setup = setup_2,
	.stop = stop_2,
	.teardown = teardown_2,
	.scope = scope_2,
	.initial_context = &initial_value_2
};

static T_fixture_node node;

T_TEST_CASE_FIXTURE(fixture, &fixture)
{
	void *ctx;

	T_assert_true(true, "all right");
	ctx = T_push_fixture(&node, &fixture_2);
	T_eq_ptr(ctx, &initial_value_2);
	++counter_2;
	T_pop_fixture();
	ctx = T_push_fixture(&node, &fixture_2);
	T_eq_ptr(ctx, &initial_value_2);
	T_assert_true(false, "test fails and we stop the test case");
	T_log(T_QUIET, "not reached");
}

#include "t-self-test.h"

T_TEST_OUTPUT(fixture,
"B:fixture\n"
"L:setup begin\n"
"P:0:0:UI1/More:test-fixture.c:13\n"
"P:1:0:UI1/More:test-fixture.c:14\n"
"P:2:0:UI1/More:test-fixture.c:18\n"
"L:setup end\n"
"P:3:0:UI1/More:test-fixture.c:125\n"
"L:setup 2 begin\n"
"P:4.0:0:UI1/More/AndMore:test-fixture.c:71\n"
"P:4.1:0:UI1/More/AndMore:test-fixture.c:72\n"
"P:4.2:0:UI1/More/AndMore:test-fixture.c:76\n"
"L:setup 2 end\n"
"P:4.3:0:UI1/More/AndMore:test-fixture.c:127\n"
"L:teardown 2 begin\n"
"P:4.4:0:UI1/More/AndMore:test-fixture.c:98\n"
"P:4.5:0:UI1/More/AndMore:test-fixture.c:100\n"
"L:teardown 2 end\n"
"L:setup 2 begin\n"
"P:4.0:0:UI1/More/AndMore:test-fixture.c:71\n"
"P:4.1:0:UI1/More/AndMore:test-fixture.c:72\n"
"P:4.2:0:UI1/More/AndMore:test-fixture.c:76\n"
"L:setup 2 end\n"
"P:4.3:0:UI1/More/AndMore:test-fixture.c:131\n"
"F:4.4:0:UI1/More/AndMore:test-fixture.c:132:test fails and we stop the test case\n"
"L:stop 2 begin\n"
"P:4.5:0:UI1/More/AndMore:test-fixture.c:86\n"
"L:stop 2 end\n"
"L:stop begin\n"
"P:4.6:0:UI1/More/AndMore:test-fixture.c:28\n"
"L:stop end\n"
"L:teardown 2 begin\n"
"P:4.7:0:UI1/More/AndMore:test-fixture.c:98\n"
"P:4.8:0:UI1/More/AndMore:test-fixture.c:100\n"
"L:teardown 2 end\n"
"L:teardown begin\n"
"P:4:0:UI1/More:test-fixture.c:40\n"
"P:5:0:UI1/More:test-fixture.c:42\n"
"L:teardown end\n"
"E:fixture:N:21:F:1:D:0.001000\n");

/*
 * The license is at the end of the file to be able to use the test code and
 * output in examples in the documentation.  This is also the reason for the
 * dual licensing.  The license for RTEMS documentation is CC-BY-SA-4.0.
 */

/*
 * SPDX-License-Identifier: BSD-2-Clause OR CC-BY-SA-4.0
 *
 * Copyright (C) 2018, 2020 embedded brains GmbH & Co. KG
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
