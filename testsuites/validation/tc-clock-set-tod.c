/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsClockValSetTod
 */

/*
 * Copyright (C) 2026 embedded brains GmbH & Co. KG
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>

#include "tr-tod-convert-date.h"
#include "tr-tod-convert-time.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsClockValSetTod spec:/rtems/clock/val/set-tod
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @brief Tests the validation and the conversion of the time of day by
 *   rtems_clock_set().
 *
 * This test case performs the following actions:
 *
 * - Validate the time of day of rtems_clock_set().
 *
 *   - Validate the date.
 *
 *   - Validate the time of the day.
 *
 *   - Restore a valid time of day.
 *
 * @{
 */

static const rtems_time_of_day tod_restore = { 1988, 1, 1, 0, 0, 0, 0 };

static bool CallClockSet(
  void                    *arg,
  const rtems_time_of_day *time_of_day,
  int64_t                 *seconds
)
{
  rtems_status_code status;
  struct timespec   now;

  (void) arg;
  status = rtems_clock_set( time_of_day );

  if ( status != RTEMS_SUCCESSFUL ) {
    T_quiet_rsc( status, RTEMS_INVALID_CLOCK );
    return false;
  }

  rtems_clock_get_realtime( &now );
  *seconds = now.tv_sec;
  return true;
}

/**
 * @brief Validate the time of day of rtems_clock_set().
 */
static void RtemsClockValSetTod_Action_0( void )
{
  rtems_status_code sc;

  /*
   * Validate the date.
   */
  ScoreTodReqConvertDate_Run( CallClockSet, NULL );

  /*
   * Validate the time of the day.
   */
  ScoreTodReqConvertTime_Run( CallClockSet, NULL, true );

  /*
   * Restore a valid time of day.
   */
  sc = rtems_clock_set( &tod_restore );
  T_rsc_success( sc );
}

/**
 * @fn void T_case_body_RtemsClockValSetTod( void )
 */
T_TEST_CASE( RtemsClockValSetTod )
{
  RtemsClockValSetTod_Action_0();
}

/** @} */
