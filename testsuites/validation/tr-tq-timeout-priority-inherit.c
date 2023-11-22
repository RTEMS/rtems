/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreTqReqTimeoutPriorityInherit
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

#include "tr-tq-timeout-priority-inherit.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreTqReqTimeoutPriorityInherit \
 *   spec:/score/tq/req/timeout-priority-inherit
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef struct {
  uint32_t Skip : 1;
  uint32_t Pre_HomeScheduler_NA : 1;
  uint32_t Pre_EligibleScheduler_NA : 1;
  uint32_t Pre_Queue_NA : 1;
  uint32_t Pre_OwnerPriority_NA : 1;
  uint32_t Pre_OwnerState_NA : 1;
  uint32_t Pre_OwnerQueue_NA : 1;
  uint32_t Pre_OwnerOwnerPriority_NA : 1;
  uint32_t Pre_WaitState_NA : 1;
  uint32_t Post_Status : 2;
  uint32_t Post_Unblock : 2;
  uint32_t Post_OwnerPriority : 2;
  uint32_t Post_OwnerOwnerPriority : 2;
} ScoreTqReqTimeoutPriorityInherit_Entry;

/**
 * @brief Test context for spec:/score/tq/req/timeout-priority-inherit test
 *   case.
 */
typedef struct {
  /**
   * @brief This member contains the call within ISR request.
   */
  CallWithinISRRequest request;

  /**
   * @brief This member specifies the scheduler of the thread.
   */
  rtems_id scheduler_id;

  /**
   * @brief If this member is true, then the thread shall have at least two
   *   eligible scheduler.
   */
  bool other_scheduler;

  /**
   * @brief This member specifies the queue node kind.
   */
  TQNodeKind queue_node;

  /**
   * @brief This member specifies the owner priority node kind.
   */
  TQNodeKind owner_node;

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
   * @brief This member specifies the owner queue node kind.
   */
  TQNodeKind owner_queue_node;

  /**
   * @brief This member specifies the kind of the priority node of the owner of
   *   the thread queue on which the owner of the thread queue is blocked.
   */
  TQNodeKind owner_owner_node;

  /**
   * @brief This member specifies the wait state.
   */
  TQWaitState wait_state;

  /**
   * @brief This member contains the thread queue priority.
   */
  rtems_task_priority queue_priority;

  /**
   * @brief This member contains the owner priority.
   */
  rtems_task_priority owner_priority;

  /**
   * @brief This member contains the owner priority after the timeout or
   *   surrender.
   */
  rtems_task_priority owner_priority_after;

  /**
   * @brief This member contains the priority of the thread queue on which the
   *   owner is enqueued.
   */
  rtems_task_priority owner_queue_priority;

  /**
   * @brief This member contains the priority of the owner of the thread queue
   *   on which the owner is enqueued.
   */
  rtems_task_priority owner_owner_priority;

  /**
   * @brief This member contains the priority after the timeout or surrender of
   *   the owner of the thread queue on which the owner is enqueued.
   */
  rtems_task_priority owner_owner_priority_after;

  /**
   * @brief If this member is true, then the queue helper shall surrender the
   *   thread queue.
   */
  bool queue_helper_surrender;

  /**
   * @brief If this member is true, then the owner helper shall release mutex
   *   A.
   */
  bool owner_helper_release;

  /**
   * @brief If this member is true, then the owner queue helper shall release
   *   the mutex on which the owner is blocked.
   */
  bool owner_queue_helper_release;

  /**
   * @brief If this member is true, then helper of the owner of the mutex which
   *   the owner blocked shall release mutex B.
   */
  bool owner_owner_helper_release;

  /**
   * @brief This member contains a copy of the corresponding
   *   ScoreTqReqTimeoutPriorityInherit_Run() parameter.
   */
  TQContext *tq_ctx;

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
    ScoreTqReqTimeoutPriorityInherit_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} ScoreTqReqTimeoutPriorityInherit_Context;

static ScoreTqReqTimeoutPriorityInherit_Context
  ScoreTqReqTimeoutPriorityInherit_Instance;

static const char * const ScoreTqReqTimeoutPriorityInherit_PreDesc_HomeScheduler[] = {
  "Home",
  "Helping",
  "NA"
};

static const char * const ScoreTqReqTimeoutPriorityInherit_PreDesc_EligibleScheduler[] = {
  "One",
  "More",
  "NA"
};

static const char * const ScoreTqReqTimeoutPriorityInherit_PreDesc_Queue[] = {
  "Only",
  "Vital",
  "Dispensable",
  "NA"
};

static const char * const ScoreTqReqTimeoutPriorityInherit_PreDesc_OwnerPriority[] = {
  "Only",
  "Vital",
  "Dispensable",
  "NA"
};

static const char * const ScoreTqReqTimeoutPriorityInherit_PreDesc_OwnerState[] = {
  "NotEnqueued",
  "FIFO",
  "Priority",
  "PriorityInherit",
  "NA"
};

static const char * const ScoreTqReqTimeoutPriorityInherit_PreDesc_OwnerQueue[] = {
  "Only",
  "Vital",
  "Dispensable",
  "NA"
};

static const char * const ScoreTqReqTimeoutPriorityInherit_PreDesc_OwnerOwnerPriority[] = {
  "Only",
  "Vital",
  "Dispensable",
  "NA"
};

static const char * const ScoreTqReqTimeoutPriorityInherit_PreDesc_WaitState[] = {
  "Blocked",
  "IntendToBlock",
  "ReadyAgain",
  "NA"
};

static const char * const * const ScoreTqReqTimeoutPriorityInherit_PreDesc[] = {
  ScoreTqReqTimeoutPriorityInherit_PreDesc_HomeScheduler,
  ScoreTqReqTimeoutPriorityInherit_PreDesc_EligibleScheduler,
  ScoreTqReqTimeoutPriorityInherit_PreDesc_Queue,
  ScoreTqReqTimeoutPriorityInherit_PreDesc_OwnerPriority,
  ScoreTqReqTimeoutPriorityInherit_PreDesc_OwnerState,
  ScoreTqReqTimeoutPriorityInherit_PreDesc_OwnerQueue,
  ScoreTqReqTimeoutPriorityInherit_PreDesc_OwnerOwnerPriority,
  ScoreTqReqTimeoutPriorityInherit_PreDesc_WaitState,
  NULL
};

typedef ScoreTqReqTimeoutPriorityInherit_Context Context;

#define THREAD TQ_BLOCKER_A

#define THREAD_HELPER_A TQ_HELPER_B

#define THREAD_HELPER_B TQ_HELPER_C

#define QUEUE_HELPER TQ_BLOCKER_B

#define OWNER TQ_BLOCKER_C

#define OWNER_HELPER TQ_BLOCKER_D

#define OWNER_QUEUE_HELPER TQ_BLOCKER_E

#define OWNER_OWNER TQ_WORKER_F

#define OWNER_OWNER_HELPER TQ_HELPER_A

static bool GetUnblock( const Context *ctx, size_t *index )
{
  while ( true ) {
    const T_scheduler_event *event;

    event = TQGetNextUnblock( ctx->tq_ctx, index );

    if ( event == &T_scheduler_event_null ) {
      return false;
    }

    if ( event->thread == ctx->tq_ctx->worker_tcb[ THREAD ] ) {
      return true;
    }
  }
}

static void Tick( void *arg )
{
  Context *ctx;

  ctx = arg;
  TQSchedulerRecordStart( ctx->tq_ctx );
  FinalClockTick();
  TQSchedulerRecordStop( ctx->tq_ctx );
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
    event->operation == T_SCHEDULER_BLOCK &&
    event->thread == ctx->tq_ctx->worker_tcb[ THREAD ]
  ) {
    T_scheduler_set_event_handler( NULL, NULL );
    ctx->request.handler = Tick;
    CallWithinISRSubmit( &ctx->request );
  }
}

static void ThreadTimeout( void *arg )
{
  Context *ctx;

  ctx = arg;
  TQSchedulerRecordStart( ctx->tq_ctx );
  _Thread_Timeout(
    &ctx->tq_ctx->worker_tcb[ THREAD ]->Timer.Watchdog
  );
  TQSchedulerRecordStop( ctx->tq_ctx );
}

static void SchedulerUnblock(
  void                    *arg,
  const T_scheduler_event *event,
  T_scheduler_when         when
)
{
  Context *ctx;

  ctx = arg;

  if (
    when == T_SCHEDULER_BEFORE &&
    event->operation == T_SCHEDULER_UNBLOCK &&
    event->thread == ctx->tq_ctx->worker_tcb[ THREAD ]
  ) {
    T_scheduler_set_event_handler( NULL, NULL );

    if ( ctx->scheduler_id == SCHEDULER_B_ID ) {
#if defined(RTEMS_SMP)
      _SMP_Unicast_action( 1, ThreadTimeout, ctx );
#else
      T_unreachable();
#endif
    } else {
      ctx->request.handler = ThreadTimeout;
      CallWithinISRSubmit( &ctx->request );
    }
  }
}

static void GetPriorities( Context *ctx )
{
  ctx->owner_priority_after = GetPriorityByScheduler(
    ctx->tq_ctx->worker_id[ OWNER ],
    ctx->scheduler_id
  );
  ctx->owner_owner_priority_after = GetPriorityByScheduler(
    ctx->tq_ctx->worker_id[ OWNER_OWNER ],
    ctx->scheduler_id
  );
}

static void PrepareThread( const Context *ctx )
{
  if ( ctx->other_scheduler ) {
    rtems_id other_scheduler_id;

    if ( ctx->scheduler_id == SCHEDULER_A_ID ) {
      other_scheduler_id = SCHEDULER_B_ID;
    } else {
      other_scheduler_id = SCHEDULER_B_ID;
    }

    TQSendAndWaitForExecutionStop(
      ctx->tq_ctx,
      THREAD,
      TQ_EVENT_MUTEX_D_OBTAIN
    );

    TQSetScheduler(
      ctx->tq_ctx,
      THREAD_HELPER_A,
      other_scheduler_id,
      PRIO_NEARLY_IDLE - 1
    );
    TQSendAndWaitForExecutionStop(
      ctx->tq_ctx,
      THREAD_HELPER_A,
      TQ_EVENT_MUTEX_D_OBTAIN
    );

    if ( rtems_scheduler_get_processor_maximum() >= 3 ) {
      TQSetScheduler(
        ctx->tq_ctx,
        THREAD_HELPER_B,
        SCHEDULER_C_ID,
        PRIO_NORMAL
      );
      TQSendAndWaitForExecutionStop(
        ctx->tq_ctx,
        THREAD_HELPER_B,
        TQ_EVENT_MUTEX_D_OBTAIN
      );
    }
  }
}

static rtems_task_priority PrepareQueue(
  Context            *ctx,
  rtems_task_priority priority
)
{
  switch ( ctx->queue_node ) {
    case TQ_NODE_ONLY:
      ctx->queue_helper_surrender = false;
      break;
    case TQ_NODE_VITAL:
      ctx->queue_helper_surrender = true;
      TQSetScheduler(
        ctx->tq_ctx,
        QUEUE_HELPER,
        ctx->scheduler_id,
        priority + 1
      );
      TQSendAndWaitForExecutionStop(
        ctx->tq_ctx,
        QUEUE_HELPER,
        TQ_EVENT_ENQUEUE
      );
      break;
    case TQ_NODE_DISPENSABLE:
      ctx->queue_helper_surrender = true;
      --priority;
      TQSetScheduler(
        ctx->tq_ctx,
        QUEUE_HELPER,
        ctx->scheduler_id,
        priority
      );
      TQSendAndWaitForExecutionStop(
        ctx->tq_ctx,
        QUEUE_HELPER,
        TQ_EVENT_ENQUEUE
      );
      break;
  }

  ctx->queue_priority = priority;

  return priority;
}

static rtems_task_priority PrepareOwner(
  Context            *ctx,
  rtems_task_priority priority
)
{
  switch ( ctx->owner_node ) {
    case TQ_NODE_ONLY:
      ctx->owner_helper_release = false;
      TQSetPriority( ctx->tq_ctx, OWNER, PRIO_FLEXIBLE );
      break;
    case TQ_NODE_VITAL:
      if ( ctx->scheduler_id == SCHEDULER_A_ID ) {
        ctx->owner_helper_release = false;
        TQSetPriority( ctx->tq_ctx, OWNER, priority + 1 );
      } else {
        ctx->owner_helper_release = true;
        TQSetPriority( ctx->tq_ctx, OWNER, PRIO_FLEXIBLE );
        TQSetScheduler(
          ctx->tq_ctx,
          OWNER_HELPER,
          ctx->scheduler_id,
          priority + 1
        );
        TQSendAndWaitForExecutionStop(
          ctx->tq_ctx,
          OWNER_HELPER,
          TQ_EVENT_MUTEX_A_OBTAIN
        );
      }
      break;
    case TQ_NODE_DISPENSABLE:
      --priority;

      if ( ctx->scheduler_id == SCHEDULER_A_ID ) {
        ctx->owner_helper_release = false;
        TQSetPriority( ctx->tq_ctx, OWNER, priority );
      } else {
        ctx->owner_helper_release = true;
        TQSetPriority( ctx->tq_ctx, OWNER, PRIO_FLEXIBLE );
        TQSetScheduler(
          ctx->tq_ctx,
          OWNER_HELPER,
          ctx->scheduler_id,
          priority
        );
        TQSendAndWaitForExecutionStop(
          ctx->tq_ctx,
          OWNER_HELPER,
          TQ_EVENT_MUTEX_A_OBTAIN
        );
      }
      break;
  }

  ctx->owner_priority = priority;

  return priority;
}

static rtems_task_priority PrepareOwnerQueue(
  Context            *ctx,
  rtems_task_priority priority
)
{
  if ( ctx->owner_obtain != 0 ) {
    switch ( ctx->owner_queue_node ) {
      case TQ_NODE_ONLY:
        ctx->owner_queue_helper_release = false;
        break;
      case TQ_NODE_VITAL:
        ctx->owner_queue_helper_release = true;
        TQSetScheduler(
          ctx->tq_ctx,
          OWNER_QUEUE_HELPER,
          ctx->scheduler_id,
          priority + 1
        );
        TQSendAndWaitForExecutionStop(
          ctx->tq_ctx,
          OWNER_QUEUE_HELPER,
          ctx->owner_obtain
        );
        break;
      case TQ_NODE_DISPENSABLE:
        ctx->owner_queue_helper_release = true;
        --priority;
        TQSetScheduler(
          ctx->tq_ctx,
          OWNER_QUEUE_HELPER,
          ctx->scheduler_id,
          priority
        );
        TQSendAndWaitForExecutionStop(
          ctx->tq_ctx,
          OWNER_QUEUE_HELPER,
          ctx->owner_obtain
        );
        break;
    }

    ctx->owner_queue_priority = priority;
  } else {
    ctx->owner_queue_helper_release = false;
    ctx->owner_queue_priority = PRIO_INVALID;
  }

  return priority;
}

static void PrepareOwnerOwner( Context *ctx, rtems_task_priority priority )
{
  if ( ctx->owner_obtain != 0 ) {
    switch ( ctx->owner_owner_node ) {
      case TQ_NODE_ONLY:
        ctx->owner_owner_helper_release = false;
        TQSetPriority( ctx->tq_ctx, OWNER_OWNER, PRIO_FLEXIBLE );
        break;
      case TQ_NODE_VITAL:
        if ( ctx->scheduler_id == SCHEDULER_A_ID ) {
          ctx->owner_owner_helper_release = false;
          TQSetPriority( ctx->tq_ctx, OWNER_OWNER, priority + 1 );
        } else {
          ctx->owner_owner_helper_release = true;
          TQSetPriority( ctx->tq_ctx, OWNER_OWNER, PRIO_FLEXIBLE );
          TQSetScheduler(
            ctx->tq_ctx,
            OWNER_OWNER_HELPER,
            ctx->scheduler_id,
            priority + 1
          );
          TQSendAndWaitForExecutionStop(
            ctx->tq_ctx,
            OWNER_OWNER_HELPER,
            TQ_EVENT_MUTEX_B_OBTAIN
          );
        }
        break;
      case TQ_NODE_DISPENSABLE:
        --priority;

        if ( ctx->scheduler_id == SCHEDULER_A_ID ) {
          ctx->owner_owner_helper_release = false;
          TQSetPriority( ctx->tq_ctx, OWNER_OWNER, priority );
        } else {
          ctx->owner_owner_helper_release = true;
          TQSetPriority( ctx->tq_ctx, OWNER_OWNER, PRIO_FLEXIBLE );
          TQSetScheduler(
            ctx->tq_ctx,
            OWNER_OWNER_HELPER,
            ctx->scheduler_id,
            priority
          );
          TQSendAndWaitForExecutionStop(
            ctx->tq_ctx,
            OWNER_OWNER_HELPER,
            TQ_EVENT_MUTEX_B_OBTAIN
          );
        }
        break;
    }

    ctx->owner_owner_priority = priority;
  } else {
    ctx->owner_owner_helper_release = false;
    ctx->owner_owner_priority = PRIO_INVALID;
  }
}

static void ScoreTqReqTimeoutPriorityInherit_Pre_HomeScheduler_Prepare(
  ScoreTqReqTimeoutPriorityInherit_Context          *ctx,
  ScoreTqReqTimeoutPriorityInherit_Pre_HomeScheduler state
)
{
  switch ( state ) {
    case ScoreTqReqTimeoutPriorityInherit_Pre_HomeScheduler_Home: {
      /*
       * While the home scheduler of the thread is the home scheduler of the
       * thread queue owner.
       */
      ctx->scheduler_id = SCHEDULER_A_ID;
      break;
    }

    case ScoreTqReqTimeoutPriorityInherit_Pre_HomeScheduler_Helping: {
      /*
       * While the home scheduler of the thread is a helping scheduler of the
       * thread queue owner.
       */
      ctx->scheduler_id = SCHEDULER_B_ID;
      break;
    }

    case ScoreTqReqTimeoutPriorityInherit_Pre_HomeScheduler_NA:
      break;
  }
}

static void ScoreTqReqTimeoutPriorityInherit_Pre_EligibleScheduler_Prepare(
  ScoreTqReqTimeoutPriorityInherit_Context              *ctx,
  ScoreTqReqTimeoutPriorityInherit_Pre_EligibleScheduler state
)
{
  switch ( state ) {
    case ScoreTqReqTimeoutPriorityInherit_Pre_EligibleScheduler_One: {
      /*
       * While the thread has exactly one eligible scheduler.
       */
      ctx->other_scheduler = false;
      break;
    }

    case ScoreTqReqTimeoutPriorityInherit_Pre_EligibleScheduler_More: {
      /*
       * While the thread has at least two eligible scheduler.
       */
      ctx->other_scheduler = true;
      break;
    }

    case ScoreTqReqTimeoutPriorityInherit_Pre_EligibleScheduler_NA:
      break;
  }
}

static void ScoreTqReqTimeoutPriorityInherit_Pre_Queue_Prepare(
  ScoreTqReqTimeoutPriorityInherit_Context  *ctx,
  ScoreTqReqTimeoutPriorityInherit_Pre_Queue state
)
{
  switch ( state ) {
    case ScoreTqReqTimeoutPriorityInherit_Pre_Queue_Only: {
      /*
       * While the priority node of the thread is the only priority node in the
       * priority queue associated with the scheduler of the thread queue.
       */
      ctx->queue_node = TQ_NODE_ONLY;
      break;
    }

    case ScoreTqReqTimeoutPriorityInherit_Pre_Queue_Vital: {
      /*
       * While the priority node of the thread is not the only priority node in
       * the priority queue associated with the scheduler of the thread queue,
       * while the priority node of the thread is the highest priority node in
       * the priority queue.
       */
      ctx->queue_node = TQ_NODE_VITAL;
      break;
    }

    case ScoreTqReqTimeoutPriorityInherit_Pre_Queue_Dispensable: {
      /*
       * While the priority node of the thread is not the only priority node in
       * the priority queue associated with the scheduler of the thread queue,
       * while the priority node of the thread is not the highest priority node
       * in the priority queue.
       */
      ctx->queue_node = TQ_NODE_DISPENSABLE;
      break;
    }

    case ScoreTqReqTimeoutPriorityInherit_Pre_Queue_NA:
      break;
  }
}

static void ScoreTqReqTimeoutPriorityInherit_Pre_OwnerPriority_Prepare(
  ScoreTqReqTimeoutPriorityInherit_Context          *ctx,
  ScoreTqReqTimeoutPriorityInherit_Pre_OwnerPriority state
)
{
  switch ( state ) {
    case ScoreTqReqTimeoutPriorityInherit_Pre_OwnerPriority_Only: {
      /*
       * While the priority node of the thread queue is the only priority node
       * associated with the scheduler available to the owner.
       */
      ctx->owner_node = TQ_NODE_ONLY;
      break;
    }

    case ScoreTqReqTimeoutPriorityInherit_Pre_OwnerPriority_Vital: {
      /*
       * While the priority node of the thread queue is not the only priority
       * node associated with the scheduler available to the owner, while the
       * priority node of the thread queue is the highest priority node
       * available to the owner.
       */
      ctx->owner_node = TQ_NODE_VITAL;
      break;
    }

    case ScoreTqReqTimeoutPriorityInherit_Pre_OwnerPriority_Dispensable: {
      /*
       * While the priority node of the thread queue is not the only priority
       * node associated with the scheduler available to the owner, while the
       * priority node of the thread queue is not the highest priority node
       * available to the owner.
       */
      ctx->owner_node = TQ_NODE_DISPENSABLE;
      break;
    }

    case ScoreTqReqTimeoutPriorityInherit_Pre_OwnerPriority_NA:
      break;
  }
}

static void ScoreTqReqTimeoutPriorityInherit_Pre_OwnerState_Prepare(
  ScoreTqReqTimeoutPriorityInherit_Context       *ctx,
  ScoreTqReqTimeoutPriorityInherit_Pre_OwnerState state
)
{
  switch ( state ) {
    case ScoreTqReqTimeoutPriorityInherit_Pre_OwnerState_NotEnqueued: {
      /*
       * While the owner of the thread queue is not enqueued on a thread queue.
       */
      ctx->owner_obtain = 0;
      ctx->owner_release = 0;
      break;
    }

    case ScoreTqReqTimeoutPriorityInherit_Pre_OwnerState_FIFO: {
      /*
       * While the owner of the thread queue is enqueued on a thread queue in
       * FIFO order.
       */
      ctx->owner_obtain = TQ_EVENT_MUTEX_FIFO_OBTAIN;
      ctx->owner_release = TQ_EVENT_MUTEX_FIFO_RELEASE;
      break;
    }

    case ScoreTqReqTimeoutPriorityInherit_Pre_OwnerState_Priority: {
      /*
       * While the owner of the thread queue is enqueued on a thread queue in
       * priority order.
       */
      ctx->owner_obtain = TQ_EVENT_MUTEX_NO_PROTOCOL_OBTAIN;
      ctx->owner_release = TQ_EVENT_MUTEX_NO_PROTOCOL_RELEASE;
      break;
    }

    case ScoreTqReqTimeoutPriorityInherit_Pre_OwnerState_PriorityInherit: {
      /*
       * While the owner of the thread queue is enqueued on a thread queue in
       * priority order with priority inheritance.
       */
      ctx->owner_obtain = TQ_EVENT_MUTEX_C_OBTAIN;
      ctx->owner_release = TQ_EVENT_MUTEX_C_RELEASE;
      break;
    }

    case ScoreTqReqTimeoutPriorityInherit_Pre_OwnerState_NA:
      break;
  }
}

static void ScoreTqReqTimeoutPriorityInherit_Pre_OwnerQueue_Prepare(
  ScoreTqReqTimeoutPriorityInherit_Context       *ctx,
  ScoreTqReqTimeoutPriorityInherit_Pre_OwnerQueue state
)
{
  switch ( state ) {
    case ScoreTqReqTimeoutPriorityInherit_Pre_OwnerQueue_Only: {
      /*
       * While the priority node of the owner is the only priority node in the
       * priority queue associated with the scheduler of the thread queue on
       * which the owner is enqueued.
       */
      ctx->owner_queue_node = TQ_NODE_ONLY;
      break;
    }

    case ScoreTqReqTimeoutPriorityInherit_Pre_OwnerQueue_Vital: {
      /*
       * While the priority node of the owner is not the only priority node in
       * the priority queue associated with the scheduler of the thread queue
       * on which the owner is enqueued, while the priority node of the owner
       * is the highest priority node in the priority queue.
       */
      ctx->owner_queue_node = TQ_NODE_VITAL;
      break;
    }

    case ScoreTqReqTimeoutPriorityInherit_Pre_OwnerQueue_Dispensable: {
      /*
       * While the priority node of the owner is not the only priority node in
       * the priority queue associated with the scheduler of the thread queue
       * on which the owner is enqueued, while the priority node of the owner
       * is not the highest priority node in the priority queue.
       */
      ctx->owner_queue_node = TQ_NODE_DISPENSABLE;
      break;
    }

    case ScoreTqReqTimeoutPriorityInherit_Pre_OwnerQueue_NA:
      break;
  }
}

static void ScoreTqReqTimeoutPriorityInherit_Pre_OwnerOwnerPriority_Prepare(
  ScoreTqReqTimeoutPriorityInherit_Context               *ctx,
  ScoreTqReqTimeoutPriorityInherit_Pre_OwnerOwnerPriority state
)
{
  switch ( state ) {
    case ScoreTqReqTimeoutPriorityInherit_Pre_OwnerOwnerPriority_Only: {
      /*
       * While the priority node of the thread queue on which the owner is
       * enqueued is the only priority node associated with the scheduler
       * available to the owner of the thread queue on which the owner is
       * enqueued.
       */
      ctx->owner_owner_node = TQ_NODE_ONLY;
      break;
    }

    case ScoreTqReqTimeoutPriorityInherit_Pre_OwnerOwnerPriority_Vital: {
      /*
       * While the priority node of the thread queue on which the owner is
       * enqueued is not the only priority node associated with the scheduler
       * available to the owner of the thread queue on which the owner is
       * enqueued, while the priority node of the thread queue on which the
       * owner is enqueued is the highest priority node available to the owner
       * of the thread queue on which the owner is enqueued.
       */
      ctx->owner_owner_node = TQ_NODE_VITAL;
      break;
    }

    case ScoreTqReqTimeoutPriorityInherit_Pre_OwnerOwnerPriority_Dispensable: {
      /*
       * While the priority node of the thread queue on which the owner is
       * enqueued is not the only priority node associated with the scheduler
       * available to the owner of the thread queue on which the owner is
       * enqueued, while the priority node of the thread queue is on which the
       * owner is enqueued not the highest priority node available to the owner
       * of the thread queue on which the owner is enqueued.
       */
      ctx->owner_owner_node = TQ_NODE_DISPENSABLE;
      break;
    }

    case ScoreTqReqTimeoutPriorityInherit_Pre_OwnerOwnerPriority_NA:
      break;
  }
}

static void ScoreTqReqTimeoutPriorityInherit_Pre_WaitState_Prepare(
  ScoreTqReqTimeoutPriorityInherit_Context      *ctx,
  ScoreTqReqTimeoutPriorityInherit_Pre_WaitState state
)
{
  switch ( state ) {
    case ScoreTqReqTimeoutPriorityInherit_Pre_WaitState_Blocked: {
      /*
       * While the thread is in the blocked wait state.
       */
      ctx->wait_state = TQ_WAIT_STATE_BLOCKED;
      break;
    }

    case ScoreTqReqTimeoutPriorityInherit_Pre_WaitState_IntendToBlock: {
      /*
       * While the thread is in the intend to block wait state.
       */
      ctx->wait_state = TQ_WAIT_STATE_INTEND_TO_BLOCK;
      break;
    }

    case ScoreTqReqTimeoutPriorityInherit_Pre_WaitState_ReadyAgain: {
      /*
       * While the thread is in the ready again wait state.
       */
      ctx->wait_state = TQ_WAIT_STATE_READY_AGAIN;
      break;
    }

    case ScoreTqReqTimeoutPriorityInherit_Pre_WaitState_NA:
      break;
  }
}

static void ScoreTqReqTimeoutPriorityInherit_Post_Status_Check(
  ScoreTqReqTimeoutPriorityInherit_Context    *ctx,
  ScoreTqReqTimeoutPriorityInherit_Post_Status state
)
{
  switch ( state ) {
    case ScoreTqReqTimeoutPriorityInherit_Post_Status_Ok: {
      /*
       * The return status of the directive call shall be derived from
       * STATUS_SUCCESSFUL.
       */
      T_eq_int(
        ctx->tq_ctx->status[ THREAD ],
        TQConvertStatus( ctx->tq_ctx, STATUS_SUCCESSFUL )
      );
      break;
    }

    case ScoreTqReqTimeoutPriorityInherit_Post_Status_Timeout: {
      /*
       * The return status of the directive call shall be derived from
       * STATUS_TIMEOUT.
       */
      T_eq_int(
        ctx->tq_ctx->status[ THREAD ],
        TQConvertStatus( ctx->tq_ctx, STATUS_TIMEOUT )
      );
      break;
    }

    case ScoreTqReqTimeoutPriorityInherit_Post_Status_NA:
      break;
  }
}

static void ScoreTqReqTimeoutPriorityInherit_Post_Unblock_Check(
  ScoreTqReqTimeoutPriorityInherit_Context     *ctx,
  ScoreTqReqTimeoutPriorityInherit_Post_Unblock state
)
{
  size_t i;

  i = 0;

  switch ( state ) {
    case ScoreTqReqTimeoutPriorityInherit_Post_Unblock_Yes: {
      /*
       * The thread shall be unblocked by the timeout operation.
       */
      T_true( GetUnblock( ctx, &i ) );
      T_false( GetUnblock( ctx, &i ) );
      break;
    }

    case ScoreTqReqTimeoutPriorityInherit_Post_Unblock_No: {
      /*
       * The thread shall not be unblocked by the timeout operation.
       */
      T_false( GetUnblock( ctx, &i ) );
      break;
    }

    case ScoreTqReqTimeoutPriorityInherit_Post_Unblock_NA:
      break;
  }
}

static void ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_Check(
  ScoreTqReqTimeoutPriorityInherit_Context           *ctx,
  ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority state
)
{
  switch ( state ) {
    case ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_Nop: {
      /*
       * The priority of the owner with respect to the scheduler shall not
       * change by the timeout operation.
       */
      T_eq_u32( ctx->owner_priority_after, ctx->owner_priority );
      break;
    }

    case ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_Lower: {
      /*
       * The priority of the owner with respect to the scheduler shall be
       * lowered to the next highest priority.
       */
      T_eq_u32( ctx->owner_priority_after, ctx->owner_priority + 1 );
      break;
    }

    case ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_Drop: {
      /*
       * The owner shall not have a priority with respect to the scheduler.
       */
      T_eq_u32( ctx->owner_priority_after, PRIO_INVALID );
      break;
    }

    case ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_NA:
      break;
  }
}

static void ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_Check(
  ScoreTqReqTimeoutPriorityInherit_Context                *ctx,
  ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority state
)
{
  switch ( state ) {
    case ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_Nop: {
      /*
       * The priority of the owner of the thread queue on which the owner is
       * enqueued with respect to the scheduler shall not change by the timeout
       * operation.
       */
      T_eq_u32( ctx->owner_owner_priority_after, ctx->owner_owner_priority );
      break;
    }

    case ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_Lower: {
      /*
       * The priority of the owner of the thread queue on which the owner is
       * enqueued with respect to the scheduler shall be lowered to the next
       * highest priority.
       */
      T_eq_u32( ctx->owner_owner_priority_after, ctx->owner_owner_priority + 1 );
      break;
    }

    case ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_Drop: {
      /*
       * The owner of the thread queue on which the owner is enqueued shall not
       * have a priority with respect to the scheduler.
       */
      T_eq_u32( ctx->owner_owner_priority_after, PRIO_INVALID );
      break;
    }

    case ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA:
      break;
  }
}

static void ScoreTqReqTimeoutPriorityInherit_Setup(
  ScoreTqReqTimeoutPriorityInherit_Context *ctx
)
{
  ctx->request.arg = ctx;
  TQReset( ctx->tq_ctx );
  SetSelfPriority( PRIO_NEARLY_IDLE );
}

static void ScoreTqReqTimeoutPriorityInherit_Setup_Wrap( void *arg )
{
  ScoreTqReqTimeoutPriorityInherit_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  ScoreTqReqTimeoutPriorityInherit_Setup( ctx );
}

static void ScoreTqReqTimeoutPriorityInherit_Teardown(
  ScoreTqReqTimeoutPriorityInherit_Context *ctx
)
{
  TQReset( ctx->tq_ctx );
}

static void ScoreTqReqTimeoutPriorityInherit_Teardown_Wrap( void *arg )
{
  ScoreTqReqTimeoutPriorityInherit_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  ScoreTqReqTimeoutPriorityInherit_Teardown( ctx );
}

static void ScoreTqReqTimeoutPriorityInherit_Prepare(
  ScoreTqReqTimeoutPriorityInherit_Context *ctx
)
{
  ctx->queue_helper_surrender = false;
  ctx->owner_helper_release = false;
  ctx->owner_queue_helper_release = false;
  ctx->owner_owner_helper_release = false;
}

static void ScoreTqReqTimeoutPriorityInherit_Action(
  ScoreTqReqTimeoutPriorityInherit_Context *ctx
)
{
  rtems_task_priority priority;

  priority = PRIO_FLEXIBLE;
  TQSetScheduler( ctx->tq_ctx, THREAD, ctx->scheduler_id, priority );

  TQSend(
    ctx->tq_ctx,
    OWNER,
    TQ_EVENT_MUTEX_A_OBTAIN | TQ_EVENT_ENQUEUE
  );

  if ( ctx->owner_obtain != 0 ) {
    TQSend(
      ctx->tq_ctx,
      OWNER_OWNER,
      TQ_EVENT_MUTEX_B_OBTAIN | ctx->owner_obtain
    );
    TQSend( ctx->tq_ctx, OWNER, ctx->owner_obtain | ctx->owner_release );
  }

  PrepareThread( ctx );
  priority = PrepareQueue( ctx, priority );
  priority = PrepareOwner( ctx, priority );
  priority = PrepareOwnerQueue( ctx, priority );
  PrepareOwnerOwner( ctx, priority );

  TQClearDone( ctx->tq_ctx, THREAD );

  switch ( ctx->wait_state ) {
    case TQ_WAIT_STATE_BLOCKED:
      TQSendAndWaitForExecutionStop(
        ctx->tq_ctx,
        THREAD,
        TQ_EVENT_ENQUEUE_TIMED
      );
      Tick( ctx );
      GetPriorities( ctx );
      TQSend( ctx->tq_ctx, OWNER, TQ_EVENT_SURRENDER );
      break;
    case TQ_WAIT_STATE_INTEND_TO_BLOCK:
      T_scheduler_set_event_handler( SchedulerBlock, ctx );
      TQSendAndWaitForExecutionStop(
        ctx->tq_ctx,
        THREAD,
        TQ_EVENT_ENQUEUE_TIMED
      );
      GetPriorities( ctx );
      TQSend( ctx->tq_ctx, OWNER, TQ_EVENT_SURRENDER );
      break;
    case TQ_WAIT_STATE_READY_AGAIN:
      TQSendAndWaitForExecutionStop(
        ctx->tq_ctx,
        THREAD,
        TQ_EVENT_ENQUEUE_TIMED
      );
      T_scheduler_set_event_handler( SchedulerUnblock, ctx );
      TQSend( ctx->tq_ctx, OWNER, TQ_EVENT_SURRENDER );
      GetPriorities( ctx );
      TQSend( ctx->tq_ctx, THREAD, TQ_EVENT_SURRENDER );
      break;
  }

  TQWaitForDone( ctx->tq_ctx, THREAD );
  TQWaitForExecutionStop( ctx->tq_ctx, THREAD );
}

static void ScoreTqReqTimeoutPriorityInherit_Cleanup(
  ScoreTqReqTimeoutPriorityInherit_Context *ctx
)
{
  if ( ctx->owner_obtain != 0 ) {
    TQSend(
      ctx->tq_ctx,
      OWNER_OWNER,
      TQ_EVENT_MUTEX_B_RELEASE | ctx->owner_release
    );

    if ( ctx->owner_queue_helper_release ) {
      TQSendAndWaitForExecutionStop(
        ctx->tq_ctx,
        OWNER_QUEUE_HELPER,
        ctx->owner_release
      );
    }

    if ( ctx->owner_owner_helper_release ) {
      TQSendAndWaitForExecutionStop(
        ctx->tq_ctx,
        OWNER_OWNER_HELPER,
        TQ_EVENT_MUTEX_B_RELEASE
      );
    }
  }

  TQSend( ctx->tq_ctx, OWNER, TQ_EVENT_MUTEX_A_RELEASE );

  if ( ctx->queue_helper_surrender ) {
    TQSendAndWaitForExecutionStop(
      ctx->tq_ctx,
      QUEUE_HELPER,
      TQ_EVENT_SURRENDER
    );
  }

  if ( ctx->owner_helper_release ) {
    TQSendAndWaitForExecutionStop(
      ctx->tq_ctx,
      OWNER_HELPER,
      TQ_EVENT_MUTEX_A_RELEASE
    );
  }

  if ( ctx->other_scheduler ) {
    TQSendAndWaitForExecutionStop(
      ctx->tq_ctx,
      THREAD,
      TQ_EVENT_MUTEX_D_RELEASE
    );
    TQSendAndWaitForExecutionStop(
      ctx->tq_ctx,
      THREAD_HELPER_A,
      TQ_EVENT_MUTEX_D_RELEASE
    );

    if ( rtems_scheduler_get_processor_maximum() >= 3 ) {
      TQSendAndWaitForExecutionStop(
        ctx->tq_ctx,
        THREAD_HELPER_B,
        TQ_EVENT_MUTEX_D_RELEASE
      );
    }
  }
}

static const ScoreTqReqTimeoutPriorityInherit_Entry
ScoreTqReqTimeoutPriorityInherit_Entries[] = {
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, ScoreTqReqTimeoutPriorityInherit_Post_Status_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
#else
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, ScoreTqReqTimeoutPriorityInherit_Post_Status_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
#endif
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, ScoreTqReqTimeoutPriorityInherit_Post_Status_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, ScoreTqReqTimeoutPriorityInherit_Post_Status_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, ScoreTqReqTimeoutPriorityInherit_Post_Status_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
#else
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, ScoreTqReqTimeoutPriorityInherit_Post_Status_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
#endif
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, ScoreTqReqTimeoutPriorityInherit_Post_Status_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
#else
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, ScoreTqReqTimeoutPriorityInherit_Post_Status_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
#endif
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, ScoreTqReqTimeoutPriorityInherit_Post_Status_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
#else
  { 0, 0, 0, 0, 0, 0, 1, 1, 0,
    ScoreTqReqTimeoutPriorityInherit_Post_Status_Timeout,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_Yes,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_Lower,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
#endif
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, ScoreTqReqTimeoutPriorityInherit_Post_Status_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
#else
  { 0, 0, 0, 0, 0, 0, 1, 1, 0,
    ScoreTqReqTimeoutPriorityInherit_Post_Status_Timeout,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_No,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_Lower,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
#endif
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, ScoreTqReqTimeoutPriorityInherit_Post_Status_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
#else
  { 0, 0, 0, 0, 0, 0, 1, 1, 0,
    ScoreTqReqTimeoutPriorityInherit_Post_Status_Timeout,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_Yes,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_Nop,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
#endif
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, ScoreTqReqTimeoutPriorityInherit_Post_Status_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
#else
  { 0, 0, 0, 0, 0, 0, 1, 1, 0,
    ScoreTqReqTimeoutPriorityInherit_Post_Status_Timeout,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_No,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_Nop,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
#endif
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, ScoreTqReqTimeoutPriorityInherit_Post_Status_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 1, 0,
    ScoreTqReqTimeoutPriorityInherit_Post_Status_Timeout,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_Yes,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_Lower,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
#endif
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, ScoreTqReqTimeoutPriorityInherit_Post_Status_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 1, 0,
    ScoreTqReqTimeoutPriorityInherit_Post_Status_Timeout,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_No,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_Lower,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
#endif
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, ScoreTqReqTimeoutPriorityInherit_Post_Status_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, ScoreTqReqTimeoutPriorityInherit_Post_Status_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
#else
  { 0, 0, 0, 0, 0, 0, 1, 1, 0, ScoreTqReqTimeoutPriorityInherit_Post_Status_Ok,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_No,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_Lower,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
#endif
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, ScoreTqReqTimeoutPriorityInherit_Post_Status_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 1, 0,
    ScoreTqReqTimeoutPriorityInherit_Post_Status_Timeout,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_Yes,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_Nop,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
#endif
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, ScoreTqReqTimeoutPriorityInherit_Post_Status_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 1, 0,
    ScoreTqReqTimeoutPriorityInherit_Post_Status_Timeout,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_No,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_Nop,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
#endif
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, ScoreTqReqTimeoutPriorityInherit_Post_Status_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqTimeoutPriorityInherit_Post_Status_Timeout,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_Yes,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_Nop,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_Nop },
#endif
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, ScoreTqReqTimeoutPriorityInherit_Post_Status_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqTimeoutPriorityInherit_Post_Status_Timeout,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_No,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_Nop,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_Nop },
#endif
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, ScoreTqReqTimeoutPriorityInherit_Post_Status_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
#else
  { 0, 0, 0, 0, 0, 0, 1, 1, 0,
    ScoreTqReqTimeoutPriorityInherit_Post_Status_Timeout,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_Yes,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_Drop,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
#endif
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, ScoreTqReqTimeoutPriorityInherit_Post_Status_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
#else
  { 0, 0, 0, 0, 0, 0, 1, 1, 0,
    ScoreTqReqTimeoutPriorityInherit_Post_Status_Timeout,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_No,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_Drop,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
#endif
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, ScoreTqReqTimeoutPriorityInherit_Post_Status_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
#else
  { 0, 0, 0, 0, 0, 0, 1, 1, 0, ScoreTqReqTimeoutPriorityInherit_Post_Status_Ok,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_No,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_Drop,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
#endif
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, ScoreTqReqTimeoutPriorityInherit_Post_Status_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqTimeoutPriorityInherit_Post_Status_Timeout,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_Yes,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_Lower,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_Nop },
#endif
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, ScoreTqReqTimeoutPriorityInherit_Post_Status_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqTimeoutPriorityInherit_Post_Status_Timeout,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_No,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_Lower,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_Nop },
#endif
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, ScoreTqReqTimeoutPriorityInherit_Post_Status_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqTimeoutPriorityInherit_Post_Status_Timeout,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_Yes,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_Lower,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_Lower },
#endif
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, ScoreTqReqTimeoutPriorityInherit_Post_Status_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqTimeoutPriorityInherit_Post_Status_Timeout,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_No,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_Lower,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_Lower },
#endif
  { 0, 0, 0, 0, 0, 0, 1, 1, 0,
    ScoreTqReqTimeoutPriorityInherit_Post_Status_Timeout,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_Yes,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_Lower,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
  { 0, 0, 0, 0, 0, 0, 1, 1, 0,
    ScoreTqReqTimeoutPriorityInherit_Post_Status_Timeout,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_No,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_Lower,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, ScoreTqReqTimeoutPriorityInherit_Post_Status_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
#else
  { 0, 0, 0, 0, 0, 0, 1, 1, 0, ScoreTqReqTimeoutPriorityInherit_Post_Status_Ok,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_No,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_Nop,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
#endif
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, ScoreTqReqTimeoutPriorityInherit_Post_Status_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 1, 0,
    ScoreTqReqTimeoutPriorityInherit_Post_Status_Timeout,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_Yes,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_Drop,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
#endif
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, ScoreTqReqTimeoutPriorityInherit_Post_Status_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 1, 0,
    ScoreTqReqTimeoutPriorityInherit_Post_Status_Timeout,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_No,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_Drop,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
#endif
  { 0, 0, 0, 0, 0, 0, 1, 1, 0,
    ScoreTqReqTimeoutPriorityInherit_Post_Status_Timeout,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_Yes,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_Nop,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
  { 0, 0, 0, 0, 0, 0, 1, 1, 0,
    ScoreTqReqTimeoutPriorityInherit_Post_Status_Timeout,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_No,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_Nop,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
  { 0, 0, 0, 0, 0, 0, 1, 1, 0, ScoreTqReqTimeoutPriorityInherit_Post_Status_Ok,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_No,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_Lower,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
  { 0, 0, 0, 0, 0, 0, 0, 1, 0,
    ScoreTqReqTimeoutPriorityInherit_Post_Status_Timeout,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_Yes,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_Lower,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
  { 0, 0, 0, 0, 0, 0, 0, 1, 0,
    ScoreTqReqTimeoutPriorityInherit_Post_Status_Timeout,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_No,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_Lower,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, ScoreTqReqTimeoutPriorityInherit_Post_Status_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqTimeoutPriorityInherit_Post_Status_Timeout,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_Yes,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_Drop,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_Nop },
#endif
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, ScoreTqReqTimeoutPriorityInherit_Post_Status_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqTimeoutPriorityInherit_Post_Status_Timeout,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_No,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_Drop,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_Nop },
#endif
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqTimeoutPriorityInherit_Post_Status_Timeout,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_Yes,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_Lower,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqTimeoutPriorityInherit_Post_Status_Timeout,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_No,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_Lower,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 1, 0,
    ScoreTqReqTimeoutPriorityInherit_Post_Status_Timeout,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_Yes,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_Nop,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
  { 0, 0, 0, 0, 0, 0, 0, 1, 0,
    ScoreTqReqTimeoutPriorityInherit_Post_Status_Timeout,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_No,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_Nop,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqTimeoutPriorityInherit_Post_Status_Timeout,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_Yes,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_Nop,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqTimeoutPriorityInherit_Post_Status_Timeout,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_No,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_Nop,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_Nop },
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, ScoreTqReqTimeoutPriorityInherit_Post_Status_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqTimeoutPriorityInherit_Post_Status_Timeout,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_Yes,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_Drop,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_Lower },
#endif
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, ScoreTqReqTimeoutPriorityInherit_Post_Status_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqTimeoutPriorityInherit_Post_Status_Timeout,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_No,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_Drop,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_Lower },
#endif
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqTimeoutPriorityInherit_Post_Status_Timeout,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_Yes,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_Lower,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_Lower },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqTimeoutPriorityInherit_Post_Status_Timeout,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_No,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_Lower,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_Lower },
  { 0, 0, 0, 0, 0, 0, 1, 1, 0, ScoreTqReqTimeoutPriorityInherit_Post_Status_Ok,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_No,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_Nop,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, ScoreTqReqTimeoutPriorityInherit_Post_Status_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqTimeoutPriorityInherit_Post_Status_Timeout,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_Yes,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_Drop,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_Drop },
#endif
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, ScoreTqReqTimeoutPriorityInherit_Post_Status_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_NA,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_NA }
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreTqReqTimeoutPriorityInherit_Post_Status_Timeout,
    ScoreTqReqTimeoutPriorityInherit_Post_Unblock_No,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_Drop,
    ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_Drop }
#endif
};

static const uint8_t
ScoreTqReqTimeoutPriorityInherit_Map[] = {
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1,
  1, 2, 2, 2, 2, 2, 2, 24, 25, 31, 24, 25, 31, 2, 2, 2, 24, 25, 31, 24, 25, 31,
  2, 2, 2, 1, 1, 1, 24, 25, 31, 2, 2, 2, 24, 25, 11, 24, 25, 11, 2, 2, 2, 24,
  25, 11, 24, 25, 11, 2, 2, 2, 1, 1, 1, 24, 25, 11, 2, 2, 2, 32, 33, 11, 32,
  33, 11, 2, 2, 2, 32, 33, 11, 32, 33, 11, 2, 2, 2, 1, 1, 1, 32, 33, 11, 2, 2,
  2, 44, 45, 11, 36, 37, 11, 2, 2, 2, 44, 45, 11, 36, 37, 11, 2, 2, 2, 1, 1, 1,
  36, 37, 11, 2, 2, 2, 1, 1, 1, 29, 30, 46, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2,
  1, 1, 1, 29, 30, 46, 2, 2, 2, 1, 1, 1, 29, 30, 11, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  2, 2, 2, 1, 1, 1, 29, 30, 11, 2, 2, 2, 1, 1, 1, 38, 39, 11, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 2, 2, 2, 1, 1, 1, 38, 39, 11, 2, 2, 2, 1, 1, 1, 40, 41, 11, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 2, 2, 2, 1, 1, 1, 40, 41, 11, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 2, 2, 2, 2, 2, 2, 24, 25,
  31, 24, 25, 31, 2, 2, 2, 24, 25, 31, 24, 25, 31, 2, 2, 2, 1, 1, 1, 24, 25,
  31, 2, 2, 2, 24, 25, 11, 24, 25, 11, 2, 2, 2, 24, 25, 11, 24, 25, 11, 2, 2,
  2, 1, 1, 1, 24, 25, 11, 2, 2, 2, 32, 33, 11, 32, 33, 11, 2, 2, 2, 32, 33, 11,
  32, 33, 11, 2, 2, 2, 1, 1, 1, 32, 33, 11, 2, 2, 2, 44, 45, 11, 36, 37, 11, 2,
  2, 2, 44, 45, 11, 36, 37, 11, 2, 2, 2, 1, 1, 1, 36, 37, 11, 2, 2, 2, 1, 1, 1,
  29, 30, 46, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 1, 1, 1, 29, 30, 46, 2, 2, 2,
  1, 1, 1, 29, 30, 11, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 1, 1, 1, 29, 30, 11,
  2, 2, 2, 1, 1, 1, 38, 39, 11, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 1, 1, 1,
  38, 39, 11, 2, 2, 2, 1, 1, 1, 40, 41, 11, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2,
  1, 1, 1, 40, 41, 11, 2, 2, 2, 1, 1, 1, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2,
  2, 2, 1, 1, 1, 2, 2, 2, 2, 2, 2, 1, 1, 1, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  2, 2, 2, 1, 1, 1, 2, 2, 2, 2, 2, 2, 1, 1, 1, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 2, 2, 2, 1, 1, 1, 2, 2, 2, 2, 2, 2, 1, 1, 1, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 2, 2, 2, 1, 1, 1, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 1, 1, 1, 29, 30, 11, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 1, 1, 1, 29, 30, 11, 2, 2, 2, 1, 1, 1, 29,
  30, 11, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 1, 1, 1, 29, 30, 11, 2, 2, 2, 1,
  1, 1, 38, 39, 11, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 1, 1, 1, 38, 39, 11, 2,
  2, 2, 1, 1, 1, 40, 41, 11, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 1, 1, 1, 40,
  41, 11, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0,
  0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0,
  0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 0, 0, 0, 3, 3, 3, 3, 3, 3, 5, 6, 12, 5, 6, 12, 3, 3, 3, 5, 6, 12, 5, 6,
  12, 3, 3, 3, 0, 0, 0, 5, 6, 12, 3, 3, 3, 5, 6, 4, 5, 6, 4, 3, 3, 3, 5, 6, 4,
  5, 6, 4, 3, 3, 3, 0, 0, 0, 5, 6, 4, 3, 3, 3, 9, 10, 4, 9, 10, 4, 3, 3, 3, 9,
  10, 4, 9, 10, 4, 3, 3, 3, 0, 0, 0, 9, 10, 4, 3, 3, 3, 22, 23, 4, 20, 21, 4,
  3, 3, 3, 22, 23, 4, 20, 21, 4, 3, 3, 3, 0, 0, 0, 20, 21, 4, 3, 3, 3, 0, 0, 0,
  7, 8, 26, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0, 7, 8, 26, 3, 3, 3, 0,
  0, 0, 7, 8, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0, 7, 8, 4, 3, 3, 3,
  0, 0, 0, 13, 14, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0, 13, 14, 4,
  3, 3, 3, 0, 0, 0, 15, 16, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0, 15,
  16, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0,
  0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0,
  0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 0, 0, 0, 3, 3, 3, 3, 3, 3, 5, 6, 12, 5, 6, 12, 3, 3, 3, 5, 6, 12, 5, 6,
  12, 3, 3, 3, 0, 0, 0, 5, 6, 12, 3, 3, 3, 5, 6, 4, 5, 6, 4, 3, 3, 3, 5, 6, 4,
  5, 6, 4, 3, 3, 3, 0, 0, 0, 5, 6, 4, 3, 3, 3, 9, 10, 4, 9, 10, 4, 3, 3, 3, 9,
  10, 4, 9, 10, 4, 3, 3, 3, 0, 0, 0, 9, 10, 4, 3, 3, 3, 22, 23, 4, 20, 21, 4,
  3, 3, 3, 22, 23, 4, 20, 21, 4, 3, 3, 3, 0, 0, 0, 20, 21, 4, 3, 3, 3, 0, 0, 0,
  7, 8, 26, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0, 7, 8, 26, 3, 3, 3, 0,
  0, 0, 7, 8, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0, 7, 8, 4, 3, 3, 3,
  0, 0, 0, 13, 14, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0, 13, 14, 4,
  3, 3, 3, 0, 0, 0, 15, 16, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0, 15,
  16, 4, 3, 3, 3, 0, 0, 0, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0,
  0, 3, 3, 3, 3, 3, 3, 0, 0, 0, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 0,
  0, 0, 3, 3, 3, 3, 3, 3, 0, 0, 0, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3,
  0, 0, 0, 3, 3, 3, 3, 3, 3, 0, 0, 0, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3,
  3, 0, 0, 0, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0, 7, 8, 4, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 3, 3, 3, 0, 0, 0, 7, 8, 4, 3, 3, 3, 0, 0, 0, 7, 8, 4, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 3, 3, 3, 0, 0, 0, 7, 8, 4, 3, 3, 3, 0, 0, 0, 13, 14, 4, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0, 13, 14, 4, 3, 3, 3, 0, 0, 0, 15, 16, 4,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0, 15, 16, 4, 17, 18, 19, 17, 18,
  19, 17, 18, 19, 17, 18, 19, 17, 18, 19, 17, 18, 19, 17, 18, 19, 0, 0, 0, 17,
  18, 19, 17, 18, 4, 17, 18, 4, 17, 18, 4, 17, 18, 4, 17, 18, 4, 17, 18, 4, 17,
  18, 4, 0, 0, 0, 17, 18, 4, 27, 28, 4, 27, 28, 4, 27, 28, 4, 27, 28, 4, 27,
  28, 4, 27, 28, 4, 27, 28, 4, 0, 0, 0, 27, 28, 4, 47, 48, 4, 42, 43, 4, 34,
  35, 4, 42, 43, 4, 42, 43, 4, 34, 35, 4, 34, 35, 4, 0, 0, 0, 34, 35, 4, 5, 6,
  12, 5, 6, 12, 5, 6, 12, 5, 6, 12, 5, 6, 12, 5, 6, 12, 5, 6, 12, 0, 0, 0, 5,
  6, 12, 5, 6, 4, 5, 6, 4, 5, 6, 4, 5, 6, 4, 5, 6, 4, 5, 6, 4, 5, 6, 4, 0, 0,
  0, 5, 6, 4, 9, 10, 4, 9, 10, 4, 9, 10, 4, 9, 10, 4, 9, 10, 4, 9, 10, 4, 9,
  10, 4, 0, 0, 0, 9, 10, 4, 22, 23, 4, 22, 23, 4, 20, 21, 4, 22, 23, 4, 22, 23,
  4, 20, 21, 4, 20, 21, 4, 0, 0, 0, 20, 21, 4, 7, 8, 26, 0, 0, 0, 7, 8, 26, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 7, 8, 26, 0, 0, 0, 7, 8, 26, 7, 8, 4, 0, 0, 0, 7, 8,
  4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 8, 4, 0, 0, 0, 7, 8, 4, 13, 14, 4, 0, 0, 0,
  13, 14, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13, 14, 4, 0, 0, 0, 13, 14, 4, 15, 16,
  4, 0, 0, 0, 15, 16, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 16, 4, 0, 0, 0, 15, 16,
  4, 5, 6, 19, 5, 6, 19, 5, 6, 19, 5, 6, 19, 5, 6, 19, 5, 6, 19, 5, 6, 19, 0,
  0, 0, 5, 6, 19, 5, 6, 4, 5, 6, 4, 5, 6, 4, 5, 6, 4, 5, 6, 4, 5, 6, 4, 5, 6,
  4, 0, 0, 0, 5, 6, 4, 9, 10, 4, 9, 10, 4, 9, 10, 4, 9, 10, 4, 9, 10, 4, 9, 10,
  4, 9, 10, 4, 0, 0, 0, 9, 10, 4, 22, 23, 4, 22, 23, 4, 20, 21, 4, 22, 23, 4,
  22, 23, 4, 20, 21, 4, 20, 21, 4, 0, 0, 0, 20, 21, 4, 5, 6, 12, 5, 6, 12, 5,
  6, 12, 5, 6, 12, 5, 6, 12, 5, 6, 12, 5, 6, 12, 0, 0, 0, 5, 6, 12, 5, 6, 4, 5,
  6, 4, 5, 6, 4, 5, 6, 4, 5, 6, 4, 5, 6, 4, 5, 6, 4, 0, 0, 0, 5, 6, 4, 9, 10,
  4, 9, 10, 4, 9, 10, 4, 9, 10, 4, 9, 10, 4, 9, 10, 4, 9, 10, 4, 0, 0, 0, 9,
  10, 4, 22, 23, 4, 22, 23, 4, 20, 21, 4, 22, 23, 4, 22, 23, 4, 20, 21, 4, 20,
  21, 4, 0, 0, 0, 20, 21, 4, 7, 8, 26, 0, 0, 0, 7, 8, 26, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 7, 8, 26, 0, 0, 0, 7, 8, 26, 7, 8, 4, 0, 0, 0, 7, 8, 4, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 7, 8, 4, 0, 0, 0, 7, 8, 4, 13, 14, 4, 0, 0, 0, 13, 14, 4, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 13, 14, 4, 0, 0, 0, 13, 14, 4, 15, 16, 4, 0, 0, 0, 15,
  16, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 16, 4, 0, 0, 0, 15, 16, 4, 7, 8, 4, 0,
  0, 0, 7, 8, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 8, 4, 0, 0, 0, 7, 8, 4, 7, 8, 4,
  0, 0, 0, 7, 8, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 8, 4, 0, 0, 0, 7, 8, 4, 13,
  14, 4, 0, 0, 0, 13, 14, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13, 14, 4, 0, 0, 0, 13,
  14, 4, 15, 16, 4, 0, 0, 0, 15, 16, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 16, 4,
  0, 0, 0, 15, 16, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 8, 4, 0, 0, 0, 7, 8, 4, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 7, 8, 4, 0, 0, 0, 7, 8, 4, 7, 8, 4, 0, 0, 0, 7, 8, 4, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 7, 8, 4, 0, 0, 0, 7, 8, 4, 13, 14, 4, 0, 0, 0, 13, 14, 4, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 13, 14, 4, 0, 0, 0, 13, 14, 4, 15, 16, 4, 0, 0, 0, 15,
  16, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 16, 4, 0, 0, 0, 15, 16, 4, 17, 18, 19,
  17, 18, 19, 17, 18, 19, 17, 18, 19, 17, 18, 19, 17, 18, 19, 17, 18, 19, 0, 0,
  0, 17, 18, 19, 17, 18, 4, 17, 18, 4, 17, 18, 4, 17, 18, 4, 17, 18, 4, 17, 18,
  4, 17, 18, 4, 0, 0, 0, 17, 18, 4, 27, 28, 4, 27, 28, 4, 27, 28, 4, 27, 28, 4,
  27, 28, 4, 27, 28, 4, 27, 28, 4, 0, 0, 0, 27, 28, 4, 47, 48, 4, 42, 43, 4,
  34, 35, 4, 42, 43, 4, 42, 43, 4, 34, 35, 4, 34, 35, 4, 0, 0, 0, 34, 35, 4, 5,
  6, 12, 5, 6, 12, 5, 6, 12, 5, 6, 12, 5, 6, 12, 5, 6, 12, 5, 6, 12, 0, 0, 0,
  5, 6, 12, 5, 6, 4, 5, 6, 4, 5, 6, 4, 5, 6, 4, 5, 6, 4, 5, 6, 4, 5, 6, 4, 0,
  0, 0, 5, 6, 4, 9, 10, 4, 9, 10, 4, 9, 10, 4, 9, 10, 4, 9, 10, 4, 9, 10, 4, 9,
  10, 4, 0, 0, 0, 9, 10, 4, 22, 23, 4, 22, 23, 4, 20, 21, 4, 22, 23, 4, 22, 23,
  4, 20, 21, 4, 20, 21, 4, 0, 0, 0, 20, 21, 4, 7, 8, 26, 0, 0, 0, 7, 8, 26, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 7, 8, 26, 0, 0, 0, 7, 8, 26, 7, 8, 4, 0, 0, 0, 7, 8,
  4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 8, 4, 0, 0, 0, 7, 8, 4, 13, 14, 4, 0, 0, 0,
  13, 14, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13, 14, 4, 0, 0, 0, 13, 14, 4, 15, 16,
  4, 0, 0, 0, 15, 16, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 16, 4, 0, 0, 0, 15, 16,
  4, 5, 6, 19, 5, 6, 19, 5, 6, 19, 5, 6, 19, 5, 6, 19, 5, 6, 19, 5, 6, 19, 0,
  0, 0, 5, 6, 19, 5, 6, 4, 5, 6, 4, 5, 6, 4, 5, 6, 4, 5, 6, 4, 5, 6, 4, 5, 6,
  4, 0, 0, 0, 5, 6, 4, 9, 10, 4, 9, 10, 4, 9, 10, 4, 9, 10, 4, 9, 10, 4, 9, 10,
  4, 9, 10, 4, 0, 0, 0, 9, 10, 4, 22, 23, 4, 22, 23, 4, 20, 21, 4, 22, 23, 4,
  22, 23, 4, 20, 21, 4, 20, 21, 4, 0, 0, 0, 20, 21, 4, 5, 6, 12, 5, 6, 12, 5,
  6, 12, 5, 6, 12, 5, 6, 12, 5, 6, 12, 5, 6, 12, 0, 0, 0, 5, 6, 12, 5, 6, 4, 5,
  6, 4, 5, 6, 4, 5, 6, 4, 5, 6, 4, 5, 6, 4, 5, 6, 4, 0, 0, 0, 5, 6, 4, 9, 10,
  4, 9, 10, 4, 9, 10, 4, 9, 10, 4, 9, 10, 4, 9, 10, 4, 9, 10, 4, 0, 0, 0, 9,
  10, 4, 22, 23, 4, 22, 23, 4, 20, 21, 4, 22, 23, 4, 22, 23, 4, 20, 21, 4, 20,
  21, 4, 0, 0, 0, 20, 21, 4, 7, 8, 26, 0, 0, 0, 7, 8, 26, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 7, 8, 26, 0, 0, 0, 7, 8, 26, 7, 8, 4, 0, 0, 0, 7, 8, 4, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 7, 8, 4, 0, 0, 0, 7, 8, 4, 13, 14, 4, 0, 0, 0, 13, 14, 4, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 13, 14, 4, 0, 0, 0, 13, 14, 4, 15, 16, 4, 0, 0, 0, 15,
  16, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 16, 4, 0, 0, 0, 15, 16, 4, 7, 8, 4, 0,
  0, 0, 7, 8, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 8, 4, 0, 0, 0, 7, 8, 4, 7, 8, 4,
  0, 0, 0, 7, 8, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 8, 4, 0, 0, 0, 7, 8, 4, 13,
  14, 4, 0, 0, 0, 13, 14, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13, 14, 4, 0, 0, 0, 13,
  14, 4, 15, 16, 4, 0, 0, 0, 15, 16, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 16, 4,
  0, 0, 0, 15, 16, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 8, 4, 0, 0, 0, 7, 8, 4, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 7, 8, 4, 0, 0, 0, 7, 8, 4, 7, 8, 4, 0, 0, 0, 7, 8, 4, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 7, 8, 4, 0, 0, 0, 7, 8, 4, 13, 14, 4, 0, 0, 0, 13, 14, 4, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 13, 14, 4, 0, 0, 0, 13, 14, 4, 15, 16, 4, 0, 0, 0, 15,
  16, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 16, 4, 0, 0, 0, 15, 16, 4
};

static size_t ScoreTqReqTimeoutPriorityInherit_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  ScoreTqReqTimeoutPriorityInherit_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      ScoreTqReqTimeoutPriorityInherit_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture ScoreTqReqTimeoutPriorityInherit_Fixture = {
  .setup = ScoreTqReqTimeoutPriorityInherit_Setup_Wrap,
  .stop = NULL,
  .teardown = ScoreTqReqTimeoutPriorityInherit_Teardown_Wrap,
  .scope = ScoreTqReqTimeoutPriorityInherit_Scope,
  .initial_context = &ScoreTqReqTimeoutPriorityInherit_Instance
};

static inline ScoreTqReqTimeoutPriorityInherit_Entry
ScoreTqReqTimeoutPriorityInherit_PopEntry(
  ScoreTqReqTimeoutPriorityInherit_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return ScoreTqReqTimeoutPriorityInherit_Entries[
    ScoreTqReqTimeoutPriorityInherit_Map[ index ]
  ];
}

static void ScoreTqReqTimeoutPriorityInherit_SetPreConditionStates(
  ScoreTqReqTimeoutPriorityInherit_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  ctx->Map.pcs[ 4 ] = ctx->Map.pci[ 4 ];

  if ( ctx->Map.entry.Pre_OwnerQueue_NA ) {
    ctx->Map.pcs[ 5 ] = ScoreTqReqTimeoutPriorityInherit_Pre_OwnerQueue_NA;
  } else {
    ctx->Map.pcs[ 5 ] = ctx->Map.pci[ 5 ];
  }

  if ( ctx->Map.entry.Pre_OwnerOwnerPriority_NA ) {
    ctx->Map.pcs[ 6 ] = ScoreTqReqTimeoutPriorityInherit_Pre_OwnerOwnerPriority_NA;
  } else {
    ctx->Map.pcs[ 6 ] = ctx->Map.pci[ 6 ];
  }

  ctx->Map.pcs[ 7 ] = ctx->Map.pci[ 7 ];
}

static void ScoreTqReqTimeoutPriorityInherit_TestVariant(
  ScoreTqReqTimeoutPriorityInherit_Context *ctx
)
{
  ScoreTqReqTimeoutPriorityInherit_Pre_HomeScheduler_Prepare(
    ctx,
    ctx->Map.pcs[ 0 ]
  );
  ScoreTqReqTimeoutPriorityInherit_Pre_EligibleScheduler_Prepare(
    ctx,
    ctx->Map.pcs[ 1 ]
  );
  ScoreTqReqTimeoutPriorityInherit_Pre_Queue_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  ScoreTqReqTimeoutPriorityInherit_Pre_OwnerPriority_Prepare(
    ctx,
    ctx->Map.pcs[ 3 ]
  );
  ScoreTqReqTimeoutPriorityInherit_Pre_OwnerState_Prepare(
    ctx,
    ctx->Map.pcs[ 4 ]
  );
  ScoreTqReqTimeoutPriorityInherit_Pre_OwnerQueue_Prepare(
    ctx,
    ctx->Map.pcs[ 5 ]
  );
  ScoreTqReqTimeoutPriorityInherit_Pre_OwnerOwnerPriority_Prepare(
    ctx,
    ctx->Map.pcs[ 6 ]
  );
  ScoreTqReqTimeoutPriorityInherit_Pre_WaitState_Prepare(
    ctx,
    ctx->Map.pcs[ 7 ]
  );
  ScoreTqReqTimeoutPriorityInherit_Action( ctx );
  ScoreTqReqTimeoutPriorityInherit_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  ScoreTqReqTimeoutPriorityInherit_Post_Unblock_Check(
    ctx,
    ctx->Map.entry.Post_Unblock
  );
  ScoreTqReqTimeoutPriorityInherit_Post_OwnerPriority_Check(
    ctx,
    ctx->Map.entry.Post_OwnerPriority
  );
  ScoreTqReqTimeoutPriorityInherit_Post_OwnerOwnerPriority_Check(
    ctx,
    ctx->Map.entry.Post_OwnerOwnerPriority
  );
}

static T_fixture_node ScoreTqReqTimeoutPriorityInherit_Node;

static T_remark ScoreTqReqTimeoutPriorityInherit_Remark = {
  .next = NULL,
  .remark = "ScoreTqReqTimeoutPriorityInherit"
};

void ScoreTqReqTimeoutPriorityInherit_Run( TQContext *tq_ctx )
{
  ScoreTqReqTimeoutPriorityInherit_Context *ctx;

  ctx = &ScoreTqReqTimeoutPriorityInherit_Instance;
  ctx->tq_ctx = tq_ctx;

  ctx = T_push_fixture(
    &ScoreTqReqTimeoutPriorityInherit_Node,
    &ScoreTqReqTimeoutPriorityInherit_Fixture
  );
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = ScoreTqReqTimeoutPriorityInherit_Pre_HomeScheduler_Home;
    ctx->Map.pci[ 0 ] < ScoreTqReqTimeoutPriorityInherit_Pre_HomeScheduler_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = ScoreTqReqTimeoutPriorityInherit_Pre_EligibleScheduler_One;
      ctx->Map.pci[ 1 ] < ScoreTqReqTimeoutPriorityInherit_Pre_EligibleScheduler_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = ScoreTqReqTimeoutPriorityInherit_Pre_Queue_Only;
        ctx->Map.pci[ 2 ] < ScoreTqReqTimeoutPriorityInherit_Pre_Queue_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = ScoreTqReqTimeoutPriorityInherit_Pre_OwnerPriority_Only;
          ctx->Map.pci[ 3 ] < ScoreTqReqTimeoutPriorityInherit_Pre_OwnerPriority_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          for (
            ctx->Map.pci[ 4 ] = ScoreTqReqTimeoutPriorityInherit_Pre_OwnerState_NotEnqueued;
            ctx->Map.pci[ 4 ] < ScoreTqReqTimeoutPriorityInherit_Pre_OwnerState_NA;
            ++ctx->Map.pci[ 4 ]
          ) {
            for (
              ctx->Map.pci[ 5 ] = ScoreTqReqTimeoutPriorityInherit_Pre_OwnerQueue_Only;
              ctx->Map.pci[ 5 ] < ScoreTqReqTimeoutPriorityInherit_Pre_OwnerQueue_NA;
              ++ctx->Map.pci[ 5 ]
            ) {
              for (
                ctx->Map.pci[ 6 ] = ScoreTqReqTimeoutPriorityInherit_Pre_OwnerOwnerPriority_Only;
                ctx->Map.pci[ 6 ] < ScoreTqReqTimeoutPriorityInherit_Pre_OwnerOwnerPriority_NA;
                ++ctx->Map.pci[ 6 ]
              ) {
                for (
                  ctx->Map.pci[ 7 ] = ScoreTqReqTimeoutPriorityInherit_Pre_WaitState_Blocked;
                  ctx->Map.pci[ 7 ] < ScoreTqReqTimeoutPriorityInherit_Pre_WaitState_NA;
                  ++ctx->Map.pci[ 7 ]
                ) {
                  ctx->Map.entry = ScoreTqReqTimeoutPriorityInherit_PopEntry(
                    ctx
                  );

                  if ( ctx->Map.entry.Skip ) {
                    continue;
                  }

                  ScoreTqReqTimeoutPriorityInherit_SetPreConditionStates(
                    ctx
                  );
                  ScoreTqReqTimeoutPriorityInherit_Prepare( ctx );
                  ScoreTqReqTimeoutPriorityInherit_TestVariant( ctx );
                  ScoreTqReqTimeoutPriorityInherit_Cleanup( ctx );
                }
              }
            }
          }
        }
      }
    }
  }

  T_add_remark( &ScoreTqReqTimeoutPriorityInherit_Remark );
  T_pop_fixture();
}

/** @} */
