/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsTypeValType
 */

/*
 * Copyright (C) 2021, 2023 embedded brains GmbH & Co. KG
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
#include <rtems/score/objectimpl.h>

#include <rtems/test.h>

/**
 * @defgroup RtemsTypeValType spec:/rtems/type/val/type
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @brief Tests some @ref RTEMSAPIClassicTypes interfaces.
 *
 * This test case performs the following actions:
 *
 * - Validate the RTEMS_ID_NONE constant.
 *
 *   - Assert that RTEMS_ID_NONE cannot be associated with an object because it
 *     has an object index outside the range of valid object indices.
 *
 * - Validate the RTEMS_NO_TIMEOUT constant.
 *
 *   - Assert that RTEMS_NO_TIMEOUT is a compile time constant which evaluates
 *     to a value of zero.
 *
 *   - Check that RTEMS_NO_TIMEOUT evaluates to a value of zero.
 *
 * @{
 */

/**
 * @brief Validate the RTEMS_ID_NONE constant.
 */
static void RtemsTypeValType_Action_0( void )
{
  /* Nothing to do */

  /*
   * Assert that RTEMS_ID_NONE cannot be associated with an object because it
   * has an object index outside the range of valid object indices.
   */
  RTEMS_STATIC_ASSERT(
    ( ( RTEMS_ID_NONE >> OBJECTS_INDEX_START_BIT ) &
      OBJECTS_INDEX_VALID_BITS ) < OBJECTS_INDEX_MINIMUM,
    ID_NONE
  );
}

/**
 * @brief Validate the RTEMS_NO_TIMEOUT constant.
 */
static void RtemsTypeValType_Action_1( void )
{
  /* Nothing to do */

  /*
   * Assert that RTEMS_NO_TIMEOUT is a compile time constant which evaluates to
   * a value of zero.
   */
  RTEMS_STATIC_ASSERT( RTEMS_NO_TIMEOUT == 0, NO_TIMEOUT );

  /*
   * Check that RTEMS_NO_TIMEOUT evaluates to a value of zero.
   */
  T_eq_u32( RTEMS_NO_TIMEOUT, 0 );
}

/**
 * @fn void T_case_body_RtemsTypeValType( void )
 */
T_TEST_CASE( RtemsTypeValType )
{
  RtemsTypeValType_Action_0();
  RtemsTypeValType_Action_1();
}

/** @} */
