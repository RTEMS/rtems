/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreThreadValTlsMaxZero
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

#include <rtems/score/thread.h>

#include <rtems/test.h>

/**
 * @defgroup ScoreThreadValTlsMaxZero spec:/score/thread/val/tls-max-zero
 *
 * @ingroup TestsuitesValidationTls1
 *
 * @brief Tests properties of thread-local objects.
 *
 * This test case performs the following actions:
 *
 * - The test action is carried out by the thread-local objects definition and
 *   the application configuration.
 *
 *   - Check that the CONFIGURE_MAXIMUM_THREAD_LOCAL_STORAGE_SIZE application
 *     configuration option resulted in the expected system setting. Since at
 *     least one thread-local objects is available, the referenced requirement
 *     is validated.
 *
 * @{
 */

static _Thread_local int volatile tls_object;

/**
 * @brief The test action is carried out by the thread-local objects definition
 *   and the application configuration.
 */
static void ScoreThreadValTlsMaxZero_Action_0( void )
{
  /* Nothing to do */

  /*
   * Check that the CONFIGURE_MAXIMUM_THREAD_LOCAL_STORAGE_SIZE application
   * configuration option resulted in the expected system setting. Since at
   * least one thread-local objects is available, the referenced requirement is
   * validated.
   */
  T_step_eq_int( 0, tls_object, 0 );
  T_step_eq_sz( 1, _Thread_Maximum_TLS_size, 0 );
}

/**
 * @fn void T_case_body_ScoreThreadValTlsMaxZero( void )
 */
T_TEST_CASE( ScoreThreadValTlsMaxZero )
{
  T_plan( 2 );

  ScoreThreadValTlsMaxZero_Action_0();
}

/** @} */
