/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreThreadValPreinitArray
 */

/*
 * Copyright (C) 2022 embedded brains GmbH & Co. KG
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

#include <rtems.h>

#include <rtems/test.h>

/**
 * @defgroup ScoreThreadValPreinitArray spec:/score/thread/val/preinit-array
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @brief Tests the ``.preinit_array`` constructor handling of the C runtime
 *   support.
 *
 * This test case performs the following actions:
 *
 * - Validate the ``.preinit_array`` constructor invocation.  Mark that the
 *   test case executed.
 *
 *   - Check that the ``.preinit_array`` constructor was called exactly once.
 *
 *   - Check that the ``.preeinit_array`` construction was done by the Classic
 *     API user initialization task.
 *
 *   - Check that the ``.preinit_array`` constructor was called before the
 *     global constructor invocations.
 *
 * @{
 */

static bool test_case_executed;

static bool constructor_test_case_executed;

static uint32_t counter;

static uint32_t normal_constructor_counter;

static uint32_t constructor_counter;

static uint32_t constructor_calls;

static rtems_id constructor_id;

static __attribute__(( __constructor__ )) void NormalConstructor( void )
{
  ++counter;
  normal_constructor_counter = counter;
}

static void Constructor( void )
{
  constructor_test_case_executed = test_case_executed;
  ++counter;
  constructor_counter = counter;
  ++constructor_calls;
  constructor_id = rtems_task_self();
}

static RTEMS_USED RTEMS_SECTION( ".preinit_array" ) void
  ( * const constructor_registration )( void ) = Constructor;

/**
 * @brief Validate the ``.preinit_array`` constructor invocation.  Mark that
 *   the test case executed.
 */
static void ScoreThreadValPreinitArray_Action_0( void )
{
  test_case_executed = true;

  /*
   * Check that the ``.preinit_array`` constructor was called exactly once.
   */
  T_eq_u32( constructor_calls, 1 );

  /*
   * Check that the ``.preeinit_array`` construction was done by the Classic
   * API user initialization task.
   */
  T_eq_u32( constructor_id, rtems_task_self() );

  /*
   * Check that the ``.preinit_array`` constructor was called before the global
   * constructor invocations.
   */
  T_eq_u32( constructor_counter, 1 );
  T_eq_u32( normal_constructor_counter, 2 );
}

/**
 * @fn void T_case_body_ScoreThreadValPreinitArray( void )
 */
T_TEST_CASE( ScoreThreadValPreinitArray )
{
  ScoreThreadValPreinitArray_Action_0();
}

/** @} */
