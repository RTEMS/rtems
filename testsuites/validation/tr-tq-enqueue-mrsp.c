/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreTqReqEnqueueMrsp
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

#include "tr-tq-enqueue-mrsp.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreTqReqEnqueueMrsp spec:/score/tq/req/enqueue-mrsp
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
} ScoreTqReqEnqueueMrsp_Entry;

/**
 * @brief Test context for spec:/score/tq/req/enqueue-mrsp test case.
 */
typedef struct {
  /**
   * @brief This this member is true, then the enqueueing thread shall have at
   *   least one helping scheduler which is an ineligible scheduler for the
   *   already enqueued threads.
   */
  bool helping;

  /**
   * @brief This member specifies the priority of an already enqueued thread
   *   with an eligible scheduler equal to an eligible scheduler of the
   *   enqueueing thread.
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
   *   ScoreTqReqEnqueueMrsp_Run() parameter.
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
    ScoreTqReqEnqueueMrsp_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} ScoreTqReqEnqueueMrsp_Context;

static ScoreTqReqEnqueueMrsp_Context
  ScoreTqReqEnqueueMrsp_Instance;

static const char * const ScoreTqReqEnqueueMrsp_PreDesc_EligibleScheduler[] = {
  "Home",
  "Helping",
  "NA"
};

static const char * const ScoreTqReqEnqueueMrsp_PreDesc_QueueEligible[] = {
  "None",
  "Equal",
  "Low",
  "NA"
};

static const char * const ScoreTqReqEnqueueMrsp_PreDesc_QueueIneligible[] = {
  "None",
  "Only",
  "Before",
  "After",
  "NA"
};

static const char * const * const ScoreTqReqEnqueueMrsp_PreDesc[] = {
  ScoreTqReqEnqueueMrsp_PreDesc_EligibleScheduler,
  ScoreTqReqEnqueueMrsp_PreDesc_QueueEligible,
  ScoreTqReqEnqueueMrsp_PreDesc_QueueIneligible,
  NULL
};

/*
 * The MrsP locking protocol uses a sticky thread queue enqueue.  This means
 * that threads waiting for the mutex ownership perform a busy wait and thus
 * occupy the processor.  For a full validation we need at least four
 * processors.
 */
static bool CanDoFullValidation( void )
{
  return rtems_scheduler_get_processor_maximum() >= 4;
}

static void ScoreTqReqEnqueueMrsp_Pre_EligibleScheduler_Prepare(
  ScoreTqReqEnqueueMrsp_Context              *ctx,
  ScoreTqReqEnqueueMrsp_Pre_EligibleScheduler state
)
{
  switch ( state ) {
    case ScoreTqReqEnqueueMrsp_Pre_EligibleScheduler_Home: {
      /*
       * While the enqueueing thread has no helping scheduler.
       */
      ctx->helping = false;
      break;
    }

    case ScoreTqReqEnqueueMrsp_Pre_EligibleScheduler_Helping: {
      /*
       * While the enqueueing thread has at least one helping scheduler.
       */
      ctx->helping = true;
      break;
    }

    case ScoreTqReqEnqueueMrsp_Pre_EligibleScheduler_NA:
      break;
  }
}

static void ScoreTqReqEnqueueMrsp_Pre_QueueEligible_Prepare(
  ScoreTqReqEnqueueMrsp_Context          *ctx,
  ScoreTqReqEnqueueMrsp_Pre_QueueEligible state
)
{
  switch ( state ) {
    case ScoreTqReqEnqueueMrsp_Pre_QueueEligible_None: {
      /*
       * While all priority queues of the thread queue associated with eligible
       * schedulers of the enqueueing thread are empty.
       */
      ctx->priority = PRIO_PSEUDO_ISR;
      break;
    }

    case ScoreTqReqEnqueueMrsp_Pre_QueueEligible_Equal: {
      /*
       * While a priority queue of the thread queue associated with an eligible
       * scheduler of the enqueueing thread is non-empty, while the highest
       * priority of the priority queue is equal to the priority of the
       * enqueueing thread with respect to the eligible scheduler.
       */
      ctx->priority = PRIO_VERY_HIGH;
      break;
    }

    case ScoreTqReqEnqueueMrsp_Pre_QueueEligible_Low: {
      /*
       * While a priority queue of the thread queue associated with an eligible
       * scheduler of the enqueueing thread is non-empty, while the highest
       * priority of the priority queue is lower than the priority of the
       * enqueueing thread with respect to the eligible scheduler.
       */
      ctx->priority = PRIO_HIGH;
      break;
    }

    case ScoreTqReqEnqueueMrsp_Pre_QueueEligible_NA:
      break;
  }
}

static void ScoreTqReqEnqueueMrsp_Pre_QueueIneligible_Prepare(
  ScoreTqReqEnqueueMrsp_Context            *ctx,
  ScoreTqReqEnqueueMrsp_Pre_QueueIneligible state
)
{
  switch ( state ) {
    case ScoreTqReqEnqueueMrsp_Pre_QueueIneligible_None: {
      /*
       * While no priority queue of the thread queue exists which is not
       * associated with an eligible scheduler of the enqueueing thread.
       */
      ctx->other_before = false;
      ctx->other_after = false;
      break;
    }

    case ScoreTqReqEnqueueMrsp_Pre_QueueIneligible_Only: {
      /*
       * While exactly one priority queue of the thread queue exists which is
       * not associated with an eligible scheduler of the enqueueing thread.
       */
      ctx->other_before = true;
      ctx->other_after = false;
      break;
    }

    case ScoreTqReqEnqueueMrsp_Pre_QueueIneligible_Before: {
      /*
       * While a priority queue of the thread queue exists which is not
       * associated with an eligible scheduler of the enqueueing thread, while
       * this priority queue is positioned before all priority queues which are
       * associated with eligible schedulers of the enqueueing thread.
       */
      ctx->other_before = true;
      ctx->other_after = false;
      break;
    }

    case ScoreTqReqEnqueueMrsp_Pre_QueueIneligible_After: {
      /*
       * While a priority queue of the thread queue exists which is not
       * associated with an eligible scheduler of the enqueueing thread, while
       * this priority queue is positioned after all priority queues which are
       * associated with eligible schedulers of the enqueueing thread.
       */
      ctx->other_before = false;
      ctx->other_after = true;
      break;
    }

    case ScoreTqReqEnqueueMrsp_Pre_QueueIneligible_NA:
      break;
  }
}

static void ScoreTqReqEnqueueMrsp_Post_Position_Check(
  ScoreTqReqEnqueueMrsp_Context      *ctx,
  ScoreTqReqEnqueueMrsp_Post_Position state
)
{
  size_t i;

  i = 0;

  /* The enqueue is sticky, so no enqueued thread is blocked by the scheduler */
  T_null( TQGetNextUnblock( ctx->tq_ctx, &i )->thread );

  switch ( state ) {
    case ScoreTqReqEnqueueMrsp_Post_Position_InitialFirst: {
      /*
       * A priority queue associated with the scheduler which contains exactly
       * the enqueueing thread shall be created as the first priority queue of
       * the thread queue.
       */
      T_eq_u32( 1, TQGetWorkerCounter( ctx->tq_ctx, TQ_BLOCKER_A ) );
      T_eq_u32( 1, TQGetCounter( ctx->tq_ctx ) );
      break;
    }

    case ScoreTqReqEnqueueMrsp_Post_Position_InitialLast: {
      /*
       * A priority queue associated with the scheduler which contains exactly
       * the enqueueing thread shall be created as the last priority queue of
       * the thread queue.
       */
      if ( CanDoFullValidation() ) {
        T_eq_u32( 1, TQGetWorkerCounter( ctx->tq_ctx, TQ_BLOCKER_C ) );
        T_eq_u32( 2, TQGetWorkerCounter( ctx->tq_ctx, TQ_BLOCKER_A ) );
        T_eq_u32( 2, TQGetCounter( ctx->tq_ctx ) );
      } else {
        T_eq_u32( 1, TQGetWorkerCounter( ctx->tq_ctx, TQ_BLOCKER_A ) );
        T_eq_u32( 1, TQGetCounter( ctx->tq_ctx ) );
      }
      break;
    }

    case ScoreTqReqEnqueueMrsp_Post_Position_Second: {
      /*
       * The enqueueing thread shall be enqueued in the priority queue
       * associated with the scheduler.
       */
      if ( CanDoFullValidation() ) {
        T_eq_u32( 1, TQGetWorkerCounter( ctx->tq_ctx, TQ_BLOCKER_B ) );
        T_eq_u32( 2, TQGetWorkerCounter( ctx->tq_ctx, TQ_BLOCKER_A ) );
        T_eq_u32( 2, TQGetCounter( ctx->tq_ctx ) );
      } else {
        T_eq_u32( 1, TQGetWorkerCounter( ctx->tq_ctx, TQ_BLOCKER_A ) );
        T_eq_u32( 1, TQGetCounter( ctx->tq_ctx ) );
      }
      break;
    }

    case ScoreTqReqEnqueueMrsp_Post_Position_SecondFirst: {
      /*
       * The enqueueing thread shall be enqueued in the priority queue
       * associated with the scheduler.
       *
       * The position of the priority queue in the thread queue shall not
       * change.
       */
      if ( CanDoFullValidation() ) {
        T_eq_u32( 1, TQGetWorkerCounter( ctx->tq_ctx, TQ_BLOCKER_B ) );
        T_eq_u32( 2, TQGetWorkerCounter( ctx->tq_ctx, TQ_BLOCKER_C ) );
        T_eq_u32( 3, TQGetWorkerCounter( ctx->tq_ctx, TQ_BLOCKER_A ) );
        T_eq_u32( 3, TQGetCounter( ctx->tq_ctx ) );
      } else {
        T_eq_u32( 1, TQGetWorkerCounter( ctx->tq_ctx, TQ_BLOCKER_A ) );
        T_eq_u32( 1, TQGetCounter( ctx->tq_ctx ) );
      }
      break;
    }

    case ScoreTqReqEnqueueMrsp_Post_Position_SecondLast: {
      /*
       * The enqueueing thread shall be enqueued in the priority queue
       * associated with the scheduler.
       *
       * The position of the priority queue in the thread queue shall not
       * change.
       */
      if ( CanDoFullValidation() ) {
        T_eq_u32( 1, TQGetWorkerCounter( ctx->tq_ctx, TQ_BLOCKER_C ) );
        T_eq_u32( 2, TQGetWorkerCounter( ctx->tq_ctx, TQ_BLOCKER_B ) );
        T_eq_u32( 3, TQGetWorkerCounter( ctx->tq_ctx, TQ_BLOCKER_A ) );
        T_eq_u32( 3, TQGetCounter( ctx->tq_ctx ) );
      } else {
        T_eq_u32( 1, TQGetWorkerCounter( ctx->tq_ctx, TQ_BLOCKER_A ) );
        T_eq_u32( 1, TQGetCounter( ctx->tq_ctx ) );
      }
      break;
    }

    case ScoreTqReqEnqueueMrsp_Post_Position_NA:
      break;
  }
}

static void ScoreTqReqEnqueueMrsp_Setup( ScoreTqReqEnqueueMrsp_Context *ctx )
{
  if ( CanDoFullValidation() ) {
    RemoveProcessor( SCHEDULER_C_ID, 2 );
    AddProcessor( SCHEDULER_B_ID, 2 );
    TQSetScheduler( ctx->tq_ctx, TQ_BLOCKER_C, SCHEDULER_C_ID, PRIO_LOW );
  }

  TQSetScheduler( ctx->tq_ctx, TQ_BLOCKER_A, SCHEDULER_B_ID, PRIO_LOW );
  TQSetScheduler( ctx->tq_ctx, TQ_BLOCKER_B, SCHEDULER_B_ID, PRIO_LOW );
  TQSetScheduler(
    ctx->tq_ctx,
    TQ_BLOCKER_D,
    SCHEDULER_A_ID,
    PRIO_ULTRA_HIGH
  );
}

static void ScoreTqReqEnqueueMrsp_Setup_Wrap( void *arg )
{
  ScoreTqReqEnqueueMrsp_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  ScoreTqReqEnqueueMrsp_Setup( ctx );
}

static void ScoreTqReqEnqueueMrsp_Teardown(
  ScoreTqReqEnqueueMrsp_Context *ctx
)
{
  if ( CanDoFullValidation() ) {
    RemoveProcessor( SCHEDULER_B_ID, 2 );
    AddProcessor( SCHEDULER_C_ID, 2 );
  }

  TQReset( ctx->tq_ctx );
}

static void ScoreTqReqEnqueueMrsp_Teardown_Wrap( void *arg )
{
  ScoreTqReqEnqueueMrsp_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  ScoreTqReqEnqueueMrsp_Teardown( ctx );
}

static void ScoreTqReqEnqueueMrsp_Action( ScoreTqReqEnqueueMrsp_Context *ctx )
{
  Status_Control status;

  TQResetCounter( ctx->tq_ctx );
  TQClearDone( ctx->tq_ctx, TQ_BLOCKER_A );
  TQClearDone( ctx->tq_ctx, TQ_BLOCKER_B );
  TQClearDone( ctx->tq_ctx, TQ_BLOCKER_C );

  status = TQEnqueue( ctx->tq_ctx, TQ_WAIT_FOREVER );
  T_eq_int( status, TQConvertStatus( ctx->tq_ctx, STATUS_SUCCESSFUL ) );

  if ( ctx->helping ) {
    TQSend(
      ctx->tq_ctx,
      TQ_BLOCKER_A,
      TQ_EVENT_MUTEX_A_OBTAIN | TQ_EVENT_RUNNER_SYNC
    );
    TQSynchronizeRunner();
    TQSend(
      ctx->tq_ctx,
      TQ_BLOCKER_D,
      TQ_EVENT_MUTEX_A_OBTAIN | TQ_EVENT_MUTEX_A_RELEASE |
        TQ_EVENT_RUNNER_SYNC_2
    );
  }

  if ( CanDoFullValidation() ) {
    if ( ctx->other_before ) {
      TQSendAndWaitForIntendToBlock(
        ctx->tq_ctx,
        TQ_BLOCKER_C,
        TQ_EVENT_ENQUEUE | TQ_EVENT_SURRENDER
      );
    }

    if ( ctx->priority != PRIO_PSEUDO_ISR ) {
      TQSendAndWaitForIntendToBlock(
        ctx->tq_ctx,
        TQ_BLOCKER_B,
        TQ_EVENT_ENQUEUE | TQ_EVENT_SURRENDER
      );
    }

    if ( ctx->other_after ) {
      TQSendAndWaitForIntendToBlock(
        ctx->tq_ctx,
        TQ_BLOCKER_C,
        TQ_EVENT_ENQUEUE | TQ_EVENT_SURRENDER
      );
    }
  }

  TQSendAndWaitForIntendToBlock(
    ctx->tq_ctx,
    TQ_BLOCKER_A,
    TQ_EVENT_ENQUEUE | TQ_EVENT_SURRENDER
  );

  TQSchedulerRecordStart( ctx->tq_ctx );
  status = TQSurrender( ctx->tq_ctx );
  T_eq_int( status, TQConvertStatus( ctx->tq_ctx, STATUS_SUCCESSFUL ) );
  TQWaitForDone( ctx->tq_ctx, TQ_BLOCKER_A );

  if ( CanDoFullValidation() ) {
    if ( ctx->priority != PRIO_PSEUDO_ISR ) {
      TQWaitForDone( ctx->tq_ctx, TQ_BLOCKER_B );
    }

    if ( ctx->other_before || ctx->other_after ) {
      TQWaitForDone( ctx->tq_ctx, TQ_BLOCKER_C );
    }
  }

  TQSchedulerRecordStop( ctx->tq_ctx );

  if ( ctx->helping ) {
    TQSend(
      ctx->tq_ctx,
      TQ_BLOCKER_A,
      TQ_EVENT_MUTEX_A_RELEASE | TQ_EVENT_RUNNER_SYNC
    );
    TQSynchronizeRunner2();
  }
}

static const ScoreTqReqEnqueueMrsp_Entry
ScoreTqReqEnqueueMrsp_Entries[] = {
  { 1, 0, 0, 0, ScoreTqReqEnqueueMrsp_Post_Position_NA },
  { 0, 0, 0, 0, ScoreTqReqEnqueueMrsp_Post_Position_Second },
  { 0, 0, 0, 0, ScoreTqReqEnqueueMrsp_Post_Position_SecondLast },
  { 0, 0, 0, 0, ScoreTqReqEnqueueMrsp_Post_Position_SecondFirst },
  { 0, 0, 0, 0, ScoreTqReqEnqueueMrsp_Post_Position_InitialFirst },
  { 0, 0, 0, 0, ScoreTqReqEnqueueMrsp_Post_Position_InitialLast }
};

static const uint8_t
ScoreTqReqEnqueueMrsp_Map[] = {
  4, 5, 0, 0, 1, 0, 2, 3, 1, 0, 2, 3, 4, 5, 0, 0, 1, 0, 2, 3, 1, 0, 2, 3
};

static size_t ScoreTqReqEnqueueMrsp_Scope( void *arg, char *buf, size_t n )
{
  ScoreTqReqEnqueueMrsp_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( ScoreTqReqEnqueueMrsp_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture ScoreTqReqEnqueueMrsp_Fixture = {
  .setup = ScoreTqReqEnqueueMrsp_Setup_Wrap,
  .stop = NULL,
  .teardown = ScoreTqReqEnqueueMrsp_Teardown_Wrap,
  .scope = ScoreTqReqEnqueueMrsp_Scope,
  .initial_context = &ScoreTqReqEnqueueMrsp_Instance
};

static inline ScoreTqReqEnqueueMrsp_Entry ScoreTqReqEnqueueMrsp_PopEntry(
  ScoreTqReqEnqueueMrsp_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return ScoreTqReqEnqueueMrsp_Entries[
    ScoreTqReqEnqueueMrsp_Map[ index ]
  ];
}

static void ScoreTqReqEnqueueMrsp_TestVariant(
  ScoreTqReqEnqueueMrsp_Context *ctx
)
{
  ScoreTqReqEnqueueMrsp_Pre_EligibleScheduler_Prepare(
    ctx,
    ctx->Map.pcs[ 0 ]
  );
  ScoreTqReqEnqueueMrsp_Pre_QueueEligible_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  ScoreTqReqEnqueueMrsp_Pre_QueueIneligible_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  ScoreTqReqEnqueueMrsp_Action( ctx );
  ScoreTqReqEnqueueMrsp_Post_Position_Check(
    ctx,
    ctx->Map.entry.Post_Position
  );
}

static T_fixture_node ScoreTqReqEnqueueMrsp_Node;

static T_remark ScoreTqReqEnqueueMrsp_Remark = {
  .next = NULL,
  .remark = "ScoreTqReqEnqueueMrsp"
};

void ScoreTqReqEnqueueMrsp_Run( TQContext *tq_ctx )
{
  ScoreTqReqEnqueueMrsp_Context *ctx;

  ctx = &ScoreTqReqEnqueueMrsp_Instance;
  ctx->tq_ctx = tq_ctx;

  ctx = T_push_fixture(
    &ScoreTqReqEnqueueMrsp_Node,
    &ScoreTqReqEnqueueMrsp_Fixture
  );
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = ScoreTqReqEnqueueMrsp_Pre_EligibleScheduler_Home;
    ctx->Map.pcs[ 0 ] < ScoreTqReqEnqueueMrsp_Pre_EligibleScheduler_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = ScoreTqReqEnqueueMrsp_Pre_QueueEligible_None;
      ctx->Map.pcs[ 1 ] < ScoreTqReqEnqueueMrsp_Pre_QueueEligible_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = ScoreTqReqEnqueueMrsp_Pre_QueueIneligible_None;
        ctx->Map.pcs[ 2 ] < ScoreTqReqEnqueueMrsp_Pre_QueueIneligible_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        ctx->Map.entry = ScoreTqReqEnqueueMrsp_PopEntry( ctx );
        ScoreTqReqEnqueueMrsp_TestVariant( ctx );
      }
    }
  }

  T_add_remark( &ScoreTqReqEnqueueMrsp_Remark );
  T_pop_fixture();
}

/** @} */
