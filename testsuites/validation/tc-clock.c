/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestCaseRtemsClockValClock
 */

/*
 * Copyright (C) 2021 embedded brains GmbH (http://www.embedded-brains.de)
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
 * @defgroup RTEMSTestCaseRtemsClockValClock spec:/rtems/clock/val/clock
 *
 * @ingroup RTEMSTestSuiteTestsuitesValidationNoClock0
 *
 * @brief Tests some @ref RTEMSAPIClassicClock directives.
 *
 * This test case performs the following actions:
 *
 * - Use the rtems_clock_get_ticks_since_boot() function.
 *
 *   - Check that clock tick gets incremented.
 *
 * - Use the rtems_clock_get_ticks_per_second() function.
 *
 *   - Check that rtems_clock_get_ticks_per_second() actually returns 1us /
 *     CONFIGURE_MICROSECONDS_PER_TICK.
 *
 * @{
 */

/**
 * @brief Use the rtems_clock_get_ticks_since_boot() function.
 */
static void RtemsClockValClock_Action_0( void )
{
  rtems_interval result_0;
  rtems_interval result_1;
  int32_t difference; /* Note: rtems_interval = uint32_t (unsigned!) */

  result_0 = rtems_clock_get_ticks_since_boot();
  ClockTick();
  result_1 = rtems_clock_get_ticks_since_boot();
  /*
   * Because of the ones-complement, the overflow
   * is handled correctly. result_0 = 0xFFFFFFFF will become -1
   * and result_1 = 0x0 will become 0.
   */
  difference = (int32_t) result_1 - (int32_t) result_0;

  /*
   * Check that clock tick gets incremented.
   */
  T_step_eq_i32( 0, difference, 1 );
}

/**
 * @brief Use the rtems_clock_get_ticks_per_second() function.
 */
static void RtemsClockValClock_Action_1( void )
{
  rtems_interval result;
  result = rtems_clock_get_ticks_per_second();

  /*
   * Check that rtems_clock_get_ticks_per_second() actually returns 1us /
   * CONFIGURE_MICROSECONDS_PER_TICK.
   */
  T_step_eq_u32( 1, result, 1000000UL / TEST_MICROSECONDS_PER_TICK );
}

/**
 * @fn void T_case_body_RtemsClockValClock( void )
 */
T_TEST_CASE( RtemsClockValClock )
{
  T_plan( 2 );

  RtemsClockValClock_Action_0();
  RtemsClockValClock_Action_1();
}

/** @} */
