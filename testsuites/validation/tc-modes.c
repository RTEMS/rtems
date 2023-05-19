/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsModeValModes
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
 * @defgroup RtemsModeValModes spec:/rtems/mode/val/modes
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @brief Tests the task mode constants and function-like macros of the Classic
 *   API.
 *
 * This test case performs the following actions:
 *
 * - Validate the non-default task mode constants.
 *
 *   - Check that RTEMS_NO_ASR is a power of two representable as an integer of
 *     type rtems_mode.
 *
 *   - Check that RTEMS_NO_PREEMPT is a power of two representable as an
 *     integer of type rtems_mode.
 *
 *   - Check that RTEMS_TIMESLICE is a power of two representable as an integer
 *     of type rtems_mode.
 *
 * - Validate the default task mode constants.
 *
 *   - Check that RTEMS_ASR is equal to zero.
 *
 *   - Check that RTEMS_DEFAULT_MODES is equal to zero.
 *
 *   - Check that RTEMS_NO_TIMESLICE is equal to zero.
 *
 *   - Check that RTEMS_PREEMPT is equal to zero.
 *
 * - Validate RTEMS_ALL_MODE_MASKS.
 *
 *   - Check that the bitwise and of RTEMS_ASR_MASK and RTEMS_ALL_MODE_MASKS is
 *     equal to RTEMS_ASR_MASK.
 *
 *   - Check that the bitwise and of RTEMS_PREEMPT_MASK and
 *     RTEMS_ALL_MODE_MASKS is equal to RTEMS_PREEMPT_MASK.
 *
 *   - Check that the bitwise and of RTEMS_TIMESLICE_MASK and
 *     RTEMS_ALL_MODE_MASKS is equal to RTEMS_TIMESLICE_MASK.
 *
 *   - Check that the bitwise and of RTEMS_INTERRUPT_MASK and
 *     RTEMS_ALL_MODE_MASKS is equal to RTEMS_INTERRUPT_MASK.
 *
 * - Validate the task mode mask constants except RTEMS_INTERRUPT_MASK.
 *
 *   - Check that RTEMS_ASR_MASK is a power of two representable as an integer
 *     of type rtems_mode.
 *
 *   - Check that RTEMS_PREEMPT_MASK is a power of two representable as an
 *     integer of type rtems_mode.
 *
 *   - Check that RTEMS_TIMESLICE_MASK is a power of two representable as an
 *     integer of type rtems_mode.
 *
 * - Calculate the bitwise or of all task mode mask constants and 0xff.
 *
 *   - Check that the count of set bits in the calculated value is equal to the
 *     count of task mode mask constants except RTEMS_INTERRUPT_MASK plus
 *     eight. Since each task mode mask constants except RTEMS_INTERRUPT_MASK
 *     is a power of two and the bitwise and of 0xff and RTEMS_INTERRUPT_MASK
 *     is equal to RTEMS_INTERRUPT_MASK this proves that each constant and 0xff
 *     has a unique value.
 *
 * - Calculate the bitwise or of all non-default task mode constants.
 *
 *   - Check that the count of set bits in the calculated value is equal to the
 *     count of non-default task mode constants.  Since each non-default task
 *     mode constants except is a power of this proves that each constant has a
 *     unique value.
 *
 * - Validate RTEMS_INTERRUPT_LEVEL().
 *
 *   - Check the result of RTEMS_INTERRUPT_LEVEL() for a sample value.
 *
 * @{
 */

static bool IsPowerOfTwo( rtems_mode mode )
{
  return mode != 0 && ( mode & ( mode - 1 ) ) == 0;
}

static int PopCount( rtems_mode modes )
{
  int count;

  count = 0;

  while ( modes != 0 ) {
    ++count;
    modes &= modes - 1;
  }

  return count;
}

/**
 * @brief Validate the non-default task mode constants.
 */
static void RtemsModeValModes_Action_0( void )
{
  /* No action */

  /*
   * Check that RTEMS_NO_ASR is a power of two representable as an integer of
   * type rtems_mode.
   */
  T_step_true( 0, IsPowerOfTwo( RTEMS_NO_ASR ) );

  /*
   * Check that RTEMS_NO_PREEMPT is a power of two representable as an integer
   * of type rtems_mode.
   */
  T_step_true( 1, IsPowerOfTwo( RTEMS_NO_PREEMPT ) );

  /*
   * Check that RTEMS_TIMESLICE is a power of two representable as an integer
   * of type rtems_mode.
   */
  T_step_true( 2, IsPowerOfTwo( RTEMS_TIMESLICE ) );
}

/**
 * @brief Validate the default task mode constants.
 */
static void RtemsModeValModes_Action_1( void )
{
  /* No action */

  /*
   * Check that RTEMS_ASR is equal to zero.
   */
  T_step_eq_u32( 3, RTEMS_ASR, 0 );

  /*
   * Check that RTEMS_DEFAULT_MODES is equal to zero.
   */
  T_step_eq_u32( 4, RTEMS_DEFAULT_MODES, 0 );

  /*
   * Check that RTEMS_NO_TIMESLICE is equal to zero.
   */
  T_step_eq_u32( 5, RTEMS_NO_TIMESLICE, 0 );

  /*
   * Check that RTEMS_PREEMPT is equal to zero.
   */
  T_step_eq_u32( 6, RTEMS_PREEMPT, 0 );
}

/**
 * @brief Validate RTEMS_ALL_MODE_MASKS.
 */
static void RtemsModeValModes_Action_2( void )
{
  /* No action */

  /*
   * Check that the bitwise and of RTEMS_ASR_MASK and RTEMS_ALL_MODE_MASKS is
   * equal to RTEMS_ASR_MASK.
   */
  T_step_eq_u32(
    7,
    RTEMS_ASR_MASK & RTEMS_ALL_MODE_MASKS,
    RTEMS_ASR_MASK
  );

  /*
   * Check that the bitwise and of RTEMS_PREEMPT_MASK and RTEMS_ALL_MODE_MASKS
   * is equal to RTEMS_PREEMPT_MASK.
   */
  T_step_eq_u32(
    8,
    RTEMS_PREEMPT_MASK & RTEMS_ALL_MODE_MASKS,
    RTEMS_PREEMPT_MASK
  );

  /*
   * Check that the bitwise and of RTEMS_TIMESLICE_MASK and
   * RTEMS_ALL_MODE_MASKS is equal to RTEMS_TIMESLICE_MASK.
   */
  T_step_eq_u32(
    9,
    RTEMS_TIMESLICE_MASK & RTEMS_ALL_MODE_MASKS,
    RTEMS_TIMESLICE_MASK
  );

  /*
   * Check that the bitwise and of RTEMS_INTERRUPT_MASK and
   * RTEMS_ALL_MODE_MASKS is equal to RTEMS_INTERRUPT_MASK.
   */
  T_step_eq_u32(
    10,
    RTEMS_INTERRUPT_MASK & RTEMS_ALL_MODE_MASKS,
    RTEMS_INTERRUPT_MASK
  );
}

/**
 * @brief Validate the task mode mask constants except RTEMS_INTERRUPT_MASK.
 */
static void RtemsModeValModes_Action_3( void )
{
  /* No action */

  /*
   * Check that RTEMS_ASR_MASK is a power of two representable as an integer of
   * type rtems_mode.
   */
  T_step_true( 11, IsPowerOfTwo( RTEMS_ASR_MASK ) );

  /*
   * Check that RTEMS_PREEMPT_MASK is a power of two representable as an
   * integer of type rtems_mode.
   */
  T_step_true( 12, IsPowerOfTwo( RTEMS_PREEMPT_MASK ) );

  /*
   * Check that RTEMS_TIMESLICE_MASK is a power of two representable as an
   * integer of type rtems_mode.
   */
  T_step_true( 13, IsPowerOfTwo( RTEMS_TIMESLICE_MASK ) );
}

/**
 * @brief Calculate the bitwise or of all task mode mask constants and 0xff.
 */
static void RtemsModeValModes_Action_4( void )
{
  rtems_mode modes;

  modes = 0;
  modes |= 0xff;
  modes |= RTEMS_ASR_MASK;
  modes |= RTEMS_PREEMPT_MASK;
  modes |= RTEMS_TIMESLICE_MASK;

  /*
   * Check that the count of set bits in the calculated value is equal to the
   * count of task mode mask constants except RTEMS_INTERRUPT_MASK plus eight.
   * Since each task mode mask constants except RTEMS_INTERRUPT_MASK is a power
   * of two and the bitwise and of 0xff and RTEMS_INTERRUPT_MASK is equal to
   * RTEMS_INTERRUPT_MASK this proves that each constant and 0xff has a unique
   * value.
   */
  T_step_eq_int( 14, PopCount( modes ), 11 );
}

/**
 * @brief Calculate the bitwise or of all non-default task mode constants.
 */
static void RtemsModeValModes_Action_5( void )
{
  rtems_mode modes;

  modes = 0;
  modes |= RTEMS_NO_ASR;
  modes |= RTEMS_NO_PREEMPT;
  modes |= RTEMS_TIMESLICE;

  /*
   * Check that the count of set bits in the calculated value is equal to the
   * count of non-default task mode constants.  Since each non-default task
   * mode constants except is a power of this proves that each constant has a
   * unique value.
   */
  T_step_eq_int( 15, PopCount( modes ), 3 );
}

/**
 * @brief Validate RTEMS_INTERRUPT_LEVEL().
 */
static void RtemsModeValModes_Action_6( void )
{
  /* Nothing to do */

  /*
   * Check the result of RTEMS_INTERRUPT_LEVEL() for a sample value.
   */
  T_step_eq_u32(
    16,
    RTEMS_INTERRUPT_LEVEL( UINT32_MAX ),
    RTEMS_INTERRUPT_MASK
  );
}

/**
 * @fn void T_case_body_RtemsModeValModes( void )
 */
T_TEST_CASE( RtemsModeValModes )
{
  T_plan( 17 );

  RtemsModeValModes_Action_0();
  RtemsModeValModes_Action_1();
  RtemsModeValModes_Action_2();
  RtemsModeValModes_Action_3();
  RtemsModeValModes_Action_4();
  RtemsModeValModes_Action_5();
  RtemsModeValModes_Action_6();
}

/** @} */
