/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsClockValClock
 */

/*
 * Copyright (C) 2021, 2022 embedded brains GmbH & Co. KG
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

#include "ts-config.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsClockValClock spec:/rtems/clock/val/clock
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @brief Tests some @ref RTEMSAPIClassicClock directives.
 *
 * This test case performs the following actions:
 *
 * - Use the rtems_clock_get_ticks_since_boot() directive before and after
 *   exactly one clock tick.
 *
 *   - Check that clock tick gets incremented.
 *
 * - Use the rtems_clock_get_ticks_since_boot() directive before and after
 *   exactly one clock tick.
 *
 *   - Check that clock tick gets incremented.
 *
 * - Use the rtems_clock_get_ticks_per_second() directive.
 *
 *   - Check that rtems_clock_get_ticks_per_second() actually returns 1us /
 *     CONFIGURE_MICROSECONDS_PER_TICK.
 *
 * - Use the rtems_clock_get_ticks_per_second() directive.
 *
 *   - Check that rtems_clock_get_ticks_per_second() actually returns 1us /
 *     CONFIGURE_MICROSECONDS_PER_TICK.
 *
 * @{
 */

/**
 * @brief Use the rtems_clock_get_ticks_since_boot() directive before and after
 *   exactly one clock tick.
 */
static void RtemsClockValClock_Action_0( void )
{
  rtems_interval result_0;
  rtems_interval result_1;

  result_0 = rtems_clock_get_ticks_since_boot();
  ClockTick();
  result_1 = rtems_clock_get_ticks_since_boot();

  /*
   * Check that clock tick gets incremented.
   */
  T_step_eq_u32( 0, result_1 - result_0, 1 );
}

/**
 * @brief Use the rtems_clock_get_ticks_since_boot() directive before and after
 *   exactly one clock tick.
 */
static void RtemsClockValClock_Action_1( void )
{
  rtems_interval result_0;
  rtems_interval result_1;

  #undef rtems_clock_get_ticks_since_boot

  result_0 = rtems_clock_get_ticks_since_boot();
  ClockTick();
  result_1 = rtems_clock_get_ticks_since_boot();

  /*
   * Check that clock tick gets incremented.
   */
  T_step_eq_u32( 1, result_1 - result_0, 1 );
}

/**
 * @brief Use the rtems_clock_get_ticks_per_second() directive.
 */
static void RtemsClockValClock_Action_2( void )
{
  rtems_interval result;

  result = rtems_clock_get_ticks_per_second();

  /*
   * Check that rtems_clock_get_ticks_per_second() actually returns 1us /
   * CONFIGURE_MICROSECONDS_PER_TICK.
   */
  T_step_eq_u32( 2, result, 1000000UL / TEST_MICROSECONDS_PER_TICK );
}

/**
 * @brief Use the rtems_clock_get_ticks_per_second() directive.
 */
static void RtemsClockValClock_Action_3( void )
{
  rtems_interval result;

  #undef rtems_clock_get_ticks_per_second

  result = rtems_clock_get_ticks_per_second();

  /*
   * Check that rtems_clock_get_ticks_per_second() actually returns 1us /
   * CONFIGURE_MICROSECONDS_PER_TICK.
   */
  T_step_eq_u32( 3, result, 1000000UL / TEST_MICROSECONDS_PER_TICK );
}

/**
 * @fn void T_case_body_RtemsClockValClock( void )
 */
T_TEST_CASE( RtemsClockValClock )
{
  T_plan( 4 );

  RtemsClockValClock_Action_0();
  RtemsClockValClock_Action_1();
  RtemsClockValClock_Action_2();
  RtemsClockValClock_Action_3();
}

/** @} */
