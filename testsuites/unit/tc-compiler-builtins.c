/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CompilerUnitBuiltins
 */

/*
 * Copyright (C) 2023 embedded brains GmbH & Co. KG
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

/*
 * This file is part of the RTEMS quality process and was automatically
 * generated.  If you find something that needs to be fixed or
 * worded better please post a report or patch to an RTEMS mailing list
 * or raise a bug report:
 *
 * https://www.rtems.org/bugs.html
 *
 * For information on updating and regenerating please refer to the How-To
 * section in the Software Requirements Engineering chapter of the
 * RTEMS Software Engineering manual.  The manual is provided as a part of
 * a release.  For development sources please refer to the online
 * documentation at:
 *
 * https://docs.rtems.org
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <setjmp.h>
#include <stdint.h>

#include "../validation/tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup CompilerUnitBuiltins spec:/compiler/unit/builtins
 *
 * @ingroup TestsuitesUnitNoClock0
 *
 * @brief These unit tests check compiler builtins.
 *
 * Explicitly test the 64-bit integer division and modulo operations.  They are
 * essential for the timekeeping services.  On most 32-bit targets, they need a
 * software implementation.
 *
 * This test case performs the following actions:
 *
 * - Check the return value of __builtin_clz() for a sample set of inputs.
 *
 * - Check the return value of __builtin_clzll() for a sample set of inputs.
 *
 * - Check the return value of __builtin_ctz() for a sample set of inputs.
 *
 * - Check signed 64-bit divisions for a sample set of values.
 *
 * - Check unsigned 64-bit divisions for a sample set of values.
 *
 * - Check signed 64-bit modulo operations for a sample set of values.
 *
 * - Check unsigned 64-bit modulo operations for a sample set of values.
 *
 * @{
 */

#if __LONG_MAX__ == 0x7fffffffL
uint64_t __udivmoddi4( uint64_t n, uint64_t d, uint64_t *r );
#endif

static bool do_longjmp;

static jmp_buf exception_return_context;

static void Fatal(
  rtems_fatal_source source,
  rtems_fatal_code   code,
  void              *arg
)
{
  (void) code;

  if ( source == RTEMS_FATAL_SOURCE_EXCEPTION && do_longjmp ) {
    do_longjmp = false;
    _ISR_Set_level( 0 );
    longjmp( arg, 1 );
  }
}

static void CompilerUnitBuiltins_Setup( void *ctx )
{
  SetFatalHandler( Fatal, exception_return_context );
}

static void CompilerUnitBuiltins_Teardown( void *ctx )
{
  SetFatalHandler( NULL, NULL );
}

static T_fixture CompilerUnitBuiltins_Fixture = {
  .setup = CompilerUnitBuiltins_Setup,
  .stop = NULL,
  .teardown = CompilerUnitBuiltins_Teardown,
  .scope = NULL,
  .initial_context = NULL
};

/**
 * @brief Check the return value of __builtin_clz() for a sample set of inputs.
 */
static void CompilerUnitBuiltins_Action_0( void )
{
  volatile unsigned int n;

  n = 1U;
  T_eq_int( __builtin_clz( n ), 31 );

  n = 1U << 31;
  T_eq_int( __builtin_clz( n ), 0 );

  n = ~0U;
  T_eq_int( __builtin_clz( n ), 0 );
}

/**
 * @brief Check the return value of __builtin_clzll() for a sample set of
 *   inputs.
 */
static void CompilerUnitBuiltins_Action_1( void )
{
  volatile unsigned long long n;

  n = 1ULL;
  T_eq_int( __builtin_clzll( n ), 63 );

  n = 1ULL << 31;
  T_eq_int( __builtin_clzll( n ), 32 );

  n = 1ULL << 32;
  T_eq_int( __builtin_clzll( n ), 31 );

  n = 1ULL << 63;
  T_eq_int( __builtin_clzll( n ), 0 );

  n = ~0ULL;
  T_eq_int( __builtin_clzll( n ), 0 );
}

/**
 * @brief Check the return value of __builtin_ctz() for a sample set of inputs.
 */
static void CompilerUnitBuiltins_Action_2( void )
{
  volatile int n;

  n = 1;
  T_eq_int( __builtin_ctz( n ), 0 );

  n = 1 << 31;
  T_eq_int( __builtin_ctz( n ), 31 );

  n = ~0;
  T_eq_int( __builtin_ctz( n ), 0 );
}

/**
 * @brief Check signed 64-bit divisions for a sample set of values.
 */
static void CompilerUnitBuiltins_Action_3( void )
{
  volatile int64_t n;
  volatile int64_t d;

  n = INT64_C( 0 );
  d = INT64_C( 0 );
  do_longjmp = true;

  if ( setjmp( exception_return_context ) == 0 ) {
    n = n / d;
  }

  n = INT64_C( 1 );
  d = INT64_C( 0 );
  do_longjmp = true;

  if ( setjmp( exception_return_context ) == 0 ) {
    n = n / d;
  }

  n = INT64_C( 0x7fffffffffffffff );
  d = INT64_C( 0 );
  do_longjmp = true;

  if ( setjmp( exception_return_context ) == 0 ) {
    n = n / d;
  }

  n = INT64_C( 0x7fffffff00000000 );
  d = INT64_C( 0 );
  do_longjmp = true;

  if ( setjmp( exception_return_context ) == 0 ) {
    n = n / d;
  }

  n = INT64_C( 0 );
  d = INT64_C( 1 );
  T_eq_i64( n / d, INT64_C( 0 ) );

  n = INT64_C( 1 );
  d = INT64_C( 1 );
  T_eq_i64( n / d, INT64_C( 1 ) );

  n = INT64_C( 0x7fffffffffffffff );
  d = INT64_C( 1 );
  T_eq_i64( n / d, INT64_C( 9223372036854775807 ) );

  n = INT64_C( 2 );
  d = INT64_C( 1 );
  T_eq_i64( n / d, INT64_C( 2 ) );

  n = INT64_C( 2 );
  d = INT64_C( 1 );
  T_eq_i64( n / d, INT64_C( 2 ) );

  n = INT64_C( 1 );
  d = INT64_C( 0x7fffffffffffffff );
  T_eq_i64( n / d, INT64_C( 0 ) );

  n = INT64_C( 0x7fffffffffffffff );
  d = INT64_C( 0x7fffffffffffffff );
  T_eq_i64( n / d, INT64_C( 1 ) );

  n = INT64_C( 1 );
  d = INT64_C( 0x7fffffff00000000 );
  T_eq_i64( n / d, INT64_C( 0 ) );

  n = INT64_C( 0x7fffffff00000000 );
  d = INT64_C( 0x7fffffff00000000 );
  T_eq_i64( n / d, INT64_C( 1 ) );

  n = INT64_C( 0x7fffffffffffffff );
  d = INT64_C( 0x7fffffff00000000 );
  T_eq_i64( n / d, INT64_C( 1 ) );

  n = INT64_C( 0x7fffffffffffffff );
  d = INT64_C( 0x8000000000000000 );
  T_eq_i64( n / d, INT64_C( 0 ) );

  n = INT64_C( 0x7fffffffffffffff );
  d = INT64_C( 0x0000000080000000 );
  T_eq_i64( n / d, INT64_C( 0xffffffff ) );

  n = INT64_C( 0x7fffffffffffffff );
  d = INT64_C( 0x00000000f0000000 );
  T_eq_i64( n / d, INT64_C( 2290649224 ) );

  n = INT64_C( 0x00000001ffffffff );
  d = INT64_C( 0x00000000f0000000 );
  T_eq_i64( n / d, INT64_C( 2 ) );

  n = INT64_C( 0x0000000fffffffff );
  d = INT64_C( 0x000000000000000f );
  T_eq_i64( n / d, INT64_C( 4581298449 ) );

  n = INT64_C( 0x0000000100000001 );
  d = INT64_C( 0x0000000f00000000 );
  T_eq_i64( n / d, INT64_C( 0 ) );

  n = INT64_C( 0x0000000f0000000f );
  d = INT64_C( 0x000000ff0000000f );
  T_eq_i64( n / d, INT64_C( 0 ) );
}

/**
 * @brief Check unsigned 64-bit divisions for a sample set of values.
 */
static void CompilerUnitBuiltins_Action_4( void )
{
  volatile uint64_t n;
  volatile uint64_t d;

  n = UINT64_C( 0 );
  d = UINT64_C( 0 );
  do_longjmp = true;

  if ( setjmp( exception_return_context ) == 0 ) {
    n = n / d;
  }

  n = UINT64_C( 1 );
  d = UINT64_C( 0 );
  do_longjmp = true;

  if ( setjmp( exception_return_context ) == 0 ) {
    n = n / d;
  }

  n = UINT64_C( 0x7fffffffffffffff );
  d = UINT64_C( 0 );
  do_longjmp = true;

  if ( setjmp( exception_return_context ) == 0 ) {
    n = n / d;
  }

  n = UINT64_C( 0x7fffffff00000000 );
  d = UINT64_C( 0 );
  do_longjmp = true;

  if ( setjmp( exception_return_context ) == 0 ) {
    n = n / d;
  }

  n = UINT64_C( 0 );
  d = UINT64_C( 1 );
  T_eq_u64( n / d, UINT64_C( 0 ) );

  n = UINT64_C( 1 );
  d = UINT64_C( 1 );
  T_eq_u64( n / d, UINT64_C( 1 ) );

  n = UINT64_C( 0xffffffffffffffff );
  d = UINT64_C( 1 );
  T_eq_u64( n / d, UINT64_C( 0xffffffffffffffff ) );

  n = UINT64_C( 2 );
  d = UINT64_C( 1 );
  T_eq_u64( n / d, UINT64_C( 2 ) );

  n = UINT64_C( 1 );
  d = UINT64_C( 0xffffffffffffffff );
  T_eq_u64( n / d, UINT64_C( 0 ) );

  n = UINT64_C( 0xffffffffffffffff );
  d = UINT64_C( 0xffffffffffffffff );
  T_eq_u64( n / d, UINT64_C( 1 ) );

  n = UINT64_C( 0xffffffffffffffff );
  d = UINT64_C( 0x8000000000000000 );
  T_eq_u64( n / d, UINT64_C( 1 ) );

  n = UINT64_C( 0x0000000100000001 );
  d = UINT64_C( 0x0000000f00000000 );
  T_eq_u64( n / d, UINT64_C( 0 ) );

  n = UINT64_C( 0xffffffff0000000f );
  d = UINT64_C( 0x000000010000000f );
  T_eq_u64( n / d, UINT64_C( 4294967280 ) );
}

/**
 * @brief Check signed 64-bit modulo operations for a sample set of values.
 */
static void CompilerUnitBuiltins_Action_5( void )
{
  volatile int64_t n;
  volatile int64_t d;

  n = INT64_C( 0 );
  d = INT64_C( 0 );
  do_longjmp = true;

  if ( setjmp( exception_return_context ) == 0 ) {
    n = n % d;
  }

  n = INT64_C( 1 );
  d = INT64_C( 0 );
  do_longjmp = true;

  if ( setjmp( exception_return_context ) == 0 ) {
    n = n % d;
  }

  n = INT64_C( 0x7fffffffffffffff );
  d = INT64_C( 0 );
  do_longjmp = true;

  if ( setjmp( exception_return_context ) == 0 ) {
    n = n % d;
  }

  n = INT64_C( 0x7fffffff00000000 );
  d = INT64_C( 0 );
  do_longjmp = true;

  if ( setjmp( exception_return_context ) == 0 ) {
    n = n % d;
  }

  n = INT64_C( 0 );
  d = INT64_C( 1 );
  T_eq_i64( n % d, INT64_C( 0 ) );

  n = INT64_C( 1 );
  d = INT64_C( 1 );
  T_eq_i64( n % d, INT64_C( 0 ) );

  n = INT64_C( 0x7fffffffffffffff );
  d = INT64_C( 1 );
  T_eq_i64( n % d, INT64_C( 0 ) );

  n = INT64_C( 2 );
  d = INT64_C( 1 );
  T_eq_i64( n % d, INT64_C( 0 ) );

  n = INT64_C( 2 );
  d = INT64_C( 1 );
  T_eq_i64( n % d, INT64_C( 0 ) );

  n = INT64_C( 1 );
  d = INT64_C( 0x7fffffffffffffff );
  T_eq_i64( n % d, INT64_C( 1 ) );

  n = INT64_C( 0x7fffffffffffffff );
  d = INT64_C( 0x7fffffffffffffff );
  T_eq_i64( n % d, INT64_C( 0 ) );

  n = INT64_C( 1 );
  d = INT64_C( 0x7fffffff00000000 );
  T_eq_i64( n % d, INT64_C( 1 ) );

  n = INT64_C( 0x7fffffff00000000 );
  d = INT64_C( 0x7fffffff00000000 );
  T_eq_i64( n % d, INT64_C( 0 ) );

  n = INT64_C( 0x7fffffffffffffff );
  d = INT64_C( 0x7fffffff00000000 );
  T_eq_i64( n % d, INT64_C( 0xffffffff ) );

  n = INT64_C( 0x7fffffffffffffff );
  d = INT64_C( 0x8000000000000000 );
  T_eq_i64( n % d, INT64_C( 0x7fffffffffffffff ) );

  n = INT64_C( 0x7fffffffffffffff );
  d = INT64_C( 0x0000000080000000 );
  T_eq_i64( n % d, INT64_C( 2147483647 ) );

  n = INT64_C( 0x7fffffffffffffff );
  d = INT64_C( 0x00000000f0000000 );
  T_eq_i64( n % d, INT64_C( 2147483647 ) );

  n = INT64_C( 0x00000001ffffffff );
  d = INT64_C( 0x00000000f0000000 );
  T_eq_i64( n % d, INT64_C( 536870911 ) );

  n = INT64_C( 0x0000000fffffffff );
  d = INT64_C( 0x000000000000000f );
  T_eq_i64( n % d, INT64_C( 0 ) );

  n = INT64_C( 0x0000000100000001 );
  d = INT64_C( 0x0000000f00000000 );
  T_eq_i64( n % d, INT64_C( 4294967297 ) );

  n = INT64_C( 0x0000000f0000000f );
  d = INT64_C( 0x000000ff0000000f );
  T_eq_i64( n % d, INT64_C( 64424509455 ) );

  #if __LONG_MAX__ == 0x7fffffffL
  /*
   * The above test cases may use __udivmoddi4().  However, the below
   * parameter values for __udivmoddi4() cannot be obtained through the
   * signed modulo or division operations.  On some targets, calls to
   * __udivmoddi4() may result from complex optimizations.
   */
  n = INT64_C( 0xffffffff0000000f );
  d = INT64_C( 0x000000010000000f );
  T_eq_u64( __udivmoddi4( n, d, NULL ), INT64_C( 4294967280 ) );
  #endif
}

/**
 * @brief Check unsigned 64-bit modulo operations for a sample set of values.
 */
static void CompilerUnitBuiltins_Action_6( void )
{
  volatile uint64_t n;
  volatile uint64_t d;

  n = UINT64_C( 0 );
  d = UINT64_C( 0 );
  do_longjmp = true;

  if ( setjmp( exception_return_context ) == 0 ) {
    n = n % d;
  }

  n = UINT64_C( 1 );
  d = UINT64_C( 0 );
  do_longjmp = true;

  if ( setjmp( exception_return_context ) == 0 ) {
    n = n % d;
  }

  n = UINT64_C( 0 );
  d = UINT64_C( 1 );
  T_eq_u64( n % d, UINT64_C( 0 ) );

  n = UINT64_C( 1 );
  d = UINT64_C( 1 );
  T_eq_u64( n % d, UINT64_C( 0 ) );

  n = UINT64_C( 0xffffffffffffffff );
  d = UINT64_C( 1 );
  T_eq_u64( n % d, UINT64_C( 0 ) );

  n = UINT64_C( 2 );
  d = UINT64_C( 1 );
  T_eq_u64( n % d, UINT64_C( 0 ) );

  n = UINT64_C( 1 );
  d = UINT64_C( 0xffffffffffffffff );
  T_eq_u64( n % d, UINT64_C( 1 ) );

  n = UINT64_C( 0xffffffffffffffff );
  d = UINT64_C( 0xffffffffffffffff );
  T_eq_u64( n % d, UINT64_C( 0 ) );
}

/**
 * @fn void T_case_body_CompilerUnitBuiltins( void )
 */
T_TEST_CASE_FIXTURE( CompilerUnitBuiltins, &CompilerUnitBuiltins_Fixture )
{
  CompilerUnitBuiltins_Action_0();
  CompilerUnitBuiltins_Action_1();
  CompilerUnitBuiltins_Action_2();
  CompilerUnitBuiltins_Action_3();
  CompilerUnitBuiltins_Action_4();
  CompilerUnitBuiltins_Action_5();
  CompilerUnitBuiltins_Action_6();
}

/** @} */
