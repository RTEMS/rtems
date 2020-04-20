/*
 *  @file
 *  @brief Test suite for fenv.h methods
 */

/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2020 Eshan Dhawan
 * Copyright (C) 2019 Vaibhav Gupta
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

/* header files are listed in lexical/lexicographical/alphabetical order */

#include <errno.h>
#include <limits.h>
#include <math.h>
#include <fenv.h>       /* contains declarations of fenv methods */
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <rtems/test.h>
#include <tmacros.h>
#include <float.h>

const char rtems_test_name[] = "PSXFENV 01";

/* forward declarations to avoid warnings */
rtems_task Init( rtems_task_argument ignored );

/* Test Function Begins */
rtems_task Init(rtems_task_argument ignored)
{
  double a, b, c;
  int r;
  fexcept_t excepts;
  TEST_BEGIN();

  /*
   * 'FE_ALL_EXCEPT' will be defined only when 'feclearexcept()',
   * fegetexceptflag() , feraiseexcept(), fesetexceptflag() and
   * fetestexcept() functions are supported by the architecture.
   * Hence their testcases can be wrapped under #ifdef and #endif.
   */
  #ifdef FE_ALL_EXCEPT /* floating-point exceptions */
    r = fesetenv( FE_DFL_ENV );
    if ( r ) {
      printf( "fesetenv ==> %d\n", r);
    }
    rtems_test_assert( r == 0 );

    /* Test feclearexcept() and fetestexcept() in one go. */
    r = feclearexcept( FE_ALL_EXCEPT );
    if ( r ) {
      printf( "feclearexcept ==> 0x%x\n", r );
    }
    rtems_test_assert( r == 0 );

    r = fetestexcept( FE_ALL_EXCEPT );
    if ( r ) {
      printf( "fetestexcept ==> 0x%x\n", r );
    }
    rtems_test_assert( r == 0 );

    /* Test 'FE_DIVBYZERO'
     * Divide by zero and confirm fetestexcept() */
    a = 0.0;
    b = 1.0;
    c = b/a;
    (void) c;
    /* Test fegetexceptflag() and fesetexceptflag() */
    r = fegetexceptflag( &excepts, FE_ALL_EXCEPT );
    if ( r ) {
      printf( "fegetexceptflag ==> 0x%x\n", r );
    }
    rtems_test_assert( r == 0 );

    r = fesetexceptflag( &excepts, FE_ALL_EXCEPT );
    if ( r ) {
      printf( "fesetexceptflag ==> 0x%x\n", r );
    }
    rtems_test_assert( r == 0 );

    /* Test for fegetround() and fesetround()
     * They have four main macros to be tested separated by ifdef
     * Since not all architectures support them
     * The test case gets and sets the rounding directions */
  #ifdef FE_TONEAREST /* Rounding direction TONEAREST */
    rtems_test_assert( fegetround() == FE_TONEAREST );
  #endif /*rounding direction TONEAREST */
  #ifdef FE_TOWARDZERO /* rounding direction TOWARDZERO */
    r = fesetround( FE_TOWARDZERO );
    if ( r ) {
      printf( "fesetround ==> 0x%x\n", r );
    }
    rtems_test_assert( r == 0 );
    rtems_test_assert( fegetround() == FE_TOWARDZERO );
  #endif/*rounding direction TOWARDZERO */
  #ifdef FE_DOWNWARD /* rounding direction DOWNWARD */
    r = fesetround( FE_DOWNWARD );
    if ( r ) {
      printf( "fesetround ==> 0x%x\n", r );
    }
    rtems_test_assert( r == 0 );
    rtems_test_assert( fegetround() == FE_DOWNWARD );
  #endif /* rounding direction DOWNWARD */
  #ifdef FE_UPWARD /* rounding direction UPWARD */
    r = fesetround( FE_UPWARD );
    if ( r ) {
      printf( "fesetround ==> 0x%x\n", r );
    }
    rtems_test_assert( r == 0 );
    rtems_test_assert( fegetround() == FE_UPWARD );
  #endif /* rounding direction upward */
  #ifdef FE_TONEAREST /* rounding direction TONEAREST */
    r = fesetround( FE_TONEAREST );
    if ( r ) {
      printf( "fesetround ==> 0x%x\n", r );
    }
    rtems_test_assert( r == 0 );
  #endif /* rounding direction TONEAREST */

  #ifdef FE_DIVBYZERO /* divide by zero exeption */
    r = feraiseexcept( FE_DIVBYZERO ) ;
    rtems_test_assert( fetestexcept( FE_DIVBYZERO ) );
  #endif /* divide by zero exeption */

    /* Test 'FE_INEXACT' */
    a = 10.0;
    c = b/a;

  #endif /* floating-point exceptions */

  TEST_END();
  rtems_test_exit(0);
}

/* NOTICE: the clock driver is explicitly disabled */

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS                  1

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 6

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
/* end of file */
