/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreCondReqWait
 */

/*
 * Copyright (C) 2026 embedded brains GmbH & Co. KG
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/statesimpl.h>
#include <rtems/score/threadimpl.h>

#include "tr-cond-wait.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreCondReqWait spec:/score/cond/req/wait
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Recursive_NA : 1;
  uint16_t Pre_Wait_NA : 1;
  uint16_t Pre_Nest_NA : 1;
  uint16_t Post_OwnerDuringCall : 1;
  uint16_t Post_WaitStateDuringCall : 1;
  uint16_t Post_OwnerAfterCall : 1;
  uint16_t Post_NestAfterCall : 1;
  uint16_t Post_Status : 1;
} ScoreCondReqWait_Entry;

/**
 * @brief Test context for spec:/score/cond/req/wait test case.
 */
typedef struct {
  /**
   * @brief This member contains the owner of the mutex during the directive
   *   call.
   */
  rtems_tcb *owner_during_call;

  /**
   * @brief This member contains the condition variable wait state of the
   *   calling thread during the directive call.
   */
  uint32_t wait_state_during_call;

  /**
   * @brief This member contains a copy of the corresponding
   *   ScoreCondReqWait_Run() parameter.
   */
  TQCondContext *tq_ctx;

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
    ScoreCondReqWait_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} ScoreCondReqWait_Context;

static ScoreCondReqWait_Context ScoreCondReqWait_Instance;

static const char *const ScoreCondReqWait_PreDesc_Recursive[] =
  { "No", "Yes", "NA" };

static const char *const ScoreCondReqWait_PreDesc_Wait[] =
  { "Forever", "Timed", "NA" };

static const char *const ScoreCondReqWait_PreDesc_Nest[] =
  { "One", "Many", "NA" };

static const char *const *const ScoreCondReqWait_PreDesc[] = {
  ScoreCondReqWait_PreDesc_Recursive,
  ScoreCondReqWait_PreDesc_Wait,
  ScoreCondReqWait_PreDesc_Nest,
  NULL
};

static void ScoreCondReqWait_Pre_Recursive_Prepare(
  ScoreCondReqWait_Context      *ctx,
  ScoreCondReqWait_Pre_Recursive state
)
{
  switch ( state ) {
    case ScoreCondReqWait_Pre_Recursive_No: {
      /*
       * Where the associated mutex is not recursive.
       */
      if ( ctx->tq_ctx->mtx.recursive == TQ_MTX_RECURSIVE_ALLOWED ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreCondReqWait_Pre_Recursive_Yes: {
      /*
       * Where the associated mutex is recursive.
       */
      if ( ctx->tq_ctx->mtx.recursive != TQ_MTX_RECURSIVE_ALLOWED ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreCondReqWait_Pre_Recursive_NA:
      break;
  }
}

static void ScoreCondReqWait_Pre_Wait_Prepare(
  ScoreCondReqWait_Context *ctx,
  ScoreCondReqWait_Pre_Wait state
)
{
  switch ( state ) {
    case ScoreCondReqWait_Pre_Wait_Forever: {
      /*
       * Where the directive waits without a timeout.
       */
      if ( ctx->tq_ctx->base.wait != TQ_WAIT_FOREVER ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreCondReqWait_Pre_Wait_Timed: {
      /*
       * Where the directive waits with a timeout.
       */
      if ( ctx->tq_ctx->base.wait != TQ_WAIT_TIMED ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreCondReqWait_Pre_Wait_NA:
      break;
  }
}

static void ScoreCondReqWait_Pre_Nest_Prepare(
  ScoreCondReqWait_Context *ctx,
  ScoreCondReqWait_Pre_Nest state
)
{
  switch ( state ) {
    case ScoreCondReqWait_Pre_Nest_One: {
      /*
       * While the calling thread seized the mutex exactly once.
       */
      ctx->tq_ctx->nest = 1;
      break;
    }

    case ScoreCondReqWait_Pre_Nest_Many: {
      /*
       * While the calling thread seized the mutex more than once.
       */
      ctx->tq_ctx->nest = 3;
      break;
    }

    case ScoreCondReqWait_Pre_Nest_NA:
      break;
  }
}

static void ScoreCondReqWait_Post_OwnerDuringCall_Check(
  ScoreCondReqWait_Context             *ctx,
  ScoreCondReqWait_Post_OwnerDuringCall state
)
{
  switch ( state ) {
    case ScoreCondReqWait_Post_OwnerDuringCall_None: {
      /*
       * The mutex shall have no owner during the directive call.
       */
      T_null( ctx->owner_during_call );
      break;
    }

    case ScoreCondReqWait_Post_OwnerDuringCall_NA:
      break;
  }
}

static void ScoreCondReqWait_Post_WaitStateDuringCall_Check(
  ScoreCondReqWait_Context                 *ctx,
  ScoreCondReqWait_Post_WaitStateDuringCall state
)
{
  switch ( state ) {
    case ScoreCondReqWait_Post_WaitStateDuringCall_Condition: {
      /*
       * The calling thread shall be in a condition variable wait during the
       * directive call.
       */
      T_eq_u32(
        ctx->wait_state_during_call,
        STATES_WAITING_FOR_CONDITION_VARIABLE
      );
      break;
    }

    case ScoreCondReqWait_Post_WaitStateDuringCall_NA:
      break;
  }
}

static void ScoreCondReqWait_Post_OwnerAfterCall_Check(
  ScoreCondReqWait_Context            *ctx,
  ScoreCondReqWait_Post_OwnerAfterCall state
)
{
  switch ( state ) {
    case ScoreCondReqWait_Post_OwnerAfterCall_Caller: {
      /*
       * The owner of the mutex after the directive call shall be the calling
       * thread.
       */
      T_eq_ptr(
        ctx->tq_ctx->owner_after,
        ctx->tq_ctx->base.worker_tcb[ TQ_BLOCKER_A ]
      );
      break;
    }

    case ScoreCondReqWait_Post_OwnerAfterCall_NA:
      break;
  }
}

static void ScoreCondReqWait_Post_NestAfterCall_Check(
  ScoreCondReqWait_Context           *ctx,
  ScoreCondReqWait_Post_NestAfterCall state
)
{
  switch ( state ) {
    case ScoreCondReqWait_Post_NestAfterCall_Same: {
      /*
       * The nest level of the mutex after the directive call shall be the nest
       * level before the call.
       */
      T_eq_uint( ctx->tq_ctx->nest_after, ctx->tq_ctx->nest );
      break;
    }

    case ScoreCondReqWait_Post_NestAfterCall_NA:
      break;
  }
}

static void ScoreCondReqWait_Post_Status_Check(
  ScoreCondReqWait_Context    *ctx,
  ScoreCondReqWait_Post_Status state
)
{
  switch ( state ) {
    case ScoreCondReqWait_Post_Status_Ok: {
      /*
       * The return status of the directive call shall be derived from
       * STATUS_SUCCESSFUL.
       */
      T_eq_int(
        ctx->tq_ctx->base.status[ TQ_BLOCKER_A ],
        TQConvertStatus( &ctx->tq_ctx->base, STATUS_SUCCESSFUL )
      );
      break;
    }

    case ScoreCondReqWait_Post_Status_NA:
      break;
  }
}

static void ScoreCondReqWait_Setup( ScoreCondReqWait_Context *ctx )
{
  TQReset( &ctx->tq_ctx->base );
  TQSetPriority( &ctx->tq_ctx->base, TQ_BLOCKER_A, PRIO_VERY_HIGH );
}

static void ScoreCondReqWait_Setup_Wrap( void *arg )
{
  ScoreCondReqWait_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  ScoreCondReqWait_Setup( ctx );
}

static void ScoreCondReqWait_Teardown( ScoreCondReqWait_Context *ctx )
{
  TQReset( &ctx->tq_ctx->base );
}

static void ScoreCondReqWait_Teardown_Wrap( void *arg )
{
  ScoreCondReqWait_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  ScoreCondReqWait_Teardown( ctx );
}

static void ScoreCondReqWait_Prepare( ScoreCondReqWait_Context *ctx )
{
  ctx->owner_during_call = NULL;
  ctx->wait_state_during_call = 0;
  ctx->tq_ctx->owner_after = NULL;
  ctx->tq_ctx->nest_after = 0;
}

static void ScoreCondReqWait_Action( ScoreCondReqWait_Context *ctx )
{
  TQSend( &ctx->tq_ctx->base, TQ_BLOCKER_A, TQ_EVENT_ENQUEUE );
  ctx->owner_during_call = TQGetOwner( &ctx->tq_ctx->base );
  ctx->wait_state_during_call = ctx->tq_ctx->base.worker_tcb[ TQ_BLOCKER_A ]
                                  ->current_state &
                                STATES_WAITING_FOR_CONDITION_VARIABLE;
  TQSurrender( &ctx->tq_ctx->base );
  TQWaitForDone( &ctx->tq_ctx->base, TQ_BLOCKER_A );
}

/* clang-format off */

static const ScoreCondReqWait_Entry
ScoreCondReqWait_Entries[] = {
  { 0, 0, 0, 0, ScoreCondReqWait_Post_OwnerDuringCall_None,
    ScoreCondReqWait_Post_WaitStateDuringCall_Condition,
    ScoreCondReqWait_Post_OwnerAfterCall_Caller,
    ScoreCondReqWait_Post_NestAfterCall_Same, ScoreCondReqWait_Post_Status_NA },
  { 0, 0, 0, 0, ScoreCondReqWait_Post_OwnerDuringCall_None,
    ScoreCondReqWait_Post_WaitStateDuringCall_Condition,
    ScoreCondReqWait_Post_OwnerAfterCall_Caller,
    ScoreCondReqWait_Post_NestAfterCall_Same, ScoreCondReqWait_Post_Status_Ok },
  { 1, 0, 0, 0, ScoreCondReqWait_Post_OwnerDuringCall_NA,
    ScoreCondReqWait_Post_WaitStateDuringCall_NA,
    ScoreCondReqWait_Post_OwnerAfterCall_NA,
    ScoreCondReqWait_Post_NestAfterCall_NA, ScoreCondReqWait_Post_Status_NA }
};

static const uint8_t
ScoreCondReqWait_Map[] = {
  0, 2, 1, 2, 0, 0, 1, 1
};

/* clang-format on */

static size_t ScoreCondReqWait_Scope( void *arg, char *buf, size_t n )
{
  ScoreCondReqWait_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( ScoreCondReqWait_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture ScoreCondReqWait_Fixture = {
  .setup = ScoreCondReqWait_Setup_Wrap,
  .stop = NULL,
  .teardown = ScoreCondReqWait_Teardown_Wrap,
  .scope = ScoreCondReqWait_Scope,
  .initial_context = &ScoreCondReqWait_Instance
};

static const uint8_t ScoreCondReqWait_Weights[] = { 4, 2, 1 };

static void ScoreCondReqWait_Skip(
  ScoreCondReqWait_Context *ctx,
  size_t                    index
)
{
  switch ( index + 1 ) {
    case 1:
      ctx->Map.pcs[ 1 ] = ScoreCondReqWait_Pre_Wait_NA - 1;
      /* Fall through */
    case 2:
      ctx->Map.pcs[ 2 ] = ScoreCondReqWait_Pre_Nest_NA - 1;
      break;
  }
}

static inline ScoreCondReqWait_Entry ScoreCondReqWait_PopEntry(
  ScoreCondReqWait_Context *ctx
)
{
  size_t index;

  if ( ctx->Map.skip ) {
    size_t i;

    ctx->Map.skip = false;
    index = 0;

    for ( i = 0; i < 3; ++i ) {
      index += ScoreCondReqWait_Weights[ i ] * ctx->Map.pcs[ i ];
    }
  } else {
    index = ctx->Map.index;
  }

  ctx->Map.index = index + 1;

  return ScoreCondReqWait_Entries[ ScoreCondReqWait_Map[ index ] ];
}

static void ScoreCondReqWait_TestVariant( ScoreCondReqWait_Context *ctx )
{
  ScoreCondReqWait_Pre_Recursive_Prepare( ctx, ctx->Map.pcs[ 0 ] );

  if ( ctx->Map.skip ) {
    ScoreCondReqWait_Skip( ctx, 0 );
    return;
  }

  ScoreCondReqWait_Pre_Wait_Prepare( ctx, ctx->Map.pcs[ 1 ] );

  if ( ctx->Map.skip ) {
    ScoreCondReqWait_Skip( ctx, 1 );
    return;
  }

  ScoreCondReqWait_Pre_Nest_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  ScoreCondReqWait_Action( ctx );
  ScoreCondReqWait_Post_OwnerDuringCall_Check(
    ctx,
    ctx->Map.entry.Post_OwnerDuringCall
  );
  ScoreCondReqWait_Post_WaitStateDuringCall_Check(
    ctx,
    ctx->Map.entry.Post_WaitStateDuringCall
  );
  ScoreCondReqWait_Post_OwnerAfterCall_Check(
    ctx,
    ctx->Map.entry.Post_OwnerAfterCall
  );
  ScoreCondReqWait_Post_NestAfterCall_Check(
    ctx,
    ctx->Map.entry.Post_NestAfterCall
  );
  ScoreCondReqWait_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
}

static T_fixture_node ScoreCondReqWait_Node;

static T_remark ScoreCondReqWait_Remark = {
  .next = NULL,
  .remark = "ScoreCondReqWait"
};

void ScoreCondReqWait_Run( TQCondContext *tq_ctx )
{
  ScoreCondReqWait_Context *ctx;

  ctx = &ScoreCondReqWait_Instance;
  ctx->tq_ctx = tq_ctx;

  ctx = T_push_fixture( &ScoreCondReqWait_Node, &ScoreCondReqWait_Fixture );
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;
  ctx->Map.skip = false;

  for (
    ctx->Map.pcs[ 0 ] = ScoreCondReqWait_Pre_Recursive_No;
    ctx->Map.pcs[ 0 ] < ScoreCondReqWait_Pre_Recursive_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = ScoreCondReqWait_Pre_Wait_Forever;
      ctx->Map.pcs[ 1 ] < ScoreCondReqWait_Pre_Wait_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = ScoreCondReqWait_Pre_Nest_One;
        ctx->Map.pcs[ 2 ] < ScoreCondReqWait_Pre_Nest_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        ctx->Map.entry = ScoreCondReqWait_PopEntry( ctx );

        if ( ctx->Map.entry.Skip ) {
          continue;
        }

        ScoreCondReqWait_Prepare( ctx );
        ScoreCondReqWait_TestVariant( ctx );
      }
    }
  }

  T_add_remark( &ScoreCondReqWait_Remark );
  T_pop_fixture();
}

/** @} */
