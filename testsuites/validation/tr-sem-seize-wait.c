/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreSemReqSeizeWait
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

#include <rtems/score/statesimpl.h>

#include "tr-sem-seize-wait.h"
#include "tr-tq-enqueue-fifo.h"
#include "tr-tq-enqueue-priority.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreSemReqSeizeWait spec:/score/sem/req/seize-wait
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Count_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_Count : 2;
  uint8_t Post_Timer : 2;
} ScoreSemReqSeizeWait_Entry;

/**
 * @brief Test context for spec:/score/sem/req/seize-wait test case.
 */
typedef struct {
  /**
   * @brief This member contains a copy of the corresponding
   *   ScoreSemReqSeizeWait_Run() parameter.
   */
  TQSemContext *tq_ctx;

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
    ScoreSemReqSeizeWait_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} ScoreSemReqSeizeWait_Context;

static ScoreSemReqSeizeWait_Context
  ScoreSemReqSeizeWait_Instance;

static const char * const ScoreSemReqSeizeWait_PreDesc_Count[] = {
  "Zero",
  "Positive",
  "NA"
};

static const char * const * const ScoreSemReqSeizeWait_PreDesc[] = {
  ScoreSemReqSeizeWait_PreDesc_Count,
  NULL
};

typedef ScoreSemReqSeizeWait_Context Context;

static Status_Control Status( const Context *ctx, Status_Control status )
{
  return TQConvertStatus( &ctx->tq_ctx->base, status );
}

static void GetProperties( TQContext *base, TQWorkerKind enqueued_worker )
{
  TQSemContext        *ctx;
  T_thread_timer_state timer_state;

  ctx = (TQSemContext *) base;
  T_eq_u32(
    ctx->base.worker_tcb[ enqueued_worker ]->current_state,
    STATES_WAITING_FOR_SEMAPHORE
  );

  timer_state = T_get_thread_timer_state(
    ctx->base.worker_id[ enqueued_worker ]
  );

  if ( base->wait == TQ_WAIT_TIMED ) {
    T_eq_int( timer_state, T_THREAD_TIMER_SCHEDULED );
  } else {
    T_eq_int( timer_state, T_THREAD_TIMER_INACTIVE );
  }

  T_eq_u32( TQSemGetCount( ctx ), 0 );
}

static void ScoreSemReqSeizeWait_Pre_Count_Prepare(
  ScoreSemReqSeizeWait_Context  *ctx,
  ScoreSemReqSeizeWait_Pre_Count state
)
{
  switch ( state ) {
    case ScoreSemReqSeizeWait_Pre_Count_Zero: {
      /*
       * While the count of the semaphore is zero.
       */
      /* Done by TQEnqueuePrepareDefault() */
      break;
    }

    case ScoreSemReqSeizeWait_Pre_Count_Positive: {
      /*
       * While the count of the semaphore is greater than zero.
       */
      TQSemSetCount( ctx->tq_ctx, 1 );
      break;
    }

    case ScoreSemReqSeizeWait_Pre_Count_NA:
      break;
  }
}

static void ScoreSemReqSeizeWait_Post_Status_Check(
  ScoreSemReqSeizeWait_Context    *ctx,
  ScoreSemReqSeizeWait_Post_Status state
)
{
  Status_Control status;

  switch ( state ) {
    case ScoreSemReqSeizeWait_Post_Status_Ok: {
      /*
       * The return status of the directive call shall be derived from
       * STATUS_SUCCESSFUL.
       */
      status = TQEnqueue( &ctx->tq_ctx->base, ctx->tq_ctx->base.wait );
      T_eq_int( status, Status( ctx, STATUS_SUCCESSFUL ) );
      break;
    }

    case ScoreSemReqSeizeWait_Post_Status_Enqueued: {
      /*
       * Where the thread queue uses the FIFO discipline, the calling thread
       * shall be enqueued in FIFO order.
       *
       * Where the thread queue uses the priority discipline, the calling
       * thread shall be enqueued in priority order.
       */
      switch ( ctx->tq_ctx->base.discipline ) {
        case TQ_FIFO:
          ScoreTqReqEnqueueFifo_Run( &ctx->tq_ctx->base );
          break;
        case TQ_PRIORITY:
          ScoreTqReqEnqueuePriority_Run( &ctx->tq_ctx->base );
          break;
        default:
          T_unreachable();
          break;
      }
      break;
    }

    case ScoreSemReqSeizeWait_Post_Status_NA:
      break;
  }
}

static void ScoreSemReqSeizeWait_Post_Count_Check(
  ScoreSemReqSeizeWait_Context   *ctx,
  ScoreSemReqSeizeWait_Post_Count state
)
{
  switch ( state ) {
    case ScoreSemReqSeizeWait_Post_Count_Nop: {
      /*
       * The count of the semaphore shall not be modified.
       */
      /* Checked by GetProperties() */
      break;
    }

    case ScoreSemReqSeizeWait_Post_Count_MinusOne: {
      /*
       * The count of the semaphore shall be decremented by one.
       */
      T_eq_u32( TQSemGetCount( ctx->tq_ctx ), 0 );
      break;
    }

    case ScoreSemReqSeizeWait_Post_Count_NA:
      break;
  }
}

static void ScoreSemReqSeizeWait_Post_Timer_Check(
  ScoreSemReqSeizeWait_Context   *ctx,
  ScoreSemReqSeizeWait_Post_Timer state
)
{
  switch ( state ) {
    case ScoreSemReqSeizeWait_Post_Timer_Optional: {
      /*
       * Where the directive was called with a timeout in clock ticks, the
       * thread timer of the calling task shall fire after the specified clock
       * ticks.
       *
       * Where the directive was called without a timeout, the thread timer of
       * the calling task shall be inactive.
       */
      /* Checked by GetProperties() */
      break;
    }

    case ScoreSemReqSeizeWait_Post_Timer_No: {
      /*
       * The thread timer of the calling task shall be inactive.
       */
      T_eq_int(
        T_get_thread_timer_state( RTEMS_SELF ),
        T_THREAD_TIMER_INACTIVE
      );
      break;
    }

    case ScoreSemReqSeizeWait_Post_Timer_NA:
      break;
  }
}

static void ScoreSemReqSeizeWait_Prepare( ScoreSemReqSeizeWait_Context *ctx )
{
  ctx->tq_ctx->base.enqueue_prepare = TQEnqueuePrepareDefault;
  ctx->tq_ctx->base.enqueue_done = TQEnqueueDoneDefault;
  ctx->tq_ctx->base.get_properties = GetProperties;
}

static void ScoreSemReqSeizeWait_Action( ScoreSemReqSeizeWait_Context *ctx )
{
  /* Action performed by Status post-condition */
}

static const ScoreSemReqSeizeWait_Entry
ScoreSemReqSeizeWait_Entries[] = {
  { 0, 0, ScoreSemReqSeizeWait_Post_Status_Enqueued,
    ScoreSemReqSeizeWait_Post_Count_Nop,
    ScoreSemReqSeizeWait_Post_Timer_Optional },
  { 0, 0, ScoreSemReqSeizeWait_Post_Status_Ok,
    ScoreSemReqSeizeWait_Post_Count_MinusOne,
    ScoreSemReqSeizeWait_Post_Timer_No }
};

static const uint8_t
ScoreSemReqSeizeWait_Map[] = {
  0, 1
};

static size_t ScoreSemReqSeizeWait_Scope( void *arg, char *buf, size_t n )
{
  ScoreSemReqSeizeWait_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( ScoreSemReqSeizeWait_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture ScoreSemReqSeizeWait_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = ScoreSemReqSeizeWait_Scope,
  .initial_context = &ScoreSemReqSeizeWait_Instance
};

static inline ScoreSemReqSeizeWait_Entry ScoreSemReqSeizeWait_PopEntry(
  ScoreSemReqSeizeWait_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return ScoreSemReqSeizeWait_Entries[
    ScoreSemReqSeizeWait_Map[ index ]
  ];
}

static void ScoreSemReqSeizeWait_TestVariant(
  ScoreSemReqSeizeWait_Context *ctx
)
{
  ScoreSemReqSeizeWait_Pre_Count_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  ScoreSemReqSeizeWait_Action( ctx );
  ScoreSemReqSeizeWait_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  ScoreSemReqSeizeWait_Post_Count_Check( ctx, ctx->Map.entry.Post_Count );
  ScoreSemReqSeizeWait_Post_Timer_Check( ctx, ctx->Map.entry.Post_Timer );
}

static T_fixture_node ScoreSemReqSeizeWait_Node;

static T_remark ScoreSemReqSeizeWait_Remark = {
  .next = NULL,
  .remark = "ScoreSemReqSeizeWait"
};

void ScoreSemReqSeizeWait_Run( TQSemContext *tq_ctx )
{
  ScoreSemReqSeizeWait_Context *ctx;

  ctx = &ScoreSemReqSeizeWait_Instance;
  ctx->tq_ctx = tq_ctx;

  ctx = T_push_fixture(
    &ScoreSemReqSeizeWait_Node,
    &ScoreSemReqSeizeWait_Fixture
  );
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = ScoreSemReqSeizeWait_Pre_Count_Zero;
    ctx->Map.pcs[ 0 ] < ScoreSemReqSeizeWait_Pre_Count_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = ScoreSemReqSeizeWait_PopEntry( ctx );
    ScoreSemReqSeizeWait_Prepare( ctx );
    ScoreSemReqSeizeWait_TestVariant( ctx );
  }

  T_add_remark( &ScoreSemReqSeizeWait_Remark );
  T_pop_fixture();
}

/** @} */
