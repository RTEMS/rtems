/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsSemReqMrspObtain
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
#include <string.h>
#include <rtems/score/percpu.h>
#include <rtems/score/threadimpl.h>

#include "tx-support.h"
#include "tx-thread-queue.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsSemReqMrspObtain spec:/rtems/sem/req/mrsp-obtain
 *
 * @ingroup TestsuitesValidationSmpOnly0
 *
 * @{
 */

typedef enum {
  RtemsSemReqMrspObtain_Pre_Home_Idle,
  RtemsSemReqMrspObtain_Pre_Home_Task,
  RtemsSemReqMrspObtain_Pre_Home_TaskIdle,
  RtemsSemReqMrspObtain_Pre_Home_Second,
  RtemsSemReqMrspObtain_Pre_Home_SecondIdle,
  RtemsSemReqMrspObtain_Pre_Home_NA
} RtemsSemReqMrspObtain_Pre_Home;

typedef enum {
  RtemsSemReqMrspObtain_Pre_Helping_Idle,
  RtemsSemReqMrspObtain_Pre_Helping_Task,
  RtemsSemReqMrspObtain_Pre_Helping_Helping,
  RtemsSemReqMrspObtain_Pre_Helping_HelpingIdle,
  RtemsSemReqMrspObtain_Pre_Helping_Third,
  RtemsSemReqMrspObtain_Pre_Helping_ThirdIdle,
  RtemsSemReqMrspObtain_Pre_Helping_NA
} RtemsSemReqMrspObtain_Pre_Helping;

typedef enum {
  RtemsSemReqMrspObtain_Pre_PriorityHome_None,
  RtemsSemReqMrspObtain_Pre_PriorityHome_NewHigh,
  RtemsSemReqMrspObtain_Pre_PriorityHome_NewEqual,
  RtemsSemReqMrspObtain_Pre_PriorityHome_SecondHigh,
  RtemsSemReqMrspObtain_Pre_PriorityHome_SecondEqual,
  RtemsSemReqMrspObtain_Pre_PriorityHome_SecondLow,
  RtemsSemReqMrspObtain_Pre_PriorityHome_NA
} RtemsSemReqMrspObtain_Pre_PriorityHome;

typedef enum {
  RtemsSemReqMrspObtain_Pre_PriorityHelping_None,
  RtemsSemReqMrspObtain_Pre_PriorityHelping_Helping,
  RtemsSemReqMrspObtain_Pre_PriorityHelping_ThirdHigh,
  RtemsSemReqMrspObtain_Pre_PriorityHelping_ThirdEqual,
  RtemsSemReqMrspObtain_Pre_PriorityHelping_ThirdLow,
  RtemsSemReqMrspObtain_Pre_PriorityHelping_NA
} RtemsSemReqMrspObtain_Pre_PriorityHelping;

typedef enum {
  RtemsSemReqMrspObtain_Post_Home_Task,
  RtemsSemReqMrspObtain_Post_Home_TaskIdle,
  RtemsSemReqMrspObtain_Post_Home_Second,
  RtemsSemReqMrspObtain_Post_Home_SecondIdle,
  RtemsSemReqMrspObtain_Post_Home_NA
} RtemsSemReqMrspObtain_Post_Home;

typedef enum {
  RtemsSemReqMrspObtain_Post_Helping_Idle,
  RtemsSemReqMrspObtain_Post_Helping_Task,
  RtemsSemReqMrspObtain_Post_Helping_TaskIdle,
  RtemsSemReqMrspObtain_Post_Helping_Helping,
  RtemsSemReqMrspObtain_Post_Helping_HelpingIdle,
  RtemsSemReqMrspObtain_Post_Helping_Third,
  RtemsSemReqMrspObtain_Post_Helping_ThirdIdle,
  RtemsSemReqMrspObtain_Post_Helping_NA
} RtemsSemReqMrspObtain_Post_Helping;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Home_NA : 1;
  uint16_t Pre_Helping_NA : 1;
  uint16_t Pre_PriorityHome_NA : 1;
  uint16_t Pre_PriorityHelping_NA : 1;
  uint16_t Post_Home : 3;
  uint16_t Post_Helping : 3;
} RtemsSemReqMrspObtain_Entry;

/**
 * @brief Test context for spec:/rtems/sem/req/mrsp-obtain test case.
 */
typedef struct {
  /**
   * @brief This member contains the thread queue test context.
   */
  TQContext tq_ctx;

  /**
   * @brief This member contains the MrsP semaphore to obtain.
   */
  rtems_id sema_id;

  /**
   * @brief This member specifies the scheduler on which the task executes.
   */
  rtems_id task_scheduler;

  /**
   * @brief If this member is true, then the task shall already own a MrsP
   *   semaphore.
   */
  bool task_owns_mrsp_semaphore;

  /**
   * @brief If this member is true, then an idle task shall execute on
   *   scheduler A.
   */
  bool scheduler_a_idle;

  /**
   * @brief If this member is true, then an idle task shall execute on
   *   scheduler B.
   */
  bool scheduler_b_idle;

  /**
   * @brief If this member is true, then the second task shall be active.
   */
  bool second_active;

  /**
   * @brief This member specifies the priority of the second task.
   */
  rtems_task_priority second_priority;

  /**
   * @brief If this member is true, then the third task shall be active.
   */
  bool third_active;

  /**
   * @brief This member specifies the priority of the third task.
   */
  rtems_task_priority third_priority;

  /**
   * @brief If this member is true, then the helping task shall be active.
   */
  bool helping_active;

  /**
   * @brief This member specifies the priority of the MrsP semaphore with
   *   respect to scheduler A.
   */
  rtems_task_priority sema_priority_scheduler_a;

  /**
   * @brief This member specifies the priority of the MrsP semaphore with
   *   respect to scheduler B.
   */
  rtems_task_priority sema_priority_scheduler_b;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 4 ];

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
    RtemsSemReqMrspObtain_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsSemReqMrspObtain_Context;

static RtemsSemReqMrspObtain_Context
  RtemsSemReqMrspObtain_Instance;

static const char * const RtemsSemReqMrspObtain_PreDesc_Home[] = {
  "Idle",
  "Task",
  "TaskIdle",
  "Second",
  "SecondIdle",
  "NA"
};

static const char * const RtemsSemReqMrspObtain_PreDesc_Helping[] = {
  "Idle",
  "Task",
  "Helping",
  "HelpingIdle",
  "Third",
  "ThirdIdle",
  "NA"
};

static const char * const RtemsSemReqMrspObtain_PreDesc_PriorityHome[] = {
  "None",
  "NewHigh",
  "NewEqual",
  "SecondHigh",
  "SecondEqual",
  "SecondLow",
  "NA"
};

static const char * const RtemsSemReqMrspObtain_PreDesc_PriorityHelping[] = {
  "None",
  "Helping",
  "ThirdHigh",
  "ThirdEqual",
  "ThirdLow",
  "NA"
};

static const char * const * const RtemsSemReqMrspObtain_PreDesc[] = {
  RtemsSemReqMrspObtain_PreDesc_Home,
  RtemsSemReqMrspObtain_PreDesc_Helping,
  RtemsSemReqMrspObtain_PreDesc_PriorityHome,
  RtemsSemReqMrspObtain_PreDesc_PriorityHelping,
  NULL
};

#define HELPING TQ_BLOCKER_A

#define SECOND TQ_BLOCKER_B

#define THIRD TQ_BLOCKER_C

#define ASSISTANT TQ_BLOCKER_D

#define MOVER TQ_BLOCKER_E

typedef RtemsSemReqMrspObtain_Context Context;

static void SetSemaphorePriority(
  rtems_id            id,
  rtems_task_priority priority_a,
  rtems_task_priority priority_b
)
{
  rtems_status_code   sc;
  rtems_task_priority priority;

  sc = rtems_semaphore_set_priority(
    id,
    SCHEDULER_A_ID,
    priority_a,
    &priority
  );
  T_rsc_success( sc );

  sc = rtems_semaphore_set_priority(
    id,
    SCHEDULER_B_ID,
    priority_b,
    &priority
  );
  T_rsc_success( sc );
}

static void MoveToScheduler( Context *ctx, rtems_id scheduler_id )
{
  rtems_id other_scheduler_id;
  uint32_t cpu;

  if ( scheduler_id == SCHEDULER_A_ID ) {
    other_scheduler_id =  SCHEDULER_B_ID;
    cpu = 0;
  } else {
    other_scheduler_id =  SCHEDULER_A_ID;
    cpu = 1;
  }

  TQSetScheduler( &ctx->tq_ctx, MOVER, other_scheduler_id, PRIO_VERY_HIGH );
  ctx->tq_ctx.busy_wait[ MOVER ] = true;
  TQSend( &ctx->tq_ctx, MOVER, TQ_EVENT_BUSY_WAIT );
  TQWaitForEventsReceived( &ctx->tq_ctx, MOVER );
  T_eq_u32( rtems_scheduler_get_processor(), cpu );
  ctx->tq_ctx.busy_wait[ MOVER ] = false;
  TQWaitForExecutionStop( &ctx->tq_ctx, MOVER );
}

static void RtemsSemReqMrspObtain_Pre_Home_Prepare(
  RtemsSemReqMrspObtain_Context *ctx,
  RtemsSemReqMrspObtain_Pre_Home state
)
{
  switch ( state ) {
    case RtemsSemReqMrspObtain_Pre_Home_Idle: {
      /*
       * While an idle task executes on the processor owned by the home
       * scheduler of the obtaining task.
       */
      ctx->scheduler_a_idle = true;
      break;
    }

    case RtemsSemReqMrspObtain_Pre_Home_Task: {
      /*
       * While the obtaining task executes on the processor owned by the home
       * scheduler of the obtaining task.
       */
      ctx->task_scheduler = SCHEDULER_A_ID;
      break;
    }

    case RtemsSemReqMrspObtain_Pre_Home_TaskIdle: {
      /*
       * While an idle task on behalf of the obtaining task executes on the
       * processor owned by the home scheduler of the obtaining task.
       */
      ctx->scheduler_a_idle = true;
      break;
    }

    case RtemsSemReqMrspObtain_Pre_Home_Second: {
      /*
       * While the second task executes on the processor owned by the home
       * scheduler of the obtaining task.
       */
      ctx->second_active = true;
      break;
    }

    case RtemsSemReqMrspObtain_Pre_Home_SecondIdle: {
      /*
       * While an idle task on behalf of the second task executes on the
       * processor owned by the home scheduler of the obtaining task.
       */
      ctx->second_active = true;
      ctx->scheduler_a_idle = true;
      break;
    }

    case RtemsSemReqMrspObtain_Pre_Home_NA:
      break;
  }
}

static void RtemsSemReqMrspObtain_Pre_Helping_Prepare(
  RtemsSemReqMrspObtain_Context    *ctx,
  RtemsSemReqMrspObtain_Pre_Helping state
)
{
  switch ( state ) {
    case RtemsSemReqMrspObtain_Pre_Helping_Idle: {
      /*
       * While an idle task executes on the processor owned by the helping
       * scheduler of the obtaining task.
       */
      ctx->scheduler_b_idle = true;
      break;
    }

    case RtemsSemReqMrspObtain_Pre_Helping_Task: {
      /*
       * While the obtaining task executes on the processor owned by the
       * helping scheduler of the obtaining task.
       */
      ctx->task_scheduler = SCHEDULER_B_ID;
      break;
    }

    case RtemsSemReqMrspObtain_Pre_Helping_Helping: {
      /*
       * While a helping task of the obtaining task executes on the processor
       * owned by the helping scheduler of the obtaining task.
       */
      ctx->task_owns_mrsp_semaphore = true;
      ctx->helping_active = true;
      break;
    }

    case RtemsSemReqMrspObtain_Pre_Helping_HelpingIdle: {
      /*
       * While an idle task on behalf of a helping task of the obtaining task
       * executes on the processor owned by the helping scheduler of the
       * obtaining task.
       */
      ctx->task_owns_mrsp_semaphore = true;
      ctx->helping_active = true;
      ctx->scheduler_b_idle = true;
      break;
    }

    case RtemsSemReqMrspObtain_Pre_Helping_Third: {
      /*
       * While the third task executes on the processor owned by the helping
       * scheduler of the obtaining task.
       */
      ctx->third_active = true;
      break;
    }

    case RtemsSemReqMrspObtain_Pre_Helping_ThirdIdle: {
      /*
       * While an idle task on behalf of the third task executes on the
       * processor owned by the helping scheduler of the obtaining task.
       */
      ctx->third_active = true;
      ctx->scheduler_b_idle = true;
      break;
    }

    case RtemsSemReqMrspObtain_Pre_Helping_NA:
      break;
  }
}

static void RtemsSemReqMrspObtain_Pre_PriorityHome_Prepare(
  RtemsSemReqMrspObtain_Context         *ctx,
  RtemsSemReqMrspObtain_Pre_PriorityHome state
)
{
  switch ( state ) {
    case RtemsSemReqMrspObtain_Pre_PriorityHome_None: {
      /*
       * While no ceiling priority with respect to the home scheduler of the
       * obtaining task is already available to the task.
       */
      ctx->second_priority = PRIO_HIGH;
      ctx->sema_priority_scheduler_a = PRIO_NORMAL;
      break;
    }

    case RtemsSemReqMrspObtain_Pre_PriorityHome_NewHigh: {
      /*
       * While the ceiling priority of the semaphore with respect to the home
       * scheduler of the obtaining task is higher than the ceiling priorities
       * already available to the task.
       */
      ctx->task_owns_mrsp_semaphore = true;
      ctx->sema_priority_scheduler_a = PRIO_HIGH;
      break;
    }

    case RtemsSemReqMrspObtain_Pre_PriorityHome_NewEqual: {
      /*
       * While the ceiling priority of the semaphore with respect to the home
       * scheduler of the obtaining task is equal to the ceiling priorities
       * already available to the task.
       */
      ctx->task_owns_mrsp_semaphore = true;
      ctx->sema_priority_scheduler_a = PRIO_NORMAL;
      break;
    }

    case RtemsSemReqMrspObtain_Pre_PriorityHome_SecondHigh: {
      /*
       * While the ceiling priority of the semaphore with respect to the home
       * scheduler of the obtaining task is higher than the priority of the
       * second task.
       */
      ctx->second_priority = PRIO_HIGH;
      ctx->sema_priority_scheduler_a = PRIO_VERY_HIGH;
      break;
    }

    case RtemsSemReqMrspObtain_Pre_PriorityHome_SecondEqual: {
      /*
       * While the ceiling priority of the semaphore with respect to the home
       * scheduler of the obtaining task is equal to the priority of the second
       * task.
       */
      ctx->second_priority = PRIO_HIGH;
      ctx->sema_priority_scheduler_a = PRIO_HIGH;
      break;
    }

    case RtemsSemReqMrspObtain_Pre_PriorityHome_SecondLow: {
      /*
       * While the ceiling priority of the semaphore with respect to the home
       * scheduler of the obtaining task is lower than the priority of the
       * second task.
       */
      ctx->second_priority = PRIO_HIGH;
      ctx->sema_priority_scheduler_a = PRIO_NORMAL;
      break;
    }

    case RtemsSemReqMrspObtain_Pre_PriorityHome_NA:
      break;
  }
}

static void RtemsSemReqMrspObtain_Pre_PriorityHelping_Prepare(
  RtemsSemReqMrspObtain_Context            *ctx,
  RtemsSemReqMrspObtain_Pre_PriorityHelping state
)
{
  switch ( state ) {
    case RtemsSemReqMrspObtain_Pre_PriorityHelping_None: {
      /*
       * While no ceiling priority with respect to the helping scheduler of the
       * obtaining task is already available to the task.
       */
      ctx->sema_priority_scheduler_b = PRIO_NORMAL;
      break;
    }

    case RtemsSemReqMrspObtain_Pre_PriorityHelping_Helping: {
      /*
       * While ceiling priorities with respect to the helping scheduler of the
       * obtaining task are already available to the task.
       */
      ctx->helping_active = true;
      ctx->task_owns_mrsp_semaphore = true;
      ctx->sema_priority_scheduler_b = PRIO_NORMAL;
      break;
    }

    case RtemsSemReqMrspObtain_Pre_PriorityHelping_ThirdHigh: {
      /*
       * While the ceiling priority of the semaphore with respect to the
       * helping scheduler of the obtaining task is higher than the priority of
       * the third task.
       */
      ctx->third_priority = PRIO_LOW;
      ctx->sema_priority_scheduler_b = PRIO_NORMAL;
      break;
    }

    case RtemsSemReqMrspObtain_Pre_PriorityHelping_ThirdEqual: {
      /*
       * While the ceiling priority of the semaphore with respect to the
       * helping scheduler of the obtaining task is equal to the priority of
       * the third task.
       */
      ctx->third_priority = PRIO_NORMAL;
      ctx->sema_priority_scheduler_b = PRIO_NORMAL;
      break;
    }

    case RtemsSemReqMrspObtain_Pre_PriorityHelping_ThirdLow: {
      /*
       * While the ceiling priority of the semaphore with respect to the
       * helping scheduler of the obtaining task is lower than the priority of
       * the third task.
       */
      ctx->third_priority = PRIO_HIGH;
      ctx->sema_priority_scheduler_b = PRIO_NORMAL;
      break;
    }

    case RtemsSemReqMrspObtain_Pre_PriorityHelping_NA:
      break;
  }
}

static void RtemsSemReqMrspObtain_Post_Home_Check(
  RtemsSemReqMrspObtain_Context  *ctx,
  RtemsSemReqMrspObtain_Post_Home state
)
{
  const Per_CPU_Control *cpu;
  const Thread_Control  *scheduled;
  const Scheduler_Node  *scheduler_node;
  uint32_t               task_cpu_index;

  cpu = _Per_CPU_Get_by_index( 0 );
  scheduled = cpu->heir;
  task_cpu_index = rtems_scheduler_get_processor(); 

  switch ( state ) {
    case RtemsSemReqMrspObtain_Post_Home_Task: {
      /*
       * The obtaining task shall execute on the processor owned by the home
       * scheduler of the obtaining task.
       */
      T_eq_u32( task_cpu_index, 0 );
      T_eq_ptr( scheduled, ctx->tq_ctx.runner_tcb );
      break;
    }

    case RtemsSemReqMrspObtain_Post_Home_TaskIdle: {
      /*
       * An idle task on behalf of the obtaining task shall execute on the
       * processor owned by the home scheduler of the obtaining task.
       */
      T_eq_u32( task_cpu_index, 1 );
      T_true( scheduled->is_idle );
      scheduler_node = _Thread_Scheduler_get_node_by_index(
        ctx->tq_ctx.runner_tcb,
        0
      );
      T_eq_ptr( scheduler_node->user, scheduled );
      break;
    }

    case RtemsSemReqMrspObtain_Post_Home_Second: {
      /*
       * The second task shall execute on the processor owned by the home
       * scheduler of the obtaining task.
       */
      T_eq_u32( task_cpu_index, 1 );
      T_eq_ptr( scheduled, ctx->tq_ctx.worker_tcb[ SECOND ] );
      break;
    }

    case RtemsSemReqMrspObtain_Post_Home_SecondIdle: {
      /*
       * An idle task on behalf of the second task shall execute on the
       * processor owned by the home scheduler of the obtaining task.
       */
      T_eq_u32( task_cpu_index, 1 );
      T_true( scheduled->is_idle );
      scheduler_node = _Thread_Scheduler_get_node_by_index(
        ctx->tq_ctx.worker_tcb[ SECOND ],
        0
      );
      T_eq_ptr( scheduler_node->user, scheduled );
      break;
    }

    case RtemsSemReqMrspObtain_Post_Home_NA:
      break;
  }
}

static void RtemsSemReqMrspObtain_Post_Helping_Check(
  RtemsSemReqMrspObtain_Context     *ctx,
  RtemsSemReqMrspObtain_Post_Helping state
)
{
  const Per_CPU_Control *cpu;
  const Thread_Control  *scheduled;
  const Scheduler_Node  *scheduler_node;
  uint32_t               task_cpu_index;

  cpu = _Per_CPU_Get_by_index( 1 );
  scheduled = cpu->heir;
  task_cpu_index = rtems_scheduler_get_processor(); 

  switch ( state ) {
    case RtemsSemReqMrspObtain_Post_Helping_Idle: {
      /*
       * An idle task shall execute on the processor owned by the helping
       * scheduler of the obtaining task.
       */
      T_eq_u32( task_cpu_index, 0 );
      T_true( scheduled->is_idle );
      break;
    }

    case RtemsSemReqMrspObtain_Post_Helping_Task: {
      /*
       * The obtaining task shall execute on the processor owned by the helping
       * scheduler of the obtaining task.
       */
      T_eq_u32( task_cpu_index, 1 );
      T_eq_ptr( scheduled, ctx->tq_ctx.runner_tcb );
      break;
    }

    case RtemsSemReqMrspObtain_Post_Helping_TaskIdle: {
      /*
       * An idle task on behalf of the obtaining task shall execute on the
       * processor owned by the helping scheduler of the obtaining task.
       */
      T_eq_u32( task_cpu_index, 0 );
      T_true( scheduled->is_idle );
      scheduler_node = _Thread_Scheduler_get_node_by_index(
        ctx->tq_ctx.runner_tcb,
        1
      );
      T_eq_ptr( scheduler_node->user, ctx->tq_ctx.runner_tcb );
      break;
    }

    case RtemsSemReqMrspObtain_Post_Helping_Helping: {
      /*
       * The helping task shall execute on the processor owned by the helping
       * scheduler of the obtaining task.
       */
      T_eq_u32( task_cpu_index, 0 );
      T_eq_ptr( scheduled, ctx->tq_ctx.worker_tcb[ HELPING ] );
      break;
    }

    case RtemsSemReqMrspObtain_Post_Helping_HelpingIdle: {
      /*
       * An idle task on behalf of the helping task shall execute on the
       * processor owned by the helping scheduler of the obtaining task.
       */
      T_eq_u32( task_cpu_index, 0 );
      T_true( scheduled->is_idle );
      scheduler_node = _Thread_Scheduler_get_node_by_index(
        ctx->tq_ctx.worker_tcb[ HELPING ],
        1
      );
      T_eq_ptr( scheduler_node->user, scheduled );
      break;
    }

    case RtemsSemReqMrspObtain_Post_Helping_Third: {
      /*
       * The third task shall execute on the processor owned by the helping
       * scheduler of the obtaining task.
       */
      T_eq_u32( task_cpu_index, 0 );
      T_eq_ptr( scheduled, ctx->tq_ctx.worker_tcb[ THIRD ] );
      break;
    }

    case RtemsSemReqMrspObtain_Post_Helping_ThirdIdle: {
      /*
       * An idle task on behalf of the third task shall execute on the
       * processor owned by the helping scheduler of the obtaining task.
       */
      T_eq_u32( task_cpu_index, 0 );
      scheduler_node = _Thread_Scheduler_get_node_by_index(
        ctx->tq_ctx.worker_tcb[ THIRD ],
        1
      );
      T_eq_ptr( scheduler_node->user, scheduled );
      break;
    }

    case RtemsSemReqMrspObtain_Post_Helping_NA:
      break;
  }
}

static void RtemsSemReqMrspObtain_Setup( RtemsSemReqMrspObtain_Context *ctx )
{
  rtems_status_code sc;
  rtems_id          mutex_b;
  rtems_id          mutex_c;

  memset( ctx, 0, sizeof( *ctx ) );

  sc = rtems_semaphore_create(
    rtems_build_name( 'S', 'E', 'M', 'A' ),
    1,
    RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY |
      RTEMS_MULTIPROCESSOR_RESOURCE_SHARING,
    PRIO_NORMAL,
    &ctx->sema_id
  );
  T_rsc_success( sc );

  ctx->tq_ctx.deadlock = TQ_DEADLOCK_STATUS;
  ctx->tq_ctx.enqueue_prepare = TQEnqueuePrepareDefault;
  ctx->tq_ctx.enqueue_done = TQEnqueueDoneDefault;
  ctx->tq_ctx.enqueue = TQEnqueueClassicSem;
  ctx->tq_ctx.surrender = TQSurrenderClassicSem;
  ctx->tq_ctx.get_owner = TQGetOwnerClassicSem;
  ctx->tq_ctx.convert_status = TQConvertStatusClassic;
  TQInitialize( &ctx->tq_ctx );

  DeleteMutex( ctx->tq_ctx.mutex_id[ TQ_MUTEX_B ] );
  DeleteMutex( ctx->tq_ctx.mutex_id[ TQ_MUTEX_C ] );

  mutex_b = 0;
  sc = rtems_semaphore_create(
    rtems_build_name( 'M', 'T', 'X', 'B' ),
    1,
    RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY |
      RTEMS_MULTIPROCESSOR_RESOURCE_SHARING,
    PRIO_NORMAL,
    &mutex_b
  );
  T_rsc_success( sc );

  mutex_c = 0;
  sc = rtems_semaphore_create(
    rtems_build_name( 'M', 'T', 'X', 'C' ),
    1,
    RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY |
      RTEMS_MULTIPROCESSOR_RESOURCE_SHARING,
    PRIO_NORMAL,
    &mutex_c
  );
  T_rsc_success( sc );

  ctx->tq_ctx.mutex_id[ TQ_MUTEX_B ] = mutex_b;
  ctx->tq_ctx.mutex_id[ TQ_MUTEX_C ] = mutex_c;

  TQSetScheduler( &ctx->tq_ctx, HELPING, SCHEDULER_B_ID, PRIO_VERY_LOW );
  TQSetScheduler( &ctx->tq_ctx, THIRD, SCHEDULER_B_ID, PRIO_NORMAL );

  TQMutexObtain( &ctx->tq_ctx, TQ_MUTEX_A );
  TQSetScheduler( &ctx->tq_ctx, ASSISTANT, SCHEDULER_B_ID, PRIO_VERY_LOW );
  TQSendAndWaitForExecutionStop(
    &ctx->tq_ctx,
    ASSISTANT,
    TQ_EVENT_MUTEX_A_OBTAIN
  );

  SetSemaphorePriority(
    ctx->tq_ctx.mutex_id[ TQ_MUTEX_B ],
    PRIO_NORMAL,
    PRIO_VERY_LOW
  );
}

static void RtemsSemReqMrspObtain_Setup_Wrap( void *arg )
{
  RtemsSemReqMrspObtain_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsSemReqMrspObtain_Setup( ctx );
}

static void RtemsSemReqMrspObtain_Teardown(
  RtemsSemReqMrspObtain_Context *ctx
)
{
  TQMutexRelease( &ctx->tq_ctx, TQ_MUTEX_A );
  TQSendAndWaitForExecutionStop(
    &ctx->tq_ctx,
    ASSISTANT,
    TQ_EVENT_MUTEX_A_RELEASE
  );
  TQDestroy( &ctx->tq_ctx );
  DeleteMutex( ctx->sema_id );
}

static void RtemsSemReqMrspObtain_Teardown_Wrap( void *arg )
{
  RtemsSemReqMrspObtain_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsSemReqMrspObtain_Teardown( ctx );
}

static void RtemsSemReqMrspObtain_Prepare( RtemsSemReqMrspObtain_Context *ctx )
{
  ctx->task_scheduler = INVALID_ID;
  ctx->task_owns_mrsp_semaphore = false;
  ctx->scheduler_a_idle = false;
  ctx->scheduler_b_idle = false;
  ctx->helping_active = false;
  ctx->second_active = false;
  ctx->third_active = false;
}

static void RtemsSemReqMrspObtain_Action( RtemsSemReqMrspObtain_Context *ctx )
{
  if ( ctx->task_owns_mrsp_semaphore ) {
    TQMutexObtain( &ctx->tq_ctx, TQ_MUTEX_B );
  }

  if ( ctx->helping_active ) {
    T_true( ctx->task_owns_mrsp_semaphore );

    TQSendAndWaitForIntendToBlock(
      &ctx->tq_ctx,
      HELPING,
      TQ_EVENT_MUTEX_B_OBTAIN
    );

    if ( ctx->scheduler_b_idle ) {
      SuspendTask( ctx->tq_ctx.worker_id[ HELPING ] );
    }
  }

  if ( ctx->scheduler_a_idle || ctx->second_active ) {
    MoveToScheduler( ctx, SCHEDULER_B_ID );
  }

  if ( ctx->second_active ) {
    T_false( ctx->third_active );

    TQSetPriority( &ctx->tq_ctx, SECOND, ctx->second_priority );

    if ( ctx->scheduler_a_idle ) {
      SetSemaphorePriority(
        ctx->tq_ctx.mutex_id[ TQ_MUTEX_C ],
        ctx->second_priority,
        ctx->second_priority
      );
      TQSendAndWaitForExecutionStop(
        &ctx->tq_ctx,
        SECOND,
        TQ_EVENT_MUTEX_C_OBTAIN
      );
    } else {
      ctx->tq_ctx.busy_wait[ SECOND ] = true;
      TQSend( &ctx->tq_ctx, SECOND, TQ_EVENT_BUSY_WAIT );
      TQWaitForEventsReceived( &ctx->tq_ctx, SECOND );
    }
  }

  if ( ctx->third_active ) {
    T_false( ctx->second_active );

    TQSetPriority( &ctx->tq_ctx, THIRD, ctx->third_priority );

    if ( ctx->scheduler_b_idle ) {
      SetSemaphorePriority(
        ctx->tq_ctx.mutex_id[ TQ_MUTEX_C ],
        ctx->third_priority,
        ctx->third_priority
      );
      TQSendAndWaitForExecutionStop(
        &ctx->tq_ctx,
        THIRD,
        TQ_EVENT_MUTEX_C_OBTAIN
      );
    } else {
      ctx->tq_ctx.busy_wait[ THIRD ] = true;
      TQSend( &ctx->tq_ctx, THIRD, TQ_EVENT_BUSY_WAIT );
      TQWaitForEventsReceived( &ctx->tq_ctx, THIRD );
    }
  }

  SetSemaphorePriority(
    ctx->sema_id,
    ctx->sema_priority_scheduler_a,
    ctx->sema_priority_scheduler_b
  );
  ObtainMutex( ctx->sema_id );
}

static void RtemsSemReqMrspObtain_Cleanup( RtemsSemReqMrspObtain_Context *ctx )
{
  ReleaseMutex( ctx->sema_id );

  if ( ctx->task_owns_mrsp_semaphore ) {
    TQMutexRelease( &ctx->tq_ctx, TQ_MUTEX_B );
  }

  if ( ctx->second_active ) {
    MoveToScheduler( ctx, SCHEDULER_B_ID );

    if ( ctx->scheduler_a_idle ) {
      TQSendAndWaitForExecutionStop(
        &ctx->tq_ctx,
        SECOND,
        TQ_EVENT_MUTEX_C_RELEASE
      );
    } else {
      ctx->tq_ctx.busy_wait[ SECOND ] = false;
      TQWaitForExecutionStop( &ctx->tq_ctx, SECOND );
    }
  }

  if ( ctx->third_active ) {
    MoveToScheduler( ctx, SCHEDULER_A_ID );

    if ( ctx->scheduler_b_idle ) {
      TQSendAndWaitForExecutionStop(
        &ctx->tq_ctx,
        THIRD,
        TQ_EVENT_MUTEX_C_RELEASE
      );
    } else {
      ctx->tq_ctx.busy_wait[ THIRD ] = false;
      TQWaitForExecutionStop( &ctx->tq_ctx, THIRD );
    }
  }

  if ( ctx->helping_active ) {
    MoveToScheduler( ctx, SCHEDULER_A_ID );

    if ( ctx->scheduler_b_idle ) {
      ResumeTask( ctx->tq_ctx.worker_id[ HELPING ] );
    }

    TQSendAndWaitForExecutionStop(
      &ctx->tq_ctx,
      HELPING,
      TQ_EVENT_MUTEX_B_RELEASE
    );
  }

  MoveToScheduler( ctx, SCHEDULER_A_ID );
}

static const RtemsSemReqMrspObtain_Entry
RtemsSemReqMrspObtain_Entries[] = {
  { 1, 0, 0, 0, 0, RtemsSemReqMrspObtain_Post_Home_NA,
    RtemsSemReqMrspObtain_Post_Helping_NA },
  { 1, 0, 0, 0, 0, RtemsSemReqMrspObtain_Post_Home_NA,
    RtemsSemReqMrspObtain_Post_Helping_NA },
  { 1, 0, 0, 0, 0, RtemsSemReqMrspObtain_Post_Home_NA,
    RtemsSemReqMrspObtain_Post_Helping_NA },
  { 1, 0, 0, 0, 0, RtemsSemReqMrspObtain_Post_Home_NA,
    RtemsSemReqMrspObtain_Post_Helping_NA },
  { 0, 0, 0, 0, 0, RtemsSemReqMrspObtain_Post_Home_Task,
    RtemsSemReqMrspObtain_Post_Helping_Third },
  { 0, 0, 0, 0, 0, RtemsSemReqMrspObtain_Post_Home_Task,
    RtemsSemReqMrspObtain_Post_Helping_ThirdIdle },
  { 0, 0, 0, 0, 0, RtemsSemReqMrspObtain_Post_Home_TaskIdle,
    RtemsSemReqMrspObtain_Post_Helping_Task },
  { 0, 0, 0, 0, 0, RtemsSemReqMrspObtain_Post_Home_Second,
    RtemsSemReqMrspObtain_Post_Helping_Task },
  { 0, 0, 0, 0, 0, RtemsSemReqMrspObtain_Post_Home_SecondIdle,
    RtemsSemReqMrspObtain_Post_Helping_Task },
  { 0, 0, 0, 0, 0, RtemsSemReqMrspObtain_Post_Home_Task,
    RtemsSemReqMrspObtain_Post_Helping_Helping },
  { 0, 0, 0, 0, 0, RtemsSemReqMrspObtain_Post_Home_Task,
    RtemsSemReqMrspObtain_Post_Helping_HelpingIdle },
  { 0, 0, 0, 0, 0, RtemsSemReqMrspObtain_Post_Home_Task,
    RtemsSemReqMrspObtain_Post_Helping_Idle }
};

static const uint8_t
RtemsSemReqMrspObtain_Map[] = {
  2, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 1, 1, 1, 3, 0, 1, 1, 1, 3,
  0, 1, 1, 1, 6, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 1, 1, 1, 3, 3,
  1, 1, 1, 3, 3, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 1,
  1, 1, 3, 3, 1, 1, 1, 3, 3, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 3, 1, 1, 1, 0, 3, 1, 1, 1, 0, 3, 1, 1, 1, 2, 0, 2, 2, 2, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 0, 2, 2, 2, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 11,
  0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 1, 1, 1, 3, 0, 1, 1, 1, 3, 0,
  1, 1, 1, 2, 0, 1, 1, 1, 0, 2, 1, 1, 1, 0, 2, 1, 1, 1, 3, 3, 1, 1, 1, 3, 3, 1,
  1, 1, 3, 3, 1, 1, 1, 0, 0, 0, 0, 0, 0, 9, 1, 1, 1, 0, 9, 1, 1, 1, 3, 3, 1, 1,
  1, 3, 3, 1, 1, 1, 3, 3, 1, 1, 1, 0, 0, 0, 0, 0, 0, 10, 1, 1, 1, 0, 10, 1, 1,
  1, 0, 3, 1, 1, 1, 0, 3, 1, 1, 1, 0, 3, 1, 1, 1, 4, 0, 4, 4, 4, 0, 4, 4, 4, 4,
  0, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 5, 0, 5, 5, 5, 0,
  5, 5, 5, 5, 0, 5, 5, 5, 5, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1,
  1, 1, 0, 0, 0, 0, 0, 0, 6, 1, 1, 1, 0, 6, 1, 1, 1, 0, 3, 1, 1, 1, 0, 3, 1, 1,
  1, 0, 3, 1, 1, 1, 0, 0, 0, 0, 0, 0, 2, 1, 1, 1, 0, 2, 1, 1, 1, 0, 3, 1, 1, 1,
  0, 3, 1, 1, 1, 0, 3, 1, 1, 1, 0, 0, 0, 0, 0, 0, 2, 1, 1, 1, 0, 2, 1, 1, 1, 0,
  3, 1, 1, 1, 0, 3, 1, 1, 1, 0, 3, 1, 1, 1, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 0, 2,
  2, 2, 2, 0, 3, 3, 3, 3, 0, 3, 3, 3, 3, 0, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 2, 2,
  2, 2, 0, 2, 2, 2, 2, 0, 3, 3, 3, 3, 0, 3, 3, 3, 3, 0, 3, 3, 3, 3, 2, 0, 1, 1,
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 1, 1, 1, 2, 0, 1, 1, 1, 2, 0, 1, 1, 1,
  7, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 1, 1, 1, 7, 7, 1, 1, 1, 7,
  7, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 1, 1, 1, 2, 2,
  1, 1, 1, 2, 2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1,
  1, 1, 0, 2, 1, 1, 1, 0, 2, 1, 1, 1, 2, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 2, 2, 2, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 1, 1, 1, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 1, 1, 1, 2, 0, 1, 1, 1, 2, 0, 1, 1, 1, 8, 0,
  1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 1, 1, 1, 8, 8, 1, 1, 1, 8, 8, 1,
  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 1, 1, 1, 2, 2, 1, 1,
  1, 2, 2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 1,
  0, 2, 1, 1, 1, 0, 2, 1, 1, 1, 2, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
};

static size_t RtemsSemReqMrspObtain_Scope( void *arg, char *buf, size_t n )
{
  RtemsSemReqMrspObtain_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsSemReqMrspObtain_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsSemReqMrspObtain_Fixture = {
  .setup = RtemsSemReqMrspObtain_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsSemReqMrspObtain_Teardown_Wrap,
  .scope = RtemsSemReqMrspObtain_Scope,
  .initial_context = &RtemsSemReqMrspObtain_Instance
};

static inline RtemsSemReqMrspObtain_Entry RtemsSemReqMrspObtain_PopEntry(
  RtemsSemReqMrspObtain_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsSemReqMrspObtain_Entries[
    RtemsSemReqMrspObtain_Map[ index ]
  ];
}

static void RtemsSemReqMrspObtain_TestVariant(
  RtemsSemReqMrspObtain_Context *ctx
)
{
  RtemsSemReqMrspObtain_Pre_Home_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsSemReqMrspObtain_Pre_Helping_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsSemReqMrspObtain_Pre_PriorityHome_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsSemReqMrspObtain_Pre_PriorityHelping_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  RtemsSemReqMrspObtain_Action( ctx );
  RtemsSemReqMrspObtain_Post_Home_Check( ctx, ctx->Map.entry.Post_Home );
  RtemsSemReqMrspObtain_Post_Helping_Check( ctx, ctx->Map.entry.Post_Helping );
}

/**
 * @fn void T_case_body_RtemsSemReqMrspObtain( void )
 */
T_TEST_CASE_FIXTURE( RtemsSemReqMrspObtain, &RtemsSemReqMrspObtain_Fixture )
{
  RtemsSemReqMrspObtain_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsSemReqMrspObtain_Pre_Home_Idle;
    ctx->Map.pcs[ 0 ] < RtemsSemReqMrspObtain_Pre_Home_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = RtemsSemReqMrspObtain_Pre_Helping_Idle;
      ctx->Map.pcs[ 1 ] < RtemsSemReqMrspObtain_Pre_Helping_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = RtemsSemReqMrspObtain_Pre_PriorityHome_None;
        ctx->Map.pcs[ 2 ] < RtemsSemReqMrspObtain_Pre_PriorityHome_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        for (
          ctx->Map.pcs[ 3 ] = RtemsSemReqMrspObtain_Pre_PriorityHelping_None;
          ctx->Map.pcs[ 3 ] < RtemsSemReqMrspObtain_Pre_PriorityHelping_NA;
          ++ctx->Map.pcs[ 3 ]
        ) {
          ctx->Map.entry = RtemsSemReqMrspObtain_PopEntry( ctx );

          if ( ctx->Map.entry.Skip ) {
            continue;
          }

          RtemsSemReqMrspObtain_Prepare( ctx );
          RtemsSemReqMrspObtain_TestVariant( ctx );
          RtemsSemReqMrspObtain_Cleanup( ctx );
        }
      }
    }
  }
}

/** @} */
