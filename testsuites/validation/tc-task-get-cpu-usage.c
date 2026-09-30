/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsTaskReqGetCpuUsage
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
#include <rtems/cpuuse.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/timestampimpl.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsTaskReqGetCpuUsage spec:/rtems/task/req/get-cpu-usage
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsTaskReqGetCpuUsage_Pre_CPUs_One,
  RtemsTaskReqGetCpuUsage_Pre_CPUs_More,
  RtemsTaskReqGetCpuUsage_Pre_CPUs_NA
} RtemsTaskReqGetCpuUsage_Pre_CPUs;

typedef enum {
  RtemsTaskReqGetCpuUsage_Pre_State_Dormant,
  RtemsTaskReqGetCpuUsage_Pre_State_Ready,
  RtemsTaskReqGetCpuUsage_Pre_State_Blocked,
  RtemsTaskReqGetCpuUsage_Pre_State_Executing,
  RtemsTaskReqGetCpuUsage_Pre_State_NA
} RtemsTaskReqGetCpuUsage_Pre_State;

typedef enum {
  RtemsTaskReqGetCpuUsage_Pre_Reset_Yes,
  RtemsTaskReqGetCpuUsage_Pre_Reset_No,
  RtemsTaskReqGetCpuUsage_Pre_Reset_NA
} RtemsTaskReqGetCpuUsage_Pre_Reset;

typedef enum {
  RtemsTaskReqGetCpuUsage_Pre_Id_Invalid,
  RtemsTaskReqGetCpuUsage_Pre_Id_Object,
  RtemsTaskReqGetCpuUsage_Pre_Id_Self,
  RtemsTaskReqGetCpuUsage_Pre_Id_Caller,
  RtemsTaskReqGetCpuUsage_Pre_Id_Task,
  RtemsTaskReqGetCpuUsage_Pre_Id_NA
} RtemsTaskReqGetCpuUsage_Pre_Id;

typedef enum {
  RtemsTaskReqGetCpuUsage_Pre_Ts_Valid,
  RtemsTaskReqGetCpuUsage_Pre_Ts_Null,
  RtemsTaskReqGetCpuUsage_Pre_Ts_NA
} RtemsTaskReqGetCpuUsage_Pre_Ts;

typedef enum {
  RtemsTaskReqGetCpuUsage_Post_Status_Ok,
  RtemsTaskReqGetCpuUsage_Post_Status_InvAddr,
  RtemsTaskReqGetCpuUsage_Post_Status_InvId,
  RtemsTaskReqGetCpuUsage_Post_Status_NA
} RtemsTaskReqGetCpuUsage_Post_Status;

typedef enum {
  RtemsTaskReqGetCpuUsage_Post_Time_Set,
  RtemsTaskReqGetCpuUsage_Post_Time_Nop,
  RtemsTaskReqGetCpuUsage_Post_Time_NA
} RtemsTaskReqGetCpuUsage_Post_Time;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_CPUs_NA : 1;
  uint16_t Pre_State_NA : 1;
  uint16_t Pre_Reset_NA : 1;
  uint16_t Pre_Id_NA : 1;
  uint16_t Pre_Ts_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_Time : 2;
} RtemsTaskReqGetCpuUsage_Entry;

/**
 * @brief Test context for spec:/rtems/task/req/get-cpu-usage test case.
 */
typedef struct {
  /**
   * @brief This member contains the identifier of a mutex.
   */
  rtems_id mutex_id;

  /**
   * @brief This member contains the identifier of the task which the test
   *   arranges.
   */
  rtems_id task_id;

  /**
   * @brief This member contains the identifier of the worker task, or
   *   RTEMS_ID_NONE.
   */
  rtems_id worker_id;

  /**
   * @brief This member is set, if the busy worker task executes.
   */
  Atomic_Uint worker_busy;

  /**
   * @brief This member is true, if the task is an executing task.
   */
  bool executing;

  /**
   * @brief This member references the thread of the task specified by the `id`
   *   parameter, or is NULL.
   */
  Thread_Control *thread;

  /**
   * @brief This member contains the processor time of the task immediately
   *   before the rtems_task_get_cpu_usage() call.
   */
  Timestamp_Control used_before;

  /**
   * @brief This member contains the processor time of the task immediately
   *   after the rtems_task_get_cpu_usage() call.
   */
  Timestamp_Control used_after;

  /**
   * @brief This member provides the object referenced by the `ts` parameter.
   */
  struct timespec ts_obj;

  /**
   * @brief This member contains the return value of the
   *   rtems_task_get_cpu_usage() call.
   */
  rtems_status_code status;

  /**
   * @brief This member specifies the `id` parameter value.
   */
  rtems_id id;

  /**
   * @brief This member specifies the `ts` parameter value.
   */
  struct timespec *ts;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 5 ];

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
    RtemsTaskReqGetCpuUsage_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsTaskReqGetCpuUsage_Context;

static RtemsTaskReqGetCpuUsage_Context RtemsTaskReqGetCpuUsage_Instance;

static const char *const RtemsTaskReqGetCpuUsage_PreDesc_CPUs[] =
  { "One", "More", "NA" };

static const char *const RtemsTaskReqGetCpuUsage_PreDesc_State[] =
  { "Dormant", "Ready", "Blocked", "Executing", "NA" };

static const char *const RtemsTaskReqGetCpuUsage_PreDesc_Reset[] =
  { "Yes", "No", "NA" };

static const char *const RtemsTaskReqGetCpuUsage_PreDesc_Id[] =
  { "Invalid", "Object", "Self", "Caller", "Task", "NA" };

static const char *const RtemsTaskReqGetCpuUsage_PreDesc_Ts[] =
  { "Valid", "Null", "NA" };

static const char *const *const RtemsTaskReqGetCpuUsage_PreDesc[] = {
  RtemsTaskReqGetCpuUsage_PreDesc_CPUs,
  RtemsTaskReqGetCpuUsage_PreDesc_State,
  RtemsTaskReqGetCpuUsage_PreDesc_Reset,
  RtemsTaskReqGetCpuUsage_PreDesc_Id,
  RtemsTaskReqGetCpuUsage_PreDesc_Ts,
  NULL
};

typedef RtemsTaskReqGetCpuUsage_Context Context;

static int64_t GetNanoseconds( const struct timespec *ts )
{
  return (int64_t) ts->tv_sec * 1000000000 + ts->tv_nsec;
}

static void WorkerReady( rtems_task_argument arg )
{
  (void) arg;
  (void) SetSelfPriority( PRIO_LOW );
  SuspendSelf();
}

static void WorkerBlocked( rtems_task_argument arg )
{
  (void) arg;
  SuspendSelf();
}

static void WorkerBusy( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;
  SetFlag( &ctx->worker_busy, 1 );
  (void) _CPU_Thread_Idle_body( 0 );
}

static void RtemsTaskReqGetCpuUsage_Pre_CPUs_Prepare(
  RtemsTaskReqGetCpuUsage_Context *ctx,
  RtemsTaskReqGetCpuUsage_Pre_CPUs state
)
{
  switch ( state ) {
    case RtemsTaskReqGetCpuUsage_Pre_CPUs_One: {
      /*
       * Where the system has exactly one processor.
       */
      if ( rtems_scheduler_get_processor_maximum() != 1 ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case RtemsTaskReqGetCpuUsage_Pre_CPUs_More: {
      /*
       * Where the system has more than one processor.
       */
      if ( rtems_scheduler_get_processor_maximum() == 1 ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case RtemsTaskReqGetCpuUsage_Pre_CPUs_NA:
      break;
  }
}

static void RtemsTaskReqGetCpuUsage_Pre_State_Prepare(
  RtemsTaskReqGetCpuUsage_Context  *ctx,
  RtemsTaskReqGetCpuUsage_Pre_State state
)
{
  switch ( state ) {
    case RtemsTaskReqGetCpuUsage_Pre_State_Dormant: {
      /*
       * While the task is dormant.
       */
      ctx->task_id = CreateTask( "WORK", PRIO_HIGH );
      ctx->worker_id = ctx->task_id;
      break;
    }

    case RtemsTaskReqGetCpuUsage_Pre_State_Ready: {
      /*
       * While the task executed at least once, while the task is a ready task.
       */
      ctx->task_id = CreateTask( "WORK", PRIO_HIGH );
      ctx->worker_id = ctx->task_id;
      StartTask( ctx->task_id, WorkerReady, ctx );
      break;
    }

    case RtemsTaskReqGetCpuUsage_Pre_State_Blocked: {
      /*
       * While the task executed at least once, while the task is a blocked
       * task.
       */
      ctx->task_id = CreateTask( "WORK", PRIO_HIGH );
      ctx->worker_id = ctx->task_id;
      StartTask( ctx->task_id, WorkerBlocked, ctx );
      break;
    }

    case RtemsTaskReqGetCpuUsage_Pre_State_Executing: {
      /*
       * While the task is an executing task.
       */
      ctx->executing = true;

      if ( rtems_scheduler_get_processor_maximum() > 1 ) {
        ctx->task_id = CreateTask( "WORK", PRIO_HIGH );
        ctx->worker_id = ctx->task_id;
        SetScheduler( ctx->task_id, SCHEDULER_B_ID, PRIO_NORMAL );
        StartTask( ctx->task_id, WorkerBusy, ctx );
        T_true( WaitForFlag( &ctx->worker_busy ) );
      }
      break;
    }

    case RtemsTaskReqGetCpuUsage_Pre_State_NA:
      break;
  }
}

static void RtemsTaskReqGetCpuUsage_Pre_Reset_Prepare(
  RtemsTaskReqGetCpuUsage_Pre_Reset state
)
{
  switch ( state ) {
    case RtemsTaskReqGetCpuUsage_Pre_Reset_Yes: {
      /*
       * While rtems_cpu_usage_reset() was called after the task used processor
       * time.
       */
      rtems_cpu_usage_reset();
      break;
    }

    case RtemsTaskReqGetCpuUsage_Pre_Reset_No: {
      /*
       * While rtems_cpu_usage_reset() was not called after the task used
       * processor time.
       */
      /* Nothing to do */
      break;
    }

    case RtemsTaskReqGetCpuUsage_Pre_Reset_NA:
      break;
  }
}

static void RtemsTaskReqGetCpuUsage_Pre_Id_Prepare(
  RtemsTaskReqGetCpuUsage_Context *ctx,
  RtemsTaskReqGetCpuUsage_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsTaskReqGetCpuUsage_Pre_Id_Invalid: {
      /*
       * While the `id` parameter is not associated with an object.
       */
      ctx->id = INVALID_ID;
      ctx->thread = NULL;
      break;
    }

    case RtemsTaskReqGetCpuUsage_Pre_Id_Object: {
      /*
       * While the `id` parameter is associated with an object which is not a
       * task.
       */
      ctx->id = ctx->mutex_id;
      ctx->thread = NULL;
      break;
    }

    case RtemsTaskReqGetCpuUsage_Pre_Id_Self: {
      /*
       * While the `id` parameter is equal to RTEMS_SELF.
       */
      ctx->id = RTEMS_SELF;
      ctx->thread = GetThread( RTEMS_SELF );
      break;
    }

    case RtemsTaskReqGetCpuUsage_Pre_Id_Caller: {
      /*
       * While the `id` parameter is the identifier of the calling task.
       */
      ctx->id = rtems_task_self();
      ctx->thread = GetThread( RTEMS_SELF );
      break;
    }

    case RtemsTaskReqGetCpuUsage_Pre_Id_Task: {
      /*
       * While the `id` parameter is the identifier of the task, while the task
       * is not the calling task.
       */
      ctx->id = ctx->task_id;
      ctx->thread = GetThread( ctx->task_id );
      break;
    }

    case RtemsTaskReqGetCpuUsage_Pre_Id_NA:
      break;
  }
}

static void RtemsTaskReqGetCpuUsage_Pre_Ts_Prepare(
  RtemsTaskReqGetCpuUsage_Context *ctx,
  RtemsTaskReqGetCpuUsage_Pre_Ts   state
)
{
  switch ( state ) {
    case RtemsTaskReqGetCpuUsage_Pre_Ts_Valid: {
      /*
       * While the `ts` parameter references an object of type struct timespec.
       */
      ctx->ts = &ctx->ts_obj;
      break;
    }

    case RtemsTaskReqGetCpuUsage_Pre_Ts_Null: {
      /*
       * While the `ts` parameter is equal to NULL.
       */
      ctx->ts = NULL;
      break;
    }

    case RtemsTaskReqGetCpuUsage_Pre_Ts_NA:
      break;
  }
}

static void RtemsTaskReqGetCpuUsage_Post_Status_Check(
  RtemsTaskReqGetCpuUsage_Context    *ctx,
  RtemsTaskReqGetCpuUsage_Post_Status state
)
{
  switch ( state ) {
    case RtemsTaskReqGetCpuUsage_Post_Status_Ok: {
      /*
       * The return status of rtems_task_get_cpu_usage() shall be
       * RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsTaskReqGetCpuUsage_Post_Status_InvAddr: {
      /*
       * The return status of rtems_task_get_cpu_usage() shall be
       * RTEMS_INVALID_ADDRESS.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ADDRESS );
      break;
    }

    case RtemsTaskReqGetCpuUsage_Post_Status_InvId: {
      /*
       * The return status of rtems_task_get_cpu_usage() shall be
       * RTEMS_INVALID_ID.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ID );
      break;
    }

    case RtemsTaskReqGetCpuUsage_Post_Status_NA:
      break;
  }
}

static void RtemsTaskReqGetCpuUsage_Post_Time_Check(
  RtemsTaskReqGetCpuUsage_Context  *ctx,
  RtemsTaskReqGetCpuUsage_Post_Time state
)
{
  struct timespec before;
  struct timespec after;

  switch ( state ) {
    case RtemsTaskReqGetCpuUsage_Post_Time_Set: {
      /*
       * The value of the object referenced by the `ts` parameter shall be set
       * to the processor time which the task specified by the `id` parameter
       * used throughout its lifetime up to some time point during the
       * rtems_task_get_cpu_usage() call.
       */
      _Timestamp_To_timespec( &ctx->used_before, &before );
      _Timestamp_To_timespec( &ctx->used_after, &after );
      if ( ctx->executing ) {
        T_gt_i64( GetNanoseconds( &ctx->ts_obj ), GetNanoseconds( &before ) );
      } else {
        T_ge_i64( GetNanoseconds( &ctx->ts_obj ), GetNanoseconds( &before ) );
      }

      T_le_i64( GetNanoseconds( &ctx->ts_obj ), GetNanoseconds( &after ) );
      break;
    }

    case RtemsTaskReqGetCpuUsage_Post_Time_Nop: {
      /*
       * The object referenced by the `ts` parameter shall not be modified by
       * the rtems_task_get_cpu_usage() call.
       */
      T_eq_i64( (int64_t) ctx->ts_obj.tv_sec, -1 );
      T_eq_long( ctx->ts_obj.tv_nsec, -1 );
      break;
    }

    case RtemsTaskReqGetCpuUsage_Post_Time_NA:
      break;
  }
}

static void RtemsTaskReqGetCpuUsage_Setup(
  RtemsTaskReqGetCpuUsage_Context *ctx
)
{
  SetSelfPriority( PRIO_NORMAL );
  ctx->mutex_id = CreateMutex();
}

static void RtemsTaskReqGetCpuUsage_Setup_Wrap( void *arg )
{
  RtemsTaskReqGetCpuUsage_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsTaskReqGetCpuUsage_Setup( ctx );
}

static void RtemsTaskReqGetCpuUsage_Teardown(
  RtemsTaskReqGetCpuUsage_Context *ctx
)
{
  DeleteMutex( ctx->mutex_id );
  RestoreRunnerPriority();
}

static void RtemsTaskReqGetCpuUsage_Teardown_Wrap( void *arg )
{
  RtemsTaskReqGetCpuUsage_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsTaskReqGetCpuUsage_Teardown( ctx );
}

static void RtemsTaskReqGetCpuUsage_Prepare(
  RtemsTaskReqGetCpuUsage_Context *ctx
)
{
  ctx->worker_id = RTEMS_ID_NONE;
  SetFlag( &ctx->worker_busy, 0 );
  ctx->executing = false;
  ctx->ts_obj.tv_sec = -1;
  ctx->ts_obj.tv_nsec = -1;
}

static void RtemsTaskReqGetCpuUsage_Action(
  RtemsTaskReqGetCpuUsage_Context *ctx
)
{
  if ( ctx->thread != NULL ) {
    ctx->used_before = _Thread_Get_CPU_time_used( ctx->thread );
  }

  ctx->status = rtems_task_get_cpu_usage( ctx->id, ctx->ts );

  if ( ctx->thread != NULL ) {
    ctx->used_after = _Thread_Get_CPU_time_used( ctx->thread );
  }
}

static void RtemsTaskReqGetCpuUsage_Cleanup(
  RtemsTaskReqGetCpuUsage_Context *ctx
)
{
  if ( ctx->worker_id != RTEMS_ID_NONE ) {
    DeleteTask( ctx->worker_id );
  }
}

/* clang-format off */

static const RtemsTaskReqGetCpuUsage_Entry
RtemsTaskReqGetCpuUsage_Entries[] = {
  { 0, 0, 0, 0, 0, 0, RtemsTaskReqGetCpuUsage_Post_Status_InvAddr,
    RtemsTaskReqGetCpuUsage_Post_Time_Nop },
  { 1, 0, 0, 0, 0, 0, RtemsTaskReqGetCpuUsage_Post_Status_NA,
    RtemsTaskReqGetCpuUsage_Post_Time_NA },
  { 0, 0, 0, 0, 0, 0, RtemsTaskReqGetCpuUsage_Post_Status_InvId,
    RtemsTaskReqGetCpuUsage_Post_Time_Nop },
  { 0, 0, 0, 0, 0, 0, RtemsTaskReqGetCpuUsage_Post_Status_Ok,
    RtemsTaskReqGetCpuUsage_Post_Time_Set },
  { 1, 0, 0, 0, 0, 0, RtemsTaskReqGetCpuUsage_Post_Status_NA,
    RtemsTaskReqGetCpuUsage_Post_Time_NA }
};

static const uint8_t
RtemsTaskReqGetCpuUsage_Map[] = {
  2, 0, 2, 0, 1, 1, 1, 1, 3, 0, 2, 0, 2, 0, 1, 1, 1, 1, 3, 0, 2, 0, 2, 0, 1, 1,
  1, 1, 3, 0, 2, 0, 2, 0, 1, 1, 1, 1, 3, 0, 2, 0, 2, 0, 1, 1, 1, 1, 3, 0, 2, 0,
  2, 0, 1, 1, 1, 1, 3, 0, 2, 0, 2, 0, 3, 0, 3, 0, 4, 4, 2, 0, 2, 0, 3, 0, 3, 0,
  4, 4, 2, 0, 2, 0, 1, 1, 1, 1, 3, 0, 2, 0, 2, 0, 1, 1, 1, 1, 3, 0, 2, 0, 2, 0,
  1, 1, 1, 1, 3, 0, 2, 0, 2, 0, 1, 1, 1, 1, 3, 0, 2, 0, 2, 0, 1, 1, 1, 1, 3, 0,
  2, 0, 2, 0, 1, 1, 1, 1, 3, 0, 2, 0, 2, 0, 3, 0, 3, 0, 3, 0, 2, 0, 2, 0, 3, 0,
  3, 0, 3, 0
};

/* clang-format on */

static size_t RtemsTaskReqGetCpuUsage_Scope( void *arg, char *buf, size_t n )
{
  RtemsTaskReqGetCpuUsage_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      RtemsTaskReqGetCpuUsage_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture RtemsTaskReqGetCpuUsage_Fixture = {
  .setup = RtemsTaskReqGetCpuUsage_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsTaskReqGetCpuUsage_Teardown_Wrap,
  .scope = RtemsTaskReqGetCpuUsage_Scope,
  .initial_context = &RtemsTaskReqGetCpuUsage_Instance
};

static const uint8_t RtemsTaskReqGetCpuUsage_Weights[] = { 80, 20, 10, 2, 1 };

static void RtemsTaskReqGetCpuUsage_Skip(
  RtemsTaskReqGetCpuUsage_Context *ctx,
  size_t                           index
)
{
  switch ( index + 1 ) {
    case 1:
      ctx->Map.pcs[ 1 ] = RtemsTaskReqGetCpuUsage_Pre_State_NA - 1;
      /* Fall through */
    case 2:
      ctx->Map.pcs[ 2 ] = RtemsTaskReqGetCpuUsage_Pre_Reset_NA - 1;
      /* Fall through */
    case 3:
      ctx->Map.pcs[ 3 ] = RtemsTaskReqGetCpuUsage_Pre_Id_NA - 1;
      /* Fall through */
    case 4:
      ctx->Map.pcs[ 4 ] = RtemsTaskReqGetCpuUsage_Pre_Ts_NA - 1;
      break;
  }
}

static inline RtemsTaskReqGetCpuUsage_Entry RtemsTaskReqGetCpuUsage_PopEntry(
  RtemsTaskReqGetCpuUsage_Context *ctx
)
{
  size_t index;

  if ( ctx->Map.skip ) {
    size_t i;

    ctx->Map.skip = false;
    index = 0;

    for ( i = 0; i < 5; ++i ) {
      index += RtemsTaskReqGetCpuUsage_Weights[ i ] * ctx->Map.pcs[ i ];
    }
  } else {
    index = ctx->Map.index;
  }

  ctx->Map.index = index + 1;

  return RtemsTaskReqGetCpuUsage_Entries
    [ RtemsTaskReqGetCpuUsage_Map[ index ] ];
}

static void RtemsTaskReqGetCpuUsage_TestVariant(
  RtemsTaskReqGetCpuUsage_Context *ctx
)
{
  RtemsTaskReqGetCpuUsage_Pre_CPUs_Prepare( ctx, ctx->Map.pcs[ 0 ] );

  if ( ctx->Map.skip ) {
    RtemsTaskReqGetCpuUsage_Skip( ctx, 0 );
    return;
  }

  RtemsTaskReqGetCpuUsage_Pre_State_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsTaskReqGetCpuUsage_Pre_Reset_Prepare( ctx->Map.pcs[ 2 ] );
  RtemsTaskReqGetCpuUsage_Pre_Id_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  RtemsTaskReqGetCpuUsage_Pre_Ts_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  RtemsTaskReqGetCpuUsage_Action( ctx );
  RtemsTaskReqGetCpuUsage_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  RtemsTaskReqGetCpuUsage_Post_Time_Check( ctx, ctx->Map.entry.Post_Time );
}

/**
 * @fn void T_case_body_RtemsTaskReqGetCpuUsage( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsTaskReqGetCpuUsage,
  &RtemsTaskReqGetCpuUsage_Fixture
)
{
  RtemsTaskReqGetCpuUsage_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;
  ctx->Map.skip = false;

  for (
    ctx->Map.pcs[ 0 ] = RtemsTaskReqGetCpuUsage_Pre_CPUs_One;
    ctx->Map.pcs[ 0 ] < RtemsTaskReqGetCpuUsage_Pre_CPUs_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = RtemsTaskReqGetCpuUsage_Pre_State_Dormant;
      ctx->Map.pcs[ 1 ] < RtemsTaskReqGetCpuUsage_Pre_State_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = RtemsTaskReqGetCpuUsage_Pre_Reset_Yes;
        ctx->Map.pcs[ 2 ] < RtemsTaskReqGetCpuUsage_Pre_Reset_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        for (
          ctx->Map.pcs[ 3 ] = RtemsTaskReqGetCpuUsage_Pre_Id_Invalid;
          ctx->Map.pcs[ 3 ] < RtemsTaskReqGetCpuUsage_Pre_Id_NA;
          ++ctx->Map.pcs[ 3 ]
        ) {
          for (
            ctx->Map.pcs[ 4 ] = RtemsTaskReqGetCpuUsage_Pre_Ts_Valid;
            ctx->Map.pcs[ 4 ] < RtemsTaskReqGetCpuUsage_Pre_Ts_NA;
            ++ctx->Map.pcs[ 4 ]
          ) {
            ctx->Map.entry = RtemsTaskReqGetCpuUsage_PopEntry( ctx );

            if ( ctx->Map.entry.Skip ) {
              continue;
            }

            RtemsTaskReqGetCpuUsage_Prepare( ctx );
            RtemsTaskReqGetCpuUsage_TestVariant( ctx );
            RtemsTaskReqGetCpuUsage_Cleanup( ctx );
          }
        }
      }
    }
  }
}

/** @} */
