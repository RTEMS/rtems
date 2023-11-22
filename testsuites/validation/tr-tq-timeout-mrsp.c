/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreTqReqTimeoutMrsp
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

#include <rtems/score/smpimpl.h>
#include <rtems/score/threadimpl.h>

#include "tr-tq-timeout-mrsp.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreTqReqTimeoutMrsp spec:/score/tq/req/timeout-mrsp
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Scheduler_NA : 1;
  uint8_t Pre_WaitState_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_Unblock : 1;
} ScoreTqReqTimeoutMrsp_Entry;

/**
 * @brief Test context for spec:/score/tq/req/timeout-mrsp test case.
 */
typedef struct {
  /**
   * @brief If this member is true, then the enqueued thread shall use a home
   *   scheduler other than the home scheduler of the owner.
   */
  bool other_scheduler;

  /**
   * @brief If this member is true, then the processor set of the schedulers
   *   shall be restored.
   */
  bool restore_scheduler;

  /**
   * @brief This member contains a copy of the corresponding
   *   ScoreTqReqTimeoutMrsp_Run() parameter.
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
    ScoreTqReqTimeoutMrsp_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} ScoreTqReqTimeoutMrsp_Context;

static ScoreTqReqTimeoutMrsp_Context
  ScoreTqReqTimeoutMrsp_Instance;

static const char * const ScoreTqReqTimeoutMrsp_PreDesc_Scheduler[] = {
  "Same",
  "Other",
  "NA"
};

static const char * const ScoreTqReqTimeoutMrsp_PreDesc_WaitState[] = {
  "IntendToBlock",
  "ReadyAgain",
  "NA"
};

static const char * const * const ScoreTqReqTimeoutMrsp_PreDesc[] = {
  ScoreTqReqTimeoutMrsp_PreDesc_Scheduler,
  ScoreTqReqTimeoutMrsp_PreDesc_WaitState,
  NULL
};

typedef ScoreTqReqTimeoutMrsp_Context Context;

static const rtems_tcb *GetUnblock( Context *ctx, size_t *index )
{
  return TQGetNextUnblock( ctx->tq_ctx, index )->thread;
}

static void Tick( void *arg )
{
  Context *ctx;

  ctx = arg;
  TQSchedulerRecordStart( ctx->tq_ctx );
  FinalClockTick();
  TQSchedulerRecordStop( ctx->tq_ctx );
}

static void ScoreTqReqTimeoutMrsp_Pre_Scheduler_Prepare(
  ScoreTqReqTimeoutMrsp_Context      *ctx,
  ScoreTqReqTimeoutMrsp_Pre_Scheduler state
)
{
  switch ( state ) {
    case ScoreTqReqTimeoutMrsp_Pre_Scheduler_Same: {
      /*
       * While the home scheduler of the thread is equal to the home scheduler
       * of the thread queue owner.
       */
      ctx->other_scheduler = false;

      TQSetScheduler(
        ctx->tq_ctx,
        TQ_BLOCKER_A,
        SCHEDULER_A_ID,
        PRIO_LOW
      );
      RemoveProcessor( SCHEDULER_B_ID, 1 );
      AddProcessor( SCHEDULER_A_ID, 1 );
      ctx->restore_scheduler = true;
      break;
    }

    case ScoreTqReqTimeoutMrsp_Pre_Scheduler_Other: {
      /*
       * While the home scheduler of the thread is not equal to the home
       * scheduler of the thread queue owner.
       */
      ctx->other_scheduler = true;

      TQSetScheduler(
        ctx->tq_ctx,
        TQ_BLOCKER_A,
        SCHEDULER_B_ID,
        PRIO_NORMAL
      );
      break;
    }

    case ScoreTqReqTimeoutMrsp_Pre_Scheduler_NA:
      break;
  }
}

static void ScoreTqReqTimeoutMrsp_Pre_WaitState_Prepare(
  ScoreTqReqTimeoutMrsp_Context      *ctx,
  ScoreTqReqTimeoutMrsp_Pre_WaitState state
)
{
  switch ( state ) {
    case ScoreTqReqTimeoutMrsp_Pre_WaitState_IntendToBlock: {
      /*
       * While the thread of the timeout operation is in the intend to block
       * wait state.
       */
      Per_CPU_Control *cpu;

      TQEnqueuePrepare( ctx->tq_ctx );
      TQSendAndWaitForIntendToBlock(
        ctx->tq_ctx,
        TQ_BLOCKER_A,
        TQ_EVENT_ENQUEUE
      );
      cpu = _Thread_Get_CPU( ctx->tq_ctx->worker_tcb[ TQ_BLOCKER_A ] );

      /*
       * We have to make sure that the worker thread inserted its thread
       * timer.  Checking the intend to block wait state is not enough to
       * ensure this.
       */
      while ( cpu->thread_dispatch_disable_level != 0 ) {
        /* Wait */
      }

      Tick( ctx );
      WaitForExecutionStop( ctx->tq_ctx->worker_id[ TQ_BLOCKER_A ] );
      TQEnqueueDone( ctx->tq_ctx );
      break;
    }

    case ScoreTqReqTimeoutMrsp_Pre_WaitState_ReadyAgain: {
      /*
       * While the thread of the timeout operation is in the ready again wait
       * state.
       */
      TQEnqueuePrepare( ctx->tq_ctx );
      TQSendAndWaitForIntendToBlock(
        ctx->tq_ctx,
        TQ_BLOCKER_A,
        TQ_EVENT_ENQUEUE | TQ_EVENT_TIMEOUT | TQ_EVENT_SURRENDER |
          TQ_EVENT_SCHEDULER_RECORD_STOP
      );
      TQSchedulerRecordStart( ctx->tq_ctx );
      TQEnqueueDone( ctx->tq_ctx );
      WaitForExecutionStop( ctx->tq_ctx->worker_id[ TQ_BLOCKER_A ] );
      break;
    }

    case ScoreTqReqTimeoutMrsp_Pre_WaitState_NA:
      break;
  }
}

static void ScoreTqReqTimeoutMrsp_Post_Status_Check(
  ScoreTqReqTimeoutMrsp_Context    *ctx,
  ScoreTqReqTimeoutMrsp_Post_Status state
)
{
  switch ( state ) {
    case ScoreTqReqTimeoutMrsp_Post_Status_Ok: {
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

    case ScoreTqReqTimeoutMrsp_Post_Status_Timeout: {
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

    case ScoreTqReqTimeoutMrsp_Post_Status_NA:
      break;
  }
}

static void ScoreTqReqTimeoutMrsp_Post_Unblock_Check(
  ScoreTqReqTimeoutMrsp_Context     *ctx,
  ScoreTqReqTimeoutMrsp_Post_Unblock state
)
{
  size_t i;

  i = 0;

  switch ( state ) {
    case ScoreTqReqTimeoutMrsp_Post_Unblock_No: {
      /*
       * The thread of the timeout operation shall not be unblocked by the
       * timeout operation.
       */
      T_eq_ptr( GetUnblock( ctx, &i ), NULL );
      break;
    }

    case ScoreTqReqTimeoutMrsp_Post_Unblock_NA:
      break;
  }
}

static void ScoreTqReqTimeoutMrsp_Setup( ScoreTqReqTimeoutMrsp_Context *ctx )
{
  TQReset( ctx->tq_ctx );
}

static void ScoreTqReqTimeoutMrsp_Setup_Wrap( void *arg )
{
  ScoreTqReqTimeoutMrsp_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  ScoreTqReqTimeoutMrsp_Setup( ctx );
}

static void ScoreTqReqTimeoutMrsp_Teardown(
  ScoreTqReqTimeoutMrsp_Context *ctx
)
{
  TQReset( ctx->tq_ctx );
}

static void ScoreTqReqTimeoutMrsp_Teardown_Wrap( void *arg )
{
  ScoreTqReqTimeoutMrsp_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  ScoreTqReqTimeoutMrsp_Teardown( ctx );
}

static void ScoreTqReqTimeoutMrsp_Prepare( ScoreTqReqTimeoutMrsp_Context *ctx )
{
  ctx->restore_scheduler = false;
}

static void ScoreTqReqTimeoutMrsp_Action( ScoreTqReqTimeoutMrsp_Context *ctx )
{
  /*
   * The action is performed by the ``WaitState`` pre-condition preparation.
   */
}

static void ScoreTqReqTimeoutMrsp_Cleanup( ScoreTqReqTimeoutMrsp_Context *ctx )
{
  if ( ctx->restore_scheduler ) {
    RemoveProcessor( SCHEDULER_A_ID, 1 );
    AddProcessor( SCHEDULER_B_ID, 1 );
  }
}

static const ScoreTqReqTimeoutMrsp_Entry
ScoreTqReqTimeoutMrsp_Entries[] = {
  { 0, 0, 0, ScoreTqReqTimeoutMrsp_Post_Status_Timeout,
    ScoreTqReqTimeoutMrsp_Post_Unblock_No },
  { 0, 0, 0, ScoreTqReqTimeoutMrsp_Post_Status_Ok,
    ScoreTqReqTimeoutMrsp_Post_Unblock_No }
};

static const uint8_t
ScoreTqReqTimeoutMrsp_Map[] = {
  0, 1, 0, 1
};

static size_t ScoreTqReqTimeoutMrsp_Scope( void *arg, char *buf, size_t n )
{
  ScoreTqReqTimeoutMrsp_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( ScoreTqReqTimeoutMrsp_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture ScoreTqReqTimeoutMrsp_Fixture = {
  .setup = ScoreTqReqTimeoutMrsp_Setup_Wrap,
  .stop = NULL,
  .teardown = ScoreTqReqTimeoutMrsp_Teardown_Wrap,
  .scope = ScoreTqReqTimeoutMrsp_Scope,
  .initial_context = &ScoreTqReqTimeoutMrsp_Instance
};

static inline ScoreTqReqTimeoutMrsp_Entry ScoreTqReqTimeoutMrsp_PopEntry(
  ScoreTqReqTimeoutMrsp_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return ScoreTqReqTimeoutMrsp_Entries[
    ScoreTqReqTimeoutMrsp_Map[ index ]
  ];
}

static void ScoreTqReqTimeoutMrsp_TestVariant(
  ScoreTqReqTimeoutMrsp_Context *ctx
)
{
  ScoreTqReqTimeoutMrsp_Pre_Scheduler_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  ScoreTqReqTimeoutMrsp_Pre_WaitState_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  ScoreTqReqTimeoutMrsp_Action( ctx );
  ScoreTqReqTimeoutMrsp_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  ScoreTqReqTimeoutMrsp_Post_Unblock_Check( ctx, ctx->Map.entry.Post_Unblock );
}

static T_fixture_node ScoreTqReqTimeoutMrsp_Node;

static T_remark ScoreTqReqTimeoutMrsp_Remark = {
  .next = NULL,
  .remark = "ScoreTqReqTimeoutMrsp"
};

void ScoreTqReqTimeoutMrsp_Run( TQContext *tq_ctx )
{
  ScoreTqReqTimeoutMrsp_Context *ctx;

  ctx = &ScoreTqReqTimeoutMrsp_Instance;
  ctx->tq_ctx = tq_ctx;

  ctx = T_push_fixture(
    &ScoreTqReqTimeoutMrsp_Node,
    &ScoreTqReqTimeoutMrsp_Fixture
  );
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = ScoreTqReqTimeoutMrsp_Pre_Scheduler_Same;
    ctx->Map.pcs[ 0 ] < ScoreTqReqTimeoutMrsp_Pre_Scheduler_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = ScoreTqReqTimeoutMrsp_Pre_WaitState_IntendToBlock;
      ctx->Map.pcs[ 1 ] < ScoreTqReqTimeoutMrsp_Pre_WaitState_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = ScoreTqReqTimeoutMrsp_PopEntry( ctx );
      ScoreTqReqTimeoutMrsp_Prepare( ctx );
      ScoreTqReqTimeoutMrsp_TestVariant( ctx );
      ScoreTqReqTimeoutMrsp_Cleanup( ctx );
    }
  }

  T_add_remark( &ScoreTqReqTimeoutMrsp_Remark );
  T_pop_fixture();
}

/** @} */
