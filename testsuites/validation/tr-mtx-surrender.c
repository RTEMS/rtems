/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreMtxReqSurrender
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

#include "tr-mtx-surrender.h"
#include "tr-tq-surrender-priority-inherit.h"
#include "tr-tq-surrender.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreMtxReqSurrender spec:/score/mtx/req/surrender
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef struct {
  uint32_t Skip : 1;
  uint32_t Pre_Protocol_NA : 1;
  uint32_t Pre_Discipline_NA : 1;
  uint32_t Pre_Recursive_NA : 1;
  uint32_t Pre_OwnerCheck_NA : 1;
  uint32_t Pre_Owner_NA : 1;
  uint32_t Pre_Nested_NA : 1;
  uint32_t Pre_Blocked_NA : 1;
  uint32_t Pre_Priority_NA : 1;
  uint32_t Post_Status : 2;
  uint32_t Post_Owner : 3;
  uint32_t Post_Surrender : 3;
  uint32_t Post_Priority : 2;
} ScoreMtxReqSurrender_Entry;

/**
 * @brief Test context for spec:/score/mtx/req/surrender test case.
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
   * @brief If this member is true, then the calling thread shall have seized
   *   the mutex recursively.
   */
  bool nested;

  /**
   * @brief If this member is true, then there shall be a thread blocked
   *   waiting for the mutex.
   */
  bool blocked;

  /**
   * @brief This member contains the real priority of the calling thread.
   */
  rtems_task_priority priority_real;

  /**
   * @brief This member contains the current priority of the calling thread
   *   before the directive call.
   */
  rtems_task_priority priority_before;

  /**
   * @brief This member contains the return status of the directive call.
   */
  Status_Control status;

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
   * @brief This member contains the counter snapshot after the directive call.
   */
  uint32_t counter;

  /**
   * @brief This member contains a copy of the corresponding
   *   ScoreMtxReqSurrender_Run() parameter.
   */
  TQMtxContext *tq_ctx;

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 8 ];

    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 8 ];

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
    ScoreMtxReqSurrender_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} ScoreMtxReqSurrender_Context;

static ScoreMtxReqSurrender_Context
  ScoreMtxReqSurrender_Instance;

static const char * const ScoreMtxReqSurrender_PreDesc_Protocol[] = {
  "None",
  "Inherit",
  "Ceiling",
  "MrsP",
  "NA"
};

static const char * const ScoreMtxReqSurrender_PreDesc_Discipline[] = {
  "FIFO",
  "Priority",
  "NA"
};

static const char * const ScoreMtxReqSurrender_PreDesc_Recursive[] = {
  "Allowed",
  "NotAllowed",
  "NA"
};

static const char * const ScoreMtxReqSurrender_PreDesc_OwnerCheck[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const ScoreMtxReqSurrender_PreDesc_Owner[] = {
  "None",
  "Caller",
  "Other",
  "NA"
};

static const char * const ScoreMtxReqSurrender_PreDesc_Nested[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const ScoreMtxReqSurrender_PreDesc_Blocked[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const ScoreMtxReqSurrender_PreDesc_Priority[] = {
  "High",
  "Equal",
  "Low",
  "NA"
};

static const char * const * const ScoreMtxReqSurrender_PreDesc[] = {
  ScoreMtxReqSurrender_PreDesc_Protocol,
  ScoreMtxReqSurrender_PreDesc_Discipline,
  ScoreMtxReqSurrender_PreDesc_Recursive,
  ScoreMtxReqSurrender_PreDesc_OwnerCheck,
  ScoreMtxReqSurrender_PreDesc_Owner,
  ScoreMtxReqSurrender_PreDesc_Nested,
  ScoreMtxReqSurrender_PreDesc_Blocked,
  ScoreMtxReqSurrender_PreDesc_Priority,
  NULL
};

#if defined(RTEMS_SMP)
#include "tr-tq-surrender-mrsp.h"
#endif

typedef ScoreMtxReqSurrender_Context Context;

static Status_Control Status( const Context *ctx, Status_Control status )
{
  return TQConvertStatus( &ctx->tq_ctx->base, status );
}

static void Action( Context *ctx )
{
  Status_Control status;

  TQSetScheduler(
    &ctx->tq_ctx->base,
    TQ_HELPER_A,
    SCHEDULER_A_ID,
    PRIO_VERY_HIGH
  );
  TQSetScheduler(
    &ctx->tq_ctx->base,
    TQ_BLOCKER_A,
    SCHEDULER_A_ID,
    PRIO_VERY_HIGH
  );

  if ( ctx->owner_caller ) {
    status = TQEnqueue( &ctx->tq_ctx->base, TQ_NO_WAIT );
    T_eq_int( status, Status( ctx, STATUS_SUCCESSFUL ) );
  } else if ( ctx->owner_other ) {
    TQSend( &ctx->tq_ctx->base, TQ_HELPER_A, TQ_EVENT_ENQUEUE );
  }

  if ( ctx->nested ) {
    status = TQEnqueue( &ctx->tq_ctx->base, TQ_NO_WAIT );
    T_eq_int( status, Status( ctx, STATUS_SUCCESSFUL ) );
  }

  if ( ctx->blocked ) {
    TQSend( &ctx->tq_ctx->base, TQ_BLOCKER_A, TQ_EVENT_ENQUEUE );
    Yield();
  }

  TQResetCounter( &ctx->tq_ctx->base );
  SetSelfPriority( ctx->priority_real );
  ctx->priority_before = GetSelfPriority();
  TQSchedulerRecordStart( &ctx->tq_ctx->base );
  ctx->status = TQSurrender( &ctx->tq_ctx->base );
  TQSchedulerRecordStop( &ctx->tq_ctx->base );
  ctx->owner_after = TQGetOwner( &ctx->tq_ctx->base );
  ctx->priority_after = GetSelfPriority();
  SetSelfPriority( PRIO_NORMAL );
  Yield();
  ctx->counter = TQGetCounter( &ctx->tq_ctx->base );

  if ( ctx->nested ) {
    status = TQSurrender( &ctx->tq_ctx->base );
    T_eq_int( status, Status( ctx, STATUS_SUCCESSFUL ) );
  }

  if ( ctx->owner_other ) {
    TQSend( &ctx->tq_ctx->base, TQ_HELPER_A, TQ_EVENT_SURRENDER );
  }

  if ( ctx->blocked ) {
    TQSend( &ctx->tq_ctx->base, TQ_BLOCKER_A, TQ_EVENT_SURRENDER );
  }
}

static void ActionSticky( Context *ctx )
{
  Status_Control status;

  TQSetScheduler(
    &ctx->tq_ctx->base,
    TQ_HELPER_A,
    SCHEDULER_A_ID,
    PRIO_VERY_HIGH
  );
  TQSetScheduler(
    &ctx->tq_ctx->base,
    TQ_BLOCKER_A,
    SCHEDULER_B_ID,
    PRIO_VERY_HIGH
  );

  if ( ctx->owner_caller ) {
    status = TQEnqueue( &ctx->tq_ctx->base, TQ_NO_WAIT );
    T_eq_int( status, Status( ctx, STATUS_SUCCESSFUL ) );
  } else if ( ctx->owner_other ) {
    SetSelfScheduler( SCHEDULER_B_ID, PRIO_ULTRA_HIGH );
    TQSendAndSynchronizeRunner(
      &ctx->tq_ctx->base,
      TQ_HELPER_A,
      TQ_EVENT_ENQUEUE
    );
    SetSelfScheduler( SCHEDULER_A_ID, PRIO_ULTRA_HIGH );
  }

  if ( ctx->nested ) {
    status = TQEnqueue( &ctx->tq_ctx->base, TQ_NO_WAIT );
    T_eq_int( status, Status( ctx, STATUS_SUCCESSFUL ) );
  }

  if ( ctx->blocked ) {
    TQSendAndWaitForIntendToBlock(
      &ctx->tq_ctx->base,
      TQ_BLOCKER_A,
      TQ_EVENT_ENQUEUE
    );
  }

  TQResetCounter( &ctx->tq_ctx->base );
  SetSelfPriority( ctx->priority_real );
  ctx->priority_before = GetSelfPriority();
  TQSchedulerRecordStart( &ctx->tq_ctx->base );
  ctx->status = TQSurrender( &ctx->tq_ctx->base );
  TQSchedulerRecordStop( &ctx->tq_ctx->base );
  ctx->owner_after = TQGetOwner( &ctx->tq_ctx->base );
  ctx->priority_after = GetSelfPriority();

  if ( ctx->status == Status( ctx, STATUS_SUCCESSFUL ) ) {
    TQWaitForExecutionStop( &ctx->tq_ctx->base, TQ_BLOCKER_A );
  }

  ctx->counter = TQGetCounter( &ctx->tq_ctx->base );

  if ( ctx->nested ) {
    status = TQSurrender( &ctx->tq_ctx->base );
    T_eq_int( status, Status( ctx, STATUS_SUCCESSFUL ) );
  }

  if ( ctx->owner_other ) {
    SetSelfScheduler( SCHEDULER_B_ID, PRIO_ULTRA_HIGH );
    TQSendAndSynchronizeRunner(
      &ctx->tq_ctx->base,
      TQ_HELPER_A,
      TQ_EVENT_SURRENDER
    );
    SetSelfScheduler( SCHEDULER_A_ID, PRIO_NORMAL );
  } else {
    SetSelfPriority( PRIO_NORMAL );
  }

  if ( ctx->blocked ) {
    TQSendAndSynchronizeRunner(
      &ctx->tq_ctx->base,
      TQ_BLOCKER_A,
      TQ_EVENT_SURRENDER
    );
  }
}

static void ScoreMtxReqSurrender_Pre_Protocol_Prepare(
  ScoreMtxReqSurrender_Context     *ctx,
  ScoreMtxReqSurrender_Pre_Protocol state
)
{
  switch ( state ) {
    case ScoreMtxReqSurrender_Pre_Protocol_None: {
      /*
       * Where the mutex does not use a locking protocol.
       */
      if ( ctx->tq_ctx->protocol != TQ_MTX_NO_PROTOCOL ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreMtxReqSurrender_Pre_Protocol_Inherit: {
      /*
       * Where the mutex uses the priority inheritance locking protocol.
       */
      if ( ctx->tq_ctx->protocol != TQ_MTX_PRIORITY_INHERIT ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreMtxReqSurrender_Pre_Protocol_Ceiling: {
      /*
       * Where the mutex uses the priority ceiling locking protocol.
       */
      if ( ctx->tq_ctx->protocol != TQ_MTX_PRIORITY_CEILING ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreMtxReqSurrender_Pre_Protocol_MrsP: {
      /*
       * Where the mutex uses the MrsP locking protocol.
       */
      if ( ctx->tq_ctx->protocol != TQ_MTX_MRSP ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreMtxReqSurrender_Pre_Protocol_NA:
      break;
  }
}

static void ScoreMtxReqSurrender_Pre_Discipline_Prepare(
  ScoreMtxReqSurrender_Context       *ctx,
  ScoreMtxReqSurrender_Pre_Discipline state
)
{
  switch ( state ) {
    case ScoreMtxReqSurrender_Pre_Discipline_FIFO: {
      /*
       * Where the thread queue of the mutex uses the FIFO discipline.
       */
      if ( ctx->tq_ctx->base.discipline != TQ_FIFO ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreMtxReqSurrender_Pre_Discipline_Priority: {
      /*
       * Where the thread queue of the mutex uses the priority discipline.
       */
      if ( ctx->tq_ctx->base.discipline != TQ_PRIORITY ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreMtxReqSurrender_Pre_Discipline_NA:
      break;
  }
}

static void ScoreMtxReqSurrender_Pre_Recursive_Prepare(
  ScoreMtxReqSurrender_Context      *ctx,
  ScoreMtxReqSurrender_Pre_Recursive state
)
{
  switch ( state ) {
    case ScoreMtxReqSurrender_Pre_Recursive_Allowed: {
      /*
       * Where a recursive seize of the mutex is allowed.
       */
      if ( ctx->tq_ctx->recursive != TQ_MTX_RECURSIVE_ALLOWED ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreMtxReqSurrender_Pre_Recursive_NotAllowed: {
      /*
       * Where a recursive seize of the mutex is not allowed.
       */
      if ( ctx->tq_ctx->recursive == TQ_MTX_RECURSIVE_ALLOWED ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreMtxReqSurrender_Pre_Recursive_NA:
      break;
  }
}

static void ScoreMtxReqSurrender_Pre_OwnerCheck_Prepare(
  ScoreMtxReqSurrender_Context       *ctx,
  ScoreMtxReqSurrender_Pre_OwnerCheck state
)
{
  switch ( state ) {
    case ScoreMtxReqSurrender_Pre_OwnerCheck_Yes: {
      /*
       * Where the surrender checks that the mutex owner is the calling thread.
       */
      if ( ctx->tq_ctx->owner_check != TQ_MTX_CHECKS_OWNER ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreMtxReqSurrender_Pre_OwnerCheck_No: {
      /*
       * Where the surrender does not check that the mutex owner is the calling
       * thread.
       */
      if ( ctx->tq_ctx->owner_check != TQ_MTX_NO_OWNER_CHECK ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreMtxReqSurrender_Pre_OwnerCheck_NA:
      break;
  }
}

static void ScoreMtxReqSurrender_Pre_Owner_Prepare(
  ScoreMtxReqSurrender_Context  *ctx,
  ScoreMtxReqSurrender_Pre_Owner state
)
{
  switch ( state ) {
    case ScoreMtxReqSurrender_Pre_Owner_None: {
      /*
       * While the mutex has no owner.
       */
      ctx->owner_caller = false;
      ctx->owner_other = false;
      break;
    }

    case ScoreMtxReqSurrender_Pre_Owner_Caller: {
      /*
       * While the owner of the mutex is the calling thread.
       */
      ctx->owner_caller = true;
      ctx->owner_other = false;
      break;
    }

    case ScoreMtxReqSurrender_Pre_Owner_Other: {
      /*
       * While the owner of the mutex is a thread other than the calling
       * thread.
       */
      ctx->owner_caller = false;
      ctx->owner_other = true;
      break;
    }

    case ScoreMtxReqSurrender_Pre_Owner_NA:
      break;
  }
}

static void ScoreMtxReqSurrender_Pre_Nested_Prepare(
  ScoreMtxReqSurrender_Context   *ctx,
  ScoreMtxReqSurrender_Pre_Nested state
)
{
  switch ( state ) {
    case ScoreMtxReqSurrender_Pre_Nested_Yes: {
      /*
       * While calling thread seized the mutex recursively.
       */
      ctx->nested = true;
      break;
    }

    case ScoreMtxReqSurrender_Pre_Nested_No: {
      /*
       * While calling thread seized the mutex not recursively.
       */
      ctx->nested = false;
      break;
    }

    case ScoreMtxReqSurrender_Pre_Nested_NA:
      break;
  }
}

static void ScoreMtxReqSurrender_Pre_Blocked_Prepare(
  ScoreMtxReqSurrender_Context    *ctx,
  ScoreMtxReqSurrender_Pre_Blocked state
)
{
  switch ( state ) {
    case ScoreMtxReqSurrender_Pre_Blocked_Yes: {
      /*
       * While the mutex has threads blocked on the mutex.
       */
      ctx->blocked = true;
      break;
    }

    case ScoreMtxReqSurrender_Pre_Blocked_No: {
      /*
       * While no threads are blocked on the mutex.
       */
      ctx->blocked = false;
      break;
    }

    case ScoreMtxReqSurrender_Pre_Blocked_NA:
      break;
  }
}

static void ScoreMtxReqSurrender_Pre_Priority_Prepare(
  ScoreMtxReqSurrender_Context     *ctx,
  ScoreMtxReqSurrender_Pre_Priority state
)
{
  switch ( state ) {
    case ScoreMtxReqSurrender_Pre_Priority_High: {
      /*
       * While the current priority of the calling thread without the
       * priorities available through the mutex would be higher than the
       * highest priority of the priorities available through the mutex.
       */
      ctx->priority_real = PRIO_ULTRA_HIGH;
      break;
    }

    case ScoreMtxReqSurrender_Pre_Priority_Equal: {
      /*
       * While the current priority of the calling thread without the
       * priorities available through the mutex would be equal to the highest
       * priority of the priorities available through the mutex.
       */
      ctx->priority_real = PRIO_VERY_HIGH;
      break;
    }

    case ScoreMtxReqSurrender_Pre_Priority_Low: {
      /*
       * While the current priority of the calling thread without the
       * priorities available through the mutex would be lower than the highest
       * priority of the priorities available through the mutex.
       */
      ctx->priority_real = PRIO_HIGH;
      break;
    }

    case ScoreMtxReqSurrender_Pre_Priority_NA:
      break;
  }
}

static void ScoreMtxReqSurrender_Post_Status_Check(
  ScoreMtxReqSurrender_Context    *ctx,
  ScoreMtxReqSurrender_Post_Status state
)
{
  switch ( state ) {
    case ScoreMtxReqSurrender_Post_Status_Ok: {
      /*
       * The return status of the directive call shall be derived from
       * STATUS_SUCCESSFUL.
       */
      T_eq_int( ctx->status, Status( ctx, STATUS_SUCCESSFUL ) );
      break;
    }

    case ScoreMtxReqSurrender_Post_Status_NotOwner: {
      /*
       * The return status of the directive call shall be derived from
       * STATUS_NOT_OWNER.
       */
      T_eq_int( ctx->status, Status( ctx, STATUS_NOT_OWNER ) );
      break;
    }

    case ScoreMtxReqSurrender_Post_Status_NA:
      break;
  }
}

static void ScoreMtxReqSurrender_Post_Owner_Check(
  ScoreMtxReqSurrender_Context   *ctx,
  ScoreMtxReqSurrender_Post_Owner state
)
{
  switch ( state ) {
    case ScoreMtxReqSurrender_Post_Owner_None: {
      /*
       * The mutex shall have no owner.
       */
      T_null( ctx->owner_after );
      break;
    }

    case ScoreMtxReqSurrender_Post_Owner_Caller: {
      /*
       * The owner of the mutex shall be the calling thread.
       */
      T_eq_ptr(
        ctx->owner_after,
        ctx->tq_ctx->base.runner_tcb
      );
      break;
    }

    case ScoreMtxReqSurrender_Post_Owner_Other: {
      /*
       * The owner of the mutex shall not be modified.
       */
      T_eq_ptr(
        ctx->owner_after,
        ctx->tq_ctx->base.worker_tcb[ TQ_HELPER_A ]
      );
      break;
    }

    case ScoreMtxReqSurrender_Post_Owner_First: {
      /*
       * The owner of the mutex shall be dequeued thread.
       */
      T_eq_ptr(
        ctx->owner_after,
        ctx->tq_ctx->base.worker_tcb[ TQ_BLOCKER_A ]
      );
      break;
    }

    case ScoreMtxReqSurrender_Post_Owner_NA:
      break;
  }
}

static void ScoreMtxReqSurrender_Post_Surrender_Check(
  ScoreMtxReqSurrender_Context       *ctx,
  ScoreMtxReqSurrender_Post_Surrender state
)
{
  switch ( state ) {
    case ScoreMtxReqSurrender_Post_Surrender_Nop: {
      /*
       * The thread queue of the mutex shall not be surrendered to a thread.
       */
      T_eq_u32( ctx->counter, 0 );
      break;
    }

    case ScoreMtxReqSurrender_Post_Surrender_FIFO: {
      /*
       * The thread queue of the mutex shall be surrendered in FIFO order.
       */
      T_eq_u32( ctx->counter, 1 );
      ScoreTqReqSurrender_Run( &ctx->tq_ctx->base );
      break;
    }

    case ScoreMtxReqSurrender_Post_Surrender_Priority: {
      /*
       * The thread queue of the mutex shall be surrendered in priority order.
       */
      T_eq_u32( ctx->counter, 1 );
      ScoreTqReqSurrender_Run( &ctx->tq_ctx->base );
      break;
    }

    case ScoreMtxReqSurrender_Post_Surrender_PriorityInherit: {
      /*
       * The thread queue of the mutex shall be surrendered in priority order
       * with priority inheritance.
       */
      T_eq_u32( ctx->counter, 1 );
      ScoreTqReqSurrenderPriorityInherit_Run( &ctx->tq_ctx->base );
      break;
    }

    case ScoreMtxReqSurrender_Post_Surrender_MrsP: {
      /*
       * The thread queue of the mutex shall be surrendered in priority order
       * with MrsP.
       */
      #if defined(RTEMS_SMP)
      T_eq_u32( ctx->counter, 1 );
      ScoreTqReqSurrenderMrsp_Run( &ctx->tq_ctx->base );
      #else
      T_unreachable();
      #endif
      break;
    }

    case ScoreMtxReqSurrender_Post_Surrender_NA:
      break;
  }
}

static void ScoreMtxReqSurrender_Post_Priority_Check(
  ScoreMtxReqSurrender_Context      *ctx,
  ScoreMtxReqSurrender_Post_Priority state
)
{
  switch ( state ) {
    case ScoreMtxReqSurrender_Post_Priority_Nop: {
      /*
       * The current priority of the calling thread shall be not be modified.
       */
      T_eq_u32( ctx->priority_after, ctx->priority_before );
      break;
    }

    case ScoreMtxReqSurrender_Post_Priority_Low: {
      /*
       * The current priority of the calling thread shall be lowered to reflect
       * the removal of the priorities available through the mutex.
       */
      T_eq_u32( ctx->priority_after, ctx->priority_real );
      break;
    }

    case ScoreMtxReqSurrender_Post_Priority_NA:
      break;
  }
}

static void ScoreMtxReqSurrender_Prepare( ScoreMtxReqSurrender_Context *ctx )
{
  ctx->owner_caller = false;
  ctx->owner_other = false;
  ctx->nested = false;
  ctx->blocked = false;

  if ( ctx->tq_ctx->base.enqueue_variant == TQ_ENQUEUE_STICKY ) {
    ctx->priority_real = PRIO_ULTRA_HIGH;
  } else {
    ctx->priority_real = PRIO_NORMAL;
  }
}

static void ScoreMtxReqSurrender_Action( ScoreMtxReqSurrender_Context *ctx )
{
  if ( ctx->tq_ctx->base.enqueue_variant == TQ_ENQUEUE_STICKY ) {
    ActionSticky( ctx );
  } else {
    Action( ctx );
  }
}

static const ScoreMtxReqSurrender_Entry
ScoreMtxReqSurrender_Entries[] = {
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, ScoreMtxReqSurrender_Post_Status_NA,
    ScoreMtxReqSurrender_Post_Owner_NA, ScoreMtxReqSurrender_Post_Surrender_NA,
    ScoreMtxReqSurrender_Post_Priority_NA },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, ScoreMtxReqSurrender_Post_Status_NA,
    ScoreMtxReqSurrender_Post_Owner_NA, ScoreMtxReqSurrender_Post_Surrender_NA,
    ScoreMtxReqSurrender_Post_Priority_NA },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, ScoreMtxReqSurrender_Post_Status_NA,
    ScoreMtxReqSurrender_Post_Owner_NA, ScoreMtxReqSurrender_Post_Surrender_NA,
    ScoreMtxReqSurrender_Post_Priority_NA },
  { 0, 0, 0, 0, 0, 0, 1, 0, 1, ScoreMtxReqSurrender_Post_Status_NotOwner,
    ScoreMtxReqSurrender_Post_Owner_Other,
    ScoreMtxReqSurrender_Post_Surrender_Nop,
    ScoreMtxReqSurrender_Post_Priority_Nop },
  { 1, 0, 0, 0, 0, 0, 1, 0, 1, ScoreMtxReqSurrender_Post_Status_NA,
    ScoreMtxReqSurrender_Post_Owner_NA, ScoreMtxReqSurrender_Post_Surrender_NA,
    ScoreMtxReqSurrender_Post_Priority_NA },
  { 0, 0, 0, 0, 0, 0, 1, 0, 1, ScoreMtxReqSurrender_Post_Status_NotOwner,
    ScoreMtxReqSurrender_Post_Owner_None,
    ScoreMtxReqSurrender_Post_Surrender_Nop,
    ScoreMtxReqSurrender_Post_Priority_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 1, ScoreMtxReqSurrender_Post_Status_Ok,
    ScoreMtxReqSurrender_Post_Owner_None,
    ScoreMtxReqSurrender_Post_Surrender_Nop,
    ScoreMtxReqSurrender_Post_Priority_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 1, ScoreMtxReqSurrender_Post_Status_Ok,
    ScoreMtxReqSurrender_Post_Owner_Caller,
    ScoreMtxReqSurrender_Post_Surrender_Nop,
    ScoreMtxReqSurrender_Post_Priority_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, ScoreMtxReqSurrender_Post_Status_Ok,
    ScoreMtxReqSurrender_Post_Owner_Caller,
    ScoreMtxReqSurrender_Post_Surrender_Nop,
    ScoreMtxReqSurrender_Post_Priority_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, ScoreMtxReqSurrender_Post_Status_Ok,
    ScoreMtxReqSurrender_Post_Owner_None,
    ScoreMtxReqSurrender_Post_Surrender_Nop,
    ScoreMtxReqSurrender_Post_Priority_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 1, ScoreMtxReqSurrender_Post_Status_Ok,
    ScoreMtxReqSurrender_Post_Owner_First,
    ScoreMtxReqSurrender_Post_Surrender_FIFO,
    ScoreMtxReqSurrender_Post_Priority_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 1, ScoreMtxReqSurrender_Post_Status_Ok,
    ScoreMtxReqSurrender_Post_Owner_First,
    ScoreMtxReqSurrender_Post_Surrender_Priority,
    ScoreMtxReqSurrender_Post_Priority_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, ScoreMtxReqSurrender_Post_Status_Ok,
    ScoreMtxReqSurrender_Post_Owner_First,
    ScoreMtxReqSurrender_Post_Surrender_PriorityInherit,
    ScoreMtxReqSurrender_Post_Priority_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, ScoreMtxReqSurrender_Post_Status_Ok,
    ScoreMtxReqSurrender_Post_Owner_First,
    ScoreMtxReqSurrender_Post_Surrender_Priority,
    ScoreMtxReqSurrender_Post_Priority_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, ScoreMtxReqSurrender_Post_Status_Ok,
    ScoreMtxReqSurrender_Post_Owner_None,
    ScoreMtxReqSurrender_Post_Surrender_Nop,
    ScoreMtxReqSurrender_Post_Priority_Low },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, ScoreMtxReqSurrender_Post_Status_Ok,
    ScoreMtxReqSurrender_Post_Owner_First,
    ScoreMtxReqSurrender_Post_Surrender_MrsP,
    ScoreMtxReqSurrender_Post_Priority_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, ScoreMtxReqSurrender_Post_Status_Ok,
    ScoreMtxReqSurrender_Post_Owner_First,
    ScoreMtxReqSurrender_Post_Surrender_PriorityInherit,
    ScoreMtxReqSurrender_Post_Priority_Low },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, ScoreMtxReqSurrender_Post_Status_Ok,
    ScoreMtxReqSurrender_Post_Owner_First,
    ScoreMtxReqSurrender_Post_Surrender_Priority,
    ScoreMtxReqSurrender_Post_Priority_Low },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, ScoreMtxReqSurrender_Post_Status_Ok,
    ScoreMtxReqSurrender_Post_Owner_First,
    ScoreMtxReqSurrender_Post_Surrender_MrsP,
    ScoreMtxReqSurrender_Post_Priority_Low }
};

static const uint8_t
ScoreMtxReqSurrender_Map[] = {
  4, 4, 4, 5, 5, 5, 4, 4, 4, 5, 5, 5, 7, 7, 7, 7, 7, 7, 10, 10, 10, 6, 6, 6, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 7, 7, 7,
  7, 7, 7, 10, 10, 10, 6, 6, 6, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2,
  2, 2, 4, 4, 4, 5, 5, 5, 2, 2, 2, 2, 2, 2, 10, 10, 10, 6, 6, 6, 2, 2, 2, 2, 2,
  2, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2,
  10, 10, 10, 6, 6, 6, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 4, 4, 4, 5, 5, 5, 4,
  4, 4, 5, 5, 5, 7, 7, 7, 7, 7, 7, 11, 11, 11, 6, 6, 6, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 7, 7, 7, 7, 7, 7, 11, 11, 11,
  6, 6, 6, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 4, 4, 4, 5, 5,
  5, 2, 2, 2, 2, 2, 2, 11, 11, 11, 6, 6, 6, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3,
  2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 11, 11, 11, 6, 6, 6, 2,
  2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4,
  4, 4, 5, 5, 5, 4, 4, 4, 5, 5, 5, 8, 8, 8, 7, 7, 7, 12, 12, 16, 6, 6, 6, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 8, 8, 8, 7,
  7, 7, 12, 12, 16, 6, 6, 6, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2,
  2, 4, 4, 4, 5, 5, 5, 2, 2, 2, 2, 2, 2, 12, 12, 16, 6, 6, 6, 2, 2, 2, 2, 2, 2,
  3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 12,
  12, 16, 6, 6, 6, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 4, 4, 4, 5, 5, 5, 4, 4, 4, 5, 5, 5, 8, 8, 8, 8, 8, 8, 13,
  13, 17, 9, 9, 14, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 8, 8, 8, 8, 8, 8, 13, 13, 17, 9, 9, 14, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 4, 4, 4, 5, 5, 5, 2, 2, 2, 2, 2, 2, 13, 13, 17,
  9, 9, 14, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1,
  1, 1, 2, 2, 2, 2, 2, 2, 13, 13, 17, 9, 9, 14, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1,
  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 5, 5, 5, 4, 4, 4, 5,
  5, 5, 8, 8, 8, 8, 8, 8, 15, 15, 18, 9, 9, 14, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 8, 8, 8, 8, 8, 8, 15, 15, 18, 9, 9,
  14, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 4, 4, 4, 5, 5, 5,
  2, 2, 2, 2, 2, 2, 15, 15, 18, 9, 9, 14, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3,
  2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 15, 15, 18, 9, 9, 14,
  2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1
};

static size_t ScoreMtxReqSurrender_Scope( void *arg, char *buf, size_t n )
{
  ScoreMtxReqSurrender_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( ScoreMtxReqSurrender_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture ScoreMtxReqSurrender_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = ScoreMtxReqSurrender_Scope,
  .initial_context = &ScoreMtxReqSurrender_Instance
};

static const uint16_t ScoreMtxReqSurrender_Weights[] = {
  288, 144, 72, 36, 12, 6, 3, 1
};

static void ScoreMtxReqSurrender_Skip(
  ScoreMtxReqSurrender_Context *ctx,
  size_t                        index
)
{
  switch ( index + 1 ) {
    case 1:
      ctx->Map.pci[ 1 ] = ScoreMtxReqSurrender_Pre_Discipline_NA - 1;
      /* Fall through */
    case 2:
      ctx->Map.pci[ 2 ] = ScoreMtxReqSurrender_Pre_Recursive_NA - 1;
      /* Fall through */
    case 3:
      ctx->Map.pci[ 3 ] = ScoreMtxReqSurrender_Pre_OwnerCheck_NA - 1;
      /* Fall through */
    case 4:
      ctx->Map.pci[ 4 ] = ScoreMtxReqSurrender_Pre_Owner_NA - 1;
      /* Fall through */
    case 5:
      ctx->Map.pci[ 5 ] = ScoreMtxReqSurrender_Pre_Nested_NA - 1;
      /* Fall through */
    case 6:
      ctx->Map.pci[ 6 ] = ScoreMtxReqSurrender_Pre_Blocked_NA - 1;
      /* Fall through */
    case 7:
      ctx->Map.pci[ 7 ] = ScoreMtxReqSurrender_Pre_Priority_NA - 1;
      break;
  }
}

static inline ScoreMtxReqSurrender_Entry ScoreMtxReqSurrender_PopEntry(
  ScoreMtxReqSurrender_Context *ctx
)
{
  size_t index;

  if ( ctx->Map.skip ) {
    size_t i;

    ctx->Map.skip = false;
    index = 0;

    for ( i = 0; i < 8; ++i ) {
      index += ScoreMtxReqSurrender_Weights[ i ] * ctx->Map.pci[ i ];
    }
  } else {
    index = ctx->Map.index;
  }

  ctx->Map.index = index + 1;

  return ScoreMtxReqSurrender_Entries[
    ScoreMtxReqSurrender_Map[ index ]
  ];
}

static void ScoreMtxReqSurrender_SetPreConditionStates(
  ScoreMtxReqSurrender_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  ctx->Map.pcs[ 4 ] = ctx->Map.pci[ 4 ];

  if ( ctx->Map.entry.Pre_Nested_NA ) {
    ctx->Map.pcs[ 5 ] = ScoreMtxReqSurrender_Pre_Nested_NA;
  } else {
    ctx->Map.pcs[ 5 ] = ctx->Map.pci[ 5 ];
  }

  ctx->Map.pcs[ 6 ] = ctx->Map.pci[ 6 ];

  if ( ctx->Map.entry.Pre_Priority_NA ) {
    ctx->Map.pcs[ 7 ] = ScoreMtxReqSurrender_Pre_Priority_NA;
  } else {
    ctx->Map.pcs[ 7 ] = ctx->Map.pci[ 7 ];
  }
}

static void ScoreMtxReqSurrender_TestVariant(
  ScoreMtxReqSurrender_Context *ctx
)
{
  ScoreMtxReqSurrender_Pre_Protocol_Prepare( ctx, ctx->Map.pcs[ 0 ] );

  if ( ctx->Map.skip ) {
    ScoreMtxReqSurrender_Skip( ctx, 0 );
    return;
  }

  ScoreMtxReqSurrender_Pre_Discipline_Prepare( ctx, ctx->Map.pcs[ 1 ] );

  if ( ctx->Map.skip ) {
    ScoreMtxReqSurrender_Skip( ctx, 1 );
    return;
  }

  ScoreMtxReqSurrender_Pre_Recursive_Prepare( ctx, ctx->Map.pcs[ 2 ] );

  if ( ctx->Map.skip ) {
    ScoreMtxReqSurrender_Skip( ctx, 2 );
    return;
  }

  ScoreMtxReqSurrender_Pre_OwnerCheck_Prepare( ctx, ctx->Map.pcs[ 3 ] );

  if ( ctx->Map.skip ) {
    ScoreMtxReqSurrender_Skip( ctx, 3 );
    return;
  }

  ScoreMtxReqSurrender_Pre_Owner_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  ScoreMtxReqSurrender_Pre_Nested_Prepare( ctx, ctx->Map.pcs[ 5 ] );
  ScoreMtxReqSurrender_Pre_Blocked_Prepare( ctx, ctx->Map.pcs[ 6 ] );
  ScoreMtxReqSurrender_Pre_Priority_Prepare( ctx, ctx->Map.pcs[ 7 ] );
  ScoreMtxReqSurrender_Action( ctx );
  ScoreMtxReqSurrender_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  ScoreMtxReqSurrender_Post_Owner_Check( ctx, ctx->Map.entry.Post_Owner );
  ScoreMtxReqSurrender_Post_Surrender_Check(
    ctx,
    ctx->Map.entry.Post_Surrender
  );
  ScoreMtxReqSurrender_Post_Priority_Check(
    ctx,
    ctx->Map.entry.Post_Priority
  );
}

static T_fixture_node ScoreMtxReqSurrender_Node;

static T_remark ScoreMtxReqSurrender_Remark = {
  .next = NULL,
  .remark = "ScoreMtxReqSurrender"
};

void ScoreMtxReqSurrender_Run( TQMtxContext *tq_ctx )
{
  ScoreMtxReqSurrender_Context *ctx;

  ctx = &ScoreMtxReqSurrender_Instance;
  ctx->tq_ctx = tq_ctx;

  ctx = T_push_fixture(
    &ScoreMtxReqSurrender_Node,
    &ScoreMtxReqSurrender_Fixture
  );
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;
  ctx->Map.skip = false;

  for (
    ctx->Map.pci[ 0 ] = ScoreMtxReqSurrender_Pre_Protocol_None;
    ctx->Map.pci[ 0 ] < ScoreMtxReqSurrender_Pre_Protocol_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = ScoreMtxReqSurrender_Pre_Discipline_FIFO;
      ctx->Map.pci[ 1 ] < ScoreMtxReqSurrender_Pre_Discipline_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = ScoreMtxReqSurrender_Pre_Recursive_Allowed;
        ctx->Map.pci[ 2 ] < ScoreMtxReqSurrender_Pre_Recursive_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = ScoreMtxReqSurrender_Pre_OwnerCheck_Yes;
          ctx->Map.pci[ 3 ] < ScoreMtxReqSurrender_Pre_OwnerCheck_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          for (
            ctx->Map.pci[ 4 ] = ScoreMtxReqSurrender_Pre_Owner_None;
            ctx->Map.pci[ 4 ] < ScoreMtxReqSurrender_Pre_Owner_NA;
            ++ctx->Map.pci[ 4 ]
          ) {
            for (
              ctx->Map.pci[ 5 ] = ScoreMtxReqSurrender_Pre_Nested_Yes;
              ctx->Map.pci[ 5 ] < ScoreMtxReqSurrender_Pre_Nested_NA;
              ++ctx->Map.pci[ 5 ]
            ) {
              for (
                ctx->Map.pci[ 6 ] = ScoreMtxReqSurrender_Pre_Blocked_Yes;
                ctx->Map.pci[ 6 ] < ScoreMtxReqSurrender_Pre_Blocked_NA;
                ++ctx->Map.pci[ 6 ]
              ) {
                for (
                  ctx->Map.pci[ 7 ] = ScoreMtxReqSurrender_Pre_Priority_High;
                  ctx->Map.pci[ 7 ] < ScoreMtxReqSurrender_Pre_Priority_NA;
                  ++ctx->Map.pci[ 7 ]
                ) {
                  ctx->Map.entry = ScoreMtxReqSurrender_PopEntry( ctx );

                  if ( ctx->Map.entry.Skip ) {
                    continue;
                  }

                  ScoreMtxReqSurrender_SetPreConditionStates( ctx );
                  ScoreMtxReqSurrender_Prepare( ctx );
                  ScoreMtxReqSurrender_TestVariant( ctx );
                }
              }
            }
          }
        }
      }
    }
  }

  T_add_remark( &ScoreMtxReqSurrender_Remark );
  T_pop_fixture();
}

/** @} */
