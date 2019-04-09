#include <t.h>

T_TEST_CASE(wrong_step)
{
	T_plan(2);
	T_step_true(0, true, "all right");
	T_step_true(2, true, "wrong step");
}

T_TEST_CASE(plan_ok)
{
	T_plan(1);
	T_step_true(0, true, "all right");
}

T_TEST_CASE(plan_failed)
{
	T_plan(2);
	T_step_true(0, true, "not enough steps");
	T_quiet_true(true, "quiet test do not count");
}

T_TEST_CASE(double_plan)
{
	T_plan(99);
	T_plan(2);
}

T_TEST_CASE(steps)
{
	T_step(0, "a");
	T_plan(3);
	T_step(1, "b");
	T_step(2, "c");
}

#include "t-self-test.h"

T_TEST_OUTPUT(wrong_step,
"B:wrong_step\n"
"P:0:0:UI1:test-plan.c:6\n"
"F:1:0:UI1:test-plan.c:7:planned step (2)\n"
"E:wrong_step:N:2:F:1:D:0.001000\n");

T_TEST_OUTPUT(plan_ok,
"B:plan_ok\n"
"P:0:0:UI1:test-plan.c:13\n"
"E:plan_ok:N:1:F:0:D:0.001000\n");

T_TEST_OUTPUT(plan_failed,
"B:plan_failed\n"
"P:0:0:UI1:test-plan.c:19\n"
"F:*:0:UI1:*:*:actual steps (1), planned steps (2)\n"
"E:plan_failed:N:1:F:1:D:0.001000\n");

T_TEST_OUTPUT(double_plan,
"B:double_plan\n"
"F:*:0:UI1:*:*:planned steps (99) already set\n"
"E:double_plan:N:0:F:1:D:0.001000\n");

T_TEST_OUTPUT(steps,
"B:steps\n"
"P:0:0:UI1:test-plan.c:31\n"
"P:1:0:UI1:test-plan.c:33\n"
"P:2:0:UI1:test-plan.c:34\n"
"E:steps:N:3:F:0:D:0.001000\n");

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
