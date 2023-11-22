/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreTqReqFlushPriorityInherit
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

#include "tr-tq-flush-priority-inherit.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreTqReqFlushPriorityInherit \
 *   spec:/score/tq/req/flush-priority-inherit
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Priority_NA : 1;
  uint8_t Pre_Queue_NA : 1;
  uint8_t Post_Extract : 2;
  uint8_t Post_PriorityUpdate : 2;
} ScoreTqReqFlushPriorityInherit_Entry;

/**
 * @brief Test context for spec:/score/tq/req/flush-priority-inherit test case.
 */
typedef struct {
  /**
   * @brief This member contains the call within ISR request.
   */
  CallWithinISRRequest request;

  /**
   * @brief If this member is true, then a minimum priority of the owner of the
   *   thread queue shall be inherited from a thread enqueued on the thread
   *   queue.
   */
  bool minimum;

  /**
   * @brief This member contains a copy of the corresponding
   *   ScoreTqReqFlushPriorityInherit_Run() parameter.
   */
  TQContext *tq_ctx;

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 2 ];

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
    ScoreTqReqFlushPriorityInherit_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} ScoreTqReqFlushPriorityInherit_Context;

static ScoreTqReqFlushPriorityInherit_Context
  ScoreTqReqFlushPriorityInherit_Instance;

static const char * const ScoreTqReqFlushPriorityInherit_PreDesc_Priority[] = {
  "Minimum",
  "NotMinimum",
  "NA"
};

static const char * const ScoreTqReqFlushPriorityInherit_PreDesc_Queue[] = {
  "Empty",
  "NonEmpty",
  "NA"
};

static const char * const * const ScoreTqReqFlushPriorityInherit_PreDesc[] = {
  ScoreTqReqFlushPriorityInherit_PreDesc_Priority,
  ScoreTqReqFlushPriorityInherit_PreDesc_Queue,
  NULL
};

typedef ScoreTqReqFlushPriorityInherit_Context Context;

static const T_scheduler_event *GetUnblock( Context *ctx, size_t *index )
{
  return TQGetNextUnblock( ctx->tq_ctx, index );
}

static const T_scheduler_event *GetPriorityUpdate( Context *ctx, size_t *index )
{
  return T_scheduler_next(
    &ctx->tq_ctx->scheduler_log.header,
    T_SCHEDULER_UPDATE_PRIORITY,
    index
  );
}

static const rtems_tcb *GetTCB( Context *ctx, TQWorkerKind worker )
{
  return ctx->tq_ctx->worker_tcb[ worker ];
}

static void Flush( void *arg )
{
  Context *ctx;

  ctx = arg;
  TQSchedulerRecordStart( ctx->tq_ctx );
  TQFlush( ctx->tq_ctx, true );
}

static void SchedulerEvent(
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
    ctx->request.handler = Flush;
    ctx->request.arg = ctx;
    CallWithinISRSubmit( &ctx->request );
    T_scheduler_set_event_handler( NULL, NULL );
  }
}

static void ScoreTqReqFlushPriorityInherit_Pre_Priority_Prepare(
  ScoreTqReqFlushPriorityInherit_Context     *ctx,
  ScoreTqReqFlushPriorityInherit_Pre_Priority state
)
{
  switch ( state ) {
    case ScoreTqReqFlushPriorityInherit_Pre_Priority_Minimum: {
      /*
       * While a minimum priority of the owner of the thread queue is inherited
       * from a thread enqueued on the thread queue.
       */
      ctx->minimum = true;
      break;
    }

    case ScoreTqReqFlushPriorityInherit_Pre_Priority_NotMinimum: {
      /*
       * While no minimum priority of the owner of the thread queue is
       * inherited from a thread enqueued on the thread queue.
       */
      ctx->minimum = false;
      break;
    }

    case ScoreTqReqFlushPriorityInherit_Pre_Priority_NA:
      break;
  }
}

static void ScoreTqReqFlushPriorityInherit_Pre_Queue_Prepare(
  ScoreTqReqFlushPriorityInherit_Context  *ctx,
  ScoreTqReqFlushPriorityInherit_Pre_Queue state
)
{
  switch ( state ) {
    case ScoreTqReqFlushPriorityInherit_Pre_Queue_Empty: {
      /*
       * While the thread queue is empty.
       */
      ctx->tq_ctx->how_many = 0;
      break;
    }

    case ScoreTqReqFlushPriorityInherit_Pre_Queue_NonEmpty: {
      /*
       * While the thread queue has at least one enqueued thread.
       */
      ctx->tq_ctx->how_many = 3;
      break;
    }

    case ScoreTqReqFlushPriorityInherit_Pre_Queue_NA:
      break;
  }
}

static void ScoreTqReqFlushPriorityInherit_Post_Extract_Check(
  ScoreTqReqFlushPriorityInherit_Context     *ctx,
  ScoreTqReqFlushPriorityInherit_Post_Extract state
)
{
  size_t                   i;
  const T_scheduler_event *event;

  i = 0;

  switch ( state ) {
    case ScoreTqReqFlushPriorityInherit_Post_Extract_Nop: {
      /*
       * No operation shall be performed.
       */
      /* Event receive */
      T_eq_ptr( GetUnblock( ctx, &i )->thread, GetTCB( ctx, TQ_BLOCKER_A ) );
      T_eq_ptr( GetPriorityUpdate( ctx, &i ), &T_scheduler_event_null );
      break;
    }

    case ScoreTqReqFlushPriorityInherit_Post_Extract_All: {
      /*
       * The enqueued threads of the thread queue shall be extracted in
       * priority order for each priority queue associated with a scheduler.
       * The priority queues of the thread queue shall be accessed in FIFO
       * order.
       */
      event = GetUnblock( ctx, &i );
      T_eq_ptr( event->executing, NULL );
      T_eq_ptr( event->thread, GetTCB( ctx, TQ_BLOCKER_D ) );

      event = GetUnblock( ctx, &i );
      T_eq_ptr( event->executing, NULL );
      T_eq_ptr( event->thread, GetTCB( ctx, TQ_BLOCKER_C ) );

      if ( ctx->minimum ) {
        /*
         * This priority update is carried out by
         * _Thread_queue_Flush_critical().
         */
        event = GetPriorityUpdate( ctx, &i );
        T_eq_ptr( event->executing, NULL );
        T_eq_ptr( event->thread, GetTCB( ctx, TQ_BLOCKER_A ) );
      }

      event = GetUnblock( ctx, &i );
      T_eq_ptr( event->executing, GetTCB( ctx, TQ_BLOCKER_B ) );
      T_eq_ptr( event->thread, GetTCB( ctx, TQ_BLOCKER_B ) );

      if ( ctx->minimum && rtems_configuration_get_maximum_processors() > 1 ) {
        /*
         * This superfluous priority update is carried out by
         * _Thread_queue_Enqueue() since TQ_BLOCKER_B would have inherited its
         * priority for scheduler B to TQ_BLOCKER_A if it were not flushed from
         * the thread queue.
         */
        event = GetPriorityUpdate( ctx, &i );
        T_eq_ptr( event->executing, GetTCB( ctx, TQ_BLOCKER_B ) );
        T_eq_ptr( event->thread, GetTCB( ctx, TQ_BLOCKER_A ) );
      }

      event = GetPriorityUpdate( ctx, &i );
      T_eq_ptr( event, &T_scheduler_event_null );

      T_eq_u32(
        GetPriority( ctx->tq_ctx->worker_id[ TQ_BLOCKER_A ] ),
        PRIO_HIGH
      );
      break;
    }

    case ScoreTqReqFlushPriorityInherit_Post_Extract_NA:
      break;
  }
}

static void ScoreTqReqFlushPriorityInherit_Post_PriorityUpdate_Check(
  ScoreTqReqFlushPriorityInherit_Context            *ctx,
  ScoreTqReqFlushPriorityInherit_Post_PriorityUpdate state
)
{
  switch ( state ) {
    case ScoreTqReqFlushPriorityInherit_Post_PriorityUpdate_No: {
      /*
       * The current priority of the owner of the thread queue shall not be
       * updated by the thread queue flush operation.
       */
      /* Checked by ``Extract`` post-condition state ``Nop`` */
      break;
    }

    case ScoreTqReqFlushPriorityInherit_Post_PriorityUpdate_Yes: {
      /*
       * The current priority of the owner of the thread queue shall be updated
       * by the thread queue flush operation to reflect the loss of inherited
       * priorities of the flushed threads.
       */
      /* Checked by ``Extract`` post-condition state ``All`` */
      break;
    }

    case ScoreTqReqFlushPriorityInherit_Post_PriorityUpdate_NA:
      break;
  }
}

static void ScoreTqReqFlushPriorityInherit_Setup(
  ScoreTqReqFlushPriorityInherit_Context *ctx
)
{
  TQReset( ctx->tq_ctx );
  TQSetPriority( ctx->tq_ctx, TQ_BLOCKER_A, PRIO_HIGH );
}

static void ScoreTqReqFlushPriorityInherit_Setup_Wrap( void *arg )
{
  ScoreTqReqFlushPriorityInherit_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  ScoreTqReqFlushPriorityInherit_Setup( ctx );
}

static void ScoreTqReqFlushPriorityInherit_Teardown(
  ScoreTqReqFlushPriorityInherit_Context *ctx
)
{
  TQReset( ctx->tq_ctx );
}

static void ScoreTqReqFlushPriorityInherit_Teardown_Wrap( void *arg )
{
  ScoreTqReqFlushPriorityInherit_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  ScoreTqReqFlushPriorityInherit_Teardown( ctx );
}

static void ScoreTqReqFlushPriorityInherit_Action(
  ScoreTqReqFlushPriorityInherit_Context *ctx
)
{
  TQSend( ctx->tq_ctx, TQ_BLOCKER_A, TQ_EVENT_ENQUEUE_PREPARE );

  if ( ctx->tq_ctx->how_many > 0 ) {
    if ( ctx->minimum ) {
      if ( rtems_configuration_get_maximum_processors() > 1 ) {
        TQSetScheduler( ctx->tq_ctx, TQ_BLOCKER_B, SCHEDULER_B_ID, PRIO_HIGH );
      } else {
        TQSetScheduler( ctx->tq_ctx, TQ_BLOCKER_B, SCHEDULER_A_ID, PRIO_HIGH );
      }

      TQSetPriority( ctx->tq_ctx, TQ_BLOCKER_C, PRIO_VERY_HIGH );
      TQSetPriority( ctx->tq_ctx, TQ_BLOCKER_D, PRIO_ULTRA_HIGH );
      TQSend( ctx->tq_ctx, TQ_BLOCKER_C, TQ_EVENT_ENQUEUE );
      TQSend( ctx->tq_ctx, TQ_BLOCKER_D, TQ_EVENT_ENQUEUE );
    } else {
      TQSetScheduler( ctx->tq_ctx, TQ_BLOCKER_B, SCHEDULER_A_ID, PRIO_HIGH );
      TQSetPriority( ctx->tq_ctx, TQ_BLOCKER_C, PRIO_HIGH );
      TQSetPriority( ctx->tq_ctx, TQ_BLOCKER_D, PRIO_HIGH );

      TQSend( ctx->tq_ctx, TQ_BLOCKER_D, TQ_EVENT_ENQUEUE );
      TQSend( ctx->tq_ctx, TQ_BLOCKER_C, TQ_EVENT_ENQUEUE );
    }

    T_scheduler_set_event_handler( SchedulerEvent, ctx );
    TQSendAndWaitForExecutionStop(
      ctx->tq_ctx,
      TQ_BLOCKER_B,
      TQ_EVENT_ENQUEUE
    );
  } else {
    TQSchedulerRecordStart( ctx->tq_ctx );
    TQSend( ctx->tq_ctx, TQ_BLOCKER_A, TQ_EVENT_FLUSH_ALL );
  }

  TQSchedulerRecordStop( ctx->tq_ctx );
  TQSend( ctx->tq_ctx, TQ_BLOCKER_A, TQ_EVENT_ENQUEUE_DONE );
}

static const ScoreTqReqFlushPriorityInherit_Entry
ScoreTqReqFlushPriorityInherit_Entries[] = {
  { 0, 1, 0, ScoreTqReqFlushPriorityInherit_Post_Extract_Nop,
    ScoreTqReqFlushPriorityInherit_Post_PriorityUpdate_No },
  { 0, 0, 0, ScoreTqReqFlushPriorityInherit_Post_Extract_All,
    ScoreTqReqFlushPriorityInherit_Post_PriorityUpdate_Yes },
  { 0, 0, 0, ScoreTqReqFlushPriorityInherit_Post_Extract_All,
    ScoreTqReqFlushPriorityInherit_Post_PriorityUpdate_No }
};

static const uint8_t
ScoreTqReqFlushPriorityInherit_Map[] = {
  0, 1, 0, 2
};

static size_t ScoreTqReqFlushPriorityInherit_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  ScoreTqReqFlushPriorityInherit_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      ScoreTqReqFlushPriorityInherit_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture ScoreTqReqFlushPriorityInherit_Fixture = {
  .setup = ScoreTqReqFlushPriorityInherit_Setup_Wrap,
  .stop = NULL,
  .teardown = ScoreTqReqFlushPriorityInherit_Teardown_Wrap,
  .scope = ScoreTqReqFlushPriorityInherit_Scope,
  .initial_context = &ScoreTqReqFlushPriorityInherit_Instance
};

static inline ScoreTqReqFlushPriorityInherit_Entry
ScoreTqReqFlushPriorityInherit_PopEntry(
  ScoreTqReqFlushPriorityInherit_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return ScoreTqReqFlushPriorityInherit_Entries[
    ScoreTqReqFlushPriorityInherit_Map[ index ]
  ];
}

static void ScoreTqReqFlushPriorityInherit_SetPreConditionStates(
  ScoreTqReqFlushPriorityInherit_Context *ctx
)
{
  if ( ctx->Map.entry.Pre_Priority_NA ) {
    ctx->Map.pcs[ 0 ] = ScoreTqReqFlushPriorityInherit_Pre_Priority_NA;
  } else {
    ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  }

  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
}

static void ScoreTqReqFlushPriorityInherit_TestVariant(
  ScoreTqReqFlushPriorityInherit_Context *ctx
)
{
  ScoreTqReqFlushPriorityInherit_Pre_Priority_Prepare(
    ctx,
    ctx->Map.pcs[ 0 ]
  );
  ScoreTqReqFlushPriorityInherit_Pre_Queue_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  ScoreTqReqFlushPriorityInherit_Action( ctx );
  ScoreTqReqFlushPriorityInherit_Post_Extract_Check(
    ctx,
    ctx->Map.entry.Post_Extract
  );
  ScoreTqReqFlushPriorityInherit_Post_PriorityUpdate_Check(
    ctx,
    ctx->Map.entry.Post_PriorityUpdate
  );
}

static T_fixture_node ScoreTqReqFlushPriorityInherit_Node;

static T_remark ScoreTqReqFlushPriorityInherit_Remark = {
  .next = NULL,
  .remark = "ScoreTqReqFlushPriorityInherit"
};

void ScoreTqReqFlushPriorityInherit_Run( TQContext *tq_ctx )
{
  ScoreTqReqFlushPriorityInherit_Context *ctx;

  ctx = &ScoreTqReqFlushPriorityInherit_Instance;
  ctx->tq_ctx = tq_ctx;

  ctx = T_push_fixture(
    &ScoreTqReqFlushPriorityInherit_Node,
    &ScoreTqReqFlushPriorityInherit_Fixture
  );
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = ScoreTqReqFlushPriorityInherit_Pre_Priority_Minimum;
    ctx->Map.pci[ 0 ] < ScoreTqReqFlushPriorityInherit_Pre_Priority_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = ScoreTqReqFlushPriorityInherit_Pre_Queue_Empty;
      ctx->Map.pci[ 1 ] < ScoreTqReqFlushPriorityInherit_Pre_Queue_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      ctx->Map.entry = ScoreTqReqFlushPriorityInherit_PopEntry( ctx );
      ScoreTqReqFlushPriorityInherit_SetPreConditionStates( ctx );
      ScoreTqReqFlushPriorityInherit_TestVariant( ctx );
    }
  }

  T_add_remark( &ScoreTqReqFlushPriorityInherit_Remark );
  T_pop_fixture();
}

/** @} */
