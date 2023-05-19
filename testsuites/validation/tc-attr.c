/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsAttrValAttr
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
 * @defgroup RtemsAttrValAttr spec:/rtems/attr/val/attr
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @brief Tests the attribute constants of the Classic API.
 *
 * This test case performs the following actions:
 *
 * - Validate the non-default attribute constants.
 *
 *   - Check that RTEMS_BARRIER_AUTOMATIC_RELEASE is a power of two.
 *
 *   - Check that RTEMS_BINARY_SEMAPHORE is a power of two.
 *
 *   - Check that RTEMS_FLOATING_POINT is a power of two.
 *
 *   - Check that RTEMS_GLOBAL is a power of two.
 *
 *   - Check that RTEMS_INHERIT_PRIORITY is a power of two.
 *
 *   - Check that RTEMS_MULTIPROCESSOR_RESOURCE_SHARING is a power of two.
 *
 *   - Check that RTEMS_PRIORITY is a power of two.
 *
 *   - Check that RTEMS_PRIORITY_CEILING is a power of two.
 *
 *   - Check that RTEMS_SIMPLE_BINARY_SEMAPHORE is a power of two.
 *
 *   - Check that RTEMS_SYSTEM_TASK is a power of two.
 *
 * - Validate the default attribute constants.
 *
 *   - Check that RTEMS_APPLICATION_TASK is equal to zero.
 *
 *   - Check that RTEMS_BARRIER_MANUAL_RELEASE is equal to zero.
 *
 *   - Check that RTEMS_COUNTING_SEMAPHORE is equal to zero.
 *
 *   - Check that RTEMS_DEFAULT_ATTRIBUTES is equal to zero.
 *
 *   - Check that RTEMS_FIFO is equal to zero.
 *
 *   - Check that RTEMS_LOCAL is equal to zero.
 *
 *   - Check that RTEMS_NO_FLOATING_POINT is equal to zero.
 *
 *   - Check that RTEMS_NO_INHERIT_PRIORITY is equal to zero.
 *
 *   - Check that RTEMS_NO_MULTIPROCESSOR_RESOURCE_SHARING is equal to zero.
 *
 *   - Check that RTEMS_NO_PRIORITY_CEILING is equal to zero.
 *
 * - Calculate the bitwise or of all non-default attribute constants.
 *
 *   - Check that the count of set bits in the calculated value is equal to the
 *     count of non-default attribute constants.  Since each non-default
 *     attribute constant is a power of two, this proves that each constant has
 *     a unique value.
 *
 * - Calculate the bitwise or of the RTEMS_BINARY_SEMAPHORE,
 *   RTEMS_COUNTING_SEMAPHORE, and RTEMS_SIMPLE_BINARY_SEMAPHORE attribute
 *   constants.
 *
 *   - Check that the calculated value is equal to RTEMS_SEMAPHORE_CLASS.
 *
 * - Check the value of RTEMS_DEFAULT_ATTRIBUTES.
 *
 *   - Check RTEMS_DEFAULT_ATTRIBUTES equals RTEMS_FIFO | RTEMS_LOCAL.
 *
 * @{
 */

static bool IsPowerOfTwo( rtems_attribute attribute )
{
  return attribute != 0 && ( attribute & ( attribute - 1 ) ) == 0;
}

static int PopCount( rtems_attribute attributes )
{
  int count;

  count = 0;

  while ( attributes != 0 ) {
    ++count;
    attributes &= attributes - 1;
  }

  return count;
}

/**
 * @brief Validate the non-default attribute constants.
 */
static void RtemsAttrValAttr_Action_0( void )
{
  /* No action */

  /*
   * Check that RTEMS_BARRIER_AUTOMATIC_RELEASE is a power of two.
   */
  T_step_true( 0, IsPowerOfTwo( RTEMS_BARRIER_AUTOMATIC_RELEASE ) );

  /*
   * Check that RTEMS_BINARY_SEMAPHORE is a power of two.
   */
  T_step_true( 1, IsPowerOfTwo( RTEMS_BINARY_SEMAPHORE ) );

  /*
   * Check that RTEMS_FLOATING_POINT is a power of two.
   */
  T_step_true( 2, IsPowerOfTwo( RTEMS_FLOATING_POINT ) );

  /*
   * Check that RTEMS_GLOBAL is a power of two.
   */
  T_step_true( 3, IsPowerOfTwo( RTEMS_GLOBAL ) );

  /*
   * Check that RTEMS_INHERIT_PRIORITY is a power of two.
   */
  T_step_true( 4, IsPowerOfTwo( RTEMS_INHERIT_PRIORITY ) );

  /*
   * Check that RTEMS_MULTIPROCESSOR_RESOURCE_SHARING is a power of two.
   */
  T_step_true(
    5,
    IsPowerOfTwo( RTEMS_MULTIPROCESSOR_RESOURCE_SHARING )
  );

  /*
   * Check that RTEMS_PRIORITY is a power of two.
   */
  T_step_true( 6, IsPowerOfTwo( RTEMS_PRIORITY ) );

  /*
   * Check that RTEMS_PRIORITY_CEILING is a power of two.
   */
  T_step_true( 7, IsPowerOfTwo( RTEMS_PRIORITY_CEILING ) );

  /*
   * Check that RTEMS_SIMPLE_BINARY_SEMAPHORE is a power of two.
   */
  T_step_true( 8, IsPowerOfTwo( RTEMS_SIMPLE_BINARY_SEMAPHORE ) );

  /*
   * Check that RTEMS_SYSTEM_TASK is a power of two.
   */
  T_step_true( 9, IsPowerOfTwo( RTEMS_SYSTEM_TASK ) );
}

/**
 * @brief Validate the default attribute constants.
 */
static void RtemsAttrValAttr_Action_1( void )
{
  /* No action */

  /*
   * Check that RTEMS_APPLICATION_TASK is equal to zero.
   */
  T_step_eq_u32( 10, RTEMS_APPLICATION_TASK, 0 );

  /*
   * Check that RTEMS_BARRIER_MANUAL_RELEASE is equal to zero.
   */
  T_step_eq_u32( 11, RTEMS_BARRIER_MANUAL_RELEASE, 0 );

  /*
   * Check that RTEMS_COUNTING_SEMAPHORE is equal to zero.
   */
  T_step_eq_u32( 12, RTEMS_COUNTING_SEMAPHORE, 0 );

  /*
   * Check that RTEMS_DEFAULT_ATTRIBUTES is equal to zero.
   */
  T_step_eq_u32( 13, RTEMS_DEFAULT_ATTRIBUTES, 0 );

  /*
   * Check that RTEMS_FIFO is equal to zero.
   */
  T_step_eq_u32( 14, RTEMS_FIFO, 0 );

  /*
   * Check that RTEMS_LOCAL is equal to zero.
   */
  T_step_eq_u32( 15, RTEMS_LOCAL, 0 );

  /*
   * Check that RTEMS_NO_FLOATING_POINT is equal to zero.
   */
  T_step_eq_u32( 16, RTEMS_NO_FLOATING_POINT, 0 );

  /*
   * Check that RTEMS_NO_INHERIT_PRIORITY is equal to zero.
   */
  T_step_eq_u32( 17, RTEMS_NO_INHERIT_PRIORITY, 0 );

  /*
   * Check that RTEMS_NO_MULTIPROCESSOR_RESOURCE_SHARING is equal to zero.
   */
  T_step_eq_u32( 18, RTEMS_NO_MULTIPROCESSOR_RESOURCE_SHARING, 0 );

  /*
   * Check that RTEMS_NO_PRIORITY_CEILING is equal to zero.
   */
  T_step_eq_u32( 19, RTEMS_NO_PRIORITY_CEILING, 0 );
}

/**
 * @brief Calculate the bitwise or of all non-default attribute constants.
 */
static void RtemsAttrValAttr_Action_2( void )
{
  rtems_attribute attributes;

  attributes = 0;
  attributes |= RTEMS_BARRIER_AUTOMATIC_RELEASE;
  attributes |= RTEMS_BINARY_SEMAPHORE;
  attributes |= RTEMS_FLOATING_POINT;
  attributes |= RTEMS_GLOBAL;
  attributes |= RTEMS_INHERIT_PRIORITY;
  attributes |= RTEMS_MULTIPROCESSOR_RESOURCE_SHARING;
  attributes |= RTEMS_PRIORITY;
  attributes |= RTEMS_PRIORITY_CEILING;
  attributes |= RTEMS_SEMAPHORE_CLASS;
  attributes |= RTEMS_SIMPLE_BINARY_SEMAPHORE;
  attributes |= RTEMS_SYSTEM_TASK;

  /*
   * Check that the count of set bits in the calculated value is equal to the
   * count of non-default attribute constants.  Since each non-default
   * attribute constant is a power of two, this proves that each constant has a
   * unique value.
   */
  T_step_eq_int( 20, PopCount( attributes ), 10 );
}

/**
 * @brief Calculate the bitwise or of the RTEMS_BINARY_SEMAPHORE,
 *   RTEMS_COUNTING_SEMAPHORE, and RTEMS_SIMPLE_BINARY_SEMAPHORE attribute
 *   constants.
 */
static void RtemsAttrValAttr_Action_3( void )
{
  rtems_attribute attributes;

  attributes = 0;
  attributes |= RTEMS_BINARY_SEMAPHORE;
  attributes |= RTEMS_COUNTING_SEMAPHORE;
  attributes |= RTEMS_SIMPLE_BINARY_SEMAPHORE;

  /*
   * Check that the calculated value is equal to RTEMS_SEMAPHORE_CLASS.
   */
  T_step_eq_u32( 21, RTEMS_SEMAPHORE_CLASS, attributes );
}

/**
 * @brief Check the value of RTEMS_DEFAULT_ATTRIBUTES.
 */
static void RtemsAttrValAttr_Action_4( void )
{
  /* No action */

  /*
   * Check RTEMS_DEFAULT_ATTRIBUTES equals RTEMS_FIFO | RTEMS_LOCAL.
   */
  T_step_eq_int(
    22,
    RTEMS_DEFAULT_ATTRIBUTES,
    RTEMS_FIFO | RTEMS_LOCAL
  );
}

/**
 * @fn void T_case_body_RtemsAttrValAttr( void )
 */
T_TEST_CASE( RtemsAttrValAttr )
{
  T_plan( 23 );

  RtemsAttrValAttr_Action_0();
  RtemsAttrValAttr_Action_1();
  RtemsAttrValAttr_Action_2();
  RtemsAttrValAttr_Action_3();
  RtemsAttrValAttr_Action_4();
}

/** @} */
