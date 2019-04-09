#include <t.h>

T_TEST_CASE(assert)
{
	T_assert_true(true, "nix");
	T_assert_true(false, "test fails and we stop the test case");
	T_log(T_QUIET, "not reached");
}

static int counter;

static void
setup(void *ctx)
{
	int *c;

	T_log(T_QUIET, "setup");
	T_eq_ptr(ctx, &counter);
	c = ctx;
	*c = 0;
}

static void
stop(void *ctx)
{
	int *c;

	T_log(T_QUIET, "stop");
	T_eq_ptr(ctx, &counter);
	c = ctx;
	++(*c);
}

static void
teardown(void *ctx)
{
	int *c;

	T_log(T_QUIET, "teardown");
	T_eq_ptr(ctx, &counter);
	c = ctx;
	T_eq_int(*c, 1);
}

static const T_fixture fixture = {
	.setup = setup,
	.stop = stop,
	.teardown = teardown,
	.initial_context = &counter
};

T_TEST_CASE_FIXTURE(assert2, &fixture)
{
	T_assert_true(true, "nix");
	T_assert_true(false, "test fails and we stop the test case");
	T_log(T_QUIET, "not reached");
}

#include "t-self-test.h"

T_TEST_OUTPUT(assert,
"B:assert\n"
"P:0:0:UI1:test-assert.c:5\n"
"F:1:0:UI1:test-assert.c:6:test fails and we stop the test case\n"
"E:assert:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(assert2,
"B:assert2\n"
"L:setup\n"
"P:0:0:UI1:test-assert.c:18\n"
"P:1:0:UI1:test-assert.c:54\n"
"F:2:0:UI1:test-assert.c:55:test fails and we stop the test case\n"
"L:stop\n"
"P:3:0:UI1:test-assert.c:29\n"
"L:teardown\n"
"P:4:0:UI1:test-assert.c:40\n"
"P:5:0:UI1:test-assert.c:42\n"
"E:assert2:N:6:F:1:D:0.001000\n");

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
