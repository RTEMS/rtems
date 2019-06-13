/**
 *  @file
 *  @brief Test suite for inttypes.h methods
 */

/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2019, Aditya Upadhyay and Vaibhav Gupta
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

#include <rtems/test.h>
#include <inttypes.h>
#include <stdio.h>
#include <errno.h>
#include <tmacros.h>
#include <stdint.h>
#include <stddef.h>

const char rtems_test_name[] = "PSXINTTYPE 01";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument ignored);

rtems_task Init(rtems_task_argument ignored)
{
  int       base            = 10;
  int       invalid_base    = 40;

  char     *nptr1_p         = "123abc";
  char 	   *nptr1_p_errange = "9999999999999999999999";
  char     *nptr1_n         = "-123abc";
  char 	   *nptr1_n_errange = "-9999999999999999999999";
  char     *endptr1         = NULL;

  wchar_t  *nptr2_p         = L"123junk";
  wchar_t  *nptr2_p_errange = L"9999999999999999999999";
  wchar_t  *nptr2_n         = L"-123junk";
  wchar_t  *nptr2_n_errange = L"-9999999999999999999999";
  wchar_t  *endptr2         = NULL;

  intmax_t  result_strtoimax;
  uintmax_t result_strtoumax;

  TEST_BEGIN();

  /* Test for strtoimax  */
  puts( "\nstrtoimax Testcases...." );
  puts( "Valid Inputs - Positive Number" );
  result_strtoimax = strtoimax( nptr1_p, &endptr1, base );
  rtems_test_assert( result_strtoimax == 123 );

  puts( "Final string pointed by endptr" );
  rtems_test_assert( endptr1 == ( nptr1_p + 3 ) );

  puts( "Valid Inputs - Negative Number" );
  result_strtoimax = strtoimax( nptr1_n, &endptr1, base );
  rtems_test_assert( result_strtoimax == -123 );

  puts( "Final string pointed by endptr" );
  rtems_test_assert( endptr1 == ( nptr1_n + 4 ) );

  puts( "Valid Input - Positive Number - Number out of Range" );
  result_strtoimax = strtoimax( nptr1_p_errange, &endptr1, base );
  rtems_test_assert( result_strtoimax == INTMAX_MAX );
  rtems_test_assert( errno == ERANGE );

  puts( "Valid Input - Negative Number - Number out of Range" );
  result_strtoimax = strtoimax( nptr1_n_errange, &endptr1, base );
  rtems_test_assert( result_strtoimax == INTMAX_MIN );
  rtems_test_assert( errno == ERANGE );

  puts( "Invalid Input - Send NULL Pointer" );
  result_strtoimax = strtoimax( NULL, &endptr1, base );
  rtems_test_assert( result_strtoimax == 0 );
  rtems_test_assert( errno == EINVAL );

  puts( "Invalid Input - Invalid base - Use base = 40" );
  result_strtoimax = strtoimax( nptr1_p, &endptr1, invalid_base );
  rtems_test_assert( result_strtoimax == 0 );
  rtems_test_assert( errno == EINVAL );

  /* Test for strtoumax  */
  puts( "\nstrtoumax Testcases...." );
  puts( "Valid Inputs - Positive Number" );
  result_strtoumax = strtoumax( nptr1_p, &endptr1, base );
  rtems_test_assert( result_strtoumax ==123 );

  puts( "Final string pointed by endptr" );
  rtems_test_assert( endptr1 == ( nptr1_p + 3 ) );

  puts( "Valid Inputs - Negative Number" );
  result_strtoumax = strtoumax( nptr1_n, &endptr1, base );
  rtems_test_assert( result_strtoumax != 0 );

  puts( "Final string pointed by endptr" );
  rtems_test_assert( endptr1 == ( nptr1_n + 4 ) );

  puts( "Valid Input - Positive Number - Number out of Range" );
  result_strtoumax = strtoumax( nptr1_p_errange, &endptr1, base );
  rtems_test_assert( result_strtoumax == UINTMAX_MAX );
  rtems_test_assert( errno == ERANGE );

  puts( "Valid Input - Negative Number - Number out of Range" );
  result_strtoumax = strtoumax( nptr1_n_errange, &endptr1, base );
  rtems_test_assert( result_strtoumax != 0 );
  rtems_test_assert( errno == ERANGE );

  puts( "Invalid Input - Send NULL Pointer" );
  result_strtoumax = strtoumax( NULL, &endptr1, base );
  rtems_test_assert( result_strtoumax == 0 );
  rtems_test_assert( errno == EINVAL );

  puts( "Invalid Input - Invalid base - Use base = 40" );
  result_strtoumax = strtoumax( nptr1_p, &endptr1, invalid_base );
  rtems_test_assert( result_strtoumax == 0 );
  rtems_test_assert( errno == EINVAL );

  /* Test for wcstoimax  */
  puts( "\nwcstoimax Testcases...." );
  puts( "Valid Inputs - Positive Number" );
  result_strtoimax = wcstoimax( nptr2_p, &endptr2, base );
  rtems_test_assert( result_strtoimax == 123 );

  puts( "Final string pointed by endptr" );
  rtems_test_assert( endptr2 == ( nptr2_p + 3 ) );

  puts( "Valid Inputs - Negative Number" );
  result_strtoimax = wcstoimax( nptr2_n, &endptr2, base );
  rtems_test_assert( result_strtoimax == -123 );

  puts( "Final string pointed by endptr" );
  rtems_test_assert( endptr2 == ( nptr2_n + 4 ) );

  puts( "Valid Input - Positive Number - Number out of Range" );
  result_strtoimax = wcstoimax( nptr2_p_errange, &endptr2, base );
  rtems_test_assert( result_strtoimax == INTMAX_MAX );
  rtems_test_assert( errno == ERANGE );

  puts( "Valid Input - Negative Number - Number out of Range" );
  result_strtoimax = wcstoimax( nptr2_n_errange, &endptr2, base );
  rtems_test_assert( result_strtoimax == INTMAX_MIN );
  rtems_test_assert( errno == ERANGE );

  puts( "Invalid Input - Send NULL Pointer" );
  result_strtoimax = wcstoimax( NULL, &endptr2, base );
  rtems_test_assert( result_strtoimax == 0 );
  rtems_test_assert( errno == EINVAL );

  puts( "Invalid Input - Invalid base - Use base = 40" );
  result_strtoimax = wcstoimax( nptr2_p, &endptr2, invalid_base );
  rtems_test_assert( result_strtoimax == 0 );
  rtems_test_assert( errno == EINVAL );

  /* Test for wcstoumax  */
  puts( "\nwcstoumax Testcases...." );
  puts( "Valid Inputs - Positive Number" );
  result_strtoumax = wcstoumax( nptr2_p, &endptr2, base );
  rtems_test_assert( result_strtoumax == 123 );

  puts( "Final string pointed by endptr" );
  rtems_test_assert( endptr2 == ( nptr2_p + 3 ) );

  puts( "Valid Inputs - Negative Number" );
  result_strtoumax = wcstoumax( nptr2_n, &endptr2, base );
  rtems_test_assert( result_strtoumax != 0 );

  puts( "Final string pointed by endptr" );
  rtems_test_assert( endptr2 == ( nptr2_n + 4 ) );

  puts( "Valid Input - Positive Number - Number out of Range" );
  result_strtoumax = wcstoumax( nptr2_p_errange, &endptr2, base );
  rtems_test_assert( result_strtoumax == UINTMAX_MAX );
  rtems_test_assert( errno == ERANGE );

  puts( "Valid Input - Negative Number - Number out of Range" );
  result_strtoumax = wcstoumax( nptr2_n_errange, &endptr2, base );
  rtems_test_assert( result_strtoumax != 0 );
  rtems_test_assert( errno == ERANGE );

  puts( "Invalid Input - Send NULL Pointer" );
  result_strtoumax = wcstoumax( NULL, &endptr2, base );
  rtems_test_assert( result_strtoumax == 0 );
  rtems_test_assert( errno == EINVAL );

  puts( "Invalid Input - Invalid base - Use base = 40" );
  result_strtoumax = wcstoumax( nptr2_p, &endptr2, invalid_base );
  rtems_test_assert( result_strtoumax == 0 );
  rtems_test_assert( errno == EINVAL );

  TEST_END();
  rtems_test_exit(0);

}

/* NOTICE: the clock driver is explicitly disabled */

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS            1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
