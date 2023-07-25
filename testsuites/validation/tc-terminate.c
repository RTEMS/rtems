/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreInterrValTerminate
 */

/*
 * Copyright (C) 2021 embedded brains GmbH & Co. KG
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

#include <bsp.h>
#include <setjmp.h>
#include <string.h>
#include <rtems/bspIo.h>
#include <rtems/test-info.h>
#include <rtems/score/atomic.h>
#include <rtems/score/percpu.h>
#include <rtems/score/sysstate.h>

#include "tc-userext.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreInterrValTerminate spec:/score/interr/val/terminate
 *
 * @ingroup TestsuitesTerminate
 *
 * @brief Tests system termination procedure.
 *
 * This test case performs the following actions:
 *
 * - Create five dynamic extensions.  Call the system termination procedure.
 *   Delete three dynamic extension during the fatal extension invocation.
 *   Delete the two remaining dynamic extensions.
 *
 *   - Where the system was built with SMP support enabled, check that a
 *     shutdown request was issued.
 *
 *   - Delete the dynamic extension sets.
 *
 *   - Check that the fatal extensions were invoked with the expected source.
 *
 *   - Check that the fatal extensions were invoked with the expected always
 *     set to false argument.
 *
 *   - Check that the fatal extensions were invoked with the expected code.
 *
 *   - Check that the fatal extensions were invoked in forward order.
 *
 *   - Check that the fatal extension in the deleted extension set was not
 *     invoked.
 *
 *   - Check that the system state is terminated.
 *
 *   - Check that the system was halted with the expected fatal source.
 *
 *   - Check that the system was halted with the expected fatal code.
 *
 *   - Check that the system was finally halted.
 *
 * @{
 */

typedef struct {
  unsigned int       counter;
  rtems_fatal_source source;
  bool               always_set_to_false;
  rtems_fatal_code   code;
} FatalInfo;

static Atomic_Uint counter;

static FatalInfo info[ 7 ];

static bool test_case_active;

static const rtems_extensions_table bsp = BSP_INITIAL_EXTENSION;

static jmp_buf before_terminate;

static unsigned int halt_counter;

static rtems_fatal_source halt_source;

static rtems_fatal_code halt_code;

static rtems_id extension_ids[ 7 ];

static unsigned int GetCounter( void )
{
  return _Atomic_Fetch_add_uint( &counter, 1, ATOMIC_ORDER_RELAXED ) + 1;
}

void __real__CPU_Fatal_halt( uint32_t source, CPU_Uint32ptr code );

void __wrap__CPU_Fatal_halt( uint32_t source, CPU_Uint32ptr code );

void __wrap__CPU_Fatal_halt( uint32_t source, CPU_Uint32ptr code )
{
  if ( test_case_active ) {
    halt_counter = GetCounter();
    halt_source = source;
    halt_code = code;
    longjmp( before_terminate, 1 );
  } else {
#if defined(RTEMS_GCOV_COVERAGE)
    rtems_test_gcov_dump_info();
#endif
    __real__CPU_Fatal_halt( source, code );
  }
}

static void FatalExtension(
  rtems_fatal_source source,
  bool               always_set_to_false,
  rtems_fatal_code   code,
  size_t             index
)
{
  if ( test_case_active ) {
    info[ index ].counter = GetCounter();
    info[ index ].source = source;
    info[ index ].always_set_to_false = always_set_to_false;
    info[ index ].code = code;
  } else {
    ( *bsp.fatal )( source, always_set_to_false, code );
  }
}

void FatalExtension0(
  rtems_fatal_source source,
  bool               always_set_to_false,
  rtems_fatal_code   code
)
{
  FatalExtension( source, always_set_to_false, code, 0 );
}

void FatalExtension1(
  rtems_fatal_source source,
  bool               always_set_to_false,
  rtems_fatal_code   code
)
{
  FatalExtension( source, always_set_to_false, code, 1 );
}

static void FatalExtension2(
  rtems_fatal_source source,
  bool               always_set_to_false,
  rtems_fatal_code   code
)
{
  rtems_status_code sc;

  FatalExtension( source, always_set_to_false, code, 2 );

  sc = rtems_extension_delete( extension_ids[ 3 ] );
  T_quiet_rsc_success( sc );
}

static void FatalExtension3(
  rtems_fatal_source source,
  bool               always_set_to_false,
  rtems_fatal_code   code
)
{
  FatalExtension( source, always_set_to_false, code, 3 );
}

static void FatalExtension4(
  rtems_fatal_source source,
  bool               always_set_to_false,
  rtems_fatal_code   code
)
{
  FatalExtension( source, always_set_to_false, code, 4 );
}

static void FatalExtension5(
  rtems_fatal_source source,
  bool               always_set_to_false,
  rtems_fatal_code   code
)
{
  rtems_status_code sc;

  FatalExtension( source, always_set_to_false, code, 5 );

  sc = rtems_extension_delete( extension_ids[ 5 ] );
  T_quiet_rsc_success( sc );
}

static void FatalExtension6(
  rtems_fatal_source source,
  bool               always_set_to_false,
  rtems_fatal_code   code
)
{
  rtems_status_code sc;

  FatalExtension( source, always_set_to_false, code, 6 );

  sc = rtems_extension_delete( extension_ids[ 4 ] );
  T_quiet_rsc_success( sc );
}

/**
 * @brief Create five dynamic extensions.  Call the system termination
 *   procedure. Delete three dynamic extension during the fatal extension
 *   invocation. Delete the two remaining dynamic extensions.
 */
static void ScoreInterrValTerminate_Action_0( void )
{
  rtems_status_code      sc;
  rtems_extensions_table table;
  bool                   shutdown_ok;

  #if defined(RTEMS_SMP)
  shutdown_ok =
    ( _Per_CPU_Get_state( _Per_CPU_Get_snapshot() ) == PER_CPU_STATE_UP );
  #else
  shutdown_ok = true;
  #endif

  memset( &table, 0, sizeof( table ) );

  table.fatal = FatalExtension2;
  sc = rtems_extension_create(
    rtems_build_name( ' ', ' ', ' ', '2' ),
    &table,
    &extension_ids[ 2 ]
  );
  T_step_rsc_success( 0, sc );

  table.fatal = FatalExtension3;
  sc = rtems_extension_create(
    rtems_build_name( ' ', ' ', ' ', '3' ),
    &table,
    &extension_ids[ 3 ]
  );
  T_step_rsc_success( 1, sc );

  table.fatal = FatalExtension4;
  sc = rtems_extension_create(
    rtems_build_name( ' ', ' ', ' ', '4' ),
    &table,
    &extension_ids[ 4 ]
  );
  T_step_rsc_success( 2, sc );

  table.fatal = FatalExtension5;
  sc = rtems_extension_create(
    rtems_build_name( ' ', ' ', ' ', '5' ),
    &table,
    &extension_ids[ 5 ]
  );
  T_step_rsc_success( 3, sc );

  table.fatal = FatalExtension6;
  sc = rtems_extension_create(
    rtems_build_name( ' ', ' ', ' ', '6' ),
    &table,
    &extension_ids[ 6 ]
  );
  T_step_rsc_success( 4, sc );

  test_case_active = true;

  if ( setjmp( before_terminate ) == 0 ) {
    _Terminate( RTEMS_FATAL_SOURCE_APPLICATION, 123456 );
  }

  test_case_active = false;

  /*
   * Where the system was built with SMP support enabled, check that a shutdown
   * request was issued.
   */
  #if defined(RTEMS_SMP)
  shutdown_ok = ( shutdown_ok && _ISR_Get_level() != 0 &&
    _Per_CPU_Get_state( _Per_CPU_Get() ) == PER_CPU_STATE_SHUTDOWN );
  _ISR_Set_level( 0 );
  #endif
  T_step_true( 5, shutdown_ok );

  /*
   * Delete the dynamic extension sets.
   */
  sc = rtems_extension_delete( extension_ids[ 2 ] );
  T_step_rsc_success( 6, sc );

  sc = rtems_extension_delete( extension_ids[ 6 ] );
  T_step_rsc_success( 7, sc );

  /*
   * Check that the fatal extensions were invoked with the expected source.
   */
  T_step_eq_int(
    8,
    info[ 0 ].source,
    RTEMS_FATAL_SOURCE_APPLICATION
  );
  T_step_eq_int(
    9,
    info[ 1 ].source,
    RTEMS_FATAL_SOURCE_APPLICATION
  );
  T_step_eq_int(
    10,
    info[ 2 ].source,
    RTEMS_FATAL_SOURCE_APPLICATION
  );
  T_step_eq_int(
    11,
    info[ 4 ].source,
    RTEMS_FATAL_SOURCE_APPLICATION
  );
  T_step_eq_int(
    12,
    info[ 5 ].source,
    RTEMS_FATAL_SOURCE_APPLICATION
  );
  T_step_eq_int(
    13,
    info[ 6 ].source,
    RTEMS_FATAL_SOURCE_APPLICATION
  );

  /*
   * Check that the fatal extensions were invoked with the expected always set
   * to false argument.
   */
  T_step_false( 14, info[ 0 ].always_set_to_false );
  T_step_false( 15, info[ 1 ].always_set_to_false );
  T_step_false( 16, info[ 2 ].always_set_to_false );
  T_step_false( 17, info[ 4 ].always_set_to_false );
  T_step_false( 18, info[ 5 ].always_set_to_false );
  T_step_false( 19, info[ 6 ].always_set_to_false );

  /*
   * Check that the fatal extensions were invoked with the expected code.
   */
  T_step_eq_ulong( 20, info[ 0 ].code, 123456 );
  T_step_eq_ulong( 21, info[ 1 ].code, 123456 );
  T_step_eq_ulong( 22, info[ 2 ].code, 123456 );
  T_step_eq_ulong( 23, info[ 4 ].code, 123456 );
  T_step_eq_ulong( 24, info[ 5 ].code, 123456 );
  T_step_eq_ulong( 25, info[ 6 ].code, 123456 );

  /*
   * Check that the fatal extensions were invoked in forward order.
   */
  T_step_eq_uint( 26, info[ 0 ].counter, 1 );
  T_step_eq_uint( 27, info[ 1 ].counter, 2 );
  T_step_eq_uint( 28, info[ 2 ].counter, 3 );
  T_step_eq_uint( 29, info[ 4 ].counter, 4 );
  T_step_eq_uint( 30, info[ 5 ].counter, 5 );
  T_step_eq_uint( 31, info[ 6 ].counter, 6 );

  /*
   * Check that the fatal extension in the deleted extension set was not
   * invoked.
   */
  T_step_eq_int( 32, info[ 3 ].source, 0 );
  T_step_false( 33, info[ 3 ].always_set_to_false );
  T_step_eq_ulong( 34, info[ 3 ].code, 0 );
  T_step_eq_uint( 35, info[ 3 ].counter, 0 );

  /*
   * Check that the system state is terminated.
   */
  T_step_eq_int( 36, _System_state_Get(), SYSTEM_STATE_TERMINATED );

  /*
   * Check that the system was halted with the expected fatal source.
   */
  T_step_eq_int( 37, halt_source, RTEMS_FATAL_SOURCE_APPLICATION );

  /*
   * Check that the system was halted with the expected fatal code.
   */
  T_step_eq_ulong( 38, halt_code, 123456 );

  /*
   * Check that the system was finally halted.
   */
  T_step_eq_uint( 39, counter, 7 );
}

/**
 * @fn void T_case_body_ScoreInterrValTerminate( void )
 */
T_TEST_CASE( ScoreInterrValTerminate )
{
  T_plan( 40 );

  ScoreInterrValTerminate_Action_0();
}

/** @} */
