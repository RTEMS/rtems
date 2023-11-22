/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreMtxReqSeizeWait
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

#include "tr-mtx-seize-wait.h"
#include "tr-tq-enqueue-ceiling.h"
#include "tr-tq-enqueue-deadlock.h"
#include "tr-tq-enqueue-fifo.h"
#include "tr-tq-enqueue-priority-inherit.h"
#include "tr-tq-enqueue-priority.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreMtxReqSeizeWait spec:/score/mtx/req/seize-wait
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef struct {
  uint32_t Skip : 1;
  uint32_t Pre_Protocol_NA : 1;
  uint32_t Pre_Discipline_NA : 1;
  uint32_t Pre_DeadlockResult_NA : 1;
  uint32_t Pre_Recursive_NA : 1;
  uint32_t Pre_Owner_NA : 1;
  uint32_t Pre_Priority_NA : 1;
  uint32_t Post_Status : 3;
  uint32_t Post_Enqueued : 3;
  uint32_t Post_Owner : 2;
  uint32_t Post_Priority : 2;
} ScoreMtxReqSeizeWait_Entry;

/**
 * @brief Test context for spec:/score/mtx/req/seize-wait test case.
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
   * @brief If this member is true, then a deadlock shall occur.
   */
  bool deadlock;

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
   *   ScoreMtxReqSeizeWait_Run() parameter.
   */
  TQMtxContext *tq_ctx;

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 6 ];

    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 6 ];

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
    ScoreMtxReqSeizeWait_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} ScoreMtxReqSeizeWait_Context;

static ScoreMtxReqSeizeWait_Context
  ScoreMtxReqSeizeWait_Instance;

static const char * const ScoreMtxReqSeizeWait_PreDesc_Protocol[] = {
  "None",
  "Inherit",
  "Ceiling",
  "MrsP",
  "NA"
};

static const char * const ScoreMtxReqSeizeWait_PreDesc_Discipline[] = {
  "FIFO",
  "Priority",
  "NA"
};

static const char * const ScoreMtxReqSeizeWait_PreDesc_DeadlockResult[] = {
  "Status",
  "Fatal",
  "NA"
};

static const char * const ScoreMtxReqSeizeWait_PreDesc_Recursive[] = {
  "Allowed",
  "Deadlock",
  "NA"
};

static const char * const ScoreMtxReqSeizeWait_PreDesc_Owner[] = {
  "None",
  "Caller",
  "Other",
  "Deadlock",
  "NA"
};

static const char * const ScoreMtxReqSeizeWait_PreDesc_Priority[] = {
  "High",
  "Equal",
  "Low",
  "NA"
};

static const char * const * const ScoreMtxReqSeizeWait_PreDesc[] = {
  ScoreMtxReqSeizeWait_PreDesc_Protocol,
  ScoreMtxReqSeizeWait_PreDesc_Discipline,
  ScoreMtxReqSeizeWait_PreDesc_DeadlockResult,
  ScoreMtxReqSeizeWait_PreDesc_Recursive,
  ScoreMtxReqSeizeWait_PreDesc_Owner,
  ScoreMtxReqSeizeWait_PreDesc_Priority,
  NULL
};

#if defined(RTEMS_SMP)
#include "tr-tq-enqueue-mrsp.h"
#endif

typedef ScoreMtxReqSeizeWait_Context Context;

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
  TQEvent enqueue;

  TQSetScheduler(
    &ctx->tq_ctx->base,
    TQ_BLOCKER_A,
    SCHEDULER_A_ID,
    PRIO_VERY_HIGH
  );

  if ( ctx->owner_caller ) {
    TQSend( &ctx->tq_ctx->base, TQ_BLOCKER_A, TQ_EVENT_ENQUEUE );
  } else if ( ctx->owner_other ) {
    if ( ctx->deadlock ) {
      TQSend(
        &ctx->tq_ctx->base,
        TQ_BLOCKER_A,
        TQ_EVENT_MUTEX_NO_PROTOCOL_OBTAIN
      );
    }

    TQSend( &ctx->tq_ctx->base, TQ_BLOCKER_B, TQ_EVENT_ENQUEUE );

    if ( ctx->deadlock ) {
      TQSend(
        &ctx->tq_ctx->base,
        TQ_BLOCKER_B,
        TQ_EVENT_MUTEX_NO_PROTOCOL_OBTAIN
      );
    }
  }

  TQSetPriority( &ctx->tq_ctx->base, TQ_BLOCKER_A, ctx->priority_before );

  if ( ctx->tq_ctx->base.deadlock == TQ_DEADLOCK_FATAL ) {
    enqueue = TQ_EVENT_ENQUEUE_FATAL;
  } else {
    enqueue = TQ_EVENT_ENQUEUE;
  }

  TQSend( &ctx->tq_ctx->base, TQ_BLOCKER_A, enqueue );
  ctx->owner_after = TQGetOwner( &ctx->tq_ctx->base );
  ctx->priority_after = TQGetPriority( &ctx->tq_ctx->base, TQ_BLOCKER_A );

  if ( ctx->owner_caller ) {
    TQSend( &ctx->tq_ctx->base, TQ_BLOCKER_A, TQ_EVENT_SURRENDER );
  } else if ( ctx->owner_other ) {
    if ( ctx->deadlock ) {
      TQSend(
        &ctx->tq_ctx->base,
        TQ_BLOCKER_A,
        TQ_EVENT_MUTEX_NO_PROTOCOL_RELEASE
      );
    }

    TQSend( &ctx->tq_ctx->base, TQ_BLOCKER_B, TQ_EVENT_SURRENDER );

    if ( ctx->deadlock ) {
      TQSend(
        &ctx->tq_ctx->base,
        TQ_BLOCKER_B,
        TQ_EVENT_MUTEX_NO_PROTOCOL_RELEASE
      );
    }
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
    if ( ctx->deadlock ) {
      TQSendAndSynchronizeRunner(
        &ctx->tq_ctx->base,
        TQ_BLOCKER_A,
        TQ_EVENT_MUTEX_NO_PROTOCOL_OBTAIN
      );
    }

    SetSelfScheduler( SCHEDULER_B_ID, PRIO_ULTRA_HIGH );
    TQSendAndSynchronizeRunner(
      &ctx->tq_ctx->base,
      TQ_BLOCKER_B,
      TQ_EVENT_ENQUEUE
    );

    if ( ctx->deadlock ) {
      TQSendAndWaitForExecutionStop(
        &ctx->tq_ctx->base,
        TQ_BLOCKER_B,
        TQ_EVENT_MUTEX_NO_PROTOCOL_OBTAIN
      );
    }

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
    if ( ctx->deadlock ) {
      TQSendAndSynchronizeRunner(
        &ctx->tq_ctx->base,
        TQ_BLOCKER_A,
        TQ_EVENT_MUTEX_NO_PROTOCOL_RELEASE
      );
    }

    SetSelfScheduler( SCHEDULER_B_ID, PRIO_ULTRA_HIGH );
    TQSendAndSynchronizeRunner(
      &ctx->tq_ctx->base,
      TQ_BLOCKER_B,
      TQ_EVENT_SURRENDER
    );

    if ( ctx->deadlock ) {
      TQSendAndSynchronizeRunner(
        &ctx->tq_ctx->base,
        TQ_BLOCKER_B,
        TQ_EVENT_MUTEX_NO_PROTOCOL_RELEASE
      );
    }

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

static void ScoreMtxReqSeizeWait_Pre_Protocol_Prepare(
  ScoreMtxReqSeizeWait_Context     *ctx,
  ScoreMtxReqSeizeWait_Pre_Protocol state
)
{
  switch ( state ) {
    case ScoreMtxReqSeizeWait_Pre_Protocol_None: {
      /*
       * Where the mutex does not use a locking protocol.
       */
      if ( ctx->tq_ctx->protocol != TQ_MTX_NO_PROTOCOL ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreMtxReqSeizeWait_Pre_Protocol_Inherit: {
      /*
       * Where the mutex uses the priority inheritance locking protocol.
       */
      if ( ctx->tq_ctx->protocol != TQ_MTX_PRIORITY_INHERIT ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreMtxReqSeizeWait_Pre_Protocol_Ceiling: {
      /*
       * Where the mutex uses the priority ceiling locking protocol.
       */
      if ( ctx->tq_ctx->protocol != TQ_MTX_PRIORITY_CEILING ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreMtxReqSeizeWait_Pre_Protocol_MrsP: {
      /*
       * Where the mutex uses the MrsP locking protocol.
       */
      if ( ctx->tq_ctx->protocol != TQ_MTX_MRSP ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreMtxReqSeizeWait_Pre_Protocol_NA:
      break;
  }
}

static void ScoreMtxReqSeizeWait_Pre_Discipline_Prepare(
  ScoreMtxReqSeizeWait_Context       *ctx,
  ScoreMtxReqSeizeWait_Pre_Discipline state
)
{
  switch ( state ) {
    case ScoreMtxReqSeizeWait_Pre_Discipline_FIFO: {
      /*
       * Where the thread queue of the mutex uses the FIFO discipline.
       */
      if ( ctx->tq_ctx->base.discipline != TQ_FIFO ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreMtxReqSeizeWait_Pre_Discipline_Priority: {
      /*
       * Where the thread queue of the mutex uses the priority discipline.
       */
      if ( ctx->tq_ctx->base.discipline != TQ_PRIORITY ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreMtxReqSeizeWait_Pre_Discipline_NA:
      break;
  }
}

static void ScoreMtxReqSeizeWait_Pre_DeadlockResult_Prepare(
  ScoreMtxReqSeizeWait_Context           *ctx,
  ScoreMtxReqSeizeWait_Pre_DeadlockResult state
)
{
  switch ( state ) {
    case ScoreMtxReqSeizeWait_Pre_DeadlockResult_Status: {
      /*
       * Where a detected deadlock results in a return with a status code.
       */
      if ( ctx->tq_ctx->base.deadlock != TQ_DEADLOCK_STATUS ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreMtxReqSeizeWait_Pre_DeadlockResult_Fatal: {
      /*
       * Where a detected deadlock results in a fatal error.
       */
      if ( ctx->tq_ctx->base.deadlock != TQ_DEADLOCK_FATAL ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreMtxReqSeizeWait_Pre_DeadlockResult_NA:
      break;
  }
}

static void ScoreMtxReqSeizeWait_Pre_Recursive_Prepare(
  ScoreMtxReqSeizeWait_Context      *ctx,
  ScoreMtxReqSeizeWait_Pre_Recursive state
)
{
  switch ( state ) {
    case ScoreMtxReqSeizeWait_Pre_Recursive_Allowed: {
      /*
       * Where a recursive seize of the mutex is allowed.
       */
      if ( ctx->tq_ctx->recursive != TQ_MTX_RECURSIVE_ALLOWED ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreMtxReqSeizeWait_Pre_Recursive_Deadlock: {
      /*
       * Where a recursive seize of the mutex results in a deadlock.
       */
      if ( ctx->tq_ctx->recursive != TQ_MTX_RECURSIVE_DEADLOCK ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreMtxReqSeizeWait_Pre_Recursive_NA:
      break;
  }
}

static void ScoreMtxReqSeizeWait_Pre_Owner_Prepare(
  ScoreMtxReqSeizeWait_Context  *ctx,
  ScoreMtxReqSeizeWait_Pre_Owner state
)
{
  switch ( state ) {
    case ScoreMtxReqSeizeWait_Pre_Owner_None: {
      /*
       * While the mutex has no owner.
       */
      /* This is the default */
      break;
    }

    case ScoreMtxReqSeizeWait_Pre_Owner_Caller: {
      /*
       * While the owner of the mutex is the calling thread.
       */
      ctx->owner_caller = true;
      break;
    }

    case ScoreMtxReqSeizeWait_Pre_Owner_Other: {
      /*
       * While the owner of the mutex is a thread other than the calling
       * thread.
       */
      ctx->owner_other = true;
      break;
    }

    case ScoreMtxReqSeizeWait_Pre_Owner_Deadlock: {
      /*
       * While the attempt to seize the mutex results in a deadlock.
       */
      ctx->owner_other = true;
      ctx->deadlock = true;
      break;
    }

    case ScoreMtxReqSeizeWait_Pre_Owner_NA:
      break;
  }
}

static void ScoreMtxReqSeizeWait_Pre_Priority_Prepare(
  ScoreMtxReqSeizeWait_Context     *ctx,
  ScoreMtxReqSeizeWait_Pre_Priority state
)
{
  switch ( state ) {
    case ScoreMtxReqSeizeWait_Pre_Priority_High: {
      /*
       * While the calling thread has a current priority higher than the
       * priority ceiling.
       */
      ctx->priority_before = ctx->tq_ctx->priority_ceiling - 1;
      break;
    }

    case ScoreMtxReqSeizeWait_Pre_Priority_Equal: {
      /*
       * While the calling thread has a current priority equal to the priority
       * ceiling.
       */
      ctx->priority_before = ctx->tq_ctx->priority_ceiling;
      break;
    }

    case ScoreMtxReqSeizeWait_Pre_Priority_Low: {
      /*
       * While the calling thread has a current priority lower than the
       * priority ceiling.
       */
      ctx->priority_before = ctx->tq_ctx->priority_ceiling + 1;
      break;
    }

    case ScoreMtxReqSeizeWait_Pre_Priority_NA:
      break;
  }
}

static void ScoreMtxReqSeizeWait_Post_Status_Check(
  ScoreMtxReqSeizeWait_Context    *ctx,
  ScoreMtxReqSeizeWait_Post_Status state
)
{
  switch ( state ) {
    case ScoreMtxReqSeizeWait_Post_Status_Ok: {
      /*
       * The return status of the directive call shall be derived from
       * STATUS_SUCCESSFUL.
       */
      T_true( IsEnqueueStatus( ctx, STATUS_SUCCESSFUL ) );
      break;
    }

    case ScoreMtxReqSeizeWait_Post_Status_MutexCeilingViolated: {
      /*
       * The return status of the directive call shall be derived from
       * STATUS_MUTEX_CEILING_VIOLATED.
       */
      T_true( IsEnqueueStatus( ctx, STATUS_MUTEX_CEILING_VIOLATED ) );
      break;
    }

    case ScoreMtxReqSeizeWait_Post_Status_DeadlockStatus: {
      /*
       * The return status of the directive call shall be derived from
       * STATUS_DEADLOCK.
       */
      T_true( IsEnqueueStatus( ctx, STATUS_DEADLOCK ) );
      ScoreTqReqEnqueueDeadlock_Run( &ctx->tq_ctx->base );
      break;
    }

    case ScoreMtxReqSeizeWait_Post_Status_DeadlockFatal: {
      /*
       * The system shall terminate with the INTERNAL_ERROR_CORE fatal source
       * and the INTERNAL_ERROR_THREAD_QUEUE_DEADLOCK fatal code.
       */
      T_eq_int( ctx->tq_ctx->base.status[ TQ_BLOCKER_A ], STATUS_DEADLOCK );
      ScoreTqReqEnqueueDeadlock_Run( &ctx->tq_ctx->base );
      break;
    }

    case ScoreMtxReqSeizeWait_Post_Status_NA:
      break;
  }
}

static void ScoreMtxReqSeizeWait_Post_Enqueued_Check(
  ScoreMtxReqSeizeWait_Context      *ctx,
  ScoreMtxReqSeizeWait_Post_Enqueued state
)
{
  switch ( state ) {
    case ScoreMtxReqSeizeWait_Post_Enqueued_No: {
      /*
       * The calling thread shall not be enqueued on the thread queue of the
       * mutex.
       */
      /* The test runner would block if the worker is enqueued */
      break;
    }

    case ScoreMtxReqSeizeWait_Post_Enqueued_FIFO: {
      /*
       * The calling thread shall be enqueued in FIFO order.
       */
      ScoreTqReqEnqueueFifo_Run( &ctx->tq_ctx->base );
      break;
    }

    case ScoreMtxReqSeizeWait_Post_Enqueued_Priority: {
      /*
       * The calling thread shall be enqueued in priority order.
       */
      ScoreTqReqEnqueuePriority_Run( &ctx->tq_ctx->base );
      break;
    }

    case ScoreMtxReqSeizeWait_Post_Enqueued_PriorityInherit: {
      /*
       * The calling thread shall be enqueued in priority order with priorit
       * inheritance.
       */
      ScoreTqReqEnqueuePriorityInherit_Run( &ctx->tq_ctx->base );
      break;
    }

    case ScoreMtxReqSeizeWait_Post_Enqueued_PriorityCeiling: {
      /*
       * The calling thread shall be enqueued in priority order according to
       * the priority ceiling locking protocol.
       */
      ScoreTqReqEnqueueCeiling_Run( &ctx->tq_ctx->base );
      break;
    }

    case ScoreMtxReqSeizeWait_Post_Enqueued_PriorityMrsP: {
      /*
       * The calling thread shall be enqueued in priority order according to
       * the MrsP locking protocol.
       */
      #if defined(RTEMS_SMP)
      ScoreTqReqEnqueueMrsp_Run( &ctx->tq_ctx->base );
      #else
      T_unreachable();
      #endif
      break;
    }

    case ScoreMtxReqSeizeWait_Post_Enqueued_NA:
      break;
  }
}

static void ScoreMtxReqSeizeWait_Post_Owner_Check(
  ScoreMtxReqSeizeWait_Context   *ctx,
  ScoreMtxReqSeizeWait_Post_Owner state
)
{
  switch ( state ) {
    case ScoreMtxReqSeizeWait_Post_Owner_Other: {
      /*
       * The owner of the mutex shall not be modified.
       */
      T_eq_ptr(
        ctx->owner_after,
        ctx->tq_ctx->base.worker_tcb[ TQ_BLOCKER_B ]
      );
      break;
    }

    case ScoreMtxReqSeizeWait_Post_Owner_Caller: {
      /*
       * The owner of the mutex shall be the calling thread.
       */
      T_eq_ptr(
        ctx->owner_after,
        ctx->tq_ctx->base.worker_tcb[ TQ_BLOCKER_A ]
      );
      break;
    }

    case ScoreMtxReqSeizeWait_Post_Owner_None: {
      /*
       * The mutex shall have no owner.
       */
      T_null( ctx->owner_after );
      break;
    }

    case ScoreMtxReqSeizeWait_Post_Owner_NA:
      break;
  }
}

static void ScoreMtxReqSeizeWait_Post_Priority_Check(
  ScoreMtxReqSeizeWait_Context      *ctx,
  ScoreMtxReqSeizeWait_Post_Priority state
)
{
  switch ( state ) {
    case ScoreMtxReqSeizeWait_Post_Priority_Nop: {
      /*
       * The priorities of the calling thread shall not be modified.
       */
      T_eq_u32( ctx->priority_after, ctx->priority_before );
      break;
    }

    case ScoreMtxReqSeizeWait_Post_Priority_Ceiling: {
      /*
       * The calling thread shall use the priority ceiling of the mutex.
       */
      T_eq_u32( ctx->priority_after, ctx->tq_ctx->priority_ceiling );
      break;
    }

    case ScoreMtxReqSeizeWait_Post_Priority_NA:
      break;
  }
}

static void ScoreMtxReqSeizeWait_Prepare( ScoreMtxReqSeizeWait_Context *ctx )
{
  ctx->owner_caller = false;
  ctx->owner_other = false;
  ctx->deadlock = false;
  ctx->priority_before = PRIO_VERY_HIGH;
}

static void ScoreMtxReqSeizeWait_Action( ScoreMtxReqSeizeWait_Context *ctx )
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

static const ScoreMtxReqSeizeWait_Entry
ScoreMtxReqSeizeWait_Entries[] = {
  { 1, 0, 0, 0, 0, 0, 0, ScoreMtxReqSeizeWait_Post_Status_NA,
    ScoreMtxReqSeizeWait_Post_Enqueued_NA, ScoreMtxReqSeizeWait_Post_Owner_NA,
    ScoreMtxReqSeizeWait_Post_Priority_NA },
  { 0, 0, 0, 0, 0, 0, 1, ScoreMtxReqSeizeWait_Post_Status_Ok,
    ScoreMtxReqSeizeWait_Post_Enqueued_No,
    ScoreMtxReqSeizeWait_Post_Owner_Caller,
    ScoreMtxReqSeizeWait_Post_Priority_Nop },
  { 0, 0, 0, 0, 0, 0, 1, ScoreMtxReqSeizeWait_Post_Status_DeadlockStatus,
    ScoreMtxReqSeizeWait_Post_Enqueued_No,
    ScoreMtxReqSeizeWait_Post_Owner_Other,
    ScoreMtxReqSeizeWait_Post_Priority_Nop },
  { 0, 0, 0, 0, 0, 0, 1, ScoreMtxReqSeizeWait_Post_Status_DeadlockFatal,
    ScoreMtxReqSeizeWait_Post_Enqueued_No,
    ScoreMtxReqSeizeWait_Post_Owner_Other,
    ScoreMtxReqSeizeWait_Post_Priority_Nop },
  { 0, 0, 0, 0, 0, 0, 0, ScoreMtxReqSeizeWait_Post_Status_Ok,
    ScoreMtxReqSeizeWait_Post_Enqueued_No,
    ScoreMtxReqSeizeWait_Post_Owner_Caller,
    ScoreMtxReqSeizeWait_Post_Priority_Ceiling },
  { 0, 0, 0, 0, 0, 0, 1, ScoreMtxReqSeizeWait_Post_Status_NA,
    ScoreMtxReqSeizeWait_Post_Enqueued_FIFO,
    ScoreMtxReqSeizeWait_Post_Owner_Other,
    ScoreMtxReqSeizeWait_Post_Priority_Nop },
  { 0, 0, 0, 0, 0, 0, 1, ScoreMtxReqSeizeWait_Post_Status_NA,
    ScoreMtxReqSeizeWait_Post_Enqueued_Priority,
    ScoreMtxReqSeizeWait_Post_Owner_Other,
    ScoreMtxReqSeizeWait_Post_Priority_Nop },
  { 0, 0, 0, 0, 0, 0, 1, ScoreMtxReqSeizeWait_Post_Status_NA,
    ScoreMtxReqSeizeWait_Post_Enqueued_PriorityInherit,
    ScoreMtxReqSeizeWait_Post_Owner_Other,
    ScoreMtxReqSeizeWait_Post_Priority_Nop },
  { 0, 0, 0, 0, 0, 0, 0, ScoreMtxReqSeizeWait_Post_Status_NA,
    ScoreMtxReqSeizeWait_Post_Enqueued_PriorityCeiling,
    ScoreMtxReqSeizeWait_Post_Owner_Other,
    ScoreMtxReqSeizeWait_Post_Priority_Nop },
  { 0, 0, 0, 0, 0, 0, 0, ScoreMtxReqSeizeWait_Post_Status_DeadlockStatus,
    ScoreMtxReqSeizeWait_Post_Enqueued_No,
    ScoreMtxReqSeizeWait_Post_Owner_Other,
    ScoreMtxReqSeizeWait_Post_Priority_Nop },
  { 0, 0, 0, 0, 0, 0, 0, ScoreMtxReqSeizeWait_Post_Status_DeadlockFatal,
    ScoreMtxReqSeizeWait_Post_Enqueued_No,
    ScoreMtxReqSeizeWait_Post_Owner_Other,
    ScoreMtxReqSeizeWait_Post_Priority_Nop },
  { 0, 0, 0, 0, 0, 0, 1, ScoreMtxReqSeizeWait_Post_Status_DeadlockStatus,
    ScoreMtxReqSeizeWait_Post_Enqueued_No,
    ScoreMtxReqSeizeWait_Post_Owner_Caller,
    ScoreMtxReqSeizeWait_Post_Priority_Nop },
  { 0, 0, 0, 0, 0, 0, 1, ScoreMtxReqSeizeWait_Post_Status_DeadlockFatal,
    ScoreMtxReqSeizeWait_Post_Enqueued_No,
    ScoreMtxReqSeizeWait_Post_Owner_Caller,
    ScoreMtxReqSeizeWait_Post_Priority_Nop },
  { 0, 0, 0, 0, 0, 0, 0, ScoreMtxReqSeizeWait_Post_Status_MutexCeilingViolated,
    ScoreMtxReqSeizeWait_Post_Enqueued_No,
    ScoreMtxReqSeizeWait_Post_Owner_None,
    ScoreMtxReqSeizeWait_Post_Priority_Nop },
  { 0, 0, 0, 0, 0, 0, 0, ScoreMtxReqSeizeWait_Post_Status_Ok,
    ScoreMtxReqSeizeWait_Post_Enqueued_No,
    ScoreMtxReqSeizeWait_Post_Owner_Caller,
    ScoreMtxReqSeizeWait_Post_Priority_Nop },
  { 1, 0, 0, 0, 0, 0, 0, ScoreMtxReqSeizeWait_Post_Status_NA,
    ScoreMtxReqSeizeWait_Post_Enqueued_NA, ScoreMtxReqSeizeWait_Post_Owner_NA,
    ScoreMtxReqSeizeWait_Post_Priority_NA },
  { 0, 0, 0, 0, 0, 0, 0, ScoreMtxReqSeizeWait_Post_Status_MutexCeilingViolated,
    ScoreMtxReqSeizeWait_Post_Enqueued_No,
    ScoreMtxReqSeizeWait_Post_Owner_Other,
    ScoreMtxReqSeizeWait_Post_Priority_Nop },
  { 0, 0, 0, 0, 0, 0, 0, ScoreMtxReqSeizeWait_Post_Status_NA,
    ScoreMtxReqSeizeWait_Post_Enqueued_PriorityMrsP,
    ScoreMtxReqSeizeWait_Post_Owner_Other,
    ScoreMtxReqSeizeWait_Post_Priority_Ceiling },
  { 0, 0, 0, 0, 0, 0, 0, ScoreMtxReqSeizeWait_Post_Status_DeadlockStatus,
    ScoreMtxReqSeizeWait_Post_Enqueued_No,
    ScoreMtxReqSeizeWait_Post_Owner_Caller,
    ScoreMtxReqSeizeWait_Post_Priority_Nop },
  { 0, 0, 0, 0, 0, 0, 0, ScoreMtxReqSeizeWait_Post_Status_DeadlockFatal,
    ScoreMtxReqSeizeWait_Post_Enqueued_No,
    ScoreMtxReqSeizeWait_Post_Owner_Caller,
    ScoreMtxReqSeizeWait_Post_Priority_Nop }
};

static const uint8_t
ScoreMtxReqSeizeWait_Map[] = {
  1, 1, 1, 1, 1, 1, 5, 5, 5, 2, 2, 2, 1, 1, 1, 11, 11, 11, 5, 5, 5, 2, 2, 2, 1,
  1, 1, 1, 1, 1, 5, 5, 5, 3, 3, 3, 1, 1, 1, 12, 12, 12, 5, 5, 5, 3, 3, 3, 1, 1,
  1, 1, 1, 1, 6, 6, 6, 2, 2, 2, 1, 1, 1, 11, 11, 11, 6, 6, 6, 2, 2, 2, 1, 1, 1,
  1, 1, 1, 6, 6, 6, 3, 3, 3, 1, 1, 1, 12, 12, 12, 6, 6, 6, 3, 3, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 7, 7,
  7, 2, 2, 2, 1, 1, 1, 11, 11, 11, 7, 7, 7, 2, 2, 2, 1, 1, 1, 1, 1, 1, 7, 7, 7,
  3, 3, 3, 1, 1, 1, 12, 12, 12, 7, 7, 7, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13, 4, 4, 14, 14, 15, 8, 8, 8, 9, 9, 9,
  13, 4, 4, 18, 18, 15, 8, 8, 8, 9, 9, 9, 13, 4, 4, 14, 14, 15, 8, 8, 8, 10,
  10, 10, 13, 4, 4, 19, 19, 15, 8, 8, 8, 10, 10, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13, 4, 4, 14, 14, 15, 16, 17, 17, 16,
  9, 9, 13, 4, 4, 18, 18, 15, 16, 17, 17, 16, 9, 9, 13, 4, 4, 14, 14, 15, 16,
  17, 17, 16, 10, 10, 13, 4, 4, 19, 19, 15, 16, 17, 17, 16, 10, 10
};

static size_t ScoreMtxReqSeizeWait_Scope( void *arg, char *buf, size_t n )
{
  ScoreMtxReqSeizeWait_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( ScoreMtxReqSeizeWait_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture ScoreMtxReqSeizeWait_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = ScoreMtxReqSeizeWait_Scope,
  .initial_context = &ScoreMtxReqSeizeWait_Instance
};

static const uint8_t ScoreMtxReqSeizeWait_Weights[] = {
  96, 48, 24, 12, 3, 1
};

static void ScoreMtxReqSeizeWait_Skip(
  ScoreMtxReqSeizeWait_Context *ctx,
  size_t                        index
)
{
  switch ( index + 1 ) {
    case 1:
      ctx->Map.pci[ 1 ] = ScoreMtxReqSeizeWait_Pre_Discipline_NA - 1;
      /* Fall through */
    case 2:
      ctx->Map.pci[ 2 ] = ScoreMtxReqSeizeWait_Pre_DeadlockResult_NA - 1;
      /* Fall through */
    case 3:
      ctx->Map.pci[ 3 ] = ScoreMtxReqSeizeWait_Pre_Recursive_NA - 1;
      /* Fall through */
    case 4:
      ctx->Map.pci[ 4 ] = ScoreMtxReqSeizeWait_Pre_Owner_NA - 1;
      /* Fall through */
    case 5:
      ctx->Map.pci[ 5 ] = ScoreMtxReqSeizeWait_Pre_Priority_NA - 1;
      break;
  }
}

static inline ScoreMtxReqSeizeWait_Entry ScoreMtxReqSeizeWait_PopEntry(
  ScoreMtxReqSeizeWait_Context *ctx
)
{
  size_t index;

  if ( ctx->Map.skip ) {
    size_t i;

    ctx->Map.skip = false;
    index = 0;

    for ( i = 0; i < 6; ++i ) {
      index += ScoreMtxReqSeizeWait_Weights[ i ] * ctx->Map.pci[ i ];
    }
  } else {
    index = ctx->Map.index;
  }

  ctx->Map.index = index + 1;

  return ScoreMtxReqSeizeWait_Entries[
    ScoreMtxReqSeizeWait_Map[ index ]
  ];
}

static void ScoreMtxReqSeizeWait_SetPreConditionStates(
  ScoreMtxReqSeizeWait_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  ctx->Map.pcs[ 4 ] = ctx->Map.pci[ 4 ];

  if ( ctx->Map.entry.Pre_Priority_NA ) {
    ctx->Map.pcs[ 5 ] = ScoreMtxReqSeizeWait_Pre_Priority_NA;
  } else {
    ctx->Map.pcs[ 5 ] = ctx->Map.pci[ 5 ];
  }
}

static void ScoreMtxReqSeizeWait_TestVariant(
  ScoreMtxReqSeizeWait_Context *ctx
)
{
  ScoreMtxReqSeizeWait_Pre_Protocol_Prepare( ctx, ctx->Map.pcs[ 0 ] );

  if ( ctx->Map.skip ) {
    ScoreMtxReqSeizeWait_Skip( ctx, 0 );
    return;
  }

  ScoreMtxReqSeizeWait_Pre_Discipline_Prepare( ctx, ctx->Map.pcs[ 1 ] );

  if ( ctx->Map.skip ) {
    ScoreMtxReqSeizeWait_Skip( ctx, 1 );
    return;
  }

  ScoreMtxReqSeizeWait_Pre_DeadlockResult_Prepare( ctx, ctx->Map.pcs[ 2 ] );

  if ( ctx->Map.skip ) {
    ScoreMtxReqSeizeWait_Skip( ctx, 2 );
    return;
  }

  ScoreMtxReqSeizeWait_Pre_Recursive_Prepare( ctx, ctx->Map.pcs[ 3 ] );

  if ( ctx->Map.skip ) {
    ScoreMtxReqSeizeWait_Skip( ctx, 3 );
    return;
  }

  ScoreMtxReqSeizeWait_Pre_Owner_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  ScoreMtxReqSeizeWait_Pre_Priority_Prepare( ctx, ctx->Map.pcs[ 5 ] );
  ScoreMtxReqSeizeWait_Action( ctx );
  ScoreMtxReqSeizeWait_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  ScoreMtxReqSeizeWait_Post_Enqueued_Check(
    ctx,
    ctx->Map.entry.Post_Enqueued
  );
  ScoreMtxReqSeizeWait_Post_Owner_Check( ctx, ctx->Map.entry.Post_Owner );
  ScoreMtxReqSeizeWait_Post_Priority_Check(
    ctx,
    ctx->Map.entry.Post_Priority
  );
}

static T_fixture_node ScoreMtxReqSeizeWait_Node;

static T_remark ScoreMtxReqSeizeWait_Remark = {
  .next = NULL,
  .remark = "ScoreMtxReqSeizeWait"
};

void ScoreMtxReqSeizeWait_Run( TQMtxContext *tq_ctx )
{
  ScoreMtxReqSeizeWait_Context *ctx;

  ctx = &ScoreMtxReqSeizeWait_Instance;
  ctx->tq_ctx = tq_ctx;

  ctx = T_push_fixture(
    &ScoreMtxReqSeizeWait_Node,
    &ScoreMtxReqSeizeWait_Fixture
  );
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;
  ctx->Map.skip = false;

  for (
    ctx->Map.pci[ 0 ] = ScoreMtxReqSeizeWait_Pre_Protocol_None;
    ctx->Map.pci[ 0 ] < ScoreMtxReqSeizeWait_Pre_Protocol_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = ScoreMtxReqSeizeWait_Pre_Discipline_FIFO;
      ctx->Map.pci[ 1 ] < ScoreMtxReqSeizeWait_Pre_Discipline_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = ScoreMtxReqSeizeWait_Pre_DeadlockResult_Status;
        ctx->Map.pci[ 2 ] < ScoreMtxReqSeizeWait_Pre_DeadlockResult_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = ScoreMtxReqSeizeWait_Pre_Recursive_Allowed;
          ctx->Map.pci[ 3 ] < ScoreMtxReqSeizeWait_Pre_Recursive_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          for (
            ctx->Map.pci[ 4 ] = ScoreMtxReqSeizeWait_Pre_Owner_None;
            ctx->Map.pci[ 4 ] < ScoreMtxReqSeizeWait_Pre_Owner_NA;
            ++ctx->Map.pci[ 4 ]
          ) {
            for (
              ctx->Map.pci[ 5 ] = ScoreMtxReqSeizeWait_Pre_Priority_High;
              ctx->Map.pci[ 5 ] < ScoreMtxReqSeizeWait_Pre_Priority_NA;
              ++ctx->Map.pci[ 5 ]
            ) {
              ctx->Map.entry = ScoreMtxReqSeizeWait_PopEntry( ctx );

              if ( ctx->Map.entry.Skip ) {
                continue;
              }

              ScoreMtxReqSeizeWait_SetPreConditionStates( ctx );
              ScoreMtxReqSeizeWait_Prepare( ctx );
              ScoreMtxReqSeizeWait_TestVariant( ctx );
            }
          }
        }
      }
    }
  }

  T_add_remark( &ScoreMtxReqSeizeWait_Remark );
  T_pop_fixture();
}

/** @} */
