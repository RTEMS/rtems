/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsTaskReqWakeWhen
 */

/*
 * Copyright (C) 2021 embedded brains GmbH & Co. KG
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
#include <string.h>
#include <rtems/test-scheduler.h>
#include <rtems/score/timecounter.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsTaskReqWakeWhen spec:/rtems/task/req/wake-when
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsTaskReqWakeWhen_Pre_TODSet_Yes,
  RtemsTaskReqWakeWhen_Pre_TODSet_No,
  RtemsTaskReqWakeWhen_Pre_TODSet_NA
} RtemsTaskReqWakeWhen_Pre_TODSet;

typedef enum {
  RtemsTaskReqWakeWhen_Pre_TOD_Valid,
  RtemsTaskReqWakeWhen_Pre_TOD_Null,
  RtemsTaskReqWakeWhen_Pre_TOD_NA
} RtemsTaskReqWakeWhen_Pre_TOD;

typedef enum {
  RtemsTaskReqWakeWhen_Pre_TODObj_Future,
  RtemsTaskReqWakeWhen_Pre_TODObj_PastOrNow,
  RtemsTaskReqWakeWhen_Pre_TODObj_Invalid,
  RtemsTaskReqWakeWhen_Pre_TODObj_NA
} RtemsTaskReqWakeWhen_Pre_TODObj;

typedef enum {
  RtemsTaskReqWakeWhen_Post_Status_Ok,
  RtemsTaskReqWakeWhen_Post_Status_NotDef,
  RtemsTaskReqWakeWhen_Post_Status_InvAddr,
  RtemsTaskReqWakeWhen_Post_Status_InvClock,
  RtemsTaskReqWakeWhen_Post_Status_NA
} RtemsTaskReqWakeWhen_Post_Status;

typedef enum {
  RtemsTaskReqWakeWhen_Post_Timer_Inactive,
  RtemsTaskReqWakeWhen_Post_Timer_Realtime,
  RtemsTaskReqWakeWhen_Post_Timer_NA
} RtemsTaskReqWakeWhen_Post_Timer;

typedef enum {
  RtemsTaskReqWakeWhen_Post_Expire_Absolute,
  RtemsTaskReqWakeWhen_Post_Expire_NA
} RtemsTaskReqWakeWhen_Post_Expire;

typedef enum {
  RtemsTaskReqWakeWhen_Post_Scheduler_Block,
  RtemsTaskReqWakeWhen_Post_Scheduler_Nop,
  RtemsTaskReqWakeWhen_Post_Scheduler_NA
} RtemsTaskReqWakeWhen_Post_Scheduler;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_TODSet_NA : 1;
  uint16_t Pre_TOD_NA : 1;
  uint16_t Pre_TODObj_NA : 1;
  uint16_t Post_Status : 3;
  uint16_t Post_Timer : 2;
  uint16_t Post_Expire : 1;
  uint16_t Post_Scheduler : 2;
} RtemsTaskReqWakeWhen_Entry;

/**
 * @brief Test context for spec:/rtems/task/req/wake-when test case.
 */
typedef struct {
  /**
   * @brief This member provides the scheduler operation records.
   */
  T_scheduler_log_4 scheduler_log;

  /**
   * @brief This member contains the CLOCK_REALTIME value before the
   *   rtems_task_wake_when() call.
   */
  struct timespec now;

  /**
   * @brief This member contains the worker task identifier.
   */
  rtems_id worker_id;

  /**
   * @brief This member contains the timer information of the worker task.
   */
  TaskTimerInfo timer_info;

  /**
   * @brief This member provides the object referenced by the ``time_buffer``
   *   parameter.
   */
  rtems_time_of_day tod_obj;

  /**
   * @brief This member contains the return value of the rtems_task_wake_when()
   *   call.
   */
  rtems_status_code status;

  /**
   * @brief This member specifies the ``time_buffer`` parameter value.
   */
  const rtems_time_of_day *tod;

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 3 ];

    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 3 ];

    /**
     * @brief If this member is true, then the test action loop is executed.
     */
    bool in_action_loop;

    /**
     * @brief This member contains the next transition map index.
     */
    size_t index;

    /**
     * @brief This member contains the current transition map entry.
     */
    RtemsTaskReqWakeWhen_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsTaskReqWakeWhen_Context;

static RtemsTaskReqWakeWhen_Context
  RtemsTaskReqWakeWhen_Instance;

static const char * const RtemsTaskReqWakeWhen_PreDesc_TODSet[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const RtemsTaskReqWakeWhen_PreDesc_TOD[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const RtemsTaskReqWakeWhen_PreDesc_TODObj[] = {
  "Future",
  "PastOrNow",
  "Invalid",
  "NA"
};

static const char * const * const RtemsTaskReqWakeWhen_PreDesc[] = {
  RtemsTaskReqWakeWhen_PreDesc_TODSet,
  RtemsTaskReqWakeWhen_PreDesc_TOD,
  RtemsTaskReqWakeWhen_PreDesc_TODObj,
  NULL
};

typedef RtemsTaskReqWakeWhen_Context Context;

static void SetTOD( rtems_time_of_day *tod, uint32_t year )
{
  memset( tod, 0, sizeof( *tod ) );
  tod->year = year;
  tod->month = 1;
  tod->day = 1;
}

static void Worker( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;

  while ( true ) {
    T_scheduler_log *log;

    SuspendSelf();

    log = T_scheduler_record_4( &ctx->scheduler_log );
    T_null( log );

    _Timecounter_Getnanotime( &ctx->now );

    ctx->status = rtems_task_wake_when( ctx->tod );

    (void) T_scheduler_record( NULL );
  }
}

static void RtemsTaskReqWakeWhen_Pre_TODSet_Prepare(
  RtemsTaskReqWakeWhen_Context   *ctx,
  RtemsTaskReqWakeWhen_Pre_TODSet state
)
{
  rtems_status_code sc;
  rtems_time_of_day tod;

  switch ( state ) {
    case RtemsTaskReqWakeWhen_Pre_TODSet_Yes: {
      /*
       * While the CLOCK_REALTIME was set at least once.
       */
      SetTOD( &tod, 2000 );
      sc = rtems_clock_set( &tod );
      T_rsc_success( sc );
      break;
    }

    case RtemsTaskReqWakeWhen_Pre_TODSet_No: {
      /*
       * While the CLOCK_REALTIME was never set.
       */
      UnsetClock();
      break;
    }

    case RtemsTaskReqWakeWhen_Pre_TODSet_NA:
      break;
  }
}

static void RtemsTaskReqWakeWhen_Pre_TOD_Prepare(
  RtemsTaskReqWakeWhen_Context *ctx,
  RtemsTaskReqWakeWhen_Pre_TOD  state
)
{
  switch ( state ) {
    case RtemsTaskReqWakeWhen_Pre_TOD_Valid: {
      /*
       * While the ``time_buffer`` parameter references an object of type
       * rtems_time_of_day.
       */
      ctx->tod = &ctx->tod_obj;
      break;
    }

    case RtemsTaskReqWakeWhen_Pre_TOD_Null: {
      /*
       * While the ``time_buffer`` parameter is equal to NULL.
       */
      ctx->tod = NULL;
      break;
    }

    case RtemsTaskReqWakeWhen_Pre_TOD_NA:
      break;
  }
}

static void RtemsTaskReqWakeWhen_Pre_TODObj_Prepare(
  RtemsTaskReqWakeWhen_Context   *ctx,
  RtemsTaskReqWakeWhen_Pre_TODObj state
)
{
  switch ( state ) {
    case RtemsTaskReqWakeWhen_Pre_TODObj_Future: {
      /*
       * While the object referenced by the ``time_buffer`` parameter specifies
       * a valid time of day in the future.
       */
      SetTOD( &ctx->tod_obj, 2010 );
      break;
    }

    case RtemsTaskReqWakeWhen_Pre_TODObj_PastOrNow: {
      /*
       * While the object referenced by the ``time_buffer`` parameter specifies
       * a valid time of day in the past or at the time of the
       * rtems_task_wake_when() call.
       */
      SetTOD( &ctx->tod_obj, 1990 );
      break;
    }

    case RtemsTaskReqWakeWhen_Pre_TODObj_Invalid: {
      /*
       * While the object referenced by the ``time_buffer`` parameter specifies
       * an invalid time of day.
       */
      memset( &ctx->tod_obj, 0xff, sizeof( ctx->tod_obj ) );
      break;
    }

    case RtemsTaskReqWakeWhen_Pre_TODObj_NA:
      break;
  }
}

static void RtemsTaskReqWakeWhen_Post_Status_Check(
  RtemsTaskReqWakeWhen_Context    *ctx,
  RtemsTaskReqWakeWhen_Post_Status state
)
{
  switch ( state ) {
    case RtemsTaskReqWakeWhen_Post_Status_Ok: {
      /*
       * The return status of rtems_task_wake_when() shall be RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsTaskReqWakeWhen_Post_Status_NotDef: {
      /*
       * The return status of rtems_task_wake_when() shall be
       * RTEMS_NOT_DEFINED.
       */
      T_rsc( ctx->status, RTEMS_NOT_DEFINED );
      break;
    }

    case RtemsTaskReqWakeWhen_Post_Status_InvAddr: {
      /*
       * The return status of rtems_task_wake_when() shall be
       * RTEMS_INVALID_ADDRESS.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ADDRESS );
      break;
    }

    case RtemsTaskReqWakeWhen_Post_Status_InvClock: {
      /*
       * The return status of rtems_task_wake_when() shall be
       * RTEMS_INVALID_CLOCK.
       */
      T_rsc( ctx->status, RTEMS_INVALID_CLOCK );
      break;
    }

    case RtemsTaskReqWakeWhen_Post_Status_NA:
      break;
  }
}

static void RtemsTaskReqWakeWhen_Post_Timer_Check(
  RtemsTaskReqWakeWhen_Context   *ctx,
  RtemsTaskReqWakeWhen_Post_Timer state
)
{
  switch ( state ) {
    case RtemsTaskReqWakeWhen_Post_Timer_Inactive: {
      /*
       * The timer of the calling task shall be inactive.
       */
      T_eq_int( ctx->timer_info.state, TASK_TIMER_INACTIVE );
      break;
    }

    case RtemsTaskReqWakeWhen_Post_Timer_Realtime: {
      /*
       * The timer of the calling task shall be active using the
       * CLOCK_REALTIME.
       */
      T_eq_int( ctx->timer_info.state, TASK_TIMER_REALTIME );
      break;
    }

    case RtemsTaskReqWakeWhen_Post_Timer_NA:
      break;
  }
}

static void RtemsTaskReqWakeWhen_Post_Expire_Check(
  RtemsTaskReqWakeWhen_Context    *ctx,
  RtemsTaskReqWakeWhen_Post_Expire state
)
{
  switch ( state ) {
    case RtemsTaskReqWakeWhen_Post_Expire_Absolute: {
      /*
       * The timer of the calling task shall expire at the time point specified
       * by the ``time_buffer`` parameter.
       */
      T_eq_i64( ctx->timer_info.expire_timespec.tv_sec, 1262304000 );
      T_eq_long( ctx->timer_info.expire_timespec.tv_nsec, 0 );
      break;
    }

    case RtemsTaskReqWakeWhen_Post_Expire_NA:
      break;
  }
}

static void RtemsTaskReqWakeWhen_Post_Scheduler_Check(
  RtemsTaskReqWakeWhen_Context       *ctx,
  RtemsTaskReqWakeWhen_Post_Scheduler state
)
{
  switch ( state ) {
    case RtemsTaskReqWakeWhen_Post_Scheduler_Block: {
      /*
       * The calling task shall be blocked by the scheduler exactly once by the
       * rtems_task_wake_when() call.
       */
      T_eq_sz( ctx->scheduler_log.header.recorded, 1 );
      T_eq_int(
        ctx->scheduler_log.events[ 0 ].operation,
        T_SCHEDULER_BLOCK
      );
      break;
    }

    case RtemsTaskReqWakeWhen_Post_Scheduler_Nop: {
      /*
       * The calling task shall not be altered by the scheduler by the
       * rtems_task_wake_when() call.
       */
      T_eq_sz( ctx->scheduler_log.header.recorded, 0 );
      break;
    }

    case RtemsTaskReqWakeWhen_Post_Scheduler_NA:
      break;
  }
}

static void RtemsTaskReqWakeWhen_Setup( RtemsTaskReqWakeWhen_Context *ctx )
{
  SetSelfPriority( PRIO_NORMAL );
  ctx->worker_id = CreateTask( "WORK", PRIO_HIGH );
  StartTask( ctx->worker_id, Worker, ctx );
}

static void RtemsTaskReqWakeWhen_Setup_Wrap( void *arg )
{
  RtemsTaskReqWakeWhen_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsTaskReqWakeWhen_Setup( ctx );
}

static void RtemsTaskReqWakeWhen_Teardown( RtemsTaskReqWakeWhen_Context *ctx )
{
  DeleteTask( ctx->worker_id );
  RestoreRunnerPriority();
}

static void RtemsTaskReqWakeWhen_Teardown_Wrap( void *arg )
{
  RtemsTaskReqWakeWhen_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsTaskReqWakeWhen_Teardown( ctx );
}

static void RtemsTaskReqWakeWhen_Prepare( RtemsTaskReqWakeWhen_Context *ctx )
{
  ctx->status = RTEMS_NOT_IMPLEMENTED;
}

static void RtemsTaskReqWakeWhen_Action( RtemsTaskReqWakeWhen_Context *ctx )
{
  ResumeTask( ctx->worker_id );
  (void) T_scheduler_record( NULL );
  GetTaskTimerInfo( ctx->worker_id, &ctx->timer_info );
  FinalClockTick();
}

static const RtemsTaskReqWakeWhen_Entry
RtemsTaskReqWakeWhen_Entries[] = {
  { 0, 0, 0, 1, RtemsTaskReqWakeWhen_Post_Status_InvAddr,
    RtemsTaskReqWakeWhen_Post_Timer_Inactive,
    RtemsTaskReqWakeWhen_Post_Expire_NA,
    RtemsTaskReqWakeWhen_Post_Scheduler_Nop },
  { 0, 0, 0, 0, RtemsTaskReqWakeWhen_Post_Status_NotDef,
    RtemsTaskReqWakeWhen_Post_Timer_Inactive,
    RtemsTaskReqWakeWhen_Post_Expire_NA,
    RtemsTaskReqWakeWhen_Post_Scheduler_Nop },
  { 0, 0, 0, 1, RtemsTaskReqWakeWhen_Post_Status_NotDef,
    RtemsTaskReqWakeWhen_Post_Timer_Inactive,
    RtemsTaskReqWakeWhen_Post_Expire_NA,
    RtemsTaskReqWakeWhen_Post_Scheduler_Nop },
  { 0, 0, 0, 0, RtemsTaskReqWakeWhen_Post_Status_InvClock,
    RtemsTaskReqWakeWhen_Post_Timer_Inactive,
    RtemsTaskReqWakeWhen_Post_Expire_NA,
    RtemsTaskReqWakeWhen_Post_Scheduler_Nop },
  { 0, 0, 0, 0, RtemsTaskReqWakeWhen_Post_Status_Ok,
    RtemsTaskReqWakeWhen_Post_Timer_Realtime,
    RtemsTaskReqWakeWhen_Post_Expire_Absolute,
    RtemsTaskReqWakeWhen_Post_Scheduler_Block }
};

static const uint8_t
RtemsTaskReqWakeWhen_Map[] = {
  4, 3, 3, 0, 0, 0, 1, 1, 1, 2, 2, 2
};

static size_t RtemsTaskReqWakeWhen_Scope( void *arg, char *buf, size_t n )
{
  RtemsTaskReqWakeWhen_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsTaskReqWakeWhen_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsTaskReqWakeWhen_Fixture = {
  .setup = RtemsTaskReqWakeWhen_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsTaskReqWakeWhen_Teardown_Wrap,
  .scope = RtemsTaskReqWakeWhen_Scope,
  .initial_context = &RtemsTaskReqWakeWhen_Instance
};

static inline RtemsTaskReqWakeWhen_Entry RtemsTaskReqWakeWhen_PopEntry(
  RtemsTaskReqWakeWhen_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsTaskReqWakeWhen_Entries[
    RtemsTaskReqWakeWhen_Map[ index ]
  ];
}

static void RtemsTaskReqWakeWhen_SetPreConditionStates(
  RtemsTaskReqWakeWhen_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];

  if ( ctx->Map.entry.Pre_TODObj_NA ) {
    ctx->Map.pcs[ 2 ] = RtemsTaskReqWakeWhen_Pre_TODObj_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }
}

static void RtemsTaskReqWakeWhen_TestVariant(
  RtemsTaskReqWakeWhen_Context *ctx
)
{
  RtemsTaskReqWakeWhen_Pre_TODSet_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsTaskReqWakeWhen_Pre_TOD_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsTaskReqWakeWhen_Pre_TODObj_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsTaskReqWakeWhen_Action( ctx );
  RtemsTaskReqWakeWhen_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  RtemsTaskReqWakeWhen_Post_Timer_Check( ctx, ctx->Map.entry.Post_Timer );
  RtemsTaskReqWakeWhen_Post_Expire_Check( ctx, ctx->Map.entry.Post_Expire );
  RtemsTaskReqWakeWhen_Post_Scheduler_Check(
    ctx,
    ctx->Map.entry.Post_Scheduler
  );
}

/**
 * @fn void T_case_body_RtemsTaskReqWakeWhen( void )
 */
T_TEST_CASE_FIXTURE( RtemsTaskReqWakeWhen, &RtemsTaskReqWakeWhen_Fixture )
{
  RtemsTaskReqWakeWhen_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = RtemsTaskReqWakeWhen_Pre_TODSet_Yes;
    ctx->Map.pci[ 0 ] < RtemsTaskReqWakeWhen_Pre_TODSet_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = RtemsTaskReqWakeWhen_Pre_TOD_Valid;
      ctx->Map.pci[ 1 ] < RtemsTaskReqWakeWhen_Pre_TOD_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = RtemsTaskReqWakeWhen_Pre_TODObj_Future;
        ctx->Map.pci[ 2 ] < RtemsTaskReqWakeWhen_Pre_TODObj_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        ctx->Map.entry = RtemsTaskReqWakeWhen_PopEntry( ctx );
        RtemsTaskReqWakeWhen_SetPreConditionStates( ctx );
        RtemsTaskReqWakeWhen_Prepare( ctx );
        RtemsTaskReqWakeWhen_TestVariant( ctx );
      }
    }
  }
}

/** @} */
