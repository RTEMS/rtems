/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreTqReqEnqueuePriorityInherit
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

#include "tr-tq-enqueue-priority-inherit.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreTqReqEnqueuePriorityInherit \
 *   spec:/score/tq/req/enqueue-priority-inherit
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef struct {
  uint32_t Skip : 1;
  uint32_t Pre_Scheduler_NA : 1;
  uint32_t Pre_QueueEligible_NA : 1;
  uint32_t Pre_QueueIneligible_NA : 1;
  uint32_t Pre_PriorityForOwner_NA : 1;
  uint32_t Pre_SchedulerForOwner_NA : 1;
  uint32_t Pre_OwnerState_NA : 1;
  uint32_t Post_Position : 4;
  uint32_t Post_OwnerPriority : 2;
  uint32_t Post_OwnerScheduler : 2;
  uint32_t Post_OwnerOwnerPriority : 2;
  uint32_t Post_OwnerOwnerScheduler : 2;
} ScoreTqReqEnqueuePriorityInherit_Entry;

/**
 * @brief Test context for spec:/score/tq/req/enqueue-priority-inherit test
 *   case.
 */
typedef struct {
  /**
   * @brief This member specifies the priority of a thread on the thread queue
   *   with an eligible scheduler equal to an eligible scheduler of the
   *   enqueueing thread.
   */
  rtems_task_priority queue_priority;

  /**
   * @brief If this member is true, then a thread those eligible schedulers are
   *   ineligible scheduler to the enqueueing task should be enqueued before a
   *   thread with an eligible scheduler equal to an eligible scheduler of the
   *   enqueueing thread.
   */
  size_t other_before;

  /**
   * @brief If this member is true, then a thread those eligible schedulers are
   *   ineligible scheduler to the enqueueing task should be enqueued after a
   *   thread with an eligible scheduler equal to an eligible scheduler of the
   *   enqueueing thread.
   */
  size_t other_after;

  /**
   * @brief This this member is true, then the priorities of the enqueueing
   *   thread shall be dispensable for the owner of the thread queue.
   */
  bool vital_priority;

  /**
   * @brief This this member is true, then the eligible scheduler of the
   *   enqueueing thread shall be dispensable for the owner of the thread
   *   queue.
   */
  bool vital_scheduler;

  /**
   * @brief This member contains the priorities of the thread queue owner after
   *   the enqueue.
   */
  rtems_task_priority owner_priority[ 4 ];

  /**
   * @brief This member contains the priorities of the owner of the thread
   *   queue on which the thread queue owner is blocked after the enqueue.
   */
  rtems_task_priority owner_owner_priority[ 4 ];

  /**
   * @brief This member specifies which mutex obtain event shall be used to
   *   block the thread queue owner.
   */
  rtems_event_set owner_obtain;

  /**
   * @brief This member specifies which mutex release event shall be used to
   *   unblock the thread queue owner.
   */
  rtems_event_set owner_release;

  /**
   * @brief This member contains a copy of the corresponding
   *   ScoreTqReqEnqueuePriorityInherit_Run() parameter.
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
    ScoreTqReqEnqueuePriorityInherit_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} ScoreTqReqEnqueuePriorityInherit_Context;

static ScoreTqReqEnqueuePriorityInherit_Context
  ScoreTqReqEnqueuePriorityInherit_Instance;

static const char * const ScoreTqReqEnqueuePriorityInherit_PreDesc_Scheduler[] = {
  "One",
  "Two",
  "Three",
  "More",
  "NA"
};

static const char * const ScoreTqReqEnqueuePriorityInherit_PreDesc_QueueEligible[] = {
  "None",
  "High",
  "Equal",
  "Low",
  "NA"
};

static const char * const ScoreTqReqEnqueuePriorityInherit_PreDesc_QueueIneligible[] = {
  "None",
  "Only",
  "Before",
  "After",
  "NA"
};

static const char * const ScoreTqReqEnqueuePriorityInherit_PreDesc_PriorityForOwner[] = {
  "Vital",
  "Dispensable",
  "NA"
};

static const char * const ScoreTqReqEnqueuePriorityInherit_PreDesc_SchedulerForOwner[] = {
  "Vital",
  "Dispensable",
  "NA"
};

static const char * const ScoreTqReqEnqueuePriorityInherit_PreDesc_OwnerState[] = {
  "NotEnqueued",
  "FIFO",
  "Priority",
  "PriorityInherit",
  "NA"
};

static const char * const * const ScoreTqReqEnqueuePriorityInherit_PreDesc[] = {
  ScoreTqReqEnqueuePriorityInherit_PreDesc_Scheduler,
  ScoreTqReqEnqueuePriorityInherit_PreDesc_QueueEligible,
  ScoreTqReqEnqueuePriorityInherit_PreDesc_QueueIneligible,
  ScoreTqReqEnqueuePriorityInherit_PreDesc_PriorityForOwner,
  ScoreTqReqEnqueuePriorityInherit_PreDesc_SchedulerForOwner,
  ScoreTqReqEnqueuePriorityInherit_PreDesc_OwnerState,
  NULL
};

typedef ScoreTqReqEnqueuePriorityInherit_Context Context;

static const rtems_tcb *GetUnblock( Context *ctx, size_t *index )
{
  const rtems_tcb *thread;

  do {
    thread = TQGetNextUnblock( ctx->tq_ctx, index )->thread;
  } while ( thread == ctx->tq_ctx->runner_tcb );

  return thread;
}

static const rtems_tcb *GetTCB( Context *ctx, TQWorkerKind worker )
{
  return ctx->tq_ctx->worker_tcb[ worker ];
}

static void CheckPriorityRaise(
  const Context             *ctx,
  const rtems_task_priority *priority
)
{
  if ( ctx->queue_priority == PRIO_ULTRA_HIGH ) {
    T_eq_u32( priority[ 0 ], PRIO_ULTRA_HIGH );
  } else {
    T_eq_u32( priority[ 0 ], PRIO_VERY_HIGH );
  }

  if (
    ctx->queue_priority == PRIO_VERY_HIGH ||
    ctx->queue_priority == PRIO_ULTRA_HIGH
  ) {
    if ( ctx->other_before || ctx->other_after ) {
      T_eq_u32( priority[ 1 ], PRIO_ULTRA_LOW );
      T_eq_u32( priority[ 2 ], PRIO_NORMAL );
    } else {
      T_eq_u32( priority[ 1 ], PRIO_NORMAL );
    }
  }
}

static void CheckPriorityNop(
  const Context             *ctx,
  const rtems_task_priority *priority
)
{
  if ( ctx->queue_priority == PRIO_ULTRA_HIGH ) {
    T_eq_u32( priority[ 0 ], PRIO_ULTRA_HIGH );
  } else {
    T_eq_u32( priority[ 0 ], PRIO_VERY_HIGH );
  }
}

static void CheckSchedulerNewHelper(
  const Context             *ctx,
  const rtems_task_priority *priority
)
{
  if (
    ctx->vital_priority &&
    ( ctx->queue_priority == PRIO_VERY_HIGH ||
      ctx->queue_priority == PRIO_ULTRA_HIGH )
  ) {
    if ( ctx->other_before || ctx->other_after ) {
      T_eq_u32( priority[ 3 ], PRIO_VERY_LOW );
    } else {
      T_eq_u32( priority[ 2 ], PRIO_VERY_LOW );
      T_eq_u32( priority[ 3 ], PRIO_INVALID );
    }
  } else {
    if ( ctx->other_before || ctx->other_after ) {
      T_eq_u32( priority[ 1 ], PRIO_ULTRA_LOW );
      T_eq_u32( priority[ 2 ], PRIO_VERY_LOW );
    } else {
      T_eq_u32( priority[ 1 ], PRIO_VERY_LOW );
      T_eq_u32( priority[ 2 ], PRIO_INVALID );
    }

     T_eq_u32( priority[ 3 ], PRIO_INVALID );
  }
}

static void CheckSchedulerNop(
  const Context             *ctx,
  const rtems_task_priority *priority
)
{
  if (
    ctx->vital_priority &&
    ( ctx->queue_priority == PRIO_VERY_HIGH ||
      ctx->queue_priority == PRIO_ULTRA_HIGH )
  ) {
    if ( !ctx->other_before && !ctx->other_after ) {
      T_eq_u32( priority[ 2 ], PRIO_INVALID );
    }

    T_eq_u32( priority[ 3 ], PRIO_INVALID );
  } else {
    if ( ctx->other_before || ctx->other_after ) {
      T_eq_u32( priority[ 1 ], PRIO_ULTRA_LOW );
    } else {
      T_eq_u32( priority[ 1 ], PRIO_INVALID );
    }

    T_eq_u32( priority[ 2 ], PRIO_INVALID );
    T_eq_u32( priority[ 3 ], PRIO_INVALID );
  }
}

static void GetPriorities(
  const Context       *ctx,
  TQWorkerKind         worker,
  rtems_task_priority *priority
)
{
  priority[ 0 ] = GetPriorityByScheduler(
    ctx->tq_ctx->worker_id[ worker ],
    SCHEDULER_A_ID
  );
  priority[ 1 ] = GetPriorityByScheduler(
    ctx->tq_ctx->worker_id[ worker ],
    SCHEDULER_B_ID
  );
  priority[ 2 ] = GetPriorityByScheduler(
    ctx->tq_ctx->worker_id[ worker ],
    SCHEDULER_C_ID
  );
  priority[ 3 ] = GetPriorityByScheduler(
    ctx->tq_ctx->worker_id[ worker ],
    SCHEDULER_D_ID
  );
}

static void AddVitalPriority( Context *ctx, rtems_id scheduler_id )
{
  TQSetScheduler( ctx->tq_ctx, TQ_BLOCKER_D, scheduler_id, PRIO_NORMAL );
  TQSendAndWaitForExecutionStop(
    ctx->tq_ctx,
    TQ_BLOCKER_D,
    TQ_EVENT_MUTEX_A_OBTAIN | TQ_EVENT_MUTEX_A_RELEASE
  );
}

static void AddVitalPriorityHelper( Context *ctx, rtems_id scheduler_id )
{
  TQSetScheduler( ctx->tq_ctx, TQ_HELPER_B, scheduler_id, PRIO_LOW );
  TQSendAndWaitForExecutionStop(
    ctx->tq_ctx,
    TQ_HELPER_B,
    TQ_EVENT_MUTEX_B_OBTAIN | TQ_EVENT_MUTEX_B_RELEASE
  );
}

static void AddVitalScheduler( Context *ctx, rtems_id scheduler_id )
{
  TQSetScheduler( ctx->tq_ctx, TQ_BLOCKER_E, scheduler_id, PRIO_VERY_LOW );
  TQSendAndWaitForExecutionStop(
    ctx->tq_ctx,
    TQ_BLOCKER_E,
    TQ_EVENT_MUTEX_A_OBTAIN | TQ_EVENT_MUTEX_A_RELEASE
  );
}

static void ScoreTqReqEnqueuePriorityInherit_Pre_Scheduler_Prepare(
  ScoreTqReqEnqueuePriorityInherit_Context      *ctx,
  ScoreTqReqEnqueuePriorityInherit_Pre_Scheduler state
)
{
  switch ( state ) {
    case ScoreTqReqEnqueuePriorityInherit_Pre_Scheduler_One: {
      /*
       * Where the system has exactly one schedulers.
       */
      if ( rtems_scheduler_get_processor_maximum() != 1 ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTqReqEnqueuePriorityInherit_Pre_Scheduler_Two: {
      /*
       * Where the system has exactly two schedulers.
       */
      if ( rtems_scheduler_get_processor_maximum() != 2 ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTqReqEnqueuePriorityInherit_Pre_Scheduler_Three: {
      /*
       * Where the system has exactly three schedulers.
       */
      if ( rtems_scheduler_get_processor_maximum() != 3 ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTqReqEnqueuePriorityInherit_Pre_Scheduler_More: {
      /*
       * Where the system has at least three schedulers.
       */
      if ( rtems_scheduler_get_processor_maximum() < 4 ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTqReqEnqueuePriorityInherit_Pre_Scheduler_NA:
      break;
  }
}

static void ScoreTqReqEnqueuePriorityInherit_Pre_QueueEligible_Prepare(
  ScoreTqReqEnqueuePriorityInherit_Context          *ctx,
  ScoreTqReqEnqueuePriorityInherit_Pre_QueueEligible state
)
{
  switch ( state ) {
    case ScoreTqReqEnqueuePriorityInherit_Pre_QueueEligible_None: {
      /*
       * While all priority queues of the thread queue associated with eligible
       * schedulers of the enqueueing thread are empty.
       */
      ctx->queue_priority = PRIO_INVALID;
      break;
    }

    case ScoreTqReqEnqueuePriorityInherit_Pre_QueueEligible_High: {
      /*
       * While a priority queue of the thread queue associated with an eligible
       * scheduler of the enqueueing thread is non-empty, while the highest
       * priority of the priority queue is higher than the priority of the
       * enqueueing thread with respect to the eligible scheduler.
       */
      ++ctx->tq_ctx->how_many;
      ctx->queue_priority = PRIO_ULTRA_HIGH;
      break;
    }

    case ScoreTqReqEnqueuePriorityInherit_Pre_QueueEligible_Equal: {
      /*
       * While a priority queue of the thread queue associated with an eligible
       * scheduler of the enqueueing thread is non-empty, while the highest
       * priority of the priority queue is equal to the priority of the
       * enqueueing thread with respect to the eligible scheduler.
       */
      ++ctx->tq_ctx->how_many;
      ctx->queue_priority = PRIO_VERY_HIGH;
      break;
    }

    case ScoreTqReqEnqueuePriorityInherit_Pre_QueueEligible_Low: {
      /*
       * While a priority queue of the thread queue associated with an eligible
       * scheduler of the enqueueing thread is non-empty, while the highest
       * priority of the priority queue is lower than the priority of the
       * enqueueing thread with respect to the eligible scheduler.
       */
      ++ctx->tq_ctx->how_many;
      ctx->queue_priority = PRIO_HIGH;
      break;
    }

    case ScoreTqReqEnqueuePriorityInherit_Pre_QueueEligible_NA:
      break;
  }
}

static void ScoreTqReqEnqueuePriorityInherit_Pre_QueueIneligible_Prepare(
  ScoreTqReqEnqueuePriorityInherit_Context            *ctx,
  ScoreTqReqEnqueuePriorityInherit_Pre_QueueIneligible state
)
{
  switch ( state ) {
    case ScoreTqReqEnqueuePriorityInherit_Pre_QueueIneligible_None: {
      /*
       * While each priority queue of the thread queue is associated with an
       * eligible scheduler of the enqueueing thread.
       */
      ctx->other_before = false;
      ctx->other_after = false;
      break;
    }

    case ScoreTqReqEnqueuePriorityInherit_Pre_QueueIneligible_Only: {
      /*
       * While exactly one priority queue of the thread queue exists which is
       * not associated with an eligible scheduler of the enqueueing thread.
       */
      ++ctx->tq_ctx->how_many;
      ctx->other_before = true;
      ctx->other_after = false;
      break;
    }

    case ScoreTqReqEnqueuePriorityInherit_Pre_QueueIneligible_Before: {
      /*
       * While a priority queue of the thread queue exists which is not
       * associated with an eligible scheduler of the enqueueing thread, while
       * the priority queue is positioned before all priority queues which are
       * associated with eligible schedulers of the enqueueing thread.
       */
      ++ctx->tq_ctx->how_many;
      ctx->other_before = true;
      ctx->other_after = false;
      break;
    }

    case ScoreTqReqEnqueuePriorityInherit_Pre_QueueIneligible_After: {
      /*
       * While a priority queue of the thread queue exists which is not
       * associated with an eligible scheduler of the enqueueing thread, while
       * the priority queue is positioned after all priority queues which are
       * associated with eligible schedulers of the enqueueing thread.
       */
      ++ctx->tq_ctx->how_many;
      ctx->other_before = false;
      ctx->other_after = true;
      break;
    }

    case ScoreTqReqEnqueuePriorityInherit_Pre_QueueIneligible_NA:
      break;
  }
}

static void ScoreTqReqEnqueuePriorityInherit_Pre_PriorityForOwner_Prepare(
  ScoreTqReqEnqueuePriorityInherit_Context             *ctx,
  ScoreTqReqEnqueuePriorityInherit_Pre_PriorityForOwner state
)
{
  switch ( state ) {
    case ScoreTqReqEnqueuePriorityInherit_Pre_PriorityForOwner_Vital: {
      /*
       * While at least one priority of the enqueueing thread is higher than
       * the highest priority of the owner of the thread queue.
       */
      ctx->vital_priority = true;
      break;
    }

    case ScoreTqReqEnqueuePriorityInherit_Pre_PriorityForOwner_Dispensable: {
      /*
       * While all priorities of the enqueueing thread are lower than or equal
       * to the highest priority of the owner of the thread queue.
       */
      ctx->vital_priority = false;
      break;
    }

    case ScoreTqReqEnqueuePriorityInherit_Pre_PriorityForOwner_NA:
      break;
  }
}

static void ScoreTqReqEnqueuePriorityInherit_Pre_SchedulerForOwner_Prepare(
  ScoreTqReqEnqueuePriorityInherit_Context              *ctx,
  ScoreTqReqEnqueuePriorityInherit_Pre_SchedulerForOwner state
)
{
  switch ( state ) {
    case ScoreTqReqEnqueuePriorityInherit_Pre_SchedulerForOwner_Vital: {
      /*
       * While at least one eligible scheduler of the enqueueing thread is not
       * an eligible scheduler of the owner of the thread queue.
       */
      ctx->vital_scheduler = true;
      break;
    }

    case ScoreTqReqEnqueuePriorityInherit_Pre_SchedulerForOwner_Dispensable: {
      /*
       * While all eligible schedulers of the enqueueing thread are an eligible
       * scheduler of the owner of the thread queue.
       */
      ctx->vital_scheduler = false;
      break;
    }

    case ScoreTqReqEnqueuePriorityInherit_Pre_SchedulerForOwner_NA:
      break;
  }
}

static void ScoreTqReqEnqueuePriorityInherit_Pre_OwnerState_Prepare(
  ScoreTqReqEnqueuePriorityInherit_Context       *ctx,
  ScoreTqReqEnqueuePriorityInherit_Pre_OwnerState state
)
{
  switch ( state ) {
    case ScoreTqReqEnqueuePriorityInherit_Pre_OwnerState_NotEnqueued: {
      /*
       * While the owner of the thread queue is not enqueued on a thread queue.
       */
      ctx->owner_obtain = 0;
      ctx->owner_release = 0;
      break;
    }

    case ScoreTqReqEnqueuePriorityInherit_Pre_OwnerState_FIFO: {
      /*
       * While the owner of the thread queue is enqueued on a thread queue in
       * FIFO order.
       */
      ctx->owner_obtain = TQ_EVENT_MUTEX_FIFO_OBTAIN;
      ctx->owner_release = TQ_EVENT_MUTEX_FIFO_RELEASE;
      break;
    }

    case ScoreTqReqEnqueuePriorityInherit_Pre_OwnerState_Priority: {
      /*
       * While the owner of the thread queue is enqueued on a thread queue in
       * priority order.
       */
      ctx->owner_obtain = TQ_EVENT_MUTEX_NO_PROTOCOL_OBTAIN;
      ctx->owner_release = TQ_EVENT_MUTEX_NO_PROTOCOL_RELEASE;
      break;
    }

    case ScoreTqReqEnqueuePriorityInherit_Pre_OwnerState_PriorityInherit: {
      /*
       * While the owner of the thread queue is enqueued on a thread queue in
       * priority order with priority inheritance.
       */
      ctx->owner_obtain = TQ_EVENT_MUTEX_C_OBTAIN;
      ctx->owner_release = TQ_EVENT_MUTEX_C_RELEASE;
      break;
    }

    case ScoreTqReqEnqueuePriorityInherit_Pre_OwnerState_NA:
      break;
  }
}

static void ScoreTqReqEnqueuePriorityInherit_Post_Position_Check(
  ScoreTqReqEnqueuePriorityInherit_Context      *ctx,
  ScoreTqReqEnqueuePriorityInherit_Post_Position state
)
{
  size_t i;

  i = 0;

  /* Event receive */
  T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_HELPER_A ) );

  switch ( state ) {
    case ScoreTqReqEnqueuePriorityInherit_Post_Position_InitialFirst: {
      /*
       * A priority queue associated with the scheduler which contains exactly
       * the enqueueing thread shall be created as the first priority queue of
       * the thread queue.
       */
      T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_A ) );
      T_eq_ptr( GetUnblock( ctx, &i ), NULL );
      break;
    }

    case ScoreTqReqEnqueuePriorityInherit_Post_Position_InitialLast: {
      /*
       * A priority queue associated with the scheduler which contains exactly
       * the enqueueing thread shall be created as the last priority queue of
       * the thread queue.
       */
      T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_C ) );
      T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_A ) );
      T_eq_ptr( GetUnblock( ctx, &i ), NULL );
      break;
    }

    case ScoreTqReqEnqueuePriorityInherit_Post_Position_First: {
      /*
       * The enqueueing thread shall be enqueued in the priority queue
       * associated with the scheduler.
       */
      T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_A ) );
      T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_B ) );
      T_eq_ptr( GetUnblock( ctx, &i ), NULL );
      break;
    }

    case ScoreTqReqEnqueuePriorityInherit_Post_Position_Second: {
      /*
       * The enqueueing thread shall be enqueued in the priority queue
       * associated with the scheduler.
       */
      T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_B ) );
      T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_A ) );
      T_eq_ptr( GetUnblock( ctx, &i ), NULL );
      break;
    }

    case ScoreTqReqEnqueuePriorityInherit_Post_Position_FirstFirst: {
      /*
       * The enqueueing thread shall be enqueued in the priority queue
       * associated with the scheduler.
       *
       * The position of the priority queue in the thread queue shall not
       * change.
       */
      T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_A ) );
      T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_C ) );
      T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_B ) );
      T_eq_ptr( GetUnblock( ctx, &i ), NULL );
      break;
    }

    case ScoreTqReqEnqueuePriorityInherit_Post_Position_SecondFirst: {
      /*
       * The enqueueing thread shall be enqueued in the priority queue
       * associated with the scheduler.
       *
       * The position of the priority queue in the thread queue shall not
       * change.
       */
      T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_B ) );
      T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_C ) );
      T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_A ) );
      T_eq_ptr( GetUnblock( ctx, &i ), NULL );
      break;
    }

    case ScoreTqReqEnqueuePriorityInherit_Post_Position_FirstLast: {
      /*
       * The enqueueing thread shall be enqueued in the priority queue
       * associated with the scheduler.
       *
       * The position of the priority queue in the thread queue shall not
       * change.
       */
      T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_C ) );
      T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_A ) );
      T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_B ) );
      T_eq_ptr( GetUnblock( ctx, &i ), NULL );
      break;
    }

    case ScoreTqReqEnqueuePriorityInherit_Post_Position_SecondLast: {
      /*
       * The enqueueing thread shall be enqueued in the priority queue
       * associated with the scheduler.
       *
       * The position of the priority queue in the thread queue shall not
       * change.
       */
      T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_C ) );
      T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_B ) );
      T_eq_ptr( GetUnblock( ctx, &i ), GetTCB( ctx, TQ_BLOCKER_A ) );
      T_eq_ptr( GetUnblock( ctx, &i ), NULL );
      break;
    }

    case ScoreTqReqEnqueuePriorityInherit_Post_Position_NA:
      break;
  }
}

static void ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Check(
  ScoreTqReqEnqueuePriorityInherit_Context           *ctx,
  ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority state
)
{
  switch ( state ) {
    case ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Raise: {
      /*
       * Each priority of the enqueueing thread which is higher than the
       * highest priority of the owner of the thread queue shall be made the
       * highest priority of the owner.
       */
      CheckPriorityRaise( ctx, ctx->owner_priority );
      break;
    }

    case ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Nop: {
      /*
       * The priorities of the owner of the thread queue shall not change.
       */
      CheckPriorityNop( ctx, ctx->owner_priority );
      break;
    }

    case ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_NA:
      break;
  }
}

static void ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_Check(
  ScoreTqReqEnqueuePriorityInherit_Context            *ctx,
  ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler state
)
{
  switch ( state ) {
    case ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NewHelper: {
      /*
       * Each eligible scheduler of the enqueueing thread which is not an
       * eligible scheduler of the owner of the thread queue shall be made a
       * helping scheduler of the owner with the priority of the enqueueing
       * thread.
       */
      CheckSchedulerNewHelper( ctx, ctx->owner_priority );
      break;
    }

    case ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_Nop: {
      /*
       * The set of eligible schedulers of the owner of the thread queue shall
       * not change.
       */
      CheckSchedulerNop( ctx, ctx->owner_priority );
      break;
    }

    case ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NA:
      break;
  }
}

static void ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Check(
  ScoreTqReqEnqueuePriorityInherit_Context                *ctx,
  ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority state
)
{
  switch ( state ) {
    case ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Raise: {
      /*
       * Each priority of the enqueueing thread which is higher than the
       * highest priority of the owner of the thread queue on which the owner
       * of the thread queue is enqueued shall be made the highest priority of
       * the owner.
       */
      CheckPriorityRaise( ctx, ctx->owner_owner_priority );
      break;
    }

    case ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Nop: {
      /*
       * The priorities of the owner of the thread queue on which the owner of
       * the thread queue is enqueued shall not change.
       */
      if ( ctx->owner_obtain == TQ_EVENT_MUTEX_C_OBTAIN ) {
        CheckPriorityNop( ctx, ctx->owner_owner_priority );
      } else {
        if ( ctx->vital_priority ) {
          T_eq_u32( ctx->owner_owner_priority[ 0 ], PRIO_HIGH );
        } else {
          T_eq_u32( ctx->owner_owner_priority[ 0 ], PRIO_VERY_HIGH );
        }
      }
      break;
    }

    case ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_NA:
      break;
  }
}

static void ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Check(
  ScoreTqReqEnqueuePriorityInherit_Context                 *ctx,
  ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler state
)
{
  switch ( state ) {
    case ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_NewHelper: {
      /*
       * Each eligible scheduler of the enqueueing thread which is not an
       * eligible scheduler of the owner of the thread queue on which the owner
       * of the thread queue is enqueued shall be made a helping scheduler of
       * the owner with the priority of the enqueueing thread.
       */
      CheckSchedulerNewHelper( ctx, ctx->owner_owner_priority );
      break;
    }

    case ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Nop: {
      /*
       * The set of eligible schedulers of the owner of the thread queue on
       * which the owner of the thread queue is enqueued shall not change.
       */
      if ( ctx->owner_obtain == TQ_EVENT_MUTEX_C_OBTAIN ) {
        CheckSchedulerNop( ctx, ctx->owner_owner_priority );
      } else {
        T_eq_u32( ctx->owner_owner_priority[ 1 ], PRIO_INVALID );
        T_eq_u32( ctx->owner_owner_priority[ 2 ], PRIO_INVALID );
        T_eq_u32( ctx->owner_owner_priority[ 3 ], PRIO_INVALID );
      }
      break;
    }

    case ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_NA:
      break;
  }
}

static void ScoreTqReqEnqueuePriorityInherit_Setup(
  ScoreTqReqEnqueuePriorityInherit_Context *ctx
)
{
  TQReset( ctx->tq_ctx );
  TQSetPriority( ctx->tq_ctx, TQ_BLOCKER_A, PRIO_VERY_HIGH );
  TQSetPriority( ctx->tq_ctx, TQ_HELPER_C, PRIO_HIGH );

  #if defined( RTEMS_SMP )
  TQSetScheduler(
    ctx->tq_ctx,
    TQ_BLOCKER_C,
    SCHEDULER_B_ID,
    PRIO_ULTRA_LOW
  );

  if ( rtems_scheduler_get_processor_maximum() > 3 ) {
    RemoveProcessor( SCHEDULER_C_ID, 3 );
    AddProcessor( SCHEDULER_D_ID, 3 );
  }
  #endif
}

static void ScoreTqReqEnqueuePriorityInherit_Setup_Wrap( void *arg )
{
  ScoreTqReqEnqueuePriorityInherit_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  ScoreTqReqEnqueuePriorityInherit_Setup( ctx );
}

static void ScoreTqReqEnqueuePriorityInherit_Teardown(
  ScoreTqReqEnqueuePriorityInherit_Context *ctx
)
{
  TQReset( ctx->tq_ctx );

  #if defined( RTEMS_SMP )
  if ( rtems_scheduler_get_processor_maximum() > 3 ) {
    RemoveProcessor( SCHEDULER_D_ID, 3 );
    AddProcessor( SCHEDULER_C_ID, 3 );
  }
  #endif
}

static void ScoreTqReqEnqueuePriorityInherit_Teardown_Wrap( void *arg )
{
  ScoreTqReqEnqueuePriorityInherit_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  ScoreTqReqEnqueuePriorityInherit_Teardown( ctx );
}

static void ScoreTqReqEnqueuePriorityInherit_Prepare(
  ScoreTqReqEnqueuePriorityInherit_Context *ctx
)
{
  ctx->tq_ctx->how_many = 1;
}

static void ScoreTqReqEnqueuePriorityInherit_Action(
  ScoreTqReqEnqueuePriorityInherit_Context *ctx
)
{
  TQSend( ctx->tq_ctx, TQ_BLOCKER_A, TQ_EVENT_MUTEX_A_OBTAIN );
  TQSend(
    ctx->tq_ctx,
    TQ_HELPER_A,
    TQ_EVENT_MUTEX_B_OBTAIN | TQ_EVENT_ENQUEUE_PREPARE
  );

  if ( ctx->owner_obtain != 0 ) {
    TQSend( ctx->tq_ctx, TQ_HELPER_C, ctx->owner_obtain );
  }

  if ( ctx->other_before ) {
    TQSendAndWaitForExecutionStop(
      ctx->tq_ctx,
      TQ_BLOCKER_C,
      TQ_EVENT_ENQUEUE | TQ_EVENT_SURRENDER | TQ_EVENT_RUNNER_SYNC_2
    );
  }

  if ( ctx->queue_priority != PRIO_INVALID ) {
    TQSetPriority( ctx->tq_ctx, TQ_BLOCKER_B , ctx->queue_priority );
    TQSend(
      ctx->tq_ctx,
      TQ_BLOCKER_B,
      TQ_EVENT_ENQUEUE | TQ_EVENT_SURRENDER
    );
  }

  if ( ctx->other_after ) {
    TQSendAndWaitForExecutionStop(
      ctx->tq_ctx,
      TQ_BLOCKER_C,
      TQ_EVENT_ENQUEUE | TQ_EVENT_SURRENDER | TQ_EVENT_RUNNER_SYNC_2
    );
  }

  if ( ctx->vital_priority ) {
    TQSetPriority( ctx->tq_ctx, TQ_HELPER_A, PRIO_HIGH );
    TQSetPriority( ctx->tq_ctx, TQ_HELPER_C, PRIO_HIGH );

    if (
      ctx->queue_priority == PRIO_VERY_HIGH ||
      ctx->queue_priority == PRIO_ULTRA_HIGH
    ) {
      if ( ctx->other_before || ctx->other_after ) {
        AddVitalPriority( ctx, SCHEDULER_C_ID );
        AddVitalPriorityHelper( ctx, SCHEDULER_C_ID );

        if ( ctx->vital_scheduler ) {
          AddVitalScheduler( ctx, SCHEDULER_D_ID );
        }
      } else {
        AddVitalPriority( ctx, SCHEDULER_B_ID );
        AddVitalPriorityHelper( ctx, SCHEDULER_B_ID );

        if ( ctx->vital_scheduler ) {
          AddVitalScheduler( ctx, SCHEDULER_C_ID );
        }
      }
    } else {
      if ( ctx->vital_scheduler ) {
        if ( ctx->other_before || ctx->other_after ) {
          AddVitalScheduler( ctx, SCHEDULER_C_ID );
        } else {
          AddVitalScheduler( ctx, SCHEDULER_B_ID );
        }
      }
    }
  } else {
    TQSetPriority( ctx->tq_ctx, TQ_HELPER_A, PRIO_VERY_HIGH );
    TQSetPriority( ctx->tq_ctx, TQ_HELPER_C, PRIO_VERY_HIGH );

    if ( ctx->vital_scheduler ) {
      if ( ctx->other_before || ctx->other_after ) {
        AddVitalScheduler( ctx, SCHEDULER_C_ID );
      } else {
        AddVitalScheduler( ctx, SCHEDULER_B_ID );
      }
    }
  }

  if ( ctx->owner_obtain != 0 ) {
    TQClearDone( ctx->tq_ctx, TQ_HELPER_A );
    TQSendAndWaitForExecutionStop(
      ctx->tq_ctx,
      TQ_HELPER_A,
      ctx->owner_obtain | ctx->owner_release
    );
  }

  TQSend(
    ctx->tq_ctx,
    TQ_BLOCKER_A,
    TQ_EVENT_ENQUEUE | TQ_EVENT_SURRENDER | TQ_EVENT_RUNNER_SYNC
  );

  GetPriorities( ctx, TQ_HELPER_A, ctx->owner_priority );
  GetPriorities( ctx, TQ_HELPER_C, ctx->owner_owner_priority );

  if ( ctx->owner_obtain != 0 ) {
    TQSend( ctx->tq_ctx, TQ_HELPER_C, ctx->owner_release );
    TQWaitForDone( ctx->tq_ctx, TQ_HELPER_A );
    TQWaitForExecutionStop( ctx->tq_ctx, TQ_HELPER_A );
  }

  TQSchedulerRecordStart( ctx->tq_ctx );
  TQSend( ctx->tq_ctx, TQ_HELPER_A, TQ_EVENT_ENQUEUE_DONE );

  if ( ctx->other_before || ctx->other_after ) {
    TQSynchronizeRunner2();
  } else {
    TQSynchronizeRunner();
  }

  TQSchedulerRecordStop( ctx->tq_ctx );

  TQSend( ctx->tq_ctx, TQ_BLOCKER_A, TQ_EVENT_MUTEX_A_RELEASE );
  TQMutexObtain( ctx->tq_ctx, TQ_MUTEX_A );
  TQMutexRelease( ctx->tq_ctx, TQ_MUTEX_A );

  TQSend( ctx->tq_ctx, TQ_HELPER_A, TQ_EVENT_MUTEX_B_RELEASE );
  TQMutexObtain( ctx->tq_ctx, TQ_MUTEX_B );
  TQMutexRelease( ctx->tq_ctx, TQ_MUTEX_B );
}

static const ScoreTqReqEnqueuePriorityInherit_Entry
ScoreTqReqEnqueuePriorityInherit_Entries[] = {
  { 1, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriorityInherit_Post_Position_NA,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_NA,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NA,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_NA,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_NA },
  { 1, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriorityInherit_Post_Position_NA,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_NA,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NA,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_NA,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_NA },
  { 1, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriorityInherit_Post_Position_NA,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_NA,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NA,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_NA,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_NA },
  { 1, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriorityInherit_Post_Position_NA,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_NA,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NA,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_NA,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_NA },
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriorityInherit_Post_Position_Second,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Nop },
  { 1, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriorityInherit_Post_Position_NA,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_NA,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NA,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_NA,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_NA },
  { 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqEnqueuePriorityInherit_Post_Position_SecondLast,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Nop },
  { 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqEnqueuePriorityInherit_Post_Position_SecondFirst,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Nop },
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriorityInherit_Post_Position_Second,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Nop },
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriorityInherit_Post_Position_Second,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NewHelper,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Nop },
  { 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqEnqueuePriorityInherit_Post_Position_InitialFirst,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Nop },
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriorityInherit_Post_Position_First,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Nop },
  { 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqEnqueuePriorityInherit_Post_Position_InitialFirst,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Nop },
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriorityInherit_Post_Position_First,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Nop },
  { 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqEnqueuePriorityInherit_Post_Position_InitialLast,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Nop },
  { 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqEnqueuePriorityInherit_Post_Position_FirstLast,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Nop },
  { 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqEnqueuePriorityInherit_Post_Position_FirstFirst,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Nop },
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriorityInherit_Post_Position_Second,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NewHelper,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Nop },
  { 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqEnqueuePriorityInherit_Post_Position_SecondLast,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Nop },
  { 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqEnqueuePriorityInherit_Post_Position_SecondLast,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NewHelper,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Nop },
  { 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqEnqueuePriorityInherit_Post_Position_SecondFirst,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Nop },
  { 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqEnqueuePriorityInherit_Post_Position_SecondFirst,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NewHelper,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Nop },
  { 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqEnqueuePriorityInherit_Post_Position_InitialFirst,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NewHelper,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Nop },
  { 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqEnqueuePriorityInherit_Post_Position_InitialFirst,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NewHelper,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Nop },
  { 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqEnqueuePriorityInherit_Post_Position_InitialLast,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Nop },
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriorityInherit_Post_Position_First,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NewHelper,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Nop },
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriorityInherit_Post_Position_First,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NewHelper,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Nop },
  { 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqEnqueuePriorityInherit_Post_Position_FirstLast,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Nop },
  { 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqEnqueuePriorityInherit_Post_Position_FirstFirst,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Nop },
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriorityInherit_Post_Position_Second,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Nop },
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriorityInherit_Post_Position_Second,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NewHelper,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_NewHelper },
  { 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqEnqueuePriorityInherit_Post_Position_InitialLast,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NewHelper,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Nop },
  { 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqEnqueuePriorityInherit_Post_Position_InitialLast,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NewHelper,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Nop },
  { 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqEnqueuePriorityInherit_Post_Position_FirstLast,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NewHelper,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Nop },
  { 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqEnqueuePriorityInherit_Post_Position_FirstLast,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NewHelper,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Nop },
  { 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqEnqueuePriorityInherit_Post_Position_FirstFirst,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NewHelper,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Nop },
  { 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqEnqueuePriorityInherit_Post_Position_FirstFirst,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NewHelper,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Nop },
  { 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqEnqueuePriorityInherit_Post_Position_SecondLast,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NewHelper,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Nop },
  { 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqEnqueuePriorityInherit_Post_Position_SecondFirst,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NewHelper,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Nop },
  { 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqEnqueuePriorityInherit_Post_Position_InitialFirst,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Nop },
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriorityInherit_Post_Position_First,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Nop },
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriorityInherit_Post_Position_Second,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NewHelper,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_NewHelper },
  { 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqEnqueuePriorityInherit_Post_Position_SecondLast,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Nop },
  { 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqEnqueuePriorityInherit_Post_Position_SecondLast,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NewHelper,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_NewHelper },
  { 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqEnqueuePriorityInherit_Post_Position_SecondFirst,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Nop },
  { 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqEnqueuePriorityInherit_Post_Position_SecondFirst,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NewHelper,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_NewHelper },
  { 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqEnqueuePriorityInherit_Post_Position_InitialFirst,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NewHelper,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_NewHelper },
  { 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqEnqueuePriorityInherit_Post_Position_InitialFirst,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NewHelper,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_NewHelper },
  { 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqEnqueuePriorityInherit_Post_Position_InitialLast,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Nop },
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriorityInherit_Post_Position_First,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NewHelper,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_NewHelper },
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriorityInherit_Post_Position_First,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NewHelper,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_NewHelper },
  { 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqEnqueuePriorityInherit_Post_Position_FirstLast,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Nop },
  { 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqEnqueuePriorityInherit_Post_Position_FirstFirst,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Nop },
  { 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqEnqueuePriorityInherit_Post_Position_InitialLast,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NewHelper,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_NewHelper },
  { 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqEnqueuePriorityInherit_Post_Position_InitialLast,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NewHelper,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_NewHelper },
  { 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqEnqueuePriorityInherit_Post_Position_FirstLast,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NewHelper,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_NewHelper },
  { 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqEnqueuePriorityInherit_Post_Position_FirstLast,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NewHelper,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_NewHelper },
  { 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqEnqueuePriorityInherit_Post_Position_FirstFirst,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NewHelper,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_NewHelper },
  { 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqEnqueuePriorityInherit_Post_Position_FirstFirst,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NewHelper,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_NewHelper },
  { 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqEnqueuePriorityInherit_Post_Position_SecondLast,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NewHelper,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_NewHelper },
  { 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqEnqueuePriorityInherit_Post_Position_SecondFirst,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NewHelper,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_NewHelper }
};

static const uint8_t
ScoreTqReqEnqueuePriorityInherit_Map[] = {
  0, 0, 0, 0, 12, 12, 12, 39, 0, 0, 0, 0, 10, 10, 10, 10, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 0, 0, 0,
  0, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 5, 5, 5, 5, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13, 13, 13, 40, 0, 0, 0,
  0, 11, 11, 11, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 22, 22, 22, 46, 12, 12, 12, 39, 23, 23, 23, 47, 10, 10, 10, 10, 3,
  3, 3, 3, 24, 24, 24, 48, 3, 3, 3, 3, 14, 14, 14, 14, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3,
  3, 3, 8, 8, 8, 29, 9, 9, 9, 30, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 3, 3, 3, 3, 5, 5, 5, 5, 3, 3, 3, 3, 6, 6, 6, 6, 3, 3, 3, 3, 5,
  5, 5, 5, 3, 3, 3, 3, 7, 7, 7, 7, 3, 3, 3, 3, 8, 8, 8, 29, 9, 9, 9, 30, 4, 4,
  4, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 3, 3, 5, 5, 5, 5,
  3, 3, 3, 3, 6, 6, 6, 6, 3, 3, 3, 3, 5, 5, 5, 5, 3, 3, 3, 3, 7, 7, 7, 7, 25,
  25, 25, 49, 13, 13, 13, 40, 26, 26, 26, 50, 11, 11, 11, 11, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 3, 3, 27, 27, 27, 51, 3, 3, 3, 3, 15, 15,
  15, 15, 3, 3, 3, 3, 28, 28, 28, 52, 3, 3, 3, 3, 16, 16, 16, 16, 22, 22, 22,
  46, 12, 12, 12, 39, 23, 23, 23, 47, 10, 10, 10, 10, 31, 31, 31, 53, 24, 24,
  24, 48, 32, 32, 32, 54, 14, 14, 14, 14, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 17, 17, 17, 41,
  8, 8, 8, 29, 9, 9, 9, 30, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 3, 3, 3, 3, 18, 18, 18, 42, 19, 19, 19, 43, 6, 6, 6, 6, 3, 3, 3, 3,
  20, 20, 20, 44, 21, 21, 21, 45, 7, 7, 7, 7, 17, 17, 17, 41, 8, 8, 8, 29, 9,
  9, 9, 30, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3,
  3, 3, 18, 18, 18, 42, 19, 19, 19, 43, 6, 6, 6, 6, 3, 3, 3, 3, 20, 20, 20, 44,
  21, 21, 21, 45, 7, 7, 7, 7, 25, 25, 25, 49, 13, 13, 13, 40, 26, 26, 26, 50,
  11, 11, 11, 11, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 33, 33, 33,
  55, 27, 27, 27, 51, 34, 34, 34, 56, 15, 15, 15, 15, 35, 35, 35, 57, 28, 28,
  28, 52, 36, 36, 36, 58, 16, 16, 16, 16, 22, 22, 22, 46, 12, 12, 12, 39, 23,
  23, 23, 47, 10, 10, 10, 10, 31, 31, 31, 53, 24, 24, 24, 48, 32, 32, 32, 54,
  14, 14, 14, 14, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 17, 17, 17, 41, 8, 8, 8, 29, 9, 9, 9, 30,
  4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 37, 37, 37, 59,
  18, 18, 18, 42, 19, 19, 19, 43, 6, 6, 6, 6, 38, 38, 38, 60, 20, 20, 20, 44,
  21, 21, 21, 45, 7, 7, 7, 7, 17, 17, 17, 41, 8, 8, 8, 29, 9, 9, 9, 30, 4, 4,
  4, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 37, 37, 37, 59, 18, 18,
  18, 42, 19, 19, 19, 43, 6, 6, 6, 6, 38, 38, 38, 60, 20, 20, 20, 44, 21, 21,
  21, 45, 7, 7, 7, 7, 25, 25, 25, 49, 13, 13, 13, 40, 26, 26, 26, 50, 11, 11,
  11, 11, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 33, 33, 33, 55, 27,
  27, 27, 51, 34, 34, 34, 56, 15, 15, 15, 15, 35, 35, 35, 57, 28, 28, 28, 52,
  36, 36, 36, 58, 16, 16, 16, 16
};

static size_t ScoreTqReqEnqueuePriorityInherit_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  ScoreTqReqEnqueuePriorityInherit_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      ScoreTqReqEnqueuePriorityInherit_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture ScoreTqReqEnqueuePriorityInherit_Fixture = {
  .setup = ScoreTqReqEnqueuePriorityInherit_Setup_Wrap,
  .stop = NULL,
  .teardown = ScoreTqReqEnqueuePriorityInherit_Teardown_Wrap,
  .scope = ScoreTqReqEnqueuePriorityInherit_Scope,
  .initial_context = &ScoreTqReqEnqueuePriorityInherit_Instance
};

static const uint16_t ScoreTqReqEnqueuePriorityInherit_Weights[] = {
  256, 64, 16, 8, 4, 1
};

static void ScoreTqReqEnqueuePriorityInherit_Skip(
  ScoreTqReqEnqueuePriorityInherit_Context *ctx,
  size_t                                    index
)
{
  switch ( index + 1 ) {
    case 1:
      ctx->Map.pcs[ 1 ] = ScoreTqReqEnqueuePriorityInherit_Pre_QueueEligible_NA - 1;
      /* Fall through */
    case 2:
      ctx->Map.pcs[ 2 ] = ScoreTqReqEnqueuePriorityInherit_Pre_QueueIneligible_NA - 1;
      /* Fall through */
    case 3:
      ctx->Map.pcs[ 3 ] = ScoreTqReqEnqueuePriorityInherit_Pre_PriorityForOwner_NA - 1;
      /* Fall through */
    case 4:
      ctx->Map.pcs[ 4 ] = ScoreTqReqEnqueuePriorityInherit_Pre_SchedulerForOwner_NA - 1;
      /* Fall through */
    case 5:
      ctx->Map.pcs[ 5 ] = ScoreTqReqEnqueuePriorityInherit_Pre_OwnerState_NA - 1;
      break;
  }
}

static inline ScoreTqReqEnqueuePriorityInherit_Entry
ScoreTqReqEnqueuePriorityInherit_PopEntry(
  ScoreTqReqEnqueuePriorityInherit_Context *ctx
)
{
  size_t index;

  if ( ctx->Map.skip ) {
    size_t i;

    ctx->Map.skip = false;
    index = 0;

    for ( i = 0; i < 6; ++i ) {
      index += ScoreTqReqEnqueuePriorityInherit_Weights[ i ] * ctx->Map.pcs[ i ];
    }
  } else {
    index = ctx->Map.index;
  }

  ctx->Map.index = index + 1;

  return ScoreTqReqEnqueuePriorityInherit_Entries[
    ScoreTqReqEnqueuePriorityInherit_Map[ index ]
  ];
}

static void ScoreTqReqEnqueuePriorityInherit_TestVariant(
  ScoreTqReqEnqueuePriorityInherit_Context *ctx
)
{
  ScoreTqReqEnqueuePriorityInherit_Pre_Scheduler_Prepare(
    ctx,
    ctx->Map.pcs[ 0 ]
  );

  if ( ctx->Map.skip ) {
    ScoreTqReqEnqueuePriorityInherit_Skip( ctx, 0 );
    return;
  }

  ScoreTqReqEnqueuePriorityInherit_Pre_QueueEligible_Prepare(
    ctx,
    ctx->Map.pcs[ 1 ]
  );
  ScoreTqReqEnqueuePriorityInherit_Pre_QueueIneligible_Prepare(
    ctx,
    ctx->Map.pcs[ 2 ]
  );
  ScoreTqReqEnqueuePriorityInherit_Pre_PriorityForOwner_Prepare(
    ctx,
    ctx->Map.pcs[ 3 ]
  );
  ScoreTqReqEnqueuePriorityInherit_Pre_SchedulerForOwner_Prepare(
    ctx,
    ctx->Map.pcs[ 4 ]
  );
  ScoreTqReqEnqueuePriorityInherit_Pre_OwnerState_Prepare(
    ctx,
    ctx->Map.pcs[ 5 ]
  );
  ScoreTqReqEnqueuePriorityInherit_Action( ctx );
  ScoreTqReqEnqueuePriorityInherit_Post_Position_Check(
    ctx,
    ctx->Map.entry.Post_Position
  );
  ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Check(
    ctx,
    ctx->Map.entry.Post_OwnerPriority
  );
  ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_Check(
    ctx,
    ctx->Map.entry.Post_OwnerScheduler
  );
  ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Check(
    ctx,
    ctx->Map.entry.Post_OwnerOwnerPriority
  );
  ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Check(
    ctx,
    ctx->Map.entry.Post_OwnerOwnerScheduler
  );
}

static T_fixture_node ScoreTqReqEnqueuePriorityInherit_Node;

static T_remark ScoreTqReqEnqueuePriorityInherit_Remark = {
  .next = NULL,
  .remark = "ScoreTqReqEnqueuePriorityInherit"
};

void ScoreTqReqEnqueuePriorityInherit_Run( TQContext *tq_ctx )
{
  ScoreTqReqEnqueuePriorityInherit_Context *ctx;

  ctx = &ScoreTqReqEnqueuePriorityInherit_Instance;
  ctx->tq_ctx = tq_ctx;

  ctx = T_push_fixture(
    &ScoreTqReqEnqueuePriorityInherit_Node,
    &ScoreTqReqEnqueuePriorityInherit_Fixture
  );
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;
  ctx->Map.skip = false;

  for (
    ctx->Map.pcs[ 0 ] = ScoreTqReqEnqueuePriorityInherit_Pre_Scheduler_One;
    ctx->Map.pcs[ 0 ] < ScoreTqReqEnqueuePriorityInherit_Pre_Scheduler_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = ScoreTqReqEnqueuePriorityInherit_Pre_QueueEligible_None;
      ctx->Map.pcs[ 1 ] < ScoreTqReqEnqueuePriorityInherit_Pre_QueueEligible_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = ScoreTqReqEnqueuePriorityInherit_Pre_QueueIneligible_None;
        ctx->Map.pcs[ 2 ] < ScoreTqReqEnqueuePriorityInherit_Pre_QueueIneligible_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        for (
          ctx->Map.pcs[ 3 ] = ScoreTqReqEnqueuePriorityInherit_Pre_PriorityForOwner_Vital;
          ctx->Map.pcs[ 3 ] < ScoreTqReqEnqueuePriorityInherit_Pre_PriorityForOwner_NA;
          ++ctx->Map.pcs[ 3 ]
        ) {
          for (
            ctx->Map.pcs[ 4 ] = ScoreTqReqEnqueuePriorityInherit_Pre_SchedulerForOwner_Vital;
            ctx->Map.pcs[ 4 ] < ScoreTqReqEnqueuePriorityInherit_Pre_SchedulerForOwner_NA;
            ++ctx->Map.pcs[ 4 ]
          ) {
            for (
              ctx->Map.pcs[ 5 ] = ScoreTqReqEnqueuePriorityInherit_Pre_OwnerState_NotEnqueued;
              ctx->Map.pcs[ 5 ] < ScoreTqReqEnqueuePriorityInherit_Pre_OwnerState_NA;
              ++ctx->Map.pcs[ 5 ]
            ) {
              ctx->Map.entry = ScoreTqReqEnqueuePriorityInherit_PopEntry(
                ctx
              );

              if ( ctx->Map.entry.Skip ) {
                continue;
              }

              ScoreTqReqEnqueuePriorityInherit_Prepare( ctx );
              ScoreTqReqEnqueuePriorityInherit_TestVariant( ctx );
            }
          }
        }
      }
    }
  }

  T_add_remark( &ScoreTqReqEnqueuePriorityInherit_Remark );
  T_pop_fixture();
}

/** @} */
