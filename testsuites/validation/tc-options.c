/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsOptionValOptions
 */

/*
 * Copyright (C) 2020 embedded brains GmbH & Co. KG
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
 * @defgroup RtemsOptionValOptions spec:/rtems/option/val/options
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @brief Tests the option constants of the Classic API.
 *
 * This test case performs the following actions:
 *
 * - Validate the non-default option constants.
 *
 *   - Check that RTEMS_EVENT_ANY is a power of two.
 *
 *   - Check that RTEMS_NO_WAIT is a power of two.
 *
 * - Validate the default option constants.
 *
 *   - Check that RTEMS_DEFAULT_OPTIONS is equal to zero.
 *
 *   - Check that RTEMS_EVENT_ALL is equal to zero.
 *
 *   - Check that RTEMS_WAIT is equal to zero.
 *
 * - Calculate the bitwise or of all non-default option constants.
 *
 *   - Check that the count of set bits in the calculated value is equal to the
 *     count of non-default option constants.  Since each non-default option
 *     constant is a power of two, this proves that each constant has a unique
 *     value.
 *
 * - Check the value of RTEMS_DEFAULT_OPTIONS.
 *
 *   - Check RTEMS_DEFAULT_OPTIONS equals RTEMS_WAIT.
 *
 * @{
 */

static bool IsPowerOfTwo( rtems_option option )
{
  return option != 0 && ( option & ( option - 1 ) ) == 0;
}

static int PopCount( rtems_option options )
{
  int count;

  count = 0;

  while ( options != 0 ) {
    ++count;
    options &= options - 1;
  }

  return count;
}

/**
 * @brief Validate the non-default option constants.
 */
static void RtemsOptionValOptions_Action_0( void )
{
  /* No action */

  /*
   * Check that RTEMS_EVENT_ANY is a power of two.
   */
  T_step_true( 0, IsPowerOfTwo( RTEMS_EVENT_ANY ) );

  /*
   * Check that RTEMS_NO_WAIT is a power of two.
   */
  T_step_true( 1, IsPowerOfTwo( RTEMS_NO_WAIT ) );
}

/**
 * @brief Validate the default option constants.
 */
static void RtemsOptionValOptions_Action_1( void )
{
  /* No action */

  /*
   * Check that RTEMS_DEFAULT_OPTIONS is equal to zero.
   */
  T_step_eq_u32( 2, RTEMS_DEFAULT_OPTIONS, 0 );

  /*
   * Check that RTEMS_EVENT_ALL is equal to zero.
   */
  T_step_eq_u32( 3, RTEMS_EVENT_ALL, 0 );

  /*
   * Check that RTEMS_WAIT is equal to zero.
   */
  T_step_eq_u32( 4, RTEMS_WAIT, 0 );
}

/**
 * @brief Calculate the bitwise or of all non-default option constants.
 */
static void RtemsOptionValOptions_Action_2( void )
{
  rtems_option options;

  options = 0;
  options |= RTEMS_EVENT_ANY;
  options |= RTEMS_NO_WAIT;

  /*
   * Check that the count of set bits in the calculated value is equal to the
   * count of non-default option constants.  Since each non-default option
   * constant is a power of two, this proves that each constant has a unique
   * value.
   */
  T_step_eq_int( 5, PopCount( options ), 2 );
}

/**
 * @brief Check the value of RTEMS_DEFAULT_OPTIONS.
 */
static void RtemsOptionValOptions_Action_3( void )
{
  /* No action */

  /*
   * Check RTEMS_DEFAULT_OPTIONS equals RTEMS_WAIT.
   */
  T_step_eq_int( 6, RTEMS_DEFAULT_OPTIONS, RTEMS_WAIT );
}

/**
 * @fn void T_case_body_RtemsOptionValOptions( void )
 */
T_TEST_CASE( RtemsOptionValOptions )
{
  T_plan( 7 );

  RtemsOptionValOptions_Action_0();
  RtemsOptionValOptions_Action_1();
  RtemsOptionValOptions_Action_2();
  RtemsOptionValOptions_Action_3();
}

/** @} */
