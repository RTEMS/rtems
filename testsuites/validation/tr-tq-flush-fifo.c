/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestCaseScoreTqReqFlushFifo
 */

/*
 * Copyright (C) 2021 embedded brains GmbH (http://www.embedded-brains.de)
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
 * @defgroup RTEMSTestCaseScoreTqReqFlushFifo spec:/score/tq/req/flush-fifo
 *
 * @ingroup RTEMSTestSuiteTestsuitesValidationNoClock0
 *
 * @{
 */

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Queue_NA : 1;
  uint8_t Post_Operation : 2;
} ScoreTqReqFlushFifo_Entry;

/**
 * @brief Test context for spec:/score/tq/req/flush-fifo test case.
 */
typedef struct {
  /**
   * @brief This member contains the call within ISR request.
   */
  CallWithinISRRequest request;;

  /**
   * @brief This member contains a copy of the corresponding
   *   ScoreTqReqFlushFifo_Run() parameter.
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

static const char * const ScoreTqReqFlushFifo_PreDesc_Queue[] = {
  "Empty",
  "NonEmpty",
  "NA"
};

static const char * const * const ScoreTqReqFlushFifo_PreDesc[] = {
  ScoreTqReqFlushFifo_PreDesc_Queue,
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

static void Flush( void *arg )
{
  Context *ctx;

  ctx = arg;
  TQSchedulerRecordStart( ctx->tq_ctx );
  TQFlush( ctx->tq_ctx );
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

static void ScoreTqReqFlushFifo_Pre_Queue_Prepare(
  ScoreTqReqFlushFifo_Context  *ctx,
  ScoreTqReqFlushFifo_Pre_Queue state
)
{
  switch ( state ) {
    case ScoreTqReqFlushFifo_Pre_Queue_Empty: {
      /*
       * While the thread queue is empty.
       */
      ctx->tq_ctx->how_many = 0;
      break;
    }

    case ScoreTqReqFlushFifo_Pre_Queue_NonEmpty: {
      /*
       * While the thread queue has at least one enqueued thread.
       */
      ctx->tq_ctx->how_many = 3;
      break;
    }

    case ScoreTqReqFlushFifo_Pre_Queue_NA:
      break;
  }
}

static void ScoreTqReqFlushFifo_Post_Operation_Check(
  ScoreTqReqFlushFifo_Context       *ctx,
  ScoreTqReqFlushFifo_Post_Operation state
)
{
  size_t                   i;
  const T_scheduler_event *event;

  i = 0;

  switch ( state ) {
    case ScoreTqReqFlushFifo_Post_Operation_Nop: {
      /*
       * No operation shall be performed.
       */
      /* Event receive */
      T_eq_ptr( GetUnblock( ctx, &i )->thread, GetTCB( ctx, TQ_BLOCKER_A ) );
      T_eq_ptr( GetUnblock( ctx, &i ), &T_scheduler_event_null );
      break;
    }

    case ScoreTqReqFlushFifo_Post_Operation_TryExtract: {
      /*
       * The enqueued threads of the thread queue may be extracted in FIFO
       * order.
       */
      event = GetUnblock( ctx, &i );
      T_eq_ptr( event->executing, NULL );
      T_eq_ptr( event->thread, GetTCB( ctx, TQ_BLOCKER_B ) );

      event = GetUnblock( ctx, &i );
      T_eq_ptr( event->executing, NULL );
      T_eq_ptr( event->thread, GetTCB( ctx, TQ_BLOCKER_C ) );

      event = GetUnblock( ctx, &i );
      T_eq_ptr( event->executing, GetTCB( ctx, TQ_BLOCKER_D ) );
      T_eq_ptr( event->thread, GetTCB( ctx, TQ_BLOCKER_D ) );

      T_eq_ptr( GetUnblock( ctx, &i ), &T_scheduler_event_null );
      break;
    }

    case ScoreTqReqFlushFifo_Post_Operation_NA:
      break;
  }
}

static void ScoreTqReqFlushFifo_Setup( ScoreTqReqFlushFifo_Context *ctx )
{
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
  TQSend( ctx->tq_ctx, TQ_BLOCKER_A, TQ_EVENT_ENQUEUE_PREPARE );

  if ( ctx->tq_ctx->how_many > 0 ) {
    TQSend( ctx->tq_ctx, TQ_BLOCKER_B, TQ_EVENT_ENQUEUE );
    TQSend( ctx->tq_ctx, TQ_BLOCKER_C, TQ_EVENT_ENQUEUE );
    T_scheduler_set_event_handler( SchedulerEvent, ctx );
    TQSend( ctx->tq_ctx, TQ_BLOCKER_D, TQ_EVENT_ENQUEUE );
  } else {
    TQSchedulerRecordStart( ctx->tq_ctx );
    TQSend( ctx->tq_ctx, TQ_BLOCKER_A, TQ_EVENT_FLUSH );
  }

  TQSchedulerRecordStop( ctx->tq_ctx );
  TQSend( ctx->tq_ctx, TQ_BLOCKER_A, TQ_EVENT_ENQUEUE_DONE );
}

static const ScoreTqReqFlushFifo_Entry
ScoreTqReqFlushFifo_Entries[] = {
  { 0, 0, ScoreTqReqFlushFifo_Post_Operation_Nop },
  { 0, 0, ScoreTqReqFlushFifo_Post_Operation_TryExtract }
};

static const uint8_t
ScoreTqReqFlushFifo_Map[] = {
  0, 1
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

static inline ScoreTqReqFlushFifo_Entry ScoreTqReqFlushFifo_PopEntry(
  ScoreTqReqFlushFifo_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return ScoreTqReqFlushFifo_Entries[
    ScoreTqReqFlushFifo_Map[ index ]
  ];
}

static void ScoreTqReqFlushFifo_TestVariant( ScoreTqReqFlushFifo_Context *ctx )
{
  ScoreTqReqFlushFifo_Pre_Queue_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  ScoreTqReqFlushFifo_Action( ctx );
  ScoreTqReqFlushFifo_Post_Operation_Check(
    ctx,
    ctx->Map.entry.Post_Operation
  );
}

static T_fixture_node ScoreTqReqFlushFifo_Node;

void ScoreTqReqFlushFifo_Run( TQContext *tq_ctx )
{
  ScoreTqReqFlushFifo_Context *ctx;

  ctx = &ScoreTqReqFlushFifo_Instance;
  ctx->tq_ctx = tq_ctx;

  ctx = T_push_fixture(
    &ScoreTqReqFlushFifo_Node,
    &ScoreTqReqFlushFifo_Fixture
  );
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = ScoreTqReqFlushFifo_Pre_Queue_Empty;
    ctx->Map.pcs[ 0 ] < ScoreTqReqFlushFifo_Pre_Queue_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = ScoreTqReqFlushFifo_PopEntry( ctx );
    ScoreTqReqFlushFifo_TestVariant( ctx );
  }

  T_pop_fixture();
}

/** @} */
