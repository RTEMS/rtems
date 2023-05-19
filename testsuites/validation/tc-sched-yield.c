/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreSchedReqYield
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
#include <rtems/score/percpu.h>

#include "tx-support.h"
#include "tx-thread-queue.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreSchedReqYield spec:/score/sched/req/yield
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  ScoreSchedReqYield_Pre_EligibleScheduler_Home,
  ScoreSchedReqYield_Pre_EligibleScheduler_Helping,
  ScoreSchedReqYield_Pre_EligibleScheduler_NA
} ScoreSchedReqYield_Pre_EligibleScheduler;

typedef enum {
  ScoreSchedReqYield_Pre_UsedScheduler_Home,
  ScoreSchedReqYield_Pre_UsedScheduler_Helping,
  ScoreSchedReqYield_Pre_UsedScheduler_NA
} ScoreSchedReqYield_Pre_UsedScheduler;

typedef enum {
  ScoreSchedReqYield_Pre_HomeSchedulerState_Blocked,
  ScoreSchedReqYield_Pre_HomeSchedulerState_Scheduled,
  ScoreSchedReqYield_Pre_HomeSchedulerState_Ready,
  ScoreSchedReqYield_Pre_HomeSchedulerState_NA
} ScoreSchedReqYield_Pre_HomeSchedulerState;

typedef enum {
  ScoreSchedReqYield_Pre_Sticky_Yes,
  ScoreSchedReqYield_Pre_Sticky_No,
  ScoreSchedReqYield_Pre_Sticky_NA
} ScoreSchedReqYield_Pre_Sticky;

typedef enum {
  ScoreSchedReqYield_Pre_Other_Yes,
  ScoreSchedReqYield_Pre_Other_No,
  ScoreSchedReqYield_Pre_Other_NA
} ScoreSchedReqYield_Pre_Other;

typedef enum {
  ScoreSchedReqYield_Post_HomeSchedulerState_Blocked,
  ScoreSchedReqYield_Post_HomeSchedulerState_Scheduled,
  ScoreSchedReqYield_Post_HomeSchedulerState_Ready,
  ScoreSchedReqYield_Post_HomeSchedulerState_Idle,
  ScoreSchedReqYield_Post_HomeSchedulerState_NA
} ScoreSchedReqYield_Post_HomeSchedulerState;

typedef enum {
  ScoreSchedReqYield_Post_AskForHelp_Yes,
  ScoreSchedReqYield_Post_AskForHelp_No,
  ScoreSchedReqYield_Post_AskForHelp_NA
} ScoreSchedReqYield_Post_AskForHelp;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_EligibleScheduler_NA : 1;
  uint16_t Pre_UsedScheduler_NA : 1;
  uint16_t Pre_HomeSchedulerState_NA : 1;
  uint16_t Pre_Sticky_NA : 1;
  uint16_t Pre_Other_NA : 1;
  uint16_t Post_HomeSchedulerState : 3;
  uint16_t Post_AskForHelp : 2;
} ScoreSchedReqYield_Entry;

/**
 * @brief Test context for spec:/score/sched/req/yield test case.
 */
typedef struct {
  /**
   * @brief This member contains the thread queue test context.
   */
  TQContext tq_ctx;

  /**
   * @brief This member contains the identifier of a sticky mutex.
   */
  rtems_id sticky_mutex;

  /**
   * @brief This member contains the processor index after yielding.
   */
  uint32_t cpu_after_yield;

  /**
   * @brief If this member is true, then the runner shall have a helping
   *   scheduler.
   */
  bool has_helping;

  /**
   * @brief If this member is true, then the runner shall use a helping
   *   scheduler.
   */
  bool use_helping;

  /**
   * @brief If this member is true, then the runner shall be ready in its home
   *   scheduler.
   */
  bool ready;

  /**
   * @brief If this member is true, then the runner shall be sticky.
   */
  bool sticky;

  /**
   * @brief If this member is true, then another ready task in the home
   *   scheduler of the runner shall be ready with an equal priority.
   */
  bool other_ready;

  /**
   * @brief If this member is true, then the processor zero was idle before
   *   yielding.
   */
  bool is_idle_before_yield;

  /**
   * @brief If this member is true, then the processor zero was idle after
   *   yielding.
   */
  bool is_idle_after_yield;

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
    ScoreSchedReqYield_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} ScoreSchedReqYield_Context;

static ScoreSchedReqYield_Context
  ScoreSchedReqYield_Instance;

static const char * const ScoreSchedReqYield_PreDesc_EligibleScheduler[] = {
  "Home",
  "Helping",
  "NA"
};

static const char * const ScoreSchedReqYield_PreDesc_UsedScheduler[] = {
  "Home",
  "Helping",
  "NA"
};

static const char * const ScoreSchedReqYield_PreDesc_HomeSchedulerState[] = {
  "Blocked",
  "Scheduled",
  "Ready",
  "NA"
};

static const char * const ScoreSchedReqYield_PreDesc_Sticky[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const ScoreSchedReqYield_PreDesc_Other[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const * const ScoreSchedReqYield_PreDesc[] = {
  ScoreSchedReqYield_PreDesc_EligibleScheduler,
  ScoreSchedReqYield_PreDesc_UsedScheduler,
  ScoreSchedReqYield_PreDesc_HomeSchedulerState,
  ScoreSchedReqYield_PreDesc_Sticky,
  ScoreSchedReqYield_PreDesc_Other,
  NULL
};

#define COUNTER TQ_BLOCKER_A

#define HELPER TQ_BLOCKER_B

#define MOVER TQ_BLOCKER_C

typedef ScoreSchedReqYield_Context Context;

static void MoveToHelping( Context *ctx )
{
  ctx->tq_ctx.busy_wait[ MOVER ] = true;
  TQSend( &ctx->tq_ctx, MOVER, TQ_EVENT_BUSY_WAIT );
  TQWaitForEventsReceived( &ctx->tq_ctx, MOVER );
  T_eq_u32( rtems_scheduler_get_processor(), 1 );
  ctx->tq_ctx.busy_wait[ MOVER ] = false;
  TQWaitForExecutionStop( &ctx->tq_ctx, MOVER );
}

static uint32_t GetCounter( const Context *ctx )
{
  return TQGetWorkerCounter( &ctx->tq_ctx, COUNTER );
}

static void ScoreSchedReqYield_Pre_EligibleScheduler_Prepare(
  ScoreSchedReqYield_Context              *ctx,
  ScoreSchedReqYield_Pre_EligibleScheduler state
)
{
  switch ( state ) {
    case ScoreSchedReqYield_Pre_EligibleScheduler_Home: {
      /*
       * While the only eligible scheduler of the thread is the home scheduler.
       */
      ctx->has_helping = false;
      break;
    }

    case ScoreSchedReqYield_Pre_EligibleScheduler_Helping: {
      /*
       * While the thread has at least one helping scheduler.
       */
      ctx->has_helping = true;
      break;
    }

    case ScoreSchedReqYield_Pre_EligibleScheduler_NA:
      break;
  }
}

static void ScoreSchedReqYield_Pre_UsedScheduler_Prepare(
  ScoreSchedReqYield_Context          *ctx,
  ScoreSchedReqYield_Pre_UsedScheduler state
)
{
  switch ( state ) {
    case ScoreSchedReqYield_Pre_UsedScheduler_Home: {
      /*
       * While the thread is scheduled on the home scheduler.
       */
      ctx->use_helping = false;
      break;
    }

    case ScoreSchedReqYield_Pre_UsedScheduler_Helping: {
      /*
       * While the thread is scheduled on a helping scheduler.
       */
      ctx->use_helping = true;
      break;
    }

    case ScoreSchedReqYield_Pre_UsedScheduler_NA:
      break;
  }
}

static void ScoreSchedReqYield_Pre_HomeSchedulerState_Prepare(
  ScoreSchedReqYield_Context               *ctx,
  ScoreSchedReqYield_Pre_HomeSchedulerState state
)
{
  switch ( state ) {
    case ScoreSchedReqYield_Pre_HomeSchedulerState_Blocked: {
      /*
       * The thread shall be blocked in its home scheduler.
       */
      ctx->ready = false;
      break;
    }

    case ScoreSchedReqYield_Pre_HomeSchedulerState_Scheduled: {
      /*
       * The thread shall be scheduled in its home scheduler.
       */
      ctx->ready = false;
      break;
    }

    case ScoreSchedReqYield_Pre_HomeSchedulerState_Ready: {
      /*
       * The thread shall be ready in its home scheduler.
       */
      ctx->ready = true;
      break;
    }

    case ScoreSchedReqYield_Pre_HomeSchedulerState_NA:
      break;
  }
}

static void ScoreSchedReqYield_Pre_Sticky_Prepare(
  ScoreSchedReqYield_Context   *ctx,
  ScoreSchedReqYield_Pre_Sticky state
)
{
  switch ( state ) {
    case ScoreSchedReqYield_Pre_Sticky_Yes: {
      /*
       * While the thread is sticky.
       */
      ctx->sticky = true;
      break;
    }

    case ScoreSchedReqYield_Pre_Sticky_No: {
      /*
       * While the thread not sticky.
       */
      ctx->sticky = false;
      break;
    }

    case ScoreSchedReqYield_Pre_Sticky_NA:
      break;
  }
}

static void ScoreSchedReqYield_Pre_Other_Prepare(
  ScoreSchedReqYield_Context  *ctx,
  ScoreSchedReqYield_Pre_Other state
)
{
  switch ( state ) {
    case ScoreSchedReqYield_Pre_Other_Yes: {
      /*
       * While at least one ready thread with a priority equal to the priority
       * of the thread exists in the home scheduler of the thread.
       */
      ctx->other_ready = true;
      break;
    }

    case ScoreSchedReqYield_Pre_Other_No: {
      /*
       * While no ready thread with a priority equal to the priority of the
       * thread exists in the home scheduler of the thread.
       */
      ctx->other_ready = false;
      break;
    }

    case ScoreSchedReqYield_Pre_Other_NA:
      break;
  }
}

static void ScoreSchedReqYield_Post_HomeSchedulerState_Check(
  ScoreSchedReqYield_Context                *ctx,
  ScoreSchedReqYield_Post_HomeSchedulerState state
)
{
  switch ( state ) {
    case ScoreSchedReqYield_Post_HomeSchedulerState_Blocked: {
      /*
       * The thread shall be blocked in its home scheduler.
       */
      T_true( ctx->is_idle_after_yield );
      T_eq_u32( ctx->cpu_after_yield, 1 );
      break;
    }

    case ScoreSchedReqYield_Post_HomeSchedulerState_Scheduled: {
      /*
       * The thread shall be scheduled in its home scheduler.
       */
      T_false( ctx->is_idle_before_yield );
      T_false( ctx->is_idle_after_yield );
      T_eq_u32( GetCounter( ctx ), 0 );
      T_eq_u32( ctx->cpu_after_yield, 0 );
      break;
    }

    case ScoreSchedReqYield_Post_HomeSchedulerState_Ready: {
      /*
       * The thread shall be ready in its home scheduler.
       */
      T_eq_u32( GetCounter( ctx ), 1 );
      break;
    }

    case ScoreSchedReqYield_Post_HomeSchedulerState_Idle: {
      /*
       * An idle thread shall execute on behalf of the thread in its home
       * scheduler.
       */
      T_true( ctx->is_idle_before_yield );
      T_true( ctx->is_idle_after_yield );
      T_eq_u32( GetCounter( ctx ), 0 );
      T_eq_u32( ctx->cpu_after_yield, 1 );
      break;
    }

    case ScoreSchedReqYield_Post_HomeSchedulerState_NA:
      break;
  }
}

static void ScoreSchedReqYield_Post_AskForHelp_Check(
  ScoreSchedReqYield_Context        *ctx,
  ScoreSchedReqYield_Post_AskForHelp state
)
{
  size_t                   index;
  const T_scheduler_event *event;

  index = 0;

  switch ( state ) {
    case ScoreSchedReqYield_Post_AskForHelp_Yes: {
      /*
       * The thread shall ask all its eligible scheduler for help.
       */
      event = TQGetNextAskForHelp( &ctx->tq_ctx, &index );
      T_eq_ptr( event->thread, ctx->tq_ctx.runner_tcb );

      event = TQGetNextAskForHelp( &ctx->tq_ctx, &index );
      T_eq_ptr( event->thread, ctx->tq_ctx.runner_tcb );

      event = TQGetNextAskForHelp( &ctx->tq_ctx, &index );
      T_eq_ptr( event, &T_scheduler_event_null );
      break;
    }

    case ScoreSchedReqYield_Post_AskForHelp_No: {
      /*
       * The thread shall not ask for help.
       */
      event = TQGetNextAskForHelp( &ctx->tq_ctx, &index );
      T_eq_ptr( event, &T_scheduler_event_null );
      break;
    }

    case ScoreSchedReqYield_Post_AskForHelp_NA:
      break;
  }
}

static void ScoreSchedReqYield_Setup( ScoreSchedReqYield_Context *ctx )
{
  rtems_status_code sc;

  memset( ctx, 0, sizeof( *ctx ) );
  ctx->tq_ctx.enqueue_prepare = TQEnqueuePrepareDefault;
  ctx->tq_ctx.enqueue_done = TQEnqueueDoneDefault;
  ctx->tq_ctx.enqueue = TQEnqueueClassicSem;
  ctx->tq_ctx.surrender = TQSurrenderClassicSem;
  ctx->tq_ctx.convert_status = TQConvertStatusClassic;
  TQInitialize( &ctx->tq_ctx );

  sc = rtems_semaphore_create(
    rtems_build_name( 'M', 'U', 'T', 'X' ),
    1,
    RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY |
      RTEMS_MULTIPROCESSOR_RESOURCE_SHARING,
    PRIO_NORMAL,
    &ctx->sticky_mutex
  );
  T_rsc_success( sc );

  TQSetPriority( &ctx->tq_ctx, COUNTER, PRIO_NORMAL );

  #if defined(RTEMS_SMP)
  TQSetScheduler( &ctx->tq_ctx, HELPER, SCHEDULER_B_ID, PRIO_NORMAL );
  TQSetPriority( &ctx->tq_ctx, MOVER, PRIO_HIGH );
  #endif
}

static void ScoreSchedReqYield_Setup_Wrap( void *arg )
{
  ScoreSchedReqYield_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  ScoreSchedReqYield_Setup( ctx );
}

static void ScoreSchedReqYield_Teardown( ScoreSchedReqYield_Context *ctx )
{
  TQDestroy( &ctx->tq_ctx );
  DeleteMutex( ctx->sticky_mutex );
}

static void ScoreSchedReqYield_Teardown_Wrap( void *arg )
{
  ScoreSchedReqYield_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  ScoreSchedReqYield_Teardown( ctx );
}

static void ScoreSchedReqYield_Action( ScoreSchedReqYield_Context *ctx )
{
  const Per_CPU_Control *cpu;
  bool                   other_busy;

  if ( ctx->has_helping ) {
    TQMutexObtain( &ctx->tq_ctx, TQ_MUTEX_A );
    TQSendAndWaitForExecutionStop(
      &ctx->tq_ctx,
      HELPER,
      TQ_EVENT_MUTEX_A_OBTAIN
    );
  }

  if ( ctx->use_helping ) {
    MoveToHelping( ctx );
  }

  TQResetCounter( &ctx->tq_ctx );

  if ( ctx->use_helping && ctx->ready ) {
    ctx->tq_ctx.busy_wait[ COUNTER ] = true;
    TQSend( &ctx->tq_ctx, COUNTER, TQ_EVENT_COUNT | TQ_EVENT_BUSY_WAIT );
    other_busy = true;
  } else {
    other_busy = false;
  }

  if ( ctx->sticky ) {
    ObtainMutex( ctx->sticky_mutex );
  }

  if ( ctx->other_ready && !other_busy ) {
    TQSend( &ctx->tq_ctx, COUNTER, TQ_EVENT_COUNT );
  }

  cpu = _Per_CPU_Get_by_index( 0 );
  ctx->is_idle_before_yield = cpu->heir->is_idle;

  TQSchedulerRecordStart( &ctx->tq_ctx );
  Yield();
  TQSchedulerRecordStop( &ctx->tq_ctx );

  #if defined(RTEMS_SMP)
  ctx->tq_ctx.busy_wait[ COUNTER ] = false;

  while ( cpu->heir == ctx->tq_ctx.worker_tcb[ COUNTER ] ) {
    RTEMS_COMPILER_MEMORY_BARRIER();
  }
  #endif

  ctx->is_idle_after_yield = cpu->heir->is_idle;
  ctx->cpu_after_yield = rtems_scheduler_get_processor();

  if ( ctx->sticky ) {
    ReleaseMutex( ctx->sticky_mutex );
  }

  if ( ctx->has_helping ) {
    TQMutexRelease( &ctx->tq_ctx, TQ_MUTEX_A );
    TQSendAndWaitForExecutionStop(
      &ctx->tq_ctx,
      HELPER,
      TQ_EVENT_MUTEX_A_RELEASE
    );
  }
}

static const ScoreSchedReqYield_Entry
ScoreSchedReqYield_Entries[] = {
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, ScoreSchedReqYield_Post_HomeSchedulerState_NA,
    ScoreSchedReqYield_Post_AskForHelp_NA },
#else
  { 1, 0, 0, 0, 0, 0, ScoreSchedReqYield_Post_HomeSchedulerState_NA,
    ScoreSchedReqYield_Post_AskForHelp_NA },
#endif
  { 1, 0, 0, 0, 0, 0, ScoreSchedReqYield_Post_HomeSchedulerState_NA,
    ScoreSchedReqYield_Post_AskForHelp_NA },
  { 1, 0, 0, 0, 0, 0, ScoreSchedReqYield_Post_HomeSchedulerState_NA,
    ScoreSchedReqYield_Post_AskForHelp_NA },
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, ScoreSchedReqYield_Post_HomeSchedulerState_NA,
    ScoreSchedReqYield_Post_AskForHelp_NA },
#else
  { 0, 0, 0, 0, 0, 0, ScoreSchedReqYield_Post_HomeSchedulerState_Ready,
    ScoreSchedReqYield_Post_AskForHelp_No },
#endif
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, ScoreSchedReqYield_Post_HomeSchedulerState_NA,
    ScoreSchedReqYield_Post_AskForHelp_NA },
#else
  { 1, 0, 0, 0, 0, 0, ScoreSchedReqYield_Post_HomeSchedulerState_NA,
    ScoreSchedReqYield_Post_AskForHelp_NA },
#endif
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, ScoreSchedReqYield_Post_HomeSchedulerState_NA,
    ScoreSchedReqYield_Post_AskForHelp_NA },
#else
  { 1, 0, 0, 0, 0, 0, ScoreSchedReqYield_Post_HomeSchedulerState_NA,
    ScoreSchedReqYield_Post_AskForHelp_NA },
#endif
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, ScoreSchedReqYield_Post_HomeSchedulerState_NA,
    ScoreSchedReqYield_Post_AskForHelp_NA },
#else
  { 1, 0, 0, 0, 0, 0, ScoreSchedReqYield_Post_HomeSchedulerState_NA,
    ScoreSchedReqYield_Post_AskForHelp_NA },
#endif
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, ScoreSchedReqYield_Post_HomeSchedulerState_NA,
    ScoreSchedReqYield_Post_AskForHelp_NA },
#else
  { 1, 0, 0, 0, 0, 0, ScoreSchedReqYield_Post_HomeSchedulerState_NA,
    ScoreSchedReqYield_Post_AskForHelp_NA },
#endif
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, ScoreSchedReqYield_Post_HomeSchedulerState_NA,
    ScoreSchedReqYield_Post_AskForHelp_NA },
#else
  { 0, 0, 0, 0, 0, 0, ScoreSchedReqYield_Post_HomeSchedulerState_Scheduled,
    ScoreSchedReqYield_Post_AskForHelp_No },
#endif
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, ScoreSchedReqYield_Post_HomeSchedulerState_NA,
    ScoreSchedReqYield_Post_AskForHelp_NA },
#else
  { 0, 0, 0, 0, 0, 0, ScoreSchedReqYield_Post_HomeSchedulerState_Blocked,
    ScoreSchedReqYield_Post_AskForHelp_No },
#endif
  { 0, 0, 0, 0, 0, 0, ScoreSchedReqYield_Post_HomeSchedulerState_Ready,
    ScoreSchedReqYield_Post_AskForHelp_No },
  { 0, 0, 0, 0, 0, 0, ScoreSchedReqYield_Post_HomeSchedulerState_Scheduled,
    ScoreSchedReqYield_Post_AskForHelp_No },
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, ScoreSchedReqYield_Post_HomeSchedulerState_NA,
    ScoreSchedReqYield_Post_AskForHelp_NA },
#else
  { 0, 0, 0, 0, 0, 0, ScoreSchedReqYield_Post_HomeSchedulerState_Ready,
    ScoreSchedReqYield_Post_AskForHelp_Yes },
#endif
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, ScoreSchedReqYield_Post_HomeSchedulerState_NA,
    ScoreSchedReqYield_Post_AskForHelp_NA },
#else
  { 0, 0, 0, 0, 0, 0, ScoreSchedReqYield_Post_HomeSchedulerState_Ready,
    ScoreSchedReqYield_Post_AskForHelp_Yes },
#endif
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, ScoreSchedReqYield_Post_HomeSchedulerState_NA,
    ScoreSchedReqYield_Post_AskForHelp_NA },
#else
  { 0, 0, 0, 0, 0, 0, ScoreSchedReqYield_Post_HomeSchedulerState_Scheduled,
    ScoreSchedReqYield_Post_AskForHelp_No },
#endif
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, ScoreSchedReqYield_Post_HomeSchedulerState_NA,
    ScoreSchedReqYield_Post_AskForHelp_NA }
#else
  { 0, 0, 0, 0, 0, 0, ScoreSchedReqYield_Post_HomeSchedulerState_Idle,
    ScoreSchedReqYield_Post_AskForHelp_No }
#endif
};

static const uint8_t
ScoreSchedReqYield_Map[] = {
  0, 0, 2, 2, 3, 8, 10, 11, 0, 0, 2, 2, 4, 4, 1, 1, 5, 5, 1, 1, 5, 5, 1, 1, 0,
  0, 6, 6, 12, 8, 13, 14, 0, 0, 6, 6, 4, 4, 9, 9, 3, 15, 7, 7, 3, 3, 7, 7
};

static size_t ScoreSchedReqYield_Scope( void *arg, char *buf, size_t n )
{
  ScoreSchedReqYield_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( ScoreSchedReqYield_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture ScoreSchedReqYield_Fixture = {
  .setup = ScoreSchedReqYield_Setup_Wrap,
  .stop = NULL,
  .teardown = ScoreSchedReqYield_Teardown_Wrap,
  .scope = ScoreSchedReqYield_Scope,
  .initial_context = &ScoreSchedReqYield_Instance
};

static inline ScoreSchedReqYield_Entry ScoreSchedReqYield_PopEntry(
  ScoreSchedReqYield_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return ScoreSchedReqYield_Entries[
    ScoreSchedReqYield_Map[ index ]
  ];
}

static void ScoreSchedReqYield_TestVariant( ScoreSchedReqYield_Context *ctx )
{
  ScoreSchedReqYield_Pre_EligibleScheduler_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  ScoreSchedReqYield_Pre_UsedScheduler_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  ScoreSchedReqYield_Pre_HomeSchedulerState_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  ScoreSchedReqYield_Pre_Sticky_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  ScoreSchedReqYield_Pre_Other_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  ScoreSchedReqYield_Action( ctx );
  ScoreSchedReqYield_Post_HomeSchedulerState_Check(
    ctx,
    ctx->Map.entry.Post_HomeSchedulerState
  );
  ScoreSchedReqYield_Post_AskForHelp_Check(
    ctx,
    ctx->Map.entry.Post_AskForHelp
  );
}

/**
 * @fn void T_case_body_ScoreSchedReqYield( void )
 */
T_TEST_CASE_FIXTURE( ScoreSchedReqYield, &ScoreSchedReqYield_Fixture )
{
  ScoreSchedReqYield_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = ScoreSchedReqYield_Pre_EligibleScheduler_Home;
    ctx->Map.pcs[ 0 ] < ScoreSchedReqYield_Pre_EligibleScheduler_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = ScoreSchedReqYield_Pre_UsedScheduler_Home;
      ctx->Map.pcs[ 1 ] < ScoreSchedReqYield_Pre_UsedScheduler_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = ScoreSchedReqYield_Pre_HomeSchedulerState_Blocked;
        ctx->Map.pcs[ 2 ] < ScoreSchedReqYield_Pre_HomeSchedulerState_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        for (
          ctx->Map.pcs[ 3 ] = ScoreSchedReqYield_Pre_Sticky_Yes;
          ctx->Map.pcs[ 3 ] < ScoreSchedReqYield_Pre_Sticky_NA;
          ++ctx->Map.pcs[ 3 ]
        ) {
          for (
            ctx->Map.pcs[ 4 ] = ScoreSchedReqYield_Pre_Other_Yes;
            ctx->Map.pcs[ 4 ] < ScoreSchedReqYield_Pre_Other_NA;
            ++ctx->Map.pcs[ 4 ]
          ) {
            ctx->Map.entry = ScoreSchedReqYield_PopEntry( ctx );

            if ( ctx->Map.entry.Skip ) {
              continue;
            }

            ScoreSchedReqYield_TestVariant( ctx );
          }
        }
      }
    }
  }
}

/** @} */
