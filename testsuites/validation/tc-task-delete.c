/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsTaskReqDelete
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

#include <limits.h>
#include <rtems.h>
#include <setjmp.h>
#include <rtems/bspIo.h>
#include <rtems/test-scheduler.h>
#include <rtems/score/statesimpl.h>
#include <rtems/score/threaddispatch.h>
#include <rtems/score/threadimpl.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsTaskReqDelete spec:/rtems/task/req/delete
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsTaskReqDelete_Pre_Id_Executing,
  RtemsTaskReqDelete_Pre_Id_Other,
  RtemsTaskReqDelete_Pre_Id_Invalid,
  RtemsTaskReqDelete_Pre_Id_NA
} RtemsTaskReqDelete_Pre_Id;

typedef enum {
  RtemsTaskReqDelete_Pre_Context_Task,
  RtemsTaskReqDelete_Pre_Context_Interrupt,
  RtemsTaskReqDelete_Pre_Context_NA
} RtemsTaskReqDelete_Pre_Context;

typedef enum {
  RtemsTaskReqDelete_Pre_ThreadDispatch_Disabled,
  RtemsTaskReqDelete_Pre_ThreadDispatch_Enabled,
  RtemsTaskReqDelete_Pre_ThreadDispatch_NA
} RtemsTaskReqDelete_Pre_ThreadDispatch;

typedef enum {
  RtemsTaskReqDelete_Pre_CallerPriority_Vital,
  RtemsTaskReqDelete_Pre_CallerPriority_Dispensable,
  RtemsTaskReqDelete_Pre_CallerPriority_NA
} RtemsTaskReqDelete_Pre_CallerPriority;

typedef enum {
  RtemsTaskReqDelete_Pre_Dormant_No,
  RtemsTaskReqDelete_Pre_Dormant_Yes,
  RtemsTaskReqDelete_Pre_Dormant_NA
} RtemsTaskReqDelete_Pre_Dormant;

typedef enum {
  RtemsTaskReqDelete_Pre_Suspended_Yes,
  RtemsTaskReqDelete_Pre_Suspended_No,
  RtemsTaskReqDelete_Pre_Suspended_NA
} RtemsTaskReqDelete_Pre_Suspended;

typedef enum {
  RtemsTaskReqDelete_Pre_Restarting_No,
  RtemsTaskReqDelete_Pre_Restarting_Yes,
  RtemsTaskReqDelete_Pre_Restarting_NA
} RtemsTaskReqDelete_Pre_Restarting;

typedef enum {
  RtemsTaskReqDelete_Pre_Terminating_No,
  RtemsTaskReqDelete_Pre_Terminating_Yes,
  RtemsTaskReqDelete_Pre_Terminating_NA
} RtemsTaskReqDelete_Pre_Terminating;

typedef enum {
  RtemsTaskReqDelete_Pre_Protected_Yes,
  RtemsTaskReqDelete_Pre_Protected_No,
  RtemsTaskReqDelete_Pre_Protected_NA
} RtemsTaskReqDelete_Pre_Protected;

typedef enum {
  RtemsTaskReqDelete_Pre_State_Enqueued,
  RtemsTaskReqDelete_Pre_State_Ready,
  RtemsTaskReqDelete_Pre_State_Blocked,
  RtemsTaskReqDelete_Pre_State_NA
} RtemsTaskReqDelete_Pre_State;

typedef enum {
  RtemsTaskReqDelete_Pre_Timer_Inactive,
  RtemsTaskReqDelete_Pre_Timer_Active,
  RtemsTaskReqDelete_Pre_Timer_NA
} RtemsTaskReqDelete_Pre_Timer;

typedef enum {
  RtemsTaskReqDelete_Post_Status_Ok,
  RtemsTaskReqDelete_Post_Status_InvId,
  RtemsTaskReqDelete_Post_Status_CalledFromISR,
  RtemsTaskReqDelete_Post_Status_NoReturn,
  RtemsTaskReqDelete_Post_Status_NA
} RtemsTaskReqDelete_Post_Status;

typedef enum {
  RtemsTaskReqDelete_Post_FatalError_Yes,
  RtemsTaskReqDelete_Post_FatalError_Nop,
  RtemsTaskReqDelete_Post_FatalError_NA
} RtemsTaskReqDelete_Post_FatalError;

typedef enum {
  RtemsTaskReqDelete_Post_Zombie_Yes,
  RtemsTaskReqDelete_Post_Zombie_No,
  RtemsTaskReqDelete_Post_Zombie_NA
} RtemsTaskReqDelete_Post_Zombie;

typedef enum {
  RtemsTaskReqDelete_Post_TaskPriority_Raise,
  RtemsTaskReqDelete_Post_TaskPriority_Nop,
  RtemsTaskReqDelete_Post_TaskPriority_NA
} RtemsTaskReqDelete_Post_TaskPriority;

typedef enum {
  RtemsTaskReqDelete_Post_RestartExtensions_Nop,
  RtemsTaskReqDelete_Post_RestartExtensions_NA
} RtemsTaskReqDelete_Post_RestartExtensions;

typedef enum {
  RtemsTaskReqDelete_Post_TerminateExtensions_Yes,
  RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
  RtemsTaskReqDelete_Post_TerminateExtensions_NA
} RtemsTaskReqDelete_Post_TerminateExtensions;

typedef enum {
  RtemsTaskReqDelete_Post_Dormant_Yes,
  RtemsTaskReqDelete_Post_Dormant_No,
  RtemsTaskReqDelete_Post_Dormant_NA
} RtemsTaskReqDelete_Post_Dormant;

typedef enum {
  RtemsTaskReqDelete_Post_Suspended_Yes,
  RtemsTaskReqDelete_Post_Suspended_No,
  RtemsTaskReqDelete_Post_Suspended_NA
} RtemsTaskReqDelete_Post_Suspended;

typedef enum {
  RtemsTaskReqDelete_Post_Restarting_Yes,
  RtemsTaskReqDelete_Post_Restarting_No,
  RtemsTaskReqDelete_Post_Restarting_NA
} RtemsTaskReqDelete_Post_Restarting;

typedef enum {
  RtemsTaskReqDelete_Post_Terminating_Yes,
  RtemsTaskReqDelete_Post_Terminating_No,
  RtemsTaskReqDelete_Post_Terminating_NA
} RtemsTaskReqDelete_Post_Terminating;

typedef enum {
  RtemsTaskReqDelete_Post_Protected_Yes,
  RtemsTaskReqDelete_Post_Protected_No,
  RtemsTaskReqDelete_Post_Protected_NA
} RtemsTaskReqDelete_Post_Protected;

typedef enum {
  RtemsTaskReqDelete_Post_State_Enqueued,
  RtemsTaskReqDelete_Post_State_Ready,
  RtemsTaskReqDelete_Post_State_Blocked,
  RtemsTaskReqDelete_Post_State_NA
} RtemsTaskReqDelete_Post_State;

typedef enum {
  RtemsTaskReqDelete_Post_Timer_Active,
  RtemsTaskReqDelete_Post_Timer_Inactive,
  RtemsTaskReqDelete_Post_Timer_NA
} RtemsTaskReqDelete_Post_Timer;

typedef struct {
  uint64_t Skip : 1;
  uint64_t Pre_Id_NA : 1;
  uint64_t Pre_Context_NA : 1;
  uint64_t Pre_ThreadDispatch_NA : 1;
  uint64_t Pre_CallerPriority_NA : 1;
  uint64_t Pre_Dormant_NA : 1;
  uint64_t Pre_Suspended_NA : 1;
  uint64_t Pre_Restarting_NA : 1;
  uint64_t Pre_Terminating_NA : 1;
  uint64_t Pre_Protected_NA : 1;
  uint64_t Pre_State_NA : 1;
  uint64_t Pre_Timer_NA : 1;
  uint64_t Post_Status : 3;
  uint64_t Post_FatalError : 2;
  uint64_t Post_Zombie : 2;
  uint64_t Post_TaskPriority : 2;
  uint64_t Post_RestartExtensions : 1;
  uint64_t Post_TerminateExtensions : 2;
  uint64_t Post_Dormant : 2;
  uint64_t Post_Suspended : 2;
  uint64_t Post_Restarting : 2;
  uint64_t Post_Terminating : 2;
  uint64_t Post_Protected : 2;
  uint64_t Post_State : 2;
  uint64_t Post_Timer : 2;
} RtemsTaskReqDelete_Entry;

/**
 * @brief Test context for spec:/rtems/task/req/delete test case.
 */
typedef struct {
  /**
   * @brief This member provides the scheduler operation records.
   */
  T_scheduler_log_10 scheduler_log;

  /**
   * @brief This member provides a jump context to resume a thread dispatch.
   */
  jmp_buf thread_dispatch_context;

  /**
   * @brief This member contains the identifier of the runner scheduler.
   */
  rtems_id scheduler_id;

  /**
   * @brief This member contains the identifier of the runner task.
   */
  rtems_id runner_id;

  /**
   * @brief This member references the TCB of the runner task.
   */
  rtems_tcb *runner_tcb;

  /**
   * @brief This member contains the identifier of the mutex.
   */
  rtems_id mutex_id;

  /**
   * @brief This member provides an event set used to set up the blocking
   *   conditions of the task to delete.
   */
  rtems_event_set events;

  /**
   * @brief This member contains the identifier of the worker task.
   */
  rtems_id worker_id;

  /**
   * @brief This member references the TCB of the worker task.
   */
  rtems_tcb *worker_tcb;

  /**
   * @brief This member contains the worker state at the end of the
   *   rtems_task_delete() call.
   */
  States_Control worker_state;

  /**
   * @brief This member contains the worker timer info at the end of the
   *   rtems_task_delete() call.
   */
  TaskTimerInfo worker_timer_info;

  /**
   * @brief This member contains the worker thread queue at the end of the
   *   rtems_task_delete() call.
   */
  const Thread_queue_Queue *worker_wait_queue;

  /**
   * @brief This member contains the worker thread life state at the end of the
   *   rtems_task_delete() call.
   */
  Thread_Life_state worker_life_state;

  /**
   * @brief This member contains the worker priority at the end of the
   *   rtems_task_delete() call.
   */
  rtems_task_priority worker_priority;

  /**
   * @brief This member contains the identifier of the deleter task.
   */
  rtems_id deleter_id;

  /**
   * @brief This member references the TCB of the deleter task.
   */
  rtems_tcb *deleter_tcb;

  /**
   * @brief This member contains the identifier of the second deleter task.
   */
  rtems_id deleter_2_id;

  /**
   * @brief This member references the TCB of the second deleter task.
   */
  rtems_tcb *deleter_2_tcb;

  /**
   * @brief This member contains the identifier of the test user extensions.
   */
  rtems_id extension_id;

  /**
   * @brief This member contains extension calls.
   */
  ExtensionCalls calls;

  /**
   * @brief This member contains extension calls after the rtems_task_delete()
   *   call.
   */
  ExtensionCalls calls_after_restart;

  /**
   * @brief This member contains the delete counter.
   */
  uint32_t restart_counter;

  /**
   * @brief If this member is true, then the worker shall be dormant before the
   *   rtems_task_delete() call.
   */
  bool dormant;

  /**
   * @brief If this member is true, then the worker shall be suspended before
   *   the rtems_task_delete() call.
   */
  bool suspended;

  /**
   * @brief If this member is true, then the thread life of the worker shall be
   *   protected before the rtems_task_delete() call.
   */
  bool protected;

  /**
   * @brief If this member is true, then the worker shall be restarting before
   *   the rtems_task_delete() call.
   */
  bool restarting;

  /**
   * @brief If this member is true, then the worker shall be terminating before
   *   the rtems_task_delete() call.
   */
  bool terminating;

  /**
   * @brief If this member is true, then the rtems_task_delete() shall be
   *   called from within interrupt context.
   */
  bool interrupt;

  /**
   * @brief If this member is true, then the worker shall be blocked before the
   *   rtems_task_delete() call.
   */
  bool blocked;

  /**
   * @brief If this member is true, then the worker shall be enqueued on a wait
   *   queue before the rtems_task_delete() call.
   */
  bool enqueued;

  /**
   * @brief If this member is true, then the worker obtained a mutex.
   */
  bool worker_is_mutex_owner;

  /**
   * @brief If this member is true, then the timer of the worker shall be
   *   active before the rtems_task_delete() call.
   */
  bool timer_active;

  /**
   * @brief If this member is true, then the deleter shall have a vital
   *   priority for the worker.
   */
  bool vital_deleter_priority;

  /**
   * @brief If this member is true, then thread dispatching is disabled by the
   *   worker task before the rtems_task_delete() call.
   */
  bool dispatch_disabled;

  /**
   * @brief If this member is true, then it is expected to delete the worker.
   */
  bool delete_worker_expected;

  /**
   * @brief This member contains the return value of the rtems_task_delete()
   *   call.
   */
  rtems_status_code status;

  /**
   * @brief This member specifies if the ``id`` parameter value.
   */
  rtems_id id;

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 11 ];

    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 11 ];

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
    RtemsTaskReqDelete_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsTaskReqDelete_Context;

static RtemsTaskReqDelete_Context
  RtemsTaskReqDelete_Instance;

static const char * const RtemsTaskReqDelete_PreDesc_Id[] = {
  "Executing",
  "Other",
  "Invalid",
  "NA"
};

static const char * const RtemsTaskReqDelete_PreDesc_Context[] = {
  "Task",
  "Interrupt",
  "NA"
};

static const char * const RtemsTaskReqDelete_PreDesc_ThreadDispatch[] = {
  "Disabled",
  "Enabled",
  "NA"
};

static const char * const RtemsTaskReqDelete_PreDesc_CallerPriority[] = {
  "Vital",
  "Dispensable",
  "NA"
};

static const char * const RtemsTaskReqDelete_PreDesc_Dormant[] = {
  "No",
  "Yes",
  "NA"
};

static const char * const RtemsTaskReqDelete_PreDesc_Suspended[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const RtemsTaskReqDelete_PreDesc_Restarting[] = {
  "No",
  "Yes",
  "NA"
};

static const char * const RtemsTaskReqDelete_PreDesc_Terminating[] = {
  "No",
  "Yes",
  "NA"
};

static const char * const RtemsTaskReqDelete_PreDesc_Protected[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const RtemsTaskReqDelete_PreDesc_State[] = {
  "Enqueued",
  "Ready",
  "Blocked",
  "NA"
};

static const char * const RtemsTaskReqDelete_PreDesc_Timer[] = {
  "Inactive",
  "Active",
  "NA"
};

static const char * const * const RtemsTaskReqDelete_PreDesc[] = {
  RtemsTaskReqDelete_PreDesc_Id,
  RtemsTaskReqDelete_PreDesc_Context,
  RtemsTaskReqDelete_PreDesc_ThreadDispatch,
  RtemsTaskReqDelete_PreDesc_CallerPriority,
  RtemsTaskReqDelete_PreDesc_Dormant,
  RtemsTaskReqDelete_PreDesc_Suspended,
  RtemsTaskReqDelete_PreDesc_Restarting,
  RtemsTaskReqDelete_PreDesc_Terminating,
  RtemsTaskReqDelete_PreDesc_Protected,
  RtemsTaskReqDelete_PreDesc_State,
  RtemsTaskReqDelete_PreDesc_Timer,
  NULL
};

typedef RtemsTaskReqDelete_Context Context;

static void CaptureWorkerState( Context *ctx )
{
  T_scheduler_log *log;

  log = T_scheduler_record( NULL );

  if ( log != NULL ) {
    T_eq_ptr( &log->header, &ctx->scheduler_log.header );

    ctx->worker_wait_queue = ctx->worker_tcb->Wait.queue;
    ctx->worker_state = ctx->worker_tcb->current_state;
    ctx->worker_life_state = ctx->worker_tcb->Life.state;
    ctx->worker_priority =
      SCHEDULER_PRIORITY_UNMAP( _Thread_Get_priority( ctx->worker_tcb ) );
    CopyExtensionCalls( &ctx->calls, &ctx->calls_after_restart );
    GetTaskTimerInfoByThread( ctx->worker_tcb, &ctx->worker_timer_info );
  }
}

static void TaskSwitch( rtems_tcb *executing, rtems_tcb *heir )
{
  (void) executing;
  (void) heir;
  CaptureWorkerState( T_fixture_context() );
}

static void VerifyTaskPreparation( const Context *ctx )
{
  if ( ctx->id != INVALID_ID ) {
    States_Control state;
    Thread_Life_state life_state;

    state = STATES_READY;
    life_state = ctx->worker_tcb->Life.state;

    if ( ctx->suspended ) {
      state |= STATES_SUSPENDED;
    }

    if ( ctx->dormant ) {
      T_eq_int( life_state, 0 );
      state |= STATES_DORMANT;
    } else {
      T_eq( ctx->protected, ( life_state & THREAD_LIFE_PROTECTED ) != 0 );
      T_eq( ctx->restarting, ( life_state & THREAD_LIFE_RESTARTING ) != 0 );
      T_eq( ctx->terminating, ( life_state & THREAD_LIFE_TERMINATING ) != 0 );

      if ( ctx->blocked ) {
        if ( ctx->enqueued ) {
          state |= STATES_WAITING_FOR_MUTEX;
        } else {
          state |= STATES_WAITING_FOR_EVENT;
        }
      }
    }

    T_eq_u32( ctx->worker_tcb->current_state, state );
  }
}

static void Fatal(
  rtems_fatal_source source,
  rtems_fatal_code   code,
  void              *arg
)
{
  Context *ctx;

  T_eq_int( source, INTERNAL_ERROR_CORE );
  T_eq_ulong( code, INTERNAL_ERROR_BAD_THREAD_DISPATCH_DISABLE_LEVEL );

  ctx = arg;
  ++ctx->calls.fatal;
  T_assert_eq_int( ctx->calls.fatal, 1 );
  longjmp( ctx->thread_dispatch_context, 1 );
}

static void ResumeThreadDispatch(
  rtems_fatal_source source,
  rtems_fatal_code   code,
  void              *arg
)
{
  Context *ctx;

  T_eq_int( source, INTERNAL_ERROR_CORE );
  T_eq_ulong( code, INTERNAL_ERROR_BAD_THREAD_DISPATCH_DISABLE_LEVEL );

  ctx = arg;
  SetFatalHandler( Fatal, ctx );
  longjmp( ctx->thread_dispatch_context, 1 );
}

static void Delete( void *arg )
{
  Context         *ctx;
  T_scheduler_log *log;

  ctx = arg;

  if ( ctx->suspended && ctx->id != INVALID_ID ) {
    if ( ctx->id != RTEMS_SELF || ctx->interrupt ) {
      SuspendTask( ctx->worker_id );
    } else {
      Per_CPU_Control *cpu_self;

      /*
       * Where the system was built with SMP support enabled, a suspended
       * executing thread during the rtems_task_delete() call can happen
       * if the thread was suspended by another processor and the
       * inter-processor interrupt did not yet arrive.  Where the system was
       * built with SMP support disabled, this state cannot happen with the
       * current implementation.  However, we still specify and validate this
       * behaviour unconditionally since there exist alternative
       * implementations which would lead to such a state if the executing
       * thread is suspended by an ISR.
       */
      cpu_self = _Thread_Dispatch_disable();
      SuspendSelf();
      cpu_self->dispatch_necessary = false;
      _Thread_Dispatch_enable( cpu_self );
    }
  }

  if ( ctx->dispatch_disabled ) {
    _Thread_Dispatch_disable();
  }

  VerifyTaskPreparation( ctx );
  ClearExtensionCalls( &ctx->calls );

  log = T_scheduler_record_10( &ctx->scheduler_log );
  T_null( log );

  if ( setjmp( ctx->thread_dispatch_context ) == 0 ) {
    ctx->status = rtems_task_delete( ctx->id );
  } else {
    _Thread_Dispatch_unnest( _Per_CPU_Get() );
  }

  CaptureWorkerState( ctx );

  if ( ctx->dispatch_disabled ) {
    _Thread_Dispatch_enable( _Per_CPU_Get() );
  }
}

static void Block( Context *ctx )
{
  rtems_interval ticks;

  if ( ctx->timer_active ) {
    ticks = UINT32_MAX;
  } else {
    ticks = RTEMS_NO_TIMEOUT;
  }

  if ( ctx->enqueued ) {
    ObtainMutexTimed( ctx->mutex_id, ticks );
    ctx->worker_is_mutex_owner = true;
  } else {
    /*
     * Do not use a stack variable for the event set, since we may jump out
     * of the directive call.
     */
    (void) rtems_event_receive(
      RTEMS_ALL_EVENTS,
      RTEMS_EVENT_ANY | RTEMS_WAIT,
      ticks,
      &ctx->events
    );
  }
}

static void BlockDone( Context *ctx )
{
  if ( ctx->enqueued ) {
    ReleaseMutex( ctx->mutex_id );
  }
}

static void Signal( rtems_signal_set signals )
{
  Context *ctx;

  (void) signals;
  ctx = T_fixture_context();

  if ( ctx->id == RTEMS_SELF ) {
    SetPriority( ctx->runner_id, PRIO_LOW );

    if ( ctx->interrupt ) {
      if ( ctx->blocked ) {
        Per_CPU_Control *cpu_self;

        SetFatalHandler( ResumeThreadDispatch, ctx );
        cpu_self = _Thread_Dispatch_disable();

        if ( setjmp( ctx->thread_dispatch_context ) == 0 ) {
          Block( ctx );
        } else {
          _Thread_Dispatch_unnest( cpu_self );
        }

        CallWithinISR( Delete, ctx );

        _Thread_Dispatch_direct( cpu_self );
        BlockDone( ctx );
      } else {
        CallWithinISR( Delete, ctx );
      }
    } else {
      Delete( ctx );
    }
  } else {
    if ( ctx->blocked ) {
      Block( ctx );
      BlockDone( ctx );
    } else {
      SetPriority( ctx->runner_id, PRIO_HIGH );
    }
  }

  if ( ctx->protected ) {
    _Thread_Set_life_protection( 0 );
  }
}

static void Deleter( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;

  if ( ctx != NULL ) {
    /* We have to prevent the priority boost in the task delete below */
    SetPriority( ctx->runner_id, PRIO_LOW );
    SetSelfPriorityNoYield( PRIO_NORMAL );

    DeleteTask( ctx->worker_id );
  }

  SuspendSelf();
}

static void SecondDeleter( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;

  if ( ctx != NULL ) {
    if ( !ctx->vital_deleter_priority ) {
      SetPriority( ctx->runner_id, PRIO_LOW );
      SetSelfPriorityNoYield( PRIO_NORMAL );
    }

    Delete( ctx );
  }

  SuspendSelf();
}

static void Worker( rtems_task_argument arg )
{
  Context *ctx;

  ctx = T_fixture_context();

  if ( arg != 0 ) {
    rtems_status_code sc;

    sc = rtems_signal_catch( Signal, RTEMS_NO_ASR );
    T_rsc_success( sc );

    if ( ctx->protected ) {
      _Thread_Set_life_protection( THREAD_LIFE_PROTECTED );
    }

    Yield();
  }

  if ( IsMutexOwner( ctx->mutex_id ) ) {
    ReleaseMutex( ctx->mutex_id );
  }

  rtems_task_exit();
}

static void ThreadDelete( rtems_tcb *executing, rtems_tcb *deleted )
{
  Context *ctx;

  ctx = T_fixture_context();
  ++ctx->calls.thread_delete;

  T_eq_u32( executing->Object.id, ctx->runner_id );

  if ( ctx->delete_worker_expected ) {
    T_eq_u32( deleted->Object.id, ctx->worker_id );
  }
}

static void ThreadRestart( rtems_tcb *executing, rtems_tcb *restarted )
{
  Context *ctx;

  ctx = T_fixture_context();
  ++ctx->calls.thread_restart;
}

static void ThreadTerminate( rtems_tcb *executing )
{
  Context *ctx;

  ctx = T_fixture_context();
  ++ctx->calls.thread_terminate;

  T_eq_u32( executing->Object.id, ctx->worker_id );

  if ( IsMutexOwner( ctx->mutex_id ) ) {
    ReleaseMutex( ctx->mutex_id );
  }
}

static void Cleanup( Context *ctx )
{
  SetSelfPriority( PRIO_VERY_LOW );

  if ( ( ctx->id == RTEMS_SELF || ctx->interrupt ) && ctx->suspended ) {
    ResumeTask( ctx->worker_id );
  }

  if ( ctx->protected && ctx->blocked ) {
    if ( ctx->enqueued ) {
      ReleaseMutex( ctx->mutex_id );
      ObtainMutex( ctx->mutex_id );
    } else {
      SendEvents( ctx->worker_id, RTEMS_EVENT_0 );
    }
  }

  if (
    ctx->id == INVALID_ID ||
    ( ctx->calls.thread_terminate == 0 && 
    !( ctx->dormant && ctx->status != RTEMS_CALLED_FROM_ISR ) )
  ) {
    DeleteTask( ctx->worker_id );
  }

  SetSelfPriority( PRIO_NORMAL );
}

static const rtems_extensions_table extensions = {
  .thread_delete = ThreadDelete,
  .thread_restart = ThreadRestart,
  .thread_terminate = ThreadTerminate
};

static void RtemsTaskReqDelete_Pre_Id_Prepare(
  RtemsTaskReqDelete_Context *ctx,
  RtemsTaskReqDelete_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsTaskReqDelete_Pre_Id_Executing: {
      /*
       * While the ``id`` parameter is associated with the calling task.
       */
      ctx->id = RTEMS_SELF;
      break;
    }

    case RtemsTaskReqDelete_Pre_Id_Other: {
      /*
       * While the ``id`` parameter is associated with a task other than the
       * calling task.
       */
      ctx->id = ctx->worker_id;
      break;
    }

    case RtemsTaskReqDelete_Pre_Id_Invalid: {
      /*
       * While the ``id`` parameter is not associated with a task.
       */
      ctx->id = INVALID_ID;
      break;
    }

    case RtemsTaskReqDelete_Pre_Id_NA:
      break;
  }
}

static void RtemsTaskReqDelete_Pre_Context_Prepare(
  RtemsTaskReqDelete_Context    *ctx,
  RtemsTaskReqDelete_Pre_Context state
)
{
  switch ( state ) {
    case RtemsTaskReqDelete_Pre_Context_Task: {
      /*
       * While the rtems_task_delete() directive is called from within task
       * context.
       */
      ctx->interrupt = false;
      break;
    }

    case RtemsTaskReqDelete_Pre_Context_Interrupt: {
      /*
       * While the rtems_task_delete() directive is called from within
       * interrupt context.
       */
      ctx->interrupt = true;
      break;
    }

    case RtemsTaskReqDelete_Pre_Context_NA:
      break;
  }
}

static void RtemsTaskReqDelete_Pre_ThreadDispatch_Prepare(
  RtemsTaskReqDelete_Context           *ctx,
  RtemsTaskReqDelete_Pre_ThreadDispatch state
)
{
  switch ( state ) {
    case RtemsTaskReqDelete_Pre_ThreadDispatch_Disabled: {
      /*
       * While thread dispatching is disabled for the calling task.
       */
      ctx->dispatch_disabled = true;
      break;
    }

    case RtemsTaskReqDelete_Pre_ThreadDispatch_Enabled: {
      /*
       * While thread dispatching is enabled for the calling task.
       */
      ctx->dispatch_disabled = false;
      break;
    }

    case RtemsTaskReqDelete_Pre_ThreadDispatch_NA:
      break;
  }
}

static void RtemsTaskReqDelete_Pre_CallerPriority_Prepare(
  RtemsTaskReqDelete_Context           *ctx,
  RtemsTaskReqDelete_Pre_CallerPriority state
)
{
  switch ( state ) {
    case RtemsTaskReqDelete_Pre_CallerPriority_Vital: {
      /*
       * While at least one priority of the calling task is higher than the
       * highest priority of the task specified by the ``id`` parameter.
       */
      ctx->vital_deleter_priority = true;
      break;
    }

    case RtemsTaskReqDelete_Pre_CallerPriority_Dispensable: {
      /*
       * While all priorities of the calling task are lower than or equal to
       * the highest priority of the task specified by the ``id`` parameter.
       */
      ctx->vital_deleter_priority = false;
      break;
    }

    case RtemsTaskReqDelete_Pre_CallerPriority_NA:
      break;
  }
}

static void RtemsTaskReqDelete_Pre_Dormant_Prepare(
  RtemsTaskReqDelete_Context    *ctx,
  RtemsTaskReqDelete_Pre_Dormant state
)
{
  switch ( state ) {
    case RtemsTaskReqDelete_Pre_Dormant_No: {
      /*
       * While the task specified by the ``id`` parameter is not dormant.
       */
      ctx->dormant = false;
      break;
    }

    case RtemsTaskReqDelete_Pre_Dormant_Yes: {
      /*
       * While the task specified by the ``id`` parameter is dormant.
       */
      ctx->dormant = true;
      break;
    }

    case RtemsTaskReqDelete_Pre_Dormant_NA:
      break;
  }
}

static void RtemsTaskReqDelete_Pre_Suspended_Prepare(
  RtemsTaskReqDelete_Context      *ctx,
  RtemsTaskReqDelete_Pre_Suspended state
)
{
  switch ( state ) {
    case RtemsTaskReqDelete_Pre_Suspended_Yes: {
      /*
       * While the task specified by the ``id`` parameter is suspended.
       */
      ctx->suspended = true;
      break;
    }

    case RtemsTaskReqDelete_Pre_Suspended_No: {
      /*
       * While the task specified by the ``id`` parameter is not suspended.
       */
      ctx->suspended = false;
      break;
    }

    case RtemsTaskReqDelete_Pre_Suspended_NA:
      break;
  }
}

static void RtemsTaskReqDelete_Pre_Restarting_Prepare(
  RtemsTaskReqDelete_Context       *ctx,
  RtemsTaskReqDelete_Pre_Restarting state
)
{
  switch ( state ) {
    case RtemsTaskReqDelete_Pre_Restarting_No: {
      /*
       * While the task specified by the ``id`` parameter is not restarting.
       */
      ctx->restarting = false;
      break;
    }

    case RtemsTaskReqDelete_Pre_Restarting_Yes: {
      /*
       * While the task specified by the ``id`` parameter is restarting.
       */
      ctx->restarting = true;
      break;
    }

    case RtemsTaskReqDelete_Pre_Restarting_NA:
      break;
  }
}

static void RtemsTaskReqDelete_Pre_Terminating_Prepare(
  RtemsTaskReqDelete_Context        *ctx,
  RtemsTaskReqDelete_Pre_Terminating state
)
{
  switch ( state ) {
    case RtemsTaskReqDelete_Pre_Terminating_No: {
      /*
       * While the task specified by the ``id`` parameter is not terminating.
       */
      ctx->terminating = false;
      break;
    }

    case RtemsTaskReqDelete_Pre_Terminating_Yes: {
      /*
       * While the task specified by the ``id`` parameter is terminating.
       */
      ctx->terminating = true;
      break;
    }

    case RtemsTaskReqDelete_Pre_Terminating_NA:
      break;
  }
}

static void RtemsTaskReqDelete_Pre_Protected_Prepare(
  RtemsTaskReqDelete_Context      *ctx,
  RtemsTaskReqDelete_Pre_Protected state
)
{
  switch ( state ) {
    case RtemsTaskReqDelete_Pre_Protected_Yes: {
      /*
       * While thread life of the task specified by the ``id`` parameter is
       * protected.
       */
      ctx->protected = true;
      break;
    }

    case RtemsTaskReqDelete_Pre_Protected_No: {
      /*
       * While thread life of the task specified by the ``id`` parameter is not
       * protected.
       */
      ctx->protected = false;
      break;
    }

    case RtemsTaskReqDelete_Pre_Protected_NA:
      break;
  }
}

static void RtemsTaskReqDelete_Pre_State_Prepare(
  RtemsTaskReqDelete_Context  *ctx,
  RtemsTaskReqDelete_Pre_State state
)
{
  switch ( state ) {
    case RtemsTaskReqDelete_Pre_State_Enqueued: {
      /*
       * While the task specified by the ``id`` parameter is enqueued on a wait
       * queue.
       */
      ctx->blocked = true;
      ctx->enqueued = true;
      break;
    }

    case RtemsTaskReqDelete_Pre_State_Ready: {
      /*
       * While the task specified by the ``id`` parameter is a ready task or a
       * scheduled task.
       */
      ctx->blocked = false;
      ctx->enqueued = false;
      break;
    }

    case RtemsTaskReqDelete_Pre_State_Blocked: {
      /*
       * While the task specified by the ``id`` parameter is blocked.
       */
      ctx->blocked = true;
      ctx->enqueued = false;
      break;
    }

    case RtemsTaskReqDelete_Pre_State_NA:
      break;
  }
}

static void RtemsTaskReqDelete_Pre_Timer_Prepare(
  RtemsTaskReqDelete_Context  *ctx,
  RtemsTaskReqDelete_Pre_Timer state
)
{
  switch ( state ) {
    case RtemsTaskReqDelete_Pre_Timer_Inactive: {
      /*
       * While timer of the task specified by the ``id`` parameter is inactive.
       */
      ctx->timer_active = false;
      break;
    }

    case RtemsTaskReqDelete_Pre_Timer_Active: {
      /*
       * While timer of the task specified by the ``id`` parameter is active.
       */
      ctx->timer_active = true;
      break;
    }

    case RtemsTaskReqDelete_Pre_Timer_NA:
      break;
  }
}

static void RtemsTaskReqDelete_Post_Status_Check(
  RtemsTaskReqDelete_Context    *ctx,
  RtemsTaskReqDelete_Post_Status state
)
{
  switch ( state ) {
    case RtemsTaskReqDelete_Post_Status_Ok: {
      /*
       * The return status of rtems_task_delete() shall be RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsTaskReqDelete_Post_Status_InvId: {
      /*
       * The return status of rtems_task_delete() shall be RTEMS_INVALID_ID.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ID );
      break;
    }

    case RtemsTaskReqDelete_Post_Status_CalledFromISR: {
      /*
       * The return status of rtems_task_delete() shall be
       * RTEMS_CALLED_FROM_ISR.
       */
      T_rsc( ctx->status, RTEMS_CALLED_FROM_ISR );
      break;
    }

    case RtemsTaskReqDelete_Post_Status_NoReturn: {
      /*
       * The rtems_task_delete() call shall not return.
       */
      T_rsc( ctx->status, RTEMS_NOT_IMPLEMENTED );
      break;
    }

    case RtemsTaskReqDelete_Post_Status_NA:
      break;
  }
}

static void RtemsTaskReqDelete_Post_FatalError_Check(
  RtemsTaskReqDelete_Context        *ctx,
  RtemsTaskReqDelete_Post_FatalError state
)
{
  switch ( state ) {
    case RtemsTaskReqDelete_Post_FatalError_Yes: {
      /*
       * The fatal error with a fatal source of INTERNAL_ERROR_CORE and a fatal
       * code of INTERNAL_ERROR_BAD_THREAD_DISPATCH_DISABLE_LEVEL shall occur
       * through the rtems_task_delete() call.
       */
      T_eq_u32( ctx->calls.fatal, 1 );
      break;
    }

    case RtemsTaskReqDelete_Post_FatalError_Nop: {
      /*
       * No fatal error shall occur through the rtems_task_delete() call.
       */
      T_eq_u32( ctx->calls.fatal, 0 );
      break;
    }

    case RtemsTaskReqDelete_Post_FatalError_NA:
      break;
  }
}

static void RtemsTaskReqDelete_Post_Zombie_Check(
  RtemsTaskReqDelete_Context    *ctx,
  RtemsTaskReqDelete_Post_Zombie state
)
{
  switch ( state ) {
    case RtemsTaskReqDelete_Post_Zombie_Yes: {
      /*
       * The task specified by the ``id`` parameter shall be in the zombie
       * state after the rtems_task_delete() call.
       */
      T_eq_u32( ctx->worker_state & STATES_ZOMBIE, STATES_ZOMBIE )
      break;
    }

    case RtemsTaskReqDelete_Post_Zombie_No: {
      /*
       * The task specified by the ``id`` parameter shall not be in the zombie
       * state after the rtems_task_delete() call.
       */
      T_eq_u32( ctx->worker_state & STATES_ZOMBIE, 0 )
      break;
    }

    case RtemsTaskReqDelete_Post_Zombie_NA:
      break;
  }
}

static void RtemsTaskReqDelete_Post_TaskPriority_Check(
  RtemsTaskReqDelete_Context          *ctx,
  RtemsTaskReqDelete_Post_TaskPriority state
)
{
  switch ( state ) {
    case RtemsTaskReqDelete_Post_TaskPriority_Raise: {
      /*
       * Each priority of the calling task which is higher than the highest
       * priority of the task specified by the ``id`` parameter shall be made
       * the highest priority of the task.
       */
      T_eq_u32( ctx->worker_priority, PRIO_ULTRA_HIGH );
      break;
    }

    case RtemsTaskReqDelete_Post_TaskPriority_Nop: {
      /*
       * The priorities of the task specified by the ``id`` parameter shall not
       * be changed by the rtems_task_delete() call.
       */
      T_eq_u32( ctx->worker_priority, PRIO_NORMAL );
      break;
    }

    case RtemsTaskReqDelete_Post_TaskPriority_NA:
      break;
  }
}

static void RtemsTaskReqDelete_Post_RestartExtensions_Check(
  RtemsTaskReqDelete_Context               *ctx,
  RtemsTaskReqDelete_Post_RestartExtensions state
)
{
  switch ( state ) {
    case RtemsTaskReqDelete_Post_RestartExtensions_Nop: {
      /*
       * The thread delete user extensions shall not be invoked by the
       * rtems_task_delete() call.
       */
      T_eq_u32( ctx->calls_after_restart.thread_restart, 0 );
      break;
    }

    case RtemsTaskReqDelete_Post_RestartExtensions_NA:
      break;
  }
}

static void RtemsTaskReqDelete_Post_TerminateExtensions_Check(
  RtemsTaskReqDelete_Context                 *ctx,
  RtemsTaskReqDelete_Post_TerminateExtensions state
)
{
  switch ( state ) {
    case RtemsTaskReqDelete_Post_TerminateExtensions_Yes: {
      /*
       * The thread terminate user extensions shall be invoked by the
       * rtems_task_delete() call.
       */
      T_eq_u32( ctx->calls_after_restart.thread_terminate, 1 );
      break;
    }

    case RtemsTaskReqDelete_Post_TerminateExtensions_Nop: {
      /*
       * The thread terminate user extensions shall not be invoked by the
       * rtems_task_delete() call.
       */
      T_eq_u32( ctx->calls_after_restart.thread_terminate, 0 );
      break;
    }

    case RtemsTaskReqDelete_Post_TerminateExtensions_NA:
      break;
  }
}

static void RtemsTaskReqDelete_Post_Dormant_Check(
  RtemsTaskReqDelete_Context     *ctx,
  RtemsTaskReqDelete_Post_Dormant state
)
{
  switch ( state ) {
    case RtemsTaskReqDelete_Post_Dormant_Yes: {
      /*
       * The task specified by the ``id`` parameter shall be dormant after the
       * rtems_task_delete() call.
       */
      T_eq_u32( ctx->worker_state & STATES_DORMANT, STATES_DORMANT )
      break;
    }

    case RtemsTaskReqDelete_Post_Dormant_No: {
      /*
       * The task specified by the ``id`` parameter shall not be dormant after
       * the rtems_task_delete() call.
       */
      T_eq_u32( ctx->worker_state & STATES_DORMANT, 0 )
      break;
    }

    case RtemsTaskReqDelete_Post_Dormant_NA:
      break;
  }
}

static void RtemsTaskReqDelete_Post_Suspended_Check(
  RtemsTaskReqDelete_Context       *ctx,
  RtemsTaskReqDelete_Post_Suspended state
)
{
  switch ( state ) {
    case RtemsTaskReqDelete_Post_Suspended_Yes: {
      /*
       * The task specified by the ``id`` parameter shall be suspended after
       * the rtems_task_delete() call.
       */
      T_eq_u32( ctx->worker_state & STATES_SUSPENDED, STATES_SUSPENDED )
      break;
    }

    case RtemsTaskReqDelete_Post_Suspended_No: {
      /*
       * The task specified by the ``id`` parameter shall not be suspended
       * after the rtems_task_delete() call.
       */
      T_eq_u32( ctx->worker_state & STATES_SUSPENDED, 0 )
      break;
    }

    case RtemsTaskReqDelete_Post_Suspended_NA:
      break;
  }
}

static void RtemsTaskReqDelete_Post_Restarting_Check(
  RtemsTaskReqDelete_Context        *ctx,
  RtemsTaskReqDelete_Post_Restarting state
)
{
  switch ( state ) {
    case RtemsTaskReqDelete_Post_Restarting_Yes: {
      /*
       * The task specified by the ``id`` parameter shall be restarting after
       * the rtems_task_delete() call.
       */
      T_ne_int( ctx->worker_life_state & THREAD_LIFE_RESTARTING, 0 );
      break;
    }

    case RtemsTaskReqDelete_Post_Restarting_No: {
      /*
       * The task specified by the ``id`` parameter shall not be restarting
       * after the rtems_task_delete() call.
       */
      T_eq_int( ctx->worker_life_state & THREAD_LIFE_RESTARTING, 0 );
      break;
    }

    case RtemsTaskReqDelete_Post_Restarting_NA:
      break;
  }
}

static void RtemsTaskReqDelete_Post_Terminating_Check(
  RtemsTaskReqDelete_Context         *ctx,
  RtemsTaskReqDelete_Post_Terminating state
)
{
  switch ( state ) {
    case RtemsTaskReqDelete_Post_Terminating_Yes: {
      /*
       * The task specified by the ``id`` parameter shall be terminating after
       * the rtems_task_delete() call.
       */
      T_ne_int( ctx->worker_life_state & THREAD_LIFE_TERMINATING, 0 );
      break;
    }

    case RtemsTaskReqDelete_Post_Terminating_No: {
      /*
       * The task specified by the ``id`` parameter shall not be terminating
       * after the rtems_task_delete() call.
       */
      T_eq_int( ctx->worker_life_state & THREAD_LIFE_TERMINATING, 0 );
      break;
    }

    case RtemsTaskReqDelete_Post_Terminating_NA:
      break;
  }
}

static void RtemsTaskReqDelete_Post_Protected_Check(
  RtemsTaskReqDelete_Context       *ctx,
  RtemsTaskReqDelete_Post_Protected state
)
{
  switch ( state ) {
    case RtemsTaskReqDelete_Post_Protected_Yes: {
      /*
       * The thread life of the task specified by the ``id`` parameter be
       * protected after the rtems_task_delete() call.
       */
      T_ne_int( ctx->worker_life_state & THREAD_LIFE_PROTECTED, 0 );
      break;
    }

    case RtemsTaskReqDelete_Post_Protected_No: {
      /*
       * The thread life of the task specified by the ``id`` parameter shall
       * not be protected after the rtems_task_delete() call.
       */
      T_eq_int( ctx->worker_life_state & THREAD_LIFE_PROTECTED, 0 );
      break;
    }

    case RtemsTaskReqDelete_Post_Protected_NA:
      break;
  }
}

static void RtemsTaskReqDelete_Post_State_Check(
  RtemsTaskReqDelete_Context   *ctx,
  RtemsTaskReqDelete_Post_State state
)
{
  switch ( state ) {
    case RtemsTaskReqDelete_Post_State_Enqueued: {
      /*
       * The task specified by the ``id`` parameter shall be enqueued on a wait
       * queue and blocked.
       */
      T_ne_u32( ctx->worker_state & STATES_BLOCKED, 0 )
      T_not_null( ctx->worker_wait_queue );
      break;
    }

    case RtemsTaskReqDelete_Post_State_Ready: {
      /*
       * The task specified by the ``id`` parameter shall not be enqueued on a
       * wait queue and not blocked.
       */
      T_eq_u32( ctx->worker_state & STATES_BLOCKED, 0 )
      T_null( ctx->worker_wait_queue );
      break;
    }

    case RtemsTaskReqDelete_Post_State_Blocked: {
      /*
       * The task specified by the ``id`` parameter shall be not enqueued on a
       * wait queue and blocked.
       */
      T_ne_u32( ctx->worker_state & STATES_BLOCKED, 0 )
      T_null( ctx->worker_wait_queue );
      break;
    }

    case RtemsTaskReqDelete_Post_State_NA:
      break;
  }
}

static void RtemsTaskReqDelete_Post_Timer_Check(
  RtemsTaskReqDelete_Context   *ctx,
  RtemsTaskReqDelete_Post_Timer state
)
{
  switch ( state ) {
    case RtemsTaskReqDelete_Post_Timer_Active: {
      /*
       * The timer of the task specified by the ``id`` parameter shall be
       * active after the rtems_task_delete() call.
       */
      T_eq_int( ctx->worker_timer_info.state, TASK_TIMER_TICKS );
      break;
    }

    case RtemsTaskReqDelete_Post_Timer_Inactive: {
      /*
       * The timer of the task specified by the ``id`` parameter shall be
       * inactive after the rtems_task_delete() call.
       */
      T_eq_int( ctx->worker_timer_info.state, TASK_TIMER_INACTIVE );
      break;
    }

    case RtemsTaskReqDelete_Post_Timer_NA:
      break;
  }
}

static void RtemsTaskReqDelete_Setup( RtemsTaskReqDelete_Context *ctx )
{
  rtems_status_code sc;

  ctx->runner_id = rtems_task_self();
  ctx->runner_tcb = GetThread( ctx->runner_id );
  ctx->scheduler_id = GetSelfScheduler();
  ctx->mutex_id = CreateMutexNoProtocol();
  ObtainMutex( ctx->mutex_id );

  sc = rtems_extension_create(
    rtems_build_name( 'T', 'E', 'S', 'T' ),
    &extensions,
    &ctx->extension_id
  );
  T_rsc_success( sc );

  SetFatalHandler( Fatal, ctx );
  SetTaskSwitchExtension( TaskSwitch );
  SetSelfPriority( PRIO_NORMAL );

  ctx->deleter_id = CreateTask( "DELE", PRIO_HIGH );
  ctx->deleter_tcb = GetThread( ctx->deleter_id );
  StartTask( ctx->deleter_id, Deleter, NULL );

  ctx->deleter_2_id = CreateTask( "DEL2", PRIO_ULTRA_HIGH );
  ctx->deleter_2_tcb = GetThread( ctx->deleter_2_id );
  StartTask( ctx->deleter_2_id, SecondDeleter, NULL );
}

static void RtemsTaskReqDelete_Setup_Wrap( void *arg )
{
  RtemsTaskReqDelete_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsTaskReqDelete_Setup( ctx );
}

static void RtemsTaskReqDelete_Teardown( RtemsTaskReqDelete_Context *ctx )
{
  rtems_status_code sc;

  sc = rtems_extension_delete( ctx->extension_id );
  T_rsc_success( sc );

  SetFatalHandler( NULL, NULL );
  SetTaskSwitchExtension( NULL );
  DeleteTask( ctx->deleter_id );
  DeleteTask( ctx->deleter_2_id );
  ReleaseMutex( ctx->mutex_id );
  DeleteMutex( ctx->mutex_id );
  RestoreRunnerASR();
  RestoreRunnerPriority();
}

static void RtemsTaskReqDelete_Teardown_Wrap( void *arg )
{
  RtemsTaskReqDelete_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsTaskReqDelete_Teardown( ctx );
}

static void RtemsTaskReqDelete_Prepare( RtemsTaskReqDelete_Context *ctx )
{
  ctx->status = RTEMS_NOT_IMPLEMENTED;
  ctx->restart_counter = 0;

  ctx->delete_worker_expected = false;
  ctx->worker_id = CreateTask( "WORK", PRIO_NORMAL );
  ctx->delete_worker_expected = true;

  ctx->worker_tcb = GetThread( ctx->worker_id );
  ctx->worker_state = UINT32_MAX;
  ctx->worker_life_state = INT_MAX;
  ctx->worker_priority = UINT32_MAX;
}

static void RtemsTaskReqDelete_Action( RtemsTaskReqDelete_Context *ctx )
{
  rtems_status_code sc;

  if ( ctx->id != INVALID_ID && !ctx->dormant ) {
    ctx->worker_is_mutex_owner = false;
    StartTask( ctx->worker_id, Worker, ctx );

    /* Let the worker catch signals and set the thread life protection state */
    Yield();

    sc = rtems_signal_send( ctx->worker_id, RTEMS_SIGNAL_0 );
    T_rsc_success( sc );

    if ( ctx->restarting ) {
      sc = rtems_task_restart( ctx->worker_id, 0 );
      T_rsc_success( sc );
    }

    if ( ctx->terminating ) {
      sc = rtems_task_restart( ctx->deleter_id, (rtems_task_argument) ctx );
      T_rsc_success( sc );
    } else {
      Yield();
    }
  }

  if ( ctx->id != RTEMS_SELF ) {
    if ( ctx->interrupt ) {
      CallWithinISR( Delete, ctx );
    } else {
      sc = rtems_task_restart( ctx->deleter_2_id, (rtems_task_argument) ctx );
      T_rsc_success( sc );
    }
  }

  Cleanup( ctx );
}

static const RtemsTaskReqDelete_Entry
RtemsTaskReqDelete_Entries[] = {
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqDelete_Post_Status_NA,
    RtemsTaskReqDelete_Post_FatalError_NA, RtemsTaskReqDelete_Post_Zombie_NA,
    RtemsTaskReqDelete_Post_TaskPriority_NA,
    RtemsTaskReqDelete_Post_RestartExtensions_NA,
    RtemsTaskReqDelete_Post_TerminateExtensions_NA,
    RtemsTaskReqDelete_Post_Dormant_NA, RtemsTaskReqDelete_Post_Suspended_NA,
    RtemsTaskReqDelete_Post_Restarting_NA,
    RtemsTaskReqDelete_Post_Terminating_NA,
    RtemsTaskReqDelete_Post_Protected_NA, RtemsTaskReqDelete_Post_State_NA,
    RtemsTaskReqDelete_Post_Timer_NA },
  { 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, RtemsTaskReqDelete_Post_Status_InvId,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_NA,
    RtemsTaskReqDelete_Post_TaskPriority_NA,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_NA, RtemsTaskReqDelete_Post_Suspended_NA,
    RtemsTaskReqDelete_Post_Restarting_NA,
    RtemsTaskReqDelete_Post_Terminating_NA,
    RtemsTaskReqDelete_Post_Protected_NA, RtemsTaskReqDelete_Post_State_NA,
    RtemsTaskReqDelete_Post_Timer_NA },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqDelete_Post_Status_NA,
    RtemsTaskReqDelete_Post_FatalError_NA, RtemsTaskReqDelete_Post_Zombie_NA,
    RtemsTaskReqDelete_Post_TaskPriority_NA,
    RtemsTaskReqDelete_Post_RestartExtensions_NA,
    RtemsTaskReqDelete_Post_TerminateExtensions_NA,
    RtemsTaskReqDelete_Post_Dormant_NA, RtemsTaskReqDelete_Post_Suspended_NA,
    RtemsTaskReqDelete_Post_Restarting_NA,
    RtemsTaskReqDelete_Post_Terminating_NA,
    RtemsTaskReqDelete_Post_Protected_NA, RtemsTaskReqDelete_Post_State_NA,
    RtemsTaskReqDelete_Post_Timer_NA },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqDelete_Post_Status_NA,
    RtemsTaskReqDelete_Post_FatalError_NA, RtemsTaskReqDelete_Post_Zombie_NA,
    RtemsTaskReqDelete_Post_TaskPriority_NA,
    RtemsTaskReqDelete_Post_RestartExtensions_NA,
    RtemsTaskReqDelete_Post_TerminateExtensions_NA,
    RtemsTaskReqDelete_Post_Dormant_NA, RtemsTaskReqDelete_Post_Suspended_NA,
    RtemsTaskReqDelete_Post_Restarting_NA,
    RtemsTaskReqDelete_Post_Terminating_NA,
    RtemsTaskReqDelete_Post_Protected_NA, RtemsTaskReqDelete_Post_State_NA,
    RtemsTaskReqDelete_Post_Timer_NA },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqDelete_Post_Status_NA,
    RtemsTaskReqDelete_Post_FatalError_NA, RtemsTaskReqDelete_Post_Zombie_NA,
    RtemsTaskReqDelete_Post_TaskPriority_NA,
    RtemsTaskReqDelete_Post_RestartExtensions_NA,
    RtemsTaskReqDelete_Post_TerminateExtensions_NA,
    RtemsTaskReqDelete_Post_Dormant_NA, RtemsTaskReqDelete_Post_Suspended_NA,
    RtemsTaskReqDelete_Post_Restarting_NA,
    RtemsTaskReqDelete_Post_Terminating_NA,
    RtemsTaskReqDelete_Post_Protected_NA, RtemsTaskReqDelete_Post_State_NA,
    RtemsTaskReqDelete_Post_Timer_NA },
  { 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, RtemsTaskReqDelete_Post_Status_Ok,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_Yes,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_Yes, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, RtemsTaskReqDelete_Post_Status_Ok,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_Yes,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_Yes, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_Yes, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_No, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_Yes, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_No, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_NoReturn,
    RtemsTaskReqDelete_Post_FatalError_Yes, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Raise,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_NoReturn,
    RtemsTaskReqDelete_Post_FatalError_Yes, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Raise,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_NoReturn,
    RtemsTaskReqDelete_Post_FatalError_Yes, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_NoReturn,
    RtemsTaskReqDelete_Post_FatalError_Yes, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqDelete_Post_Status_Ok,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Raise,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqDelete_Post_Status_Ok,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Raise,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqDelete_Post_Status_Ok,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqDelete_Post_Status_Ok,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_NoReturn,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_Yes,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Yes,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_NoReturn,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_Yes,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Yes,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_NoReturn,
    RtemsTaskReqDelete_Post_FatalError_Yes, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_NoReturn,
    RtemsTaskReqDelete_Post_FatalError_Yes, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_NoReturn,
    RtemsTaskReqDelete_Post_FatalError_Yes, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_NoReturn,
    RtemsTaskReqDelete_Post_FatalError_Yes, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_NoReturn,
    RtemsTaskReqDelete_Post_FatalError_Yes, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_NoReturn,
    RtemsTaskReqDelete_Post_FatalError_Yes, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_NoReturn,
    RtemsTaskReqDelete_Post_FatalError_Yes, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_NoReturn,
    RtemsTaskReqDelete_Post_FatalError_Yes, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_NoReturn,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_NoReturn,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_NoReturn,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_NoReturn,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_NoReturn,
    RtemsTaskReqDelete_Post_FatalError_Yes, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Raise,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_NoReturn,
    RtemsTaskReqDelete_Post_FatalError_Yes, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Raise,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_NoReturn,
    RtemsTaskReqDelete_Post_FatalError_Yes, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Raise,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_NoReturn,
    RtemsTaskReqDelete_Post_FatalError_Yes, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Raise,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Blocked,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_NoReturn,
    RtemsTaskReqDelete_Post_FatalError_Yes, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Raise,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Blocked, RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_NoReturn,
    RtemsTaskReqDelete_Post_FatalError_Yes, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Raise,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_NoReturn,
    RtemsTaskReqDelete_Post_FatalError_Yes, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Raise,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_NoReturn,
    RtemsTaskReqDelete_Post_FatalError_Yes, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Raise,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_NoReturn,
    RtemsTaskReqDelete_Post_FatalError_Yes, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Raise,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Blocked,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_NoReturn,
    RtemsTaskReqDelete_Post_FatalError_Yes, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Raise,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Blocked, RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_NoReturn,
    RtemsTaskReqDelete_Post_FatalError_Yes, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_NoReturn,
    RtemsTaskReqDelete_Post_FatalError_Yes, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_NoReturn,
    RtemsTaskReqDelete_Post_FatalError_Yes, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_NoReturn,
    RtemsTaskReqDelete_Post_FatalError_Yes, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Blocked,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_NoReturn,
    RtemsTaskReqDelete_Post_FatalError_Yes, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Blocked, RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_NoReturn,
    RtemsTaskReqDelete_Post_FatalError_Yes, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_NoReturn,
    RtemsTaskReqDelete_Post_FatalError_Yes, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_NoReturn,
    RtemsTaskReqDelete_Post_FatalError_Yes, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_NoReturn,
    RtemsTaskReqDelete_Post_FatalError_Yes, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Blocked,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_NoReturn,
    RtemsTaskReqDelete_Post_FatalError_Yes, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Blocked, RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqDelete_Post_Status_Ok,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Raise,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqDelete_Post_Status_Ok,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Raise,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqDelete_Post_Status_Ok,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Raise,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqDelete_Post_Status_Ok,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Raise,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Blocked,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqDelete_Post_Status_Ok,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Raise,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Blocked, RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqDelete_Post_Status_Ok,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Raise,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqDelete_Post_Status_Ok,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Raise,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqDelete_Post_Status_Ok,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Raise,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqDelete_Post_Status_Ok,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Raise,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Blocked,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqDelete_Post_Status_Ok,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Raise,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Blocked, RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqDelete_Post_Status_Ok,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqDelete_Post_Status_Ok,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqDelete_Post_Status_Ok,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqDelete_Post_Status_Ok,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Blocked,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqDelete_Post_Status_Ok,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Blocked, RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqDelete_Post_Status_Ok,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqDelete_Post_Status_Ok,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqDelete_Post_Status_Ok,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqDelete_Post_Status_Ok,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Blocked,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqDelete_Post_Status_Ok,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Blocked, RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_Yes, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Blocked,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Blocked, RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_No, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Blocked,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Blocked, RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Blocked,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Blocked, RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Blocked,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Blocked, RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_Yes, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Blocked,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Blocked, RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_No, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Blocked,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Blocked, RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Blocked,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Blocked, RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Blocked,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Blocked, RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_Yes, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Blocked,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Blocked, RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_No, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Blocked,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Blocked, RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Blocked,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Blocked, RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Blocked,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Blocked, RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_Yes, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Blocked,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Blocked, RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_No, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Blocked,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Blocked, RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Blocked,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Blocked, RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Blocked,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Blocked, RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_Yes, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Blocked,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Blocked, RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_No, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Blocked,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Blocked, RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Blocked,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Blocked, RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Blocked,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Blocked, RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_Yes, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Blocked,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Blocked, RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_No, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Blocked,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Blocked, RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Blocked,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Blocked, RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Blocked,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_Yes,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Blocked, RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_Yes, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Blocked,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Blocked, RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_No, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Blocked,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Blocked, RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Blocked,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Blocked, RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Blocked,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_No,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Blocked, RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_Yes, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Blocked,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Blocked, RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_No, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Blocked,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_No,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Blocked, RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Blocked,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_Yes,
    RtemsTaskReqDelete_Post_State_Blocked, RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Enqueued,
    RtemsTaskReqDelete_Post_Timer_Active },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No, RtemsTaskReqDelete_Post_State_Ready,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Blocked,
    RtemsTaskReqDelete_Post_Timer_Inactive },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqDelete_Post_Status_CalledFromISR,
    RtemsTaskReqDelete_Post_FatalError_Nop, RtemsTaskReqDelete_Post_Zombie_No,
    RtemsTaskReqDelete_Post_TaskPriority_Nop,
    RtemsTaskReqDelete_Post_RestartExtensions_Nop,
    RtemsTaskReqDelete_Post_TerminateExtensions_Nop,
    RtemsTaskReqDelete_Post_Dormant_No, RtemsTaskReqDelete_Post_Suspended_No,
    RtemsTaskReqDelete_Post_Restarting_Yes,
    RtemsTaskReqDelete_Post_Terminating_Yes,
    RtemsTaskReqDelete_Post_Protected_No,
    RtemsTaskReqDelete_Post_State_Blocked, RtemsTaskReqDelete_Post_Timer_Active }
};

static const uint8_t
RtemsTaskReqDelete_Map[] = {
  3, 3, 19, 4, 3, 3, 3, 3, 20, 4, 3, 3, 3, 3, 19, 4, 3, 3, 3, 3, 20, 4, 3, 3,
  3, 3, 21, 4, 3, 3, 3, 3, 22, 4, 3, 3, 3, 3, 21, 4, 3, 3, 3, 3, 22, 4, 3, 3,
  3, 3, 23, 4, 3, 3, 3, 3, 24, 4, 3, 3, 3, 3, 23, 4, 3, 3, 3, 3, 24, 4, 3, 3,
  3, 3, 25, 4, 3, 3, 3, 3, 26, 4, 3, 3, 3, 3, 25, 4, 3, 3, 3, 3, 26, 4, 3, 3,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 19, 4, 3, 3, 3,
  3, 20, 4, 3, 3, 3, 3, 19, 4, 3, 3, 3, 3, 20, 4, 3, 3, 3, 3, 21, 4, 3, 3, 3,
  3, 22, 4, 3, 3, 3, 3, 21, 4, 3, 3, 3, 3, 22, 4, 3, 3, 3, 3, 23, 4, 3, 3, 3,
  3, 24, 4, 3, 3, 3, 3, 23, 4, 3, 3, 3, 3, 24, 4, 3, 3, 3, 3, 25, 4, 3, 3, 3,
  3, 26, 4, 3, 3, 3, 3, 25, 4, 3, 3, 3, 3, 26, 4, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 27, 4, 3, 3, 3, 3, 28, 4, 3, 3, 3, 3, 27,
  4, 3, 3, 3, 3, 28, 4, 3, 3, 3, 3, 29, 4, 3, 3, 3, 3, 30, 4, 3, 3, 3, 3, 29,
  4, 3, 3, 3, 3, 30, 4, 3, 3, 3, 3, 17, 4, 3, 3, 3, 3, 17, 4, 3, 3, 3, 3, 17,
  4, 3, 3, 3, 3, 17, 4, 3, 3, 3, 3, 18, 4, 3, 3, 3, 3, 18, 4, 3, 3, 3, 3, 18,
  4, 3, 3, 3, 3, 18, 4, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 3, 3, 27, 4, 3, 3, 3, 3, 28, 4, 3, 3, 3, 3, 27, 4, 3, 3, 3, 3, 28, 4,
  3, 3, 3, 3, 29, 4, 3, 3, 3, 3, 30, 4, 3, 3, 3, 3, 29, 4, 3, 3, 3, 3, 30, 4,
  3, 3, 3, 3, 17, 4, 3, 3, 3, 3, 17, 4, 3, 3, 3, 3, 17, 4, 3, 3, 3, 3, 17, 4,
  3, 3, 3, 3, 18, 4, 3, 3, 3, 3, 18, 4, 3, 3, 3, 3, 18, 4, 3, 3, 3, 3, 18, 4,
  3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 71, 72, 73, 4,
  74, 75, 76, 77, 78, 4, 79, 80, 81, 82, 83, 4, 84, 85, 86, 87, 88, 4, 89, 90,
  91, 92, 93, 4, 94, 95, 96, 97, 98, 4, 99, 100, 101, 102, 103, 4, 104, 105,
  106, 107, 108, 4, 109, 110, 111, 112, 113, 4, 114, 115, 116, 117, 118, 4,
  119, 120, 121, 122, 123, 4, 124, 125, 126, 127, 128, 4, 129, 130, 131, 132,
  133, 4, 134, 135, 136, 137, 138, 4, 139, 140, 141, 142, 143, 4, 144, 145,
  146, 147, 148, 4, 149, 150, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 71, 72, 73, 4, 74, 75, 76, 77, 78, 4, 79, 80, 81, 82, 83, 4, 84, 85,
  86, 87, 88, 4, 89, 90, 91, 92, 93, 4, 94, 95, 96, 97, 98, 4, 99, 100, 101,
  102, 103, 4, 104, 105, 106, 107, 108, 4, 109, 110, 111, 112, 113, 4, 114,
  115, 116, 117, 118, 4, 119, 120, 121, 122, 123, 4, 124, 125, 126, 127, 128,
  4, 129, 130, 131, 132, 133, 4, 134, 135, 136, 137, 138, 4, 139, 140, 141,
  142, 143, 4, 144, 145, 146, 147, 148, 4, 149, 150, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 31, 32, 33, 4, 34, 35, 9, 9, 9, 4, 9, 9, 31, 32, 33, 4, 34, 35, 9,
  9, 9, 4, 9, 9, 36, 37, 38, 4, 39, 40, 10, 10, 10, 4, 10, 10, 36, 37, 38, 4,
  39, 40, 10, 10, 10, 4, 10, 10, 31, 32, 33, 4, 34, 35, 9, 9, 9, 4, 9, 9, 31,
  32, 33, 4, 34, 35, 9, 9, 9, 4, 9, 9, 36, 37, 38, 4, 39, 40, 10, 10, 10, 4,
  10, 10, 36, 37, 38, 4, 39, 40, 10, 10, 10, 4, 10, 10, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 41, 42, 43, 4, 44, 45, 11, 11, 11, 4, 11, 11,
  41, 42, 43, 4, 44, 45, 11, 11, 11, 4, 11, 11, 46, 47, 48, 4, 49, 50, 12, 12,
  12, 4, 12, 12, 46, 47, 48, 4, 49, 50, 12, 12, 12, 4, 12, 12, 41, 42, 43, 4,
  44, 45, 11, 11, 11, 4, 11, 11, 41, 42, 43, 4, 44, 45, 11, 11, 11, 4, 11, 11,
  46, 47, 48, 4, 49, 50, 12, 12, 12, 4, 12, 12, 46, 47, 48, 4, 49, 50, 12, 12,
  12, 4, 12, 12, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 51, 52,
  53, 4, 54, 55, 13, 13, 13, 4, 13, 13, 51, 52, 53, 4, 54, 55, 13, 13, 13, 4,
  13, 13, 56, 57, 58, 4, 59, 60, 14, 14, 14, 4, 14, 14, 56, 57, 58, 4, 59, 60,
  14, 14, 14, 4, 14, 14, 51, 52, 53, 4, 54, 55, 13, 13, 13, 4, 13, 13, 51, 52,
  53, 4, 54, 55, 13, 13, 13, 4, 13, 13, 56, 57, 58, 4, 59, 60, 14, 14, 14, 4,
  14, 14, 56, 57, 58, 4, 59, 60, 14, 14, 14, 4, 14, 14, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 61, 62, 63, 4, 64, 65, 15, 15, 15, 4, 15, 15,
  61, 62, 63, 4, 64, 65, 15, 15, 15, 4, 15, 15, 66, 67, 68, 4, 69, 70, 16, 16,
  16, 4, 16, 16, 66, 67, 68, 4, 69, 70, 16, 16, 16, 4, 16, 16, 61, 62, 63, 4,
  64, 65, 15, 15, 15, 4, 15, 15, 61, 62, 63, 4, 64, 65, 15, 15, 15, 4, 15, 15,
  66, 67, 68, 4, 69, 70, 16, 16, 16, 4, 16, 16, 66, 67, 68, 4, 69, 70, 16, 16,
  16, 4, 16, 16, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 151,
  152, 153, 4, 154, 155, 156, 157, 158, 4, 159, 160, 161, 162, 163, 4, 164,
  165, 166, 167, 168, 4, 169, 170, 171, 172, 173, 4, 174, 175, 176, 177, 178,
  4, 179, 180, 181, 182, 183, 4, 184, 185, 186, 187, 188, 4, 189, 190, 191,
  192, 193, 4, 194, 195, 196, 197, 198, 4, 199, 200, 201, 202, 203, 4, 204,
  205, 206, 207, 208, 4, 209, 210, 211, 212, 213, 4, 214, 215, 216, 217, 218,
  4, 219, 220, 221, 222, 223, 4, 224, 225, 226, 227, 228, 4, 229, 230, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 151, 152, 153, 4, 154, 155, 156,
  157, 158, 4, 159, 160, 161, 162, 163, 4, 164, 165, 166, 167, 168, 4, 169,
  170, 171, 172, 173, 4, 174, 175, 176, 177, 178, 4, 179, 180, 181, 182, 183,
  4, 184, 185, 186, 187, 188, 4, 189, 190, 191, 192, 193, 4, 194, 195, 196,
  197, 198, 4, 199, 200, 201, 202, 203, 4, 204, 205, 206, 207, 208, 4, 209,
  210, 211, 212, 213, 4, 214, 215, 216, 217, 218, 4, 219, 220, 221, 222, 223,
  4, 224, 225, 226, 227, 228, 4, 229, 230, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0
};

static size_t RtemsTaskReqDelete_Scope( void *arg, char *buf, size_t n )
{
  RtemsTaskReqDelete_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsTaskReqDelete_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsTaskReqDelete_Fixture = {
  .setup = RtemsTaskReqDelete_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsTaskReqDelete_Teardown_Wrap,
  .scope = RtemsTaskReqDelete_Scope,
  .initial_context = &RtemsTaskReqDelete_Instance
};

static inline RtemsTaskReqDelete_Entry RtemsTaskReqDelete_PopEntry(
  RtemsTaskReqDelete_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsTaskReqDelete_Entries[
    RtemsTaskReqDelete_Map[ index ]
  ];
}

static void RtemsTaskReqDelete_SetPreConditionStates(
  RtemsTaskReqDelete_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];

  if ( ctx->Map.entry.Pre_CallerPriority_NA ) {
    ctx->Map.pcs[ 3 ] = RtemsTaskReqDelete_Pre_CallerPriority_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }

  if ( ctx->Map.entry.Pre_Dormant_NA ) {
    ctx->Map.pcs[ 4 ] = RtemsTaskReqDelete_Pre_Dormant_NA;
  } else {
    ctx->Map.pcs[ 4 ] = ctx->Map.pci[ 4 ];
  }

  if ( ctx->Map.entry.Pre_Suspended_NA ) {
    ctx->Map.pcs[ 5 ] = RtemsTaskReqDelete_Pre_Suspended_NA;
  } else {
    ctx->Map.pcs[ 5 ] = ctx->Map.pci[ 5 ];
  }

  if ( ctx->Map.entry.Pre_Restarting_NA ) {
    ctx->Map.pcs[ 6 ] = RtemsTaskReqDelete_Pre_Restarting_NA;
  } else {
    ctx->Map.pcs[ 6 ] = ctx->Map.pci[ 6 ];
  }

  if ( ctx->Map.entry.Pre_Terminating_NA ) {
    ctx->Map.pcs[ 7 ] = RtemsTaskReqDelete_Pre_Terminating_NA;
  } else {
    ctx->Map.pcs[ 7 ] = ctx->Map.pci[ 7 ];
  }

  if ( ctx->Map.entry.Pre_Protected_NA ) {
    ctx->Map.pcs[ 8 ] = RtemsTaskReqDelete_Pre_Protected_NA;
  } else {
    ctx->Map.pcs[ 8 ] = ctx->Map.pci[ 8 ];
  }

  if ( ctx->Map.entry.Pre_State_NA ) {
    ctx->Map.pcs[ 9 ] = RtemsTaskReqDelete_Pre_State_NA;
  } else {
    ctx->Map.pcs[ 9 ] = ctx->Map.pci[ 9 ];
  }

  if ( ctx->Map.entry.Pre_Timer_NA ) {
    ctx->Map.pcs[ 10 ] = RtemsTaskReqDelete_Pre_Timer_NA;
  } else {
    ctx->Map.pcs[ 10 ] = ctx->Map.pci[ 10 ];
  }
}

static void RtemsTaskReqDelete_TestVariant( RtemsTaskReqDelete_Context *ctx )
{
  RtemsTaskReqDelete_Pre_Id_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsTaskReqDelete_Pre_Context_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsTaskReqDelete_Pre_ThreadDispatch_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsTaskReqDelete_Pre_CallerPriority_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  RtemsTaskReqDelete_Pre_Dormant_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  RtemsTaskReqDelete_Pre_Suspended_Prepare( ctx, ctx->Map.pcs[ 5 ] );
  RtemsTaskReqDelete_Pre_Restarting_Prepare( ctx, ctx->Map.pcs[ 6 ] );
  RtemsTaskReqDelete_Pre_Terminating_Prepare( ctx, ctx->Map.pcs[ 7 ] );
  RtemsTaskReqDelete_Pre_Protected_Prepare( ctx, ctx->Map.pcs[ 8 ] );
  RtemsTaskReqDelete_Pre_State_Prepare( ctx, ctx->Map.pcs[ 9 ] );
  RtemsTaskReqDelete_Pre_Timer_Prepare( ctx, ctx->Map.pcs[ 10 ] );
  RtemsTaskReqDelete_Action( ctx );
  RtemsTaskReqDelete_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  RtemsTaskReqDelete_Post_FatalError_Check(
    ctx,
    ctx->Map.entry.Post_FatalError
  );
  RtemsTaskReqDelete_Post_Zombie_Check( ctx, ctx->Map.entry.Post_Zombie );
  RtemsTaskReqDelete_Post_TaskPriority_Check(
    ctx,
    ctx->Map.entry.Post_TaskPriority
  );
  RtemsTaskReqDelete_Post_RestartExtensions_Check(
    ctx,
    ctx->Map.entry.Post_RestartExtensions
  );
  RtemsTaskReqDelete_Post_TerminateExtensions_Check(
    ctx,
    ctx->Map.entry.Post_TerminateExtensions
  );
  RtemsTaskReqDelete_Post_Dormant_Check( ctx, ctx->Map.entry.Post_Dormant );
  RtemsTaskReqDelete_Post_Suspended_Check(
    ctx,
    ctx->Map.entry.Post_Suspended
  );
  RtemsTaskReqDelete_Post_Restarting_Check(
    ctx,
    ctx->Map.entry.Post_Restarting
  );
  RtemsTaskReqDelete_Post_Terminating_Check(
    ctx,
    ctx->Map.entry.Post_Terminating
  );
  RtemsTaskReqDelete_Post_Protected_Check(
    ctx,
    ctx->Map.entry.Post_Protected
  );
  RtemsTaskReqDelete_Post_State_Check( ctx, ctx->Map.entry.Post_State );
  RtemsTaskReqDelete_Post_Timer_Check( ctx, ctx->Map.entry.Post_Timer );
}

/**
 * @fn void T_case_body_RtemsTaskReqDelete( void )
 */
T_TEST_CASE_FIXTURE( RtemsTaskReqDelete, &RtemsTaskReqDelete_Fixture )
{
  RtemsTaskReqDelete_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = RtemsTaskReqDelete_Pre_Id_Executing;
    ctx->Map.pci[ 0 ] < RtemsTaskReqDelete_Pre_Id_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = RtemsTaskReqDelete_Pre_Context_Task;
      ctx->Map.pci[ 1 ] < RtemsTaskReqDelete_Pre_Context_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = RtemsTaskReqDelete_Pre_ThreadDispatch_Disabled;
        ctx->Map.pci[ 2 ] < RtemsTaskReqDelete_Pre_ThreadDispatch_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = RtemsTaskReqDelete_Pre_CallerPriority_Vital;
          ctx->Map.pci[ 3 ] < RtemsTaskReqDelete_Pre_CallerPriority_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          for (
            ctx->Map.pci[ 4 ] = RtemsTaskReqDelete_Pre_Dormant_No;
            ctx->Map.pci[ 4 ] < RtemsTaskReqDelete_Pre_Dormant_NA;
            ++ctx->Map.pci[ 4 ]
          ) {
            for (
              ctx->Map.pci[ 5 ] = RtemsTaskReqDelete_Pre_Suspended_Yes;
              ctx->Map.pci[ 5 ] < RtemsTaskReqDelete_Pre_Suspended_NA;
              ++ctx->Map.pci[ 5 ]
            ) {
              for (
                ctx->Map.pci[ 6 ] = RtemsTaskReqDelete_Pre_Restarting_No;
                ctx->Map.pci[ 6 ] < RtemsTaskReqDelete_Pre_Restarting_NA;
                ++ctx->Map.pci[ 6 ]
              ) {
                for (
                  ctx->Map.pci[ 7 ] = RtemsTaskReqDelete_Pre_Terminating_No;
                  ctx->Map.pci[ 7 ] < RtemsTaskReqDelete_Pre_Terminating_NA;
                  ++ctx->Map.pci[ 7 ]
                ) {
                  for (
                    ctx->Map.pci[ 8 ] = RtemsTaskReqDelete_Pre_Protected_Yes;
                    ctx->Map.pci[ 8 ] < RtemsTaskReqDelete_Pre_Protected_NA;
                    ++ctx->Map.pci[ 8 ]
                  ) {
                    for (
                      ctx->Map.pci[ 9 ] = RtemsTaskReqDelete_Pre_State_Enqueued;
                      ctx->Map.pci[ 9 ] < RtemsTaskReqDelete_Pre_State_NA;
                      ++ctx->Map.pci[ 9 ]
                    ) {
                      for (
                        ctx->Map.pci[ 10 ] = RtemsTaskReqDelete_Pre_Timer_Inactive;
                        ctx->Map.pci[ 10 ] < RtemsTaskReqDelete_Pre_Timer_NA;
                        ++ctx->Map.pci[ 10 ]
                      ) {
                        ctx->Map.entry = RtemsTaskReqDelete_PopEntry( ctx );

                        if ( ctx->Map.entry.Skip ) {
                          continue;
                        }

                        RtemsTaskReqDelete_SetPreConditionStates( ctx );
                        RtemsTaskReqDelete_Prepare( ctx );
                        RtemsTaskReqDelete_TestVariant( ctx );
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
