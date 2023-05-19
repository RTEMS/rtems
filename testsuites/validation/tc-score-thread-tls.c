/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreThreadValTls
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

#include <rtems.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreThreadValTls spec:/score/thread/val/tls
 *
 * @ingroup TestsuitesValidationTls0
 * @ingroup TestsuitesValidationTls1
 *
 * @brief Tests properties of thread-local objects.
 *
 * This test case performs the following actions:
 *
 * - The test action is carried out by the thread-local objects definition.
 *
 *   - Check that the initial value of the zero initialized thread-local object
 *     has the expected value.
 *
 *   - Check that the alignment of the zero initialized thread-local object has
 *     the expected value.
 *
 *   - Check that the initial value of the non-zero initialized thread-local
 *     object has the expected value.
 *
 *   - Check that the alignment of the non-zero initialized thread-local object
 *     has the expected value.
 *
 * @{
 */

static RTEMS_ALIGNED( 256 ) _Thread_local int volatile tls_object_0;

static RTEMS_ALIGNED( 256 ) _Thread_local int volatile tls_object_1 = 123;

/**
 * @brief The test action is carried out by the thread-local objects
 *   definition.
 */
static void ScoreThreadValTls_Action_0( void )
{
  /* Nothing to do */

  /*
   * Check that the initial value of the zero initialized thread-local object
   * has the expected value.
   */
  T_step_eq_int( 0, tls_object_0, 0 );

  /*
   * Check that the alignment of the zero initialized thread-local object has
   * the expected value.
   */
  T_step_eq_uptr( 1, ( (uintptr_t) &tls_object_0 ) % 256, 0 );

  /*
   * Check that the initial value of the non-zero initialized thread-local
   * object has the expected value.
   */
  T_step_eq_int( 2, tls_object_1, 123 );

  /*
   * Check that the alignment of the non-zero initialized thread-local object
   * has the expected value.
   */
  T_step_eq_uptr( 3, ( (uintptr_t) &tls_object_1 ) % 256, 0 );
}

/**
 * @fn void T_case_body_ScoreThreadValTls( void )
 */
T_TEST_CASE( ScoreThreadValTls )
{
  T_plan( 4 );

  ScoreThreadValTls_Action_0();
}

/** @} */
