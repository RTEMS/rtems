/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsSchedulerReqRemoveProcessor
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
#include <rtems/test-scheduler.h>
#include <rtems/score/percpu.h>
#include <rtems/score/smpbarrier.h>

#include "ts-config.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsSchedulerReqRemoveProcessor \
 *   spec:/rtems/scheduler/req/remove-processor
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsSchedulerReqRemoveProcessor_Pre_Id_Invalid,
  RtemsSchedulerReqRemoveProcessor_Pre_Id_Scheduler,
  RtemsSchedulerReqRemoveProcessor_Pre_Id_NA
} RtemsSchedulerReqRemoveProcessor_Pre_Id;

typedef enum {
  RtemsSchedulerReqRemoveProcessor_Pre_CPUIndex_Valid,
  RtemsSchedulerReqRemoveProcessor_Pre_CPUIndex_Invalid,
  RtemsSchedulerReqRemoveProcessor_Pre_CPUIndex_NA
} RtemsSchedulerReqRemoveProcessor_Pre_CPUIndex;

typedef enum {
  RtemsSchedulerReqRemoveProcessor_Pre_Owned_Yes,
  RtemsSchedulerReqRemoveProcessor_Pre_Owned_No,
  RtemsSchedulerReqRemoveProcessor_Pre_Owned_NA
} RtemsSchedulerReqRemoveProcessor_Pre_Owned;

typedef enum {
  RtemsSchedulerReqRemoveProcessor_Pre_Last_Yes,
  RtemsSchedulerReqRemoveProcessor_Pre_Last_No,
  RtemsSchedulerReqRemoveProcessor_Pre_Last_NA
} RtemsSchedulerReqRemoveProcessor_Pre_Last;

typedef enum {
  RtemsSchedulerReqRemoveProcessor_Pre_Home_Yes,
  RtemsSchedulerReqRemoveProcessor_Pre_Home_No,
  RtemsSchedulerReqRemoveProcessor_Pre_Home_NA
} RtemsSchedulerReqRemoveProcessor_Pre_Home;

typedef enum {
  RtemsSchedulerReqRemoveProcessor_Pre_RequiredByAffinity_Yes,
  RtemsSchedulerReqRemoveProcessor_Pre_RequiredByAffinity_No,
  RtemsSchedulerReqRemoveProcessor_Pre_RequiredByAffinity_NA
} RtemsSchedulerReqRemoveProcessor_Pre_RequiredByAffinity;

typedef enum {
  RtemsSchedulerReqRemoveProcessor_Pre_UsedBy_Idle,
  RtemsSchedulerReqRemoveProcessor_Pre_UsedBy_Task,
  RtemsSchedulerReqRemoveProcessor_Pre_UsedBy_TaskIdle,
  RtemsSchedulerReqRemoveProcessor_Pre_UsedBy_Helping,
  RtemsSchedulerReqRemoveProcessor_Pre_UsedBy_NA
} RtemsSchedulerReqRemoveProcessor_Pre_UsedBy;

typedef enum {
  RtemsSchedulerReqRemoveProcessor_Post_Status_Ok,
  RtemsSchedulerReqRemoveProcessor_Post_Status_InvId,
  RtemsSchedulerReqRemoveProcessor_Post_Status_InvNum,
  RtemsSchedulerReqRemoveProcessor_Post_Status_InUse,
  RtemsSchedulerReqRemoveProcessor_Post_Status_NA
} RtemsSchedulerReqRemoveProcessor_Post_Status;

typedef enum {
  RtemsSchedulerReqRemoveProcessor_Post_Removed_Yes,
  RtemsSchedulerReqRemoveProcessor_Post_Removed_Nop,
  RtemsSchedulerReqRemoveProcessor_Post_Removed_NA
} RtemsSchedulerReqRemoveProcessor_Post_Removed;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Id_NA : 1;
  uint16_t Pre_CPUIndex_NA : 1;
  uint16_t Pre_Owned_NA : 1;
  uint16_t Pre_Last_NA : 1;
  uint16_t Pre_Home_NA : 1;
  uint16_t Pre_RequiredByAffinity_NA : 1;
  uint16_t Pre_UsedBy_NA : 1;
  uint16_t Post_Status : 3;
  uint16_t Post_Removed : 2;
} RtemsSchedulerReqRemoveProcessor_Entry;

typedef enum {
  WORKER_A,
  WORKER_B,
  WORKER_C,
  WORKER_COUNT
} WorkerIndex;

/**
 * @brief Test context for spec:/rtems/scheduler/req/remove-processor test
 *   case.
 */
typedef struct {
  /**
   * @brief This member contains the runner identifier.
   */
  rtems_id runner_id;

  /**
   * @brief This member contains the worker identifiers.
   */
  rtems_id worker_id[ WORKER_COUNT ];

  /**
   * @brief This member contains the mutex identifier.
   */
  rtems_id mutex_id;

  /**
   * @brief This member contains the sticky mutex identifier.
   */
  rtems_id sticky_id;

  /**
   * @brief This member contains the worker busy status.
   */
  volatile bool busy[ WORKER_COUNT ];

  /**
   * @brief This member contains the worker busy status.
   */
  volatile uint32_t busy_counter[ WORKER_COUNT ];

  /**
   * @brief This member contains the barrier to synchronize the runner and the
   *   workers.
   */
  SMP_barrier_Control barrier;

  /**
   * @brief This member contains the call within ISR request.
   */
  CallWithinISRRequest request;

  /**
   * @brief This member provides the context to wrap thread queue operations.
   */
  WrapThreadQueueContext wrap_tq_ctx;

  /**
   * @brief If this member is true, then the processor to remove shall be owned
   *   by the scheduler.
   */
  bool owned;

  /**
   * @brief If this member is true, then the processor to remove shall be the
   *   last processor of the scheduler.
   */
  bool last;

  /**
   * @brief If this member is true, then at least one non-idle task shall use
   *   the scheduler as its home scheduler.
   */
  bool home;

  /**
   * @brief If this member is true, then at least one non-idle task shall
   *   required the processor to remove due to its affinity set.
   */
  bool required_by_affinity;

  /**
   * @brief If this member is true, then the processor to remove shall be used
   *   by an idle task.
   */
  bool idle;

  /**
   * @brief If this member is true, then the processor to remove shall be used
   *   by a task or on behalf of a task which uses the scheduler as its home
   *   scheduler.
   */
  bool task;

  /**
   * @brief If this member is true, then the processor to remove shall be used
   *   by a task which uses the scheduler as a helping scheduler.
   */
  bool helping;

  /**
   * @brief This member provides the scheduler operation records.
   */
  T_scheduler_log_4 scheduler_log;

  /**
   * @brief This member contains the return value of the
   *   rtems_scheduler_remove_processor() call.
   */
  rtems_status_code status;

  /**
   * @brief This member specifies the ``scheduler_id`` parameter value.
   */
  rtems_id id;

  /**
   * @brief This member specifies the ``cpu_index`` parameter value.
   */
  uint32_t cpu_index;

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 7 ];

    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 7 ];

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
    RtemsSchedulerReqRemoveProcessor_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsSchedulerReqRemoveProcessor_Context;

static RtemsSchedulerReqRemoveProcessor_Context
  RtemsSchedulerReqRemoveProcessor_Instance;

static const char * const RtemsSchedulerReqRemoveProcessor_PreDesc_Id[] = {
  "Invalid",
  "Scheduler",
  "NA"
};

static const char * const RtemsSchedulerReqRemoveProcessor_PreDesc_CPUIndex[] = {
  "Valid",
  "Invalid",
  "NA"
};

static const char * const RtemsSchedulerReqRemoveProcessor_PreDesc_Owned[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const RtemsSchedulerReqRemoveProcessor_PreDesc_Last[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const RtemsSchedulerReqRemoveProcessor_PreDesc_Home[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const RtemsSchedulerReqRemoveProcessor_PreDesc_RequiredByAffinity[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const RtemsSchedulerReqRemoveProcessor_PreDesc_UsedBy[] = {
  "Idle",
  "Task",
  "TaskIdle",
  "Helping",
  "NA"
};

static const char * const * const RtemsSchedulerReqRemoveProcessor_PreDesc[] = {
  RtemsSchedulerReqRemoveProcessor_PreDesc_Id,
  RtemsSchedulerReqRemoveProcessor_PreDesc_CPUIndex,
  RtemsSchedulerReqRemoveProcessor_PreDesc_Owned,
  RtemsSchedulerReqRemoveProcessor_PreDesc_Last,
  RtemsSchedulerReqRemoveProcessor_PreDesc_Home,
  RtemsSchedulerReqRemoveProcessor_PreDesc_RequiredByAffinity,
  RtemsSchedulerReqRemoveProcessor_PreDesc_UsedBy,
  NULL
};

typedef RtemsSchedulerReqRemoveProcessor_Context Context;

static void DoRemoveProcessor( Context *ctx )
{
  T_scheduler_log *log;

  log = T_scheduler_record_4( &ctx->scheduler_log );
  T_null( log );

  ctx->status = rtems_scheduler_remove_processor( ctx->id, ctx->cpu_index );

  log = T_scheduler_record( NULL );
  T_eq_ptr( &log->header, &ctx->scheduler_log.header );

  if ( ctx->status == RTEMS_SUCCESSFUL ) {
    AddProcessor( ctx->id, ctx->cpu_index );
  }
}

#if defined(RTEMS_SMP)

#define EVENT_SYNC_RUNNER RTEMS_EVENT_0

#define EVENT_OBTAIN RTEMS_EVENT_1

#define EVENT_RELEASE RTEMS_EVENT_2

#define EVENT_STICKY_OBTAIN RTEMS_EVENT_3

#define EVENT_STICKY_RELEASE RTEMS_EVENT_4

#define EVENT_RESTART RTEMS_EVENT_5

#define EVENT_BUSY RTEMS_EVENT_6

#define EVENT_SYNC_RUNNER_LATE RTEMS_EVENT_7

static void Barriers( void *arg )
{
  Context          *ctx;
  SMP_barrier_State barrier_state;

  ctx = arg;
  _SMP_barrier_State_initialize( &barrier_state );

  /* A */
  _SMP_barrier_Wait( &ctx->barrier, &barrier_state, 2 );

  /* B */
  _SMP_barrier_Wait( &ctx->barrier, &barrier_state, 2 );
}

static void RequestISR( void *arg )
{
  Context *ctx;

  ctx = arg;
  ctx->request.handler = Barriers;
  ctx->request.arg = ctx;
  CallWithinISRSubmit( &ctx->request );
}

static void SendAndSync(
  Context        *ctx,
  WorkerIndex     worker,
  rtems_event_set event
)
{
  SendEvents( ctx->worker_id[ worker ], EVENT_SYNC_RUNNER | event );
  ReceiveAllEvents( EVENT_SYNC_RUNNER );
  WaitForExecutionStop( ctx->worker_id[ worker ] );
}

static void MakeBusy( Context *ctx, WorkerIndex worker )
{
  ctx->busy_counter[ worker ] = 0;
  ctx->busy[ worker ] = true;
  SendEvents( ctx->worker_id[ worker ], EVENT_BUSY );
}

static void MakeBusyAndSync( Context *ctx, WorkerIndex worker )
{
  ctx->busy_counter[ worker ] = 0;
  ctx->busy[ worker ] = true;
  SendEvents( ctx->worker_id[ worker ], EVENT_SYNC_RUNNER | EVENT_BUSY );
  ReceiveAllEvents( EVENT_SYNC_RUNNER );
}

static void StopBusy( Context *ctx, WorkerIndex worker )
{
  ctx->busy[ worker ] = false;
}

static void StopBusyAndWait( Context *ctx, WorkerIndex worker )
{
  StopBusy( ctx, worker );
  WaitForExecutionStop( ctx->worker_id[ worker ] );
}

static void WaitForBusy( Context *ctx, WorkerIndex worker )
{
  while ( ctx->busy_counter[ worker ] == 0 ) {
    /* Wait */
  }
}

static void RemoveWithHelpingOnly( Context *ctx )
{
  SMP_barrier_State barrier_state;

  /*
   * Use the mutex and the worker to construct the removal of the last
   * processor of a scheduler while a thread is scheduled.
   */

  _SMP_barrier_Control_initialize( &ctx->barrier );
  _SMP_barrier_State_initialize( &barrier_state );

  SetScheduler( ctx->worker_id[ WORKER_B ], SCHEDULER_B_ID, PRIO_NORMAL );

  /* Let worker B help worker A */
  SendEvents( ctx->worker_id[ WORKER_A ], EVENT_OBTAIN );
  SendAndSync( ctx, WORKER_B, EVENT_OBTAIN );

  /*
   * Restart the worker B to withdraw the help offer and wait on barriers.
   * Move worker B to scheduler A.  Remove the processor while worker A is
   * scheduled.
   */

  SendEvents( ctx->worker_id[ WORKER_A ], EVENT_RESTART );

  /* A */
  _SMP_barrier_Wait( &ctx->barrier, &barrier_state, 2 );

  SetScheduler( ctx->worker_id[ WORKER_B ], SCHEDULER_A_ID, PRIO_HIGH );

  ctx->id = SCHEDULER_B_ID;
  ctx->cpu_index = 1;
  DoRemoveProcessor( ctx );

  /* B */
  _SMP_barrier_Wait( &ctx->barrier, &barrier_state, 2 );

  /* Clean up all used resources */
  SetSelfPriority( PRIO_NORMAL );
  SendEvents( ctx->worker_id[ WORKER_A ], EVENT_RELEASE );
  T_busy(100000);
}

static void Worker( rtems_task_argument arg, WorkerIndex worker )
{
  Context *ctx;

  ctx = (Context *) arg;

  while ( true ) {
    rtems_event_set events;

    events = ReceiveAnyEvents();

    if ( ( events & EVENT_SYNC_RUNNER ) != 0 ) {
      SendEvents( ctx->runner_id, EVENT_SYNC_RUNNER );
    }

    if ( ( events & EVENT_OBTAIN ) != 0 ) {
      ObtainMutex( ctx->mutex_id );
    }

    if ( ( events & EVENT_RELEASE ) != 0 ) {
      ReleaseMutex( ctx->mutex_id );
    }

    if ( ( events & EVENT_STICKY_OBTAIN ) != 0 ) {
      ObtainMutex( ctx->sticky_id );
    }

    if ( ( events & EVENT_STICKY_RELEASE ) != 0 ) {
      ReleaseMutex( ctx->sticky_id );
    }

    if ( ( events & EVENT_RESTART ) != 0 ) {
      rtems_status_code sc;

      T_eq_u32( rtems_scheduler_get_processor(), 0 );
      SetPriority( ctx->runner_id, PRIO_VERY_HIGH );
      T_eq_u32( rtems_scheduler_get_processor(), 1 );

      if ( !ctx->last ) {
        SetScheduler( ctx->worker_id[ WORKER_C ], SCHEDULER_A_ID, PRIO_LOW );
        RemoveProcessor( SCHEDULER_C_ID, 2 );
        AddProcessor( SCHEDULER_B_ID, 2 );
      }

      WrapThreadQueueExtract(
        &ctx->wrap_tq_ctx,
        GetThread( ctx->worker_id[ WORKER_B ] )
      );

      sc = rtems_task_restart(
        ctx->worker_id[ WORKER_B ],
        (rtems_task_argument) ctx
        );
      T_rsc_success( sc );

      T_eq_u32( rtems_scheduler_get_processor(), 0 );

      if ( !ctx->last ) {
        RemoveProcessor( SCHEDULER_B_ID, 2 );
        AddProcessor( SCHEDULER_C_ID, 2 );
        SetScheduler( ctx->worker_id[ WORKER_C ], SCHEDULER_C_ID, PRIO_NORMAL );
      }
    }

    if ( ( events & EVENT_BUSY ) != 0 ) {
      while ( ctx->busy[ worker ] ) {
        ++ctx->busy_counter[ worker ];
      }
    }

    if ( ( events & EVENT_SYNC_RUNNER_LATE ) != 0 ) {
      SendEvents( ctx->runner_id, EVENT_SYNC_RUNNER );
    }
  }
}

static void WorkerA( rtems_task_argument arg )
{
  Worker( arg, WORKER_A );
}

static void WorkerB( rtems_task_argument arg )
{
  Worker( arg, WORKER_B );
}

static void WorkerC( rtems_task_argument arg )
{
  Worker( arg, WORKER_C );
}
#endif

static void RtemsSchedulerReqRemoveProcessor_Pre_Id_Prepare(
  RtemsSchedulerReqRemoveProcessor_Context *ctx,
  RtemsSchedulerReqRemoveProcessor_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsSchedulerReqRemoveProcessor_Pre_Id_Invalid: {
      /*
       * While the ``scheduler_id`` parameter is not associated with a
       * scheduler.
       */
      ctx->id = INVALID_ID;
      break;
    }

    case RtemsSchedulerReqRemoveProcessor_Pre_Id_Scheduler: {
      /*
       * While the ``scheduler_id`` parameter is associated with a scheduler.
       */
      ctx->id = SCHEDULER_A_ID;
      break;
    }

    case RtemsSchedulerReqRemoveProcessor_Pre_Id_NA:
      break;
  }
}

static void RtemsSchedulerReqRemoveProcessor_Pre_CPUIndex_Prepare(
  RtemsSchedulerReqRemoveProcessor_Context     *ctx,
  RtemsSchedulerReqRemoveProcessor_Pre_CPUIndex state
)
{
  switch ( state ) {
    case RtemsSchedulerReqRemoveProcessor_Pre_CPUIndex_Valid: {
      /*
       * While the ``cpu_index`` parameter is less than the configured
       * processor maximum.
       */
      ctx->cpu_index = 0;
      break;
    }

    case RtemsSchedulerReqRemoveProcessor_Pre_CPUIndex_Invalid: {
      /*
       * While the ``cpu_index`` parameter is greater than or equal to the
       * configured processor maximum.
       */
      ctx->cpu_index = rtems_configuration_get_maximum_processors();
      break;
    }

    case RtemsSchedulerReqRemoveProcessor_Pre_CPUIndex_NA:
      break;
  }
}

static void RtemsSchedulerReqRemoveProcessor_Pre_Owned_Prepare(
  RtemsSchedulerReqRemoveProcessor_Context  *ctx,
  RtemsSchedulerReqRemoveProcessor_Pre_Owned state
)
{
  switch ( state ) {
    case RtemsSchedulerReqRemoveProcessor_Pre_Owned_Yes: {
      /*
       * While the processor specified by the ``cpu_index`` parameter is owned
       * by the scheduler specified by the ``scheduler_id`` parameter.
       */
      ctx->owned = true;
      break;
    }

    case RtemsSchedulerReqRemoveProcessor_Pre_Owned_No: {
      /*
       * While the processor specified by the ``cpu_index`` parameter is not
       * owned by the scheduler specified by the ``scheduler_id`` parameter.
       */
      ctx->owned = false;
      break;
    }

    case RtemsSchedulerReqRemoveProcessor_Pre_Owned_NA:
      break;
  }
}

static void RtemsSchedulerReqRemoveProcessor_Pre_Last_Prepare(
  RtemsSchedulerReqRemoveProcessor_Context *ctx,
  RtemsSchedulerReqRemoveProcessor_Pre_Last state
)
{
  switch ( state ) {
    case RtemsSchedulerReqRemoveProcessor_Pre_Last_Yes: {
      /*
       * While the processor specified by the ``cpu_index`` parameter is the
       * last processor owned by the scheduler specified by the
       * ``scheduler_id`` parameter.
       */
      ctx->last = true;
      break;
    }

    case RtemsSchedulerReqRemoveProcessor_Pre_Last_No: {
      /*
       * While the processor specified by the ``cpu_index`` parameter is not
       * the last processor owned by the scheduler specified by the
       * ``scheduler_id`` parameter.
       */
      ctx->last = false;
      break;
    }

    case RtemsSchedulerReqRemoveProcessor_Pre_Last_NA:
      break;
  }
}

static void RtemsSchedulerReqRemoveProcessor_Pre_Home_Prepare(
  RtemsSchedulerReqRemoveProcessor_Context *ctx,
  RtemsSchedulerReqRemoveProcessor_Pre_Home state
)
{
  switch ( state ) {
    case RtemsSchedulerReqRemoveProcessor_Pre_Home_Yes: {
      /*
       * While at least one non-idle task exists which uses the scheduler
       * specified by the ``scheduler_id`` parameter as its home scheduler.
       */
      ctx->home = true;
      break;
    }

    case RtemsSchedulerReqRemoveProcessor_Pre_Home_No: {
      /*
       * While no non-idle task exists which uses the scheduler specified by
       * the ``scheduler_id`` parameter as its home scheduler.
       */
      ctx->home = false;
      break;
    }

    case RtemsSchedulerReqRemoveProcessor_Pre_Home_NA:
      break;
  }
}

static void RtemsSchedulerReqRemoveProcessor_Pre_RequiredByAffinity_Prepare(
  RtemsSchedulerReqRemoveProcessor_Context               *ctx,
  RtemsSchedulerReqRemoveProcessor_Pre_RequiredByAffinity state
)
{
  switch ( state ) {
    case RtemsSchedulerReqRemoveProcessor_Pre_RequiredByAffinity_Yes: {
      /*
       * While at least one non-idle task which uses the scheduler specified by
       * the ``scheduler_id`` parameter as its home scheduler exists those
       * processor affinity set requires the processor specified by the
       * ``cpu_index`` parameter.
       */
      ctx->required_by_affinity = true;
      break;
    }

    case RtemsSchedulerReqRemoveProcessor_Pre_RequiredByAffinity_No: {
      /*
       * While no non-idle task which uses the scheduler specified by the
       * ``scheduler_id`` parameter as its home scheduler exists those
       * processor affinity set requires the processor specified by the
       * ``cpu_index`` parameter.
       */
      ctx->required_by_affinity = false;
      break;
    }

    case RtemsSchedulerReqRemoveProcessor_Pre_RequiredByAffinity_NA:
      break;
  }
}

static void RtemsSchedulerReqRemoveProcessor_Pre_UsedBy_Prepare(
  RtemsSchedulerReqRemoveProcessor_Context   *ctx,
  RtemsSchedulerReqRemoveProcessor_Pre_UsedBy state
)
{
  switch ( state ) {
    case RtemsSchedulerReqRemoveProcessor_Pre_UsedBy_Idle: {
      /*
       * While the processor specified by the ``cpu_index`` parameter is used
       * by an idle task.
       */
      ctx->idle = true;
      ctx->task = false;
      ctx->helping = false;
      break;
    }

    case RtemsSchedulerReqRemoveProcessor_Pre_UsedBy_Task: {
      /*
       * While the processor specified by the ``cpu_index`` parameter is used
       * by a task task which uses the scheduler specified by the
       * ``scheduler_id`` parameter as its home scheduler.
       */
      ctx->idle = false;
      ctx->task = true;
      ctx->helping = false;
      break;
    }

    case RtemsSchedulerReqRemoveProcessor_Pre_UsedBy_TaskIdle: {
      /*
       * While the processor specified by the ``cpu_index`` parameter is used
       * by an idle task on behalf of a task task which uses the scheduler
       * specified by the ``scheduler_id`` parameter as its home scheduler.
       */
      ctx->idle = true;
      ctx->task = true;
      ctx->helping = false;
      break;
    }

    case RtemsSchedulerReqRemoveProcessor_Pre_UsedBy_Helping: {
      /*
       * While the processor specified by the ``cpu_index`` parameter is used
       * by a task task which uses the scheduler specified by the
       * ``scheduler_id`` parameter as a helping scheduler.
       */
      if ( !ctx->last && rtems_scheduler_get_processor_maximum() < 3 ) {
        ctx->Map.skip = true;
      } else {
        ctx->idle = false;
        ctx->task = false;
        ctx->helping = true;
      }
      break;
    }

    case RtemsSchedulerReqRemoveProcessor_Pre_UsedBy_NA:
      break;
  }
}

static void RtemsSchedulerReqRemoveProcessor_Post_Status_Check(
  RtemsSchedulerReqRemoveProcessor_Context    *ctx,
  RtemsSchedulerReqRemoveProcessor_Post_Status state
)
{
  switch ( state ) {
    case RtemsSchedulerReqRemoveProcessor_Post_Status_Ok: {
      /*
       * The return status of rtems_scheduler_remove_processor() shall be
       * RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsSchedulerReqRemoveProcessor_Post_Status_InvId: {
      /*
       * The return status of rtems_scheduler_remove_processor() shall be
       * RTEMS_INVALID_ID.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ID );
      break;
    }

    case RtemsSchedulerReqRemoveProcessor_Post_Status_InvNum: {
      /*
       * The return status of rtems_scheduler_remove_processor() shall be
       * RTEMS_INVALID_NUMBER.
       */
      T_rsc( ctx->status, RTEMS_INVALID_NUMBER );
      break;
    }

    case RtemsSchedulerReqRemoveProcessor_Post_Status_InUse: {
      /*
       * The return status of rtems_scheduler_remove_processor() shall be
       * RTEMS_RESOURCE_IN_USE.
       */
      T_rsc( ctx->status, RTEMS_RESOURCE_IN_USE );
      break;
    }

    case RtemsSchedulerReqRemoveProcessor_Post_Status_NA:
      break;
  }
}

static void RtemsSchedulerReqRemoveProcessor_Post_Removed_Check(
  RtemsSchedulerReqRemoveProcessor_Context     *ctx,
  RtemsSchedulerReqRemoveProcessor_Post_Removed state
)
{
  switch ( state ) {
    case RtemsSchedulerReqRemoveProcessor_Post_Removed_Yes: {
      /*
       * The processor specified by the ``cpu_index`` parameter shall be
       * removed from the scheduler specified by the ``scheduler_id`` by the
       * rtems_scheduler_remove_processor() call.
       */
      if ( ctx->home && ctx->helping ) {
        /*
         * For these test scenarios we use scheduler A in which the runner
         * remains scheduled.  So, an ask for help request is issued, when the
         * processor allocated to a task which uses the scheduler as a helping
         * scheduler is removed.
         */
        T_eq_u32( ctx->id, SCHEDULER_A_ID );
        T_eq_sz( ctx->scheduler_log.header.recorded, 3 );
        T_eq_int(
          ctx->scheduler_log.events[ 0 ].operation,
          T_SCHEDULER_REMOVE_PROCESSOR
        );
        T_eq_int(
          ctx->scheduler_log.events[ 1 ].operation,
          T_SCHEDULER_ASK_FOR_HELP
        );
        T_eq_int(
          ctx->scheduler_log.events[ 2 ].operation,
          T_SCHEDULER_ASK_FOR_HELP
        );
      } else {
        T_eq_sz( ctx->scheduler_log.header.recorded, 1 );
        T_eq_int(
          ctx->scheduler_log.events[ 0 ].operation,
          T_SCHEDULER_REMOVE_PROCESSOR
        );
      }
      break;
    }

    case RtemsSchedulerReqRemoveProcessor_Post_Removed_Nop: {
      /*
       * No processor shall be removed from a scheduler by the
       * rtems_scheduler_remove_processor() call.
       */
      T_eq_sz( ctx->scheduler_log.header.recorded, 0 );
      break;
    }

    case RtemsSchedulerReqRemoveProcessor_Post_Removed_NA:
      break;
  }
}

static void RtemsSchedulerReqRemoveProcessor_Setup(
  RtemsSchedulerReqRemoveProcessor_Context *ctx
)
{
  #if defined(RTEMS_SMP)
  rtems_status_code   sc;
  rtems_task_priority priority;

  ctx->runner_id = rtems_task_self();
  ctx->mutex_id = CreateMutex();

  sc = rtems_semaphore_create(
    rtems_build_name( 'S', 'T', 'K', 'Y' ),
    1,
    RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY |
      RTEMS_MULTIPROCESSOR_RESOURCE_SHARING,
    PRIO_NORMAL,
    &ctx->sticky_id
  );
  T_rsc_success( sc );

  sc = rtems_semaphore_set_priority(
    ctx->sticky_id,
    SCHEDULER_B_ID,
    PRIO_NORMAL,
    &priority
  );
  T_rsc_success( sc );

  if ( rtems_scheduler_get_processor_maximum() >= 3 ) {
    sc = rtems_semaphore_set_priority(
      ctx->sticky_id,
      SCHEDULER_C_ID,
      PRIO_LOW,
      &priority
    );
    T_rsc_success( sc );

    ctx->worker_id[ WORKER_C ] = CreateTask( "WRKC", PRIO_NORMAL );
    SetScheduler( ctx->worker_id[ WORKER_C ], SCHEDULER_C_ID, PRIO_NORMAL );
    StartTask( ctx->worker_id[ WORKER_C ], WorkerC, ctx );

    if ( rtems_scheduler_get_processor_maximum() >= 4 ) {
      RemoveProcessor( SCHEDULER_C_ID, 3 );
    }
  }

  SetSelfPriority( PRIO_NORMAL );
  SetSelfAffinityOne( 0 );

  ctx->worker_id[ WORKER_A ] = CreateTask( "WRKA", PRIO_HIGH );
  StartTask( ctx->worker_id[ WORKER_A ], WorkerA, ctx );

  ctx->worker_id[ WORKER_B ] = CreateTask( "WRKB", PRIO_HIGH );
  StartTask( ctx->worker_id[ WORKER_B ], WorkerB, ctx );

  WrapThreadQueueInitialize( &ctx->wrap_tq_ctx, RequestISR, ctx );
  #endif
}

static void RtemsSchedulerReqRemoveProcessor_Setup_Wrap( void *arg )
{
  RtemsSchedulerReqRemoveProcessor_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsSchedulerReqRemoveProcessor_Setup( ctx );
}

static void RtemsSchedulerReqRemoveProcessor_Teardown(
  RtemsSchedulerReqRemoveProcessor_Context *ctx
)
{
  #if defined(RTEMS_SMP)
  DeleteTask( ctx->worker_id[ WORKER_A ] );
  DeleteTask( ctx->worker_id[ WORKER_B ] );
  DeleteTask( ctx->worker_id[ WORKER_C ] );
  DeleteMutex( ctx->mutex_id );
  DeleteMutex( ctx->sticky_id );
  WrapThreadQueueDestroy( &ctx->wrap_tq_ctx );

  if ( rtems_scheduler_get_processor_maximum() >= 4 ) {
    AddProcessor( SCHEDULER_C_ID, 3 );
  }

  RestoreRunnerPriority();
  SetSelfAffinityAll();
  #endif
}

static void RtemsSchedulerReqRemoveProcessor_Teardown_Wrap( void *arg )
{
  RtemsSchedulerReqRemoveProcessor_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsSchedulerReqRemoveProcessor_Teardown( ctx );
}

static void RtemsSchedulerReqRemoveProcessor_Prepare(
  RtemsSchedulerReqRemoveProcessor_Context *ctx
)
{
  ctx->status = RTEMS_NOT_IMPLEMENTED;
}

static void RtemsSchedulerReqRemoveProcessor_Action(
  RtemsSchedulerReqRemoveProcessor_Context *ctx
)
{
  if (
    ctx->id == INVALID_ID ||
    ctx->cpu_index == rtems_configuration_get_maximum_processors() ||
    ( ctx->owned && ctx->last && ctx->home && ctx->required_by_affinity &&
      ( ctx->task || ctx->idle ) )
  ) {
    DoRemoveProcessor( ctx );
  } else {
  #if defined(RTEMS_SMP)
    if ( ctx->owned && !ctx->home && ctx->helping ) {
      RemoveWithHelpingOnly( ctx );
    } else {
      if ( ctx->owned ) {
        rtems_id worker_a;
        rtems_id worker_b;

        worker_a = ctx->worker_id[ WORKER_A ];
        worker_b = ctx->worker_id[ WORKER_B ];

        ctx->cpu_index = 1;

        if ( ctx->last ) {
          ctx->id = SCHEDULER_B_ID;
        } else {
          RemoveProcessor( SCHEDULER_B_ID, 1 );
          AddProcessor( SCHEDULER_A_ID, 1 );
        }

        if ( ctx->home ) {
          SetScheduler( worker_a, ctx->id, PRIO_LOW );

          if ( ctx->required_by_affinity ) {
            SetAffinityOne( worker_a, 1 );
          } else {
            SetAffinityAll( worker_a );
          }
        }

        if ( ctx->idle ) {
          if ( ctx->task ) {
            SendAndSync( ctx, WORKER_A, EVENT_STICKY_OBTAIN );
            SuspendTask( worker_a );
          }
        } else if ( ctx->task ) {
          MakeBusy( ctx, WORKER_A );
        } else if ( ctx->helping ) {
          T_true( ctx->home );

          if ( ctx->last ) {
            SendEvents( worker_b, EVENT_OBTAIN );
            SetPriority( worker_b, PRIO_LOW );
          } else {
            SetScheduler( worker_b, SCHEDULER_C_ID, PRIO_LOW );
            SendAndSync( ctx, WORKER_B, EVENT_OBTAIN );
            MakeBusyAndSync( ctx, WORKER_C );
          }

          SendAndSync( ctx, WORKER_A, EVENT_OBTAIN );
          MakeBusy( ctx, WORKER_B );
          WaitForBusy( ctx, WORKER_B );
        }

        DoRemoveProcessor( ctx );

        if ( ctx->idle ) {
          if ( ctx->task ) {
            ResumeTask( worker_a );
            SendAndSync( ctx, WORKER_A, EVENT_STICKY_RELEASE );
          }
        } else if ( ctx->task ) {
          StopBusyAndWait( ctx, WORKER_A );
        } else if ( ctx->helping ) {
          StopBusy( ctx, WORKER_B );

          if ( ctx->last ) {
            SetPriority( worker_b, PRIO_HIGH );
            SendEvents( worker_b, EVENT_RELEASE );
          } else {
            StopBusyAndWait( ctx, WORKER_C );
            SendAndSync( ctx, WORKER_B, EVENT_RELEASE );
            SetScheduler( worker_b, SCHEDULER_A_ID, PRIO_HIGH );
          }

          WaitForExecutionStop( worker_b );
          SendAndSync( ctx, WORKER_A, EVENT_RELEASE );
        }

        SetAffinityAll( worker_a );
        SetScheduler( worker_a, SCHEDULER_A_ID, PRIO_HIGH );

        if ( !ctx->last ) {
          RemoveProcessor( SCHEDULER_A_ID, 1 );
          AddProcessor( SCHEDULER_B_ID, 1 );
        }
      } else {
        ctx->id = SCHEDULER_B_ID;
        DoRemoveProcessor( ctx );
      }
    }
  #else
    T_unreachable();
  #endif
  }
}

static const RtemsSchedulerReqRemoveProcessor_Entry
RtemsSchedulerReqRemoveProcessor_Entries[] = {
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, RtemsSchedulerReqRemoveProcessor_Post_Status_NA,
    RtemsSchedulerReqRemoveProcessor_Post_Removed_NA },
#else
  { 0, 0, 0, 1, 1, 1, 1, 1, RtemsSchedulerReqRemoveProcessor_Post_Status_InvId,
    RtemsSchedulerReqRemoveProcessor_Post_Removed_Nop },
#endif
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, RtemsSchedulerReqRemoveProcessor_Post_Status_NA,
    RtemsSchedulerReqRemoveProcessor_Post_Removed_NA },
#else
  { 0, 0, 0, 1, 1, 1, 1, 1,
    RtemsSchedulerReqRemoveProcessor_Post_Status_InvNum,
    RtemsSchedulerReqRemoveProcessor_Post_Removed_Nop },
#endif
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, RtemsSchedulerReqRemoveProcessor_Post_Status_NA,
    RtemsSchedulerReqRemoveProcessor_Post_Removed_NA },
#else
  { 0, 0, 0, 0, 1, 1, 1, 1,
    RtemsSchedulerReqRemoveProcessor_Post_Status_InvNum,
    RtemsSchedulerReqRemoveProcessor_Post_Removed_Nop },
#endif
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, RtemsSchedulerReqRemoveProcessor_Post_Status_NA,
    RtemsSchedulerReqRemoveProcessor_Post_Removed_NA },
#else
  { 1, 0, 0, 0, 0, 0, 1, 0, RtemsSchedulerReqRemoveProcessor_Post_Status_NA,
    RtemsSchedulerReqRemoveProcessor_Post_Removed_NA },
#endif
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, RtemsSchedulerReqRemoveProcessor_Post_Status_NA,
    RtemsSchedulerReqRemoveProcessor_Post_Removed_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, RtemsSchedulerReqRemoveProcessor_Post_Status_InUse,
    RtemsSchedulerReqRemoveProcessor_Post_Removed_Nop },
#endif
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, RtemsSchedulerReqRemoveProcessor_Post_Status_NA,
    RtemsSchedulerReqRemoveProcessor_Post_Removed_NA },
#else
  { 0, 0, 0, 0, 0, 0, 1, 0, RtemsSchedulerReqRemoveProcessor_Post_Status_Ok,
    RtemsSchedulerReqRemoveProcessor_Post_Removed_Yes },
#endif
  { 0, 0, 0, 1, 1, 1, 1, 1, RtemsSchedulerReqRemoveProcessor_Post_Status_InvId,
    RtemsSchedulerReqRemoveProcessor_Post_Removed_Nop },
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, RtemsSchedulerReqRemoveProcessor_Post_Status_NA,
    RtemsSchedulerReqRemoveProcessor_Post_Removed_NA },
#else
  { 1, 0, 0, 0, 0, 0, 0, 0, RtemsSchedulerReqRemoveProcessor_Post_Status_NA,
    RtemsSchedulerReqRemoveProcessor_Post_Removed_NA },
#endif
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, RtemsSchedulerReqRemoveProcessor_Post_Status_NA,
    RtemsSchedulerReqRemoveProcessor_Post_Removed_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, RtemsSchedulerReqRemoveProcessor_Post_Status_Ok,
    RtemsSchedulerReqRemoveProcessor_Post_Removed_Yes },
#endif
  { 0, 0, 0, 0, 0, 0, 0, 0, RtemsSchedulerReqRemoveProcessor_Post_Status_InUse,
    RtemsSchedulerReqRemoveProcessor_Post_Removed_Nop },
#if !defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, 0, RtemsSchedulerReqRemoveProcessor_Post_Status_NA,
    RtemsSchedulerReqRemoveProcessor_Post_Removed_NA },
#else
  { 0, 0, 0, 0, 0, 0, 1, 0, RtemsSchedulerReqRemoveProcessor_Post_Status_InUse,
    RtemsSchedulerReqRemoveProcessor_Post_Removed_Nop },
#endif
  { 0, 0, 0, 1, 1, 1, 1, 1,
    RtemsSchedulerReqRemoveProcessor_Post_Status_InvNum,
    RtemsSchedulerReqRemoveProcessor_Post_Removed_Nop }
};

static const uint8_t
RtemsSchedulerReqRemoveProcessor_Map[] = {
  6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 9,
  4, 4, 7, 7, 7, 7, 5, 3, 3, 10, 5, 3, 3, 10, 4, 4, 4, 4, 8, 8, 8, 8, 5, 3, 3,
  5, 5, 3, 3, 5, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 11, 11, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

static size_t RtemsSchedulerReqRemoveProcessor_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  RtemsSchedulerReqRemoveProcessor_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      RtemsSchedulerReqRemoveProcessor_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture RtemsSchedulerReqRemoveProcessor_Fixture = {
  .setup = RtemsSchedulerReqRemoveProcessor_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsSchedulerReqRemoveProcessor_Teardown_Wrap,
  .scope = RtemsSchedulerReqRemoveProcessor_Scope,
  .initial_context = &RtemsSchedulerReqRemoveProcessor_Instance
};

static const uint8_t RtemsSchedulerReqRemoveProcessor_Weights[] = {
  128, 64, 32, 16, 8, 4, 1
};

static void RtemsSchedulerReqRemoveProcessor_Skip(
  RtemsSchedulerReqRemoveProcessor_Context *ctx,
  size_t                                    index
)
{
  switch ( index + 1 ) {
    case 1:
      ctx->Map.pci[ 1 ] = RtemsSchedulerReqRemoveProcessor_Pre_CPUIndex_NA - 1;
      /* Fall through */
    case 2:
      ctx->Map.pci[ 2 ] = RtemsSchedulerReqRemoveProcessor_Pre_Owned_NA - 1;
      /* Fall through */
    case 3:
      ctx->Map.pci[ 3 ] = RtemsSchedulerReqRemoveProcessor_Pre_Last_NA - 1;
      /* Fall through */
    case 4:
      ctx->Map.pci[ 4 ] = RtemsSchedulerReqRemoveProcessor_Pre_Home_NA - 1;
      /* Fall through */
    case 5:
      ctx->Map.pci[ 5 ] = RtemsSchedulerReqRemoveProcessor_Pre_RequiredByAffinity_NA - 1;
      /* Fall through */
    case 6:
      ctx->Map.pci[ 6 ] = RtemsSchedulerReqRemoveProcessor_Pre_UsedBy_NA - 1;
      break;
  }
}

static inline RtemsSchedulerReqRemoveProcessor_Entry
RtemsSchedulerReqRemoveProcessor_PopEntry(
  RtemsSchedulerReqRemoveProcessor_Context *ctx
)
{
  size_t index;

  if ( ctx->Map.skip ) {
    size_t i;

    ctx->Map.skip = false;
    index = 0;

    for ( i = 0; i < 7; ++i ) {
      index += RtemsSchedulerReqRemoveProcessor_Weights[ i ] * ctx->Map.pci[ i ];
    }
  } else {
    index = ctx->Map.index;
  }

  ctx->Map.index = index + 1;

  return RtemsSchedulerReqRemoveProcessor_Entries[
    RtemsSchedulerReqRemoveProcessor_Map[ index ]
  ];
}

static void RtemsSchedulerReqRemoveProcessor_SetPreConditionStates(
  RtemsSchedulerReqRemoveProcessor_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];

  if ( ctx->Map.entry.Pre_Owned_NA ) {
    ctx->Map.pcs[ 2 ] = RtemsSchedulerReqRemoveProcessor_Pre_Owned_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }

  if ( ctx->Map.entry.Pre_Last_NA ) {
    ctx->Map.pcs[ 3 ] = RtemsSchedulerReqRemoveProcessor_Pre_Last_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }

  if ( ctx->Map.entry.Pre_Home_NA ) {
    ctx->Map.pcs[ 4 ] = RtemsSchedulerReqRemoveProcessor_Pre_Home_NA;
  } else {
    ctx->Map.pcs[ 4 ] = ctx->Map.pci[ 4 ];
  }

  if ( ctx->Map.entry.Pre_RequiredByAffinity_NA ) {
    ctx->Map.pcs[ 5 ] = RtemsSchedulerReqRemoveProcessor_Pre_RequiredByAffinity_NA;
  } else {
    ctx->Map.pcs[ 5 ] = ctx->Map.pci[ 5 ];
  }

  if ( ctx->Map.entry.Pre_UsedBy_NA ) {
    ctx->Map.pcs[ 6 ] = RtemsSchedulerReqRemoveProcessor_Pre_UsedBy_NA;
  } else {
    ctx->Map.pcs[ 6 ] = ctx->Map.pci[ 6 ];
  }
}

static void RtemsSchedulerReqRemoveProcessor_TestVariant(
  RtemsSchedulerReqRemoveProcessor_Context *ctx
)
{
  RtemsSchedulerReqRemoveProcessor_Pre_Id_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsSchedulerReqRemoveProcessor_Pre_CPUIndex_Prepare(
    ctx,
    ctx->Map.pcs[ 1 ]
  );
  RtemsSchedulerReqRemoveProcessor_Pre_Owned_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsSchedulerReqRemoveProcessor_Pre_Last_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  RtemsSchedulerReqRemoveProcessor_Pre_Home_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  RtemsSchedulerReqRemoveProcessor_Pre_RequiredByAffinity_Prepare(
    ctx,
    ctx->Map.pcs[ 5 ]
  );
  RtemsSchedulerReqRemoveProcessor_Pre_UsedBy_Prepare(
    ctx,
    ctx->Map.pcs[ 6 ]
  );

  if ( ctx->Map.skip ) {
    RtemsSchedulerReqRemoveProcessor_Skip( ctx, 6 );
    return;
  }

  RtemsSchedulerReqRemoveProcessor_Action( ctx );
  RtemsSchedulerReqRemoveProcessor_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  RtemsSchedulerReqRemoveProcessor_Post_Removed_Check(
    ctx,
    ctx->Map.entry.Post_Removed
  );
}

/**
 * @fn void T_case_body_RtemsSchedulerReqRemoveProcessor( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsSchedulerReqRemoveProcessor,
  &RtemsSchedulerReqRemoveProcessor_Fixture
)
{
  RtemsSchedulerReqRemoveProcessor_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;
  ctx->Map.skip = false;

  for (
    ctx->Map.pci[ 0 ] = RtemsSchedulerReqRemoveProcessor_Pre_Id_Invalid;
    ctx->Map.pci[ 0 ] < RtemsSchedulerReqRemoveProcessor_Pre_Id_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = RtemsSchedulerReqRemoveProcessor_Pre_CPUIndex_Valid;
      ctx->Map.pci[ 1 ] < RtemsSchedulerReqRemoveProcessor_Pre_CPUIndex_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = RtemsSchedulerReqRemoveProcessor_Pre_Owned_Yes;
        ctx->Map.pci[ 2 ] < RtemsSchedulerReqRemoveProcessor_Pre_Owned_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = RtemsSchedulerReqRemoveProcessor_Pre_Last_Yes;
          ctx->Map.pci[ 3 ] < RtemsSchedulerReqRemoveProcessor_Pre_Last_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          for (
            ctx->Map.pci[ 4 ] = RtemsSchedulerReqRemoveProcessor_Pre_Home_Yes;
            ctx->Map.pci[ 4 ] < RtemsSchedulerReqRemoveProcessor_Pre_Home_NA;
            ++ctx->Map.pci[ 4 ]
          ) {
            for (
              ctx->Map.pci[ 5 ] = RtemsSchedulerReqRemoveProcessor_Pre_RequiredByAffinity_Yes;
              ctx->Map.pci[ 5 ] < RtemsSchedulerReqRemoveProcessor_Pre_RequiredByAffinity_NA;
              ++ctx->Map.pci[ 5 ]
            ) {
              for (
                ctx->Map.pci[ 6 ] = RtemsSchedulerReqRemoveProcessor_Pre_UsedBy_Idle;
                ctx->Map.pci[ 6 ] < RtemsSchedulerReqRemoveProcessor_Pre_UsedBy_NA;
                ++ctx->Map.pci[ 6 ]
              ) {
                ctx->Map.entry = RtemsSchedulerReqRemoveProcessor_PopEntry(
                  ctx
                );

                if ( ctx->Map.entry.Skip ) {
                  continue;
                }

                RtemsSchedulerReqRemoveProcessor_SetPreConditionStates( ctx );
                RtemsSchedulerReqRemoveProcessor_Prepare( ctx );
                RtemsSchedulerReqRemoveProcessor_TestVariant( ctx );
              }
            }
          }
        }
      }
    }
  }
}

/** @} */
