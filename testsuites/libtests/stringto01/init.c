/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "tmacros.h"
#include <rtems/stringto.h>
#include <limits.h>
#include <math.h>

#include <stdio.h>

const char rtems_test_name[] = "STRINGTO 1";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);

#define __STRING(x)     #x              /* stringify without expanding x */
#define __XSTRING(x)    __STRING(x)     /* expand x, then stringify */

static int get_base_10_or_16(const char *s)
{
  return (s[0] == '0' && s[1] == 'x') ? 16 : 10;
}

/* c99 has LLONG_MAX instead of LONG_LONG_MAX */
#ifndef LONG_LONG_MAX
#define LONG_LONG_MAX	LLONG_MAX
#endif
/* c99 has LLONG_MIN instead of LONG_LONG_MIN */
#ifndef LONG_LONG_MIN
#define LONG_LONG_MIN	LLONG_MIN
#endif

/* Test pointer conversions */
#define TEST_STRING_TO_TYPE          void *
#define TEST_STRING_TO_NAME          test_rtems_string_to_pointer
#define STRING_TO_NAME_METHOD        rtems_string_to_pointer
#define STRING_TO_NAME_METHOD_STRING "rtems_string_to_pointer"
#define STRING_TO_POINTER
#include "stringto_test_template.h"

/* Test unsigned char conversions */
#define TEST_STRING_TO_TYPE          unsigned char
#define TEST_STRING_TO_NAME          test_rtems_string_to_unsigned_char
#define STRING_TO_NAME_METHOD        rtems_string_to_unsigned_char
#define STRING_TO_NAME_METHOD_STRING "rtems_string_to_unsigned_char"
#define TEST_TOO_LARGE_STRING        "987654321123456789123456789"
#define TEST_TOO_LARGE_FOR_UCHAR     "256" 
#define STRING_TO_INTEGER
#include "stringto_test_template.h"

/* Test integer conversions */
#define TEST_STRING_TO_TYPE          int
#define STRING_TO_MAX                INT_MAX
#define STRING_TO_MAX_STRING         __XSTRING(INT_MAX)
#define TEST_STRING_TO_NAME          test_rtems_string_to_int
#define STRING_TO_NAME_METHOD        rtems_string_to_int
#define STRING_TO_NAME_METHOD_STRING "rtems_string_to_int"
#define TEST_TOO_LARGE_STRING        "987654321123456789123456789"
#define TEST_TOO_SMALL_STRING        "-98765432198765432123456789"
#define STRING_TO_INTEGER
#include "stringto_test_template.h"

/* Test unsigned int conversions */
#define TEST_STRING_TO_TYPE          unsigned int
#define TEST_STRING_TO_NAME          test_rtems_string_to_unsigned_int
#define STRING_TO_NAME_METHOD        rtems_string_to_unsigned_int
#define STRING_TO_NAME_METHOD_STRING "rtems_string_to_unsigned_int"
#define TEST_TOO_LARGE_STRING        "987654321123456789123456789"
#define STRING_TO_INTEGER
#include "stringto_test_template.h"

/* Test long conversions */
#define TEST_STRING_TO_TYPE          long
#define STRING_TO_MAX                LONG_MAX
#define STRING_TO_MAX_STRING         __XSTRING(LONG_MAX)
#define TEST_STRING_TO_NAME          test_rtems_string_to_long
#define STRING_TO_NAME_METHOD        rtems_string_to_long
#define STRING_TO_NAME_METHOD_STRING "rtems_string_to_long"
#define TEST_TOO_LARGE_STRING        "987654321123456789123456789"
#define TEST_TOO_SMALL_STRING        "-98765432198765432123456789"
#define STRING_TO_INTEGER
#include "stringto_test_template.h"

/* Test unsigned long conversions */
#define TEST_STRING_TO_TYPE          unsigned long
#define TEST_STRING_TO_NAME          test_rtems_string_to_unsigned_long
#define STRING_TO_NAME_METHOD        rtems_string_to_unsigned_long
#define STRING_TO_NAME_METHOD_STRING "rtems_string_to_unsigned_long"
#define TEST_TOO_LARGE_STRING        "987654321123456789123456789"
#define STRING_TO_INTEGER
#include "stringto_test_template.h"

/* Test long long conversions */
#define TEST_STRING_TO_TYPE          long long
#define STRING_TO_MAX                LONG_LONG_MAX
#define STRING_TO_MAX_STRING         __XSTRING(LONG_LONG_MAX)
#define TEST_STRING_TO_NAME          test_rtems_string_to_long_long
#define STRING_TO_NAME_METHOD        rtems_string_to_long_long
#define STRING_TO_NAME_METHOD_STRING "rtems_string_to_long_long"
#define TEST_TOO_LARGE_STRING        "987654321123456789123456789"
#define TEST_TOO_SMALL_STRING        "-98765432198765432123456789"
#define STRING_TO_INTEGER
#include "stringto_test_template.h"

/* Test unsigned long long conversions */
#define TEST_STRING_TO_TYPE          unsigned long long
#define TEST_STRING_TO_NAME          test_rtems_string_to_unsigned_long_long
#define STRING_TO_NAME_METHOD        rtems_string_to_unsigned_long_long
#define STRING_TO_NAME_METHOD_STRING "rtems_string_to_unsigned_long_long"
#define TEST_TOO_LARGE_STRING        "987654321123456789123456789"
#define STRING_TO_INTEGER
#include "stringto_test_template.h"

/* Test float conversions */
#define TEST_STRING_TO_TYPE          float
#define TEST_STRING_TO_NAME          test_rtems_string_to_float
#define STRING_TO_NAME_METHOD        rtems_string_to_float
#define STRING_TO_NAME_METHOD_STRING "rtems_string_to_float"
#define TEST_TOO_LARGE_STRING        "9.87654321123456789123456789E10240"
#define TEST_TOO_SMALL_STRING        "-9.87654321123456789123456789E10240"
#define STRING_TO_FLOAT
#include "stringto_test_template.h"

/* Test double conversions */
#define TEST_STRING_TO_TYPE          double
#define TEST_STRING_TO_NAME          test_rtems_string_to_double
#define STRING_TO_NAME_METHOD        rtems_string_to_double
#define STRING_TO_NAME_METHOD_STRING "rtems_string_to_double"
#define TEST_TOO_LARGE_STRING        "9.87654321123456789123456789E10240"
#define TEST_TOO_SMALL_STRING        "-9.87654321123456789123456789E10240"
#define STRING_TO_FLOAT
#include "stringto_test_template.h"

rtems_task Init(
  rtems_task_argument ignored
)
{
  TEST_BEGIN();
  test_rtems_string_to_pointer();
  test_rtems_string_to_unsigned_char();
  test_rtems_string_to_int();
  test_rtems_string_to_unsigned_int();
  test_rtems_string_to_long();
  test_rtems_string_to_unsigned_long();
  test_rtems_string_to_long_long();
  test_rtems_string_to_unsigned_long_long();

  test_rtems_string_to_float();
  test_rtems_string_to_double();
  TEST_END();
  rtems_test_exit(0);
}

/* NOTICE: the clock driver is explicitly disabled */
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS            1

#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
