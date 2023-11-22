/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreTqReqEnqueuePriority
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

#include "tr-tq-enqueue-priority.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreTqReqEnqueuePriority spec:/score/tq/req/enqueue-priority
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_EligibleScheduler_NA : 1;
  uint8_t Pre_QueueEligible_NA : 1;
  uint8_t Pre_QueueIneligible_NA : 1;
  uint8_t Post_Position : 4;
} ScoreTqReqEnqueuePriority_Entry;

/**
 * @brief Test context for spec:/score/tq/req/enqueue-priority test case.
 */
typedef struct {
  /**
   * @brief This this member is true, then the enqueueing thread shall have at
   *   least one helping scheduler.
   */
  bool helping;

  /**
   * @brief This member specifies the priority of a thread with an eligible
   *   scheduler equal to an eligible scheduler of the enqueueing thread.
   */
  rtems_task_priority priority;

  /**
   * @brief If this member is true, then a thread those eligible schedulers are
   *   ineligible scheduler to the enqueueing task should be enqueued before a
   *   thread with an eligible scheduler equal to an eligible scheduler of the
   *   enqueueing thread.
   */
  size_t other_before;

  /**
   * @brief If this member is true, then a thread those eligible schedulers are
   *   ineligible scheduler to the enqueueing task should be enqueued after a
   *   thread with an eligible scheduler equal to an eligible scheduler of the
   *   enqueueing thread.
   */
  size_t other_after;

  /**
   * @brief This member contains a copy of the corresponding
   *   ScoreTqReqEnqueuePriority_Run() parameter.
   */
  TQContext *tq_ctx;

  struct {
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
    ScoreTqReqEnqueuePriority_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} ScoreTqReqEnqueuePriority_Context;

static ScoreTqReqEnqueuePriority_Context
  ScoreTqReqEnqueuePriority_Instance;

static const char * const ScoreTqReqEnqueuePriority_PreDesc_EligibleScheduler[] = {
  "Home",
  "Helping",
  "NA"
};

static const char * const ScoreTqReqEnqueuePriority_PreDesc_QueueEligible[] = {
  "None",
  "High",
  "Equal",
  "Low",
  "NA"
};

static const char * const ScoreTqReqEnqueuePriority_PreDesc_QueueIneligible[] = {
  "None",
  "Only",
  "Before",
  "After",
  "NA"
};

static const char * const * const ScoreTqReqEnqueuePriority_PreDesc[] = {
  ScoreTqReqEnqueuePriority_PreDesc_EligibleScheduler,
  ScoreTqReqEnqueuePriority_PreDesc_QueueEligible,
  ScoreTqReqEnqueuePriority_PreDesc_QueueIneligible,
  NULL
};

typedef ScoreTqReqEnqueuePriority_Context Context;

static const rtems_tcb *GetUnblock( Context *ctx, size_t *index )
{
  const rtems_tcb *thread;

  do {
    thread = TQGetNextUnblock( ctx->tq_ctx, index )->thread;
  } while ( thread == ctx->tq_ctx->runner_tcb );

  return thread;
}

static const rtems_tcb *GetTCB( Context *ctx, TQWorkerKind worker )
{
  return ctx->tq_ctx->worker_tcb[ worker ];
}

static void AddHelper( TQContext *tq_ctx, rtems_id scheduler_id )
{
  TQSend( tq_ctx, TQ_BLOCKER_C, TQ_EVENT_MUTEX_A_OBTAIN );
  TQSetScheduler( tq_ctx, TQ_BLOCKER_E, scheduler_id, PRIO_LOW );
  TQSendAndWaitForExecutionStop(
    tq_ctx,
    TQ_BLOCKER_E,
    TQ_EVENT_MUTEX_A_OBTAIN | TQ_EVENT_MUTEX_A_RELEASE
  );
}

static void RemoveHelper( TQContext *tq_ctx )
{
  TQSend( tq_ctx, TQ_BLOCKER_C, TQ_EVENT_MUTEX_A_RELEASE );
  TQMutexObtain( tq_ctx, TQ_MUTEX_A );
  TQMutexRelease( tq_ctx, TQ_MUTEX_A );
}

static void ScoreTqReqEnqueuePriority_Pre_EligibleScheduler_Prepare(
  ScoreTqReqEnqueuePriority_Context              *ctx,
  ScoreTqReqEnqueuePriority_Pre_EligibleScheduler state
)
{
  switch ( state ) {
    case ScoreTqReqEnqueuePriority_Pre_EligibleScheduler_Home: {
      /*
       * While the enqueueing thread has no helping scheduler.
       */
      ctx->helping = false;
      break;
    }

    case ScoreTqReqEnqueuePriority_Pre_EligibleScheduler_Helping: {
      /*
       * While the enqueueing thread has at least one helping scheduler.
       */
      ctx->helping = true;
      break;
    }

    case ScoreTqReqEnqueuePriority_Pre_EligibleScheduler_NA:
      break;
  }
}

static void ScoreTqReqEnqueuePriority_Pre_QueueEligible_Prepare(
  ScoreTqReqEnqueuePriority_Context          *ctx,
  ScoreTqReqEnqueuePriority_Pre_QueueEligible state
)
{
  switch ( state ) {
    case ScoreTqReqEnqueuePriority_Pre_QueueEligible_None: {
      /*
       * While all priority queues of the thread queue associated with eligible
       * schedulers of the enqueueing thread are empty.
       */
      ctx->priority = PRIO_PSEUDO_ISR;
      break;
    }

    case ScoreTqReqEnqueuePriority_Pre_QueueEligible_High: {
      /*
       * While a priority queue of the thread queue associated with an eligible
       * scheduler of the enqueueing thread is non-empty, while the highest
       * priority of the priority queue is higher than the priority of the
       * enqueueing thread with respect to the eligible scheduler.
       */
      ++ctx->tq_ctx->how_many;
      ctx->priority = PRIO_ULTRA_HIGH;
      break;
    }

    case ScoreTqReqEnqueuePriority_Pre_QueueEligible_Equal: {
      /*
       * While a priority queue of the thread queue associated with an eligible
       * scheduler of the enqueueing thread is non-empty, while the highest
       * priority of the priority queue is equal to the priority of the
       * enqueueing thread with respect to the eligible scheduler.
       */
      ++ctx->tq_ctx->how_many;
      ctx->priority = PRIO_VERY_HIGH;
      break;
    }

    case ScoreTqReqEnqueuePriority_Pre_QueueEligible_Low: {
      /*
       * While a priority queue of the thread queue associated with an eligible
       * scheduler of the enqueueing thread is non-empty, while the highest
       * priority of the priority queue is lower than the priority of the
       * enqueueing thread with respect to the eligible scheduler.
       */
      ++ctx->tq_ctx->how_many;
      ctx->priority = PRIO_HIGH;
      break;
    }

    case ScoreTqReqEnqueuePriority_Pre_QueueEligible_NA:
      break;
  }
}

static void ScoreTqReqEnqueuePriority_Pre_QueueIneligible_Prepare(
  ScoreTqReqEnqueuePriority_Context            *ctx,
  ScoreTqReqEnqueuePriority_Pre_QueueIneligible state
)
{
  switch ( state ) {
    case ScoreTqReqEnqueuePriority_Pre_QueueIneligible_None: {
      /*
       * While no priority queue of the thread queue exists which is not
       * associated with an eligible scheduler of the enqueueing thread.
       */
      ctx->other_before = false;
      ctx->other_after = false;
      break;
    }

    case ScoreTqReqEnqueuePriority_Pre_QueueIneligible_Only: {
      /*
       * While exactly one priority queue of the thread queue exists which is
       * not associated with an eligible scheduler of the enqueueing thread.
       */
      ++ctx->tq_ctx->how_many;
      ctx->other_before = true;
      ctx->other_after = false;
      break;
    }

    case ScoreTqReqEnqueuePriority_Pre_QueueIneligible_Before: {
      /*
       * While a priority queue of the thread queue exists which is not
       * associated with an eligible scheduler of the enqueueing thread, while
       * this priority queue is positioned before all priority queues which are
       * associated with eligible schedulers of the enqueueing thread.
       */
      ++ctx->tq_ctx->how_many;
      ctx->other_before = true;
      ctx->other_after = false;
      break;
    }

    case ScoreTqReqEnqueuePriority_Pre_QueueIneligible_After: {
      /*
       * While a priority queue of the thread queue exists which is not
       * associated with an eligible scheduler of the enqueueing thread, while
       * this priority queue is positioned after all priority queues which are
       * associated with eligible schedulers of the enqueueing thread.
       */
      ++ctx->tq_ctx->how_many;
      ctx->other_before = false;
      ctx->other_after = true;
      break;
    }

    case ScoreTqReqEnqueuePriority_Pre_QueueIneligible_NA:
      break;
  }
}

static void ScoreTqReqEnqueuePriority_Post_Position_Check(
  ScoreTqReqEnqueuePriority_Context      *ctx,
  ScoreTqReqEnqueuePriority_Post_Position state
)
{
  size_t i;

  i = 0;

  /* Event receives */
  T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_C ) );
  T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_A ) );

  switch ( state ) {
    case ScoreTqReqEnqueuePriority_Post_Position_InitialFirst: {
      /*
       * A priority queue associated with the scheduler which contains exactly
       * the enqueueing thread shall be created as the first priority queue of
       * the thread queue.
       */
      T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_C ) );
      T_eq_ptr( GetUnblock( ctx, &i ), NULL );
      break;
    }

    case ScoreTqReqEnqueuePriority_Post_Position_InitialLast: {
      /*
       * A priority queue associated with the scheduler which contains exactly
       * the enqueueing thread shall be created as the last priority queue of
       * the thread queue.
       */
      T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_D ) );
      T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_C ) );
      T_eq_ptr( GetUnblock( ctx, &i ), NULL );
      break;
    }

    case ScoreTqReqEnqueuePriority_Post_Position_First: {
      /*
       * The enqueueing thread shall be enqueued in the priority queue
       * associated with the scheduler.
       */
      T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_C ) );
      T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_B ) );
      T_eq_ptr( GetUnblock( ctx, &i ), NULL );
      break;
    }

    case ScoreTqReqEnqueuePriority_Post_Position_Second: {
      /*
       * The enqueueing thread shall be enqueued in the priority queue
       * associated with the scheduler.
       */
      T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_B ) );
      T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_C ) );
      T_eq_ptr( GetUnblock( ctx, &i ), NULL );
      break;
    }

    case ScoreTqReqEnqueuePriority_Post_Position_FirstFirst: {
      /*
       * The enqueueing thread shall be enqueued in the priority queue
       * associated with the scheduler.
       *
       * The position of the priority queue in the thread queue shall not
       * change.
       */
      T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_C ) );
      T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_D ) );
      T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_B ) );
      T_eq_ptr( GetUnblock( ctx, &i ), NULL );
      break;
    }

    case ScoreTqReqEnqueuePriority_Post_Position_SecondFirst: {
      /*
       * The enqueueing thread shall be enqueued in the priority queue
       * associated with the scheduler.
       *
       * The position of the priority queue in the thread queue shall not
       * change.
       */
      T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_B ) );
      T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_D ) );
      T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_C ) );
      T_eq_ptr( GetUnblock( ctx, &i ), NULL );
      break;
    }

    case ScoreTqReqEnqueuePriority_Post_Position_FirstLast: {
      /*
       * The enqueueing thread shall be enqueued in the priority queue
       * associated with the scheduler.
       *
       * The position of the priority queue in the thread queue shall not
       * change.
       */
      T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_D ) );
      T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_C ) );
      T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_B ) );
      T_eq_ptr( GetUnblock( ctx, &i ), NULL );
      break;
    }

    case ScoreTqReqEnqueuePriority_Post_Position_SecondLast: {
      /*
       * The enqueueing thread shall be enqueued in the priority queue
       * associated with the scheduler.
       *
       * The position of the priority queue in the thread queue shall not
       * change.
       */
      T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_D ) );
      T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_B ) );
      T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_C ) );
      T_eq_ptr( GetUnblock( ctx, &i ), NULL );
      break;
    }

    case ScoreTqReqEnqueuePriority_Post_Position_NA:
      break;
  }
}

static void ScoreTqReqEnqueuePriority_Setup(
  ScoreTqReqEnqueuePriority_Context *ctx
)
{
  TQReset( ctx->tq_ctx );
  TQSetPriority( ctx->tq_ctx, TQ_BLOCKER_A, PRIO_ULTRA_HIGH );
  TQSetPriority( ctx->tq_ctx, TQ_BLOCKER_B, PRIO_LOW );
  TQSetPriority( ctx->tq_ctx, TQ_BLOCKER_C, PRIO_VERY_HIGH );

  #if defined( RTEMS_SMP )
  TQSetScheduler( ctx->tq_ctx, TQ_BLOCKER_D, SCHEDULER_B_ID, PRIO_LOW );
  #endif
}

static void ScoreTqReqEnqueuePriority_Setup_Wrap( void *arg )
{
  ScoreTqReqEnqueuePriority_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  ScoreTqReqEnqueuePriority_Setup( ctx );
}

static void ScoreTqReqEnqueuePriority_Teardown(
  ScoreTqReqEnqueuePriority_Context *ctx
)
{
  TQReset( ctx->tq_ctx );
}

static void ScoreTqReqEnqueuePriority_Teardown_Wrap( void *arg )
{
  ScoreTqReqEnqueuePriority_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  ScoreTqReqEnqueuePriority_Teardown( ctx );
}

static void ScoreTqReqEnqueuePriority_Prepare(
  ScoreTqReqEnqueuePriority_Context *ctx
)
{
  ctx->tq_ctx->how_many = 1;
}

static void ScoreTqReqEnqueuePriority_Action(
  ScoreTqReqEnqueuePriority_Context *ctx
)
{
  TQSend( ctx->tq_ctx, TQ_BLOCKER_A, TQ_EVENT_ENQUEUE_PREPARE );

  if ( ctx->other_before ) {
    TQSendAndWaitForExecutionStop(
      ctx->tq_ctx,
      TQ_BLOCKER_D,
      TQ_EVENT_ENQUEUE | TQ_EVENT_SURRENDER | TQ_EVENT_RUNNER_SYNC_2
    );
  }

  if ( ctx->priority != PRIO_PSEUDO_ISR ) {
    TQSetPriority( ctx->tq_ctx, TQ_BLOCKER_B , ctx->priority );
    TQSend(
      ctx->tq_ctx,
      TQ_BLOCKER_B,
      TQ_EVENT_ENQUEUE | TQ_EVENT_SURRENDER
    );
  }

  if ( ctx->other_after ) {
    TQSendAndWaitForExecutionStop(
      ctx->tq_ctx,
      TQ_BLOCKER_D,
      TQ_EVENT_ENQUEUE | TQ_EVENT_SURRENDER | TQ_EVENT_RUNNER_SYNC_2
    );
  }

  if ( ctx->helping ) {
    if ( ctx->other_before || ctx->other_after ) {
      if ( rtems_scheduler_get_processor_maximum() > 2 ) {
        AddHelper( ctx->tq_ctx, SCHEDULER_C_ID );
      }
    } else {
      AddHelper( ctx->tq_ctx, SCHEDULER_B_ID );
    }
  }

  TQSchedulerRecordStart( ctx->tq_ctx );
  TQSend(
    ctx->tq_ctx,
    TQ_BLOCKER_C,
    TQ_EVENT_ENQUEUE | TQ_EVENT_SURRENDER | TQ_EVENT_RUNNER_SYNC
  );
  TQSend( ctx->tq_ctx, TQ_BLOCKER_A, TQ_EVENT_ENQUEUE_DONE );

  if ( ctx->other_before || ctx->other_after ) {
    TQSynchronizeRunner2();
  } else {
    TQSynchronizeRunner();
  }

  TQSchedulerRecordStop( ctx->tq_ctx );

  if ( ctx->helping ) {
    if ( ctx->other_before || ctx->other_after ) {
      if ( rtems_scheduler_get_processor_maximum() > 2 ) {
        RemoveHelper( ctx->tq_ctx );
      }
    } else {
      RemoveHelper( ctx->tq_ctx );
    }
  }
}

static const ScoreTqReqEnqueuePriority_Entry
ScoreTqReqEnqueuePriority_Entries[] = {
  { 1, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_Position_NA },
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_Position_SecondLast },
#else
  { 1, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_Position_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_Position_SecondFirst },
#else
  { 1, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_Position_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_Position_InitialLast },
#else
  { 1, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_Position_NA },
#endif
  { 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_Position_Second },
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_Position_FirstLast },
#else
  { 1, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_Position_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_Position_FirstFirst },
#else
  { 1, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_Position_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_Position_Second },
#else
  { 1, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_Position_NA },
#endif
  { 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_Position_InitialFirst },
  { 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_Position_First },
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_Position_InitialFirst },
#else
  { 1, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_Position_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_Position_First }
#else
  { 1, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_Position_NA }
#endif
};

static const uint8_t
ScoreTqReqEnqueuePriority_Map[] = {
  8, 3, 0, 0, 4, 0, 1, 2, 4, 0, 1, 2, 9, 0, 5, 6, 10, 3, 0, 0, 7, 0, 1, 2, 7,
  0, 1, 2, 11, 0, 5, 6
};

static size_t ScoreTqReqEnqueuePriority_Scope( void *arg, char *buf, size_t n )
{
  ScoreTqReqEnqueuePriority_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      ScoreTqReqEnqueuePriority_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture ScoreTqReqEnqueuePriority_Fixture = {
  .setup = ScoreTqReqEnqueuePriority_Setup_Wrap,
  .stop = NULL,
  .teardown = ScoreTqReqEnqueuePriority_Teardown_Wrap,
  .scope = ScoreTqReqEnqueuePriority_Scope,
  .initial_context = &ScoreTqReqEnqueuePriority_Instance
};

static inline ScoreTqReqEnqueuePriority_Entry
ScoreTqReqEnqueuePriority_PopEntry( ScoreTqReqEnqueuePriority_Context *ctx )
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return ScoreTqReqEnqueuePriority_Entries[
    ScoreTqReqEnqueuePriority_Map[ index ]
  ];
}

static void ScoreTqReqEnqueuePriority_TestVariant(
  ScoreTqReqEnqueuePriority_Context *ctx
)
{
  ScoreTqReqEnqueuePriority_Pre_EligibleScheduler_Prepare(
    ctx,
    ctx->Map.pcs[ 0 ]
  );
  ScoreTqReqEnqueuePriority_Pre_QueueEligible_Prepare(
    ctx,
    ctx->Map.pcs[ 1 ]
  );
  ScoreTqReqEnqueuePriority_Pre_QueueIneligible_Prepare(
    ctx,
    ctx->Map.pcs[ 2 ]
  );
  ScoreTqReqEnqueuePriority_Action( ctx );
  ScoreTqReqEnqueuePriority_Post_Position_Check(
    ctx,
    ctx->Map.entry.Post_Position
  );
}

static T_fixture_node ScoreTqReqEnqueuePriority_Node;

static T_remark ScoreTqReqEnqueuePriority_Remark = {
  .next = NULL,
  .remark = "ScoreTqReqEnqueuePriority"
};

void ScoreTqReqEnqueuePriority_Run( TQContext *tq_ctx )
{
  ScoreTqReqEnqueuePriority_Context *ctx;

  ctx = &ScoreTqReqEnqueuePriority_Instance;
  ctx->tq_ctx = tq_ctx;

  ctx = T_push_fixture(
    &ScoreTqReqEnqueuePriority_Node,
    &ScoreTqReqEnqueuePriority_Fixture
  );
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = ScoreTqReqEnqueuePriority_Pre_EligibleScheduler_Home;
    ctx->Map.pcs[ 0 ] < ScoreTqReqEnqueuePriority_Pre_EligibleScheduler_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = ScoreTqReqEnqueuePriority_Pre_QueueEligible_None;
      ctx->Map.pcs[ 1 ] < ScoreTqReqEnqueuePriority_Pre_QueueEligible_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = ScoreTqReqEnqueuePriority_Pre_QueueIneligible_None;
        ctx->Map.pcs[ 2 ] < ScoreTqReqEnqueuePriority_Pre_QueueIneligible_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        ctx->Map.entry = ScoreTqReqEnqueuePriority_PopEntry( ctx );
        ScoreTqReqEnqueuePriority_Prepare( ctx );
        ScoreTqReqEnqueuePriority_TestVariant( ctx );
      }
    }
  }

  T_add_remark( &ScoreTqReqEnqueuePriority_Remark );
  T_pop_fixture();
}

/** @} */
