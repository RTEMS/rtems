/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreTqReqSurrenderMrsp
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

#include "tr-tq-surrender-mrsp.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreTqReqSurrenderMrsp spec:/score/tq/req/surrender-mrsp
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef struct {
  uint32_t Skip : 1;
  uint32_t Pre_InheritedPriority_NA : 1;
  uint32_t Pre_PreviousHelping_NA : 1;
  uint32_t Pre_Scheduler_NA : 1;
  uint32_t Pre_NewHelping_NA : 1;
  uint32_t Pre_Suspended_NA : 1;
  uint32_t Pre_WaitState_NA : 1;
  uint32_t Post_Dequeue : 1;
  uint32_t Post_Unblock : 1;
  uint32_t Post_PreviousOwnerPriority : 2;
  uint32_t Post_RemoveHelper : 2;
  uint32_t Post_AddHelper : 2;
  uint32_t Post_Suspended : 2;
} ScoreTqReqSurrenderMrsp_Entry;

/**
 * @brief Test context for spec:/score/tq/req/surrender-mrsp test case.
 */
typedef struct {
  /**
   * @brief If this member is true, then all priorities of the previous owner
   *   inherited from the thread queue thread shall be dispensable.
   */
  bool inherited_priorities_are_dispensible;

  /**
   * @brief If this member is true, then all helping schedulers of the previous
   *   owner thread gained through the thread queue shall be dispensable.
   */
  bool helping_schedules_are_dispensible;

  /**
   * @brief If this member is true, then the previous owner thread shall use
   *   helping scheduler.
   */
  bool use_helping_scheduler;

  /**
   * @brief If this member is true, then the new owner thread shall gain a
   *   vital helping scheduler.
   */
  bool gains_new_helping_scheduler;

  /**
   * @brief If this member is true, then the new owner thread shall be
   *   suspended.
   */
  bool suspended;

  /**
   * @brief If this member is true, then the new owner thread shall be in the
   *   intend to block wait state.
   */
  bool intend_to_block;

  /**
   * @brief This member contains the current priority of the previous owner
   *   thread before the thread queue surrender operation.
   */
  rtems_task_priority priority_before;

  /**
   * @brief This member contains the current priority of the previous owner
   *   thread after the thread queue surrender operation.
   */
  rtems_task_priority priority_after;

  /**
   * @brief This member contains the identifier of the previous owner thread.
   */
  rtems_id previous_owner;

  /**
   * @brief This member contains a copy of the corresponding
   *   ScoreTqReqSurrenderMrsp_Run() parameter.
   */
  TQContext *tq_ctx;

  struct {
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
    ScoreTqReqSurrenderMrsp_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} ScoreTqReqSurrenderMrsp_Context;

static ScoreTqReqSurrenderMrsp_Context
  ScoreTqReqSurrenderMrsp_Instance;

static const char * const ScoreTqReqSurrenderMrsp_PreDesc_InheritedPriority[] = {
  "Vital",
  "Dispensable",
  "NA"
};

static const char * const ScoreTqReqSurrenderMrsp_PreDesc_PreviousHelping[] = {
  "Vital",
  "Dispensable",
  "NA"
};

static const char * const ScoreTqReqSurrenderMrsp_PreDesc_Scheduler[] = {
  "Home",
  "Helping",
  "NA"
};

static const char * const ScoreTqReqSurrenderMrsp_PreDesc_NewHelping[] = {
  "Vital",
  "Dispensable",
  "NA"
};

static const char * const ScoreTqReqSurrenderMrsp_PreDesc_Suspended[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const ScoreTqReqSurrenderMrsp_PreDesc_WaitState[] = {
  "IntendToBlock",
  "NA"
};

static const char * const * const ScoreTqReqSurrenderMrsp_PreDesc[] = {
  ScoreTqReqSurrenderMrsp_PreDesc_InheritedPriority,
  ScoreTqReqSurrenderMrsp_PreDesc_PreviousHelping,
  ScoreTqReqSurrenderMrsp_PreDesc_Scheduler,
  ScoreTqReqSurrenderMrsp_PreDesc_NewHelping,
  ScoreTqReqSurrenderMrsp_PreDesc_Suspended,
  ScoreTqReqSurrenderMrsp_PreDesc_WaitState,
  NULL
};

typedef ScoreTqReqSurrenderMrsp_Context Context;

static const rtems_tcb *GetUnblock( Context *ctx, size_t *index )
{
  return TQGetNextUnblock( ctx->tq_ctx, index )->thread;
}

static void ScoreTqReqSurrenderMrsp_Pre_InheritedPriority_Prepare(
  ScoreTqReqSurrenderMrsp_Context              *ctx,
  ScoreTqReqSurrenderMrsp_Pre_InheritedPriority state
)
{
  switch ( state ) {
    case ScoreTqReqSurrenderMrsp_Pre_InheritedPriority_Vital: {
      /*
       * While at least one priority inherited through the thread queue for the
       * previous owner is the highest priority of the previous owner.
       */
      ctx->inherited_priorities_are_dispensible = false;
      break;
    }

    case ScoreTqReqSurrenderMrsp_Pre_InheritedPriority_Dispensable: {
      /*
       * While all priorities inherited through the thread queue for the
       * previous owner are not the highest priority of the previous owner.
       */
      ctx->inherited_priorities_are_dispensible = true;
      break;
    }

    case ScoreTqReqSurrenderMrsp_Pre_InheritedPriority_NA:
      break;
  }
}

static void ScoreTqReqSurrenderMrsp_Pre_PreviousHelping_Prepare(
  ScoreTqReqSurrenderMrsp_Context            *ctx,
  ScoreTqReqSurrenderMrsp_Pre_PreviousHelping state
)
{
  switch ( state ) {
    case ScoreTqReqSurrenderMrsp_Pre_PreviousHelping_Vital: {
      /*
       * While at least one helping scheduler of the previous owner is only
       * available due to a priority inherited through the thread queue.
       */
      ctx->helping_schedules_are_dispensible = false;
      break;
    }

    case ScoreTqReqSurrenderMrsp_Pre_PreviousHelping_Dispensable: {
      /*
       * While all helping scheduler of the previous owner are not only
       * available due to a priority inherited through the thread queue.
       */
      ctx->helping_schedules_are_dispensible = true;
      break;
    }

    case ScoreTqReqSurrenderMrsp_Pre_PreviousHelping_NA:
      break;
  }
}

static void ScoreTqReqSurrenderMrsp_Pre_Scheduler_Prepare(
  ScoreTqReqSurrenderMrsp_Context      *ctx,
  ScoreTqReqSurrenderMrsp_Pre_Scheduler state
)
{
  switch ( state ) {
    case ScoreTqReqSurrenderMrsp_Pre_Scheduler_Home: {
      /*
       * While the previous owner executes in its home scheduler.
       */
      ctx->use_helping_scheduler = false;
      break;
    }

    case ScoreTqReqSurrenderMrsp_Pre_Scheduler_Helping: {
      /*
       * While the previous owner executes in a helping scheduler which is
       * available due to a priority inherited through the thread queue.
       */
      ctx->use_helping_scheduler = true;
      break;
    }

    case ScoreTqReqSurrenderMrsp_Pre_Scheduler_NA:
      break;
  }
}

static void ScoreTqReqSurrenderMrsp_Pre_NewHelping_Prepare(
  ScoreTqReqSurrenderMrsp_Context       *ctx,
  ScoreTqReqSurrenderMrsp_Pre_NewHelping state
)
{
  switch ( state ) {
    case ScoreTqReqSurrenderMrsp_Pre_NewHelping_Vital: {
      /*
       * While at least one helping scheduler of the new owner is only
       * available due to a priority inherited through the thread queue.
       */
      ctx->gains_new_helping_scheduler = true;
      break;
    }

    case ScoreTqReqSurrenderMrsp_Pre_NewHelping_Dispensable: {
      /*
       * While all helping scheduler of the new owner are not only available
       * due to a priority inherited through the thread queue.
       */
      ctx->gains_new_helping_scheduler = false;
      break;
    }

    case ScoreTqReqSurrenderMrsp_Pre_NewHelping_NA:
      break;
  }
}

static void ScoreTqReqSurrenderMrsp_Pre_Suspended_Prepare(
  ScoreTqReqSurrenderMrsp_Context      *ctx,
  ScoreTqReqSurrenderMrsp_Pre_Suspended state
)
{
  switch ( state ) {
    case ScoreTqReqSurrenderMrsp_Pre_Suspended_Yes: {
      /*
       * While the new owner is suspended.
       */
      ctx->suspended = true;
      break;
    }

    case ScoreTqReqSurrenderMrsp_Pre_Suspended_No: {
      /*
       * While the new owner is not suspended.
       */
      ctx->suspended = false;
      break;
    }

    case ScoreTqReqSurrenderMrsp_Pre_Suspended_NA:
      break;
  }
}

static void ScoreTqReqSurrenderMrsp_Pre_WaitState_Prepare(
  ScoreTqReqSurrenderMrsp_Context      *ctx,
  ScoreTqReqSurrenderMrsp_Pre_WaitState state
)
{
  switch ( state ) {
    case ScoreTqReqSurrenderMrsp_Pre_WaitState_IntendToBlock: {
      /*
       * While the new owner is in the intend to block wait state.
       */
      ctx->intend_to_block = true;
      break;
    }

    case ScoreTqReqSurrenderMrsp_Pre_WaitState_NA:
      break;
  }
}

static void ScoreTqReqSurrenderMrsp_Post_Dequeue_Check(
  ScoreTqReqSurrenderMrsp_Context     *ctx,
  ScoreTqReqSurrenderMrsp_Post_Dequeue state
)
{
  switch ( state ) {
    case ScoreTqReqSurrenderMrsp_Post_Dequeue_Priority: {
      /*
       * The first thread in priority order shall be dequeued from the thread
       * queue.
       */
      /* Validation is done by spec:/score/tq/req/enqueue-priority */
      break;
    }

    case ScoreTqReqSurrenderMrsp_Post_Dequeue_NA:
      break;
  }
}

static void ScoreTqReqSurrenderMrsp_Post_Unblock_Check(
  ScoreTqReqSurrenderMrsp_Context     *ctx,
  ScoreTqReqSurrenderMrsp_Post_Unblock state
)
{
  size_t i;

  i = 0;

  switch ( state ) {
    case ScoreTqReqSurrenderMrsp_Post_Unblock_No: {
      /*
       * The dequeued thread shall not be unblocked by the thread queue
       * surrender operation.
       */
      T_eq_ptr( GetUnblock( ctx, &i ), NULL );
      break;
    }

    case ScoreTqReqSurrenderMrsp_Post_Unblock_NA:
      break;
  }
}

static void ScoreTqReqSurrenderMrsp_Post_PreviousOwnerPriority_Check(
  ScoreTqReqSurrenderMrsp_Context                   *ctx,
  ScoreTqReqSurrenderMrsp_Post_PreviousOwnerPriority state
)
{
  switch ( state ) {
    case ScoreTqReqSurrenderMrsp_Post_PreviousOwnerPriority_Drop: {
      /*
       * Each eligible priority of the previous owner which had the highest
       * priority inherited through the thread queue shall be updated.
       */
      T_eq_u32( ctx->priority_after, PRIO_NORMAL );
      break;
    }

    case ScoreTqReqSurrenderMrsp_Post_PreviousOwnerPriority_Nop: {
      /*
       * No eligible priority of the previous owner shall be updated.
       */
      T_eq_u32( ctx->priority_after, ctx->priority_before );
      break;
    }

    case ScoreTqReqSurrenderMrsp_Post_PreviousOwnerPriority_NA:
      break;
  }
}

static void ScoreTqReqSurrenderMrsp_Post_RemoveHelper_Check(
  ScoreTqReqSurrenderMrsp_Context          *ctx,
  ScoreTqReqSurrenderMrsp_Post_RemoveHelper state
)
{
  rtems_status_code   sc;
  rtems_task_priority priority;

  switch ( state ) {
    case ScoreTqReqSurrenderMrsp_Post_RemoveHelper_Yes: {
      /*
       * Each helping scheduler of the previous owner which was only available
       * due to a priority inherited through the thread queue shall be removed
       * from the previous owner.
       */
      sc = rtems_task_get_priority(
        ctx->previous_owner,
        SCHEDULER_B_ID,
        &priority
      );
      T_rsc( sc, RTEMS_NOT_DEFINED );
      break;
    }

    case ScoreTqReqSurrenderMrsp_Post_RemoveHelper_No: {
      /*
       * No helping scheduler shall be removed from the previous owner.
       */
      sc = rtems_task_get_priority(
        ctx->previous_owner,
        SCHEDULER_B_ID,
        &priority
      );
      #if defined(RTEMS_SMP)
      T_rsc_success( sc );

      if ( ctx->tq_ctx->enqueue_variant == TQ_ENQUEUE_STICKY ) {
        T_eq_u32( priority, PRIO_LOW );
      } else {
        T_eq_u32( priority, PRIO_HIGH );
      }
      #else
      T_rsc( sc, RTEMS_INVALID_ID );
      #endif
      break;
    }

    case ScoreTqReqSurrenderMrsp_Post_RemoveHelper_NA:
      break;
  }
}

static void ScoreTqReqSurrenderMrsp_Post_AddHelper_Check(
  ScoreTqReqSurrenderMrsp_Context       *ctx,
  ScoreTqReqSurrenderMrsp_Post_AddHelper state
)
{
  rtems_status_code   sc;
  rtems_task_priority priority;

  switch ( state ) {
    case ScoreTqReqSurrenderMrsp_Post_AddHelper_Yes: {
      /*
       * Each helping scheduler of the new owner which is only available due to
       * a priority inherited through the thread queue shall be added to the
       * new owner.
       */
      sc = rtems_task_get_priority(
        ctx->tq_ctx->worker_id[ TQ_BLOCKER_A ],
        SCHEDULER_A_ID,
        &priority
      );
      T_rsc_success( sc );

      if ( ctx->tq_ctx->enqueue_variant == TQ_ENQUEUE_STICKY ) {
        T_eq_u32( priority, PRIO_VERY_HIGH );
      } else {
        T_eq_u32( priority, PRIO_LOW );
      }
      break;
    }

    case ScoreTqReqSurrenderMrsp_Post_AddHelper_No: {
      /*
       * No helping scheduler shall added to the new owner.
       */
      sc = rtems_task_get_priority(
        ctx->tq_ctx->worker_id[ TQ_BLOCKER_A ],
        SCHEDULER_A_ID,
        &priority
      );
      #if defined(RTEMS_SMP)
      T_rsc( sc, RTEMS_NOT_DEFINED );
      #else
      T_rsc_success( sc );
      T_eq_u32( priority, PRIO_HIGH );
      #endif
      break;
    }

    case ScoreTqReqSurrenderMrsp_Post_AddHelper_NA:
      break;
  }
}

static void ScoreTqReqSurrenderMrsp_Post_Suspended_Check(
  ScoreTqReqSurrenderMrsp_Context       *ctx,
  ScoreTqReqSurrenderMrsp_Post_Suspended state
)
{
  switch ( state ) {
    case ScoreTqReqSurrenderMrsp_Post_Suspended_Yes: {
      /*
       * The new owner shall be suspended.
       */
      T_true( IsTaskSuspended( ctx->tq_ctx->worker_id[ TQ_BLOCKER_A ] ) );
      break;
    }

    case ScoreTqReqSurrenderMrsp_Post_Suspended_No: {
      /*
       * The new owner shall be not suspended.
       */
      T_false( IsTaskSuspended( ctx->tq_ctx->worker_id[ TQ_BLOCKER_A ] ) );
      break;
    }

    case ScoreTqReqSurrenderMrsp_Post_Suspended_NA:
      break;
  }
}

static void ScoreTqReqSurrenderMrsp_Setup(
  ScoreTqReqSurrenderMrsp_Context *ctx
)
{
  TQReset( ctx->tq_ctx );
  TQSetScheduler( ctx->tq_ctx, TQ_BLOCKER_A, SCHEDULER_B_ID, PRIO_NORMAL );
  TQSetPriority( ctx->tq_ctx, TQ_BLOCKER_B, PRIO_VERY_HIGH );
  TQSetScheduler( ctx->tq_ctx, TQ_BLOCKER_C, SCHEDULER_B_ID, PRIO_LOW );
  TQSetPriority( ctx->tq_ctx, TQ_BLOCKER_D, PRIO_VERY_HIGH );
  TQSetPriority( ctx->tq_ctx, TQ_HELPER_A, PRIO_NORMAL );
}

static void ScoreTqReqSurrenderMrsp_Setup_Wrap( void *arg )
{
  ScoreTqReqSurrenderMrsp_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  ScoreTqReqSurrenderMrsp_Setup( ctx );
}

static void ScoreTqReqSurrenderMrsp_Teardown(
  ScoreTqReqSurrenderMrsp_Context *ctx
)
{
  SetSelfScheduler( SCHEDULER_A_ID, PRIO_NORMAL );
}

static void ScoreTqReqSurrenderMrsp_Teardown_Wrap( void *arg )
{
  ScoreTqReqSurrenderMrsp_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  ScoreTqReqSurrenderMrsp_Teardown( ctx );
}

static void ScoreTqReqSurrenderMrsp_Prepare(
  ScoreTqReqSurrenderMrsp_Context *ctx
)
{
  ctx->inherited_priorities_are_dispensible = true;
  ctx->helping_schedules_are_dispensible = true;
  ctx->use_helping_scheduler = false;
  ctx->gains_new_helping_scheduler = false;
  ctx->intend_to_block = false;
}

static void ScoreTqReqSurrenderMrsp_Action(
  ScoreTqReqSurrenderMrsp_Context *ctx
)
{
  ctx->previous_owner = ctx->tq_ctx->worker_id[ TQ_HELPER_A ];

  SetSelfPriority( PRIO_LOW );

  if (
    ctx->inherited_priorities_are_dispensible ||
    ctx->helping_schedules_are_dispensible
  ) {
    TQSend( ctx->tq_ctx, TQ_HELPER_A, TQ_EVENT_MUTEX_A_OBTAIN );

    if ( ctx->inherited_priorities_are_dispensible ) {
      TQSend( ctx->tq_ctx, TQ_BLOCKER_B, TQ_EVENT_MUTEX_A_OBTAIN );
    }

    if ( ctx->helping_schedules_are_dispensible ) {
      TQSendAndWaitForExecutionStop(
        ctx->tq_ctx,
        TQ_BLOCKER_C,
        TQ_EVENT_MUTEX_A_OBTAIN
      );
    }
  }

  /*
   * Take only the priorities into account which are inherited from the
   * priority inheritance mutex.  This avoids having to deal with the ceiling
   * priority.
   */
  ctx->priority_before = TQGetPriority( ctx->tq_ctx, TQ_HELPER_A );

  SetSelfScheduler( SCHEDULER_B_ID, PRIO_ULTRA_HIGH );
  ctx->tq_ctx->busy_wait[ TQ_HELPER_A ] = true;
  TQSendAndSynchronizeRunner(
    ctx->tq_ctx,
    TQ_HELPER_A,
    TQ_EVENT_ENQUEUE | TQ_EVENT_BUSY_WAIT
  );
  SetSelfScheduler( SCHEDULER_A_ID, PRIO_ULTRA_HIGH );

  TQSendAndWaitForIntendToBlock(
    ctx->tq_ctx,
    TQ_BLOCKER_A,
    TQ_EVENT_ENQUEUE
  );

  SetSelfScheduler( SCHEDULER_B_ID, PRIO_ULTRA_HIGH );

  if ( ctx->gains_new_helping_scheduler ) {
    TQSend(
      ctx->tq_ctx,
      TQ_BLOCKER_D,
      TQ_EVENT_ENQUEUE
    );
    YieldTask( ctx->tq_ctx->worker_id[ TQ_HELPER_A ] );
    TQWaitForEventsReceived( ctx->tq_ctx, TQ_BLOCKER_D );
    TQWaitForIntendToBlock( ctx->tq_ctx, TQ_BLOCKER_D );
    YieldTask( ctx->tq_ctx->worker_id[ TQ_BLOCKER_D ] );
  }

  if ( ctx->use_helping_scheduler ) {
    SetSelfScheduler( SCHEDULER_A_ID, PRIO_ULTRA_HIGH );
    WaitForHeir( 1, ctx->tq_ctx->worker_id[ TQ_HELPER_A ] );
  }

  if ( ctx->suspended ) {
    SuspendTask( ctx->tq_ctx->worker_id[ TQ_BLOCKER_A ] );
  }

  ctx->tq_ctx->busy_wait[ TQ_HELPER_A ] = false;
  TQSendAndWaitForExecutionStop(
    ctx->tq_ctx,
    TQ_HELPER_A,
    TQ_EVENT_SCHEDULER_RECORD_START |
      TQ_EVENT_SURRENDER
  );
  TQSchedulerRecordStop( ctx->tq_ctx );
  T_eq_ptr(
    TQGetOwner( ctx->tq_ctx ),
    ctx->tq_ctx->worker_tcb[ TQ_BLOCKER_A ]
  );
  ctx->priority_after = TQGetPriority( ctx->tq_ctx, TQ_HELPER_A );
}

static void ScoreTqReqSurrenderMrsp_Cleanup(
  ScoreTqReqSurrenderMrsp_Context *ctx
)
{
  SetSelfScheduler( SCHEDULER_A_ID, PRIO_ULTRA_HIGH );

  if ( ctx->suspended ) {
    ResumeTask( ctx->tq_ctx->worker_id[ TQ_BLOCKER_A ] );
  }

  TQSendAndSynchronizeRunner(
    ctx->tq_ctx,
    TQ_BLOCKER_A,
    TQ_EVENT_SURRENDER
  );

  if ( ctx->gains_new_helping_scheduler ) {
    TQSendAndSynchronizeRunner(
      ctx->tq_ctx,
      TQ_BLOCKER_D,
      TQ_EVENT_SURRENDER
    );
  }

  if (
    ctx->inherited_priorities_are_dispensible ||
    ctx->helping_schedules_are_dispensible
  ) {
    TQSendAndSynchronizeRunner(
      ctx->tq_ctx,
      TQ_HELPER_A,
      TQ_EVENT_MUTEX_A_RELEASE
    );

    if ( ctx->inherited_priorities_are_dispensible ) {
      TQSendAndSynchronizeRunner(
        ctx->tq_ctx,
        TQ_BLOCKER_B,
        TQ_EVENT_MUTEX_A_RELEASE
      );
    }

    if ( ctx->helping_schedules_are_dispensible ) {
      TQSendAndSynchronizeRunner(
        ctx->tq_ctx,
        TQ_BLOCKER_C,
        TQ_EVENT_MUTEX_A_RELEASE
      );
    }
  }

  T_eq_u32( rtems_scheduler_get_processor(), 0 );
}

static const ScoreTqReqSurrenderMrsp_Entry
ScoreTqReqSurrenderMrsp_Entries[] = {
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqSurrenderMrsp_Post_Dequeue_Priority,
    ScoreTqReqSurrenderMrsp_Post_Unblock_No,
    ScoreTqReqSurrenderMrsp_Post_PreviousOwnerPriority_Drop,
    ScoreTqReqSurrenderMrsp_Post_RemoveHelper_Yes,
    ScoreTqReqSurrenderMrsp_Post_AddHelper_Yes,
    ScoreTqReqSurrenderMrsp_Post_Suspended_Yes },
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqSurrenderMrsp_Post_Dequeue_Priority,
    ScoreTqReqSurrenderMrsp_Post_Unblock_No,
    ScoreTqReqSurrenderMrsp_Post_PreviousOwnerPriority_Drop,
    ScoreTqReqSurrenderMrsp_Post_RemoveHelper_Yes,
    ScoreTqReqSurrenderMrsp_Post_AddHelper_Yes,
    ScoreTqReqSurrenderMrsp_Post_Suspended_No },
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqSurrenderMrsp_Post_Dequeue_Priority,
    ScoreTqReqSurrenderMrsp_Post_Unblock_No,
    ScoreTqReqSurrenderMrsp_Post_PreviousOwnerPriority_Drop,
    ScoreTqReqSurrenderMrsp_Post_RemoveHelper_Yes,
    ScoreTqReqSurrenderMrsp_Post_AddHelper_No,
    ScoreTqReqSurrenderMrsp_Post_Suspended_Yes },
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqSurrenderMrsp_Post_Dequeue_Priority,
    ScoreTqReqSurrenderMrsp_Post_Unblock_No,
    ScoreTqReqSurrenderMrsp_Post_PreviousOwnerPriority_Drop,
    ScoreTqReqSurrenderMrsp_Post_RemoveHelper_Yes,
    ScoreTqReqSurrenderMrsp_Post_AddHelper_No,
    ScoreTqReqSurrenderMrsp_Post_Suspended_No },
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqSurrenderMrsp_Post_Dequeue_Priority,
    ScoreTqReqSurrenderMrsp_Post_Unblock_No,
    ScoreTqReqSurrenderMrsp_Post_PreviousOwnerPriority_Drop,
    ScoreTqReqSurrenderMrsp_Post_RemoveHelper_No,
    ScoreTqReqSurrenderMrsp_Post_AddHelper_Yes,
    ScoreTqReqSurrenderMrsp_Post_Suspended_Yes },
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqSurrenderMrsp_Post_Dequeue_Priority,
    ScoreTqReqSurrenderMrsp_Post_Unblock_No,
    ScoreTqReqSurrenderMrsp_Post_PreviousOwnerPriority_Drop,
    ScoreTqReqSurrenderMrsp_Post_RemoveHelper_No,
    ScoreTqReqSurrenderMrsp_Post_AddHelper_Yes,
    ScoreTqReqSurrenderMrsp_Post_Suspended_No },
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqSurrenderMrsp_Post_Dequeue_Priority,
    ScoreTqReqSurrenderMrsp_Post_Unblock_No,
    ScoreTqReqSurrenderMrsp_Post_PreviousOwnerPriority_Drop,
    ScoreTqReqSurrenderMrsp_Post_RemoveHelper_No,
    ScoreTqReqSurrenderMrsp_Post_AddHelper_No,
    ScoreTqReqSurrenderMrsp_Post_Suspended_Yes },
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqSurrenderMrsp_Post_Dequeue_Priority,
    ScoreTqReqSurrenderMrsp_Post_Unblock_No,
    ScoreTqReqSurrenderMrsp_Post_PreviousOwnerPriority_Drop,
    ScoreTqReqSurrenderMrsp_Post_RemoveHelper_No,
    ScoreTqReqSurrenderMrsp_Post_AddHelper_No,
    ScoreTqReqSurrenderMrsp_Post_Suspended_No },
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqSurrenderMrsp_Post_Dequeue_Priority,
    ScoreTqReqSurrenderMrsp_Post_Unblock_No,
    ScoreTqReqSurrenderMrsp_Post_PreviousOwnerPriority_Nop,
    ScoreTqReqSurrenderMrsp_Post_RemoveHelper_Yes,
    ScoreTqReqSurrenderMrsp_Post_AddHelper_Yes,
    ScoreTqReqSurrenderMrsp_Post_Suspended_Yes },
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqSurrenderMrsp_Post_Dequeue_Priority,
    ScoreTqReqSurrenderMrsp_Post_Unblock_No,
    ScoreTqReqSurrenderMrsp_Post_PreviousOwnerPriority_Nop,
    ScoreTqReqSurrenderMrsp_Post_RemoveHelper_Yes,
    ScoreTqReqSurrenderMrsp_Post_AddHelper_Yes,
    ScoreTqReqSurrenderMrsp_Post_Suspended_No },
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqSurrenderMrsp_Post_Dequeue_Priority,
    ScoreTqReqSurrenderMrsp_Post_Unblock_No,
    ScoreTqReqSurrenderMrsp_Post_PreviousOwnerPriority_Nop,
    ScoreTqReqSurrenderMrsp_Post_RemoveHelper_Yes,
    ScoreTqReqSurrenderMrsp_Post_AddHelper_No,
    ScoreTqReqSurrenderMrsp_Post_Suspended_Yes },
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqSurrenderMrsp_Post_Dequeue_Priority,
    ScoreTqReqSurrenderMrsp_Post_Unblock_No,
    ScoreTqReqSurrenderMrsp_Post_PreviousOwnerPriority_Nop,
    ScoreTqReqSurrenderMrsp_Post_RemoveHelper_Yes,
    ScoreTqReqSurrenderMrsp_Post_AddHelper_No,
    ScoreTqReqSurrenderMrsp_Post_Suspended_No },
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqSurrenderMrsp_Post_Dequeue_Priority,
    ScoreTqReqSurrenderMrsp_Post_Unblock_No,
    ScoreTqReqSurrenderMrsp_Post_PreviousOwnerPriority_Nop,
    ScoreTqReqSurrenderMrsp_Post_RemoveHelper_No,
    ScoreTqReqSurrenderMrsp_Post_AddHelper_Yes,
    ScoreTqReqSurrenderMrsp_Post_Suspended_Yes },
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqSurrenderMrsp_Post_Dequeue_Priority,
    ScoreTqReqSurrenderMrsp_Post_Unblock_No,
    ScoreTqReqSurrenderMrsp_Post_PreviousOwnerPriority_Nop,
    ScoreTqReqSurrenderMrsp_Post_RemoveHelper_No,
    ScoreTqReqSurrenderMrsp_Post_AddHelper_Yes,
    ScoreTqReqSurrenderMrsp_Post_Suspended_No },
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqSurrenderMrsp_Post_Dequeue_Priority,
    ScoreTqReqSurrenderMrsp_Post_Unblock_No,
    ScoreTqReqSurrenderMrsp_Post_PreviousOwnerPriority_Nop,
    ScoreTqReqSurrenderMrsp_Post_RemoveHelper_No,
    ScoreTqReqSurrenderMrsp_Post_AddHelper_No,
    ScoreTqReqSurrenderMrsp_Post_Suspended_Yes },
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqSurrenderMrsp_Post_Dequeue_Priority,
    ScoreTqReqSurrenderMrsp_Post_Unblock_No,
    ScoreTqReqSurrenderMrsp_Post_PreviousOwnerPriority_Nop,
    ScoreTqReqSurrenderMrsp_Post_RemoveHelper_No,
    ScoreTqReqSurrenderMrsp_Post_AddHelper_No,
    ScoreTqReqSurrenderMrsp_Post_Suspended_No }
};

static const uint8_t
ScoreTqReqSurrenderMrsp_Map[] = {
  0, 1, 2, 3, 0, 1, 2, 3, 4, 5, 6, 7, 4, 5, 6, 7, 8, 9, 10, 11, 8, 9, 10, 11,
  12, 13, 14, 15, 12, 13, 14, 15
};

static size_t ScoreTqReqSurrenderMrsp_Scope( void *arg, char *buf, size_t n )
{
  ScoreTqReqSurrenderMrsp_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      ScoreTqReqSurrenderMrsp_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture ScoreTqReqSurrenderMrsp_Fixture = {
  .setup = ScoreTqReqSurrenderMrsp_Setup_Wrap,
  .stop = NULL,
  .teardown = ScoreTqReqSurrenderMrsp_Teardown_Wrap,
  .scope = ScoreTqReqSurrenderMrsp_Scope,
  .initial_context = &ScoreTqReqSurrenderMrsp_Instance
};

static inline ScoreTqReqSurrenderMrsp_Entry ScoreTqReqSurrenderMrsp_PopEntry(
  ScoreTqReqSurrenderMrsp_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return ScoreTqReqSurrenderMrsp_Entries[
    ScoreTqReqSurrenderMrsp_Map[ index ]
  ];
}

static void ScoreTqReqSurrenderMrsp_TestVariant(
  ScoreTqReqSurrenderMrsp_Context *ctx
)
{
  ScoreTqReqSurrenderMrsp_Pre_InheritedPriority_Prepare(
    ctx,
    ctx->Map.pcs[ 0 ]
  );
  ScoreTqReqSurrenderMrsp_Pre_PreviousHelping_Prepare(
    ctx,
    ctx->Map.pcs[ 1 ]
  );
  ScoreTqReqSurrenderMrsp_Pre_Scheduler_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  ScoreTqReqSurrenderMrsp_Pre_NewHelping_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  ScoreTqReqSurrenderMrsp_Pre_Suspended_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  ScoreTqReqSurrenderMrsp_Pre_WaitState_Prepare( ctx, ctx->Map.pcs[ 5 ] );
  ScoreTqReqSurrenderMrsp_Action( ctx );
  ScoreTqReqSurrenderMrsp_Post_Dequeue_Check(
    ctx,
    ctx->Map.entry.Post_Dequeue
  );
  ScoreTqReqSurrenderMrsp_Post_Unblock_Check(
    ctx,
    ctx->Map.entry.Post_Unblock
  );
  ScoreTqReqSurrenderMrsp_Post_PreviousOwnerPriority_Check(
    ctx,
    ctx->Map.entry.Post_PreviousOwnerPriority
  );
  ScoreTqReqSurrenderMrsp_Post_RemoveHelper_Check(
    ctx,
    ctx->Map.entry.Post_RemoveHelper
  );
  ScoreTqReqSurrenderMrsp_Post_AddHelper_Check(
    ctx,
    ctx->Map.entry.Post_AddHelper
  );
  ScoreTqReqSurrenderMrsp_Post_Suspended_Check(
    ctx,
    ctx->Map.entry.Post_Suspended
  );
}

static T_fixture_node ScoreTqReqSurrenderMrsp_Node;

static T_remark ScoreTqReqSurrenderMrsp_Remark = {
  .next = NULL,
  .remark = "ScoreTqReqSurrenderMrsp"
};

void ScoreTqReqSurrenderMrsp_Run( TQContext *tq_ctx )
{
  ScoreTqReqSurrenderMrsp_Context *ctx;

  ctx = &ScoreTqReqSurrenderMrsp_Instance;
  ctx->tq_ctx = tq_ctx;

  ctx = T_push_fixture(
    &ScoreTqReqSurrenderMrsp_Node,
    &ScoreTqReqSurrenderMrsp_Fixture
  );
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = ScoreTqReqSurrenderMrsp_Pre_InheritedPriority_Vital;
    ctx->Map.pcs[ 0 ] < ScoreTqReqSurrenderMrsp_Pre_InheritedPriority_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = ScoreTqReqSurrenderMrsp_Pre_PreviousHelping_Vital;
      ctx->Map.pcs[ 1 ] < ScoreTqReqSurrenderMrsp_Pre_PreviousHelping_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = ScoreTqReqSurrenderMrsp_Pre_Scheduler_Home;
        ctx->Map.pcs[ 2 ] < ScoreTqReqSurrenderMrsp_Pre_Scheduler_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        for (
          ctx->Map.pcs[ 3 ] = ScoreTqReqSurrenderMrsp_Pre_NewHelping_Vital;
          ctx->Map.pcs[ 3 ] < ScoreTqReqSurrenderMrsp_Pre_NewHelping_NA;
          ++ctx->Map.pcs[ 3 ]
        ) {
          for (
            ctx->Map.pcs[ 4 ] = ScoreTqReqSurrenderMrsp_Pre_Suspended_Yes;
            ctx->Map.pcs[ 4 ] < ScoreTqReqSurrenderMrsp_Pre_Suspended_NA;
            ++ctx->Map.pcs[ 4 ]
          ) {
            for (
              ctx->Map.pcs[ 5 ] = ScoreTqReqSurrenderMrsp_Pre_WaitState_IntendToBlock;
              ctx->Map.pcs[ 5 ] < ScoreTqReqSurrenderMrsp_Pre_WaitState_NA;
              ++ctx->Map.pcs[ 5 ]
            ) {
              ctx->Map.entry = ScoreTqReqSurrenderMrsp_PopEntry( ctx );
              ScoreTqReqSurrenderMrsp_Prepare( ctx );
              ScoreTqReqSurrenderMrsp_TestVariant( ctx );
              ScoreTqReqSurrenderMrsp_Cleanup( ctx );
            }
          }
        }
      }
    }
  }

  T_add_remark( &ScoreTqReqSurrenderMrsp_Remark );
  T_pop_fixture();
}

/** @} */
