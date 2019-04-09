#include <t.h>
#include <errno.h>

T_TEST_CASE(test_eno)
{
	T_eno(ENOMEM, ENOMEM);
	T_eno(ENOMEM, ERANGE);
	T_quiet_eno(ENOMEM, ENOMEM);
	T_quiet_eno(ENOMEM, ERANGE);
	T_assert_eno(ENOMEM, ENOMEM);
	T_assert_eno(ENOMEM, ERANGE);
}

T_TEST_CASE(test_eno_success)
{
	T_eno_success(0);
	T_eno_success(ENOMEM);
	T_quiet_eno_success(0);
	T_quiet_eno_success(ENOMEM);
	T_assert_eno_success(0);
	T_assert_eno_success(ENOMEM);
}

#include "t-self-test.h"

T_TEST_OUTPUT(test_eno,
"B:test_eno\n"
"P:0:0:UI1:test-eno.c:6\n"
"F:1:0:UI1:test-eno.c:7:ENOMEM == ERANGE\n"
"F:*:0:UI1:test-eno.c:9:ENOMEM == ERANGE\n"
"P:2:0:UI1:test-eno.c:10\n"
"F:3:0:UI1:test-eno.c:11:ENOMEM == ERANGE\n"
"E:test_eno:N:4:F:3:D:0.001000\n");

T_TEST_OUTPUT(test_eno_success,
"B:test_eno_success\n"
"P:0:0:UI1:test-eno.c:16\n"
"F:1:0:UI1:test-eno.c:17:ENOMEM == 0\n"
"F:*:0:UI1:test-eno.c:19:ENOMEM == 0\n"
"P:2:0:UI1:test-eno.c:20\n"
"F:3:0:UI1:test-eno.c:21:ENOMEM == 0\n"
"E:test_eno_success:N:4:F:3:D:0.001000\n");

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
