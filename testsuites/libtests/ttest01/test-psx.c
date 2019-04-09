#include <t.h>

#include <sys/stat.h>
#include <errno.h>

T_TEST_CASE(stat)
{
	struct stat st;
	int rv;

	errno = 0;
	rv = stat("foobar", &st);
	T_psx_error(rv, ENOENT);
}

T_TEST_CASE(test_psx_error)
{
	errno = ENOMEM;
	T_psx_error(-1, ENOMEM);
	T_psx_error(-1, ERANGE);
	T_psx_error(0, ENOMEM);
	T_quiet_psx_error(-1, ENOMEM);
	T_quiet_psx_error(-1, ERANGE);
	T_quiet_psx_error(0, ENOMEM);
	T_assert_psx_error(-1, ENOMEM);
	T_assert_psx_error(0, ENOMEM);
}

T_TEST_CASE(test_psx_success)
{
	T_psx_success(0);
	T_psx_success(-1);
	T_quiet_psx_success(0);
	T_quiet_psx_success(-1);
	T_assert_psx_success(0);
	T_assert_psx_success(-1);
}

#include "t-self-test.h"

T_TEST_OUTPUT(stat,
"B:stat\n"
"P:0:0:UI1:test-psx.c:13\n"
"E:stat:N:1:F:0:D:0.001000\n");

T_TEST_OUTPUT(test_psx_error,
"B:test_psx_error\n"
"P:0:0:UI1:test-psx.c:19\n"
"F:1:0:UI1:test-psx.c:20:-1 == -1, ENOMEM == ERANGE\n"
"F:2:0:UI1:test-psx.c:21:0 == -1, ENOMEM == ENOMEM\n"
"F:*:0:UI1:test-psx.c:23:-1 == -1, ENOMEM == ERANGE\n"
"F:*:0:UI1:test-psx.c:24:0 == -1, ENOMEM == ENOMEM\n"
"P:3:0:UI1:test-psx.c:25\n"
"F:4:0:UI1:test-psx.c:26:0 == -1, ENOMEM == ENOMEM\n"
"E:test_psx_error:N:5:F:5:D:0.001000\n");

T_TEST_OUTPUT(test_psx_success,
"B:test_psx_success\n"
"P:0:0:UI1:test-psx.c:31\n"
"F:1:0:UI1:test-psx.c:32:-1 == 0, 0\n"
"F:*:0:UI1:test-psx.c:34:-1 == 0, 0\n"
"P:2:0:UI1:test-psx.c:35\n"
"F:3:0:UI1:test-psx.c:36:-1 == 0, 0\n"
"E:test_psx_success:N:4:F:3:D:0.001000\n");

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
