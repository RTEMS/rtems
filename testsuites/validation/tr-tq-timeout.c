/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreTqReqTimeout
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

#include <rtems/score/threadimpl.h>

#include "tr-tq-timeout.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreTqReqTimeout spec:/score/tq/req/timeout
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_WaitState_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_Unblock : 2;
} ScoreTqReqTimeout_Entry;

/**
 * @brief Test context for spec:/score/tq/req/timeout test case.
 */
typedef struct {
  /**
   * @brief This member contains the call within ISR request.
   */
  CallWithinISRRequest request;

  /**
   * @brief This member contains a copy of the corresponding
   *   ScoreTqReqTimeout_Run() parameter.
   */
  TQContext *tq_ctx;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 1 ];

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
    ScoreTqReqTimeout_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} ScoreTqReqTimeout_Context;

static ScoreTqReqTimeout_Context
  ScoreTqReqTimeout_Instance;

static const char * const ScoreTqReqTimeout_PreDesc_WaitState[] = {
  "Blocked",
  "IntendToBlock",
  "ReadyAgain",
  "NA"
};

static const char * const * const ScoreTqReqTimeout_PreDesc[] = {
  ScoreTqReqTimeout_PreDesc_WaitState,
  NULL
};

typedef ScoreTqReqTimeout_Context Context;

static const rtems_tcb *GetUnblock( Context *ctx, size_t *index )
{
  return TQGetNextUnblock( ctx->tq_ctx, index )->thread;
}

static const rtems_tcb *GetTCB( Context *ctx, TQWorkerKind worker )
{
  return ctx->tq_ctx->worker_tcb[ worker ];
}

static void Tick( void *arg )
{
  Context *ctx;

  ctx = arg;
  TQSchedulerRecordStart( ctx->tq_ctx );
  FinalClockTick();
  TQSchedulerRecordStop( ctx->tq_ctx );
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

static void ThreadTimeout( void *arg )
{
  Context *ctx;

  ctx = arg;
  TQSchedulerRecordStart( ctx->tq_ctx );
  _Thread_Timeout(
    &ctx->tq_ctx->worker_tcb[ TQ_BLOCKER_A ]->Timer.Watchdog
  );
  TQSchedulerRecordStop( ctx->tq_ctx );
}

static void SchedulerUnblock(
  void                    *arg,
  const T_scheduler_event *event,
  T_scheduler_when         when
)
{
  Context *ctx;

  ctx = arg;

  if (
    when == T_SCHEDULER_BEFORE &&
    event->operation == T_SCHEDULER_UNBLOCK
  ) {
    T_scheduler_set_event_handler( NULL, NULL );
    ctx->request.handler = ThreadTimeout;
    CallWithinISRSubmit( &ctx->request );
  }
}

static void ScoreTqReqTimeout_Pre_WaitState_Prepare(
  ScoreTqReqTimeout_Context      *ctx,
  ScoreTqReqTimeout_Pre_WaitState state
)
{
  switch ( state ) {
    case ScoreTqReqTimeout_Pre_WaitState_Blocked: {
      /*
       * While the thread of the timeout operation is in the blocked wait
       * state.
       */
      TQEnqueuePrepare( ctx->tq_ctx );
      TQSend( ctx->tq_ctx, TQ_BLOCKER_A, TQ_EVENT_ENQUEUE );
      Yield();
      Tick( ctx );
      TQEnqueueDone( ctx->tq_ctx );
      break;
    }

    case ScoreTqReqTimeout_Pre_WaitState_IntendToBlock: {
      /*
       * While the thread of the timeout operation is in the intend to block
       * wait state.
       */
      TQEnqueuePrepare( ctx->tq_ctx );
      T_scheduler_set_event_handler( SchedulerBlock, ctx );
      TQSend( ctx->tq_ctx, TQ_BLOCKER_A, TQ_EVENT_ENQUEUE );
      Yield();
      TQEnqueueDone( ctx->tq_ctx );
      break;
    }

    case ScoreTqReqTimeout_Pre_WaitState_ReadyAgain: {
      /*
       * While the thread of the timeout operation is in the ready again wait
       * state.
       */
      TQEnqueuePrepare( ctx->tq_ctx );
      TQSend( ctx->tq_ctx, TQ_BLOCKER_A, TQ_EVENT_ENQUEUE );
      Yield();
      T_scheduler_set_event_handler( SchedulerUnblock, ctx );
      TQEnqueueDone( ctx->tq_ctx );
      TQSend( ctx->tq_ctx, TQ_BLOCKER_A, TQ_EVENT_SURRENDER );
      break;
    }

    case ScoreTqReqTimeout_Pre_WaitState_NA:
      break;
  }
}

static void ScoreTqReqTimeout_Post_Status_Check(
  ScoreTqReqTimeout_Context    *ctx,
  ScoreTqReqTimeout_Post_Status state
)
{
  switch ( state ) {
    case ScoreTqReqTimeout_Post_Status_Ok: {
      /*
       * The return status of the directive call shall be derived from
       * STATUS_SUCCESSFUL.
       */
      T_eq_int(
        ctx->tq_ctx->status[ TQ_BLOCKER_A ],
        TQConvertStatus( ctx->tq_ctx, STATUS_SUCCESSFUL )
      );
      break;
    }

    case ScoreTqReqTimeout_Post_Status_Timeout: {
      /*
       * The return status of the directive call shall be derived from
       * STATUS_TIMEOUT.
       */
      T_eq_int(
        ctx->tq_ctx->status[ TQ_BLOCKER_A ],
        TQConvertStatus( ctx->tq_ctx, STATUS_TIMEOUT )
      );
      break;
    }

    case ScoreTqReqTimeout_Post_Status_NA:
      break;
  }
}

static void ScoreTqReqTimeout_Post_Unblock_Check(
  ScoreTqReqTimeout_Context     *ctx,
  ScoreTqReqTimeout_Post_Unblock state
)
{
  size_t i;

  i = 0;

  switch ( state ) {
    case ScoreTqReqTimeout_Post_Unblock_Yes: {
      /*
       * The thread of the timeout operation shall be unblocked by the timeout
       * operation.
       */
      T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_A ) );
      T_eq_ptr( GetUnblock( ctx, &i ), NULL );
      break;
    }

    case ScoreTqReqTimeout_Post_Unblock_No: {
      /*
       * The thread of the timeout operation shall not be unblocked by the
       * timeout operation.
       */
      T_eq_ptr( GetUnblock( ctx, &i ), NULL );
      break;
    }

    case ScoreTqReqTimeout_Post_Unblock_NA:
      break;
  }
}

static void ScoreTqReqTimeout_Setup( ScoreTqReqTimeout_Context *ctx )
{
  ctx->request.arg = ctx;
  TQReset( ctx->tq_ctx );

  if ( ctx->tq_ctx->enqueue_variant == TQ_ENQUEUE_STICKY ) {
    TQSetScheduler( ctx->tq_ctx, TQ_BLOCKER_A, SCHEDULER_B_ID, PRIO_NORMAL );
  } else {
    TQSetPriority( ctx->tq_ctx, TQ_BLOCKER_A, PRIO_HIGH );
  }
}

static void ScoreTqReqTimeout_Setup_Wrap( void *arg )
{
  ScoreTqReqTimeout_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  ScoreTqReqTimeout_Setup( ctx );
}

static void ScoreTqReqTimeout_Teardown( ScoreTqReqTimeout_Context *ctx )
{
  TQReset( ctx->tq_ctx );
}

static void ScoreTqReqTimeout_Teardown_Wrap( void *arg )
{
  ScoreTqReqTimeout_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  ScoreTqReqTimeout_Teardown( ctx );
}

static void ScoreTqReqTimeout_Action( ScoreTqReqTimeout_Context *ctx )
{
  /*
   * The action is performed by the ``WaitState`` pre-condition preparation.
   */
}

static const ScoreTqReqTimeout_Entry
ScoreTqReqTimeout_Entries[] = {
  { 0, 0, ScoreTqReqTimeout_Post_Status_Timeout,
    ScoreTqReqTimeout_Post_Unblock_Yes },
  { 0, 0, ScoreTqReqTimeout_Post_Status_Timeout,
    ScoreTqReqTimeout_Post_Unblock_No },
  { 0, 0, ScoreTqReqTimeout_Post_Status_Ok, ScoreTqReqTimeout_Post_Unblock_No }
};

static const uint8_t
ScoreTqReqTimeout_Map[] = {
  0, 1, 2
};

static size_t ScoreTqReqTimeout_Scope( void *arg, char *buf, size_t n )
{
  ScoreTqReqTimeout_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( ScoreTqReqTimeout_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture ScoreTqReqTimeout_Fixture = {
  .setup = ScoreTqReqTimeout_Setup_Wrap,
  .stop = NULL,
  .teardown = ScoreTqReqTimeout_Teardown_Wrap,
  .scope = ScoreTqReqTimeout_Scope,
  .initial_context = &ScoreTqReqTimeout_Instance
};

static inline ScoreTqReqTimeout_Entry ScoreTqReqTimeout_PopEntry(
  ScoreTqReqTimeout_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return ScoreTqReqTimeout_Entries[
    ScoreTqReqTimeout_Map[ index ]
  ];
}

static void ScoreTqReqTimeout_TestVariant( ScoreTqReqTimeout_Context *ctx )
{
  ScoreTqReqTimeout_Pre_WaitState_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  ScoreTqReqTimeout_Action( ctx );
  ScoreTqReqTimeout_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  ScoreTqReqTimeout_Post_Unblock_Check( ctx, ctx->Map.entry.Post_Unblock );
}

static T_fixture_node ScoreTqReqTimeout_Node;

static T_remark ScoreTqReqTimeout_Remark = {
  .next = NULL,
  .remark = "ScoreTqReqTimeout"
};

void ScoreTqReqTimeout_Run( TQContext *tq_ctx )
{
  ScoreTqReqTimeout_Context *ctx;

  ctx = &ScoreTqReqTimeout_Instance;
  ctx->tq_ctx = tq_ctx;

  ctx = T_push_fixture( &ScoreTqReqTimeout_Node, &ScoreTqReqTimeout_Fixture );
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = ScoreTqReqTimeout_Pre_WaitState_Blocked;
    ctx->Map.pcs[ 0 ] < ScoreTqReqTimeout_Pre_WaitState_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = ScoreTqReqTimeout_PopEntry( ctx );
    ScoreTqReqTimeout_TestVariant( ctx );
  }

  T_add_remark( &ScoreTqReqTimeout_Remark );
  T_pop_fixture();
}

/** @} */
