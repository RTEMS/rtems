/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreTqReqSurrenderPriorityInherit
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

#include <rtems/score/smpbarrier.h>
#include <rtems/score/threadimpl.h>

#include "tr-tq-surrender-priority-inherit.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreTqReqSurrenderPriorityInherit \
 *   spec:/score/tq/req/surrender-priority-inherit
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef struct {
  uint32_t Skip : 1;
  uint32_t Pre_SchedulerCount_NA : 1;
  uint32_t Pre_InheritedPriority_NA : 1;
  uint32_t Pre_PreviousHelping_NA : 1;
  uint32_t Pre_UsedScheduler_NA : 1;
  uint32_t Pre_NewPriority_NA : 1;
  uint32_t Pre_NewHelping_NA : 1;
  uint32_t Pre_Suspended_NA : 1;
  uint32_t Pre_WaitState_NA : 1;
  uint32_t Post_Dequeue : 1;
  uint32_t Post_Unblock : 2;
  uint32_t Post_PreviousOwnerPriority : 2;
  uint32_t Post_NewPriority : 2;
  uint32_t Post_RemoveHelper : 2;
  uint32_t Post_AddHelper : 2;
  uint32_t Post_Suspended : 2;
} ScoreTqReqSurrenderPriorityInherit_Entry;

/**
 * @brief Test context for spec:/score/tq/req/surrender-priority-inherit test
 *   case.
 */
typedef struct {
  /**
   * @brief This member contains the call within ISR request.
   */
  CallWithinISRRequest request;

  /**
   * @brief This member contains the barrier to synchronize the runner and the
   *   worker.
   */
  SMP_barrier_Control barrier;

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
   *   vital priority.
   */
  bool gains_new_priority;

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
   * @brief If this member is true, then the action was performed.
   */
  bool action_performed;

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
   *   ScoreTqReqSurrenderPriorityInherit_Run() parameter.
   */
  TQContext *tq_ctx;

  struct {
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
    ScoreTqReqSurrenderPriorityInherit_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} ScoreTqReqSurrenderPriorityInherit_Context;

static ScoreTqReqSurrenderPriorityInherit_Context
  ScoreTqReqSurrenderPriorityInherit_Instance;

static const char * const ScoreTqReqSurrenderPriorityInherit_PreDesc_SchedulerCount[] = {
  "One",
  "Two",
  "More",
  "NA"
};

static const char * const ScoreTqReqSurrenderPriorityInherit_PreDesc_InheritedPriority[] = {
  "Vital",
  "Dispensable",
  "NA"
};

static const char * const ScoreTqReqSurrenderPriorityInherit_PreDesc_PreviousHelping[] = {
  "Vital",
  "Dispensable",
  "NA"
};

static const char * const ScoreTqReqSurrenderPriorityInherit_PreDesc_UsedScheduler[] = {
  "Home",
  "Helping",
  "NA"
};

static const char * const ScoreTqReqSurrenderPriorityInherit_PreDesc_NewPriority[] = {
  "Vital",
  "Dispensable",
  "NA"
};

static const char * const ScoreTqReqSurrenderPriorityInherit_PreDesc_NewHelping[] = {
  "Vital",
  "Dispensable",
  "NA"
};

static const char * const ScoreTqReqSurrenderPriorityInherit_PreDesc_Suspended[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const ScoreTqReqSurrenderPriorityInherit_PreDesc_WaitState[] = {
  "Blocked",
  "IntendToBlock",
  "NA"
};

static const char * const * const ScoreTqReqSurrenderPriorityInherit_PreDesc[] = {
  ScoreTqReqSurrenderPriorityInherit_PreDesc_SchedulerCount,
  ScoreTqReqSurrenderPriorityInherit_PreDesc_InheritedPriority,
  ScoreTqReqSurrenderPriorityInherit_PreDesc_PreviousHelping,
  ScoreTqReqSurrenderPriorityInherit_PreDesc_UsedScheduler,
  ScoreTqReqSurrenderPriorityInherit_PreDesc_NewPriority,
  ScoreTqReqSurrenderPriorityInherit_PreDesc_NewHelping,
  ScoreTqReqSurrenderPriorityInherit_PreDesc_Suspended,
  ScoreTqReqSurrenderPriorityInherit_PreDesc_WaitState,
  NULL
};

typedef ScoreTqReqSurrenderPriorityInherit_Context Context;

#define NEW_OWNER TQ_BLOCKER_A

#define PREV_OWNER_HELPER_A TQ_BLOCKER_B

#define PREV_OWNER_HELPER_B TQ_BLOCKER_C

#define PREV_OWNER_HELPER_C TQ_BLOCKER_D

#define NEW_OWNER_NEW_HELPER TQ_BLOCKER_E

#define NEW_OWNER_NEW_PRIORITY TQ_WORKER_F

#define NEW_OWNER_OLD_PRIORITY TQ_HELPER_C

#define PREV_OWNER_MOVER TQ_HELPER_A

#define PREV_OWNER TQ_HELPER_A

static const rtems_tcb *GetUnblock( Context *ctx, size_t *index )
{
  return TQGetNextUnblock( ctx->tq_ctx, index )->thread;
}

static const rtems_tcb *GetTCB( Context *ctx, TQWorkerKind worker )
{
  return ctx->tq_ctx->worker_tcb[ worker ];
}

static void Surrender( Context *ctx )
{
  Status_Control status;

  if ( ctx->suspended ) {
    SuspendTask( ctx->tq_ctx->worker_id[ NEW_OWNER ] );
  }

  ctx->priority_before = GetSelfPriority();
  TQSchedulerRecordStart( ctx->tq_ctx );
  status = TQSurrender( ctx->tq_ctx );
  T_eq_int( status, TQConvertStatus( ctx->tq_ctx, STATUS_SUCCESSFUL ) );
  TQSchedulerRecordStop( ctx->tq_ctx );
  T_eq_ptr( TQGetOwner( ctx->tq_ctx ), ctx->tq_ctx->worker_tcb[ NEW_OWNER ] );
  ctx->priority_after = GetSelfPriority();
}

#if defined(RTEMS_SMP)
static void Delay( void *arg )
{
  Context          *ctx;
  SMP_barrier_State state;

  ctx = arg;
  _SMP_barrier_State_initialize( &state );

  /* B0 */
  _SMP_barrier_Wait( &ctx->barrier, &state, 2 );

  /* B1 */
  _SMP_barrier_Wait( &ctx->barrier, &state, 2 );
}

static void SchedulerBlock(
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
    T_scheduler_set_event_handler( NULL, NULL );
    ctx->request.handler = Delay;
    CallWithinISRSubmit( &ctx->request );
  }
}
#endif

static void Setup( Context *ctx )
{
#if defined(RTEMS_SMP)
  TQSetScheduler( ctx->tq_ctx, NEW_OWNER, SCHEDULER_B_ID, PRIO_NORMAL );
  TQSetPriority( ctx->tq_ctx, PREV_OWNER_HELPER_A, PRIO_VERY_HIGH );
  TQSetScheduler(
    ctx->tq_ctx,
    PREV_OWNER_HELPER_B,
    SCHEDULER_B_ID,
    PRIO_HIGH
  );
  TQSetPriority( ctx->tq_ctx, NEW_OWNER_OLD_PRIORITY, PRIO_VERY_LOW );
  TQSetPriority( ctx->tq_ctx, NEW_OWNER_NEW_PRIORITY, PRIO_LOW );

  TQSetPriority( ctx->tq_ctx, PREV_OWNER_MOVER, PRIO_ULTRA_HIGH );

  if ( rtems_scheduler_get_processor_maximum() >= 3 ) {
    TQSetScheduler(
      ctx->tq_ctx,
      PREV_OWNER_HELPER_C,
      SCHEDULER_C_ID,
      PRIO_HIGH
    );
  }
#else
  TQSetPriority( ctx->tq_ctx, NEW_OWNER, PRIO_HIGH );
  TQSetPriority( ctx->tq_ctx, PREV_OWNER_HELPER_A, PRIO_HIGH );
#endif

  TQSetPriority( ctx->tq_ctx, TQ_HELPER_B, PRIO_ULTRA_LOW );
}

static void Action( Context *ctx )
{
  Status_Control    status;
#if defined(RTEMS_SMP)
  SMP_barrier_State state;
#endif

  ctx->action_performed = true;
  ctx->previous_owner = ctx->tq_ctx->runner_id;
  TQMutexObtain( ctx->tq_ctx, TQ_MUTEX_A );
#if defined(RTEMS_SMP)
  TQSendAndWaitForExecutionStop(
    ctx->tq_ctx,
    NEW_OWNER,
    TQ_EVENT_MUTEX_B_OBTAIN
  );
#endif

  if ( ctx->inherited_priorities_are_dispensible ) {
    TQSend(
      ctx->tq_ctx,
      PREV_OWNER_HELPER_A,
      TQ_EVENT_MUTEX_A_OBTAIN | TQ_EVENT_MUTEX_A_RELEASE
    );
  }

#if defined(RTEMS_SMP)
  if ( ctx->helping_schedules_are_dispensible ) {
    TQSendAndWaitForExecutionStop(
      ctx->tq_ctx,
      PREV_OWNER_HELPER_B,
      TQ_EVENT_MUTEX_A_OBTAIN | TQ_EVENT_MUTEX_A_RELEASE
    );

    if ( ctx->gains_new_priority && ctx->gains_new_helping_scheduler ) {
      TQSendAndWaitForExecutionStop(
        ctx->tq_ctx,
        PREV_OWNER_HELPER_C,
        TQ_EVENT_MUTEX_A_OBTAIN | TQ_EVENT_MUTEX_A_RELEASE
      );
    }
  }
#endif

  status = TQEnqueue( ctx->tq_ctx, TQ_NO_WAIT );
  T_eq_int( status, TQConvertStatus( ctx->tq_ctx, STATUS_SUCCESSFUL ) );

#if defined(RTEMS_SMP)
  if ( ctx->intend_to_block ) {
    _SMP_barrier_Control_initialize( &ctx->barrier );
    _SMP_barrier_State_initialize( &state );

    T_scheduler_set_event_handler( SchedulerBlock, ctx );
    TQSend( ctx->tq_ctx, NEW_OWNER, TQ_EVENT_ENQUEUE );

    /* B0 */
    _SMP_barrier_Wait( &ctx->barrier, &state, 2 );
  } else {
    TQSendAndWaitForExecutionStop(
      ctx->tq_ctx,
      NEW_OWNER,
      TQ_EVENT_ENQUEUE
    );
  }

  if ( ctx->gains_new_priority ) {
    TQSend(
      ctx->tq_ctx,
      NEW_OWNER_OLD_PRIORITY,
      TQ_EVENT_HELPER_B_SYNC | TQ_EVENT_MUTEX_B_OBTAIN
    );
    TQSynchronizeRunner();
    TQSend(
      ctx->tq_ctx,
      NEW_OWNER_NEW_PRIORITY,
      TQ_EVENT_HELPER_B_SYNC | TQ_EVENT_ENQUEUE
    );
    TQSynchronizeRunner();

    if ( ctx->gains_new_helping_scheduler ) {
      TQSetScheduler(
        ctx->tq_ctx,
        NEW_OWNER_NEW_HELPER,
        SCHEDULER_C_ID,
        PRIO_LOW
      );
      TQSendAndWaitForExecutionStop(
        ctx->tq_ctx,
        NEW_OWNER_NEW_HELPER,
        TQ_EVENT_ENQUEUE
      );
    }
  } else if ( ctx->gains_new_helping_scheduler ) {
    TQSetScheduler(
      ctx->tq_ctx,
      NEW_OWNER_NEW_HELPER,
      SCHEDULER_A_ID,
      PRIO_LOW
    );
    TQSend(
      ctx->tq_ctx,
      NEW_OWNER_NEW_HELPER,
      TQ_EVENT_HELPER_B_SYNC | TQ_EVENT_ENQUEUE
    );
    TQSynchronizeRunner();
  }

  /*
   * In order to run the validation test variant also for the intend to block
   * wait state, we would need at least three processors.  Skip it for now.
   */
  if ( ctx->use_helping_scheduler && !ctx->intend_to_block ) {
    ctx->tq_ctx->busy_wait[ PREV_OWNER_MOVER ] = true;
    TQSend( ctx->tq_ctx, PREV_OWNER_MOVER, TQ_EVENT_BUSY_WAIT );

    while ( rtems_scheduler_get_processor() != 1 ) {
      /* Wait */
    }

    ctx->tq_ctx->busy_wait[ PREV_OWNER_MOVER ] = false;
  }
#else
  TQSend(
    ctx->tq_ctx,
    NEW_OWNER,
    TQ_EVENT_HELPER_B_SYNC | TQ_EVENT_ENQUEUE
  );
  TQSynchronizeRunner();
#endif

  Surrender( ctx );

#if defined(RTEMS_SMP)
  if ( ctx->intend_to_block ) {
    /* B1 */
    _SMP_barrier_Wait( &ctx->barrier, &state, 2 );
  }
#endif
}

static void Cleanup( Context *ctx )
{
  if ( ctx->suspended ) {
    ResumeTask( ctx->tq_ctx->worker_id[ NEW_OWNER ] );
  }

  TQSendAndSynchronizeRunner(
    ctx->tq_ctx,
    NEW_OWNER,
    TQ_EVENT_SURRENDER
  );
  TQWaitForExecutionStop( ctx->tq_ctx, NEW_OWNER );

#if defined(RTEMS_SMP)
  TQSendAndWaitForExecutionStop(
    ctx->tq_ctx,
    NEW_OWNER,
    TQ_EVENT_MUTEX_B_RELEASE
  );

  if ( ctx->gains_new_priority ) {
    TQSendAndSynchronizeRunner(
      ctx->tq_ctx,
      NEW_OWNER_OLD_PRIORITY,
      TQ_EVENT_MUTEX_B_RELEASE
    );
    TQSendAndSynchronizeRunner(
      ctx->tq_ctx,
      NEW_OWNER_NEW_PRIORITY,
      TQ_EVENT_SURRENDER
    );
  }

  if ( ctx->gains_new_helping_scheduler ) {
    TQSendAndSynchronizeRunner(
      ctx->tq_ctx,
      NEW_OWNER_NEW_HELPER,
      TQ_EVENT_SURRENDER
    );
  }
#endif

  TQMutexRelease( ctx->tq_ctx, TQ_MUTEX_A );
  TQMutexObtain( ctx->tq_ctx, TQ_MUTEX_A );
  TQMutexRelease( ctx->tq_ctx, TQ_MUTEX_A );

  T_eq_u32( rtems_scheduler_get_processor(), 0 );
}

static void SetupSticky( Context *ctx )
{
#if defined(RTEMS_SMP)
  TQSetScheduler( ctx->tq_ctx, NEW_OWNER, SCHEDULER_B_ID, PRIO_NORMAL );
  TQSetPriority( ctx->tq_ctx, PREV_OWNER_HELPER_A, PRIO_VERY_HIGH );
  TQSetScheduler( ctx->tq_ctx,
    PREV_OWNER_HELPER_B,
    SCHEDULER_B_ID,
    PRIO_LOW
  );
  TQSetPriority( ctx->tq_ctx, NEW_OWNER_NEW_HELPER, PRIO_VERY_HIGH );
  TQSetPriority( ctx->tq_ctx, PREV_OWNER, PRIO_NORMAL );
#endif
}

static void ActionSticky( Context *ctx )
{
#if defined(RTEMS_SMP)
  ctx->action_performed = true;
  ctx->previous_owner = ctx->tq_ctx->worker_id[ PREV_OWNER ];

  SetSelfPriority( PRIO_LOW );

  if (
    ctx->inherited_priorities_are_dispensible ||
    ctx->helping_schedules_are_dispensible
  ) {
    TQSend( ctx->tq_ctx, PREV_OWNER, TQ_EVENT_MUTEX_A_OBTAIN );

    if ( ctx->inherited_priorities_are_dispensible ) {
      TQSend( ctx->tq_ctx, PREV_OWNER_HELPER_A, TQ_EVENT_MUTEX_A_OBTAIN );
    }

    if ( ctx->helping_schedules_are_dispensible ) {
      TQSendAndWaitForExecutionStop(
        ctx->tq_ctx,
        PREV_OWNER_HELPER_B,
        TQ_EVENT_MUTEX_A_OBTAIN
      );
    }
  }

  /*
   * Take only the priorities into account which are inherited from the
   * priority inheritance mutex.  This avoids having to deal with the ceiling
   * priority.
   */
  ctx->priority_before = TQGetPriority( ctx->tq_ctx, PREV_OWNER );

  SetSelfScheduler( SCHEDULER_B_ID, PRIO_ULTRA_HIGH );
  ctx->tq_ctx->busy_wait[ PREV_OWNER ] = true;
  TQSendAndSynchronizeRunner(
    ctx->tq_ctx,
    PREV_OWNER,
    TQ_EVENT_ENQUEUE | TQ_EVENT_BUSY_WAIT
  );
  SetSelfScheduler( SCHEDULER_A_ID, PRIO_ULTRA_HIGH );

  TQSendAndWaitForIntendToBlock(
    ctx->tq_ctx,
    NEW_OWNER,
    TQ_EVENT_ENQUEUE
  );

  SetSelfScheduler( SCHEDULER_B_ID, PRIO_ULTRA_HIGH );

  if ( ctx->gains_new_helping_scheduler ) {
    TQSend(
      ctx->tq_ctx,
      NEW_OWNER_NEW_HELPER,
      TQ_EVENT_ENQUEUE
    );
    YieldTask( ctx->tq_ctx->worker_id[ PREV_OWNER ] );
    TQWaitForEventsReceived( ctx->tq_ctx, NEW_OWNER_NEW_HELPER );
    TQWaitForIntendToBlock( ctx->tq_ctx, NEW_OWNER_NEW_HELPER );
    YieldTask( ctx->tq_ctx->worker_id[ NEW_OWNER_NEW_HELPER ] );
  }

  if ( ctx->use_helping_scheduler ) {
    SetSelfScheduler( SCHEDULER_A_ID, PRIO_ULTRA_HIGH );
  }

  if ( ctx->suspended ) {
    SuspendTask( ctx->tq_ctx->worker_id[ NEW_OWNER ] );
  }

  ctx->tq_ctx->busy_wait[ PREV_OWNER ] = false;
  TQSendAndWaitForExecutionStop(
    ctx->tq_ctx,
    PREV_OWNER,
    TQ_EVENT_SCHEDULER_RECORD_START |
      TQ_EVENT_SURRENDER
  );
  TQSchedulerRecordStop( ctx->tq_ctx );
  T_eq_ptr(
    TQGetOwner( ctx->tq_ctx ),
    ctx->tq_ctx->worker_tcb[ NEW_OWNER ]
  );
  ctx->priority_after = TQGetPriority( ctx->tq_ctx, PREV_OWNER );
#endif
}

static void CleanupSticky( Context *ctx )
{
#if defined(RTEMS_SMP)
  SetSelfScheduler( SCHEDULER_A_ID, PRIO_ULTRA_HIGH );

  if ( ctx->suspended ) {
    ResumeTask( ctx->tq_ctx->worker_id[ NEW_OWNER ] );
  }

  TQSendAndSynchronizeRunner(
    ctx->tq_ctx,
    NEW_OWNER,
    TQ_EVENT_SURRENDER
  );

  if ( ctx->gains_new_helping_scheduler ) {
    TQSendAndSynchronizeRunner(
      ctx->tq_ctx,
      NEW_OWNER_NEW_HELPER,
      TQ_EVENT_SURRENDER
    );
  }

  if (
    ctx->inherited_priorities_are_dispensible ||
    ctx->helping_schedules_are_dispensible
  ) {
    TQSendAndSynchronizeRunner(
      ctx->tq_ctx,
      PREV_OWNER,
      TQ_EVENT_MUTEX_A_RELEASE
    );

    if ( ctx->inherited_priorities_are_dispensible ) {
      TQSendAndSynchronizeRunner(
        ctx->tq_ctx,
        PREV_OWNER_HELPER_A,
        TQ_EVENT_MUTEX_A_RELEASE
      );
    }

    if ( ctx->helping_schedules_are_dispensible ) {
      TQSendAndSynchronizeRunner(
        ctx->tq_ctx,
        PREV_OWNER_HELPER_B,
        TQ_EVENT_MUTEX_A_RELEASE
      );
    }
  }

  T_eq_u32( rtems_scheduler_get_processor(), 0 );
#endif
}

static void ScoreTqReqSurrenderPriorityInherit_Pre_SchedulerCount_Prepare(
  ScoreTqReqSurrenderPriorityInherit_Context           *ctx,
  ScoreTqReqSurrenderPriorityInherit_Pre_SchedulerCount state
)
{
  switch ( state ) {
    case ScoreTqReqSurrenderPriorityInherit_Pre_SchedulerCount_One: {
      /*
       * Where the system has exactly one schedulers.
       */
      if ( rtems_scheduler_get_processor_maximum() != 1 ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTqReqSurrenderPriorityInherit_Pre_SchedulerCount_Two: {
      /*
       * Where the system has exactly two schedulers.
       */
      if ( rtems_scheduler_get_processor_maximum() != 2 ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTqReqSurrenderPriorityInherit_Pre_SchedulerCount_More: {
      /*
       * Where the system has at least three schedulers.
       */
      if ( rtems_scheduler_get_processor_maximum() < 3 ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTqReqSurrenderPriorityInherit_Pre_SchedulerCount_NA:
      break;
  }
}

static void ScoreTqReqSurrenderPriorityInherit_Pre_InheritedPriority_Prepare(
  ScoreTqReqSurrenderPriorityInherit_Context              *ctx,
  ScoreTqReqSurrenderPriorityInherit_Pre_InheritedPriority state
)
{
  switch ( state ) {
    case ScoreTqReqSurrenderPriorityInherit_Pre_InheritedPriority_Vital: {
      /*
       * While at least one priority inherited through the thread queue for the
       * previous owner is the highest priority of the previous owner.
       */
      ctx->inherited_priorities_are_dispensible = false;
      break;
    }

    case ScoreTqReqSurrenderPriorityInherit_Pre_InheritedPriority_Dispensable: {
      /*
       * While all priorities inherited through the thread queue for the
       * previous owner are not the highest priority of the previous owner.
       */
      ctx->inherited_priorities_are_dispensible = true;
      break;
    }

    case ScoreTqReqSurrenderPriorityInherit_Pre_InheritedPriority_NA:
      break;
  }
}

static void ScoreTqReqSurrenderPriorityInherit_Pre_PreviousHelping_Prepare(
  ScoreTqReqSurrenderPriorityInherit_Context            *ctx,
  ScoreTqReqSurrenderPriorityInherit_Pre_PreviousHelping state
)
{
  switch ( state ) {
    case ScoreTqReqSurrenderPriorityInherit_Pre_PreviousHelping_Vital: {
      /*
       * While at least one helping scheduler of the previous owner is only
       * available due to a priority inherited through the thread queue.
       */
      ctx->helping_schedules_are_dispensible = false;
      break;
    }

    case ScoreTqReqSurrenderPriorityInherit_Pre_PreviousHelping_Dispensable: {
      /*
       * While all helping scheduler of the previous owner are not only
       * available due to a priority inherited through the thread queue.
       */
      ctx->helping_schedules_are_dispensible = true;
      break;
    }

    case ScoreTqReqSurrenderPriorityInherit_Pre_PreviousHelping_NA:
      break;
  }
}

static void ScoreTqReqSurrenderPriorityInherit_Pre_UsedScheduler_Prepare(
  ScoreTqReqSurrenderPriorityInherit_Context          *ctx,
  ScoreTqReqSurrenderPriorityInherit_Pre_UsedScheduler state
)
{
  switch ( state ) {
    case ScoreTqReqSurrenderPriorityInherit_Pre_UsedScheduler_Home: {
      /*
       * While the previous owner executes in its home scheduler.
       */
      ctx->use_helping_scheduler = false;
      break;
    }

    case ScoreTqReqSurrenderPriorityInherit_Pre_UsedScheduler_Helping: {
      /*
       * While the previous owner executes in a helping scheduler which is
       * available due to a priority inherited through the thread queue.
       */
      ctx->use_helping_scheduler = true;
      break;
    }

    case ScoreTqReqSurrenderPriorityInherit_Pre_UsedScheduler_NA:
      break;
  }
}

static void ScoreTqReqSurrenderPriorityInherit_Pre_NewPriority_Prepare(
  ScoreTqReqSurrenderPriorityInherit_Context        *ctx,
  ScoreTqReqSurrenderPriorityInherit_Pre_NewPriority state
)
{
  switch ( state ) {
    case ScoreTqReqSurrenderPriorityInherit_Pre_NewPriority_Vital: {
      /*
       * While at least one highest priority of the new owner is only available
       * due to a priority inherited through the thread queue.
       */
      ctx->gains_new_priority = true;
      break;
    }

    case ScoreTqReqSurrenderPriorityInherit_Pre_NewPriority_Dispensable: {
      /*
       * While all highest priorities of the new owner are not only available
       * due to a priority inherited through the thread queue.
       */
      ctx->gains_new_priority = false;
      break;
    }

    case ScoreTqReqSurrenderPriorityInherit_Pre_NewPriority_NA:
      break;
  }
}

static void ScoreTqReqSurrenderPriorityInherit_Pre_NewHelping_Prepare(
  ScoreTqReqSurrenderPriorityInherit_Context       *ctx,
  ScoreTqReqSurrenderPriorityInherit_Pre_NewHelping state
)
{
  switch ( state ) {
    case ScoreTqReqSurrenderPriorityInherit_Pre_NewHelping_Vital: {
      /*
       * While at least one helping scheduler of the new owner is only
       * available due to a priority inherited through the thread queue.
       */
      ctx->gains_new_helping_scheduler = true;
      break;
    }

    case ScoreTqReqSurrenderPriorityInherit_Pre_NewHelping_Dispensable: {
      /*
       * While all helping scheduler of the new owner are not only available
       * due to a priority inherited through the thread queue.
       */
      ctx->gains_new_helping_scheduler = false;
      break;
    }

    case ScoreTqReqSurrenderPriorityInherit_Pre_NewHelping_NA:
      break;
  }
}

static void ScoreTqReqSurrenderPriorityInherit_Pre_Suspended_Prepare(
  ScoreTqReqSurrenderPriorityInherit_Context      *ctx,
  ScoreTqReqSurrenderPriorityInherit_Pre_Suspended state
)
{
  switch ( state ) {
    case ScoreTqReqSurrenderPriorityInherit_Pre_Suspended_Yes: {
      /*
       * While the new owner is suspended.
       */
      ctx->suspended = true;
      break;
    }

    case ScoreTqReqSurrenderPriorityInherit_Pre_Suspended_No: {
      /*
       * While the new owner is not suspended.
       */
      ctx->suspended = false;
      break;
    }

    case ScoreTqReqSurrenderPriorityInherit_Pre_Suspended_NA:
      break;
  }
}

static void ScoreTqReqSurrenderPriorityInherit_Pre_WaitState_Prepare(
  ScoreTqReqSurrenderPriorityInherit_Context      *ctx,
  ScoreTqReqSurrenderPriorityInherit_Pre_WaitState state
)
{
  switch ( state ) {
    case ScoreTqReqSurrenderPriorityInherit_Pre_WaitState_Blocked: {
      /*
       * While the new owner is in the blocked wait state.
       */
      ctx->intend_to_block = false;
      break;
    }

    case ScoreTqReqSurrenderPriorityInherit_Pre_WaitState_IntendToBlock: {
      /*
       * While the new owner is in the intend to block wait state.
       */
      ctx->intend_to_block = true;
      break;
    }

    case ScoreTqReqSurrenderPriorityInherit_Pre_WaitState_NA:
      break;
  }
}

static void ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Check(
  ScoreTqReqSurrenderPriorityInherit_Context     *ctx,
  ScoreTqReqSurrenderPriorityInherit_Post_Dequeue state
)
{
  switch ( state ) {
    case ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority: {
      /*
       * The first thread in priority order shall be dequeued from the thread
       * queue.
       */
      /* Validation is done by spec:/score/tq/req/enqueue-priority */
      break;
    }

    case ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_NA:
      break;
  }
}

static void ScoreTqReqSurrenderPriorityInherit_Post_Unblock_Check(
  ScoreTqReqSurrenderPriorityInherit_Context     *ctx,
  ScoreTqReqSurrenderPriorityInherit_Post_Unblock state
)
{
  size_t i;

  i = 0;

  switch ( state ) {
    case ScoreTqReqSurrenderPriorityInherit_Post_Unblock_Yes: {
      /*
       * The dequeued thread shall be unblocked by the thread queue surrender
       * operation.
       */
      T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, NEW_OWNER ) );
      T_eq_ptr( GetUnblock( ctx, &i ), NULL );
      break;
    }

    case ScoreTqReqSurrenderPriorityInherit_Post_Unblock_No: {
      /*
       * The dequeued thread shall not be unblocked by the thread queue
       * surrender operation.
       */
      T_eq_ptr( GetUnblock( ctx, &i ), NULL );
      break;
    }

    case ScoreTqReqSurrenderPriorityInherit_Post_Unblock_NA:
      break;
  }
}

static void
ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Check(
  ScoreTqReqSurrenderPriorityInherit_Context                   *ctx,
  ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority state
)
{
  switch ( state ) {
    case ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Drop: {
      /*
       * Each eligible priority of the previous owner which had the highest
       * priority inherited through the thread queue shall be updated.
       */
      T_eq_u32( ctx->priority_after, PRIO_NORMAL );
      break;
    }

    case ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Nop: {
      /*
       * No eligible priority of the previous owner shall be updated.
       */
      T_eq_u32( ctx->priority_after, ctx->priority_before );
      break;
    }

    case ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_NA:
      break;
  }
}

static void ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Check(
  ScoreTqReqSurrenderPriorityInherit_Context         *ctx,
  ScoreTqReqSurrenderPriorityInherit_Post_NewPriority state
)
{
  rtems_id            scheduler_id;
  rtems_task_priority priority;
  rtems_status_code   sc;

  switch ( state ) {
    case ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Raise: {
      /*
       * Each eligible priority of the new owner which inherited the highest
       * priority through the thread queue shall be updated.
       */
      sc = rtems_task_get_priority(
        ctx->tq_ctx->worker_id[ NEW_OWNER ],
        SCHEDULER_A_ID,
        &priority
      );
      T_rsc_success( sc );
      T_eq_u32( priority, PRIO_LOW );
      break;
    }

    case ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Nop: {
      /*
       * No highest priority of the new owner shall be changed.
       */
      if ( ctx->gains_new_helping_scheduler ) {
        scheduler_id = SCHEDULER_C_ID;
      } else {
        scheduler_id = SCHEDULER_A_ID;
      }

      sc = rtems_task_get_priority(
        ctx->tq_ctx->worker_id[ NEW_OWNER ],
        scheduler_id,
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

    case ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_NA:
      break;
  }
}

static void ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_Check(
  ScoreTqReqSurrenderPriorityInherit_Context          *ctx,
  ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper state
)
{
  rtems_task_priority priority;
  rtems_status_code   sc;

  switch ( state ) {
    case ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_Yes: {
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

    case ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_No: {
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

    case ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_NA:
      break;
  }
}

static void ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_Check(
  ScoreTqReqSurrenderPriorityInherit_Context       *ctx,
  ScoreTqReqSurrenderPriorityInherit_Post_AddHelper state
)
{
  rtems_id            scheduler_id;
  rtems_task_priority priority;
  rtems_status_code   sc;

  switch ( state ) {
    case ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_Yes: {
      /*
       * Each helping scheduler of the new owner which is only available due to
       * a priority inherited through the thread queue shall be added to the
       * new owner.
       */
      if ( ctx->gains_new_priority ) {
        scheduler_id = SCHEDULER_C_ID;
      } else {
        scheduler_id = SCHEDULER_A_ID;
      }

      sc = rtems_task_get_priority(
        ctx->tq_ctx->worker_id[ NEW_OWNER ],
        scheduler_id,
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

    case ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_No: {
      /*
       * No helping scheduler shall added to the new owner.
       */
      if ( ctx->gains_new_priority ) {
        scheduler_id = SCHEDULER_C_ID;
      } else {
        scheduler_id = SCHEDULER_A_ID;
      }

      sc = rtems_task_get_priority(
        ctx->tq_ctx->worker_id[ NEW_OWNER ],
        scheduler_id,
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

    case ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_NA:
      break;
  }
}

static void ScoreTqReqSurrenderPriorityInherit_Post_Suspended_Check(
  ScoreTqReqSurrenderPriorityInherit_Context       *ctx,
  ScoreTqReqSurrenderPriorityInherit_Post_Suspended state
)
{
  switch ( state ) {
    case ScoreTqReqSurrenderPriorityInherit_Post_Suspended_Yes: {
      /*
       * The new owner shall be suspended.
       */
      T_true( IsTaskSuspended( ctx->tq_ctx->worker_id[ NEW_OWNER ] ) );
      break;
    }

    case ScoreTqReqSurrenderPriorityInherit_Post_Suspended_No: {
      /*
       * The new owner shall be not suspended.
       */
      T_false( IsTaskSuspended( ctx->tq_ctx->worker_id[ NEW_OWNER ] ) );
      break;
    }

    case ScoreTqReqSurrenderPriorityInherit_Post_Suspended_NA:
      break;
  }
}

static void ScoreTqReqSurrenderPriorityInherit_Setup(
  ScoreTqReqSurrenderPriorityInherit_Context *ctx
)
{
  ctx->request.arg = ctx;
  TQReset( ctx->tq_ctx );

  if ( ctx->tq_ctx->enqueue_variant == TQ_ENQUEUE_STICKY ) {
    SetupSticky( ctx );
  } else {
    Setup( ctx );
  }
}

static void ScoreTqReqSurrenderPriorityInherit_Setup_Wrap( void *arg )
{
  ScoreTqReqSurrenderPriorityInherit_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  ScoreTqReqSurrenderPriorityInherit_Setup( ctx );
}

static void ScoreTqReqSurrenderPriorityInherit_Teardown(
  ScoreTqReqSurrenderPriorityInherit_Context *ctx
)
{
  SetSelfScheduler( SCHEDULER_A_ID, PRIO_NORMAL );
}

static void ScoreTqReqSurrenderPriorityInherit_Teardown_Wrap( void *arg )
{
  ScoreTqReqSurrenderPriorityInherit_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  ScoreTqReqSurrenderPriorityInherit_Teardown( ctx );
}

static void ScoreTqReqSurrenderPriorityInherit_Prepare(
  ScoreTqReqSurrenderPriorityInherit_Context *ctx
)
{
  ctx->action_performed = false;
  ctx->inherited_priorities_are_dispensible = true;
  ctx->helping_schedules_are_dispensible = true;
  ctx->use_helping_scheduler = false;
  ctx->gains_new_priority = false;
  ctx->gains_new_helping_scheduler = false;
  ctx->intend_to_block = false;
}

static void ScoreTqReqSurrenderPriorityInherit_Action(
  ScoreTqReqSurrenderPriorityInherit_Context *ctx
)
{
  if ( ctx->tq_ctx->enqueue_variant == TQ_ENQUEUE_STICKY ) {
    ActionSticky( ctx );
  } else {
    Action( ctx );
  }
}

static void ScoreTqReqSurrenderPriorityInherit_Cleanup(
  ScoreTqReqSurrenderPriorityInherit_Context *ctx
)
{
  if ( ctx->action_performed ) {
    if ( ctx->tq_ctx->enqueue_variant == TQ_ENQUEUE_STICKY ) {
      CleanupSticky( ctx );
    } else {
      Cleanup( ctx );
    }
  }
}

static const ScoreTqReqSurrenderPriorityInherit_Entry
ScoreTqReqSurrenderPriorityInherit_Entries[] = {
  { 1, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_NA },
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_NA },
#else
  { 1, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_NA },
#endif
  { 1, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_NA },
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_NA },
#else
  { 1, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_NA },
#endif
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_No,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Drop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_Yes },
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_No,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Drop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_Yes },
#endif
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Drop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_No },
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_No,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Drop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_No },
#endif
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_No,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_Yes },
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_No,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_Yes },
#endif
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_No },
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_No,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_No },
#endif
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_No,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Drop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Raise,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_Yes },
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_No,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Drop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Raise,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_Yes },
#endif
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Drop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Raise,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_No },
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_No,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Drop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Raise,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_No },
#endif
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_No,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Drop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_Yes },
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_No,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Drop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_Yes },
#endif
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Drop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_No },
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_No,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Drop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_No },
#endif
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_No,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Drop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_Yes },
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_No,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Drop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_Yes },
#endif
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Drop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_No },
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_No,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Drop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_No },
#endif
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_No,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Drop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Raise,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_Yes },
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_No,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Drop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Raise,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_Yes },
#endif
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Drop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Raise,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_No },
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_No,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Drop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Raise,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_No },
#endif
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_No,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Drop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_Yes },
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_No,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Drop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_Yes },
#endif
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Drop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_No },
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_No,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Drop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_No },
#endif
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_No,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Raise,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_Yes },
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_No,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Raise,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_Yes },
#endif
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Raise,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_No },
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_No,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Raise,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_No },
#endif
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_No,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_Yes },
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_No,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_Yes },
#endif
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_No },
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_No,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_No },
#endif
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_No,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_Yes },
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_No,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_Yes },
#endif
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_No },
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_No,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_No },
#endif
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_No,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Raise,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_Yes },
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_No,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Raise,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_Yes },
#endif
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Raise,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_No },
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_No,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Raise,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_No },
#endif
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_No,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_Yes },
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_No,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_Yes },
#endif
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_No },
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_No,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_No },
#endif
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_No,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Drop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Raise,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_Yes },
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_No,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Drop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Raise,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_Yes },
#endif
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Drop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Raise,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_No },
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_No,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Drop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Raise,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_No },
#endif
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_No,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Drop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Raise,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_Yes },
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_No,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Drop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Raise,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_Yes },
#endif
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Drop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Raise,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_No },
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_No,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Drop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Raise,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_No },
#endif
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_No,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Raise,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_Yes },
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_No,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Raise,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_Yes },
#endif
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Raise,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_No },
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_No,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Raise,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_No },
#endif
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_No,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Raise,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_Yes },
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_No,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Raise,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_Yes },
#endif
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Raise,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_No },
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_NA,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_NA }
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Priority,
    ScoreTqReqSurrenderPriorityInherit_Post_Unblock_No,
    ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Nop,
    ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Raise,
    ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_No,
    ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_Yes,
    ScoreTqReqSurrenderPriorityInherit_Post_Suspended_No }
#endif
};

static const uint8_t
ScoreTqReqSurrenderPriorityInherit_Map[] = {
  0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
  0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 4, 5, 6, 7, 0, 1, 0, 1,
  0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
  0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
  0, 1, 0, 1, 8, 9, 10, 11, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 2,
  3, 2, 3, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 2, 3, 2, 3, 12, 13,
  14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 2, 3, 2, 3, 24, 25, 26, 27, 28, 29,
  30, 31, 4, 5, 6, 7, 2, 3, 2, 3, 24, 25, 26, 27, 28, 29, 30, 31, 4, 5, 6, 7,
  2, 3, 2, 3, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 2, 3, 2, 3, 32,
  33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 2, 3, 2, 3, 44, 45, 46, 47, 48,
  49, 50, 51, 8, 9, 10, 11, 2, 3, 2, 3, 44, 45, 46, 47, 48, 49, 50, 51, 8, 9,
  10, 11, 52, 53, 54, 55, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 52,
  53, 54, 55, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 56, 57, 58, 59,
  24, 25, 26, 27, 28, 29, 30, 31, 4, 5, 6, 7, 56, 57, 58, 59, 24, 25, 26, 27,
  28, 29, 30, 31, 4, 5, 6, 7, 60, 61, 62, 63, 32, 33, 34, 35, 36, 37, 38, 39,
  40, 41, 42, 43, 60, 61, 62, 63, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42,
  43, 64, 65, 66, 67, 44, 45, 46, 47, 48, 49, 50, 51, 8, 9, 10, 11, 64, 65, 66,
  67, 44, 45, 46, 47, 48, 49, 50, 51, 8, 9, 10, 11
};

static size_t ScoreTqReqSurrenderPriorityInherit_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  ScoreTqReqSurrenderPriorityInherit_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      ScoreTqReqSurrenderPriorityInherit_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture ScoreTqReqSurrenderPriorityInherit_Fixture = {
  .setup = ScoreTqReqSurrenderPriorityInherit_Setup_Wrap,
  .stop = NULL,
  .teardown = ScoreTqReqSurrenderPriorityInherit_Teardown_Wrap,
  .scope = ScoreTqReqSurrenderPriorityInherit_Scope,
  .initial_context = &ScoreTqReqSurrenderPriorityInherit_Instance
};

static const uint8_t ScoreTqReqSurrenderPriorityInherit_Weights[] = {
  128, 64, 32, 16, 8, 4, 2, 1
};

static void ScoreTqReqSurrenderPriorityInherit_Skip(
  ScoreTqReqSurrenderPriorityInherit_Context *ctx,
  size_t                                      index
)
{
  switch ( index + 1 ) {
    case 1:
      ctx->Map.pcs[ 1 ] = ScoreTqReqSurrenderPriorityInherit_Pre_InheritedPriority_NA - 1;
      /* Fall through */
    case 2:
      ctx->Map.pcs[ 2 ] = ScoreTqReqSurrenderPriorityInherit_Pre_PreviousHelping_NA - 1;
      /* Fall through */
    case 3:
      ctx->Map.pcs[ 3 ] = ScoreTqReqSurrenderPriorityInherit_Pre_UsedScheduler_NA - 1;
      /* Fall through */
    case 4:
      ctx->Map.pcs[ 4 ] = ScoreTqReqSurrenderPriorityInherit_Pre_NewPriority_NA - 1;
      /* Fall through */
    case 5:
      ctx->Map.pcs[ 5 ] = ScoreTqReqSurrenderPriorityInherit_Pre_NewHelping_NA - 1;
      /* Fall through */
    case 6:
      ctx->Map.pcs[ 6 ] = ScoreTqReqSurrenderPriorityInherit_Pre_Suspended_NA - 1;
      /* Fall through */
    case 7:
      ctx->Map.pcs[ 7 ] = ScoreTqReqSurrenderPriorityInherit_Pre_WaitState_NA - 1;
      break;
  }
}

static inline ScoreTqReqSurrenderPriorityInherit_Entry
ScoreTqReqSurrenderPriorityInherit_PopEntry(
  ScoreTqReqSurrenderPriorityInherit_Context *ctx
)
{
  size_t index;

  if ( ctx->Map.skip ) {
    size_t i;

    ctx->Map.skip = false;
    index = 0;

    for ( i = 0; i < 8; ++i ) {
      index += ScoreTqReqSurrenderPriorityInherit_Weights[ i ] * ctx->Map.pcs[ i ];
    }
  } else {
    index = ctx->Map.index;
  }

  ctx->Map.index = index + 1;

  return ScoreTqReqSurrenderPriorityInherit_Entries[
    ScoreTqReqSurrenderPriorityInherit_Map[ index ]
  ];
}

static void ScoreTqReqSurrenderPriorityInherit_TestVariant(
  ScoreTqReqSurrenderPriorityInherit_Context *ctx
)
{
  ScoreTqReqSurrenderPriorityInherit_Pre_SchedulerCount_Prepare(
    ctx,
    ctx->Map.pcs[ 0 ]
  );

  if ( ctx->Map.skip ) {
    ScoreTqReqSurrenderPriorityInherit_Skip( ctx, 0 );
    return;
  }

  ScoreTqReqSurrenderPriorityInherit_Pre_InheritedPriority_Prepare(
    ctx,
    ctx->Map.pcs[ 1 ]
  );
  ScoreTqReqSurrenderPriorityInherit_Pre_PreviousHelping_Prepare(
    ctx,
    ctx->Map.pcs[ 2 ]
  );
  ScoreTqReqSurrenderPriorityInherit_Pre_UsedScheduler_Prepare(
    ctx,
    ctx->Map.pcs[ 3 ]
  );
  ScoreTqReqSurrenderPriorityInherit_Pre_NewPriority_Prepare(
    ctx,
    ctx->Map.pcs[ 4 ]
  );
  ScoreTqReqSurrenderPriorityInherit_Pre_NewHelping_Prepare(
    ctx,
    ctx->Map.pcs[ 5 ]
  );
  ScoreTqReqSurrenderPriorityInherit_Pre_Suspended_Prepare(
    ctx,
    ctx->Map.pcs[ 6 ]
  );
  ScoreTqReqSurrenderPriorityInherit_Pre_WaitState_Prepare(
    ctx,
    ctx->Map.pcs[ 7 ]
  );
  ScoreTqReqSurrenderPriorityInherit_Action( ctx );
  ScoreTqReqSurrenderPriorityInherit_Post_Dequeue_Check(
    ctx,
    ctx->Map.entry.Post_Dequeue
  );
  ScoreTqReqSurrenderPriorityInherit_Post_Unblock_Check(
    ctx,
    ctx->Map.entry.Post_Unblock
  );
  ScoreTqReqSurrenderPriorityInherit_Post_PreviousOwnerPriority_Check(
    ctx,
    ctx->Map.entry.Post_PreviousOwnerPriority
  );
  ScoreTqReqSurrenderPriorityInherit_Post_NewPriority_Check(
    ctx,
    ctx->Map.entry.Post_NewPriority
  );
  ScoreTqReqSurrenderPriorityInherit_Post_RemoveHelper_Check(
    ctx,
    ctx->Map.entry.Post_RemoveHelper
  );
  ScoreTqReqSurrenderPriorityInherit_Post_AddHelper_Check(
    ctx,
    ctx->Map.entry.Post_AddHelper
  );
  ScoreTqReqSurrenderPriorityInherit_Post_Suspended_Check(
    ctx,
    ctx->Map.entry.Post_Suspended
  );
}

static T_fixture_node ScoreTqReqSurrenderPriorityInherit_Node;

static T_remark ScoreTqReqSurrenderPriorityInherit_Remark = {
  .next = NULL,
  .remark = "ScoreTqReqSurrenderPriorityInherit"
};

void ScoreTqReqSurrenderPriorityInherit_Run( TQContext *tq_ctx )
{
  ScoreTqReqSurrenderPriorityInherit_Context *ctx;

  ctx = &ScoreTqReqSurrenderPriorityInherit_Instance;
  ctx->tq_ctx = tq_ctx;

  ctx = T_push_fixture(
    &ScoreTqReqSurrenderPriorityInherit_Node,
    &ScoreTqReqSurrenderPriorityInherit_Fixture
  );
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;
  ctx->Map.skip = false;

  for (
    ctx->Map.pcs[ 0 ] = ScoreTqReqSurrenderPriorityInherit_Pre_SchedulerCount_One;
    ctx->Map.pcs[ 0 ] < ScoreTqReqSurrenderPriorityInherit_Pre_SchedulerCount_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = ScoreTqReqSurrenderPriorityInherit_Pre_InheritedPriority_Vital;
      ctx->Map.pcs[ 1 ] < ScoreTqReqSurrenderPriorityInherit_Pre_InheritedPriority_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = ScoreTqReqSurrenderPriorityInherit_Pre_PreviousHelping_Vital;
        ctx->Map.pcs[ 2 ] < ScoreTqReqSurrenderPriorityInherit_Pre_PreviousHelping_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        for (
          ctx->Map.pcs[ 3 ] = ScoreTqReqSurrenderPriorityInherit_Pre_UsedScheduler_Home;
          ctx->Map.pcs[ 3 ] < ScoreTqReqSurrenderPriorityInherit_Pre_UsedScheduler_NA;
          ++ctx->Map.pcs[ 3 ]
        ) {
          for (
            ctx->Map.pcs[ 4 ] = ScoreTqReqSurrenderPriorityInherit_Pre_NewPriority_Vital;
            ctx->Map.pcs[ 4 ] < ScoreTqReqSurrenderPriorityInherit_Pre_NewPriority_NA;
            ++ctx->Map.pcs[ 4 ]
          ) {
            for (
              ctx->Map.pcs[ 5 ] = ScoreTqReqSurrenderPriorityInherit_Pre_NewHelping_Vital;
              ctx->Map.pcs[ 5 ] < ScoreTqReqSurrenderPriorityInherit_Pre_NewHelping_NA;
              ++ctx->Map.pcs[ 5 ]
            ) {
              for (
                ctx->Map.pcs[ 6 ] = ScoreTqReqSurrenderPriorityInherit_Pre_Suspended_Yes;
                ctx->Map.pcs[ 6 ] < ScoreTqReqSurrenderPriorityInherit_Pre_Suspended_NA;
                ++ctx->Map.pcs[ 6 ]
              ) {
                for (
                  ctx->Map.pcs[ 7 ] = ScoreTqReqSurrenderPriorityInherit_Pre_WaitState_Blocked;
                  ctx->Map.pcs[ 7 ] < ScoreTqReqSurrenderPriorityInherit_Pre_WaitState_NA;
                  ++ctx->Map.pcs[ 7 ]
                ) {
                  ctx->Map.entry = ScoreTqReqSurrenderPriorityInherit_PopEntry(
                    ctx
                  );

                  if ( ctx->Map.entry.Skip ) {
                    continue;
                  }

                  ScoreTqReqSurrenderPriorityInherit_Prepare( ctx );
                  ScoreTqReqSurrenderPriorityInherit_TestVariant( ctx );
                  ScoreTqReqSurrenderPriorityInherit_Cleanup( ctx );
                }
              }
            }
          }
        }
      }
    }
  }

  T_add_remark( &ScoreTqReqSurrenderPriorityInherit_Remark );
  T_pop_fixture();
}

/** @} */
