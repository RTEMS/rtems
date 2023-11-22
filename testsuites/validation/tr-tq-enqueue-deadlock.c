/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreTqReqEnqueueDeadlock
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

#include "tr-tq-enqueue-deadlock.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreTqReqEnqueueDeadlock spec:/score/tq/req/enqueue-deadlock
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Notification_NA : 1;
  uint8_t Pre_Deadlock_NA : 1;
  uint8_t Post_Result : 2;
} ScoreTqReqEnqueueDeadlock_Entry;

/**
 * @brief Test context for spec:/score/tq/req/enqueue-deadlock test case.
 */
typedef struct {
  /**
   * @brief If this member is true, then more than one mutex shall be used for
   *   the deadlock scenario.
   */
  bool more;

  /**
   * @brief This member contains a copy of the corresponding
   *   ScoreTqReqEnqueueDeadlock_Run() parameter.
   */
  TQContext *tq_ctx;

  struct {
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
    ScoreTqReqEnqueueDeadlock_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} ScoreTqReqEnqueueDeadlock_Context;

static ScoreTqReqEnqueueDeadlock_Context
  ScoreTqReqEnqueueDeadlock_Instance;

static const char * const ScoreTqReqEnqueueDeadlock_PreDesc_Notification[] = {
  "Status",
  "Fatal",
  "NA"
};

static const char * const ScoreTqReqEnqueueDeadlock_PreDesc_Deadlock[] = {
  "One",
  "More",
  "NA"
};

static const char * const * const ScoreTqReqEnqueueDeadlock_PreDesc[] = {
  ScoreTqReqEnqueueDeadlock_PreDesc_Notification,
  ScoreTqReqEnqueueDeadlock_PreDesc_Deadlock,
  NULL
};

static void ScoreTqReqEnqueueDeadlock_Pre_Notification_Prepare(
  ScoreTqReqEnqueueDeadlock_Context         *ctx,
  ScoreTqReqEnqueueDeadlock_Pre_Notification state
)
{
  switch ( state ) {
    case ScoreTqReqEnqueueDeadlock_Pre_Notification_Status: {
      /*
       * Where a detected deadlock results in a return with a status code.
       */
      if ( ctx->tq_ctx->deadlock != TQ_DEADLOCK_STATUS ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTqReqEnqueueDeadlock_Pre_Notification_Fatal: {
      /*
       * Where a detected deadlock results in a fatal error.
       */
      if ( ctx->tq_ctx->deadlock != TQ_DEADLOCK_FATAL ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTqReqEnqueueDeadlock_Pre_Notification_NA:
      break;
  }
}

static void ScoreTqReqEnqueueDeadlock_Pre_Deadlock_Prepare(
  ScoreTqReqEnqueueDeadlock_Context     *ctx,
  ScoreTqReqEnqueueDeadlock_Pre_Deadlock state
)
{
  switch ( state ) {
    case ScoreTqReqEnqueueDeadlock_Pre_Deadlock_One: {
      /*
       * While the owner of the thread queue is enqueued on another thread
       * queue owned by the calling thread.
       */
      ctx->more = false;
      break;
    }

    case ScoreTqReqEnqueueDeadlock_Pre_Deadlock_More: {
      /*
       * While the owner of the thread queue is enqueued on another thread
       * queue owned by a thread other than the calling thread, and so on,
       * while the owner of the last thread queue of this dependency chain is
       * enqueued on a thread queue owned by the calling thread.
       */
      ctx->more = true;
      break;
    }

    case ScoreTqReqEnqueueDeadlock_Pre_Deadlock_NA:
      break;
  }
}

static void ScoreTqReqEnqueueDeadlock_Post_Result_Check(
  ScoreTqReqEnqueueDeadlock_Context    *ctx,
  ScoreTqReqEnqueueDeadlock_Post_Result state
)
{
  switch ( state ) {
    case ScoreTqReqEnqueueDeadlock_Post_Result_Status: {
      /*
       * The return status of the directive call shall be derived from
       * STATUS_DEADLOCK.
       */
      /* Checked by action */
      break;
    }

    case ScoreTqReqEnqueueDeadlock_Post_Result_Fatal: {
      /*
       * The system shall terminate with the INTERNAL_ERROR_CORE fatal source
       * and the INTERNAL_ERROR_THREAD_QUEUE_DEADLOCK fatal code.
       */
      /* Checked by action */
      break;
    }

    case ScoreTqReqEnqueueDeadlock_Post_Result_NA:
      break;
  }
}

static void ScoreTqReqEnqueueDeadlock_Action(
  ScoreTqReqEnqueueDeadlock_Context *ctx
)
{
  Status_Control status;

  if ( ctx->tq_ctx->enqueue_variant == TQ_ENQUEUE_STICKY ) {
    TQSetScheduler( ctx->tq_ctx, TQ_BLOCKER_A, SCHEDULER_B_ID, PRIO_NORMAL );
  } else {
    TQSetScheduler(
      ctx->tq_ctx,
      TQ_BLOCKER_A,
      SCHEDULER_A_ID,
      PRIO_VERY_HIGH
    );
  }

  TQSetScheduler( ctx->tq_ctx, TQ_BLOCKER_B, SCHEDULER_A_ID, PRIO_HIGH );
  TQSetScheduler( ctx->tq_ctx, TQ_BLOCKER_C, SCHEDULER_A_ID, PRIO_HIGH );

  TQSortMutexesByID( ctx->tq_ctx );
  TQMutexObtain( ctx->tq_ctx, TQ_MUTEX_C );
  TQSendAndWaitForExecutionStop( ctx->tq_ctx, TQ_BLOCKER_A, TQ_EVENT_ENQUEUE );

  if ( ctx->more ) {
    TQSend( ctx->tq_ctx, TQ_BLOCKER_B, TQ_EVENT_MUTEX_A_OBTAIN );
    TQSend( ctx->tq_ctx, TQ_BLOCKER_B, TQ_EVENT_MUTEX_C_OBTAIN );
    Yield();
    TQSend( ctx->tq_ctx, TQ_BLOCKER_C, TQ_EVENT_MUTEX_B_OBTAIN );
    Yield();
    TQSend( ctx->tq_ctx, TQ_BLOCKER_C, TQ_EVENT_MUTEX_A_OBTAIN );
    Yield();
    TQSendAndWaitForExecutionStop(
      ctx->tq_ctx,
      TQ_BLOCKER_A,
      TQ_EVENT_MUTEX_B_OBTAIN
    );
  } else {
    TQSendAndWaitForExecutionStop(
      ctx->tq_ctx,
      TQ_BLOCKER_A,
      TQ_EVENT_MUTEX_C_OBTAIN
    );
  }

  if ( ctx->tq_ctx->deadlock == TQ_DEADLOCK_FATAL ) {
    status = TQEnqueueFatal( ctx->tq_ctx );
    T_eq_int( status, STATUS_DEADLOCK );
  } else {
    status = TQEnqueue( ctx->tq_ctx, TQ_WAIT_FOREVER );
    T_eq_int( status, TQConvertStatus( ctx->tq_ctx, STATUS_DEADLOCK ) );
  }

  TQMutexRelease( ctx->tq_ctx, TQ_MUTEX_C );

  if ( ctx->more ) {
    TQSend( ctx->tq_ctx, TQ_BLOCKER_B, TQ_EVENT_MUTEX_C_RELEASE );
    TQSend( ctx->tq_ctx, TQ_BLOCKER_B, TQ_EVENT_MUTEX_A_RELEASE );
    TQSend( ctx->tq_ctx, TQ_BLOCKER_C, TQ_EVENT_MUTEX_A_RELEASE );
    TQSend( ctx->tq_ctx, TQ_BLOCKER_C, TQ_EVENT_MUTEX_B_RELEASE );
    TQSend( ctx->tq_ctx, TQ_BLOCKER_A, TQ_EVENT_MUTEX_B_RELEASE );
  } else {
    TQSend( ctx->tq_ctx, TQ_BLOCKER_A, TQ_EVENT_MUTEX_C_RELEASE );
  }

  if ( ctx->tq_ctx->enqueue_variant == TQ_ENQUEUE_STICKY ) {
    TQSend(
      ctx->tq_ctx,
      TQ_BLOCKER_A,
      TQ_EVENT_SURRENDER | TQ_EVENT_RUNNER_SYNC
    );
    TQSynchronizeRunner();
    TQSetScheduler( ctx->tq_ctx, TQ_BLOCKER_A, SCHEDULER_A_ID, PRIO_HIGH );
  } else {
    TQSend( ctx->tq_ctx, TQ_BLOCKER_A, TQ_EVENT_SURRENDER );
  }
}

static const ScoreTqReqEnqueueDeadlock_Entry
ScoreTqReqEnqueueDeadlock_Entries[] = {
  { 0, 0, 0, ScoreTqReqEnqueueDeadlock_Post_Result_Status },
  { 0, 0, 0, ScoreTqReqEnqueueDeadlock_Post_Result_Fatal }
};

static const uint8_t
ScoreTqReqEnqueueDeadlock_Map[] = {
  0, 0, 1, 1
};

static size_t ScoreTqReqEnqueueDeadlock_Scope( void *arg, char *buf, size_t n )
{
  ScoreTqReqEnqueueDeadlock_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      ScoreTqReqEnqueueDeadlock_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture ScoreTqReqEnqueueDeadlock_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = ScoreTqReqEnqueueDeadlock_Scope,
  .initial_context = &ScoreTqReqEnqueueDeadlock_Instance
};

static const uint8_t ScoreTqReqEnqueueDeadlock_Weights[] = {
  2, 1
};

static void ScoreTqReqEnqueueDeadlock_Skip(
  ScoreTqReqEnqueueDeadlock_Context *ctx,
  size_t                             index
)
{
  switch ( index + 1 ) {
    case 1:
      ctx->Map.pcs[ 1 ] = ScoreTqReqEnqueueDeadlock_Pre_Deadlock_NA - 1;
      break;
  }
}

static inline ScoreTqReqEnqueueDeadlock_Entry
ScoreTqReqEnqueueDeadlock_PopEntry( ScoreTqReqEnqueueDeadlock_Context *ctx )
{
  size_t index;

  if ( ctx->Map.skip ) {
    size_t i;

    ctx->Map.skip = false;
    index = 0;

    for ( i = 0; i < 2; ++i ) {
      index += ScoreTqReqEnqueueDeadlock_Weights[ i ] * ctx->Map.pcs[ i ];
    }
  } else {
    index = ctx->Map.index;
  }

  ctx->Map.index = index + 1;

  return ScoreTqReqEnqueueDeadlock_Entries[
    ScoreTqReqEnqueueDeadlock_Map[ index ]
  ];
}

static void ScoreTqReqEnqueueDeadlock_TestVariant(
  ScoreTqReqEnqueueDeadlock_Context *ctx
)
{
  ScoreTqReqEnqueueDeadlock_Pre_Notification_Prepare( ctx, ctx->Map.pcs[ 0 ] );

  if ( ctx->Map.skip ) {
    ScoreTqReqEnqueueDeadlock_Skip( ctx, 0 );
    return;
  }

  ScoreTqReqEnqueueDeadlock_Pre_Deadlock_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  ScoreTqReqEnqueueDeadlock_Action( ctx );
  ScoreTqReqEnqueueDeadlock_Post_Result_Check(
    ctx,
    ctx->Map.entry.Post_Result
  );
}

static T_fixture_node ScoreTqReqEnqueueDeadlock_Node;

static T_remark ScoreTqReqEnqueueDeadlock_Remark = {
  .next = NULL,
  .remark = "ScoreTqReqEnqueueDeadlock"
};

void ScoreTqReqEnqueueDeadlock_Run( TQContext *tq_ctx )
{
  ScoreTqReqEnqueueDeadlock_Context *ctx;

  ctx = &ScoreTqReqEnqueueDeadlock_Instance;
  ctx->tq_ctx = tq_ctx;

  ctx = T_push_fixture(
    &ScoreTqReqEnqueueDeadlock_Node,
    &ScoreTqReqEnqueueDeadlock_Fixture
  );
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;
  ctx->Map.skip = false;

  for (
    ctx->Map.pcs[ 0 ] = ScoreTqReqEnqueueDeadlock_Pre_Notification_Status;
    ctx->Map.pcs[ 0 ] < ScoreTqReqEnqueueDeadlock_Pre_Notification_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = ScoreTqReqEnqueueDeadlock_Pre_Deadlock_One;
      ctx->Map.pcs[ 1 ] < ScoreTqReqEnqueueDeadlock_Pre_Deadlock_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = ScoreTqReqEnqueueDeadlock_PopEntry( ctx );
      ScoreTqReqEnqueueDeadlock_TestVariant( ctx );
    }
  }

  T_add_remark( &ScoreTqReqEnqueueDeadlock_Remark );
  T_pop_fixture();
}

/** @} */
