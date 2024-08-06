/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreInterrValTerminate
 */

/*
 * Copyright (C) 2021, 2024 embedded brains GmbH & Co. KG
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
#include <rtems/score/isrlevel.h>

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
 *   - Check that maskable interrupts were enabled for the user extensions.
 *     Check that the idle loop executes with maskable interrupts disabled.
 *
 *   - Check that an idle loop executed after invocation of the user
 *     extensions.
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

static unsigned int idle_counter;

static uint32_t extension_isr_level;

static uint32_t idle_isr_level;

static rtems_id extension_ids[ 7 ];

static unsigned int GetCounter( void )
{
  return _Atomic_Fetch_add_uint( &counter, 1, ATOMIC_ORDER_RELAXED ) + 1;
}

void *__real__CPU_Thread_Idle_body( void *arg );

void *__wrap__CPU_Thread_Idle_body( void *arg );

void *__wrap__CPU_Thread_Idle_body( void *arg )
{
  if ( test_case_active ) {
    idle_counter = GetCounter();
    idle_isr_level = _ISR_Get_level();
    _ISR_Set_level( 0 );
    longjmp( before_terminate, 1 );
  } else {
#if defined(RTEMS_GCOV_COVERAGE)
    rtems_test_gcov_dump_info();
#endif
    return __real__CPU_Thread_Idle_body( arg );
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
  extension_isr_level = _ISR_Get_level();
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
  rtems_status_code     sc;
  rtems_interrupt_level level;

  FatalExtension( source, always_set_to_false, code, 2 );

  _ISR_Set_level( 0 );
  sc = rtems_extension_delete( extension_ids[ 3 ] );
  T_quiet_rsc_success( sc );
  rtems_interrupt_local_disable( level );
  (void) level;
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
  rtems_status_code     sc;
  rtems_interrupt_level level;

  FatalExtension( source, always_set_to_false, code, 5 );

  _ISR_Set_level( 0 );
  sc = rtems_extension_delete( extension_ids[ 5 ] );
  T_quiet_rsc_success( sc );
  rtems_interrupt_local_disable( level );
  (void) level;
}

static void FatalExtension6(
  rtems_fatal_source source,
  bool               always_set_to_false,
  rtems_fatal_code   code
)
{
  rtems_status_code     sc;
  rtems_interrupt_level level;

  FatalExtension( source, always_set_to_false, code, 6 );

  _ISR_Set_level( 0 );
  sc = rtems_extension_delete( extension_ids[ 4 ] );
  T_quiet_rsc_success( sc );
  rtems_interrupt_local_disable( level );
  (void) level;
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
  uint32_t               test_case_isr_level;

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

  test_case_isr_level = _ISR_Get_level();
  test_case_active = true;

  if ( setjmp( before_terminate ) == 0 ) {
    _Terminate( RTEMS_FATAL_SOURCE_APPLICATION, 123456 );
  }

  test_case_active = false;

  /*
   * Delete the dynamic extension sets.
   */
  sc = rtems_extension_delete( extension_ids[ 2 ] );
  T_step_rsc_success( 5, sc );

  sc = rtems_extension_delete( extension_ids[ 6 ] );
  T_step_rsc_success( 6, sc );

  /*
   * Check that the fatal extensions were invoked with the expected source.
   */
  T_step_eq_int(
    7,
    info[ 0 ].source,
    RTEMS_FATAL_SOURCE_APPLICATION
  );
  T_step_eq_int(
    8,
    info[ 1 ].source,
    RTEMS_FATAL_SOURCE_APPLICATION
  );
  T_step_eq_int(
    9,
    info[ 2 ].source,
    RTEMS_FATAL_SOURCE_APPLICATION
  );
  T_step_eq_int(
    10,
    info[ 4 ].source,
    RTEMS_FATAL_SOURCE_APPLICATION
  );
  T_step_eq_int(
    11,
    info[ 5 ].source,
    RTEMS_FATAL_SOURCE_APPLICATION
  );
  T_step_eq_int(
    12,
    info[ 6 ].source,
    RTEMS_FATAL_SOURCE_APPLICATION
  );

  /*
   * Check that the fatal extensions were invoked with the expected always set
   * to false argument.
   */
  T_step_false( 13, info[ 0 ].always_set_to_false );
  T_step_false( 14, info[ 1 ].always_set_to_false );
  T_step_false( 15, info[ 2 ].always_set_to_false );
  T_step_false( 16, info[ 4 ].always_set_to_false );
  T_step_false( 17, info[ 5 ].always_set_to_false );
  T_step_false( 18, info[ 6 ].always_set_to_false );

  /*
   * Check that the fatal extensions were invoked with the expected code.
   */
  T_step_eq_ulong( 19, info[ 0 ].code, 123456 );
  T_step_eq_ulong( 20, info[ 1 ].code, 123456 );
  T_step_eq_ulong( 21, info[ 2 ].code, 123456 );
  T_step_eq_ulong( 22, info[ 4 ].code, 123456 );
  T_step_eq_ulong( 23, info[ 5 ].code, 123456 );
  T_step_eq_ulong( 24, info[ 6 ].code, 123456 );

  /*
   * Check that the fatal extensions were invoked in forward order.
   */
  T_step_eq_uint( 25, info[ 0 ].counter, 1 );
  T_step_eq_uint( 26, info[ 1 ].counter, 2 );
  T_step_eq_uint( 27, info[ 2 ].counter, 3 );
  T_step_eq_uint( 28, info[ 4 ].counter, 4 );
  T_step_eq_uint( 29, info[ 5 ].counter, 5 );
  T_step_eq_uint( 30, info[ 6 ].counter, 6 );

  /*
   * Check that the fatal extension in the deleted extension set was not
   * invoked.
   */
  T_step_eq_int( 31, info[ 3 ].source, 0 );
  T_step_false( 32, info[ 3 ].always_set_to_false );
  T_step_eq_ulong( 33, info[ 3 ].code, 0 );
  T_step_eq_uint( 34, info[ 3 ].counter, 0 );

  /*
   * Check that maskable interrupts were enabled for the user extensions. Check
   * that the idle loop executes with maskable interrupts disabled.
   */
  T_step_eq_u32( 35, test_case_isr_level, 0 );
  T_step_ne_u32( 36, extension_isr_level, 0 );
  T_step_ne_u32( 37, idle_isr_level, 0 );

  /*
   * Check that an idle loop executed after invocation of the user extensions.
   */
  T_step_eq_uint( 38, idle_counter, 7 );
  T_step_eq_uint(
    39,
    _Atomic_Load_uint( &counter, ATOMIC_ORDER_RELAXED ),
    7
  );
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
