/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreTqReqEnqueueFifo
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

#include "tr-tq-enqueue-fifo.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreTqReqEnqueueFifo spec:/score/tq/req/enqueue-fifo
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Queue_NA : 1;
  uint8_t Post_Position : 2;
} ScoreTqReqEnqueueFifo_Entry;

/**
 * @brief Test context for spec:/score/tq/req/enqueue-fifo test case.
 */
typedef struct {
  /**
   * @brief This member contains a copy of the corresponding
   *   ScoreTqReqEnqueueFifo_Run() parameter.
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
    ScoreTqReqEnqueueFifo_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} ScoreTqReqEnqueueFifo_Context;

static ScoreTqReqEnqueueFifo_Context
  ScoreTqReqEnqueueFifo_Instance;

static const char * const ScoreTqReqEnqueueFifo_PreDesc_Queue[] = {
  "Empty",
  "NonEmpty",
  "NA"
};

static const char * const * const ScoreTqReqEnqueueFifo_PreDesc[] = {
  ScoreTqReqEnqueueFifo_PreDesc_Queue,
  NULL
};

typedef ScoreTqReqEnqueueFifo_Context Context;

static const rtems_tcb *GetUnblock( Context *ctx, size_t *index )
{
  return TQGetNextUnblock( ctx->tq_ctx, index )->thread;
}

static const rtems_tcb *GetTCB( Context *ctx, TQWorkerKind worker )
{
  return ctx->tq_ctx->worker_tcb[ worker ];
}

static void ScoreTqReqEnqueueFifo_Pre_Queue_Prepare(
  ScoreTqReqEnqueueFifo_Context  *ctx,
  ScoreTqReqEnqueueFifo_Pre_Queue state
)
{
  switch ( state ) {
    case ScoreTqReqEnqueueFifo_Pre_Queue_Empty: {
      /*
       * While the queue is empty.
       */
      ctx->tq_ctx->how_many = 1;
      break;
    }

    case ScoreTqReqEnqueueFifo_Pre_Queue_NonEmpty: {
      /*
       * While the queue is non-empty.
       */
      ctx->tq_ctx->how_many = 2;
      break;
    }

    case ScoreTqReqEnqueueFifo_Pre_Queue_NA:
      break;
  }
}

static void ScoreTqReqEnqueueFifo_Post_Position_Check(
  ScoreTqReqEnqueueFifo_Context      *ctx,
  ScoreTqReqEnqueueFifo_Post_Position state
)
{
  size_t i;

  i = 0;

  /* Event receives */
  T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_C ) );
  T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_A ) );

  switch ( state ) {
    case ScoreTqReqEnqueueFifo_Post_Position_First: {
      /*
       * The thread shall be the first thread in the queue.
       */
      T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_C ) );
      T_eq_ptr( GetUnblock( ctx, &i ), NULL );
      break;
    }

    case ScoreTqReqEnqueueFifo_Post_Position_Last: {
      /*
       * The thread shall be the last thread in the queue.
       */
      T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_B ) );
      T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_C ) );
      T_eq_ptr( GetUnblock( ctx, &i ), NULL );
      break;
    }

    case ScoreTqReqEnqueueFifo_Post_Position_NA:
      break;
  }
}

static void ScoreTqReqEnqueueFifo_Setup( ScoreTqReqEnqueueFifo_Context *ctx )
{
  TQSetPriority( ctx->tq_ctx, TQ_BLOCKER_A, PRIO_HIGH );
  TQSetPriority( ctx->tq_ctx, TQ_BLOCKER_B, PRIO_VERY_HIGH );
  TQSetPriority( ctx->tq_ctx, TQ_BLOCKER_C, PRIO_ULTRA_HIGH );
}

static void ScoreTqReqEnqueueFifo_Setup_Wrap( void *arg )
{
  ScoreTqReqEnqueueFifo_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  ScoreTqReqEnqueueFifo_Setup( ctx );
}

static void ScoreTqReqEnqueueFifo_Teardown(
  ScoreTqReqEnqueueFifo_Context *ctx
)
{
  TQReset( ctx->tq_ctx );
}

static void ScoreTqReqEnqueueFifo_Teardown_Wrap( void *arg )
{
  ScoreTqReqEnqueueFifo_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  ScoreTqReqEnqueueFifo_Teardown( ctx );
}

static void ScoreTqReqEnqueueFifo_Action( ScoreTqReqEnqueueFifo_Context *ctx )
{
  TQSend( ctx->tq_ctx, TQ_BLOCKER_A, TQ_EVENT_ENQUEUE_PREPARE );

  if ( ctx->tq_ctx->how_many >= 2 ) {
    TQSend( ctx->tq_ctx, TQ_BLOCKER_B, TQ_EVENT_ENQUEUE | TQ_EVENT_SURRENDER );
  }

  TQSchedulerRecordStart( ctx->tq_ctx );
  TQSend( ctx->tq_ctx, TQ_BLOCKER_C, TQ_EVENT_ENQUEUE | TQ_EVENT_SURRENDER );
  TQSend( ctx->tq_ctx, TQ_BLOCKER_A, TQ_EVENT_ENQUEUE_DONE );
  TQSchedulerRecordStop( ctx->tq_ctx );
}

static const ScoreTqReqEnqueueFifo_Entry
ScoreTqReqEnqueueFifo_Entries[] = {
  { 0, 0, ScoreTqReqEnqueueFifo_Post_Position_First },
  { 0, 0, ScoreTqReqEnqueueFifo_Post_Position_Last }
};

static const uint8_t
ScoreTqReqEnqueueFifo_Map[] = {
  0, 1
};

static size_t ScoreTqReqEnqueueFifo_Scope( void *arg, char *buf, size_t n )
{
  ScoreTqReqEnqueueFifo_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( ScoreTqReqEnqueueFifo_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture ScoreTqReqEnqueueFifo_Fixture = {
  .setup = ScoreTqReqEnqueueFifo_Setup_Wrap,
  .stop = NULL,
  .teardown = ScoreTqReqEnqueueFifo_Teardown_Wrap,
  .scope = ScoreTqReqEnqueueFifo_Scope,
  .initial_context = &ScoreTqReqEnqueueFifo_Instance
};

static inline ScoreTqReqEnqueueFifo_Entry ScoreTqReqEnqueueFifo_PopEntry(
  ScoreTqReqEnqueueFifo_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return ScoreTqReqEnqueueFifo_Entries[
    ScoreTqReqEnqueueFifo_Map[ index ]
  ];
}

static void ScoreTqReqEnqueueFifo_TestVariant(
  ScoreTqReqEnqueueFifo_Context *ctx
)
{
  ScoreTqReqEnqueueFifo_Pre_Queue_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  ScoreTqReqEnqueueFifo_Action( ctx );
  ScoreTqReqEnqueueFifo_Post_Position_Check(
    ctx,
    ctx->Map.entry.Post_Position
  );
}

static T_fixture_node ScoreTqReqEnqueueFifo_Node;

static T_remark ScoreTqReqEnqueueFifo_Remark = {
  .next = NULL,
  .remark = "ScoreTqReqEnqueueFifo"
};

void ScoreTqReqEnqueueFifo_Run( TQContext *tq_ctx )
{
  ScoreTqReqEnqueueFifo_Context *ctx;

  ctx = &ScoreTqReqEnqueueFifo_Instance;
  ctx->tq_ctx = tq_ctx;

  ctx = T_push_fixture(
    &ScoreTqReqEnqueueFifo_Node,
    &ScoreTqReqEnqueueFifo_Fixture
  );
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = ScoreTqReqEnqueueFifo_Pre_Queue_Empty;
    ctx->Map.pcs[ 0 ] < ScoreTqReqEnqueueFifo_Pre_Queue_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = ScoreTqReqEnqueueFifo_PopEntry( ctx );
    ScoreTqReqEnqueueFifo_TestVariant( ctx );
  }

  T_add_remark( &ScoreTqReqEnqueueFifo_Remark );
  T_pop_fixture();
}

/** @} */
