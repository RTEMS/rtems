/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsTaskReqSetScheduler
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

#include <rtems.h>
#include <rtems/score/threadimpl.h>

#include "ts-config.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsTaskReqSetScheduler spec:/rtems/task/req/set-scheduler
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsTaskReqSetScheduler_Pre_TaskId_Task,
  RtemsTaskReqSetScheduler_Pre_TaskId_Invalid,
  RtemsTaskReqSetScheduler_Pre_TaskId_NA
} RtemsTaskReqSetScheduler_Pre_TaskId;

typedef enum {
  RtemsTaskReqSetScheduler_Pre_Scheduler_Home,
  RtemsTaskReqSetScheduler_Pre_Scheduler_Other,
  RtemsTaskReqSetScheduler_Pre_Scheduler_NA
} RtemsTaskReqSetScheduler_Pre_Scheduler;

typedef enum {
  RtemsTaskReqSetScheduler_Pre_SchedulerHasCPU_Yes,
  RtemsTaskReqSetScheduler_Pre_SchedulerHasCPU_No,
  RtemsTaskReqSetScheduler_Pre_SchedulerHasCPU_NA
} RtemsTaskReqSetScheduler_Pre_SchedulerHasCPU;

typedef enum {
  RtemsTaskReqSetScheduler_Pre_SchedulerId_Scheduler,
  RtemsTaskReqSetScheduler_Pre_SchedulerId_Invalid,
  RtemsTaskReqSetScheduler_Pre_SchedulerId_NA
} RtemsTaskReqSetScheduler_Pre_SchedulerId;

typedef enum {
  RtemsTaskReqSetScheduler_Pre_Priority_Valid,
  RtemsTaskReqSetScheduler_Pre_Priority_Invalid,
  RtemsTaskReqSetScheduler_Pre_Priority_NA
} RtemsTaskReqSetScheduler_Pre_Priority;

typedef enum {
  RtemsTaskReqSetScheduler_Pre_HomePriority_Real,
  RtemsTaskReqSetScheduler_Pre_HomePriority_More,
  RtemsTaskReqSetScheduler_Pre_HomePriority_NA
} RtemsTaskReqSetScheduler_Pre_HomePriority;

typedef enum {
  RtemsTaskReqSetScheduler_Pre_EligiblePriorities_OnlyOne,
  RtemsTaskReqSetScheduler_Pre_EligiblePriorities_More,
  RtemsTaskReqSetScheduler_Pre_EligiblePriorities_NA
} RtemsTaskReqSetScheduler_Pre_EligiblePriorities;

typedef enum {
  RtemsTaskReqSetScheduler_Pre_Pinned_Yes,
  RtemsTaskReqSetScheduler_Pre_Pinned_No,
  RtemsTaskReqSetScheduler_Pre_Pinned_NA
} RtemsTaskReqSetScheduler_Pre_Pinned;

typedef enum {
  RtemsTaskReqSetScheduler_Pre_TaskState_Ready,
  RtemsTaskReqSetScheduler_Pre_TaskState_Blocked,
  RtemsTaskReqSetScheduler_Pre_TaskState_Enqueued,
  RtemsTaskReqSetScheduler_Pre_TaskState_NA
} RtemsTaskReqSetScheduler_Pre_TaskState;

typedef enum {
  RtemsTaskReqSetScheduler_Pre_AffinitySupported_Yes,
  RtemsTaskReqSetScheduler_Pre_AffinitySupported_No,
  RtemsTaskReqSetScheduler_Pre_AffinitySupported_NA
} RtemsTaskReqSetScheduler_Pre_AffinitySupported;

typedef enum {
  RtemsTaskReqSetScheduler_Post_Status_Ok,
  RtemsTaskReqSetScheduler_Post_Status_InvAddr,
  RtemsTaskReqSetScheduler_Post_Status_InvId,
  RtemsTaskReqSetScheduler_Post_Status_InvPrio,
  RtemsTaskReqSetScheduler_Post_Status_InUse,
  RtemsTaskReqSetScheduler_Post_Status_Unsat,
  RtemsTaskReqSetScheduler_Post_Status_NA
} RtemsTaskReqSetScheduler_Post_Status;

typedef enum {
  RtemsTaskReqSetScheduler_Post_Scheduler_Set,
  RtemsTaskReqSetScheduler_Post_Scheduler_Nop,
  RtemsTaskReqSetScheduler_Post_Scheduler_NA
} RtemsTaskReqSetScheduler_Post_Scheduler;

typedef enum {
  RtemsTaskReqSetScheduler_Post_Priority_Set,
  RtemsTaskReqSetScheduler_Post_Priority_Nop,
  RtemsTaskReqSetScheduler_Post_Priority_NA
} RtemsTaskReqSetScheduler_Post_Priority;

typedef struct {
  uint32_t Skip : 1;
  uint32_t Pre_TaskId_NA : 1;
  uint32_t Pre_Scheduler_NA : 1;
  uint32_t Pre_SchedulerHasCPU_NA : 1;
  uint32_t Pre_SchedulerId_NA : 1;
  uint32_t Pre_Priority_NA : 1;
  uint32_t Pre_HomePriority_NA : 1;
  uint32_t Pre_EligiblePriorities_NA : 1;
  uint32_t Pre_Pinned_NA : 1;
  uint32_t Pre_TaskState_NA : 1;
  uint32_t Pre_AffinitySupported_NA : 1;
  uint32_t Post_Status : 3;
  uint32_t Post_Scheduler : 2;
  uint32_t Post_Priority : 2;
} RtemsTaskReqSetScheduler_Entry;

/**
 * @brief Test context for spec:/rtems/task/req/set-scheduler test case.
 */
typedef struct {
  /**
   * @brief This member contains the runner task identifier.
   */
  rtems_id runner_id;

  /**
   * @brief This member contains the scheduler A identifier.
   */
  rtems_id scheduler_a_id;

  /**
   * @brief This member contains the scheduler B identifier.
   */
  rtems_id scheduler_b_id;

  /**
   * @brief This member contains the scheduler D identifier.
   */
  rtems_id scheduler_d_id;

  /**
   * @brief This member contains the worker task identifiers.
   */
  rtems_id worker_id[ 3 ];

  /**
   * @brief This member contains the mutex identifiers.
   */
  rtems_id mutex_id[ 2 ];

  /**
   * @brief If this member is true, then the task shall have an additional
   *   priority for the home scheduler.
   */
  bool additional_home_priority;

  /**
   * @brief If this member is true, then the task shall have a second eligible
   *   scheduler.
   */
  bool second_eligible_scheduler;

  /**
   * @brief If this member is true, then the task shall be pinned to a
   *   processor.
   */
  bool pinned;

  /**
   * @brief If this member is true, then the task shall be blocked.
   */
  bool blocked;

  /**
   * @brief If this member is true, then the task shall be enqueued on a thread
   *   queue.
   */
  bool enqueued;

  /**
   * @brief This member specifies the scheduler identifier to set.
   */
  rtems_id scheduler_to_set_id;

  /**
   * @brief If this member is true, then the affinity of the task shall be
   *   supported by the scheduler.
   */
  bool affinity_supported;

  /**
   * @brief This member contains the return value of the
   *   rtems_task_set_scheduler() call.
   */
  rtems_status_code status;

  /**
   * @brief This member specifies if the ``task_id`` parameter value.
   */
  rtems_id task_id;

  /**
   * @brief This member specifies if the ``scheduler_id`` parameter value.
   */
  rtems_id scheduler_id;

  /**
   * @brief This member specifies if the ``priority`` parameter value.
   */
  rtems_task_priority priority;

  /**
   * @brief This member contains the identifier of the new scheduler.
   */
  rtems_id new_scheduler;

  /**
   * @brief This member contains the new priorities of the task.
   */
  rtems_task_priority new_priority[ 2 ];

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 10 ];

    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 10 ];

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
    RtemsTaskReqSetScheduler_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsTaskReqSetScheduler_Context;

static RtemsTaskReqSetScheduler_Context
  RtemsTaskReqSetScheduler_Instance;

static const char * const RtemsTaskReqSetScheduler_PreDesc_TaskId[] = {
  "Task",
  "Invalid",
  "NA"
};

static const char * const RtemsTaskReqSetScheduler_PreDesc_Scheduler[] = {
  "Home",
  "Other",
  "NA"
};

static const char * const RtemsTaskReqSetScheduler_PreDesc_SchedulerHasCPU[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const RtemsTaskReqSetScheduler_PreDesc_SchedulerId[] = {
  "Scheduler",
  "Invalid",
  "NA"
};

static const char * const RtemsTaskReqSetScheduler_PreDesc_Priority[] = {
  "Valid",
  "Invalid",
  "NA"
};

static const char * const RtemsTaskReqSetScheduler_PreDesc_HomePriority[] = {
  "Real",
  "More",
  "NA"
};

static const char * const RtemsTaskReqSetScheduler_PreDesc_EligiblePriorities[] = {
  "OnlyOne",
  "More",
  "NA"
};

static const char * const RtemsTaskReqSetScheduler_PreDesc_Pinned[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const RtemsTaskReqSetScheduler_PreDesc_TaskState[] = {
  "Ready",
  "Blocked",
  "Enqueued",
  "NA"
};

static const char * const RtemsTaskReqSetScheduler_PreDesc_AffinitySupported[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const * const RtemsTaskReqSetScheduler_PreDesc[] = {
  RtemsTaskReqSetScheduler_PreDesc_TaskId,
  RtemsTaskReqSetScheduler_PreDesc_Scheduler,
  RtemsTaskReqSetScheduler_PreDesc_SchedulerHasCPU,
  RtemsTaskReqSetScheduler_PreDesc_SchedulerId,
  RtemsTaskReqSetScheduler_PreDesc_Priority,
  RtemsTaskReqSetScheduler_PreDesc_HomePriority,
  RtemsTaskReqSetScheduler_PreDesc_EligiblePriorities,
  RtemsTaskReqSetScheduler_PreDesc_Pinned,
  RtemsTaskReqSetScheduler_PreDesc_TaskState,
  RtemsTaskReqSetScheduler_PreDesc_AffinitySupported,
  NULL
};

typedef RtemsTaskReqSetScheduler_Context Context;

#define EVENT_OBTAIN_MUTEX_A RTEMS_EVENT_0

#define EVENT_RELEASE_MUTEX_A RTEMS_EVENT_1

#define EVENT_OBTAIN_MUTEX_B RTEMS_EVENT_2

#define EVENT_RELEASE_MUTEX_B RTEMS_EVENT_3

#define EVENT_PIN RTEMS_EVENT_4

#define EVENT_UNPIN RTEMS_EVENT_5

#define EVENT_SET_LOW_PRIO RTEMS_EVENT_6

#define EVENT_RUNNER_SYNC_0 RTEMS_EVENT_7

#define EVENT_RUNNER_SYNC_1 RTEMS_EVENT_8

static void Worker( rtems_task_argument arg )
{
  Context        *ctx;
  Thread_Control *executing;

  ctx = (Context *) arg;
  executing = _Thread_Get_executing();

  while ( true ) {
    rtems_event_set events;

    events = ReceiveAnyEvents();

    if ( ( events & EVENT_RUNNER_SYNC_0 ) != 0 ) {
      SendEvents( ctx->runner_id, EVENT_RUNNER_SYNC_0 );
    }

    if ( ( events & EVENT_OBTAIN_MUTEX_A ) != 0 ) {
      ObtainMutex( ctx->mutex_id[ 0 ] );
    }

    if ( ( events & EVENT_RELEASE_MUTEX_A ) != 0 ) {
      ReleaseMutex( ctx->mutex_id[ 0 ] );
    }

    if ( ( events & EVENT_OBTAIN_MUTEX_B ) != 0 ) {
      ObtainMutex( ctx->mutex_id[ 1 ] );
    }

    if ( ( events & EVENT_RELEASE_MUTEX_B ) != 0 ) {
      ReleaseMutex( ctx->mutex_id[ 1 ] );
    }

    if ( ( events & EVENT_PIN ) != 0 ) {
      _Thread_Pin( executing );
    }

    if ( ( events & EVENT_UNPIN ) != 0 ) {
      _Thread_Unpin( executing, _Per_CPU_Get_snapshot() );
    }

    if ( ( events & EVENT_SET_LOW_PRIO ) != 0 ) {
      SetSelfPriority( PRIO_LOW );
    }

    if ( ( events & EVENT_RUNNER_SYNC_1 ) != 0 ) {
      SendEvents( ctx->runner_id, EVENT_RUNNER_SYNC_1 );
    }
  }
}

static void RtemsTaskReqSetScheduler_Pre_TaskId_Prepare(
  RtemsTaskReqSetScheduler_Context   *ctx,
  RtemsTaskReqSetScheduler_Pre_TaskId state
)
{
  switch ( state ) {
    case RtemsTaskReqSetScheduler_Pre_TaskId_Task: {
      /*
       * While the ``task_id`` parameter is associated with a task.
       */
      ctx->task_id = ctx->worker_id[ 0 ];
      break;
    }

    case RtemsTaskReqSetScheduler_Pre_TaskId_Invalid: {
      /*
       * While the ``task_id`` parameter is not associated with a task.
       */
      ctx->task_id = INVALID_ID;
      break;
    }

    case RtemsTaskReqSetScheduler_Pre_TaskId_NA:
      break;
  }
}

static void RtemsTaskReqSetScheduler_Pre_Scheduler_Prepare(
  RtemsTaskReqSetScheduler_Context      *ctx,
  RtemsTaskReqSetScheduler_Pre_Scheduler state
)
{
  switch ( state ) {
    case RtemsTaskReqSetScheduler_Pre_Scheduler_Home: {
      /*
       * While the scheduler specified by the ``scheduler_id`` parameter is the
       * home scheduler of the task specified by the ``task_id`` parameter.
       */
      ctx->scheduler_to_set_id = ctx->scheduler_a_id;
      break;
    }

    case RtemsTaskReqSetScheduler_Pre_Scheduler_Other: {
      /*
       * While the scheduler specified by the ``scheduler_id`` parameter is not
       * the home scheduler of the task specified by the ``task_id`` parameter.
       */
      ctx->scheduler_to_set_id = ctx->scheduler_b_id;
      break;
    }

    case RtemsTaskReqSetScheduler_Pre_Scheduler_NA:
      break;
  }
}

static void RtemsTaskReqSetScheduler_Pre_SchedulerHasCPU_Prepare(
  RtemsTaskReqSetScheduler_Context            *ctx,
  RtemsTaskReqSetScheduler_Pre_SchedulerHasCPU state
)
{
  switch ( state ) {
    case RtemsTaskReqSetScheduler_Pre_SchedulerHasCPU_Yes: {
      /*
       * While the scheduler specified by the ``scheduler_id`` parameter owns
       * at least one processor.
       */
      /* Already set by Scheduler pre-condition */
      break;
    }

    case RtemsTaskReqSetScheduler_Pre_SchedulerHasCPU_No: {
      /*
       * While the scheduler specified by the ``scheduler_id`` parameter owns
       * no processor.
       */
      ctx->scheduler_to_set_id = ctx->scheduler_d_id;
      break;
    }

    case RtemsTaskReqSetScheduler_Pre_SchedulerHasCPU_NA:
      break;
  }
}

static void RtemsTaskReqSetScheduler_Pre_SchedulerId_Prepare(
  RtemsTaskReqSetScheduler_Context        *ctx,
  RtemsTaskReqSetScheduler_Pre_SchedulerId state
)
{
  switch ( state ) {
    case RtemsTaskReqSetScheduler_Pre_SchedulerId_Scheduler: {
      /*
       * While the ``scheduler_id`` parameter is associated with a scheduler.
       */
      ctx->scheduler_id = ctx->scheduler_to_set_id;
      break;
    }

    case RtemsTaskReqSetScheduler_Pre_SchedulerId_Invalid: {
      /*
       * While the ``scheduler_id`` parameter is not associated with a
       * scheduler.
       */
      ctx->scheduler_id = INVALID_ID;
      break;
    }

    case RtemsTaskReqSetScheduler_Pre_SchedulerId_NA:
      break;
  }
}

static void RtemsTaskReqSetScheduler_Pre_Priority_Prepare(
  RtemsTaskReqSetScheduler_Context     *ctx,
  RtemsTaskReqSetScheduler_Pre_Priority state
)
{
  switch ( state ) {
    case RtemsTaskReqSetScheduler_Pre_Priority_Valid: {
      /*
       * While the task priority specified by the ``priority`` parameter is
       * valid with respect to the scheduler specified by the ``scheduler_id``
       * parameter.
       */
      ctx->priority = PRIO_VERY_LOW;
      break;
    }

    case RtemsTaskReqSetScheduler_Pre_Priority_Invalid: {
      /*
       * While the task priority specified by the ``priority`` parameter is
       * invalid with respect to the scheduler specified by the
       * ``scheduler_id`` parameter.
       */
      ctx->priority = PRIO_INVALID;
      break;
    }

    case RtemsTaskReqSetScheduler_Pre_Priority_NA:
      break;
  }
}

static void RtemsTaskReqSetScheduler_Pre_HomePriority_Prepare(
  RtemsTaskReqSetScheduler_Context         *ctx,
  RtemsTaskReqSetScheduler_Pre_HomePriority state
)
{
  switch ( state ) {
    case RtemsTaskReqSetScheduler_Pre_HomePriority_Real: {
      /*
       * While the current priority of the task specified by the ``task_id``
       * parameter consists only of the real priority.
       */
      ctx->additional_home_priority = false;
      break;
    }

    case RtemsTaskReqSetScheduler_Pre_HomePriority_More: {
      /*
       * While the current priority of the task specified by the ``task_id``
       * parameter consists of more than the real priority.
       */
      ctx->additional_home_priority = true;
      break;
    }

    case RtemsTaskReqSetScheduler_Pre_HomePriority_NA:
      break;
  }
}

static void RtemsTaskReqSetScheduler_Pre_EligiblePriorities_Prepare(
  RtemsTaskReqSetScheduler_Context               *ctx,
  RtemsTaskReqSetScheduler_Pre_EligiblePriorities state
)
{
  switch ( state ) {
    case RtemsTaskReqSetScheduler_Pre_EligiblePriorities_OnlyOne: {
      /*
       * While the set of eligible priorities of the task specified by the
       * ``task_id`` parameter consists of exactly the current priority.
       */
      ctx->second_eligible_scheduler = false;
      break;
    }

    case RtemsTaskReqSetScheduler_Pre_EligiblePriorities_More: {
      /*
       * While the set of eligible priorities of the task specified by the
       * ``task_id`` parameter consists of more than the current priority.
       */
      ctx->second_eligible_scheduler = true;
      break;
    }

    case RtemsTaskReqSetScheduler_Pre_EligiblePriorities_NA:
      break;
  }
}

static void RtemsTaskReqSetScheduler_Pre_Pinned_Prepare(
  RtemsTaskReqSetScheduler_Context   *ctx,
  RtemsTaskReqSetScheduler_Pre_Pinned state
)
{
  switch ( state ) {
    case RtemsTaskReqSetScheduler_Pre_Pinned_Yes: {
      /*
       * While the task specified by the ``task_id`` parameter is pinned.
       */
      ctx->pinned = true;
      break;
    }

    case RtemsTaskReqSetScheduler_Pre_Pinned_No: {
      /*
       * While the task specified by the ``task_id`` parameter is not pinned.
       */
      ctx->pinned = false;
      break;
    }

    case RtemsTaskReqSetScheduler_Pre_Pinned_NA:
      break;
  }
}

static void RtemsTaskReqSetScheduler_Pre_TaskState_Prepare(
  RtemsTaskReqSetScheduler_Context      *ctx,
  RtemsTaskReqSetScheduler_Pre_TaskState state
)
{
  switch ( state ) {
    case RtemsTaskReqSetScheduler_Pre_TaskState_Ready: {
      /*
       * While the task specified by the ``task_id`` parameter is ready.
       */
      ctx->blocked = false;
      ctx->enqueued = false;
      break;
    }

    case RtemsTaskReqSetScheduler_Pre_TaskState_Blocked: {
      /*
       * While the task specified by the ``task_id`` parameter is blocked,
       * while the task specified by the ``task_id`` parameter is not enqueued
       * on a wait queue.
       */
      ctx->blocked = true;
      ctx->enqueued = false;
      break;
    }

    case RtemsTaskReqSetScheduler_Pre_TaskState_Enqueued: {
      /*
       * While the task specified by the ``task_id`` parameter is blocked,
       * while the task specified by the ``task_id`` parameter is enqueued on a
       * wait queue.
       */
      ctx->blocked = true;
      ctx->enqueued = true;
      break;
    }

    case RtemsTaskReqSetScheduler_Pre_TaskState_NA:
      break;
  }
}

static void RtemsTaskReqSetScheduler_Pre_AffinitySupported_Prepare(
  RtemsTaskReqSetScheduler_Context              *ctx,
  RtemsTaskReqSetScheduler_Pre_AffinitySupported state
)
{
  switch ( state ) {
    case RtemsTaskReqSetScheduler_Pre_AffinitySupported_Yes: {
      /*
       * While the affinity set of the task specified by the ``task_id``
       * parameter is supported by the scheduler specified by the
       * ``scheduler_id`` parameter.
       */
      ctx->affinity_supported = true;
      break;
    }

    case RtemsTaskReqSetScheduler_Pre_AffinitySupported_No: {
      /*
       * While the affinity set of the task specified by the ``task_id``
       * parameter is not supported by the scheduler specified by the
       * ``scheduler_id`` parameter.
       */
      ctx->affinity_supported = false;
      break;
    }

    case RtemsTaskReqSetScheduler_Pre_AffinitySupported_NA:
      break;
  }
}

static void RtemsTaskReqSetScheduler_Post_Status_Check(
  RtemsTaskReqSetScheduler_Context    *ctx,
  RtemsTaskReqSetScheduler_Post_Status state
)
{
  switch ( state ) {
    case RtemsTaskReqSetScheduler_Post_Status_Ok: {
      /*
       * The return status of rtems_task_set_scheduler() shall be
       * RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsTaskReqSetScheduler_Post_Status_InvAddr: {
      /*
       * The return status of rtems_task_set_scheduler() shall be
       * RTEMS_INVALID_ADDRESS.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ADDRESS );
      break;
    }

    case RtemsTaskReqSetScheduler_Post_Status_InvId: {
      /*
       * The return status of rtems_task_set_scheduler() shall be
       * RTEMS_INVALID_ID.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ID );
      break;
    }

    case RtemsTaskReqSetScheduler_Post_Status_InvPrio: {
      /*
       * The return status of rtems_task_set_scheduler() shall be
       * RTEMS_INVALID_PRIORITY.
       */
      T_rsc( ctx->status, RTEMS_INVALID_PRIORITY );
      break;
    }

    case RtemsTaskReqSetScheduler_Post_Status_InUse: {
      /*
       * The return status of rtems_task_set_scheduler() shall be
       * RTEMS_RESOURCE_IN_USE.
       */
      T_rsc( ctx->status, RTEMS_RESOURCE_IN_USE );
      break;
    }

    case RtemsTaskReqSetScheduler_Post_Status_Unsat: {
      /*
       * The return status of rtems_task_set_scheduler() shall be
       * RTEMS_UNSATISFIED.
       */
      T_rsc( ctx->status, RTEMS_UNSATISFIED );
      break;
    }

    case RtemsTaskReqSetScheduler_Post_Status_NA:
      break;
  }
}

static void RtemsTaskReqSetScheduler_Post_Scheduler_Check(
  RtemsTaskReqSetScheduler_Context       *ctx,
  RtemsTaskReqSetScheduler_Post_Scheduler state
)
{
  switch ( state ) {
    case RtemsTaskReqSetScheduler_Post_Scheduler_Set: {
      /*
       * The home scheduler of the task specified by the ``task_id`` parameter
       * shall be set to the scheduler specified by the ``scheduler_id``
       * parameter at some point during the rtems_task_set_scheduler() call.
       */
      T_eq_u32( ctx->new_scheduler, ctx->scheduler_to_set_id );
      break;
    }

    case RtemsTaskReqSetScheduler_Post_Scheduler_Nop: {
      /*
       * No home scheduler of a task shall be modified by the
       * rtems_task_set_scheduler() call.
       */
      T_eq_u32( ctx->new_scheduler, ctx->scheduler_a_id );
      break;
    }

    case RtemsTaskReqSetScheduler_Post_Scheduler_NA:
      break;
  }
}

static void RtemsTaskReqSetScheduler_Post_Priority_Check(
  RtemsTaskReqSetScheduler_Context      *ctx,
  RtemsTaskReqSetScheduler_Post_Priority state
)
{
  switch ( state ) {
    case RtemsTaskReqSetScheduler_Post_Priority_Set: {
      /*
       * The real priority of the task specified by the ``task_id`` parameter
       * shall be set to the priority specified by the ``priority`` parameter
       * at some point during the rtems_task_set_scheduler() call.
       */
      if ( ctx->scheduler_to_set_id == ctx->scheduler_a_id ) {
        T_eq_u32( ctx->new_priority[ 0 ], PRIO_VERY_LOW );
        T_eq_u32( ctx->new_priority[ 1 ], PRIO_INVALID );
      } else {
        T_eq_u32( ctx->new_priority[ 0 ], PRIO_INVALID );
        T_eq_u32( ctx->new_priority[ 1 ], PRIO_VERY_LOW );
      }
      break;
    }

    case RtemsTaskReqSetScheduler_Post_Priority_Nop: {
      /*
       * No task priority shall be modified by the rtems_task_set_scheduler()
       * call.
       */
      if ( ctx->blocked ) {
        T_eq_u32( ctx->new_priority[ 0 ], PRIO_HIGH );
      } else {
        T_eq_u32( ctx->new_priority[ 0 ], PRIO_LOW );
      }

      T_eq_u32( ctx->new_priority[ 1 ], PRIO_INVALID );
      break;
    }

    case RtemsTaskReqSetScheduler_Post_Priority_NA:
      break;
  }
}

static void RtemsTaskReqSetScheduler_Setup(
  RtemsTaskReqSetScheduler_Context *ctx
)
{
  rtems_status_code sc;
  size_t            i;

  memset( ctx, 0, sizeof( *ctx ) );
  ctx->runner_id = rtems_task_self();
  SetSelfPriority( PRIO_NORMAL );

  sc = rtems_scheduler_ident(
    TEST_SCHEDULER_A_NAME,
    &ctx->scheduler_a_id
  );
  T_rsc_success( sc );

  for ( i = 0; i < RTEMS_ARRAY_SIZE( ctx->mutex_id ); ++i ) {
    ctx->mutex_id[ i ] = CreateMutex();
  }

  for ( i = 0; i < RTEMS_ARRAY_SIZE( ctx->worker_id ); ++i ) {
    ctx->worker_id[ i ] = CreateTask( "WORK", PRIO_HIGH - i );
    StartTask( ctx->worker_id[ i ], Worker, ctx );
  }

  #if defined(RTEMS_SMP)
  sc = rtems_scheduler_ident( TEST_SCHEDULER_B_NAME, &ctx->scheduler_b_id );
  T_rsc_success( sc );

  sc = rtems_scheduler_ident( TEST_SCHEDULER_D_NAME, &ctx->scheduler_d_id );
  T_rsc_success( sc );

  SetScheduler( ctx->worker_id[ 2 ], ctx->scheduler_b_id, PRIO_NORMAL );
  #else
  ctx->scheduler_b_id = INVALID_ID;
  #endif
}

static void RtemsTaskReqSetScheduler_Setup_Wrap( void *arg )
{
  RtemsTaskReqSetScheduler_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsTaskReqSetScheduler_Setup( ctx );
}

static void RtemsTaskReqSetScheduler_Teardown(
  RtemsTaskReqSetScheduler_Context *ctx
)
{
  size_t i;

  for ( i = 0; i < RTEMS_ARRAY_SIZE( ctx->worker_id ); ++i ) {
    DeleteTask( ctx->worker_id[ i ] );
  }

  for ( i = 0; i < RTEMS_ARRAY_SIZE( ctx->mutex_id ); ++i ) {
    DeleteMutex( ctx->mutex_id[ i ] );
  }

  RestoreRunnerPriority();
}

static void RtemsTaskReqSetScheduler_Teardown_Wrap( void *arg )
{
  RtemsTaskReqSetScheduler_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsTaskReqSetScheduler_Teardown( ctx );
}

static void RtemsTaskReqSetScheduler_Action(
  RtemsTaskReqSetScheduler_Context *ctx
)
{
  rtems_status_code sc;

  if ( ctx->additional_home_priority || ctx->second_eligible_scheduler ) {
    SendEvents( ctx->worker_id[ 0 ], EVENT_OBTAIN_MUTEX_A );

    if ( ctx->additional_home_priority ) {
      SendEvents( ctx->worker_id[ 1 ], EVENT_OBTAIN_MUTEX_A );
    }

    if ( ctx->second_eligible_scheduler ) {
      SendEvents(
        ctx->worker_id[ 2 ],
        EVENT_RUNNER_SYNC_0 | EVENT_OBTAIN_MUTEX_A
      );
      ReceiveAllEvents( EVENT_RUNNER_SYNC_0 );
      WaitForExecutionStop( ctx->worker_id[ 2 ] );
    }
  }

  if ( ctx->blocked && ctx->enqueued ) {
    ObtainMutex( ctx->mutex_id[ 1 ] );
    SendEvents( ctx->worker_id[ 0 ], EVENT_OBTAIN_MUTEX_B );
  }

  if ( !ctx->affinity_supported ) {
    SetAffinityOne( ctx->worker_id[ 0 ], 0 );
  }

  if ( ctx->pinned ) {
    SendEvents( ctx->worker_id[ 0 ], EVENT_PIN );
  }

  if ( !ctx->blocked ) {
    SendEvents( ctx->worker_id[ 0 ], EVENT_SET_LOW_PRIO );
  }

  ctx->status = rtems_task_set_scheduler(
    ctx->task_id,
    ctx->scheduler_id,
    ctx->priority
  );

  ctx->new_scheduler = GetScheduler( ctx->worker_id[ 0 ] );

  if ( ctx->pinned ) {
    SendEvents( ctx->worker_id[ 0 ], EVENT_UNPIN );
  }

  if ( !ctx->affinity_supported ) {
    SetAffinityAll( ctx->worker_id[ 0 ] );
  }

  if ( ctx->blocked && ctx->enqueued ) {
    ReleaseMutex( ctx->mutex_id[ 1 ] );
    SendEvents( ctx->worker_id[ 0 ], EVENT_RELEASE_MUTEX_B );
  }

  if ( ctx->additional_home_priority || ctx->second_eligible_scheduler ) {
    SendEvents( ctx->worker_id[ 0 ], EVENT_RELEASE_MUTEX_A );

    if ( ctx->additional_home_priority ) {
      SendEvents( ctx->worker_id[ 1 ], EVENT_RELEASE_MUTEX_A );
    }

    if ( ctx->second_eligible_scheduler ) {
      SendEvents(
        ctx->worker_id[ 2 ],
        EVENT_RELEASE_MUTEX_A | EVENT_RUNNER_SYNC_1
      );
      ReceiveAllEvents( EVENT_RUNNER_SYNC_1 );
    }
  }

  sc = rtems_task_get_priority(
    ctx->worker_id[ 0 ],
    ctx->scheduler_a_id,
    &ctx->new_priority[ 0 ]
  );

  if ( sc == RTEMS_NOT_DEFINED ) {
    ctx->new_priority[ 0 ] = PRIO_INVALID;
  } else {
    T_rsc_success( sc );
  }

  #if defined(RTEMS_SMP)
  sc = rtems_task_get_priority(
    ctx->worker_id[ 0 ],
    ctx->scheduler_b_id,
    &ctx->new_priority[ 1 ]
  );

  if ( sc == RTEMS_NOT_DEFINED ) {
    ctx->new_priority[ 1 ] = PRIO_INVALID;
  } else {
    T_rsc_success( sc );
  }
  #else
  ctx->new_priority[ 1 ] = PRIO_INVALID;
  #endif

  if ( ctx->status == RTEMS_SUCCESSFUL ) {
    SetScheduler( ctx->worker_id[ 0 ], ctx->scheduler_a_id, PRIO_HIGH );
  } else if ( !ctx->blocked ) {
    SetPriority( ctx->worker_id[ 0 ], PRIO_HIGH );
  }
}

static const RtemsTaskReqSetScheduler_Entry
RtemsTaskReqSetScheduler_Entries[] = {
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqSetScheduler_Post_Status_NA,
    RtemsTaskReqSetScheduler_Post_Scheduler_NA,
    RtemsTaskReqSetScheduler_Post_Priority_NA },
#else
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqSetScheduler_Post_Status_NA,
    RtemsTaskReqSetScheduler_Post_Scheduler_NA,
    RtemsTaskReqSetScheduler_Post_Priority_NA },
#endif
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqSetScheduler_Post_Status_NA,
    RtemsTaskReqSetScheduler_Post_Scheduler_NA,
    RtemsTaskReqSetScheduler_Post_Priority_NA },
#else
  { 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1,
    RtemsTaskReqSetScheduler_Post_Status_InvId,
    RtemsTaskReqSetScheduler_Post_Scheduler_Nop,
    RtemsTaskReqSetScheduler_Post_Priority_Nop },
#endif
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqSetScheduler_Post_Status_NA,
    RtemsTaskReqSetScheduler_Post_Scheduler_NA,
    RtemsTaskReqSetScheduler_Post_Priority_NA },
#else
  { 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1,
    RtemsTaskReqSetScheduler_Post_Status_InvId,
    RtemsTaskReqSetScheduler_Post_Scheduler_Nop,
    RtemsTaskReqSetScheduler_Post_Priority_Nop },
#endif
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqSetScheduler_Post_Status_NA,
    RtemsTaskReqSetScheduler_Post_Scheduler_NA,
    RtemsTaskReqSetScheduler_Post_Priority_NA },
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqSetScheduler_Post_Status_NA,
    RtemsTaskReqSetScheduler_Post_Scheduler_NA,
    RtemsTaskReqSetScheduler_Post_Priority_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqSetScheduler_Post_Status_InvPrio,
    RtemsTaskReqSetScheduler_Post_Scheduler_Nop,
    RtemsTaskReqSetScheduler_Post_Priority_Nop },
#endif
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqSetScheduler_Post_Status_NA,
    RtemsTaskReqSetScheduler_Post_Scheduler_NA,
    RtemsTaskReqSetScheduler_Post_Priority_NA },
#else
  { 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0,
    RtemsTaskReqSetScheduler_Post_Status_InvId,
    RtemsTaskReqSetScheduler_Post_Scheduler_Nop,
    RtemsTaskReqSetScheduler_Post_Priority_Nop },
#endif
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqSetScheduler_Post_Status_NA,
    RtemsTaskReqSetScheduler_Post_Scheduler_NA,
    RtemsTaskReqSetScheduler_Post_Priority_NA },
#else
  { 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0,
    RtemsTaskReqSetScheduler_Post_Status_InvPrio,
    RtemsTaskReqSetScheduler_Post_Scheduler_Nop,
    RtemsTaskReqSetScheduler_Post_Priority_Nop },
#endif
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqSetScheduler_Post_Status_NA,
    RtemsTaskReqSetScheduler_Post_Scheduler_NA,
    RtemsTaskReqSetScheduler_Post_Priority_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqSetScheduler_Post_Status_InUse,
    RtemsTaskReqSetScheduler_Post_Scheduler_Nop,
    RtemsTaskReqSetScheduler_Post_Priority_Nop },
#endif
  { 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1,
    RtemsTaskReqSetScheduler_Post_Status_InvId,
    RtemsTaskReqSetScheduler_Post_Scheduler_Nop,
    RtemsTaskReqSetScheduler_Post_Priority_Nop },
  { 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1,
    RtemsTaskReqSetScheduler_Post_Status_InvId,
    RtemsTaskReqSetScheduler_Post_Scheduler_Nop,
    RtemsTaskReqSetScheduler_Post_Priority_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqSetScheduler_Post_Status_InvPrio,
    RtemsTaskReqSetScheduler_Post_Scheduler_Nop,
    RtemsTaskReqSetScheduler_Post_Priority_Nop },
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqSetScheduler_Post_Status_NA,
    RtemsTaskReqSetScheduler_Post_Scheduler_NA,
    RtemsTaskReqSetScheduler_Post_Priority_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqSetScheduler_Post_Status_Unsat,
    RtemsTaskReqSetScheduler_Post_Scheduler_Nop,
    RtemsTaskReqSetScheduler_Post_Priority_Nop },
#endif
  { 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0,
    RtemsTaskReqSetScheduler_Post_Status_InvId,
    RtemsTaskReqSetScheduler_Post_Scheduler_Nop,
    RtemsTaskReqSetScheduler_Post_Priority_Nop },
  { 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0,
    RtemsTaskReqSetScheduler_Post_Status_InvPrio,
    RtemsTaskReqSetScheduler_Post_Scheduler_Nop,
    RtemsTaskReqSetScheduler_Post_Priority_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqSetScheduler_Post_Status_InUse,
    RtemsTaskReqSetScheduler_Post_Scheduler_Nop,
    RtemsTaskReqSetScheduler_Post_Priority_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqSetScheduler_Post_Status_Ok,
    RtemsTaskReqSetScheduler_Post_Scheduler_Set,
    RtemsTaskReqSetScheduler_Post_Priority_Set },
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqSetScheduler_Post_Status_NA,
    RtemsTaskReqSetScheduler_Post_Scheduler_NA,
    RtemsTaskReqSetScheduler_Post_Priority_NA }
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqSetScheduler_Post_Status_Ok,
    RtemsTaskReqSetScheduler_Post_Scheduler_Set,
    RtemsTaskReqSetScheduler_Post_Priority_Set }
#endif
};

static const uint8_t
RtemsTaskReqSetScheduler_Map[] = {
  7, 0, 7, 0, 7, 0, 15, 3, 15, 3, 14, 3, 7, 0, 7, 0, 7, 0, 7, 0, 7, 0, 7, 0, 7,
  0, 7, 0, 7, 0, 14, 3, 14, 3, 14, 3, 7, 0, 7, 0, 7, 0, 7, 0, 7, 0, 7, 0, 4, 0,
  4, 0, 4, 0, 10, 3, 10, 3, 10, 3, 4, 0, 4, 0, 4, 0, 4, 0, 4, 0, 4, 0, 4, 0, 4,
  0, 4, 0, 10, 3, 10, 3, 10, 3, 4, 0, 4, 0, 4, 0, 4, 0, 4, 0, 4, 0, 1, 0, 1, 0,
  1, 0, 8, 3, 8, 3, 8, 3, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0,
  8, 3, 8, 3, 8, 3, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 8, 3,
  8, 3, 8, 3, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 8, 3, 8, 3,
  8, 3, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7,
  7, 7, 7, 7, 16, 11, 16, 11, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 7, 7, 7, 7, 7, 7, 11, 11, 11, 11, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 0, 5, 0,
  5, 0, 12, 3, 12, 3, 12, 3, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 5,
  0, 12, 3, 12, 3, 12, 3, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 6, 0, 6, 0, 6, 0,
  13, 3, 13, 3, 13, 3, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0,
  13, 3, 13, 3, 13, 3, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 2, 0, 2, 0, 2, 0, 9,
  3, 9, 3, 9, 3, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 9, 3, 9,
  3, 9, 3, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 9, 3, 9, 3, 9,
  3, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 9, 3, 9, 3, 9, 3, 2,
  0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
};

static size_t RtemsTaskReqSetScheduler_Scope( void *arg, char *buf, size_t n )
{
  RtemsTaskReqSetScheduler_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      RtemsTaskReqSetScheduler_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture RtemsTaskReqSetScheduler_Fixture = {
  .setup = RtemsTaskReqSetScheduler_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsTaskReqSetScheduler_Teardown_Wrap,
  .scope = RtemsTaskReqSetScheduler_Scope,
  .initial_context = &RtemsTaskReqSetScheduler_Instance
};

static inline RtemsTaskReqSetScheduler_Entry RtemsTaskReqSetScheduler_PopEntry(
  RtemsTaskReqSetScheduler_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsTaskReqSetScheduler_Entries[
    RtemsTaskReqSetScheduler_Map[ index ]
  ];
}

static void RtemsTaskReqSetScheduler_SetPreConditionStates(
  RtemsTaskReqSetScheduler_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Scheduler_NA ) {
    ctx->Map.pcs[ 1 ] = RtemsTaskReqSetScheduler_Pre_Scheduler_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  if ( ctx->Map.entry.Pre_SchedulerHasCPU_NA ) {
    ctx->Map.pcs[ 2 ] = RtemsTaskReqSetScheduler_Pre_SchedulerHasCPU_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }

  ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];

  if ( ctx->Map.entry.Pre_Priority_NA ) {
    ctx->Map.pcs[ 4 ] = RtemsTaskReqSetScheduler_Pre_Priority_NA;
  } else {
    ctx->Map.pcs[ 4 ] = ctx->Map.pci[ 4 ];
  }

  if ( ctx->Map.entry.Pre_HomePriority_NA ) {
    ctx->Map.pcs[ 5 ] = RtemsTaskReqSetScheduler_Pre_HomePriority_NA;
  } else {
    ctx->Map.pcs[ 5 ] = ctx->Map.pci[ 5 ];
  }

  if ( ctx->Map.entry.Pre_EligiblePriorities_NA ) {
    ctx->Map.pcs[ 6 ] = RtemsTaskReqSetScheduler_Pre_EligiblePriorities_NA;
  } else {
    ctx->Map.pcs[ 6 ] = ctx->Map.pci[ 6 ];
  }

  if ( ctx->Map.entry.Pre_Pinned_NA ) {
    ctx->Map.pcs[ 7 ] = RtemsTaskReqSetScheduler_Pre_Pinned_NA;
  } else {
    ctx->Map.pcs[ 7 ] = ctx->Map.pci[ 7 ];
  }

  if ( ctx->Map.entry.Pre_TaskState_NA ) {
    ctx->Map.pcs[ 8 ] = RtemsTaskReqSetScheduler_Pre_TaskState_NA;
  } else {
    ctx->Map.pcs[ 8 ] = ctx->Map.pci[ 8 ];
  }

  if ( ctx->Map.entry.Pre_AffinitySupported_NA ) {
    ctx->Map.pcs[ 9 ] = RtemsTaskReqSetScheduler_Pre_AffinitySupported_NA;
  } else {
    ctx->Map.pcs[ 9 ] = ctx->Map.pci[ 9 ];
  }
}

static void RtemsTaskReqSetScheduler_TestVariant(
  RtemsTaskReqSetScheduler_Context *ctx
)
{
  RtemsTaskReqSetScheduler_Pre_TaskId_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsTaskReqSetScheduler_Pre_Scheduler_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsTaskReqSetScheduler_Pre_SchedulerHasCPU_Prepare(
    ctx,
    ctx->Map.pcs[ 2 ]
  );
  RtemsTaskReqSetScheduler_Pre_SchedulerId_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  RtemsTaskReqSetScheduler_Pre_Priority_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  RtemsTaskReqSetScheduler_Pre_HomePriority_Prepare( ctx, ctx->Map.pcs[ 5 ] );
  RtemsTaskReqSetScheduler_Pre_EligiblePriorities_Prepare(
    ctx,
    ctx->Map.pcs[ 6 ]
  );
  RtemsTaskReqSetScheduler_Pre_Pinned_Prepare( ctx, ctx->Map.pcs[ 7 ] );
  RtemsTaskReqSetScheduler_Pre_TaskState_Prepare( ctx, ctx->Map.pcs[ 8 ] );
  RtemsTaskReqSetScheduler_Pre_AffinitySupported_Prepare(
    ctx,
    ctx->Map.pcs[ 9 ]
  );
  RtemsTaskReqSetScheduler_Action( ctx );
  RtemsTaskReqSetScheduler_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  RtemsTaskReqSetScheduler_Post_Scheduler_Check(
    ctx,
    ctx->Map.entry.Post_Scheduler
  );
  RtemsTaskReqSetScheduler_Post_Priority_Check(
    ctx,
    ctx->Map.entry.Post_Priority
  );
}

/**
 * @fn void T_case_body_RtemsTaskReqSetScheduler( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsTaskReqSetScheduler,
  &RtemsTaskReqSetScheduler_Fixture
)
{
  RtemsTaskReqSetScheduler_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = RtemsTaskReqSetScheduler_Pre_TaskId_Task;
    ctx->Map.pci[ 0 ] < RtemsTaskReqSetScheduler_Pre_TaskId_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = RtemsTaskReqSetScheduler_Pre_Scheduler_Home;
      ctx->Map.pci[ 1 ] < RtemsTaskReqSetScheduler_Pre_Scheduler_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = RtemsTaskReqSetScheduler_Pre_SchedulerHasCPU_Yes;
        ctx->Map.pci[ 2 ] < RtemsTaskReqSetScheduler_Pre_SchedulerHasCPU_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = RtemsTaskReqSetScheduler_Pre_SchedulerId_Scheduler;
          ctx->Map.pci[ 3 ] < RtemsTaskReqSetScheduler_Pre_SchedulerId_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          for (
            ctx->Map.pci[ 4 ] = RtemsTaskReqSetScheduler_Pre_Priority_Valid;
            ctx->Map.pci[ 4 ] < RtemsTaskReqSetScheduler_Pre_Priority_NA;
            ++ctx->Map.pci[ 4 ]
          ) {
            for (
              ctx->Map.pci[ 5 ] = RtemsTaskReqSetScheduler_Pre_HomePriority_Real;
              ctx->Map.pci[ 5 ] < RtemsTaskReqSetScheduler_Pre_HomePriority_NA;
              ++ctx->Map.pci[ 5 ]
            ) {
              for (
                ctx->Map.pci[ 6 ] = RtemsTaskReqSetScheduler_Pre_EligiblePriorities_OnlyOne;
                ctx->Map.pci[ 6 ] < RtemsTaskReqSetScheduler_Pre_EligiblePriorities_NA;
                ++ctx->Map.pci[ 6 ]
              ) {
                for (
                  ctx->Map.pci[ 7 ] = RtemsTaskReqSetScheduler_Pre_Pinned_Yes;
                  ctx->Map.pci[ 7 ] < RtemsTaskReqSetScheduler_Pre_Pinned_NA;
                  ++ctx->Map.pci[ 7 ]
                ) {
                  for (
                    ctx->Map.pci[ 8 ] = RtemsTaskReqSetScheduler_Pre_TaskState_Ready;
                    ctx->Map.pci[ 8 ] < RtemsTaskReqSetScheduler_Pre_TaskState_NA;
                    ++ctx->Map.pci[ 8 ]
                  ) {
                    for (
                      ctx->Map.pci[ 9 ] = RtemsTaskReqSetScheduler_Pre_AffinitySupported_Yes;
                      ctx->Map.pci[ 9 ] < RtemsTaskReqSetScheduler_Pre_AffinitySupported_NA;
                      ++ctx->Map.pci[ 9 ]
                    ) {
                      ctx->Map.entry = RtemsTaskReqSetScheduler_PopEntry(
                        ctx
                      );

                      if ( ctx->Map.entry.Skip ) {
                        continue;
                      }

                      RtemsTaskReqSetScheduler_SetPreConditionStates( ctx );
                      RtemsTaskReqSetScheduler_TestVariant( ctx );
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}

/** @} */
