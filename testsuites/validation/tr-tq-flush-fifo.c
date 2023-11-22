/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreTqReqFlushFifo
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

#include "tr-tq-flush-fifo.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreTqReqFlushFifo spec:/score/tq/req/flush-fifo
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_MayStop_NA : 1;
  uint8_t Pre_QueueEmpty_NA : 1;
  uint8_t Pre_Stop_NA : 1;
  uint8_t Pre_WaitState_NA : 1;
  uint8_t Post_Operation : 2;
} ScoreTqReqFlushFifo_Entry;

/**
 * @brief Test context for spec:/score/tq/req/flush-fifo test case.
 */
typedef struct {
  /**
   * @brief If this member is true, then the flush filter shall return NULL.
   */
  bool stop;

  /**
   * @brief If this member is true, then the least recently enqueued thread
   *   shall be in the intend to block wait state.
   */
  bool intend_to_block;

  /**
   * @brief This member contains the call within ISR request.
   */
  CallWithinISRRequest request;

  /**
   * @brief This member contains a copy of the corresponding
   *   ScoreTqReqFlushFifo_Run() parameter.
   */
  TQContext *tq_ctx;

  /**
   * @brief This member contains a copy of the corresponding
   *   ScoreTqReqFlushFifo_Run() parameter.
   */
  bool may_stop;

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 4 ];

    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 4 ];

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
    ScoreTqReqFlushFifo_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} ScoreTqReqFlushFifo_Context;

static ScoreTqReqFlushFifo_Context
  ScoreTqReqFlushFifo_Instance;

static const char * const ScoreTqReqFlushFifo_PreDesc_MayStop[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const ScoreTqReqFlushFifo_PreDesc_QueueEmpty[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const ScoreTqReqFlushFifo_PreDesc_Stop[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const ScoreTqReqFlushFifo_PreDesc_WaitState[] = {
  "Blocked",
  "IntendToBlock",
  "NA"
};

static const char * const * const ScoreTqReqFlushFifo_PreDesc[] = {
  ScoreTqReqFlushFifo_PreDesc_MayStop,
  ScoreTqReqFlushFifo_PreDesc_QueueEmpty,
  ScoreTqReqFlushFifo_PreDesc_Stop,
  ScoreTqReqFlushFifo_PreDesc_WaitState,
  NULL
};

typedef ScoreTqReqFlushFifo_Context Context;

static const T_scheduler_event *GetUnblock( Context *ctx, size_t *index )
{
  return TQGetNextUnblock( ctx->tq_ctx, index );
}

static const rtems_tcb *GetTCB( Context *ctx, TQWorkerKind worker )
{
  return ctx->tq_ctx->worker_tcb[ worker ];
}

static void BlockerAFlush( Context *ctx )
{
  TQSchedulerRecordStart( ctx->tq_ctx );

  if ( ctx->stop ) {
    TQSend( ctx->tq_ctx, TQ_BLOCKER_A, TQ_EVENT_FLUSH_PARTIAL );
  } else {
    TQSend( ctx->tq_ctx, TQ_BLOCKER_A, TQ_EVENT_FLUSH_ALL );
  }
}

static void InterruptFlush( void *arg )
{
  Context *ctx;

  ctx = arg;
  TQSchedulerRecordStart( ctx->tq_ctx );
  TQFlush( ctx->tq_ctx, !ctx->stop );
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
    T_scheduler_set_event_handler( NULL, NULL );
    ctx->request.handler = InterruptFlush;
    CallWithinISRSubmit( &ctx->request );
  }
}

static uint32_t CheckExtractions( Context *ctx )
{
  uint32_t                 extracted_threads;
  size_t                   i;
  const T_scheduler_event *event;

  extracted_threads = 0;
  i = 0;

  if ( !ctx->intend_to_block ) {
    /* Event receive */
    T_eq_ptr( GetUnblock( ctx, &i )->thread, GetTCB( ctx, TQ_BLOCKER_A ) );
  }

  event = GetUnblock( ctx, &i );

  if ( event != &T_scheduler_event_null ) {
    if ( ctx->intend_to_block ) {
      T_eq_ptr( event->executing, NULL );
    } else {
      T_eq_ptr( event->executing, GetTCB( ctx, TQ_BLOCKER_A ) );
    }

    T_eq_ptr( event->thread, GetTCB( ctx, TQ_BLOCKER_B ) );
    ++extracted_threads;
  }

  event = GetUnblock( ctx, &i );

  if ( event != &T_scheduler_event_null ) {
    if ( ctx->intend_to_block ) {
      T_eq_ptr( event->executing, NULL );
    } else {
      T_eq_ptr( event->executing, GetTCB( ctx, TQ_BLOCKER_A ) );
    }

    T_eq_ptr( event->thread, GetTCB( ctx, TQ_BLOCKER_C ) );
    ++extracted_threads;
  }

  event = GetUnblock( ctx, &i );

  if ( event != &T_scheduler_event_null ) {
    if ( ctx->intend_to_block ) {
      T_eq_ptr( event->executing, GetTCB( ctx, TQ_BLOCKER_D ) );
    } else {
      T_eq_ptr( event->executing, GetTCB( ctx, TQ_BLOCKER_A ) );
    }

    T_eq_ptr( event->thread, GetTCB( ctx, TQ_BLOCKER_D ) );
    ++extracted_threads;
  }

  T_eq_ptr( GetUnblock( ctx, &i ), &T_scheduler_event_null );
  T_eq_u32( extracted_threads, ctx->tq_ctx->flush_count );

  return extracted_threads;
}

static void ScoreTqReqFlushFifo_Pre_MayStop_Prepare(
  ScoreTqReqFlushFifo_Context    *ctx,
  ScoreTqReqFlushFifo_Pre_MayStop state
)
{
  switch ( state ) {
    case ScoreTqReqFlushFifo_Pre_MayStop_Yes: {
      /*
       * Where the flush filter may return NULL.
       */
      if ( !ctx->may_stop ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTqReqFlushFifo_Pre_MayStop_No: {
      /*
       * Where the flush filter does not return NULL.
       */
      if ( ctx->may_stop ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTqReqFlushFifo_Pre_MayStop_NA:
      break;
  }
}

static void ScoreTqReqFlushFifo_Pre_QueueEmpty_Prepare(
  ScoreTqReqFlushFifo_Context       *ctx,
  ScoreTqReqFlushFifo_Pre_QueueEmpty state
)
{
  switch ( state ) {
    case ScoreTqReqFlushFifo_Pre_QueueEmpty_Yes: {
      /*
       * While the thread queue is empty.
       */
      ctx->tq_ctx->how_many = 0;
      break;
    }

    case ScoreTqReqFlushFifo_Pre_QueueEmpty_No: {
      /*
       * While the thread queue has at least one enqueued thread.
       */
      ctx->tq_ctx->how_many = 3;
      break;
    }

    case ScoreTqReqFlushFifo_Pre_QueueEmpty_NA:
      break;
  }
}

static void ScoreTqReqFlushFifo_Pre_Stop_Prepare(
  ScoreTqReqFlushFifo_Context *ctx,
  ScoreTqReqFlushFifo_Pre_Stop state
)
{
  switch ( state ) {
    case ScoreTqReqFlushFifo_Pre_Stop_Yes: {
      /*
       * While the flush filter returns NULL for an enqueued thread.
       */
      ctx->stop = true;
      break;
    }

    case ScoreTqReqFlushFifo_Pre_Stop_No: {
      /*
       * While the flush filter does not return NULL for an enqueued thread.
       */
      ctx->stop = false;
      break;
    }

    case ScoreTqReqFlushFifo_Pre_Stop_NA:
      break;
  }
}

static void ScoreTqReqFlushFifo_Pre_WaitState_Prepare(
  ScoreTqReqFlushFifo_Context      *ctx,
  ScoreTqReqFlushFifo_Pre_WaitState state
)
{
  switch ( state ) {
    case ScoreTqReqFlushFifo_Pre_WaitState_Blocked: {
      /*
       * While the least recently enqueued thread on the thread queue is in the
       * blocked wait state.
       */
      ctx->intend_to_block = false;
      break;
    }

    case ScoreTqReqFlushFifo_Pre_WaitState_IntendToBlock: {
      /*
       * While the least recently enqueued thread on the thread queue is in the
       * intend to block wait state.
       */
      ctx->intend_to_block = true;
      break;
    }

    case ScoreTqReqFlushFifo_Pre_WaitState_NA:
      break;
  }
}

static void ScoreTqReqFlushFifo_Post_Operation_Check(
  ScoreTqReqFlushFifo_Context       *ctx,
  ScoreTqReqFlushFifo_Post_Operation state
)
{
  size_t   i;
  uint32_t extracted_threads;

  switch ( state ) {
    case ScoreTqReqFlushFifo_Post_Operation_Nop: {
      /*
       * No thread queue extraction operation shall be performed.
       */
      /* Event receive */
      i = 0;
      T_eq_ptr( GetUnblock( ctx, &i )->thread, GetTCB( ctx, TQ_BLOCKER_A ) );
      T_eq_ptr( GetUnblock( ctx, &i ), &T_scheduler_event_null );
      break;
    }

    case ScoreTqReqFlushFifo_Post_Operation_ExtractAll: {
      /*
       * The enqueued threads shall be extracted from the thread queue in FIFO
       * order.
       */
      extracted_threads = CheckExtractions( ctx );
      T_eq_sz( extracted_threads, ctx->tq_ctx->how_many );
      break;
    }

    case ScoreTqReqFlushFifo_Post_Operation_ExtractPartial: {
      /*
       * The enqueued threads which precede in FIFO order the enqueued thread
       * for which the flush filter returned NULL shall be extracted from the
       * thread queue in FIFO order.
       */
      extracted_threads = CheckExtractions( ctx );
      T_lt_sz( extracted_threads, ctx->tq_ctx->how_many );
      break;
    }

    case ScoreTqReqFlushFifo_Post_Operation_NA:
      break;
  }
}

static void ScoreTqReqFlushFifo_Setup( ScoreTqReqFlushFifo_Context *ctx )
{
  ctx->request.arg = ctx;
  TQReset( ctx->tq_ctx );
  TQSetPriority( ctx->tq_ctx, TQ_BLOCKER_A, PRIO_ULTRA_HIGH );
  TQSetPriority( ctx->tq_ctx, TQ_BLOCKER_B, PRIO_VERY_HIGH );
  TQSetPriority( ctx->tq_ctx, TQ_BLOCKER_C, PRIO_HIGH );
  TQSetPriority( ctx->tq_ctx, TQ_BLOCKER_D, PRIO_HIGH );
}

static void ScoreTqReqFlushFifo_Setup_Wrap( void *arg )
{
  ScoreTqReqFlushFifo_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  ScoreTqReqFlushFifo_Setup( ctx );
}

static void ScoreTqReqFlushFifo_Teardown( ScoreTqReqFlushFifo_Context *ctx )
{
  TQReset( ctx->tq_ctx );
}

static void ScoreTqReqFlushFifo_Teardown_Wrap( void *arg )
{
  ScoreTqReqFlushFifo_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  ScoreTqReqFlushFifo_Teardown( ctx );
}

static void ScoreTqReqFlushFifo_Action( ScoreTqReqFlushFifo_Context *ctx )
{
  uint32_t flush_count;

  TQSend( ctx->tq_ctx, TQ_BLOCKER_A, TQ_EVENT_ENQUEUE_PREPARE );

  if ( ctx->tq_ctx->how_many > 0 ) {
    TQSend( ctx->tq_ctx, TQ_BLOCKER_B, TQ_EVENT_ENQUEUE );
    TQSend( ctx->tq_ctx, TQ_BLOCKER_C, TQ_EVENT_ENQUEUE );

    if ( ctx->intend_to_block ) {
      T_scheduler_set_event_handler( SchedulerEvent, ctx );
    }

    TQSend( ctx->tq_ctx, TQ_BLOCKER_D, TQ_EVENT_ENQUEUE );

    if ( !ctx->intend_to_block ) {
      BlockerAFlush( ctx );
    }
  } else {
    BlockerAFlush( ctx );
  }

  flush_count = ctx->tq_ctx->flush_count;
  TQSchedulerRecordStop( ctx->tq_ctx );
  TQSend( ctx->tq_ctx, TQ_BLOCKER_A, TQ_EVENT_FLUSH_ALL );
  TQSend( ctx->tq_ctx, TQ_BLOCKER_A, TQ_EVENT_ENQUEUE_DONE );
  ctx->tq_ctx->flush_count = flush_count;
}

static const ScoreTqReqFlushFifo_Entry
ScoreTqReqFlushFifo_Entries[] = {
  { 0, 0, 0, 1, 1, ScoreTqReqFlushFifo_Post_Operation_Nop },
  { 0, 0, 0, 0, 0, ScoreTqReqFlushFifo_Post_Operation_ExtractAll },
  { 0, 0, 0, 0, 0, ScoreTqReqFlushFifo_Post_Operation_ExtractPartial },
  { 1, 0, 0, 0, 0, ScoreTqReqFlushFifo_Post_Operation_NA }
};

static const uint8_t
ScoreTqReqFlushFifo_Map[] = {
  0, 0, 0, 0, 2, 2, 1, 1, 0, 0, 0, 0, 3, 3, 1, 1
};

static size_t ScoreTqReqFlushFifo_Scope( void *arg, char *buf, size_t n )
{
  ScoreTqReqFlushFifo_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( ScoreTqReqFlushFifo_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture ScoreTqReqFlushFifo_Fixture = {
  .setup = ScoreTqReqFlushFifo_Setup_Wrap,
  .stop = NULL,
  .teardown = ScoreTqReqFlushFifo_Teardown_Wrap,
  .scope = ScoreTqReqFlushFifo_Scope,
  .initial_context = &ScoreTqReqFlushFifo_Instance
};

static const uint8_t ScoreTqReqFlushFifo_Weights[] = {
  8, 4, 2, 1
};

static void ScoreTqReqFlushFifo_Skip(
  ScoreTqReqFlushFifo_Context *ctx,
  size_t                       index
)
{
  switch ( index + 1 ) {
    case 1:
      ctx->Map.pci[ 1 ] = ScoreTqReqFlushFifo_Pre_QueueEmpty_NA - 1;
      /* Fall through */
    case 2:
      ctx->Map.pci[ 2 ] = ScoreTqReqFlushFifo_Pre_Stop_NA - 1;
      /* Fall through */
    case 3:
      ctx->Map.pci[ 3 ] = ScoreTqReqFlushFifo_Pre_WaitState_NA - 1;
      break;
  }
}

static inline ScoreTqReqFlushFifo_Entry ScoreTqReqFlushFifo_PopEntry(
  ScoreTqReqFlushFifo_Context *ctx
)
{
  size_t index;

  if ( ctx->Map.skip ) {
    size_t i;

    ctx->Map.skip = false;
    index = 0;

    for ( i = 0; i < 4; ++i ) {
      index += ScoreTqReqFlushFifo_Weights[ i ] * ctx->Map.pci[ i ];
    }
  } else {
    index = ctx->Map.index;
  }

  ctx->Map.index = index + 1;

  return ScoreTqReqFlushFifo_Entries[
    ScoreTqReqFlushFifo_Map[ index ]
  ];
}

static void ScoreTqReqFlushFifo_SetPreConditionStates(
  ScoreTqReqFlushFifo_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];

  if ( ctx->Map.entry.Pre_Stop_NA ) {
    ctx->Map.pcs[ 2 ] = ScoreTqReqFlushFifo_Pre_Stop_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }

  if ( ctx->Map.entry.Pre_WaitState_NA ) {
    ctx->Map.pcs[ 3 ] = ScoreTqReqFlushFifo_Pre_WaitState_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }
}

static void ScoreTqReqFlushFifo_TestVariant( ScoreTqReqFlushFifo_Context *ctx )
{
  ScoreTqReqFlushFifo_Pre_MayStop_Prepare( ctx, ctx->Map.pcs[ 0 ] );

  if ( ctx->Map.skip ) {
    ScoreTqReqFlushFifo_Skip( ctx, 0 );
    return;
  }

  ScoreTqReqFlushFifo_Pre_QueueEmpty_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  ScoreTqReqFlushFifo_Pre_Stop_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  ScoreTqReqFlushFifo_Pre_WaitState_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  ScoreTqReqFlushFifo_Action( ctx );
  ScoreTqReqFlushFifo_Post_Operation_Check(
    ctx,
    ctx->Map.entry.Post_Operation
  );
}

static T_fixture_node ScoreTqReqFlushFifo_Node;

static T_remark ScoreTqReqFlushFifo_Remark = {
  .next = NULL,
  .remark = "ScoreTqReqFlushFifo"
};

void ScoreTqReqFlushFifo_Run( TQContext *tq_ctx, bool may_stop )
{
  ScoreTqReqFlushFifo_Context *ctx;

  ctx = &ScoreTqReqFlushFifo_Instance;
  ctx->tq_ctx = tq_ctx;
  ctx->may_stop = may_stop;

  ctx = T_push_fixture(
    &ScoreTqReqFlushFifo_Node,
    &ScoreTqReqFlushFifo_Fixture
  );
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;
  ctx->Map.skip = false;

  for (
    ctx->Map.pci[ 0 ] = ScoreTqReqFlushFifo_Pre_MayStop_Yes;
    ctx->Map.pci[ 0 ] < ScoreTqReqFlushFifo_Pre_MayStop_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = ScoreTqReqFlushFifo_Pre_QueueEmpty_Yes;
      ctx->Map.pci[ 1 ] < ScoreTqReqFlushFifo_Pre_QueueEmpty_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = ScoreTqReqFlushFifo_Pre_Stop_Yes;
        ctx->Map.pci[ 2 ] < ScoreTqReqFlushFifo_Pre_Stop_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = ScoreTqReqFlushFifo_Pre_WaitState_Blocked;
          ctx->Map.pci[ 3 ] < ScoreTqReqFlushFifo_Pre_WaitState_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          ctx->Map.entry = ScoreTqReqFlushFifo_PopEntry( ctx );

          if ( ctx->Map.entry.Skip ) {
            continue;
          }

          ScoreTqReqFlushFifo_SetPreConditionStates( ctx );
          ScoreTqReqFlushFifo_TestVariant( ctx );
        }
      }
    }
  }

  T_add_remark( &ScoreTqReqFlushFifo_Remark );
  T_pop_fixture();
}

/** @} */
