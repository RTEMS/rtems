/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsRatemonReqTimeout
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

#include <rtems/test-scheduler.h>
#include <rtems/rtems/ratemonimpl.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsRatemonReqTimeout spec:/rtems/ratemon/req/timeout
 *
 * @ingroup TestsuitesValidationNoClock0
 * @ingroup TestsuitesValidationOneCpu0
 *
 * @{
 */

typedef enum {
  RtemsRatemonReqTimeout_Pre_WaitFor_PeriodSelf,
  RtemsRatemonReqTimeout_Pre_WaitFor_PeriodOther,
  RtemsRatemonReqTimeout_Pre_WaitFor_Other,
  RtemsRatemonReqTimeout_Pre_WaitFor_NA
} RtemsRatemonReqTimeout_Pre_WaitFor;

typedef enum {
  RtemsRatemonReqTimeout_Pre_WaitState_Blocked,
  RtemsRatemonReqTimeout_Pre_WaitState_IntendToBlock,
  RtemsRatemonReqTimeout_Pre_WaitState_NA
} RtemsRatemonReqTimeout_Pre_WaitState;

typedef enum {
  RtemsRatemonReqTimeout_Pre_PostponedJobs_Zero,
  RtemsRatemonReqTimeout_Pre_PostponedJobs_NotZeroOrMax,
  RtemsRatemonReqTimeout_Pre_PostponedJobs_Max,
  RtemsRatemonReqTimeout_Pre_PostponedJobs_NA
} RtemsRatemonReqTimeout_Pre_PostponedJobs;

typedef enum {
  RtemsRatemonReqTimeout_Post_PostponedJobs_Nop,
  RtemsRatemonReqTimeout_Post_PostponedJobs_PlusOne,
  RtemsRatemonReqTimeout_Post_PostponedJobs_NA
} RtemsRatemonReqTimeout_Post_PostponedJobs;

typedef enum {
  RtemsRatemonReqTimeout_Post_ReleaseJob_Yes,
  RtemsRatemonReqTimeout_Post_ReleaseJob_No,
  RtemsRatemonReqTimeout_Post_ReleaseJob_NA
} RtemsRatemonReqTimeout_Post_ReleaseJob;

typedef enum {
  RtemsRatemonReqTimeout_Post_Unblock_Yes,
  RtemsRatemonReqTimeout_Post_Unblock_No,
  RtemsRatemonReqTimeout_Post_Unblock_NA
} RtemsRatemonReqTimeout_Post_Unblock;

typedef enum {
  RtemsRatemonReqTimeout_Post_PeriodState_Active,
  RtemsRatemonReqTimeout_Post_PeriodState_Expired,
  RtemsRatemonReqTimeout_Post_PeriodState_NA
} RtemsRatemonReqTimeout_Post_PeriodState;

typedef enum {
  RtemsRatemonReqTimeout_Post_Timer_Active,
  RtemsRatemonReqTimeout_Post_Timer_NA
} RtemsRatemonReqTimeout_Post_Timer;

typedef enum {
  RtemsRatemonReqTimeout_Post_Uptime_Nop,
  RtemsRatemonReqTimeout_Post_Uptime_Set,
  RtemsRatemonReqTimeout_Post_Uptime_NA
} RtemsRatemonReqTimeout_Post_Uptime;

typedef enum {
  RtemsRatemonReqTimeout_Post_CPUUsage_Nop,
  RtemsRatemonReqTimeout_Post_CPUUsage_Set,
  RtemsRatemonReqTimeout_Post_CPUUsage_NA
} RtemsRatemonReqTimeout_Post_CPUUsage;

typedef struct {
  uint32_t Skip : 1;
  uint32_t Pre_WaitFor_NA : 1;
  uint32_t Pre_WaitState_NA : 1;
  uint32_t Pre_PostponedJobs_NA : 1;
  uint32_t Post_PostponedJobs : 2;
  uint32_t Post_ReleaseJob : 2;
  uint32_t Post_Unblock : 2;
  uint32_t Post_PeriodState : 2;
  uint32_t Post_Timer : 1;
  uint32_t Post_Uptime : 2;
  uint32_t Post_CPUUsage : 2;
} RtemsRatemonReqTimeout_Entry;

/**
 * @brief Test context for spec:/rtems/ratemon/req/timeout test case.
 */
typedef struct {
  /**
   * @brief This member contains the period identifier.
   */
  rtems_id period_id;

  /**
   * @brief This member references the period control block.
   */
  Rate_monotonic_Control *period;

  /**
   * @brief This member contains another period identifier.
   */
  rtems_id other_period_id;

  /**
   * @brief This member contains the worker task identifier.
   */
  rtems_id worker_id;

  /**
   * @brief This member contains the call within ISR request.
   */
  CallWithinISRRequest request;

  /**
   * @brief If this member is true, then the worker shall wait for a period.
   */
  bool wait_for_period;

  /**
   * @brief If this member is true, then the worker shall wait for another
   *   period.
   */
  bool period_is_other;

  /**
   * @brief If this member is true, then the worker shall intend to block for a
   *   period.
   */
  bool intend_to_block;

  /**
   * @brief This member contains the postponed jobs count before the timeout.
   */
  uint32_t postponed_jobs;

  /**
   * @brief This member contains the uptime of the period before the timeout.
   */
  Timestamp_Control uptime_before;

  /**
   * @brief This member contains the CPU usage of the period before the
   *   timeout.
   */
  Timestamp_Control cpu_usage_before;

  /**
   * @brief This member contains the release job counter.
   */
  uint32_t release_job_counter;

  /**
   * @brief This member contains the unblock counter.
   */
  uint32_t unblock_counter;

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
    RtemsRatemonReqTimeout_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsRatemonReqTimeout_Context;

static RtemsRatemonReqTimeout_Context
  RtemsRatemonReqTimeout_Instance;

static const char * const RtemsRatemonReqTimeout_PreDesc_WaitFor[] = {
  "PeriodSelf",
  "PeriodOther",
  "Other",
  "NA"
};

static const char * const RtemsRatemonReqTimeout_PreDesc_WaitState[] = {
  "Blocked",
  "IntendToBlock",
  "NA"
};

static const char * const RtemsRatemonReqTimeout_PreDesc_PostponedJobs[] = {
  "Zero",
  "NotZeroOrMax",
  "Max",
  "NA"
};

static const char * const * const RtemsRatemonReqTimeout_PreDesc[] = {
  RtemsRatemonReqTimeout_PreDesc_WaitFor,
  RtemsRatemonReqTimeout_PreDesc_WaitState,
  RtemsRatemonReqTimeout_PreDesc_PostponedJobs,
  NULL
};

#define EVENT_RESET RTEMS_EVENT_0

#define EVENT_PERIOD_WAIT RTEMS_EVENT_1

#define EVENT_PERIOD_OTHER RTEMS_EVENT_2

typedef RtemsRatemonReqTimeout_Context Context;

static void Tick( void *arg )
{
  Context               *ctx;
  T_scheduler_log_10     scheduler_log_10;
  const T_scheduler_log *scheduler_log;
  size_t                 index;

  ctx = arg;
  ctx->release_job_counter = 0;
  ctx->unblock_counter = 0;
  ctx->uptime_before = ctx->period->time_period_initiated;
  ctx->cpu_usage_before = ctx->period->cpu_usage_period_initiated;
  scheduler_log = T_scheduler_record_10( &scheduler_log_10 );
  T_null( scheduler_log );
  ClockTick();
  scheduler_log = T_scheduler_record( NULL );
  T_eq_ptr( &scheduler_log->header, &scheduler_log_10.header );

  index = 0;

  while ( true ) {
    const T_scheduler_event *event;

    event = T_scheduler_next_any( &scheduler_log_10.header, &index );

    if ( event == &T_scheduler_event_null ) {
      break;
    }

    T_eq_u32( event->thread->Object.id, ctx->worker_id );

    switch ( event->operation ) {
      case T_SCHEDULER_RELEASE_JOB:
        ++ctx->release_job_counter;
        T_eq_u64(
          event->release_job.deadline,
          rtems_clock_get_ticks_since_boot() + 1
        );
        break;
      case T_SCHEDULER_UNBLOCK:
        ++ctx->unblock_counter;
        break;
      default:
        break;
    }
  }
}

static void SchedulerBlock(
  void                    *arg,
  const T_scheduler_event *event,
  T_scheduler_when         when
)
{
  Context *ctx;

  ctx = arg;

  if (
    when == T_SCHEDULER_BEFORE &&
    event->operation == T_SCHEDULER_BLOCK
  ) {
    T_scheduler_set_event_handler( NULL, NULL );
    ctx->request.handler = Tick;
    CallWithinISRSubmit( &ctx->request );
  }
}

static Rate_monotonic_Control *GetControl( rtems_id id )
{
  Rate_monotonic_Control *period;
  ISR_lock_Context        lock_context;

  period = _Rate_monotonic_Get( id, &lock_context );
  T_assert_not_null( period );
  _ISR_lock_ISR_enable( &lock_context );

  return period;
}

static void Worker( rtems_task_argument arg )
{
  Context          *ctx;
  rtems_status_code sc;

  ctx = (Context *) arg;

  sc = rtems_rate_monotonic_create( OBJECT_NAME, &ctx->period_id );
  T_rsc_success( sc );

  sc = rtems_rate_monotonic_create( OBJECT_NAME, &ctx->other_period_id );
  T_rsc_success( sc );

  while ( true ) {
    rtems_event_set events;

    events = ReceiveAnyEvents();

    if ( ( events & EVENT_RESET ) != 0 ) {
      sc = rtems_rate_monotonic_cancel( ctx->period_id );
      T_rsc_success( sc );

      sc = rtems_rate_monotonic_cancel( ctx->other_period_id );
      T_rsc_success( sc );

      sc = rtems_rate_monotonic_period( ctx->period_id, 1 );
      T_rsc_success( sc );

      ctx->period->postponed_jobs = ctx->postponed_jobs;
    }

    if ( ( events & EVENT_PERIOD_WAIT ) != 0 ) {
      if ( ctx->intend_to_block ) {
        T_scheduler_set_event_handler( SchedulerBlock, ctx );
      }

      sc = rtems_rate_monotonic_period( ctx->period_id, 1 );
      T_rsc_success( sc );
    }

    if ( ( events & EVENT_PERIOD_OTHER ) != 0 ) {
      sc = rtems_rate_monotonic_period( ctx->other_period_id, 2 );
      T_rsc_success( sc );

      sc = rtems_rate_monotonic_period( ctx->other_period_id, 2 );
      T_rsc_success( sc );
    }
  }
}

static void RtemsRatemonReqTimeout_Pre_WaitFor_Prepare(
  RtemsRatemonReqTimeout_Context    *ctx,
  RtemsRatemonReqTimeout_Pre_WaitFor state
)
{
  switch ( state ) {
    case RtemsRatemonReqTimeout_Pre_WaitFor_PeriodSelf: {
      /*
       * While the owner task of the period waits for the period.
       */
      ctx->wait_for_period = true;
      ctx->period_is_other = false;
      break;
    }

    case RtemsRatemonReqTimeout_Pre_WaitFor_PeriodOther: {
      /*
       * While the owner task of the period waits for another period.
       */
      ctx->wait_for_period = true;
      ctx->period_is_other = true;
      break;
    }

    case RtemsRatemonReqTimeout_Pre_WaitFor_Other: {
      /*
       * While the owner task of the period does not wait for a period.
       */
      ctx->wait_for_period = false;
      break;
    }

    case RtemsRatemonReqTimeout_Pre_WaitFor_NA:
      break;
  }
}

static void RtemsRatemonReqTimeout_Pre_WaitState_Prepare(
  RtemsRatemonReqTimeout_Context      *ctx,
  RtemsRatemonReqTimeout_Pre_WaitState state
)
{
  switch ( state ) {
    case RtemsRatemonReqTimeout_Pre_WaitState_Blocked: {
      /*
       * While the owner task is in the blocked wait state.
       */
      ctx->intend_to_block = false;
      break;
    }

    case RtemsRatemonReqTimeout_Pre_WaitState_IntendToBlock: {
      /*
       * While the owner task is in the intend to block wait state.
       */
      ctx->intend_to_block = true;
      break;
    }

    case RtemsRatemonReqTimeout_Pre_WaitState_NA:
      break;
  }
}

static void RtemsRatemonReqTimeout_Pre_PostponedJobs_Prepare(
  RtemsRatemonReqTimeout_Context          *ctx,
  RtemsRatemonReqTimeout_Pre_PostponedJobs state
)
{
  switch ( state ) {
    case RtemsRatemonReqTimeout_Pre_PostponedJobs_Zero: {
      /*
       * While the count of postponed jobs is equal to zero.
       */
      ctx->postponed_jobs = 0;
      break;
    }

    case RtemsRatemonReqTimeout_Pre_PostponedJobs_NotZeroOrMax: {
      /*
       * While the count of postponed jobs is not equal to zero or UINT32_MAX.
       */
      ctx->postponed_jobs = 123;
      break;
    }

    case RtemsRatemonReqTimeout_Pre_PostponedJobs_Max: {
      /*
       * While the count of postponed jobs is equal to UINT32_MAX.
       */
      ctx->postponed_jobs = UINT32_MAX;
      break;
    }

    case RtemsRatemonReqTimeout_Pre_PostponedJobs_NA:
      break;
  }
}

static void RtemsRatemonReqTimeout_Post_PostponedJobs_Check(
  RtemsRatemonReqTimeout_Context           *ctx,
  RtemsRatemonReqTimeout_Post_PostponedJobs state
)
{
  switch ( state ) {
    case RtemsRatemonReqTimeout_Post_PostponedJobs_Nop: {
      /*
       * The count of postponed jobs of the period shall not be modified.
       */
      T_eq_u32( ctx->period->postponed_jobs, ctx->postponed_jobs );
      break;
    }

    case RtemsRatemonReqTimeout_Post_PostponedJobs_PlusOne: {
      /*
       * The count of postponed jobs of the period shall be incremented by one.
       */
      T_eq_u32( ctx->period->postponed_jobs, ctx->postponed_jobs + 1 );
      break;
    }

    case RtemsRatemonReqTimeout_Post_PostponedJobs_NA:
      break;
  }
}

static void RtemsRatemonReqTimeout_Post_ReleaseJob_Check(
  RtemsRatemonReqTimeout_Context        *ctx,
  RtemsRatemonReqTimeout_Post_ReleaseJob state
)
{
  switch ( state ) {
    case RtemsRatemonReqTimeout_Post_ReleaseJob_Yes: {
      /*
       * The owner task of the period shall release a job with a deadline equal
       * to the clock tick plus the next period length by the timeout
       * operation.
       */
      T_eq_u32( ctx->release_job_counter, 1 );
      break;
    }

    case RtemsRatemonReqTimeout_Post_ReleaseJob_No: {
      /*
       * The owner task of the period shall not release a job by the timeout
       * operation.
       */
      T_eq_u32( ctx->release_job_counter, 0 );
      break;
    }

    case RtemsRatemonReqTimeout_Post_ReleaseJob_NA:
      break;
  }
}

static void RtemsRatemonReqTimeout_Post_Unblock_Check(
  RtemsRatemonReqTimeout_Context     *ctx,
  RtemsRatemonReqTimeout_Post_Unblock state
)
{
  switch ( state ) {
    case RtemsRatemonReqTimeout_Post_Unblock_Yes: {
      /*
       * The owner task of the period shall be unblocked by the timeout
       * operation.
       */
      T_eq_u32( ctx->unblock_counter, 1 );
      break;
    }

    case RtemsRatemonReqTimeout_Post_Unblock_No: {
      /*
       * The owner task of the period shall not be unblocked by the timeout
       * operation.
       */
      T_eq_u32( ctx->unblock_counter, 0 );
      break;
    }

    case RtemsRatemonReqTimeout_Post_Unblock_NA:
      break;
  }
}

static void RtemsRatemonReqTimeout_Post_PeriodState_Check(
  RtemsRatemonReqTimeout_Context         *ctx,
  RtemsRatemonReqTimeout_Post_PeriodState state
)
{
  switch ( state ) {
    case RtemsRatemonReqTimeout_Post_PeriodState_Active: {
      /*
       * The period state shall be active.
       */
      T_eq_int( ctx->period->state, RATE_MONOTONIC_ACTIVE );
      break;
    }

    case RtemsRatemonReqTimeout_Post_PeriodState_Expired: {
      /*
       * The period state shall be expired.
       */
      T_eq_int( ctx->period->state, RATE_MONOTONIC_EXPIRED );
      break;
    }

    case RtemsRatemonReqTimeout_Post_PeriodState_NA:
      break;
  }
}

static void RtemsRatemonReqTimeout_Post_Timer_Check(
  RtemsRatemonReqTimeout_Context   *ctx,
  RtemsRatemonReqTimeout_Post_Timer state
)
{
  switch ( state ) {
    case RtemsRatemonReqTimeout_Post_Timer_Active: {
      /*
       * The timeout timer of the period shall expired at the current clock
       * tick plus the next period length.
       */
      T_true( _Watchdog_Is_scheduled( &ctx->period->Timer ) );
      T_eq_u64(
        ctx->period->Timer.expire,
        rtems_clock_get_ticks_since_boot() + 1
      );
      break;
    }

    case RtemsRatemonReqTimeout_Post_Timer_NA:
      break;
  }
}

static void RtemsRatemonReqTimeout_Post_Uptime_Check(
  RtemsRatemonReqTimeout_Context    *ctx,
  RtemsRatemonReqTimeout_Post_Uptime state
)
{
  switch ( state ) {
    case RtemsRatemonReqTimeout_Post_Uptime_Nop: {
      /*
       * The period initiated CLOCK_MONOTONIC value shall not be modified.
       */
      T_eq_i64( ctx->uptime_before, ctx->period->time_period_initiated );
      break;
    }

    case RtemsRatemonReqTimeout_Post_Uptime_Set: {
      /*
       * The period initiated CLOCK_MONOTONIC value shall be set to the
       * CLOCK_MONOTONIC at some time point during the timeout operation.
       */
      T_ne_i64( ctx->uptime_before, ctx->period->time_period_initiated );
      break;
    }

    case RtemsRatemonReqTimeout_Post_Uptime_NA:
      break;
  }
}

static void RtemsRatemonReqTimeout_Post_CPUUsage_Check(
  RtemsRatemonReqTimeout_Context      *ctx,
  RtemsRatemonReqTimeout_Post_CPUUsage state
)
{
  switch ( state ) {
    case RtemsRatemonReqTimeout_Post_CPUUsage_Nop: {
      /*
       * The period initiated CPU usage of the owner task value shall not be
       * modified.
       */
      T_eq_i64(
        ctx->cpu_usage_before,
        ctx->period->cpu_usage_period_initiated
      );
      break;
    }

    case RtemsRatemonReqTimeout_Post_CPUUsage_Set: {
      /*
       * The period initiated CPU usage of the owner task value shall be set to
       * the CPU usage of the owner task at some time point during the timeout
       * operation.
       */
      T_ne_i64(
        ctx->cpu_usage_before,
        ctx->period->cpu_usage_period_initiated
      );
      break;
    }

    case RtemsRatemonReqTimeout_Post_CPUUsage_NA:
      break;
  }
}

static void RtemsRatemonReqTimeout_Setup( RtemsRatemonReqTimeout_Context *ctx )
{
  ctx->request.arg = ctx;
  ctx->worker_id = CreateTask( "WORK", GetSelfPriority() );
  StartTask( ctx->worker_id, Worker, ctx );
  Yield();
  ctx->period = GetControl( ctx->period_id );
}

static void RtemsRatemonReqTimeout_Setup_Wrap( void *arg )
{
  RtemsRatemonReqTimeout_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsRatemonReqTimeout_Setup( ctx );
}

static void RtemsRatemonReqTimeout_Teardown(
  RtemsRatemonReqTimeout_Context *ctx
)
{
  rtems_status_code sc;

  DeleteTask( ctx->worker_id );

  sc = rtems_rate_monotonic_delete( ctx->period_id );
  T_rsc_success( sc );

  sc = rtems_rate_monotonic_delete( ctx->other_period_id );
  T_rsc_success( sc );
}

static void RtemsRatemonReqTimeout_Teardown_Wrap( void *arg )
{
  RtemsRatemonReqTimeout_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsRatemonReqTimeout_Teardown( ctx );
}

static void RtemsRatemonReqTimeout_Action(
  RtemsRatemonReqTimeout_Context *ctx
)
{
  SendEvents( ctx->worker_id, EVENT_RESET );
  Yield();

  if ( ctx->wait_for_period ) {
    if ( ctx->period_is_other ) {
      SendEvents( ctx->worker_id, EVENT_PERIOD_OTHER );
      Yield();
      Tick( ctx );
    } else {
      SendEvents( ctx->worker_id, EVENT_PERIOD_WAIT );
      Yield();

      if ( !ctx->intend_to_block ) {
        Tick( ctx );
      }
    }
  } else {
    Tick( ctx );
  }
}

static void RtemsRatemonReqTimeout_Cleanup(
  RtemsRatemonReqTimeout_Context *ctx
)
{
  ClockTick();
  Yield();
}

static const RtemsRatemonReqTimeout_Entry
RtemsRatemonReqTimeout_Entries[] = {
  { 0, 0, 1, 0, RtemsRatemonReqTimeout_Post_PostponedJobs_PlusOne,
    RtemsRatemonReqTimeout_Post_ReleaseJob_No,
    RtemsRatemonReqTimeout_Post_Unblock_No,
    RtemsRatemonReqTimeout_Post_PeriodState_Expired,
    RtemsRatemonReqTimeout_Post_Timer_Active,
    RtemsRatemonReqTimeout_Post_Uptime_Nop,
    RtemsRatemonReqTimeout_Post_CPUUsage_Nop },
  { 1, 0, 0, 0, RtemsRatemonReqTimeout_Post_PostponedJobs_NA,
    RtemsRatemonReqTimeout_Post_ReleaseJob_NA,
    RtemsRatemonReqTimeout_Post_Unblock_NA,
    RtemsRatemonReqTimeout_Post_PeriodState_NA,
    RtemsRatemonReqTimeout_Post_Timer_NA,
    RtemsRatemonReqTimeout_Post_Uptime_NA,
    RtemsRatemonReqTimeout_Post_CPUUsage_NA },
  { 0, 0, 1, 0, RtemsRatemonReqTimeout_Post_PostponedJobs_Nop,
    RtemsRatemonReqTimeout_Post_ReleaseJob_No,
    RtemsRatemonReqTimeout_Post_Unblock_No,
    RtemsRatemonReqTimeout_Post_PeriodState_Expired,
    RtemsRatemonReqTimeout_Post_Timer_Active,
    RtemsRatemonReqTimeout_Post_Uptime_Nop,
    RtemsRatemonReqTimeout_Post_CPUUsage_Nop },
  { 0, 0, 0, 0, RtemsRatemonReqTimeout_Post_PostponedJobs_Nop,
    RtemsRatemonReqTimeout_Post_ReleaseJob_Yes,
    RtemsRatemonReqTimeout_Post_Unblock_Yes,
    RtemsRatemonReqTimeout_Post_PeriodState_Active,
    RtemsRatemonReqTimeout_Post_Timer_Active,
    RtemsRatemonReqTimeout_Post_Uptime_Set,
    RtemsRatemonReqTimeout_Post_CPUUsage_Set },
  { 0, 0, 0, 0, RtemsRatemonReqTimeout_Post_PostponedJobs_Nop,
    RtemsRatemonReqTimeout_Post_ReleaseJob_Yes,
    RtemsRatemonReqTimeout_Post_Unblock_No,
    RtemsRatemonReqTimeout_Post_PeriodState_Active,
    RtemsRatemonReqTimeout_Post_Timer_Active,
    RtemsRatemonReqTimeout_Post_Uptime_Set,
    RtemsRatemonReqTimeout_Post_CPUUsage_Set }
};

static const uint8_t
RtemsRatemonReqTimeout_Map[] = {
  3, 1, 1, 4, 1, 1, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 2
};

static size_t RtemsRatemonReqTimeout_Scope( void *arg, char *buf, size_t n )
{
  RtemsRatemonReqTimeout_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsRatemonReqTimeout_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsRatemonReqTimeout_Fixture = {
  .setup = RtemsRatemonReqTimeout_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsRatemonReqTimeout_Teardown_Wrap,
  .scope = RtemsRatemonReqTimeout_Scope,
  .initial_context = &RtemsRatemonReqTimeout_Instance
};

static inline RtemsRatemonReqTimeout_Entry RtemsRatemonReqTimeout_PopEntry(
  RtemsRatemonReqTimeout_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsRatemonReqTimeout_Entries[
    RtemsRatemonReqTimeout_Map[ index ]
  ];
}

static void RtemsRatemonReqTimeout_SetPreConditionStates(
  RtemsRatemonReqTimeout_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_WaitState_NA ) {
    ctx->Map.pcs[ 1 ] = RtemsRatemonReqTimeout_Pre_WaitState_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
}

static void RtemsRatemonReqTimeout_TestVariant(
  RtemsRatemonReqTimeout_Context *ctx
)
{
  RtemsRatemonReqTimeout_Pre_WaitFor_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsRatemonReqTimeout_Pre_WaitState_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsRatemonReqTimeout_Pre_PostponedJobs_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsRatemonReqTimeout_Action( ctx );
  RtemsRatemonReqTimeout_Post_PostponedJobs_Check(
    ctx,
    ctx->Map.entry.Post_PostponedJobs
  );
  RtemsRatemonReqTimeout_Post_ReleaseJob_Check(
    ctx,
    ctx->Map.entry.Post_ReleaseJob
  );
  RtemsRatemonReqTimeout_Post_Unblock_Check(
    ctx,
    ctx->Map.entry.Post_Unblock
  );
  RtemsRatemonReqTimeout_Post_PeriodState_Check(
    ctx,
    ctx->Map.entry.Post_PeriodState
  );
  RtemsRatemonReqTimeout_Post_Timer_Check( ctx, ctx->Map.entry.Post_Timer );
  RtemsRatemonReqTimeout_Post_Uptime_Check( ctx, ctx->Map.entry.Post_Uptime );
  RtemsRatemonReqTimeout_Post_CPUUsage_Check(
    ctx,
    ctx->Map.entry.Post_CPUUsage
  );
}

/**
 * @fn void T_case_body_RtemsRatemonReqTimeout( void )
 */
T_TEST_CASE_FIXTURE( RtemsRatemonReqTimeout, &RtemsRatemonReqTimeout_Fixture )
{
  RtemsRatemonReqTimeout_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = RtemsRatemonReqTimeout_Pre_WaitFor_PeriodSelf;
    ctx->Map.pci[ 0 ] < RtemsRatemonReqTimeout_Pre_WaitFor_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = RtemsRatemonReqTimeout_Pre_WaitState_Blocked;
      ctx->Map.pci[ 1 ] < RtemsRatemonReqTimeout_Pre_WaitState_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = RtemsRatemonReqTimeout_Pre_PostponedJobs_Zero;
        ctx->Map.pci[ 2 ] < RtemsRatemonReqTimeout_Pre_PostponedJobs_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        ctx->Map.entry = RtemsRatemonReqTimeout_PopEntry( ctx );

        if ( ctx->Map.entry.Skip ) {
          continue;
        }

        RtemsRatemonReqTimeout_SetPreConditionStates( ctx );
        RtemsRatemonReqTimeout_TestVariant( ctx );
        RtemsRatemonReqTimeout_Cleanup( ctx );
      }
    }
  }
}

/** @} */
