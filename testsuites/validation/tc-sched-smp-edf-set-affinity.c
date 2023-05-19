/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreSchedSmpEdfReqSetAffinity
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
#include <rtems/score/percpu.h>
#include <rtems/score/threaddispatch.h>
#include <rtems/score/threadimpl.h>

#include "tx-support.h"
#include "tx-thread-queue.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreSchedSmpEdfReqSetAffinity \
 *   spec:/score/sched/smp/edf/req/set-affinity
 *
 * @ingroup TestsuitesValidationSmpOnly0
 *
 * @{
 */

typedef enum {
  ScoreSchedSmpEdfReqSetAffinity_Pre_Before_All,
  ScoreSchedSmpEdfReqSetAffinity_Pre_Before_X,
  ScoreSchedSmpEdfReqSetAffinity_Pre_Before_NA
} ScoreSchedSmpEdfReqSetAffinity_Pre_Before;

typedef enum {
  ScoreSchedSmpEdfReqSetAffinity_Pre_After_All,
  ScoreSchedSmpEdfReqSetAffinity_Pre_After_X,
  ScoreSchedSmpEdfReqSetAffinity_Pre_After_Y,
  ScoreSchedSmpEdfReqSetAffinity_Pre_After_NA
} ScoreSchedSmpEdfReqSetAffinity_Pre_After;

typedef enum {
  ScoreSchedSmpEdfReqSetAffinity_Pre_Priority_High,
  ScoreSchedSmpEdfReqSetAffinity_Pre_Priority_Low,
  ScoreSchedSmpEdfReqSetAffinity_Pre_Priority_NA
} ScoreSchedSmpEdfReqSetAffinity_Pre_Priority;

typedef enum {
  ScoreSchedSmpEdfReqSetAffinity_Pre_State_Ready,
  ScoreSchedSmpEdfReqSetAffinity_Pre_State_Blocked,
  ScoreSchedSmpEdfReqSetAffinity_Pre_State_NA
} ScoreSchedSmpEdfReqSetAffinity_Pre_State;

typedef enum {
  ScoreSchedSmpEdfReqSetAffinity_Pre_Sticky_Yes,
  ScoreSchedSmpEdfReqSetAffinity_Pre_Sticky_No,
  ScoreSchedSmpEdfReqSetAffinity_Pre_Sticky_NA
} ScoreSchedSmpEdfReqSetAffinity_Pre_Sticky;

typedef enum {
  ScoreSchedSmpEdfReqSetAffinity_Pre_Pinned_Yes,
  ScoreSchedSmpEdfReqSetAffinity_Pre_Pinned_No,
  ScoreSchedSmpEdfReqSetAffinity_Pre_Pinned_NA
} ScoreSchedSmpEdfReqSetAffinity_Pre_Pinned;

typedef enum {
  ScoreSchedSmpEdfReqSetAffinity_Pre_AlphaPriority_High,
  ScoreSchedSmpEdfReqSetAffinity_Pre_AlphaPriority_Low,
  ScoreSchedSmpEdfReqSetAffinity_Pre_AlphaPriority_NA
} ScoreSchedSmpEdfReqSetAffinity_Pre_AlphaPriority;

typedef enum {
  ScoreSchedSmpEdfReqSetAffinity_Pre_AlphaAffinity_All,
  ScoreSchedSmpEdfReqSetAffinity_Pre_AlphaAffinity_X,
  ScoreSchedSmpEdfReqSetAffinity_Pre_AlphaAffinity_NA
} ScoreSchedSmpEdfReqSetAffinity_Pre_AlphaAffinity;

typedef enum {
  ScoreSchedSmpEdfReqSetAffinity_Pre_AlphaIdle_Yes,
  ScoreSchedSmpEdfReqSetAffinity_Pre_AlphaIdle_No,
  ScoreSchedSmpEdfReqSetAffinity_Pre_AlphaIdle_NA
} ScoreSchedSmpEdfReqSetAffinity_Pre_AlphaIdle;

typedef enum {
  ScoreSchedSmpEdfReqSetAffinity_Pre_BetaPriority_High,
  ScoreSchedSmpEdfReqSetAffinity_Pre_BetaPriority_Low,
  ScoreSchedSmpEdfReqSetAffinity_Pre_BetaPriority_NA
} ScoreSchedSmpEdfReqSetAffinity_Pre_BetaPriority;

typedef enum {
  ScoreSchedSmpEdfReqSetAffinity_Pre_BetaAffinity_All,
  ScoreSchedSmpEdfReqSetAffinity_Pre_BetaAffinity_Y,
  ScoreSchedSmpEdfReqSetAffinity_Pre_BetaAffinity_NA
} ScoreSchedSmpEdfReqSetAffinity_Pre_BetaAffinity;

typedef enum {
  ScoreSchedSmpEdfReqSetAffinity_Pre_BetaIdle_Yes,
  ScoreSchedSmpEdfReqSetAffinity_Pre_BetaIdle_No,
  ScoreSchedSmpEdfReqSetAffinity_Pre_BetaIdle_NA
} ScoreSchedSmpEdfReqSetAffinity_Pre_BetaIdle;

typedef enum {
  ScoreSchedSmpEdfReqSetAffinity_Post_X_Task,
  ScoreSchedSmpEdfReqSetAffinity_Post_X_TaskIdle,
  ScoreSchedSmpEdfReqSetAffinity_Post_X_Alpha,
  ScoreSchedSmpEdfReqSetAffinity_Post_X_AlphaIdle,
  ScoreSchedSmpEdfReqSetAffinity_Post_X_Beta,
  ScoreSchedSmpEdfReqSetAffinity_Post_X_BetaIdle,
  ScoreSchedSmpEdfReqSetAffinity_Post_X_NA
} ScoreSchedSmpEdfReqSetAffinity_Post_X;

typedef enum {
  ScoreSchedSmpEdfReqSetAffinity_Post_Y_Task,
  ScoreSchedSmpEdfReqSetAffinity_Post_Y_TaskIdle,
  ScoreSchedSmpEdfReqSetAffinity_Post_Y_Alpha,
  ScoreSchedSmpEdfReqSetAffinity_Post_Y_AlphaIdle,
  ScoreSchedSmpEdfReqSetAffinity_Post_Y_Beta,
  ScoreSchedSmpEdfReqSetAffinity_Post_Y_BetaIdle,
  ScoreSchedSmpEdfReqSetAffinity_Post_Y_NA
} ScoreSchedSmpEdfReqSetAffinity_Post_Y;

typedef struct {
  uint32_t Skip : 1;
  uint32_t Pre_Before_NA : 1;
  uint32_t Pre_After_NA : 1;
  uint32_t Pre_Priority_NA : 1;
  uint32_t Pre_State_NA : 1;
  uint32_t Pre_Sticky_NA : 1;
  uint32_t Pre_Pinned_NA : 1;
  uint32_t Pre_AlphaPriority_NA : 1;
  uint32_t Pre_AlphaAffinity_NA : 1;
  uint32_t Pre_AlphaIdle_NA : 1;
  uint32_t Pre_BetaPriority_NA : 1;
  uint32_t Pre_BetaAffinity_NA : 1;
  uint32_t Pre_BetaIdle_NA : 1;
  uint32_t Post_X : 3;
  uint32_t Post_Y : 3;
} ScoreSchedSmpEdfReqSetAffinity_Entry;

/**
 * @brief Test context for spec:/score/sched/smp/edf/req/set-affinity test
 *   case.
 */
typedef struct {
  /**
   * @brief This member contains the thread queue test context.
   */
  TQContext tq_ctx;

  /**
   * @brief This member specifies the task affinity before changing the
   *   affinity.
   */
  cpu_set_t task_affinity_before;

  /**
   * @brief This member specifies the task affinity after changing the
   *   affinity.
   */
  cpu_set_t task_affinity_after;

  /**
   * @brief This member specifies the priority of the task.
   */
  rtems_task_priority task_priority;

  /**
   * @brief If this member is true, then the task state shall be ready.
   */
  bool task_ready;

  /**
   * @brief If this member is true, then the task shall have obtained a sticky
   *   mutex.
   */
  bool task_sticky;

  /**
   * @brief If this member is true, then the task shall be pinned.
   */
  bool task_pinned;

  /**
   * @brief This member specifies the priority of the alpha task.
   */
  rtems_task_priority alpha_priority;

  /**
   * @brief This member specifies the affinity of the alpha task.
   */
  cpu_set_t alpha_affinity;

  /**
   * @brief If this member is true, then an idle task shall execute on behalf
   *   of the alpha task.
   */
  bool alpha_idle;

  /**
   * @brief This member specifies the priority of the beta task.
   */
  rtems_task_priority beta_priority;

  /**
   * @brief This member specifies the affinity of the beta task.
   */
  cpu_set_t beta_affinity;

  /**
   * @brief If this member is true, then an idle task shall execute on behalf
   *   of the beta task.
   */
  bool beta_idle;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 12 ];

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
    ScoreSchedSmpEdfReqSetAffinity_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} ScoreSchedSmpEdfReqSetAffinity_Context;

static ScoreSchedSmpEdfReqSetAffinity_Context
  ScoreSchedSmpEdfReqSetAffinity_Instance;

static const char * const ScoreSchedSmpEdfReqSetAffinity_PreDesc_Before[] = {
  "All",
  "X",
  "NA"
};

static const char * const ScoreSchedSmpEdfReqSetAffinity_PreDesc_After[] = {
  "All",
  "X",
  "Y",
  "NA"
};

static const char * const ScoreSchedSmpEdfReqSetAffinity_PreDesc_Priority[] = {
  "High",
  "Low",
  "NA"
};

static const char * const ScoreSchedSmpEdfReqSetAffinity_PreDesc_State[] = {
  "Ready",
  "Blocked",
  "NA"
};

static const char * const ScoreSchedSmpEdfReqSetAffinity_PreDesc_Sticky[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const ScoreSchedSmpEdfReqSetAffinity_PreDesc_Pinned[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const ScoreSchedSmpEdfReqSetAffinity_PreDesc_AlphaPriority[] = {
  "High",
  "Low",
  "NA"
};

static const char * const ScoreSchedSmpEdfReqSetAffinity_PreDesc_AlphaAffinity[] = {
  "All",
  "X",
  "NA"
};

static const char * const ScoreSchedSmpEdfReqSetAffinity_PreDesc_AlphaIdle[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const ScoreSchedSmpEdfReqSetAffinity_PreDesc_BetaPriority[] = {
  "High",
  "Low",
  "NA"
};

static const char * const ScoreSchedSmpEdfReqSetAffinity_PreDesc_BetaAffinity[] = {
  "All",
  "Y",
  "NA"
};

static const char * const ScoreSchedSmpEdfReqSetAffinity_PreDesc_BetaIdle[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const * const ScoreSchedSmpEdfReqSetAffinity_PreDesc[] = {
  ScoreSchedSmpEdfReqSetAffinity_PreDesc_Before,
  ScoreSchedSmpEdfReqSetAffinity_PreDesc_After,
  ScoreSchedSmpEdfReqSetAffinity_PreDesc_Priority,
  ScoreSchedSmpEdfReqSetAffinity_PreDesc_State,
  ScoreSchedSmpEdfReqSetAffinity_PreDesc_Sticky,
  ScoreSchedSmpEdfReqSetAffinity_PreDesc_Pinned,
  ScoreSchedSmpEdfReqSetAffinity_PreDesc_AlphaPriority,
  ScoreSchedSmpEdfReqSetAffinity_PreDesc_AlphaAffinity,
  ScoreSchedSmpEdfReqSetAffinity_PreDesc_AlphaIdle,
  ScoreSchedSmpEdfReqSetAffinity_PreDesc_BetaPriority,
  ScoreSchedSmpEdfReqSetAffinity_PreDesc_BetaAffinity,
  ScoreSchedSmpEdfReqSetAffinity_PreDesc_BetaIdle,
  NULL
};

#define TASK TQ_BLOCKER_C

#define ALPHA TQ_BLOCKER_A

#define BETA TQ_BLOCKER_B

static void ScoreSchedSmpEdfReqSetAffinity_Pre_Before_Prepare(
  ScoreSchedSmpEdfReqSetAffinity_Context   *ctx,
  ScoreSchedSmpEdfReqSetAffinity_Pre_Before state
)
{
  switch ( state ) {
    case ScoreSchedSmpEdfReqSetAffinity_Pre_Before_All: {
      /*
       * While task ``T`` is affine to all processors of its home scheduler
       * before the new thread to processor affinity is set.
       */
      CPU_FILL( &ctx->task_affinity_before );
      break;
    }

    case ScoreSchedSmpEdfReqSetAffinity_Pre_Before_X: {
      /*
       * While task ``T`` is affine to processor ``X`` before the new thread to
       * processor affinity is set.
       */
      CPU_ZERO( &ctx->task_affinity_before );
      CPU_SET( 0, &ctx->task_affinity_before );
      break;
    }

    case ScoreSchedSmpEdfReqSetAffinity_Pre_Before_NA:
      break;
  }
}

static void ScoreSchedSmpEdfReqSetAffinity_Pre_After_Prepare(
  ScoreSchedSmpEdfReqSetAffinity_Context  *ctx,
  ScoreSchedSmpEdfReqSetAffinity_Pre_After state
)
{
  switch ( state ) {
    case ScoreSchedSmpEdfReqSetAffinity_Pre_After_All: {
      /*
       * While task ``T`` is set to be affine to all processors of its home
       * scheduler.
       */
      CPU_FILL( &ctx->task_affinity_after );
      break;
    }

    case ScoreSchedSmpEdfReqSetAffinity_Pre_After_X: {
      /*
       * While task ``T`` is set to be affine to processor ``X``.
       */
      CPU_ZERO( &ctx->task_affinity_after );
      CPU_SET( 0, &ctx->task_affinity_after );
      break;
    }

    case ScoreSchedSmpEdfReqSetAffinity_Pre_After_Y: {
      /*
       * While task ``T`` is set to be affine to processor ``Y``.
       */
      CPU_ZERO( &ctx->task_affinity_after );
      CPU_SET( 1, &ctx->task_affinity_after );
      break;
    }

    case ScoreSchedSmpEdfReqSetAffinity_Pre_After_NA:
      break;
  }
}

static void ScoreSchedSmpEdfReqSetAffinity_Pre_Priority_Prepare(
  ScoreSchedSmpEdfReqSetAffinity_Context     *ctx,
  ScoreSchedSmpEdfReqSetAffinity_Pre_Priority state
)
{
  switch ( state ) {
    case ScoreSchedSmpEdfReqSetAffinity_Pre_Priority_High: {
      /*
       * While task ``T`` has a high priority.
       */
      ctx->task_priority = PRIO_HIGH;
      break;
    }

    case ScoreSchedSmpEdfReqSetAffinity_Pre_Priority_Low: {
      /*
       * While task ``T`` has a low priority.
       */
      ctx->task_priority = PRIO_NORMAL;
      break;
    }

    case ScoreSchedSmpEdfReqSetAffinity_Pre_Priority_NA:
      break;
  }
}

static void ScoreSchedSmpEdfReqSetAffinity_Pre_State_Prepare(
  ScoreSchedSmpEdfReqSetAffinity_Context  *ctx,
  ScoreSchedSmpEdfReqSetAffinity_Pre_State state
)
{
  switch ( state ) {
    case ScoreSchedSmpEdfReqSetAffinity_Pre_State_Ready: {
      /*
       * While task ``T`` is ready.
       */
      ctx->task_ready = true;
      break;
    }

    case ScoreSchedSmpEdfReqSetAffinity_Pre_State_Blocked: {
      /*
       * While task ``T`` is blocked.
       */
      ctx->task_ready = false;
      break;
    }

    case ScoreSchedSmpEdfReqSetAffinity_Pre_State_NA:
      break;
  }
}

static void ScoreSchedSmpEdfReqSetAffinity_Pre_Sticky_Prepare(
  ScoreSchedSmpEdfReqSetAffinity_Context   *ctx,
  ScoreSchedSmpEdfReqSetAffinity_Pre_Sticky state
)
{
  switch ( state ) {
    case ScoreSchedSmpEdfReqSetAffinity_Pre_Sticky_Yes: {
      /*
       * While task ``T`` is sticky.
       */
      ctx->task_sticky = true;
      break;
    }

    case ScoreSchedSmpEdfReqSetAffinity_Pre_Sticky_No: {
      /*
       * While task ``T`` is not sticky.
       */
      ctx->task_sticky = false;
      break;
    }

    case ScoreSchedSmpEdfReqSetAffinity_Pre_Sticky_NA:
      break;
  }
}

static void ScoreSchedSmpEdfReqSetAffinity_Pre_Pinned_Prepare(
  ScoreSchedSmpEdfReqSetAffinity_Context   *ctx,
  ScoreSchedSmpEdfReqSetAffinity_Pre_Pinned state
)
{
  switch ( state ) {
    case ScoreSchedSmpEdfReqSetAffinity_Pre_Pinned_Yes: {
      /*
       * While task ``T`` is pinned to a processor.
       */
      ctx->task_pinned = true;
      break;
    }

    case ScoreSchedSmpEdfReqSetAffinity_Pre_Pinned_No: {
      /*
       * While task ``T`` is not pinned to a processor.
       */
      ctx->task_pinned = false;
      break;
    }

    case ScoreSchedSmpEdfReqSetAffinity_Pre_Pinned_NA:
      break;
  }
}

static void ScoreSchedSmpEdfReqSetAffinity_Pre_AlphaPriority_Prepare(
  ScoreSchedSmpEdfReqSetAffinity_Context          *ctx,
  ScoreSchedSmpEdfReqSetAffinity_Pre_AlphaPriority state
)
{
  switch ( state ) {
    case ScoreSchedSmpEdfReqSetAffinity_Pre_AlphaPriority_High: {
      /*
       * While task ``A`` has a high priority.
       */
      ctx->alpha_priority = PRIO_HIGH;
      break;
    }

    case ScoreSchedSmpEdfReqSetAffinity_Pre_AlphaPriority_Low: {
      /*
       * While task ``A`` has a low priority.
       */
      ctx->alpha_priority = PRIO_NORMAL;
      break;
    }

    case ScoreSchedSmpEdfReqSetAffinity_Pre_AlphaPriority_NA:
      break;
  }
}

static void ScoreSchedSmpEdfReqSetAffinity_Pre_AlphaAffinity_Prepare(
  ScoreSchedSmpEdfReqSetAffinity_Context          *ctx,
  ScoreSchedSmpEdfReqSetAffinity_Pre_AlphaAffinity state
)
{
  switch ( state ) {
    case ScoreSchedSmpEdfReqSetAffinity_Pre_AlphaAffinity_All: {
      /*
       * While task ``A`` is affine to all processors of its home scheduler.
       */
      CPU_FILL( &ctx->alpha_affinity );
      break;
    }

    case ScoreSchedSmpEdfReqSetAffinity_Pre_AlphaAffinity_X: {
      /*
       * While task ``A`` is affine to processor ``X``.
       */
      CPU_ZERO( &ctx->alpha_affinity );
      CPU_SET( 0, &ctx->alpha_affinity );
      break;
    }

    case ScoreSchedSmpEdfReqSetAffinity_Pre_AlphaAffinity_NA:
      break;
  }
}

static void ScoreSchedSmpEdfReqSetAffinity_Pre_AlphaIdle_Prepare(
  ScoreSchedSmpEdfReqSetAffinity_Context      *ctx,
  ScoreSchedSmpEdfReqSetAffinity_Pre_AlphaIdle state
)
{
  switch ( state ) {
    case ScoreSchedSmpEdfReqSetAffinity_Pre_AlphaIdle_Yes: {
      /*
       * While task ``A`` is sticky, while task ``A`` is blocked.
       */
      ctx->alpha_idle = true;
      break;
    }

    case ScoreSchedSmpEdfReqSetAffinity_Pre_AlphaIdle_No: {
      /*
       * While task ``A`` is not sticky.
       */
      ctx->alpha_idle = false;
      break;
    }

    case ScoreSchedSmpEdfReqSetAffinity_Pre_AlphaIdle_NA:
      break;
  }
}

static void ScoreSchedSmpEdfReqSetAffinity_Pre_BetaPriority_Prepare(
  ScoreSchedSmpEdfReqSetAffinity_Context         *ctx,
  ScoreSchedSmpEdfReqSetAffinity_Pre_BetaPriority state
)
{
  switch ( state ) {
    case ScoreSchedSmpEdfReqSetAffinity_Pre_BetaPriority_High: {
      /*
       * While task ``B`` has a high priority.
       */
      ctx->beta_priority = PRIO_HIGH;
      break;
    }

    case ScoreSchedSmpEdfReqSetAffinity_Pre_BetaPriority_Low: {
      /*
       * While task ``B`` has a low priority.
       */
      ctx->beta_priority = PRIO_NORMAL;
      break;
    }

    case ScoreSchedSmpEdfReqSetAffinity_Pre_BetaPriority_NA:
      break;
  }
}

static void ScoreSchedSmpEdfReqSetAffinity_Pre_BetaAffinity_Prepare(
  ScoreSchedSmpEdfReqSetAffinity_Context         *ctx,
  ScoreSchedSmpEdfReqSetAffinity_Pre_BetaAffinity state
)
{
  switch ( state ) {
    case ScoreSchedSmpEdfReqSetAffinity_Pre_BetaAffinity_All: {
      /*
       * While task ``B`` is affine to all processors of its home scheduler.
       */
      CPU_FILL( &ctx->beta_affinity );
      break;
    }

    case ScoreSchedSmpEdfReqSetAffinity_Pre_BetaAffinity_Y: {
      /*
       * While task ``B`` is affine to processor ``Y``.
       */
      CPU_ZERO( &ctx->beta_affinity );
      CPU_SET( 1, &ctx->beta_affinity );
      break;
    }

    case ScoreSchedSmpEdfReqSetAffinity_Pre_BetaAffinity_NA:
      break;
  }
}

static void ScoreSchedSmpEdfReqSetAffinity_Pre_BetaIdle_Prepare(
  ScoreSchedSmpEdfReqSetAffinity_Context     *ctx,
  ScoreSchedSmpEdfReqSetAffinity_Pre_BetaIdle state
)
{
  switch ( state ) {
    case ScoreSchedSmpEdfReqSetAffinity_Pre_BetaIdle_Yes: {
      /*
       * While task ``B`` is sticky, while task ``B`` is blocked.
       */
      ctx->beta_idle = true;
      break;
    }

    case ScoreSchedSmpEdfReqSetAffinity_Pre_BetaIdle_No: {
      /*
       * While task ``B`` is not sticky,
       */
      ctx->beta_idle = false;
      break;
    }

    case ScoreSchedSmpEdfReqSetAffinity_Pre_BetaIdle_NA:
      break;
  }
}

static void ScoreSchedSmpEdfReqSetAffinity_Post_X_Check(
  ScoreSchedSmpEdfReqSetAffinity_Context *ctx,
  ScoreSchedSmpEdfReqSetAffinity_Post_X   state
)
{
  const Per_CPU_Control *cpu;
  const Thread_Control  *scheduled;
  const Scheduler_Node  *scheduler_node;

  cpu = _Per_CPU_Get_by_index( 0 );
  scheduled = cpu->heir;

  switch ( state ) {
    case ScoreSchedSmpEdfReqSetAffinity_Post_X_Task: {
      /*
       * The task ``T`` shall be scheduled on processor ``X``.
       */
      T_eq_ptr( scheduled, ctx->tq_ctx.worker_tcb[ TASK ] );
      break;
    }

    case ScoreSchedSmpEdfReqSetAffinity_Post_X_TaskIdle: {
      /*
       * An idle task on behalf of task ``T`` shall be scheduled on processor
       * ``X``.
       */
      T_true( scheduled->is_idle );
      scheduler_node = _Thread_Scheduler_get_home_node(
        ctx->tq_ctx.worker_tcb[ TASK ]
      );
      T_eq_ptr( scheduler_node->user, scheduled );
      break;
    }

    case ScoreSchedSmpEdfReqSetAffinity_Post_X_Alpha: {
      /*
       * The task ``A`` shall be scheduled on processor ``X``.
       */
      T_eq_ptr( scheduled, ctx->tq_ctx.worker_tcb[ ALPHA ] );
      break;
    }

    case ScoreSchedSmpEdfReqSetAffinity_Post_X_AlphaIdle: {
      /*
       * An idle task on behalf of task ``A`` shall be scheduled on processor
       * ``X``.
       */
      T_true( scheduled->is_idle );
      scheduler_node = _Thread_Scheduler_get_home_node(
        ctx->tq_ctx.worker_tcb[ ALPHA ]
      );
      T_eq_ptr( scheduler_node->user, scheduled );
      break;
    }

    case ScoreSchedSmpEdfReqSetAffinity_Post_X_Beta: {
      /*
       * The task ``B`` shall be scheduled on processor ``X``.
       */
      T_eq_ptr( scheduled, ctx->tq_ctx.worker_tcb[ BETA ] );
      break;
    }

    case ScoreSchedSmpEdfReqSetAffinity_Post_X_BetaIdle: {
      /*
       * An idle task on behalf of task ``B`` shall be scheduled on processor
       * ``X``.
       */
      T_true( scheduled->is_idle );
      scheduler_node = _Thread_Scheduler_get_home_node(
        ctx->tq_ctx.worker_tcb[ BETA ]
      );
      T_eq_ptr( scheduler_node->user, scheduled );
      break;
    }

    case ScoreSchedSmpEdfReqSetAffinity_Post_X_NA:
      break;
  }
}

static void ScoreSchedSmpEdfReqSetAffinity_Post_Y_Check(
  ScoreSchedSmpEdfReqSetAffinity_Context *ctx,
  ScoreSchedSmpEdfReqSetAffinity_Post_Y   state
)
{
  const Per_CPU_Control *cpu;
  const Thread_Control  *scheduled;
  const Scheduler_Node  *scheduler_node;

  cpu = _Per_CPU_Get_by_index( 1 );
  scheduled = cpu->heir;

  switch ( state ) {
    case ScoreSchedSmpEdfReqSetAffinity_Post_Y_Task: {
      /*
       * The task ``T`` shall be scheduled on processor ``Y``.
       */
      T_eq_ptr( scheduled, ctx->tq_ctx.worker_tcb[ TASK ] );
      break;
    }

    case ScoreSchedSmpEdfReqSetAffinity_Post_Y_TaskIdle: {
      /*
       * An idle task on behalf of task ``T`` shall be scheduled on processor
       * ``Y``.
       */
      T_true( scheduled->is_idle );
      scheduler_node = _Thread_Scheduler_get_home_node(
        ctx->tq_ctx.worker_tcb[ TASK ]
      );
      T_eq_ptr( scheduler_node->user, scheduled );
      break;
    }

    case ScoreSchedSmpEdfReqSetAffinity_Post_Y_Alpha: {
      /*
       * The task ``A`` shall be scheduled on processor ``Y``.
       */
      T_eq_ptr( scheduled, ctx->tq_ctx.worker_tcb[ ALPHA ] );
      break;
    }

    case ScoreSchedSmpEdfReqSetAffinity_Post_Y_AlphaIdle: {
      /*
       * An idle task on behalf of task ``A`` shall be scheduled on processor
       * ``Y``.
       */
      T_true( scheduled->is_idle );
      scheduler_node = _Thread_Scheduler_get_home_node(
        ctx->tq_ctx.worker_tcb[ ALPHA ]
      );
      T_eq_ptr( scheduler_node->user, scheduled );
      break;
    }

    case ScoreSchedSmpEdfReqSetAffinity_Post_Y_Beta: {
      /*
       * The task ``B`` shall be scheduled on processor ``Y``.
       */
      T_eq_ptr( scheduled, ctx->tq_ctx.worker_tcb[ BETA ] );
      break;
    }

    case ScoreSchedSmpEdfReqSetAffinity_Post_Y_BetaIdle: {
      /*
       * An idle task on behalf of task ``B`` shall be scheduled on processor
       * ``Y``.
       */
      T_true( scheduled->is_idle );
      scheduler_node = _Thread_Scheduler_get_home_node(
        ctx->tq_ctx.worker_tcb[ BETA ]
      );
      T_eq_ptr( scheduler_node->user, scheduled );
      break;
    }

    case ScoreSchedSmpEdfReqSetAffinity_Post_Y_NA:
      break;
  }
}

static void ScoreSchedSmpEdfReqSetAffinity_Setup(
  ScoreSchedSmpEdfReqSetAffinity_Context *ctx
)
{
  rtems_status_code sc;
  rtems_id          mutex_a;
  rtems_id          mutex_b;
  rtems_id          mutex_c;

  memset( ctx, 0, sizeof( *ctx ) );
  ctx->tq_ctx.deadlock = TQ_DEADLOCK_STATUS;
  ctx->tq_ctx.enqueue_prepare = TQEnqueuePrepareDefault;
  ctx->tq_ctx.enqueue_done = TQEnqueueDoneDefault;
  ctx->tq_ctx.enqueue = TQEnqueueClassicSem;
  ctx->tq_ctx.surrender = TQSurrenderClassicSem;
  ctx->tq_ctx.get_owner = TQGetOwnerClassicSem;
  ctx->tq_ctx.convert_status = TQConvertStatusClassic;
  TQInitialize( &ctx->tq_ctx );

  DeleteMutex( ctx->tq_ctx.mutex_id[ TQ_MUTEX_A ] );
  DeleteMutex( ctx->tq_ctx.mutex_id[ TQ_MUTEX_B ] );
  DeleteMutex( ctx->tq_ctx.mutex_id[ TQ_MUTEX_C ] );

  mutex_a = 0;
  sc = rtems_semaphore_create(
    rtems_build_name( 'M', 'T', 'X', 'A' ),
    1,
    RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY |
      RTEMS_MULTIPROCESSOR_RESOURCE_SHARING,
    PRIO_LOW,
    &mutex_a
  );
  T_rsc_success( sc );

  mutex_b = 0;
  sc = rtems_semaphore_create(
    rtems_build_name( 'M', 'T', 'X', 'B' ),
    1,
    RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY |
      RTEMS_MULTIPROCESSOR_RESOURCE_SHARING,
    PRIO_VERY_LOW,
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

  ctx->tq_ctx.mutex_id[ TQ_MUTEX_A ] = mutex_a;
  ctx->tq_ctx.mutex_id[ TQ_MUTEX_B ] = mutex_b;
  ctx->tq_ctx.mutex_id[ TQ_MUTEX_C ] = mutex_c;

  RemoveProcessor( SCHEDULER_B_ID, 1 );
  AddProcessor( SCHEDULER_A_ID, 1 );

  TQSetPriority( &ctx->tq_ctx, TASK, PRIO_NORMAL );
  TQSetPriority( &ctx->tq_ctx, ALPHA, PRIO_LOW );
  TQSetPriority( &ctx->tq_ctx, BETA, PRIO_VERY_LOW );
}

static void ScoreSchedSmpEdfReqSetAffinity_Setup_Wrap( void *arg )
{
  ScoreSchedSmpEdfReqSetAffinity_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  ScoreSchedSmpEdfReqSetAffinity_Setup( ctx );
}

static void ScoreSchedSmpEdfReqSetAffinity_Teardown(
  ScoreSchedSmpEdfReqSetAffinity_Context *ctx
)
{
  SetSelfAffinityAll();
  TQDestroy( &ctx->tq_ctx );
  RemoveProcessor( SCHEDULER_A_ID, 1 );
  AddProcessor( SCHEDULER_B_ID, 1 );
}

static void ScoreSchedSmpEdfReqSetAffinity_Teardown_Wrap( void *arg )
{
  ScoreSchedSmpEdfReqSetAffinity_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  ScoreSchedSmpEdfReqSetAffinity_Teardown( ctx );
}

static void ScoreSchedSmpEdfReqSetAffinity_Action(
  ScoreSchedSmpEdfReqSetAffinity_Context *ctx
)
{
  rtems_event_set events;

  SetSelfPriority( PRIO_ULTRA_HIGH );
  SetSelfAffinityAll();

  if ( ctx->beta_idle ) {
    events = TQ_EVENT_MUTEX_B_OBTAIN;
    TQSendAndWaitForExecutionStop( &ctx->tq_ctx, BETA, events );
  } else {
    ctx->tq_ctx.busy_wait[ BETA ] = true;
    events = TQ_EVENT_BUSY_WAIT;
    TQSendAndSynchronizeRunner( &ctx->tq_ctx, BETA, events );
  }

  if ( ctx->alpha_idle ) {
    events = TQ_EVENT_MUTEX_A_OBTAIN;
    TQSendAndWaitForExecutionStop( &ctx->tq_ctx, ALPHA, events );
  } else {
    ctx->tq_ctx.busy_wait[ ALPHA ] = true;
    events = TQ_EVENT_BUSY_WAIT;
    TQSendAndSynchronizeRunner( &ctx->tq_ctx, ALPHA, events );
  }

  if ( ctx->task_pinned ) {
    SetSelfAffinityOne( 1 );
    TQSendAndSynchronizeRunner( &ctx->tq_ctx, TASK, TQ_EVENT_PIN );
    SetSelfAffinityAll();
  }

  if ( ctx->task_ready ) {
    ctx->tq_ctx.busy_wait[ TASK ] = true;
    events = TQ_EVENT_BUSY_WAIT;
  } else {
    events = 0;
  }

  if ( ctx->task_sticky ) {
    events |= TQ_EVENT_MUTEX_C_OBTAIN;
  }

  TQSendAndSynchronizeRunner( &ctx->tq_ctx, TASK, events );

  if ( !ctx->task_ready ) {
    TQWaitForExecutionStop( &ctx->tq_ctx, TASK );
  }

  (void) _Thread_Dispatch_disable();

  SetAffinity( ctx->tq_ctx.worker_id[ TASK ], &ctx->task_affinity_before );
  SetAffinity( ctx->tq_ctx.worker_id[ ALPHA ], &ctx->alpha_affinity );
  SetAffinity( ctx->tq_ctx.worker_id[ BETA ], &ctx->beta_affinity );
  SetSelfAffinityOne( 1 );
  TQSetPriority( &ctx->tq_ctx, TASK, ctx->task_priority );
  SetSelfPriority( PRIO_ULTRA_LOW );
  TQSetPriority( &ctx->tq_ctx, ALPHA, ctx->alpha_priority );
  TQSetPriority( &ctx->tq_ctx, BETA, ctx->beta_priority );

  SetAffinity( ctx->tq_ctx.worker_id[ TASK ], &ctx->task_affinity_after );
}

static void ScoreSchedSmpEdfReqSetAffinity_Cleanup(
  ScoreSchedSmpEdfReqSetAffinity_Context *ctx
)
{
  rtems_event_set events;

  SetSelfPriority( PRIO_ULTRA_HIGH );
  _Thread_Dispatch_enable( _Per_CPU_Get() );

  SetSelfAffinityAll();

  ctx->tq_ctx.busy_wait[ TASK ] = false;
  ctx->tq_ctx.busy_wait[ ALPHA ] = false;
  ctx->tq_ctx.busy_wait[ BETA ] = false;

  TQSetPriority( &ctx->tq_ctx, TASK, PRIO_NORMAL );
  TQSetPriority( &ctx->tq_ctx, ALPHA, PRIO_LOW );
  TQSetPriority( &ctx->tq_ctx, BETA, PRIO_VERY_LOW );

  if ( ctx->task_sticky ) {
    events = TQ_EVENT_MUTEX_C_RELEASE;
  } else {
    events = 0;
  }

  if ( ctx->task_pinned ) {
    events |= TQ_EVENT_UNPIN;
  }

  if ( events != 0 ) {
    TQSendAndWaitForExecutionStop( &ctx->tq_ctx, TASK, events );
  } else {
    TQWaitForExecutionStop( &ctx->tq_ctx, TASK );
  }

  SetAffinityAll( ctx->tq_ctx.worker_id[ TASK ] );
  SetAffinityAll( ctx->tq_ctx.worker_id[ ALPHA ] );

  if ( ctx->alpha_idle ) {
    events = TQ_EVENT_MUTEX_A_RELEASE;
  } else {
    events = 0;
  }

  if ( events != 0 ) {
    TQSendAndWaitForExecutionStop( &ctx->tq_ctx, ALPHA, events );
  } else {
    TQWaitForExecutionStop( &ctx->tq_ctx, ALPHA );
  }

  SetAffinityAll( ctx->tq_ctx.worker_id[ BETA ] );

  if ( ctx->beta_idle ) {
    events = TQ_EVENT_MUTEX_B_RELEASE;
  } else {
    events = 0;
  }

  if ( events != 0 ) {
    TQSendAndWaitForExecutionStop( &ctx->tq_ctx, BETA, events );
  } else {
    TQWaitForExecutionStop( &ctx->tq_ctx, BETA );
  }
}

static const ScoreSchedSmpEdfReqSetAffinity_Entry
ScoreSchedSmpEdfReqSetAffinity_Entries[] = {
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreSchedSmpEdfReqSetAffinity_Post_X_NA,
    ScoreSchedSmpEdfReqSetAffinity_Post_Y_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreSchedSmpEdfReqSetAffinity_Post_X_AlphaIdle,
    ScoreSchedSmpEdfReqSetAffinity_Post_Y_BetaIdle },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreSchedSmpEdfReqSetAffinity_Post_X_AlphaIdle,
    ScoreSchedSmpEdfReqSetAffinity_Post_Y_Beta },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreSchedSmpEdfReqSetAffinity_Post_X_Alpha,
    ScoreSchedSmpEdfReqSetAffinity_Post_Y_BetaIdle },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreSchedSmpEdfReqSetAffinity_Post_X_Alpha,
    ScoreSchedSmpEdfReqSetAffinity_Post_Y_Beta },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreSchedSmpEdfReqSetAffinity_Post_X_Task,
    ScoreSchedSmpEdfReqSetAffinity_Post_Y_BetaIdle },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreSchedSmpEdfReqSetAffinity_Post_X_Task,
    ScoreSchedSmpEdfReqSetAffinity_Post_Y_Beta },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreSchedSmpEdfReqSetAffinity_Post_X_Task,
    ScoreSchedSmpEdfReqSetAffinity_Post_Y_AlphaIdle },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreSchedSmpEdfReqSetAffinity_Post_X_Task,
    ScoreSchedSmpEdfReqSetAffinity_Post_Y_Alpha },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreSchedSmpEdfReqSetAffinity_Post_X_AlphaIdle,
    ScoreSchedSmpEdfReqSetAffinity_Post_Y_Task },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreSchedSmpEdfReqSetAffinity_Post_X_Alpha,
    ScoreSchedSmpEdfReqSetAffinity_Post_Y_Task },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreSchedSmpEdfReqSetAffinity_Post_X_BetaIdle,
    ScoreSchedSmpEdfReqSetAffinity_Post_Y_AlphaIdle },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreSchedSmpEdfReqSetAffinity_Post_X_Beta,
    ScoreSchedSmpEdfReqSetAffinity_Post_Y_AlphaIdle },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreSchedSmpEdfReqSetAffinity_Post_X_BetaIdle,
    ScoreSchedSmpEdfReqSetAffinity_Post_Y_Alpha },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreSchedSmpEdfReqSetAffinity_Post_X_Beta,
    ScoreSchedSmpEdfReqSetAffinity_Post_Y_Alpha },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreSchedSmpEdfReqSetAffinity_Post_X_AlphaIdle,
    ScoreSchedSmpEdfReqSetAffinity_Post_Y_TaskIdle },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreSchedSmpEdfReqSetAffinity_Post_X_Alpha,
    ScoreSchedSmpEdfReqSetAffinity_Post_Y_TaskIdle },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreSchedSmpEdfReqSetAffinity_Post_X_TaskIdle,
    ScoreSchedSmpEdfReqSetAffinity_Post_Y_BetaIdle },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreSchedSmpEdfReqSetAffinity_Post_X_TaskIdle,
    ScoreSchedSmpEdfReqSetAffinity_Post_Y_Beta },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreSchedSmpEdfReqSetAffinity_Post_X_TaskIdle,
    ScoreSchedSmpEdfReqSetAffinity_Post_Y_AlphaIdle },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreSchedSmpEdfReqSetAffinity_Post_X_TaskIdle,
    ScoreSchedSmpEdfReqSetAffinity_Post_Y_Alpha },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreSchedSmpEdfReqSetAffinity_Post_X_BetaIdle,
    ScoreSchedSmpEdfReqSetAffinity_Post_Y_Task },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreSchedSmpEdfReqSetAffinity_Post_X_Beta,
    ScoreSchedSmpEdfReqSetAffinity_Post_Y_Task },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreSchedSmpEdfReqSetAffinity_Post_X_BetaIdle,
    ScoreSchedSmpEdfReqSetAffinity_Post_Y_TaskIdle },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ScoreSchedSmpEdfReqSetAffinity_Post_X_Beta,
    ScoreSchedSmpEdfReqSetAffinity_Post_Y_TaskIdle }
};

static const uint8_t
ScoreSchedSmpEdfReqSetAffinity_Map[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 12, 1, 2, 7, 7, 9, 9, 13, 14, 3, 4,
  8, 8, 10, 10, 1, 2, 1, 2, 9, 9, 9, 9, 3, 4, 3, 4, 10, 10, 10, 10, 5, 6, 5, 6,
  7, 7, 9, 9, 5, 6, 5, 6, 8, 8, 10, 10, 5, 6, 5, 6, 9, 9, 9, 9, 5, 6, 5, 6, 10,
  10, 10, 10, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 5, 6, 5, 6, 5, 6,
  5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 7, 7, 7, 7, 5, 6, 5, 6, 8, 8, 8, 8,
  5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 11, 12, 1, 2, 7, 7, 9, 9, 13,
  14, 3, 4, 8, 8, 10, 10, 1, 2, 1, 2, 9, 9, 9, 9, 3, 4, 3, 4, 10, 10, 10, 10,
  5, 6, 5, 6, 7, 7, 9, 9, 5, 6, 5, 6, 8, 8, 10, 10, 5, 6, 5, 6, 9, 9, 9, 9, 5,
  6, 5, 6, 10, 10, 10, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 12, 1, 2,
  19, 19, 15, 15, 13, 14, 3, 4, 20, 20, 16, 16, 1, 2, 1, 2, 15, 15, 15, 15, 3,
  4, 3, 4, 16, 16, 16, 16, 17, 18, 17, 18, 19, 19, 15, 15, 17, 18, 17, 18, 20,
  20, 16, 16, 17, 18, 17, 18, 15, 15, 15, 15, 17, 18, 17, 18, 16, 16, 16, 16,
  1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4,
  3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2,
  1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4,
  3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2,
  3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 12, 1, 2, 11, 12, 1, 2, 13, 14, 3, 4, 13,
  14, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 11, 12,
  1, 2, 3, 4, 3, 4, 13, 14, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3,
  4, 11, 12, 1, 2, 7, 7, 7, 7, 13, 14, 3, 4, 8, 8, 8, 8, 1, 2, 1, 2, 1, 2, 1,
  2, 3, 4, 3, 4, 3, 4, 3, 4, 5, 6, 5, 6, 7, 7, 7, 7, 5, 6, 5, 6, 8, 8, 8, 8, 5,
  6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 11, 12, 1, 2, 11, 12, 1, 2, 13,
  14, 3, 4, 13, 14, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2,
  1, 2, 11, 12, 1, 2, 3, 4, 3, 4, 13, 14, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4,
  3, 4, 3, 4, 3, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 12, 1, 2, 11, 12,
  1, 2, 13, 14, 3, 4, 13, 14, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4,
  3, 4, 1, 2, 1, 2, 11, 12, 1, 2, 3, 4, 3, 4, 13, 14, 3, 4, 1, 2, 1, 2, 1, 2,
  1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4,
  1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4,
  3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2,
  1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4,
  3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2,
  3, 4, 3, 4, 3, 4, 3, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 12, 1, 2, 7,
  7, 7, 7, 13, 14, 3, 4, 8, 8, 8, 8, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4,
  3, 4, 5, 6, 5, 6, 7, 7, 7, 7, 5, 6, 5, 6, 8, 8, 8, 8, 5, 6, 5, 6, 5, 6, 5, 6,
  5, 6, 5, 6, 5, 6, 5, 6, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 5, 6,
  5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 7, 7, 7, 7, 5, 6, 5, 6,
  8, 8, 8, 8, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 11, 12, 1, 2, 7,
  7, 7, 7, 13, 14, 3, 4, 8, 8, 8, 8, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4,
  3, 4, 5, 6, 5, 6, 7, 7, 7, 7, 5, 6, 5, 6, 8, 8, 8, 8, 5, 6, 5, 6, 5, 6, 5, 6,
  5, 6, 5, 6, 5, 6, 5, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 12, 1, 2, 19,
  19, 19, 19, 13, 14, 3, 4, 20, 20, 20, 20, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4,
  3, 4, 3, 4, 17, 18, 17, 18, 19, 19, 19, 19, 17, 18, 17, 18, 20, 20, 20, 20,
  17, 18, 17, 18, 17, 18, 17, 18, 17, 18, 17, 18, 17, 18, 17, 18, 1, 2, 1, 2,
  1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4,
  3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2,
  3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2,
  1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4,
  3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 11, 12, 1, 2, 11, 12, 1, 2, 13, 14, 3, 4, 13, 14, 3, 4, 1,
  2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 11, 12, 1, 2, 3, 4,
  3, 4, 13, 14, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 11, 12,
  1, 2, 7, 7, 7, 7, 13, 14, 3, 4, 8, 8, 8, 8, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3,
  4, 3, 4, 3, 4, 5, 6, 5, 6, 7, 7, 7, 7, 5, 6, 5, 6, 8, 8, 8, 8, 5, 6, 5, 6, 5,
  6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 11, 12, 1, 2, 11, 12, 1, 2, 13, 14, 3, 4,
  13, 14, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 11,
  12, 1, 2, 3, 4, 3, 4, 13, 14, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4,
  3, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 12, 1, 2, 11, 12, 1, 2, 13, 14,
  3, 4, 13, 14, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1,
  2, 11, 12, 1, 2, 3, 4, 3, 4, 13, 14, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3,
  4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1,
  2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3,
  4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3,
  4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1,
  2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3,
  4, 3, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 12, 1, 2, 9, 9, 9, 9, 13,
  14, 3, 4, 10, 10, 10, 10, 1, 2, 1, 2, 9, 9, 9, 9, 3, 4, 3, 4, 10, 10, 10, 10,
  21, 22, 1, 2, 9, 9, 9, 9, 21, 22, 3, 4, 10, 10, 10, 10, 21, 22, 1, 2, 9, 9,
  9, 9, 21, 22, 3, 4, 10, 10, 10, 10, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8,
  8, 8, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 7, 7, 7, 7,
  5, 6, 5, 6, 8, 8, 8, 8, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 11,
  12, 1, 2, 9, 9, 9, 9, 13, 14, 3, 4, 10, 10, 10, 10, 1, 2, 1, 2, 9, 9, 9, 9,
  3, 4, 3, 4, 10, 10, 10, 10, 21, 22, 1, 2, 9, 9, 9, 9, 21, 22, 3, 4, 10, 10,
  10, 10, 21, 22, 1, 2, 9, 9, 9, 9, 21, 22, 3, 4, 10, 10, 10, 10, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 11, 12, 1, 2, 15, 15, 15, 15, 13, 14, 3, 4, 16, 16,
  16, 16, 1, 2, 1, 2, 15, 15, 15, 15, 3, 4, 3, 4, 16, 16, 16, 16, 23, 24, 1, 2,
  15, 15, 15, 15, 23, 24, 3, 4, 16, 16, 16, 16, 23, 24, 1, 2, 15, 15, 15, 15,
  23, 24, 3, 4, 16, 16, 16, 16, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4,
  1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4,
  3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2,
  1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4,
  3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2,
  3, 4, 3, 4, 3, 4, 3, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 12, 1, 2, 11,
  12, 1, 2, 13, 14, 3, 4, 13, 14, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3,
  4, 3, 4, 1, 2, 1, 2, 11, 12, 1, 2, 3, 4, 3, 4, 13, 14, 3, 4, 1, 2, 1, 2, 1,
  2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 11, 12, 1, 2, 7, 7, 7, 7, 13, 14, 3, 4, 8,
  8, 8, 8, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 5, 6, 5, 6, 7, 7, 7,
  7, 5, 6, 5, 6, 8, 8, 8, 8, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6,
  11, 12, 1, 2, 11, 12, 1, 2, 13, 14, 3, 4, 13, 14, 3, 4, 1, 2, 1, 2, 1, 2, 1,
  2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 11, 12, 1, 2, 3, 4, 3, 4, 13, 14, 3,
  4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 11, 12, 1, 2, 11, 12, 1, 2, 13, 14, 3, 4, 13, 14, 3, 4, 1, 2, 1, 2,
  1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 11, 12, 1, 2, 3, 4, 3, 4, 13,
  14, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2,
  1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4,
  1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4,
  3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2,
  1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4,
  3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 11, 12, 1, 2, 7, 7, 9, 9, 13, 14, 3, 4, 8, 8, 10, 10, 1, 2, 1, 2,
  9, 9, 9, 9, 3, 4, 3, 4, 10, 10, 10, 10, 5, 6, 5, 6, 7, 7, 9, 9, 5, 6, 5, 6,
  8, 8, 10, 10, 5, 6, 5, 6, 9, 9, 9, 9, 5, 6, 5, 6, 10, 10, 10, 10, 7, 7, 7, 7,
  7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6,
  5, 6, 5, 6, 5, 6, 7, 7, 7, 7, 5, 6, 5, 6, 8, 8, 8, 8, 5, 6, 5, 6, 5, 6, 5, 6,
  5, 6, 5, 6, 5, 6, 5, 6, 11, 12, 1, 2, 7, 7, 9, 9, 13, 14, 3, 4, 8, 8, 10, 10,
  1, 2, 1, 2, 9, 9, 9, 9, 3, 4, 3, 4, 10, 10, 10, 10, 5, 6, 5, 6, 7, 7, 9, 9,
  5, 6, 5, 6, 8, 8, 10, 10, 5, 6, 5, 6, 9, 9, 9, 9, 5, 6, 5, 6, 10, 10, 10, 10,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 12, 1, 2, 19, 19, 15, 15, 13, 14, 3,
  4, 20, 20, 16, 16, 1, 2, 1, 2, 15, 15, 15, 15, 3, 4, 3, 4, 16, 16, 16, 16,
  17, 18, 17, 18, 19, 19, 15, 15, 17, 18, 17, 18, 20, 20, 16, 16, 17, 18, 17,
  18, 15, 15, 15, 15, 17, 18, 17, 18, 16, 16, 16, 16, 1, 2, 1, 2, 1, 2, 1, 2,
  3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2,
  1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4,
  3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2,
  1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4,
  1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 11, 12, 1, 2, 11, 12, 1, 2, 13, 14, 3, 4, 13, 14, 3, 4, 1, 2, 1, 2, 1,
  2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 11, 12, 1, 2, 3, 4, 3, 4, 13,
  14, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 11, 12, 1, 2, 7, 7,
  7, 7, 13, 14, 3, 4, 8, 8, 8, 8, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3,
  4, 5, 6, 5, 6, 7, 7, 7, 7, 5, 6, 5, 6, 8, 8, 8, 8, 5, 6, 5, 6, 5, 6, 5, 6, 5,
  6, 5, 6, 5, 6, 5, 6, 11, 12, 1, 2, 11, 12, 1, 2, 13, 14, 3, 4, 13, 14, 3, 4,
  1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 11, 12, 1, 2, 3,
  4, 3, 4, 13, 14, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 12, 1, 2, 11, 12, 1, 2, 13, 14, 3, 4, 13,
  14, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 11, 12,
  1, 2, 3, 4, 3, 4, 13, 14, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3,
  4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3,
  4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1,
  2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3,
  4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1,
  2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 12, 1, 2, 7, 7, 7, 7, 13, 14, 3, 4, 8,
  8, 8, 8, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 5, 6, 5, 6, 7, 7, 7,
  7, 5, 6, 5, 6, 8, 8, 8, 8, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 7,
  7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5,
  6, 5, 6, 5, 6, 5, 6, 5, 6, 7, 7, 7, 7, 5, 6, 5, 6, 8, 8, 8, 8, 5, 6, 5, 6, 5,
  6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 11, 12, 1, 2, 7, 7, 7, 7, 13, 14, 3, 4, 8,
  8, 8, 8, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 5, 6, 5, 6, 7, 7, 7,
  7, 5, 6, 5, 6, 8, 8, 8, 8, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 12, 1, 2, 19, 19, 19, 19, 13, 14, 3, 4,
  20, 20, 20, 20, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 17, 18, 17,
  18, 19, 19, 19, 19, 17, 18, 17, 18, 20, 20, 20, 20, 17, 18, 17, 18, 17, 18,
  17, 18, 17, 18, 17, 18, 17, 18, 17, 18, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4,
  3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2,
  1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4,
  1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4,
  3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2,
  1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11,
  12, 1, 2, 11, 12, 1, 2, 13, 14, 3, 4, 13, 14, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2,
  3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 11, 12, 1, 2, 3, 4, 3, 4, 13, 14, 3, 4,
  1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 11, 12, 1, 2, 7, 7, 7, 7, 13,
  14, 3, 4, 8, 8, 8, 8, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 5, 6,
  5, 6, 7, 7, 7, 7, 5, 6, 5, 6, 8, 8, 8, 8, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6,
  5, 6, 5, 6, 11, 12, 1, 2, 11, 12, 1, 2, 13, 14, 3, 4, 13, 14, 3, 4, 1, 2, 1,
  2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 11, 12, 1, 2, 3, 4, 3, 4,
  13, 14, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 11, 12, 1, 2, 11, 12, 1, 2, 13, 14, 3, 4, 13, 14, 3, 4,
  1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 11, 12, 1, 2, 3,
  4, 3, 4, 13, 14, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2,
  1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4,
  3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2,
  1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4,
  1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4,
  3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 11, 12, 1, 2, 9, 9, 9, 9, 13, 14, 3, 4, 10, 10, 10,
  10, 1, 2, 1, 2, 9, 9, 9, 9, 3, 4, 3, 4, 10, 10, 10, 10, 21, 22, 1, 2, 9, 9,
  9, 9, 21, 22, 3, 4, 10, 10, 10, 10, 21, 22, 1, 2, 9, 9, 9, 9, 21, 22, 3, 4,
  10, 10, 10, 10, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 5, 6, 5, 6,
  5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 7, 7, 7, 7, 5, 6, 5, 6, 8, 8,
  8, 8, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 11, 12, 1, 2, 9, 9, 9,
  9, 13, 14, 3, 4, 10, 10, 10, 10, 1, 2, 1, 2, 9, 9, 9, 9, 3, 4, 3, 4, 10, 10,
  10, 10, 21, 22, 1, 2, 9, 9, 9, 9, 21, 22, 3, 4, 10, 10, 10, 10, 21, 22, 1, 2,
  9, 9, 9, 9, 21, 22, 3, 4, 10, 10, 10, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  11, 12, 1, 2, 15, 15, 15, 15, 13, 14, 3, 4, 16, 16, 16, 16, 1, 2, 1, 2, 15,
  15, 15, 15, 3, 4, 3, 4, 16, 16, 16, 16, 23, 24, 1, 2, 15, 15, 15, 15, 23, 24,
  3, 4, 16, 16, 16, 16, 23, 24, 1, 2, 15, 15, 15, 15, 23, 24, 3, 4, 16, 16, 16,
  16, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2,
  3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2,
  1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4,
  3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2,
  1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 12, 1, 2, 11, 12, 1, 2, 13, 14, 3, 4,
  13, 14, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 11,
  12, 1, 2, 3, 4, 3, 4, 13, 14, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4,
  3, 4, 11, 12, 1, 2, 7, 7, 7, 7, 13, 14, 3, 4, 8, 8, 8, 8, 1, 2, 1, 2, 1, 2,
  1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 5, 6, 5, 6, 7, 7, 7, 7, 5, 6, 5, 6, 8, 8, 8, 8,
  5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 11, 12, 1, 2, 11, 12, 1, 2,
  13, 14, 3, 4, 13, 14, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4,
  1, 2, 1, 2, 11, 12, 1, 2, 3, 4, 3, 4, 13, 14, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2,
  3, 4, 3, 4, 3, 4, 3, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 12, 1, 2, 11,
  12, 1, 2, 13, 14, 3, 4, 13, 14, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3,
  4, 3, 4, 1, 2, 1, 2, 11, 12, 1, 2, 3, 4, 3, 4, 13, 14, 3, 4, 1, 2, 1, 2, 1,
  2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3,
  4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3,
  4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1,
  2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3,
  4, 3, 4, 1, 2, 1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4, 3, 4, 1, 2, 1, 2, 1, 2, 1,
  2, 3, 4, 3, 4, 3, 4, 3, 4
};

static size_t ScoreSchedSmpEdfReqSetAffinity_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  ScoreSchedSmpEdfReqSetAffinity_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      ScoreSchedSmpEdfReqSetAffinity_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture ScoreSchedSmpEdfReqSetAffinity_Fixture = {
  .setup = ScoreSchedSmpEdfReqSetAffinity_Setup_Wrap,
  .stop = NULL,
  .teardown = ScoreSchedSmpEdfReqSetAffinity_Teardown_Wrap,
  .scope = ScoreSchedSmpEdfReqSetAffinity_Scope,
  .initial_context = &ScoreSchedSmpEdfReqSetAffinity_Instance
};

static inline ScoreSchedSmpEdfReqSetAffinity_Entry
ScoreSchedSmpEdfReqSetAffinity_PopEntry(
  ScoreSchedSmpEdfReqSetAffinity_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return ScoreSchedSmpEdfReqSetAffinity_Entries[
    ScoreSchedSmpEdfReqSetAffinity_Map[ index ]
  ];
}

static void ScoreSchedSmpEdfReqSetAffinity_TestVariant(
  ScoreSchedSmpEdfReqSetAffinity_Context *ctx
)
{
  ScoreSchedSmpEdfReqSetAffinity_Pre_Before_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  ScoreSchedSmpEdfReqSetAffinity_Pre_After_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  ScoreSchedSmpEdfReqSetAffinity_Pre_Priority_Prepare(
    ctx,
    ctx->Map.pcs[ 2 ]
  );
  ScoreSchedSmpEdfReqSetAffinity_Pre_State_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  ScoreSchedSmpEdfReqSetAffinity_Pre_Sticky_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  ScoreSchedSmpEdfReqSetAffinity_Pre_Pinned_Prepare( ctx, ctx->Map.pcs[ 5 ] );
  ScoreSchedSmpEdfReqSetAffinity_Pre_AlphaPriority_Prepare(
    ctx,
    ctx->Map.pcs[ 6 ]
  );
  ScoreSchedSmpEdfReqSetAffinity_Pre_AlphaAffinity_Prepare(
    ctx,
    ctx->Map.pcs[ 7 ]
  );
  ScoreSchedSmpEdfReqSetAffinity_Pre_AlphaIdle_Prepare(
    ctx,
    ctx->Map.pcs[ 8 ]
  );
  ScoreSchedSmpEdfReqSetAffinity_Pre_BetaPriority_Prepare(
    ctx,
    ctx->Map.pcs[ 9 ]
  );
  ScoreSchedSmpEdfReqSetAffinity_Pre_BetaAffinity_Prepare(
    ctx,
    ctx->Map.pcs[ 10 ]
  );
  ScoreSchedSmpEdfReqSetAffinity_Pre_BetaIdle_Prepare(
    ctx,
    ctx->Map.pcs[ 11 ]
  );
  ScoreSchedSmpEdfReqSetAffinity_Action( ctx );
  ScoreSchedSmpEdfReqSetAffinity_Post_X_Check( ctx, ctx->Map.entry.Post_X );
  ScoreSchedSmpEdfReqSetAffinity_Post_Y_Check( ctx, ctx->Map.entry.Post_Y );
}

/**
 * @fn void T_case_body_ScoreSchedSmpEdfReqSetAffinity( void )
 */
T_TEST_CASE_FIXTURE(
  ScoreSchedSmpEdfReqSetAffinity,
  &ScoreSchedSmpEdfReqSetAffinity_Fixture
)
{
  ScoreSchedSmpEdfReqSetAffinity_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = ScoreSchedSmpEdfReqSetAffinity_Pre_Before_All;
    ctx->Map.pcs[ 0 ] < ScoreSchedSmpEdfReqSetAffinity_Pre_Before_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = ScoreSchedSmpEdfReqSetAffinity_Pre_After_All;
      ctx->Map.pcs[ 1 ] < ScoreSchedSmpEdfReqSetAffinity_Pre_After_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = ScoreSchedSmpEdfReqSetAffinity_Pre_Priority_High;
        ctx->Map.pcs[ 2 ] < ScoreSchedSmpEdfReqSetAffinity_Pre_Priority_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        for (
          ctx->Map.pcs[ 3 ] = ScoreSchedSmpEdfReqSetAffinity_Pre_State_Ready;
          ctx->Map.pcs[ 3 ] < ScoreSchedSmpEdfReqSetAffinity_Pre_State_NA;
          ++ctx->Map.pcs[ 3 ]
        ) {
          for (
            ctx->Map.pcs[ 4 ] = ScoreSchedSmpEdfReqSetAffinity_Pre_Sticky_Yes;
            ctx->Map.pcs[ 4 ] < ScoreSchedSmpEdfReqSetAffinity_Pre_Sticky_NA;
            ++ctx->Map.pcs[ 4 ]
          ) {
            for (
              ctx->Map.pcs[ 5 ] = ScoreSchedSmpEdfReqSetAffinity_Pre_Pinned_Yes;
              ctx->Map.pcs[ 5 ] < ScoreSchedSmpEdfReqSetAffinity_Pre_Pinned_NA;
              ++ctx->Map.pcs[ 5 ]
            ) {
              for (
                ctx->Map.pcs[ 6 ] = ScoreSchedSmpEdfReqSetAffinity_Pre_AlphaPriority_High;
                ctx->Map.pcs[ 6 ] < ScoreSchedSmpEdfReqSetAffinity_Pre_AlphaPriority_NA;
                ++ctx->Map.pcs[ 6 ]
              ) {
                for (
                  ctx->Map.pcs[ 7 ] = ScoreSchedSmpEdfReqSetAffinity_Pre_AlphaAffinity_All;
                  ctx->Map.pcs[ 7 ] < ScoreSchedSmpEdfReqSetAffinity_Pre_AlphaAffinity_NA;
                  ++ctx->Map.pcs[ 7 ]
                ) {
                  for (
                    ctx->Map.pcs[ 8 ] = ScoreSchedSmpEdfReqSetAffinity_Pre_AlphaIdle_Yes;
                    ctx->Map.pcs[ 8 ] < ScoreSchedSmpEdfReqSetAffinity_Pre_AlphaIdle_NA;
                    ++ctx->Map.pcs[ 8 ]
                  ) {
                    for (
                      ctx->Map.pcs[ 9 ] = ScoreSchedSmpEdfReqSetAffinity_Pre_BetaPriority_High;
                      ctx->Map.pcs[ 9 ] < ScoreSchedSmpEdfReqSetAffinity_Pre_BetaPriority_NA;
                      ++ctx->Map.pcs[ 9 ]
                    ) {
                      for (
                        ctx->Map.pcs[ 10 ] = ScoreSchedSmpEdfReqSetAffinity_Pre_BetaAffinity_All;
                        ctx->Map.pcs[ 10 ] < ScoreSchedSmpEdfReqSetAffinity_Pre_BetaAffinity_NA;
                        ++ctx->Map.pcs[ 10 ]
                      ) {
                        for (
                          ctx->Map.pcs[ 11 ] = ScoreSchedSmpEdfReqSetAffinity_Pre_BetaIdle_Yes;
                          ctx->Map.pcs[ 11 ] < ScoreSchedSmpEdfReqSetAffinity_Pre_BetaIdle_NA;
                          ++ctx->Map.pcs[ 11 ]
                        ) {
                          ctx->Map.entry =
                          ScoreSchedSmpEdfReqSetAffinity_PopEntry( ctx );

                          if ( ctx->Map.entry.Skip ) {
                            continue;
                          }

                          ScoreSchedSmpEdfReqSetAffinity_TestVariant( ctx );
                          ScoreSchedSmpEdfReqSetAffinity_Cleanup( ctx );
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
  }
}

/** @} */
