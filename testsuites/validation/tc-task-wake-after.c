/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsTaskReqWakeAfter
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
#include <rtems/test-scheduler.h>
#include <rtems/score/threaddispatch.h>
#include <rtems/score/timecounter.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsTaskReqWakeAfter spec:/rtems/task/req/wake-after
 *
 * @ingroup TestsuitesValidationNoClock0
 * @ingroup TestsuitesValidationOneCpu0
 *
 * @{
 */

typedef enum {
  RtemsTaskReqWakeAfter_Pre_Ticks_Yield,
  RtemsTaskReqWakeAfter_Pre_Ticks_Interval,
  RtemsTaskReqWakeAfter_Pre_Ticks_NA
} RtemsTaskReqWakeAfter_Pre_Ticks;

typedef enum {
  RtemsTaskReqWakeAfter_Pre_Suspended_Yes,
  RtemsTaskReqWakeAfter_Pre_Suspended_No,
  RtemsTaskReqWakeAfter_Pre_Suspended_NA
} RtemsTaskReqWakeAfter_Pre_Suspended;

typedef enum {
  RtemsTaskReqWakeAfter_Post_Status_Ok,
  RtemsTaskReqWakeAfter_Post_Status_NA
} RtemsTaskReqWakeAfter_Post_Status;

typedef enum {
  RtemsTaskReqWakeAfter_Post_Timer_Inactive,
  RtemsTaskReqWakeAfter_Post_Timer_Ticks,
  RtemsTaskReqWakeAfter_Post_Timer_NA
} RtemsTaskReqWakeAfter_Post_Timer;

typedef enum {
  RtemsTaskReqWakeAfter_Post_Expire_Relative,
  RtemsTaskReqWakeAfter_Post_Expire_NA
} RtemsTaskReqWakeAfter_Post_Expire;

typedef enum {
  RtemsTaskReqWakeAfter_Post_Scheduler_Block,
  RtemsTaskReqWakeAfter_Post_Scheduler_Yield,
  RtemsTaskReqWakeAfter_Post_Scheduler_Nop,
  RtemsTaskReqWakeAfter_Post_Scheduler_NA
} RtemsTaskReqWakeAfter_Post_Scheduler;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Ticks_NA : 1;
  uint16_t Pre_Suspended_NA : 1;
  uint16_t Post_Status : 1;
  uint16_t Post_Timer : 2;
  uint16_t Post_Expire : 1;
  uint16_t Post_Scheduler : 2;
} RtemsTaskReqWakeAfter_Entry;

/**
 * @brief Test context for spec:/rtems/task/req/wake-after test case.
 */
typedef struct {
  /**
   * @brief This member provides the scheduler operation records.
   */
  T_scheduler_log_4 scheduler_log;

  /**
   * @brief This member contains the clock tick value before the
   *   rtems_task_wake_after() call.
   */
  uint64_t now;

  /**
   * @brief This member contains the worker task identifier.
   */
  rtems_id worker_id;

  /**
   * @brief If this member is true, then the worker shall be suspended during
   *   the rtems_task_wake_after() call.
   */
  bool suspended;

  /**
   * @brief This member contains the timer information of the worker task.
   */
  TaskTimerInfo timer_info;

  /**
   * @brief This member contains the return value of the
   *   rtems_task_wake_after() call.
   */
  rtems_status_code status;

  /**
   * @brief This member specifies the ``ticks`` parameter value.
   */
  rtems_interval ticks;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 2 ];

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
    RtemsTaskReqWakeAfter_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsTaskReqWakeAfter_Context;

static RtemsTaskReqWakeAfter_Context
  RtemsTaskReqWakeAfter_Instance;

static const char * const RtemsTaskReqWakeAfter_PreDesc_Ticks[] = {
  "Yield",
  "Interval",
  "NA"
};

static const char * const RtemsTaskReqWakeAfter_PreDesc_Suspended[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const * const RtemsTaskReqWakeAfter_PreDesc[] = {
  RtemsTaskReqWakeAfter_PreDesc_Ticks,
  RtemsTaskReqWakeAfter_PreDesc_Suspended,
  NULL
};

typedef RtemsTaskReqWakeAfter_Context Context;

static void Worker( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;

  while ( true ) {
    T_scheduler_log *log;

    SuspendSelf();

    ctx->now = rtems_clock_get_ticks_since_boot();

    if ( ctx->suspended ) {
      Per_CPU_Control *cpu_self;

      /*
       * The rtems_task_wake_after() disables thread dispatching to carry out
       * its operations. While thread dispatching is disabled, when an
       * interrupt suspends the calling task, the suspended task executes
       * until it enables thread dispatching.  We simulate this situation
       * with the code below.  Where the system was built with SMP support
       * enabled, other processors may suspend an executing task in parallel.
       * This case is also simulated by the code below.
       */
      cpu_self = _Thread_Dispatch_disable();
      SuspendSelf();
      cpu_self->dispatch_necessary = false;
      _Thread_Dispatch_enable( cpu_self );
    }

    log = T_scheduler_record_4( &ctx->scheduler_log );
    T_null( log );

    ctx->status = rtems_task_wake_after( ctx->ticks );

    (void) T_scheduler_record( NULL );
  }
}

static void RtemsTaskReqWakeAfter_Pre_Ticks_Prepare(
  RtemsTaskReqWakeAfter_Context  *ctx,
  RtemsTaskReqWakeAfter_Pre_Ticks state
)
{
  switch ( state ) {
    case RtemsTaskReqWakeAfter_Pre_Ticks_Yield: {
      /*
       * While the ``ticks`` parameter is equal to RTEMS_YIELD_PROCESSOR.
       */
      ctx->ticks = RTEMS_YIELD_PROCESSOR;
      break;
    }

    case RtemsTaskReqWakeAfter_Pre_Ticks_Interval: {
      /*
       * While the ``ticks`` parameter is not equal to RTEMS_YIELD_PROCESSOR.
       */
      ctx->ticks = UINT32_MAX;
      break;
    }

    case RtemsTaskReqWakeAfter_Pre_Ticks_NA:
      break;
  }
}

static void RtemsTaskReqWakeAfter_Pre_Suspended_Prepare(
  RtemsTaskReqWakeAfter_Context      *ctx,
  RtemsTaskReqWakeAfter_Pre_Suspended state
)
{
  switch ( state ) {
    case RtemsTaskReqWakeAfter_Pre_Suspended_Yes: {
      /*
       * While the calling task is suspended.
       */
      ctx->suspended = true;
      break;
    }

    case RtemsTaskReqWakeAfter_Pre_Suspended_No: {
      /*
       * While the calling task is not suspended.
       */
      ctx->suspended = false;
      break;
    }

    case RtemsTaskReqWakeAfter_Pre_Suspended_NA:
      break;
  }
}

static void RtemsTaskReqWakeAfter_Post_Status_Check(
  RtemsTaskReqWakeAfter_Context    *ctx,
  RtemsTaskReqWakeAfter_Post_Status state
)
{
  switch ( state ) {
    case RtemsTaskReqWakeAfter_Post_Status_Ok: {
      /*
       * The return status of rtems_task_wake_after() shall be
       * RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsTaskReqWakeAfter_Post_Status_NA:
      break;
  }
}

static void RtemsTaskReqWakeAfter_Post_Timer_Check(
  RtemsTaskReqWakeAfter_Context   *ctx,
  RtemsTaskReqWakeAfter_Post_Timer state
)
{
  switch ( state ) {
    case RtemsTaskReqWakeAfter_Post_Timer_Inactive: {
      /*
       * The timer of the calling task shall be inactive.
       */
      T_eq_int( ctx->timer_info.state, TASK_TIMER_INACTIVE );
      break;
    }

    case RtemsTaskReqWakeAfter_Post_Timer_Ticks: {
      /*
       * The timer of the calling task shall be active using the clock tick.
       */
      T_eq_int( ctx->timer_info.state, TASK_TIMER_TICKS );
      break;
    }

    case RtemsTaskReqWakeAfter_Post_Timer_NA:
      break;
  }
}

static void RtemsTaskReqWakeAfter_Post_Expire_Check(
  RtemsTaskReqWakeAfter_Context    *ctx,
  RtemsTaskReqWakeAfter_Post_Expire state
)
{
  switch ( state ) {
    case RtemsTaskReqWakeAfter_Post_Expire_Relative: {
      /*
       * The timer of the calling task shall expire at the time point specified
       * by the sum of the current clock tick and the interval specified by the
       * ``ticks`` parameter.
       */
      T_eq_u64( ctx->timer_info.expire_ticks, ctx->now + UINT32_MAX );
      break;
    }

    case RtemsTaskReqWakeAfter_Post_Expire_NA:
      break;
  }
}

static void RtemsTaskReqWakeAfter_Post_Scheduler_Check(
  RtemsTaskReqWakeAfter_Context       *ctx,
  RtemsTaskReqWakeAfter_Post_Scheduler state
)
{
  switch ( state ) {
    case RtemsTaskReqWakeAfter_Post_Scheduler_Block: {
      /*
       * The calling task shall be blocked by the scheduler exactly once by the
       * rtems_task_wake_after() call.
       */
      T_eq_sz( ctx->scheduler_log.header.recorded, 1 );
      T_eq_int(
        ctx->scheduler_log.events[ 0 ].operation,
        T_SCHEDULER_BLOCK
      );
      break;
    }

    case RtemsTaskReqWakeAfter_Post_Scheduler_Yield: {
      /*
       * The calling task shall yield by the scheduler exactly once by the
       * rtems_task_wake_after() call.
       */
      T_eq_sz( ctx->scheduler_log.header.recorded, 1 );
      T_eq_int(
        ctx->scheduler_log.events[ 0 ].operation,
        T_SCHEDULER_YIELD
      );
      break;
    }

    case RtemsTaskReqWakeAfter_Post_Scheduler_Nop: {
      /*
       * The calling task shall not carry out a scheduler operation through the
       * rtems_task_wake_after() call.
       */
      T_eq_sz( ctx->scheduler_log.header.recorded, 0 );
      break;
    }

    case RtemsTaskReqWakeAfter_Post_Scheduler_NA:
      break;
  }
}

static void RtemsTaskReqWakeAfter_Setup( RtemsTaskReqWakeAfter_Context *ctx )
{
  SetSelfPriority( PRIO_NORMAL );
  ctx->worker_id = CreateTask( "WORK", PRIO_HIGH );
  StartTask( ctx->worker_id, Worker, ctx );
}

static void RtemsTaskReqWakeAfter_Setup_Wrap( void *arg )
{
  RtemsTaskReqWakeAfter_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsTaskReqWakeAfter_Setup( ctx );
}

static void RtemsTaskReqWakeAfter_Teardown(
  RtemsTaskReqWakeAfter_Context *ctx
)
{
  DeleteTask( ctx->worker_id );
  RestoreRunnerPriority();
}

static void RtemsTaskReqWakeAfter_Teardown_Wrap( void *arg )
{
  RtemsTaskReqWakeAfter_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsTaskReqWakeAfter_Teardown( ctx );
}

static void RtemsTaskReqWakeAfter_Prepare( RtemsTaskReqWakeAfter_Context *ctx )
{
  ctx->status = RTEMS_NOT_IMPLEMENTED;
}

static void RtemsTaskReqWakeAfter_Action( RtemsTaskReqWakeAfter_Context *ctx )
{
  ResumeTask( ctx->worker_id );
  (void) T_scheduler_record( NULL );
  GetTaskTimerInfo( ctx->worker_id, &ctx->timer_info );

  if ( ctx->suspended ) {
    ResumeTask( ctx->worker_id );
  }

  FinalClockTick();
}

static const RtemsTaskReqWakeAfter_Entry
RtemsTaskReqWakeAfter_Entries[] = {
  { 0, 0, 0, RtemsTaskReqWakeAfter_Post_Status_Ok,
    RtemsTaskReqWakeAfter_Post_Timer_Inactive,
    RtemsTaskReqWakeAfter_Post_Expire_NA,
    RtemsTaskReqWakeAfter_Post_Scheduler_Nop },
  { 0, 0, 0, RtemsTaskReqWakeAfter_Post_Status_Ok,
    RtemsTaskReqWakeAfter_Post_Timer_Inactive,
    RtemsTaskReqWakeAfter_Post_Expire_NA,
    RtemsTaskReqWakeAfter_Post_Scheduler_Yield },
  { 0, 0, 0, RtemsTaskReqWakeAfter_Post_Status_Ok,
    RtemsTaskReqWakeAfter_Post_Timer_Ticks,
    RtemsTaskReqWakeAfter_Post_Expire_Relative,
    RtemsTaskReqWakeAfter_Post_Scheduler_Nop },
  { 0, 0, 0, RtemsTaskReqWakeAfter_Post_Status_Ok,
    RtemsTaskReqWakeAfter_Post_Timer_Ticks,
    RtemsTaskReqWakeAfter_Post_Expire_Relative,
    RtemsTaskReqWakeAfter_Post_Scheduler_Block }
};

static const uint8_t
RtemsTaskReqWakeAfter_Map[] = {
  0, 1, 2, 3
};

static size_t RtemsTaskReqWakeAfter_Scope( void *arg, char *buf, size_t n )
{
  RtemsTaskReqWakeAfter_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsTaskReqWakeAfter_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsTaskReqWakeAfter_Fixture = {
  .setup = RtemsTaskReqWakeAfter_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsTaskReqWakeAfter_Teardown_Wrap,
  .scope = RtemsTaskReqWakeAfter_Scope,
  .initial_context = &RtemsTaskReqWakeAfter_Instance
};

static inline RtemsTaskReqWakeAfter_Entry RtemsTaskReqWakeAfter_PopEntry(
  RtemsTaskReqWakeAfter_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsTaskReqWakeAfter_Entries[
    RtemsTaskReqWakeAfter_Map[ index ]
  ];
}

static void RtemsTaskReqWakeAfter_TestVariant(
  RtemsTaskReqWakeAfter_Context *ctx
)
{
  RtemsTaskReqWakeAfter_Pre_Ticks_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsTaskReqWakeAfter_Pre_Suspended_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsTaskReqWakeAfter_Action( ctx );
  RtemsTaskReqWakeAfter_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  RtemsTaskReqWakeAfter_Post_Timer_Check( ctx, ctx->Map.entry.Post_Timer );
  RtemsTaskReqWakeAfter_Post_Expire_Check( ctx, ctx->Map.entry.Post_Expire );
  RtemsTaskReqWakeAfter_Post_Scheduler_Check(
    ctx,
    ctx->Map.entry.Post_Scheduler
  );
}

/**
 * @fn void T_case_body_RtemsTaskReqWakeAfter( void )
 */
T_TEST_CASE_FIXTURE( RtemsTaskReqWakeAfter, &RtemsTaskReqWakeAfter_Fixture )
{
  RtemsTaskReqWakeAfter_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsTaskReqWakeAfter_Pre_Ticks_Yield;
    ctx->Map.pcs[ 0 ] < RtemsTaskReqWakeAfter_Pre_Ticks_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = RtemsTaskReqWakeAfter_Pre_Suspended_Yes;
      ctx->Map.pcs[ 1 ] < RtemsTaskReqWakeAfter_Pre_Suspended_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = RtemsTaskReqWakeAfter_PopEntry( ctx );
      RtemsTaskReqWakeAfter_Prepare( ctx );
      RtemsTaskReqWakeAfter_TestVariant( ctx );
    }
  }
}

/** @} */
