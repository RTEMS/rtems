/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsTimerValFireWhenTod
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
#include <rtems/score/watchdogimpl.h>

#include "tr-tod-convert-date.h"
#include "tr-tod-convert-time.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsTimerValFireWhenTod spec:/rtems/timer/val/fire-when-tod
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @brief Tests the validation and the conversion of the time of day by
 *   rtems_timer_fire_when().
 *
 * This test case performs the following actions:
 *
 * - Validate the time of day of rtems_timer_fire_when().
 *
 *   - Validate the date.
 *
 *   - Validate the time of the day.
 *
 *   - Restore a valid time of day.
 *
 * @{
 */

/**
 * @brief Test context for spec:/rtems/timer/val/fire-when-tod test case.
 */
typedef struct {
  /**
   * @brief This member contains the identifier of the timer.
   */
  rtems_id timer_id;
} RtemsTimerValFireWhenTod_Context;

static RtemsTimerValFireWhenTod_Context RtemsTimerValFireWhenTod_Instance;

typedef RtemsTimerValFireWhenTod_Context Context;

static const rtems_time_of_day tod_earliest = { 1988, 1, 1, 0, 0, 0, 0 };

static void TimerServiceRoutine( rtems_id timer_id, void *arg )
{
  (void) timer_id;
  (void) arg;
}

static bool CallFireWhen(
  void                    *arg,
  const rtems_time_of_day *time_of_day,
  int64_t                 *seconds
)
{
  Context              *ctx;
  rtems_status_code     sc;
  Timer_Scheduling_Data data;

  ctx = arg;
  T_quiet_rsc_success( rtems_clock_set( &tod_earliest ) );
  sc = rtems_timer_fire_when(
    ctx->timer_id,
    time_of_day,
    TimerServiceRoutine,
    ctx
  );

  if ( sc != RTEMS_SUCCESSFUL ) {
    T_quiet_rsc( sc, RTEMS_INVALID_CLOCK );
    return false;
  }

  GetTimerSchedulingData( ctx->timer_id, &data );
  *seconds = (int64_t) ( data.expire >> WATCHDOG_BITS_FOR_1E9_NANOSECONDS );
  T_quiet_rsc_success( rtems_timer_cancel( ctx->timer_id ) );
  return true;
}

static void RtemsTimerValFireWhenTod_Setup(
  RtemsTimerValFireWhenTod_Context *ctx
)
{
  rtems_status_code sc;

  sc = rtems_timer_create( OBJECT_NAME, &ctx->timer_id );
  T_rsc_success( sc );
}

static void RtemsTimerValFireWhenTod_Setup_Wrap( void *arg )
{
  RtemsTimerValFireWhenTod_Context *ctx;

  ctx = arg;
  RtemsTimerValFireWhenTod_Setup( ctx );
}

static void RtemsTimerValFireWhenTod_Teardown(
  RtemsTimerValFireWhenTod_Context *ctx
)
{
  rtems_status_code sc;

  sc = rtems_timer_delete( ctx->timer_id );
  T_rsc_success( sc );
}

static void RtemsTimerValFireWhenTod_Teardown_Wrap( void *arg )
{
  RtemsTimerValFireWhenTod_Context *ctx;

  ctx = arg;
  RtemsTimerValFireWhenTod_Teardown( ctx );
}

static T_fixture RtemsTimerValFireWhenTod_Fixture = {
  .setup = RtemsTimerValFireWhenTod_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsTimerValFireWhenTod_Teardown_Wrap,
  .scope = NULL,
  .initial_context = &RtemsTimerValFireWhenTod_Instance
};

/**
 * @brief Validate the time of day of rtems_timer_fire_when().
 */
static void RtemsTimerValFireWhenTod_Action_0(
  RtemsTimerValFireWhenTod_Context *ctx
)
{
  rtems_status_code sc;

  /*
   * Validate the date.
   */
  ScoreTodReqConvertDate_Run( CallFireWhen, ctx );

  /*
   * Validate the time of the day.
   */
  ScoreTodReqConvertTime_Run( CallFireWhen, ctx, true );

  /*
   * Restore a valid time of day.
   */
  sc = rtems_clock_set( &tod_earliest );
  T_rsc_success( sc );
}

/**
 * @fn void T_case_body_RtemsTimerValFireWhenTod( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsTimerValFireWhenTod,
  &RtemsTimerValFireWhenTod_Fixture
)
{
  RtemsTimerValFireWhenTod_Context *ctx;

  ctx = T_fixture_context();

  RtemsTimerValFireWhenTod_Action_0( ctx );
}

/** @} */
