/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreTqReqSurrender
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

#include <rtems/score/smpbarrier.h>
#include <rtems/score/threadimpl.h>

#include "tr-tq-surrender.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreTqReqSurrender spec:/score/tq/req/surrender
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_HasOwner_NA : 1;
  uint8_t Pre_Discipline_NA : 1;
  uint8_t Pre_WaitState_NA : 1;
  uint8_t Post_Dequeue : 2;
  uint8_t Post_Unblock : 2;
} ScoreTqReqSurrender_Entry;

/**
 * @brief Test context for spec:/score/tq/req/surrender test case.
 */
typedef struct {
  /**
   * @brief This member contains the call within ISR request.
   */
  CallWithinISRRequest request;

  /**
   * @brief This member contains the barrier to synchronize the runner and the
   *   worker.
   */
  SMP_barrier_Control barrier;

  /**
   * @brief If this member is true, then the dequeued thread shall be in the
   *   intend to block wait state.
   */
  bool intend_to_block;

  /**
   * @brief If this member contains the expected counter of worker B.
   */
  uint32_t expected_blocker_b_counter;

  /**
   * @brief This member contains a copy of the corresponding
   *   ScoreTqReqSurrender_Run() parameter.
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
    ScoreTqReqSurrender_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} ScoreTqReqSurrender_Context;

static ScoreTqReqSurrender_Context
  ScoreTqReqSurrender_Instance;

static const char * const ScoreTqReqSurrender_PreDesc_HasOwner[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const ScoreTqReqSurrender_PreDesc_Discipline[] = {
  "FIFO",
  "Priority",
  "NA"
};

static const char * const ScoreTqReqSurrender_PreDesc_WaitState[] = {
  "Blocked",
  "IntendToBlock",
  "NA"
};

static const char * const * const ScoreTqReqSurrender_PreDesc[] = {
  ScoreTqReqSurrender_PreDesc_HasOwner,
  ScoreTqReqSurrender_PreDesc_Discipline,
  ScoreTqReqSurrender_PreDesc_WaitState,
  NULL
};

typedef ScoreTqReqSurrender_Context Context;

static const rtems_tcb *GetUnblock( Context *ctx, size_t *index )
{
  return TQGetNextUnblock( ctx->tq_ctx, index )->thread;
}

static const rtems_tcb *GetTCB( Context *ctx, TQWorkerKind worker )
{
  return ctx->tq_ctx->worker_tcb[ worker ];
}

static void Surrender( void *arg )
{
  Context       *ctx;
  Status_Control status;

  ctx = arg;
  TQSchedulerRecordStart( ctx->tq_ctx );

  status = TQSurrender( ctx->tq_ctx );
  T_eq_int( status, TQConvertStatus( ctx->tq_ctx, STATUS_SUCCESSFUL ) );

  TQSchedulerRecordStop( ctx->tq_ctx );
}

#if defined(RTEMS_SMP)
static void Delay( void *arg )
{
  Context          *ctx;
  SMP_barrier_State state;

  ctx = arg;
  _SMP_barrier_State_initialize( &state );

  /* B0 */
  _SMP_barrier_Wait( &ctx->barrier, &state, 2 );

  /* B1 */
  _SMP_barrier_Wait( &ctx->barrier, &state, 2 );
}
#endif

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
#if defined(RTEMS_SMP)
    ctx->request.handler = Delay;
#else
    ctx->request.handler = Surrender;
#endif
    CallWithinISRSubmit( &ctx->request );
  }
}

static void ScoreTqReqSurrender_Pre_HasOwner_Prepare(
  ScoreTqReqSurrender_Context     *ctx,
  ScoreTqReqSurrender_Pre_HasOwner state
)
{
  switch ( state ) {
    case ScoreTqReqSurrender_Pre_HasOwner_Yes: {
      /*
       * Where the thread queue has a previous owner thread.
       */
      if ( ctx->tq_ctx->get_owner == NULL ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTqReqSurrender_Pre_HasOwner_No: {
      /*
       * Where the thread queue has no owner threads.
       */
      if ( ctx->tq_ctx->get_owner != NULL ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTqReqSurrender_Pre_HasOwner_NA:
      break;
  }
}

static void ScoreTqReqSurrender_Pre_Discipline_Prepare(
  ScoreTqReqSurrender_Context       *ctx,
  ScoreTqReqSurrender_Pre_Discipline state
)
{
  switch ( state ) {
    case ScoreTqReqSurrender_Pre_Discipline_FIFO: {
      /*
       * Where the thread queue uses the FIFO discipline.
       */
      if ( ctx->tq_ctx->discipline != TQ_FIFO ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTqReqSurrender_Pre_Discipline_Priority: {
      /*
       * Where the thread queue uses the priority discipline.
       */
      if ( ctx->tq_ctx->discipline != TQ_PRIORITY ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTqReqSurrender_Pre_Discipline_NA:
      break;
  }
}

static void ScoreTqReqSurrender_Pre_WaitState_Prepare(
  ScoreTqReqSurrender_Context      *ctx,
  ScoreTqReqSurrender_Pre_WaitState state
)
{
  switch ( state ) {
    case ScoreTqReqSurrender_Pre_WaitState_Blocked: {
      /*
       * While the dequeued thread is in the blocked wait state.
       */
      ctx->intend_to_block = false;
      break;
    }

    case ScoreTqReqSurrender_Pre_WaitState_IntendToBlock: {
      /*
       * While the dequeued thread is in the intend to block wait state.
       */
      ctx->intend_to_block = true;
      break;
    }

    case ScoreTqReqSurrender_Pre_WaitState_NA:
      break;
  }
}

static void ScoreTqReqSurrender_Post_Dequeue_Check(
  ScoreTqReqSurrender_Context     *ctx,
  ScoreTqReqSurrender_Post_Dequeue state
)
{
  switch ( state ) {
    case ScoreTqReqSurrender_Post_Dequeue_FIFO: {
      /*
       * The first thread in FIFO order shall be dequeued from the thread
       * queue.
       */
      T_eq_u32( TQGetWorkerCounter( ctx->tq_ctx, TQ_BLOCKER_A ), 1 );
      T_eq_u32(
        TQGetWorkerCounter( ctx->tq_ctx, TQ_BLOCKER_B ),
        ctx->expected_blocker_b_counter
      );
      break;
    }

    case ScoreTqReqSurrender_Post_Dequeue_Priority: {
      /*
       * The first thread in priority order shall be dequeued from the thread
       * queue.
       */
      T_eq_u32( TQGetWorkerCounter( ctx->tq_ctx, TQ_BLOCKER_A ), 1 );
      T_eq_u32( TQGetWorkerCounter( ctx->tq_ctx, TQ_BLOCKER_B ), 2 );
      break;
    }

    case ScoreTqReqSurrender_Post_Dequeue_NA:
      break;
  }
}

static void ScoreTqReqSurrender_Post_Unblock_Check(
  ScoreTqReqSurrender_Context     *ctx,
  ScoreTqReqSurrender_Post_Unblock state
)
{
  size_t i;

  i = 0;

  switch ( state ) {
    case ScoreTqReqSurrender_Post_Unblock_Yes: {
      /*
       * The dequeued thread shall be unblocked by surrender operation.
       */
      T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_A ) );
      T_eq_ptr( GetUnblock( ctx, &i ), NULL );
      break;
    }

    case ScoreTqReqSurrender_Post_Unblock_No: {
      /*
       * The dequeued thread shall not be unblocked by surrender operation.
       */
      T_eq_ptr( GetUnblock( ctx, &i ), NULL );
      break;
    }

    case ScoreTqReqSurrender_Post_Unblock_NA:
      break;
  }
}

static void ScoreTqReqSurrender_Setup( ScoreTqReqSurrender_Context *ctx )
{
  ctx->request.arg = ctx;
  TQReset( ctx->tq_ctx );
  TQSetPriority( ctx->tq_ctx, TQ_BLOCKER_A, PRIO_VERY_HIGH );
  TQSetPriority( ctx->tq_ctx, TQ_BLOCKER_B, PRIO_HIGH );

  #if defined(RTEMS_SMP)
  /*
   * For the mutexes with priority ceiling protocol, we need a scheduler with
   * two processors to set up the intend to block wait state.
   */
  RemoveProcessor( SCHEDULER_B_ID, 1 );
  AddProcessor( SCHEDULER_A_ID, 1 );
  #endif
}

static void ScoreTqReqSurrender_Setup_Wrap( void *arg )
{
  ScoreTqReqSurrender_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  ScoreTqReqSurrender_Setup( ctx );
}

static void ScoreTqReqSurrender_Teardown( ScoreTqReqSurrender_Context *ctx )
{
  TQReset( ctx->tq_ctx );

  #if defined(RTEMS_SMP)
  RemoveProcessor( SCHEDULER_A_ID, 1 );
  AddProcessor( SCHEDULER_B_ID, 1 );
  #endif
}

static void ScoreTqReqSurrender_Teardown_Wrap( void *arg )
{
  ScoreTqReqSurrender_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  ScoreTqReqSurrender_Teardown( ctx );
}

static void ScoreTqReqSurrender_Action( ScoreTqReqSurrender_Context *ctx )
{
  Status_Control status;

  TQResetCounter( ctx->tq_ctx );
  ctx->expected_blocker_b_counter = 0;

  status = TQEnqueue( ctx->tq_ctx, TQ_NO_WAIT );
  T_eq_int( status, TQConvertStatus( ctx->tq_ctx, STATUS_SUCCESSFUL ) );

  if ( ctx->intend_to_block ) {
  #if defined(RTEMS_SMP)
    SMP_barrier_State state;
  #endif

    /*
     * In uniprocessor configurations, it is impossible to dequeue a thread
     * in FIFO order which is in the intend to block wait state.  Run this
     * test with just one worker.
     */
    if ( ctx->tq_ctx->discipline != TQ_FIFO ) {
      TQSendAndWaitForExecutionStop(
        ctx->tq_ctx,
        TQ_BLOCKER_B,
        TQ_EVENT_ENQUEUE
      );
      ctx->expected_blocker_b_counter = 2;
    }


  #if defined(RTEMS_SMP)
    _SMP_barrier_Control_initialize( &ctx->barrier );
    _SMP_barrier_State_initialize( &state );
  #endif

    T_scheduler_set_event_handler( SchedulerBlock, ctx );
    TQSend( ctx->tq_ctx, TQ_BLOCKER_A, TQ_EVENT_ENQUEUE );

  #if defined(RTEMS_SMP)
    /* B0 */
    _SMP_barrier_Wait( &ctx->barrier, &state, 2 );

    Surrender( ctx );

    /* B1 */
    _SMP_barrier_Wait( &ctx->barrier, &state, 2 );
  #endif
  } else {
    TQSend(
      ctx->tq_ctx,
      TQ_BLOCKER_A,
      TQ_EVENT_HELPER_A_SYNC | TQ_EVENT_ENQUEUE
    );
    TQSynchronizeRunner();
    TQWaitForExecutionStop( ctx->tq_ctx, TQ_BLOCKER_A );

    TQSend(
      ctx->tq_ctx,
      TQ_BLOCKER_B,
      TQ_EVENT_HELPER_A_SYNC | TQ_EVENT_ENQUEUE
    );
    TQSynchronizeRunner();
    TQWaitForExecutionStop( ctx->tq_ctx, TQ_BLOCKER_B );
    ctx->expected_blocker_b_counter = 2;

    Surrender( ctx );
  }

  TQSendAndWaitForExecutionStop(
    ctx->tq_ctx,
    TQ_BLOCKER_A,
    TQ_EVENT_SURRENDER
  );

  if ( ctx->expected_blocker_b_counter != 0 ) {
    TQSendAndWaitForExecutionStop(
      ctx->tq_ctx,
      TQ_BLOCKER_B,
      TQ_EVENT_SURRENDER
    );
  }
}

static const ScoreTqReqSurrender_Entry
ScoreTqReqSurrender_Entries[] = {
  { 0, 0, 0, 0, ScoreTqReqSurrender_Post_Dequeue_FIFO,
    ScoreTqReqSurrender_Post_Unblock_Yes },
  { 0, 0, 0, 0, ScoreTqReqSurrender_Post_Dequeue_Priority,
    ScoreTqReqSurrender_Post_Unblock_Yes },
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, ScoreTqReqSurrender_Post_Dequeue_NA,
    ScoreTqReqSurrender_Post_Unblock_NA },
#else
  { 0, 0, 0, 0, ScoreTqReqSurrender_Post_Dequeue_FIFO,
    ScoreTqReqSurrender_Post_Unblock_No },
#endif
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, ScoreTqReqSurrender_Post_Dequeue_NA,
    ScoreTqReqSurrender_Post_Unblock_NA },
#else
  { 0, 0, 0, 0, ScoreTqReqSurrender_Post_Dequeue_Priority,
    ScoreTqReqSurrender_Post_Unblock_No },
#endif
  { 0, 0, 0, 0, ScoreTqReqSurrender_Post_Dequeue_FIFO,
    ScoreTqReqSurrender_Post_Unblock_No },
  { 0, 0, 0, 0, ScoreTqReqSurrender_Post_Dequeue_Priority,
    ScoreTqReqSurrender_Post_Unblock_No }
};

static const uint8_t
ScoreTqReqSurrender_Map[] = {
  0, 2, 1, 3, 0, 4, 1, 5
};

static size_t ScoreTqReqSurrender_Scope( void *arg, char *buf, size_t n )
{
  ScoreTqReqSurrender_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( ScoreTqReqSurrender_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture ScoreTqReqSurrender_Fixture = {
  .setup = ScoreTqReqSurrender_Setup_Wrap,
  .stop = NULL,
  .teardown = ScoreTqReqSurrender_Teardown_Wrap,
  .scope = ScoreTqReqSurrender_Scope,
  .initial_context = &ScoreTqReqSurrender_Instance
};

static const uint8_t ScoreTqReqSurrender_Weights[] = {
  4, 2, 1
};

static void ScoreTqReqSurrender_Skip(
  ScoreTqReqSurrender_Context *ctx,
  size_t                       index
)
{
  switch ( index + 1 ) {
    case 1:
      ctx->Map.pcs[ 1 ] = ScoreTqReqSurrender_Pre_Discipline_NA - 1;
      /* Fall through */
    case 2:
      ctx->Map.pcs[ 2 ] = ScoreTqReqSurrender_Pre_WaitState_NA - 1;
      break;
  }
}

static inline ScoreTqReqSurrender_Entry ScoreTqReqSurrender_PopEntry(
  ScoreTqReqSurrender_Context *ctx
)
{
  size_t index;

  if ( ctx->Map.skip ) {
    size_t i;

    ctx->Map.skip = false;
    index = 0;

    for ( i = 0; i < 3; ++i ) {
      index += ScoreTqReqSurrender_Weights[ i ] * ctx->Map.pcs[ i ];
    }
  } else {
    index = ctx->Map.index;
  }

  ctx->Map.index = index + 1;

  return ScoreTqReqSurrender_Entries[
    ScoreTqReqSurrender_Map[ index ]
  ];
}

static void ScoreTqReqSurrender_TestVariant( ScoreTqReqSurrender_Context *ctx )
{
  ScoreTqReqSurrender_Pre_HasOwner_Prepare( ctx, ctx->Map.pcs[ 0 ] );

  if ( ctx->Map.skip ) {
    ScoreTqReqSurrender_Skip( ctx, 0 );
    return;
  }

  ScoreTqReqSurrender_Pre_Discipline_Prepare( ctx, ctx->Map.pcs[ 1 ] );

  if ( ctx->Map.skip ) {
    ScoreTqReqSurrender_Skip( ctx, 1 );
    return;
  }

  ScoreTqReqSurrender_Pre_WaitState_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  ScoreTqReqSurrender_Action( ctx );
  ScoreTqReqSurrender_Post_Dequeue_Check( ctx, ctx->Map.entry.Post_Dequeue );
  ScoreTqReqSurrender_Post_Unblock_Check( ctx, ctx->Map.entry.Post_Unblock );
}

static T_fixture_node ScoreTqReqSurrender_Node;

static T_remark ScoreTqReqSurrender_Remark = {
  .next = NULL,
  .remark = "ScoreTqReqSurrender"
};

void ScoreTqReqSurrender_Run( TQContext *tq_ctx )
{
  ScoreTqReqSurrender_Context *ctx;

  ctx = &ScoreTqReqSurrender_Instance;
  ctx->tq_ctx = tq_ctx;

  ctx = T_push_fixture(
    &ScoreTqReqSurrender_Node,
    &ScoreTqReqSurrender_Fixture
  );
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;
  ctx->Map.skip = false;

  for (
    ctx->Map.pcs[ 0 ] = ScoreTqReqSurrender_Pre_HasOwner_Yes;
    ctx->Map.pcs[ 0 ] < ScoreTqReqSurrender_Pre_HasOwner_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = ScoreTqReqSurrender_Pre_Discipline_FIFO;
      ctx->Map.pcs[ 1 ] < ScoreTqReqSurrender_Pre_Discipline_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = ScoreTqReqSurrender_Pre_WaitState_Blocked;
        ctx->Map.pcs[ 2 ] < ScoreTqReqSurrender_Pre_WaitState_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        ctx->Map.entry = ScoreTqReqSurrender_PopEntry( ctx );

        if ( ctx->Map.entry.Skip ) {
          continue;
        }

        ScoreTqReqSurrender_TestVariant( ctx );
      }
    }
  }

  T_add_remark( &ScoreTqReqSurrender_Remark );
  T_pop_fixture();
}

/** @} */
