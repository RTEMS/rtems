/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreMtxReqSeizeTry
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

#include "tr-mtx-seize-try.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreMtxReqSeizeTry spec:/score/mtx/req/seize-try
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Protocol_NA : 1;
  uint16_t Pre_Discipline_NA : 1;
  uint16_t Pre_Recursive_NA : 1;
  uint16_t Pre_Owner_NA : 1;
  uint16_t Pre_Priority_NA : 1;
  uint16_t Post_Status : 3;
  uint16_t Post_Owner : 2;
  uint16_t Post_Priority : 2;
} ScoreMtxReqSeizeTry_Entry;

/**
 * @brief Test context for spec:/score/mtx/req/seize-try test case.
 */
typedef struct {
  /**
   * @brief If this member is true, then the calling thread shall be the owner
   *   of the mutex.
   */
  bool owner_caller;

  /**
   * @brief If this member is true, then a thread other than the calling thread
   *   shall be the owner of the mutex.
   */
  bool owner_other;

  /**
   * @brief This member contains the current priority of the calling thread
   *   before the directive call.
   */
  rtems_task_priority priority_before;

  /**
   * @brief This member contains the owner of the mutex after the directive
   *   call.
   */
  const rtems_tcb *owner_after;

  /**
   * @brief This member contains the current priority of the calling thread
   *   after the directive call.
   */
  rtems_task_priority priority_after;

  /**
   * @brief This member contains a copy of the corresponding
   *   ScoreMtxReqSeizeTry_Run() parameter.
   */
  TQMtxContext *tq_ctx;

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 5 ];

    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 5 ];

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
    ScoreMtxReqSeizeTry_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} ScoreMtxReqSeizeTry_Context;

static ScoreMtxReqSeizeTry_Context
  ScoreMtxReqSeizeTry_Instance;

static const char * const ScoreMtxReqSeizeTry_PreDesc_Protocol[] = {
  "Ceiling",
  "MrsP",
  "Other",
  "NA"
};

static const char * const ScoreMtxReqSeizeTry_PreDesc_Discipline[] = {
  "FIFO",
  "Priority",
  "NA"
};

static const char * const ScoreMtxReqSeizeTry_PreDesc_Recursive[] = {
  "Allowed",
  "Unavailable",
  "Deadlock",
  "NA"
};

static const char * const ScoreMtxReqSeizeTry_PreDesc_Owner[] = {
  "None",
  "Caller",
  "Other",
  "NA"
};

static const char * const ScoreMtxReqSeizeTry_PreDesc_Priority[] = {
  "High",
  "Equal",
  "Low",
  "NA"
};

static const char * const * const ScoreMtxReqSeizeTry_PreDesc[] = {
  ScoreMtxReqSeizeTry_PreDesc_Protocol,
  ScoreMtxReqSeizeTry_PreDesc_Discipline,
  ScoreMtxReqSeizeTry_PreDesc_Recursive,
  ScoreMtxReqSeizeTry_PreDesc_Owner,
  ScoreMtxReqSeizeTry_PreDesc_Priority,
  NULL
};

typedef ScoreMtxReqSeizeTry_Context Context;

static Status_Control Status( const Context *ctx, Status_Control status )
{
  return TQConvertStatus( &ctx->tq_ctx->base, status );
}

static bool IsEnqueueStatus( const Context *ctx, Status_Control expected )
{
  return ctx->tq_ctx->base.status[ TQ_BLOCKER_A ] == Status( ctx, expected );
}

static void Action( Context *ctx )
{
  TQSetScheduler(
    &ctx->tq_ctx->base,
    TQ_BLOCKER_A,
    SCHEDULER_A_ID,
    PRIO_VERY_HIGH
  );

  if ( ctx->owner_caller ) {
    TQSend( &ctx->tq_ctx->base, TQ_BLOCKER_A, TQ_EVENT_ENQUEUE );
  } else if ( ctx->owner_other ) {
    TQSend( &ctx->tq_ctx->base, TQ_BLOCKER_B, TQ_EVENT_ENQUEUE );
  }

  TQSetPriority( &ctx->tq_ctx->base, TQ_BLOCKER_A, ctx->priority_before );
  TQSend( &ctx->tq_ctx->base, TQ_BLOCKER_A, TQ_EVENT_ENQUEUE );
  ctx->owner_after = TQGetOwner( &ctx->tq_ctx->base );
  ctx->priority_after = TQGetPriority( &ctx->tq_ctx->base, TQ_BLOCKER_A );

  if ( ctx->owner_caller ) {
    TQSend( &ctx->tq_ctx->base, TQ_BLOCKER_A, TQ_EVENT_SURRENDER );
  } else if ( ctx->owner_other ) {
    TQSend( &ctx->tq_ctx->base, TQ_BLOCKER_B, TQ_EVENT_SURRENDER );
  }

  if ( IsEnqueueStatus( ctx, STATUS_SUCCESSFUL ) ) {
    TQSend( &ctx->tq_ctx->base, TQ_BLOCKER_A, TQ_EVENT_SURRENDER );
  }
}

static void ActionSticky( Context *ctx )
{
  TQSetScheduler(
    &ctx->tq_ctx->base,
    TQ_BLOCKER_A,
    SCHEDULER_B_ID,
    PRIO_VERY_HIGH
  );

  if ( ctx->owner_caller ) {
    TQSendAndSynchronizeRunner(
      &ctx->tq_ctx->base,
      TQ_BLOCKER_A,
      TQ_EVENT_ENQUEUE
    );
  } else if ( ctx->owner_other ) {
    SetSelfScheduler( SCHEDULER_B_ID, PRIO_ULTRA_HIGH );
    TQSendAndSynchronizeRunner(
      &ctx->tq_ctx->base,
      TQ_BLOCKER_B,
      TQ_EVENT_ENQUEUE
    );
    SetSelfScheduler( SCHEDULER_A_ID, PRIO_ULTRA_HIGH );
  }

  TQSetPriority( &ctx->tq_ctx->base, TQ_BLOCKER_A, ctx->priority_before );
  TQClearDone( &ctx->tq_ctx->base, TQ_BLOCKER_A );
  TQSendAndWaitForExecutionStopOrIntendToBlock(
    &ctx->tq_ctx->base,
    TQ_BLOCKER_A,
    TQ_EVENT_ENQUEUE
  );
  ctx->owner_after = TQGetOwner( &ctx->tq_ctx->base );
  ctx->priority_after = TQGetPriority( &ctx->tq_ctx->base, TQ_BLOCKER_A );

  if ( ctx->owner_caller ) {
    TQSendAndSynchronizeRunner(
      &ctx->tq_ctx->base,
      TQ_BLOCKER_A,
      TQ_EVENT_SURRENDER
    );
  } else if ( ctx->owner_other ) {
    SetSelfScheduler( SCHEDULER_B_ID, PRIO_ULTRA_HIGH );
    TQSendAndSynchronizeRunner(
      &ctx->tq_ctx->base,
      TQ_BLOCKER_B,
      TQ_EVENT_SURRENDER
    );
    SetSelfScheduler( SCHEDULER_A_ID, PRIO_NORMAL );
  }

  TQWaitForDone( &ctx->tq_ctx->base, TQ_BLOCKER_A );

  if ( IsEnqueueStatus( ctx, STATUS_SUCCESSFUL ) ) {
    TQSendAndSynchronizeRunner(
      &ctx->tq_ctx->base,
      TQ_BLOCKER_A,
      TQ_EVENT_SURRENDER
    );
  }
}

static void ScoreMtxReqSeizeTry_Pre_Protocol_Prepare(
  ScoreMtxReqSeizeTry_Context     *ctx,
  ScoreMtxReqSeizeTry_Pre_Protocol state
)
{
  switch ( state ) {
    case ScoreMtxReqSeizeTry_Pre_Protocol_Ceiling: {
      /*
       * Where the mutex uses the priority ceiling locking protocol.
       */
      if (
        ctx->tq_ctx->priority_ceiling == PRIO_INVALID ||
        ctx->tq_ctx->base.enqueue_variant == TQ_ENQUEUE_STICKY
      ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreMtxReqSeizeTry_Pre_Protocol_MrsP: {
      /*
       * Where the mutex uses the MrsP locking protocol.
       */
      if (
        ctx->tq_ctx->priority_ceiling == PRIO_INVALID ||
        ctx->tq_ctx->base.enqueue_variant != TQ_ENQUEUE_STICKY
      ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreMtxReqSeizeTry_Pre_Protocol_Other: {
      /*
       * Where the mutex does not use the priority ceiling or MrsP locking
       * protocol.
       */
      if ( ctx->tq_ctx->priority_ceiling != PRIO_INVALID ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreMtxReqSeizeTry_Pre_Protocol_NA:
      break;
  }
}

static void ScoreMtxReqSeizeTry_Pre_Discipline_Prepare(
  ScoreMtxReqSeizeTry_Context       *ctx,
  ScoreMtxReqSeizeTry_Pre_Discipline state
)
{
  switch ( state ) {
    case ScoreMtxReqSeizeTry_Pre_Discipline_FIFO: {
      /*
       * Where the thread queue of the mutex uses the FIFO discipline.
       */
      if ( ctx->tq_ctx->base.discipline != TQ_FIFO ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreMtxReqSeizeTry_Pre_Discipline_Priority: {
      /*
       * Where the thread queue of the mutex uses the priority discipline.
       */
      if ( ctx->tq_ctx->base.discipline != TQ_PRIORITY ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreMtxReqSeizeTry_Pre_Discipline_NA:
      break;
  }
}

static void ScoreMtxReqSeizeTry_Pre_Recursive_Prepare(
  ScoreMtxReqSeizeTry_Context      *ctx,
  ScoreMtxReqSeizeTry_Pre_Recursive state
)
{
  switch ( state ) {
    case ScoreMtxReqSeizeTry_Pre_Recursive_Allowed: {
      /*
       * Where a recursive seize of the mutex is allowed.
       */
      if ( ctx->tq_ctx->recursive != TQ_MTX_RECURSIVE_ALLOWED ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreMtxReqSeizeTry_Pre_Recursive_Unavailable: {
      /*
       * Where a recursive seize of the mutex results in an unavailable status.
       */
      if ( ctx->tq_ctx->recursive != TQ_MTX_RECURSIVE_UNAVAILABLE ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreMtxReqSeizeTry_Pre_Recursive_Deadlock: {
      /*
       * Where a recursive seize of the mutex results in a deadlock status.
       */
      if ( ctx->tq_ctx->recursive != TQ_MTX_RECURSIVE_DEADLOCK ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreMtxReqSeizeTry_Pre_Recursive_NA:
      break;
  }
}

static void ScoreMtxReqSeizeTry_Pre_Owner_Prepare(
  ScoreMtxReqSeizeTry_Context  *ctx,
  ScoreMtxReqSeizeTry_Pre_Owner state
)
{
  switch ( state ) {
    case ScoreMtxReqSeizeTry_Pre_Owner_None: {
      /*
       * While the mutex has no owner.
       */
      /* This is the default */
      break;
    }

    case ScoreMtxReqSeizeTry_Pre_Owner_Caller: {
      /*
       * While the owner of the mutex is the calling thread.
       */
      ctx->owner_caller = true;
      break;
    }

    case ScoreMtxReqSeizeTry_Pre_Owner_Other: {
      /*
       * While the owner of the mutex is a thread other than the calling
       * thread.
       */
      ctx->owner_other = true;
      break;
    }

    case ScoreMtxReqSeizeTry_Pre_Owner_NA:
      break;
  }
}

static void ScoreMtxReqSeizeTry_Pre_Priority_Prepare(
  ScoreMtxReqSeizeTry_Context     *ctx,
  ScoreMtxReqSeizeTry_Pre_Priority state
)
{
  switch ( state ) {
    case ScoreMtxReqSeizeTry_Pre_Priority_High: {
      /*
       * While the calling thread has a current priority higher than the
       * priority ceiling.
       */
      ctx->priority_before = ctx->tq_ctx->priority_ceiling - 1;
      break;
    }

    case ScoreMtxReqSeizeTry_Pre_Priority_Equal: {
      /*
       * While the calling thread has a current priority equal to the priority
       * ceiling.
       */
      ctx->priority_before = ctx->tq_ctx->priority_ceiling;
      break;
    }

    case ScoreMtxReqSeizeTry_Pre_Priority_Low: {
      /*
       * While the calling thread has a current priority lower than the
       * priority ceiling.
       */
      ctx->priority_before = ctx->tq_ctx->priority_ceiling + 1;
      break;
    }

    case ScoreMtxReqSeizeTry_Pre_Priority_NA:
      break;
  }
}

static void ScoreMtxReqSeizeTry_Post_Status_Check(
  ScoreMtxReqSeizeTry_Context    *ctx,
  ScoreMtxReqSeizeTry_Post_Status state
)
{
  switch ( state ) {
    case ScoreMtxReqSeizeTry_Post_Status_Ok: {
      /*
       * The return status of the directive call shall be derived from
       * STATUS_SUCCESSFUL.
       */
      T_true( IsEnqueueStatus( ctx, STATUS_SUCCESSFUL ) );
      break;
    }

    case ScoreMtxReqSeizeTry_Post_Status_MutexCeilingViolated: {
      /*
       * The return status of the directive call shall be derived from
       * STATUS_MUTEX_CEILING_VIOLATED.
       */
      T_true( IsEnqueueStatus( ctx, STATUS_MUTEX_CEILING_VIOLATED ) );
      break;
    }

    case ScoreMtxReqSeizeTry_Post_Status_Deadlock: {
      /*
       * The return status of the directive call shall be derived from
       * STATUS_DEADLOCK.
       */
      T_true( IsEnqueueStatus( ctx, STATUS_DEADLOCK ) );
      break;
    }

    case ScoreMtxReqSeizeTry_Post_Status_Unavailable: {
      /*
       * The return status of the directive call shall be derived from
       * STATUS_UNAVAILABLE.
       */
      T_true( IsEnqueueStatus( ctx, STATUS_UNAVAILABLE ) );
      break;
    }

    case ScoreMtxReqSeizeTry_Post_Status_NA:
      break;
  }
}

static void ScoreMtxReqSeizeTry_Post_Owner_Check(
  ScoreMtxReqSeizeTry_Context   *ctx,
  ScoreMtxReqSeizeTry_Post_Owner state
)
{
  switch ( state ) {
    case ScoreMtxReqSeizeTry_Post_Owner_Other: {
      /*
       * The owner of the mutex shall not be modified.
       */
      T_eq_ptr(
        ctx->owner_after,
        ctx->tq_ctx->base.worker_tcb[ TQ_BLOCKER_B ]
      );
      break;
    }

    case ScoreMtxReqSeizeTry_Post_Owner_Caller: {
      /*
       * The owner of the mutex shall be the calling thread.
       */
      T_eq_ptr(
        ctx->owner_after,
        ctx->tq_ctx->base.worker_tcb[ TQ_BLOCKER_A ]
      );
      break;
    }

    case ScoreMtxReqSeizeTry_Post_Owner_None: {
      /*
       * The mutex shall have no owner.
       */
      T_null( ctx->owner_after );
      break;
    }

    case ScoreMtxReqSeizeTry_Post_Owner_NA:
      break;
  }
}

static void ScoreMtxReqSeizeTry_Post_Priority_Check(
  ScoreMtxReqSeizeTry_Context      *ctx,
  ScoreMtxReqSeizeTry_Post_Priority state
)
{
  switch ( state ) {
    case ScoreMtxReqSeizeTry_Post_Priority_Nop: {
      /*
       * The priorities of the calling thread shall not be modified.
       */
      T_eq_u32( ctx->priority_after, ctx->priority_before );
      break;
    }

    case ScoreMtxReqSeizeTry_Post_Priority_Ceiling: {
      /*
       * The calling thread shall use the priority ceiling of the mutex.
       */
      T_eq_u32( ctx->priority_after, ctx->tq_ctx->priority_ceiling );
      break;
    }

    case ScoreMtxReqSeizeTry_Post_Priority_NA:
      break;
  }
}

static void ScoreMtxReqSeizeTry_Prepare( ScoreMtxReqSeizeTry_Context *ctx )
{
  ctx->owner_caller = false;
  ctx->owner_other = false;
  ctx->priority_before = PRIO_VERY_HIGH;
}

static void ScoreMtxReqSeizeTry_Action( ScoreMtxReqSeizeTry_Context *ctx )
{
  TQSetScheduler(
    &ctx->tq_ctx->base,
    TQ_BLOCKER_B,
    SCHEDULER_A_ID,
    PRIO_VERY_HIGH
  );

  if ( ctx->tq_ctx->base.enqueue_variant == TQ_ENQUEUE_STICKY ) {
    ActionSticky( ctx );
  } else {
    Action( ctx );
  }
}

static const ScoreMtxReqSeizeTry_Entry
ScoreMtxReqSeizeTry_Entries[] = {
  { 1, 0, 0, 0, 0, 0, ScoreMtxReqSeizeTry_Post_Status_NA,
    ScoreMtxReqSeizeTry_Post_Owner_NA, ScoreMtxReqSeizeTry_Post_Priority_NA },
  { 0, 0, 0, 0, 0, 1, ScoreMtxReqSeizeTry_Post_Status_Ok,
    ScoreMtxReqSeizeTry_Post_Owner_Caller,
    ScoreMtxReqSeizeTry_Post_Priority_Nop },
  { 0, 0, 0, 0, 0, 0, ScoreMtxReqSeizeTry_Post_Status_Unavailable,
    ScoreMtxReqSeizeTry_Post_Owner_Other, ScoreMtxReqSeizeTry_Post_Priority_Nop },
  { 0, 0, 0, 0, 0, 1, ScoreMtxReqSeizeTry_Post_Status_Unavailable,
    ScoreMtxReqSeizeTry_Post_Owner_Other, ScoreMtxReqSeizeTry_Post_Priority_Nop },
  { 0, 0, 0, 0, 0, 0, ScoreMtxReqSeizeTry_Post_Status_Ok,
    ScoreMtxReqSeizeTry_Post_Owner_Caller,
    ScoreMtxReqSeizeTry_Post_Priority_Ceiling },
  { 0, 0, 0, 0, 0, 0, ScoreMtxReqSeizeTry_Post_Status_MutexCeilingViolated,
    ScoreMtxReqSeizeTry_Post_Owner_None, ScoreMtxReqSeizeTry_Post_Priority_Nop },
  { 1, 0, 0, 0, 0, 0, ScoreMtxReqSeizeTry_Post_Status_NA,
    ScoreMtxReqSeizeTry_Post_Owner_NA, ScoreMtxReqSeizeTry_Post_Priority_NA },
  { 0, 0, 0, 0, 0, 1, ScoreMtxReqSeizeTry_Post_Status_Unavailable,
    ScoreMtxReqSeizeTry_Post_Owner_Caller,
    ScoreMtxReqSeizeTry_Post_Priority_Nop },
  { 0, 0, 0, 0, 0, 1, ScoreMtxReqSeizeTry_Post_Status_Deadlock,
    ScoreMtxReqSeizeTry_Post_Owner_Caller,
    ScoreMtxReqSeizeTry_Post_Priority_Nop },
  { 0, 0, 0, 0, 0, 0, ScoreMtxReqSeizeTry_Post_Status_Ok,
    ScoreMtxReqSeizeTry_Post_Owner_Caller,
    ScoreMtxReqSeizeTry_Post_Priority_Nop },
  { 0, 0, 0, 0, 0, 0, ScoreMtxReqSeizeTry_Post_Status_Unavailable,
    ScoreMtxReqSeizeTry_Post_Owner_Caller,
    ScoreMtxReqSeizeTry_Post_Priority_Nop },
  { 0, 0, 0, 0, 0, 0, ScoreMtxReqSeizeTry_Post_Status_Deadlock,
    ScoreMtxReqSeizeTry_Post_Owner_Caller,
    ScoreMtxReqSeizeTry_Post_Priority_Nop }
};

static const uint8_t
ScoreMtxReqSeizeTry_Map[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 5, 4, 4, 9, 9, 6, 2, 2, 2, 5, 4, 4, 10, 10, 6, 2, 2, 2, 5, 4, 4, 11, 11,
  6, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 5, 4, 4, 9, 9, 6, 2, 2, 2, 5, 4, 4, 10, 10, 6, 2, 2, 2, 5, 4,
  4, 11, 11, 6, 2, 2, 2, 1, 1, 1, 1, 1, 1, 3, 3, 3, 1, 1, 1, 7, 7, 7, 3, 3, 3,
  1, 1, 1, 8, 8, 8, 3, 3, 3, 1, 1, 1, 1, 1, 1, 3, 3, 3, 1, 1, 1, 7, 7, 7, 3, 3,
  3, 1, 1, 1, 8, 8, 8, 3, 3, 3
};

static size_t ScoreMtxReqSeizeTry_Scope( void *arg, char *buf, size_t n )
{
  ScoreMtxReqSeizeTry_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( ScoreMtxReqSeizeTry_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture ScoreMtxReqSeizeTry_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = ScoreMtxReqSeizeTry_Scope,
  .initial_context = &ScoreMtxReqSeizeTry_Instance
};

static const uint8_t ScoreMtxReqSeizeTry_Weights[] = {
  54, 27, 9, 3, 1
};

static void ScoreMtxReqSeizeTry_Skip(
  ScoreMtxReqSeizeTry_Context *ctx,
  size_t                       index
)
{
  switch ( index + 1 ) {
    case 1:
      ctx->Map.pci[ 1 ] = ScoreMtxReqSeizeTry_Pre_Discipline_NA - 1;
      /* Fall through */
    case 2:
      ctx->Map.pci[ 2 ] = ScoreMtxReqSeizeTry_Pre_Recursive_NA - 1;
      /* Fall through */
    case 3:
      ctx->Map.pci[ 3 ] = ScoreMtxReqSeizeTry_Pre_Owner_NA - 1;
      /* Fall through */
    case 4:
      ctx->Map.pci[ 4 ] = ScoreMtxReqSeizeTry_Pre_Priority_NA - 1;
      break;
  }
}

static inline ScoreMtxReqSeizeTry_Entry ScoreMtxReqSeizeTry_PopEntry(
  ScoreMtxReqSeizeTry_Context *ctx
)
{
  size_t index;

  if ( ctx->Map.skip ) {
    size_t i;

    ctx->Map.skip = false;
    index = 0;

    for ( i = 0; i < 5; ++i ) {
      index += ScoreMtxReqSeizeTry_Weights[ i ] * ctx->Map.pci[ i ];
    }
  } else {
    index = ctx->Map.index;
  }

  ctx->Map.index = index + 1;

  return ScoreMtxReqSeizeTry_Entries[
    ScoreMtxReqSeizeTry_Map[ index ]
  ];
}

static void ScoreMtxReqSeizeTry_SetPreConditionStates(
  ScoreMtxReqSeizeTry_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];

  if ( ctx->Map.entry.Pre_Priority_NA ) {
    ctx->Map.pcs[ 4 ] = ScoreMtxReqSeizeTry_Pre_Priority_NA;
  } else {
    ctx->Map.pcs[ 4 ] = ctx->Map.pci[ 4 ];
  }
}

static void ScoreMtxReqSeizeTry_TestVariant( ScoreMtxReqSeizeTry_Context *ctx )
{
  ScoreMtxReqSeizeTry_Pre_Protocol_Prepare( ctx, ctx->Map.pcs[ 0 ] );

  if ( ctx->Map.skip ) {
    ScoreMtxReqSeizeTry_Skip( ctx, 0 );
    return;
  }

  ScoreMtxReqSeizeTry_Pre_Discipline_Prepare( ctx, ctx->Map.pcs[ 1 ] );

  if ( ctx->Map.skip ) {
    ScoreMtxReqSeizeTry_Skip( ctx, 1 );
    return;
  }

  ScoreMtxReqSeizeTry_Pre_Recursive_Prepare( ctx, ctx->Map.pcs[ 2 ] );

  if ( ctx->Map.skip ) {
    ScoreMtxReqSeizeTry_Skip( ctx, 2 );
    return;
  }

  ScoreMtxReqSeizeTry_Pre_Owner_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  ScoreMtxReqSeizeTry_Pre_Priority_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  ScoreMtxReqSeizeTry_Action( ctx );
  ScoreMtxReqSeizeTry_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  ScoreMtxReqSeizeTry_Post_Owner_Check( ctx, ctx->Map.entry.Post_Owner );
  ScoreMtxReqSeizeTry_Post_Priority_Check( ctx, ctx->Map.entry.Post_Priority );
}

static T_fixture_node ScoreMtxReqSeizeTry_Node;

static T_remark ScoreMtxReqSeizeTry_Remark = {
  .next = NULL,
  .remark = "ScoreMtxReqSeizeTry"
};

void ScoreMtxReqSeizeTry_Run( TQMtxContext *tq_ctx )
{
  ScoreMtxReqSeizeTry_Context *ctx;

  ctx = &ScoreMtxReqSeizeTry_Instance;
  ctx->tq_ctx = tq_ctx;

  ctx = T_push_fixture(
    &ScoreMtxReqSeizeTry_Node,
    &ScoreMtxReqSeizeTry_Fixture
  );
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;
  ctx->Map.skip = false;

  for (
    ctx->Map.pci[ 0 ] = ScoreMtxReqSeizeTry_Pre_Protocol_Ceiling;
    ctx->Map.pci[ 0 ] < ScoreMtxReqSeizeTry_Pre_Protocol_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = ScoreMtxReqSeizeTry_Pre_Discipline_FIFO;
      ctx->Map.pci[ 1 ] < ScoreMtxReqSeizeTry_Pre_Discipline_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = ScoreMtxReqSeizeTry_Pre_Recursive_Allowed;
        ctx->Map.pci[ 2 ] < ScoreMtxReqSeizeTry_Pre_Recursive_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = ScoreMtxReqSeizeTry_Pre_Owner_None;
          ctx->Map.pci[ 3 ] < ScoreMtxReqSeizeTry_Pre_Owner_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          for (
            ctx->Map.pci[ 4 ] = ScoreMtxReqSeizeTry_Pre_Priority_High;
            ctx->Map.pci[ 4 ] < ScoreMtxReqSeizeTry_Pre_Priority_NA;
            ++ctx->Map.pci[ 4 ]
          ) {
            ctx->Map.entry = ScoreMtxReqSeizeTry_PopEntry( ctx );

            if ( ctx->Map.entry.Skip ) {
              continue;
            }

            ScoreMtxReqSeizeTry_SetPreConditionStates( ctx );
            ScoreMtxReqSeizeTry_Prepare( ctx );
            ScoreMtxReqSeizeTry_TestVariant( ctx );
          }
        }
      }
    }
  }

  T_add_remark( &ScoreMtxReqSeizeTry_Remark );
  T_pop_fixture();
}

/** @} */
