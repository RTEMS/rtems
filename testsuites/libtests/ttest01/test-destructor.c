#include <t.h>

static void
destroy(T_destructor *dtor)
{
	(void)dtor;
	T_step(0, "destroy");
}

T_TEST_CASE(destructor)
{
	static T_destructor dtor;

	T_plan(1);
	T_add_destructor(&dtor, destroy);
}

#include "t-self-test.h"

T_TEST_OUTPUT(destructor,
"B:destructor\n"
"P:0:0:UI1:test-destructor.c:7\n"
"E:destructor:N:1:F:0:D:0.001000\n");

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
