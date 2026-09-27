/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsTaskValWakeWhenTod
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
 * @defgroup RtemsTaskValWakeWhenTod spec:/rtems/task/val/wake-when-tod
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @brief Tests the validation and the conversion of the time of day by
 *   rtems_task_wake_when().
 *
 * This test case performs the following actions:
 *
 * - Validate the time of day of rtems_task_wake_when().
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
 * @brief Test context for spec:/rtems/task/val/wake-when-tod test case.
 */
typedef struct {
  /**
   * @brief This member contains the identifier of the worker.
   */
  rtems_id worker_id;

  /**
   * @brief This member references the time of day of the call.
   */
  const rtems_time_of_day *tod;

  /**
   * @brief This member contains the return status of the call.
   */
  rtems_status_code status;
} RtemsTaskValWakeWhenTod_Context;

static RtemsTaskValWakeWhenTod_Context RtemsTaskValWakeWhenTod_Instance;

typedef RtemsTaskValWakeWhenTod_Context Context;

static const rtems_time_of_day tod_restore = { 1988, 1, 1, 0, 0, 0, 0 };

static void Worker( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;

  while ( true ) {
    SuspendSelf();
    ctx->status = rtems_task_wake_when( ctx->tod );
  }
}

static bool CallWakeWhen(
  void                    *arg,
  const rtems_time_of_day *time_of_day,
  int64_t                 *seconds
)
{
  Context          *ctx;
  rtems_status_code sc;
  TaskTimerInfo     info;

  ctx = arg;
  sc = rtems_clock_set( &tod_restore );
  T_quiet_rsc_success( sc );

  ctx->tod = time_of_day;
  ctx->status = RTEMS_NOT_IMPLEMENTED;
  ResumeTask( ctx->worker_id );
  GetTaskTimerInfo( ctx->worker_id, &info );

  if ( info.state == TASK_TIMER_REALTIME ) {
    *seconds = info.expire_timespec.tv_sec;
  }

  FinalClockTick();

  if ( ctx->status != RTEMS_SUCCESSFUL ) {
    T_quiet_rsc( ctx->status, RTEMS_INVALID_CLOCK );
    return false;
  }

  return true;
}

static void RtemsTaskValWakeWhenTod_Setup(
  RtemsTaskValWakeWhenTod_Context *ctx
)
{
  SetSelfPriority( PRIO_NORMAL );
  ctx->worker_id = CreateTask( "WORK", PRIO_HIGH );
  StartTask( ctx->worker_id, Worker, ctx );
}

static void RtemsTaskValWakeWhenTod_Setup_Wrap( void *arg )
{
  RtemsTaskValWakeWhenTod_Context *ctx;

  ctx = arg;
  RtemsTaskValWakeWhenTod_Setup( ctx );
}

static void RtemsTaskValWakeWhenTod_Teardown(
  RtemsTaskValWakeWhenTod_Context *ctx
)
{
  DeleteTask( ctx->worker_id );
  RestoreRunnerPriority();
}

static void RtemsTaskValWakeWhenTod_Teardown_Wrap( void *arg )
{
  RtemsTaskValWakeWhenTod_Context *ctx;

  ctx = arg;
  RtemsTaskValWakeWhenTod_Teardown( ctx );
}

static T_fixture RtemsTaskValWakeWhenTod_Fixture = {
  .setup = RtemsTaskValWakeWhenTod_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsTaskValWakeWhenTod_Teardown_Wrap,
  .scope = NULL,
  .initial_context = &RtemsTaskValWakeWhenTod_Instance
};

/**
 * @brief Validate the time of day of rtems_task_wake_when().
 */
static void RtemsTaskValWakeWhenTod_Action_0(
  RtemsTaskValWakeWhenTod_Context *ctx
)
{
  rtems_status_code sc;

  /*
   * Validate the date.
   */
  ScoreTodReqConvertDate_Run( CallWakeWhen, ctx );

  /*
   * Validate the time of the day.
   */
  ScoreTodReqConvertTime_Run( CallWakeWhen, ctx, false );

  /*
   * Restore a valid time of day.
   */
  sc = rtems_clock_set( &tod_restore );
  T_rsc_success( sc );
}

/**
 * @fn void T_case_body_RtemsTaskValWakeWhenTod( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsTaskValWakeWhenTod,
  &RtemsTaskValWakeWhenTod_Fixture
)
{
  RtemsTaskValWakeWhenTod_Context *ctx;

  ctx = T_fixture_context();

  RtemsTaskValWakeWhenTod_Action_0( ctx );
}

/** @} */
