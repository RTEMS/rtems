/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreTqReqEnqueueCeiling
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

#include "tr-tq-enqueue-ceiling.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreTqReqEnqueueCeiling spec:/score/tq/req/enqueue-ceiling
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
  uint8_t Post_Position : 3;
} ScoreTqReqEnqueueCeiling_Entry;

/**
 * @brief Test context for spec:/score/tq/req/enqueue-ceiling test case.
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
   *   ScoreTqReqEnqueueCeiling_Run() parameter.
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
    ScoreTqReqEnqueueCeiling_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} ScoreTqReqEnqueueCeiling_Context;

static ScoreTqReqEnqueueCeiling_Context
  ScoreTqReqEnqueueCeiling_Instance;

static const char * const ScoreTqReqEnqueueCeiling_PreDesc_EligibleScheduler[] = {
  "Home",
  "Helping",
  "NA"
};

static const char * const ScoreTqReqEnqueueCeiling_PreDesc_QueueEligible[] = {
  "None",
  "Equal",
  "Low",
  "NA"
};

static const char * const ScoreTqReqEnqueueCeiling_PreDesc_QueueIneligible[] = {
  "None",
  "Before",
  "After",
  "NA"
};

static const char * const * const ScoreTqReqEnqueueCeiling_PreDesc[] = {
  ScoreTqReqEnqueueCeiling_PreDesc_EligibleScheduler,
  ScoreTqReqEnqueueCeiling_PreDesc_QueueEligible,
  ScoreTqReqEnqueueCeiling_PreDesc_QueueIneligible,
  NULL
};

typedef ScoreTqReqEnqueueCeiling_Context Context;

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

static void ScoreTqReqEnqueueCeiling_Pre_EligibleScheduler_Prepare(
  ScoreTqReqEnqueueCeiling_Context              *ctx,
  ScoreTqReqEnqueueCeiling_Pre_EligibleScheduler state
)
{
  switch ( state ) {
    case ScoreTqReqEnqueueCeiling_Pre_EligibleScheduler_Home: {
      /*
       * While the enqueueing thread has no helping scheduler.
       */
      ctx->helping = false;
      break;
    }

    case ScoreTqReqEnqueueCeiling_Pre_EligibleScheduler_Helping: {
      /*
       * While the enqueueing thread has at least one helping scheduler.
       */
      ctx->helping = true;
      break;
    }

    case ScoreTqReqEnqueueCeiling_Pre_EligibleScheduler_NA:
      break;
  }
}

static void ScoreTqReqEnqueueCeiling_Pre_QueueEligible_Prepare(
  ScoreTqReqEnqueueCeiling_Context          *ctx,
  ScoreTqReqEnqueueCeiling_Pre_QueueEligible state
)
{
  switch ( state ) {
    case ScoreTqReqEnqueueCeiling_Pre_QueueEligible_None: {
      /*
       * While all priority queues of the thread queue associated with eligible
       * schedulers of the enqueueing thread are empty.
       */
      /* This is the default */
      break;
    }

    case ScoreTqReqEnqueueCeiling_Pre_QueueEligible_Equal: {
      /*
       * While a priority queue of the thread queue associated with an eligible
       * scheduler of the enqueueing thread is non-empty, while the highest
       * priority of the priority queue is equal to the priority of the
       * enqueueing thread with respect to the eligible scheduler.
       */
      ctx->priority = PRIO_VERY_HIGH;
      break;
    }

    case ScoreTqReqEnqueueCeiling_Pre_QueueEligible_Low: {
      /*
       * While a priority queue of the thread queue associated with an eligible
       * scheduler of the enqueueing thread is non-empty, while the highest
       * priority of the priority queue is lower than the priority of the
       * enqueueing thread with respect to the eligible scheduler.
       */
      ctx->priority = PRIO_HIGH;
      break;
    }

    case ScoreTqReqEnqueueCeiling_Pre_QueueEligible_NA:
      break;
  }
}

static void ScoreTqReqEnqueueCeiling_Pre_QueueIneligible_Prepare(
  ScoreTqReqEnqueueCeiling_Context            *ctx,
  ScoreTqReqEnqueueCeiling_Pre_QueueIneligible state
)
{
  switch ( state ) {
    case ScoreTqReqEnqueueCeiling_Pre_QueueIneligible_None: {
      /*
       * While no priority queue of the thread queue exists which is not
       * associated with an eligible scheduler of the enqueueing thread.
       */
      /* This is the default */
      break;
    }

    case ScoreTqReqEnqueueCeiling_Pre_QueueIneligible_Before: {
      /*
       * While a priority queue of the thread queue exists which is not
       * associated with an eligible scheduler of the enqueueing thread, while
       * this priority queue is positioned before all priority queues which are
       * associated with eligible schedulers of the enqueueing thread.
       */
      ctx->other_before = true;
      break;
    }

    case ScoreTqReqEnqueueCeiling_Pre_QueueIneligible_After: {
      /*
       * While a priority queue of the thread queue exists which is not
       * associated with an eligible scheduler of the enqueueing thread, while
       * this priority queue is positioned after all priority queues which are
       * associated with eligible schedulers of the enqueueing thread.
       */
      ctx->other_after = true;
      break;
    }

    case ScoreTqReqEnqueueCeiling_Pre_QueueIneligible_NA:
      break;
  }
}

static void ScoreTqReqEnqueueCeiling_Post_Position_Check(
  ScoreTqReqEnqueueCeiling_Context      *ctx,
  ScoreTqReqEnqueueCeiling_Post_Position state
)
{
  size_t i;

  i = 0;

  /* Event receives */
  T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_C ) );
  T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_A ) );

  switch ( state ) {
    case ScoreTqReqEnqueueCeiling_Post_Position_InitialFirst: {
      /*
       * A priority queue associated with the scheduler which contains exactly
       * the enqueueing thread shall be created as the first priority queue of
       * the thread queue.
       */
      T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_C ) );
      T_eq_ptr( GetUnblock( ctx, &i ), NULL );
      break;
    }

    case ScoreTqReqEnqueueCeiling_Post_Position_First: {
      /*
       * The enqueueing thread shall be enqueued in the priority queue
       * associated with the scheduler.
       */
      T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_C ) );
      T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_B ) );
      T_eq_ptr( GetUnblock( ctx, &i ), NULL );
      break;
    }

    case ScoreTqReqEnqueueCeiling_Post_Position_Second: {
      /*
       * The enqueueing thread shall be enqueued in the priority queue
       * associated with the scheduler.
       */
      T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_B ) );
      T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_C ) );
      T_eq_ptr( GetUnblock( ctx, &i ), NULL );
      break;
    }

    case ScoreTqReqEnqueueCeiling_Post_Position_FirstFirst: {
      /*
       * The enqueueing thread shall be enqueued in the priority queue
       * associated with the scheduler.
       *
       * The position of the priority queue in the thread queue shall not
       * change.
       */
      T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_C ) );
      T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_B ) );
      T_eq_ptr( GetUnblock( ctx, &i ), NULL );
      break;
    }

    case ScoreTqReqEnqueueCeiling_Post_Position_SecondFirst: {
      /*
       * The enqueueing thread shall be enqueued in the priority queue
       * associated with the scheduler.
       *
       * The position of the priority queue in the thread queue shall not
       * change.
       */
      T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_B ) );
      T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_C ) );
      T_eq_ptr( GetUnblock( ctx, &i ), NULL );
      break;
    }

    case ScoreTqReqEnqueueCeiling_Post_Position_SecondQueue: {
      /*
       * The enqueueing thread shall be enqueued in the priority queue
       * associated with the scheduler.
       *
       * The position of the priority queue in the thread queue shall not
       * change.
       */
      T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_B ) );
      T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_C ) );
      T_eq_ptr( GetUnblock( ctx, &i ), NULL );
      break;
    }

    case ScoreTqReqEnqueueCeiling_Post_Position_NA:
      break;
  }
}

static void ScoreTqReqEnqueueCeiling_Setup(
  ScoreTqReqEnqueueCeiling_Context *ctx
)
{
  rtems_id scheduler_id;

  scheduler_id = SCHEDULER_A_ID;
  TQSetScheduler( ctx->tq_ctx, TQ_BLOCKER_A, scheduler_id, PRIO_VERY_HIGH );
  TQSetScheduler( ctx->tq_ctx, TQ_BLOCKER_B, scheduler_id, PRIO_VERY_HIGH );
  TQSetScheduler( ctx->tq_ctx, TQ_BLOCKER_C, scheduler_id, PRIO_VERY_HIGH );
  #if defined( RTEMS_SMP )
  TQSetScheduler( ctx->tq_ctx, TQ_BLOCKER_D, SCHEDULER_B_ID, PRIO_LOW );
  #endif
}

static void ScoreTqReqEnqueueCeiling_Setup_Wrap( void *arg )
{
  ScoreTqReqEnqueueCeiling_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  ScoreTqReqEnqueueCeiling_Setup( ctx );
}

static void ScoreTqReqEnqueueCeiling_Teardown(
  ScoreTqReqEnqueueCeiling_Context *ctx
)
{
  TQReset( ctx->tq_ctx );
}

static void ScoreTqReqEnqueueCeiling_Teardown_Wrap( void *arg )
{
  ScoreTqReqEnqueueCeiling_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  ScoreTqReqEnqueueCeiling_Teardown( ctx );
}

static void ScoreTqReqEnqueueCeiling_Prepare(
  ScoreTqReqEnqueueCeiling_Context *ctx
)
{
  ctx->priority = PRIO_PSEUDO_ISR;
  ctx->other_before = false;
  ctx->other_after = false;
}

static void ScoreTqReqEnqueueCeiling_Action(
  ScoreTqReqEnqueueCeiling_Context *ctx
)
{
  Status_Control status;

  if ( ctx->priority == PRIO_PSEUDO_ISR ) {
    TQSend( ctx->tq_ctx, TQ_BLOCKER_A, TQ_EVENT_ENQUEUE );
  } else {
    TQSetPriority( ctx->tq_ctx, TQ_BLOCKER_B , ctx->priority );

    if ( ctx->other_before || ctx->other_after ) {
      TQSend( ctx->tq_ctx, TQ_BLOCKER_B, TQ_EVENT_MUTEX_B_OBTAIN );
      TQSendAndWaitForExecutionStop(
        ctx->tq_ctx,
        TQ_BLOCKER_D,
        TQ_EVENT_MUTEX_B_OBTAIN | TQ_EVENT_MUTEX_B_RELEASE |
          TQ_EVENT_RUNNER_SYNC
      );

      if ( ctx->other_before ) {
        TQSend( ctx->tq_ctx, TQ_BLOCKER_C, TQ_EVENT_ENQUEUE );
      }

      TQSend( ctx->tq_ctx, TQ_BLOCKER_A, TQ_EVENT_ENQUEUE );
      TQSend(
        ctx->tq_ctx,
        TQ_BLOCKER_B,
        TQ_EVENT_ENQUEUE | TQ_EVENT_SURRENDER
      );

      if ( ctx->other_before ) {
        TQSend( ctx->tq_ctx, TQ_BLOCKER_C, TQ_EVENT_SURRENDER );
      }
    } else {
      TQSend( ctx->tq_ctx, TQ_BLOCKER_A, TQ_EVENT_ENQUEUE );
      TQSend(
        ctx->tq_ctx,
        TQ_BLOCKER_B,
        TQ_EVENT_ENQUEUE | TQ_EVENT_SURRENDER
      );
    }
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
  TQSend( ctx->tq_ctx, TQ_BLOCKER_C, TQ_EVENT_ENQUEUE | TQ_EVENT_SURRENDER );
  TQSend( ctx->tq_ctx, TQ_BLOCKER_A, TQ_EVENT_SURRENDER );
  status = TQEnqueue( ctx->tq_ctx, TQ_WAIT_FOREVER );
  T_eq_int( status, TQConvertStatus( ctx->tq_ctx, STATUS_SUCCESSFUL ) );
  TQSchedulerRecordStop( ctx->tq_ctx );
  status = TQSurrender( ctx->tq_ctx );
  T_eq_int( status, TQConvertStatus( ctx->tq_ctx, STATUS_SUCCESSFUL ) );

  if (
    ctx->priority != PRIO_PSEUDO_ISR &&
    ( ctx->other_before || ctx->other_after )
  ) {
    TQSend( ctx->tq_ctx, TQ_BLOCKER_B, TQ_EVENT_MUTEX_B_RELEASE );
    TQSynchronizeRunner();
  }

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

static const ScoreTqReqEnqueueCeiling_Entry
ScoreTqReqEnqueueCeiling_Entries[] = {
  { 1, 0, 0, 0, ScoreTqReqEnqueueCeiling_Post_Position_NA },
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, ScoreTqReqEnqueueCeiling_Post_Position_SecondQueue },
#else
  { 1, 0, 0, 0, ScoreTqReqEnqueueCeiling_Post_Position_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, ScoreTqReqEnqueueCeiling_Post_Position_SecondFirst },
#else
  { 1, 0, 0, 0, ScoreTqReqEnqueueCeiling_Post_Position_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, ScoreTqReqEnqueueCeiling_Post_Position_FirstFirst },
#else
  { 1, 0, 0, 0, ScoreTqReqEnqueueCeiling_Post_Position_NA },
#endif
  { 0, 0, 0, 0, ScoreTqReqEnqueueCeiling_Post_Position_InitialFirst },
  { 0, 0, 0, 0, ScoreTqReqEnqueueCeiling_Post_Position_Second },
  { 0, 0, 0, 0, ScoreTqReqEnqueueCeiling_Post_Position_First },
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, ScoreTqReqEnqueueCeiling_Post_Position_InitialFirst },
#else
  { 1, 0, 0, 0, ScoreTqReqEnqueueCeiling_Post_Position_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, ScoreTqReqEnqueueCeiling_Post_Position_Second },
#else
  { 1, 0, 0, 0, ScoreTqReqEnqueueCeiling_Post_Position_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, ScoreTqReqEnqueueCeiling_Post_Position_First }
#else
  { 1, 0, 0, 0, ScoreTqReqEnqueueCeiling_Post_Position_NA }
#endif
};

static const uint8_t
ScoreTqReqEnqueueCeiling_Map[] = {
  4, 0, 0, 5, 1, 2, 6, 1, 3, 7, 0, 0, 8, 1, 2, 9, 1, 3
};

static size_t ScoreTqReqEnqueueCeiling_Scope( void *arg, char *buf, size_t n )
{
  ScoreTqReqEnqueueCeiling_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      ScoreTqReqEnqueueCeiling_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture ScoreTqReqEnqueueCeiling_Fixture = {
  .setup = ScoreTqReqEnqueueCeiling_Setup_Wrap,
  .stop = NULL,
  .teardown = ScoreTqReqEnqueueCeiling_Teardown_Wrap,
  .scope = ScoreTqReqEnqueueCeiling_Scope,
  .initial_context = &ScoreTqReqEnqueueCeiling_Instance
};

static inline ScoreTqReqEnqueueCeiling_Entry ScoreTqReqEnqueueCeiling_PopEntry(
  ScoreTqReqEnqueueCeiling_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return ScoreTqReqEnqueueCeiling_Entries[
    ScoreTqReqEnqueueCeiling_Map[ index ]
  ];
}

static void ScoreTqReqEnqueueCeiling_TestVariant(
  ScoreTqReqEnqueueCeiling_Context *ctx
)
{
  ScoreTqReqEnqueueCeiling_Pre_EligibleScheduler_Prepare(
    ctx,
    ctx->Map.pcs[ 0 ]
  );
  ScoreTqReqEnqueueCeiling_Pre_QueueEligible_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  ScoreTqReqEnqueueCeiling_Pre_QueueIneligible_Prepare(
    ctx,
    ctx->Map.pcs[ 2 ]
  );
  ScoreTqReqEnqueueCeiling_Action( ctx );
  ScoreTqReqEnqueueCeiling_Post_Position_Check(
    ctx,
    ctx->Map.entry.Post_Position
  );
}

static T_fixture_node ScoreTqReqEnqueueCeiling_Node;

static T_remark ScoreTqReqEnqueueCeiling_Remark = {
  .next = NULL,
  .remark = "ScoreTqReqEnqueueCeiling"
};

void ScoreTqReqEnqueueCeiling_Run( TQContext *tq_ctx )
{
  ScoreTqReqEnqueueCeiling_Context *ctx;

  ctx = &ScoreTqReqEnqueueCeiling_Instance;
  ctx->tq_ctx = tq_ctx;

  ctx = T_push_fixture(
    &ScoreTqReqEnqueueCeiling_Node,
    &ScoreTqReqEnqueueCeiling_Fixture
  );
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = ScoreTqReqEnqueueCeiling_Pre_EligibleScheduler_Home;
    ctx->Map.pcs[ 0 ] < ScoreTqReqEnqueueCeiling_Pre_EligibleScheduler_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = ScoreTqReqEnqueueCeiling_Pre_QueueEligible_None;
      ctx->Map.pcs[ 1 ] < ScoreTqReqEnqueueCeiling_Pre_QueueEligible_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = ScoreTqReqEnqueueCeiling_Pre_QueueIneligible_None;
        ctx->Map.pcs[ 2 ] < ScoreTqReqEnqueueCeiling_Pre_QueueIneligible_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        ctx->Map.entry = ScoreTqReqEnqueueCeiling_PopEntry( ctx );
        ScoreTqReqEnqueueCeiling_Prepare( ctx );
        ScoreTqReqEnqueueCeiling_TestVariant( ctx );
      }
    }
  }

  T_add_remark( &ScoreTqReqEnqueueCeiling_Remark );
  T_pop_fixture();
}

/** @} */
