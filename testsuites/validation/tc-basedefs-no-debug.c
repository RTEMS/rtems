/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsBasedefsValBasedefsNoDebug
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

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsBasedefsValBasedefsNoDebug \
 *   spec:/rtems/basedefs/val/basedefs-no-debug
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @brief Tests the basedefs macros where RTEMS_DEBUG is disabled.
 *
 * This test case performs the following actions:
 *
 * - Expand and stringify RTEMS_UNREACHABLE().
 *
 *   - Check that the string is equal to the expected statement.
 *
 * - Expand RTEMS_FUNCTION_NAME.
 *
 *   - Check that the string is equal to the expected function name.
 *
 * @{
 */

/**
 * @brief Expand and stringify RTEMS_UNREACHABLE().
 */
static void RtemsBasedefsValBasedefsNoDebug_Action_0( void )
{
  const char *s;

  s = RTEMS_XSTRING( RTEMS_UNREACHABLE() );

  /*
   * Check that the string is equal to the expected statement.
   */
  T_step_true(
    0,
    IsEqualIgnoreWhiteSpace(
      s,
      "__builtin_unreachable()"
    )
  );
}

/**
 * @brief Expand RTEMS_FUNCTION_NAME.
 */
static void RtemsBasedefsValBasedefsNoDebug_Action_1( void )
{
  const char *s;

  s = RTEMS_FUNCTION_NAME;

  /*
   * Check that the string is equal to the expected function name.
   */
  T_step_true(
    1,
    IsEqualIgnoreWhiteSpace(
      s,
      "RtemsBasedefsValBasedefsNoDebug_Action_1"
    )
  );
}

/**
 * @fn void T_case_body_RtemsBasedefsValBasedefsNoDebug( void )
 */
T_TEST_CASE( RtemsBasedefsValBasedefsNoDebug )
{
  T_plan( 2 );

  RtemsBasedefsValBasedefsNoDebug_Action_0();
  RtemsBasedefsValBasedefsNoDebug_Action_1();
}

/** @} */
