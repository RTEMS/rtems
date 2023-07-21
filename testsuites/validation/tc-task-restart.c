/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsTaskReqRestart
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
#include <rtems/test-scheduler.h>
#include <rtems/score/statesimpl.h>
#include <rtems/score/threaddispatch.h>
#include <rtems/score/threadimpl.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsTaskReqRestart spec:/rtems/task/req/restart
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsTaskReqRestart_Pre_Id_Invalid,
  RtemsTaskReqRestart_Pre_Id_Executing,
  RtemsTaskReqRestart_Pre_Id_Other,
  RtemsTaskReqRestart_Pre_Id_NA
} RtemsTaskReqRestart_Pre_Id;

typedef enum {
  RtemsTaskReqRestart_Pre_Dormant_Yes,
  RtemsTaskReqRestart_Pre_Dormant_No,
  RtemsTaskReqRestart_Pre_Dormant_NA
} RtemsTaskReqRestart_Pre_Dormant;

typedef enum {
  RtemsTaskReqRestart_Pre_Suspended_Yes,
  RtemsTaskReqRestart_Pre_Suspended_No,
  RtemsTaskReqRestart_Pre_Suspended_NA
} RtemsTaskReqRestart_Pre_Suspended;

typedef enum {
  RtemsTaskReqRestart_Pre_Restarting_Yes,
  RtemsTaskReqRestart_Pre_Restarting_No,
  RtemsTaskReqRestart_Pre_Restarting_NA
} RtemsTaskReqRestart_Pre_Restarting;

typedef enum {
  RtemsTaskReqRestart_Pre_Terminating_Yes,
  RtemsTaskReqRestart_Pre_Terminating_No,
  RtemsTaskReqRestart_Pre_Terminating_NA
} RtemsTaskReqRestart_Pre_Terminating;

typedef enum {
  RtemsTaskReqRestart_Pre_Protected_Yes,
  RtemsTaskReqRestart_Pre_Protected_No,
  RtemsTaskReqRestart_Pre_Protected_NA
} RtemsTaskReqRestart_Pre_Protected;

typedef enum {
  RtemsTaskReqRestart_Pre_Context_Task,
  RtemsTaskReqRestart_Pre_Context_Interrupt,
  RtemsTaskReqRestart_Pre_Context_NestedRequest,
  RtemsTaskReqRestart_Pre_Context_NA
} RtemsTaskReqRestart_Pre_Context;

typedef enum {
  RtemsTaskReqRestart_Pre_State_Ready,
  RtemsTaskReqRestart_Pre_State_Blocked,
  RtemsTaskReqRestart_Pre_State_Enqueued,
  RtemsTaskReqRestart_Pre_State_NA
} RtemsTaskReqRestart_Pre_State;

typedef enum {
  RtemsTaskReqRestart_Pre_Timer_Inactive,
  RtemsTaskReqRestart_Pre_Timer_Active,
  RtemsTaskReqRestart_Pre_Timer_NA
} RtemsTaskReqRestart_Pre_Timer;

typedef enum {
  RtemsTaskReqRestart_Pre_RealPriority_Initial,
  RtemsTaskReqRestart_Pre_RealPriority_Changed,
  RtemsTaskReqRestart_Pre_RealPriority_NA
} RtemsTaskReqRestart_Pre_RealPriority;

typedef enum {
  RtemsTaskReqRestart_Pre_ThreadDispatch_Disabled,
  RtemsTaskReqRestart_Pre_ThreadDispatch_Enabled,
  RtemsTaskReqRestart_Pre_ThreadDispatch_NA
} RtemsTaskReqRestart_Pre_ThreadDispatch;

typedef enum {
  RtemsTaskReqRestart_Post_Status_Ok,
  RtemsTaskReqRestart_Post_Status_InvId,
  RtemsTaskReqRestart_Post_Status_IncStat,
  RtemsTaskReqRestart_Post_Status_NoReturn,
  RtemsTaskReqRestart_Post_Status_NA
} RtemsTaskReqRestart_Post_Status;

typedef enum {
  RtemsTaskReqRestart_Post_FatalError_Yes,
  RtemsTaskReqRestart_Post_FatalError_Nop,
  RtemsTaskReqRestart_Post_FatalError_NA
} RtemsTaskReqRestart_Post_FatalError;

typedef enum {
  RtemsTaskReqRestart_Post_Argument_Set,
  RtemsTaskReqRestart_Post_Argument_Nop,
  RtemsTaskReqRestart_Post_Argument_NA
} RtemsTaskReqRestart_Post_Argument;

typedef enum {
  RtemsTaskReqRestart_Post_State_Dormant,
  RtemsTaskReqRestart_Post_State_DormantSuspended,
  RtemsTaskReqRestart_Post_State_Blocked,
  RtemsTaskReqRestart_Post_State_Ready,
  RtemsTaskReqRestart_Post_State_Zombie,
  RtemsTaskReqRestart_Post_State_Nop,
  RtemsTaskReqRestart_Post_State_NA
} RtemsTaskReqRestart_Post_State;

typedef enum {
  RtemsTaskReqRestart_Post_Enqueued_Yes,
  RtemsTaskReqRestart_Post_Enqueued_No,
  RtemsTaskReqRestart_Post_Enqueued_NA
} RtemsTaskReqRestart_Post_Enqueued;

typedef enum {
  RtemsTaskReqRestart_Post_Timer_Active,
  RtemsTaskReqRestart_Post_Timer_Inactive,
  RtemsTaskReqRestart_Post_Timer_NA
} RtemsTaskReqRestart_Post_Timer;

typedef enum {
  RtemsTaskReqRestart_Post_Restarting_Yes,
  RtemsTaskReqRestart_Post_Restarting_No,
  RtemsTaskReqRestart_Post_Restarting_NA
} RtemsTaskReqRestart_Post_Restarting;

typedef enum {
  RtemsTaskReqRestart_Post_Terminating_Yes,
  RtemsTaskReqRestart_Post_Terminating_No,
  RtemsTaskReqRestart_Post_Terminating_NA
} RtemsTaskReqRestart_Post_Terminating;

typedef enum {
  RtemsTaskReqRestart_Post_Protected_Yes,
  RtemsTaskReqRestart_Post_Protected_No,
  RtemsTaskReqRestart_Post_Protected_NA
} RtemsTaskReqRestart_Post_Protected;

typedef enum {
  RtemsTaskReqRestart_Post_RestartExtensions_Yes,
  RtemsTaskReqRestart_Post_RestartExtensions_Nop,
  RtemsTaskReqRestart_Post_RestartExtensions_NA
} RtemsTaskReqRestart_Post_RestartExtensions;

typedef enum {
  RtemsTaskReqRestart_Post_TerminateExtensions_Yes,
  RtemsTaskReqRestart_Post_TerminateExtensions_Nop,
  RtemsTaskReqRestart_Post_TerminateExtensions_NA
} RtemsTaskReqRestart_Post_TerminateExtensions;

typedef struct {
  uint64_t Skip : 1;
  uint64_t Pre_Id_NA : 1;
  uint64_t Pre_Dormant_NA : 1;
  uint64_t Pre_Suspended_NA : 1;
  uint64_t Pre_Restarting_NA : 1;
  uint64_t Pre_Terminating_NA : 1;
  uint64_t Pre_Protected_NA : 1;
  uint64_t Pre_Context_NA : 1;
  uint64_t Pre_State_NA : 1;
  uint64_t Pre_Timer_NA : 1;
  uint64_t Pre_RealPriority_NA : 1;
  uint64_t Pre_ThreadDispatch_NA : 1;
  uint64_t Post_Status : 3;
  uint64_t Post_FatalError : 2;
  uint64_t Post_Argument : 2;
  uint64_t Post_State : 3;
  uint64_t Post_Enqueued : 2;
  uint64_t Post_Timer : 2;
  uint64_t Post_Restarting : 2;
  uint64_t Post_Terminating : 2;
  uint64_t Post_Protected : 2;
  uint64_t Post_RestartExtensions : 2;
  uint64_t Post_TerminateExtensions : 2;
} RtemsTaskReqRestart_Entry;

/**
 * @brief Test context for spec:/rtems/task/req/restart test case.
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
   * @brief This member provides the context to wrap thread queue operations.
   */
  WrapThreadQueueContext wrap_tq_ctx;

  /**
   * @brief This member contains the identifier of the runner scheduler.
   */
  rtems_id scheduler_id;

  /**
   * @brief This member contains the identifier of the runner task.
   */
  rtems_id runner_id;

  /**
   * @brief This member contains the identifier of the mutex.
   */
  rtems_id mutex_id;

  /**
   * @brief This member provides an event set used to set up the blocking
   *   conditions of the task to restart.
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
   *   rtems_task_restart() call.
   */
  States_Control worker_state;

  /**
   * @brief This member contains the worker thread life state at the end of the
   *   rtems_task_restart() call.
   */
  Thread_Life_state worker_life_state;

  /**
   * @brief This member contains the identifier of the deleter task.
   */
  rtems_id deleter_id;

  /**
   * @brief This member references the TCB of the deleter task.
   */
  rtems_tcb *deleter_tcb;

  /**
   * @brief This member contains the identifier of the test user extensions.
   */
  rtems_id extension_id;

  /**
   * @brief This member contains extension calls.
   */
  ExtensionCalls calls;

  /**
   * @brief This member contains extension calls after the rtems_task_restart()
   *   call.
   */
  ExtensionCalls calls_after_restart;

  /**
   * @brief This member contains the actual argument passed to the entry point.
   */
  rtems_task_argument actual_argument;

  /**
   * @brief This member contains the restart counter.
   */
  uint32_t restart_counter;

  /**
   * @brief If this member is true, then the worker shall be dormant before the
   *   rtems_task_restart() call.
   */
  bool dormant;

  /**
   * @brief If this member is true, then the worker shall be suspended before
   *   the rtems_task_restart() call.
   */
  bool suspended;

  /**
   * @brief If this member is true, then the thread life of the worker shall be
   *   protected before the rtems_task_restart() call.
   */
  bool protected;

  /**
   * @brief If this member is true, then the worker shall be restarting before
   *   the rtems_task_restart() call.
   */
  bool restarting;

  /**
   * @brief If this member is true, then the worker shall be terminating before
   *   the rtems_task_restart() call.
   */
  bool terminating;

  /**
   * @brief If this member is true, then the rtems_task_restart() shall be
   *   called from within interrupt context.
   */
  bool interrupt;

  /**
   * @brief If this member is true, then the rtems_task_restart() shall be
   *   called during another rtems_task_restart() call with the same task as a
   *   nested request.
   */
  bool nested_request;

  /**
   * @brief If this member is true, then the worker shall be blocked before the
   *   rtems_task_restart() call.
   */
  bool blocked;

  /**
   * @brief If this member is true, then the worker shall be enqueued on a wait
   *   queue before the rtems_task_restart() call.
   */
  bool enqueued;

  /**
   * @brief If this member is true, then the timer of the worker shall be
   *   active before the rtems_task_restart() call.
   */
  bool timer_active;

  /**
   * @brief If this member is true, then the real priority of the worker shall
   *   be equal to its initial priority before the rtems_task_restart() call.
   */
  bool real_priority_is_initial;

  /**
   * @brief If this member is true, then thread dispatching is disabled by the
   *   worker task before the rtems_task_restart() call.
   */
  bool dispatch_disabled;

  /**
   * @brief If this member is true, then it is expected to delete the worker.
   */
  bool delete_worker_expected;

  /**
   * @brief This member contains the return value of the rtems_task_restart()
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
    RtemsTaskReqRestart_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsTaskReqRestart_Context;

static RtemsTaskReqRestart_Context
  RtemsTaskReqRestart_Instance;

static const char * const RtemsTaskReqRestart_PreDesc_Id[] = {
  "Invalid",
  "Executing",
  "Other",
  "NA"
};

static const char * const RtemsTaskReqRestart_PreDesc_Dormant[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const RtemsTaskReqRestart_PreDesc_Suspended[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const RtemsTaskReqRestart_PreDesc_Restarting[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const RtemsTaskReqRestart_PreDesc_Terminating[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const RtemsTaskReqRestart_PreDesc_Protected[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const RtemsTaskReqRestart_PreDesc_Context[] = {
  "Task",
  "Interrupt",
  "NestedRequest",
  "NA"
};

static const char * const RtemsTaskReqRestart_PreDesc_State[] = {
  "Ready",
  "Blocked",
  "Enqueued",
  "NA"
};

static const char * const RtemsTaskReqRestart_PreDesc_Timer[] = {
  "Inactive",
  "Active",
  "NA"
};

static const char * const RtemsTaskReqRestart_PreDesc_RealPriority[] = {
  "Initial",
  "Changed",
  "NA"
};

static const char * const RtemsTaskReqRestart_PreDesc_ThreadDispatch[] = {
  "Disabled",
  "Enabled",
  "NA"
};

static const char * const * const RtemsTaskReqRestart_PreDesc[] = {
  RtemsTaskReqRestart_PreDesc_Id,
  RtemsTaskReqRestart_PreDesc_Dormant,
  RtemsTaskReqRestart_PreDesc_Suspended,
  RtemsTaskReqRestart_PreDesc_Restarting,
  RtemsTaskReqRestart_PreDesc_Terminating,
  RtemsTaskReqRestart_PreDesc_Protected,
  RtemsTaskReqRestart_PreDesc_Context,
  RtemsTaskReqRestart_PreDesc_State,
  RtemsTaskReqRestart_PreDesc_Timer,
  RtemsTaskReqRestart_PreDesc_RealPriority,
  RtemsTaskReqRestart_PreDesc_ThreadDispatch,
  NULL
};

#if CPU_SIZEOF_POINTER > 4
#define RESTART_ARGUMENT 0xfedcba0987654321U
#else
#define RESTART_ARGUMENT 0x87654321U
#endif

#define UNSET_ARGUMENT 1

typedef RtemsTaskReqRestart_Context Context;

static void PrepareRealPriority( Context *ctx )
{
  if ( !ctx->real_priority_is_initial ) {
    SetScheduler( ctx->worker_id, ctx->scheduler_id, PRIO_LOW );
    SetPriority( ctx->worker_id, PRIO_NORMAL );
  }
}

static void CaptureWorkerState( Context *ctx )
{
  T_scheduler_log *log;

  log = T_scheduler_record( NULL );

  if ( log != NULL ) {
    T_eq_ptr( &log->header, &ctx->scheduler_log.header );

    ctx->worker_state = ctx->worker_tcb->current_state;
    ctx->worker_life_state = ctx->worker_tcb->Life.state;
    CopyExtensionCalls( &ctx->calls, &ctx->calls_after_restart );
  }
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

      if ( ctx->nested_request ) {
        state |= STATES_LIFE_IS_CHANGING;
      }
    }

    T_eq_u32( ctx->worker_tcb->current_state, state );
  }
}

static void Restart( void *arg )
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
       * executing thread during the rtems_task_restart() call can happen
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

  ctx->status = rtems_task_restart( ctx->id, RESTART_ARGUMENT );

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

static void BlockDone( const Context *ctx )
{
  if ( ctx->enqueued ) {
    ReleaseMutex( ctx->mutex_id );
  }
}

static void Fatal(
  rtems_fatal_source source,
  rtems_fatal_code   code,
  void              *arg
)
{
  Context         *ctx;
  Per_CPU_Control *cpu_self;

  T_eq_int( source, INTERNAL_ERROR_CORE );
  T_eq_ulong( code, INTERNAL_ERROR_BAD_THREAD_DISPATCH_DISABLE_LEVEL );

  ctx = arg;
  ++ctx->calls.fatal;
  T_assert_eq_int( ctx->calls.fatal, 1 );

  CaptureWorkerState( ctx );

  cpu_self = _Per_CPU_Get();
  _Thread_Dispatch_unnest( cpu_self );
  _Thread_Dispatch_direct_no_return( cpu_self );
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

static void TriggerNestedRequestViaSelfRestart(
  Context         *ctx,
  Per_CPU_Control *cpu_self
)
{
  WrapThreadQueueExtract( &ctx->wrap_tq_ctx, ctx->worker_tcb );
  SetFatalHandler( ResumeThreadDispatch, ctx );

  if ( setjmp( ctx->thread_dispatch_context ) == 0 ) {
    (void) rtems_task_restart(
      RTEMS_SELF,
      (rtems_task_argument) ctx
    );
  } else {
    _Thread_Dispatch_unnest( cpu_self );
  }
}

static void Signal( rtems_signal_set signals )
{
  Context *ctx;

  (void) signals;
  ctx = T_fixture_context();

  if ( ctx->id == RTEMS_SELF ) {
    Per_CPU_Control *cpu_self;

    SetPriority( ctx->runner_id, PRIO_VERY_LOW );
    SetPriority( ctx->deleter_id, PRIO_VERY_LOW );

    if ( ctx->interrupt || ctx->nested_request ) {
      if ( ctx->blocked ) {
        SetFatalHandler( ResumeThreadDispatch, ctx );
        cpu_self = _Thread_Dispatch_disable();

        if ( setjmp( ctx->thread_dispatch_context ) == 0 ) {
          Block( ctx );
        } else {
          _Thread_Dispatch_unnest( cpu_self );
        }

        if ( ctx->interrupt ) {
          CallWithinISR( Restart, ctx );
        } else {
          TriggerNestedRequestViaSelfRestart( ctx, cpu_self );
        }

        _Thread_Dispatch_direct( cpu_self );
        BlockDone( ctx );
      } else {
        if ( ctx->interrupt ) {
          CallWithinISR( Restart, ctx );
        } else {
          cpu_self = _Thread_Dispatch_disable();
          TriggerNestedRequestViaSelfRestart( ctx, cpu_self );
          _Thread_Dispatch_direct( cpu_self );
        }
      }
    } else {
      Restart( ctx );
    }
  } else {
    if ( ctx->blocked ) {
      Block( ctx );
      BlockDone( ctx );
    } else if ( ctx->nested_request ) {
      Yield();
    } else {
      SetPriority( ctx->runner_id, PRIO_VERY_HIGH );
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
    if ( ctx->real_priority_is_initial ) {
      /*
       * We have to prevent a potential priority boost in the task delete
       * below.
       */
      if ( ctx->nested_request ) {
        /* Lower the priority to PRIO_NORMAL without an * implicit yield */
        SetSelfPriorityNoYield( PRIO_NORMAL );
      } else {
        SetScheduler( ctx->worker_id, ctx->scheduler_id, PRIO_HIGH );
      }
    }

    if ( ctx->nested_request ) {
      WrapThreadQueueExtract( &ctx->wrap_tq_ctx, ctx->worker_tcb );
      DeleteTask( ctx->worker_id );
    } else {
      DeleteTask( ctx->worker_id );
    }
  }

  SuspendSelf();
}

static void Worker( rtems_task_argument arg )
{
  Context          *ctx;
  rtems_status_code sc;

  ctx = T_fixture_context();

  if ( arg == 0 ) {
    sc = rtems_signal_catch( Signal, RTEMS_NO_ASR );
    T_rsc_success( sc );

    if ( ctx->protected ) {
      _Thread_Set_life_protection( THREAD_LIFE_PROTECTED );
    }

    Yield();
  } else {
    ctx->actual_argument = arg;
    ++ctx->restart_counter;

    CaptureWorkerState( ctx );
    SuspendSelf();
  }
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
}

static const rtems_extensions_table extensions = {
  .thread_delete = ThreadDelete,
  .thread_restart = ThreadRestart,
  .thread_terminate = ThreadTerminate
};

static void RtemsTaskReqRestart_Pre_Id_Prepare(
  RtemsTaskReqRestart_Context *ctx,
  RtemsTaskReqRestart_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsTaskReqRestart_Pre_Id_Invalid: {
      /*
       * While the ``id`` parameter is not associated with a task.
       */
      ctx->id = INVALID_ID;
      break;
    }

    case RtemsTaskReqRestart_Pre_Id_Executing: {
      /*
       * While the ``id`` parameter is associated with the calling task.
       */
      ctx->id = RTEMS_SELF;
      break;
    }

    case RtemsTaskReqRestart_Pre_Id_Other: {
      /*
       * While the ``id`` parameter is associated with a task other than the
       * calling task.
       */
      ctx->id = ctx->worker_id;
      break;
    }

    case RtemsTaskReqRestart_Pre_Id_NA:
      break;
  }
}

static void RtemsTaskReqRestart_Pre_Dormant_Prepare(
  RtemsTaskReqRestart_Context    *ctx,
  RtemsTaskReqRestart_Pre_Dormant state
)
{
  switch ( state ) {
    case RtemsTaskReqRestart_Pre_Dormant_Yes: {
      /*
       * While the task specified by the ``id`` parameter is dormant.
       */
      ctx->dormant = true;
      break;
    }

    case RtemsTaskReqRestart_Pre_Dormant_No: {
      /*
       * While the task specified by the ``id`` parameter is not dormant.
       */
      ctx->dormant = false;
      break;
    }

    case RtemsTaskReqRestart_Pre_Dormant_NA:
      break;
  }
}

static void RtemsTaskReqRestart_Pre_Suspended_Prepare(
  RtemsTaskReqRestart_Context      *ctx,
  RtemsTaskReqRestart_Pre_Suspended state
)
{
  switch ( state ) {
    case RtemsTaskReqRestart_Pre_Suspended_Yes: {
      /*
       * While the task specified by the ``id`` parameter is suspended.
       */
      ctx->suspended = true;
      break;
    }

    case RtemsTaskReqRestart_Pre_Suspended_No: {
      /*
       * While the task specified by the ``id`` parameter is not suspended.
       */
      ctx->suspended = false;
      break;
    }

    case RtemsTaskReqRestart_Pre_Suspended_NA:
      break;
  }
}

static void RtemsTaskReqRestart_Pre_Restarting_Prepare(
  RtemsTaskReqRestart_Context       *ctx,
  RtemsTaskReqRestart_Pre_Restarting state
)
{
  switch ( state ) {
    case RtemsTaskReqRestart_Pre_Restarting_Yes: {
      /*
       * While the task specified by the ``id`` parameter is restarting.
       */
      ctx->restarting = true;
      break;
    }

    case RtemsTaskReqRestart_Pre_Restarting_No: {
      /*
       * While the task specified by the ``id`` parameter is not restarting.
       */
      ctx->restarting = false;
      break;
    }

    case RtemsTaskReqRestart_Pre_Restarting_NA:
      break;
  }
}

static void RtemsTaskReqRestart_Pre_Terminating_Prepare(
  RtemsTaskReqRestart_Context        *ctx,
  RtemsTaskReqRestart_Pre_Terminating state
)
{
  switch ( state ) {
    case RtemsTaskReqRestart_Pre_Terminating_Yes: {
      /*
       * While the task specified by the ``id`` parameter is terminating.
       */
      ctx->terminating = true;
      break;
    }

    case RtemsTaskReqRestart_Pre_Terminating_No: {
      /*
       * While the task specified by the ``id`` parameter is not terminating.
       */
      ctx->terminating = false;
      break;
    }

    case RtemsTaskReqRestart_Pre_Terminating_NA:
      break;
  }
}

static void RtemsTaskReqRestart_Pre_Protected_Prepare(
  RtemsTaskReqRestart_Context      *ctx,
  RtemsTaskReqRestart_Pre_Protected state
)
{
  switch ( state ) {
    case RtemsTaskReqRestart_Pre_Protected_Yes: {
      /*
       * While thread life of the task specified by the ``id`` parameter is
       * protected.
       */
      ctx->protected = true;
      break;
    }

    case RtemsTaskReqRestart_Pre_Protected_No: {
      /*
       * While thread life of the task specified by the ``id`` parameter is not
       * protected.
       */
      ctx->protected = false;
      break;
    }

    case RtemsTaskReqRestart_Pre_Protected_NA:
      break;
  }
}

static void RtemsTaskReqRestart_Pre_Context_Prepare(
  RtemsTaskReqRestart_Context    *ctx,
  RtemsTaskReqRestart_Pre_Context state
)
{
  switch ( state ) {
    case RtemsTaskReqRestart_Pre_Context_Task: {
      /*
       * While the rtems_task_restart() directive is called from within task
       * context.
       */
      ctx->interrupt = false;
      ctx->nested_request = false;
      break;
    }

    case RtemsTaskReqRestart_Pre_Context_Interrupt: {
      /*
       * While the rtems_task_restart() directive is called from within
       * interrupt context.
       */
      ctx->interrupt = true;
      ctx->nested_request = false;
      break;
    }

    case RtemsTaskReqRestart_Pre_Context_NestedRequest: {
      /*
       * While the rtems_task_restart() directive is called during another
       * rtems_task_restart() call with the same task as a nested request.
       */
      ctx->interrupt = false;
      ctx->nested_request = true;
      break;
    }

    case RtemsTaskReqRestart_Pre_Context_NA:
      break;
  }
}

static void RtemsTaskReqRestart_Pre_State_Prepare(
  RtemsTaskReqRestart_Context  *ctx,
  RtemsTaskReqRestart_Pre_State state
)
{
  switch ( state ) {
    case RtemsTaskReqRestart_Pre_State_Ready: {
      /*
       * While the task specified by the ``id`` parameter is a ready task or
       * scheduled task.
       */
      ctx->blocked = false;
      break;
    }

    case RtemsTaskReqRestart_Pre_State_Blocked: {
      /*
       * While the task specified by the ``id`` parameter is blocked.
       */
      ctx->blocked = true;
      ctx->enqueued = false;
      break;
    }

    case RtemsTaskReqRestart_Pre_State_Enqueued: {
      /*
       * While the task specified by the ``id`` parameter is enqueued on a wait
       * queue.
       */
      ctx->blocked = true;
      ctx->enqueued = true;
      break;
    }

    case RtemsTaskReqRestart_Pre_State_NA:
      break;
  }
}

static void RtemsTaskReqRestart_Pre_Timer_Prepare(
  RtemsTaskReqRestart_Context  *ctx,
  RtemsTaskReqRestart_Pre_Timer state
)
{
  switch ( state ) {
    case RtemsTaskReqRestart_Pre_Timer_Inactive: {
      /*
       * While timer of the task specified by the ``id`` parameter is inactive.
       */
      ctx->timer_active = false;
      break;
    }

    case RtemsTaskReqRestart_Pre_Timer_Active: {
      /*
       * While timer of the task specified by the ``id`` parameter is active.
       */
      ctx->timer_active = true;
      break;
    }

    case RtemsTaskReqRestart_Pre_Timer_NA:
      break;
  }
}

static void RtemsTaskReqRestart_Pre_RealPriority_Prepare(
  RtemsTaskReqRestart_Context         *ctx,
  RtemsTaskReqRestart_Pre_RealPriority state
)
{
  switch ( state ) {
    case RtemsTaskReqRestart_Pre_RealPriority_Initial: {
      /*
       * While real priority of the task specified by the ``id`` parameter is
       * equal to the initial priority.
       */
      ctx->real_priority_is_initial = true;
      break;
    }

    case RtemsTaskReqRestart_Pre_RealPriority_Changed: {
      /*
       * While real priority of the task specified by the ``id`` parameter is
       * not equal to the initial priority.
       */
      ctx->real_priority_is_initial = false;
      break;
    }

    case RtemsTaskReqRestart_Pre_RealPriority_NA:
      break;
  }
}

static void RtemsTaskReqRestart_Pre_ThreadDispatch_Prepare(
  RtemsTaskReqRestart_Context           *ctx,
  RtemsTaskReqRestart_Pre_ThreadDispatch state
)
{
  switch ( state ) {
    case RtemsTaskReqRestart_Pre_ThreadDispatch_Disabled: {
      /*
       * While thread dispatching is disabled for the calling task.
       */
      ctx->dispatch_disabled = true;
      break;
    }

    case RtemsTaskReqRestart_Pre_ThreadDispatch_Enabled: {
      /*
       * While thread dispatching is enabled for the calling task.
       */
      ctx->dispatch_disabled = false;
      break;
    }

    case RtemsTaskReqRestart_Pre_ThreadDispatch_NA:
      break;
  }
}

static void RtemsTaskReqRestart_Post_Status_Check(
  RtemsTaskReqRestart_Context    *ctx,
  RtemsTaskReqRestart_Post_Status state
)
{
  switch ( state ) {
    case RtemsTaskReqRestart_Post_Status_Ok: {
      /*
       * The return status of rtems_task_restart() shall be RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsTaskReqRestart_Post_Status_InvId: {
      /*
       * The return status of rtems_task_restart() shall be RTEMS_INVALID_ID.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ID );
      break;
    }

    case RtemsTaskReqRestart_Post_Status_IncStat: {
      /*
       * The return status of rtems_task_restart() shall be
       * RTEMS_INCORRECT_STATE.
       */
      T_rsc( ctx->status, RTEMS_INCORRECT_STATE );
      break;
    }

    case RtemsTaskReqRestart_Post_Status_NoReturn: {
      /*
       * The rtems_task_restart() call shall not return.
       */
      T_rsc( ctx->status, RTEMS_NOT_IMPLEMENTED );
      break;
    }

    case RtemsTaskReqRestart_Post_Status_NA:
      break;
  }
}

static void RtemsTaskReqRestart_Post_FatalError_Check(
  RtemsTaskReqRestart_Context        *ctx,
  RtemsTaskReqRestart_Post_FatalError state
)
{
  switch ( state ) {
    case RtemsTaskReqRestart_Post_FatalError_Yes: {
      /*
       * The fatal error with a fatal source of INTERNAL_ERROR_CORE and a fatal
       * code of INTERNAL_ERROR_BAD_THREAD_DISPATCH_DISABLE_LEVEL shall occur
       * through the rtems_task_restart() call.
       */
      T_eq_u32( ctx->calls.fatal, 1 );
      break;
    }

    case RtemsTaskReqRestart_Post_FatalError_Nop: {
      /*
       * No fatal error shall occur through the rtems_task_restart() call.
       */
      T_eq_u32( ctx->calls.fatal, 0 );
      break;
    }

    case RtemsTaskReqRestart_Post_FatalError_NA:
      break;
  }
}

static void RtemsTaskReqRestart_Post_Argument_Check(
  RtemsTaskReqRestart_Context      *ctx,
  RtemsTaskReqRestart_Post_Argument state
)
{
  switch ( state ) {
    case RtemsTaskReqRestart_Post_Argument_Set: {
      /*
       * The entry point argument of the task specified by the ``id`` parameter
       * shall be set to the value specified by the ``argument`` parameter
       * before the task is unblocked by the rtems_task_restart() call.
       */
      if ( ctx->restart_counter != 0 ) {
        #if CPU_SIZEOF_POINTER > 4
        T_eq_u64( ctx->actual_argument, RESTART_ARGUMENT );
        #else
        T_eq_u32( ctx->actual_argument, RESTART_ARGUMENT );
        #endif

        T_eq_u32( ctx->restart_counter, 1 );
      } else {
        #if CPU_SIZEOF_POINTER > 4
        T_eq_u64(
          ctx->worker_tcb->Start.Entry.Kinds.Numeric.argument,
          RESTART_ARGUMENT
        );
        T_eq_u64( ctx->actual_argument, UNSET_ARGUMENT );
        #else
        T_eq_u32(
          ctx->worker_tcb->Start.Entry.Kinds.Numeric.argument,
          RESTART_ARGUMENT
        );
        T_eq_u32( ctx->actual_argument, UNSET_ARGUMENT );
        #endif
      }
      break;
    }

    case RtemsTaskReqRestart_Post_Argument_Nop: {
      /*
       * No entry point argument of a task shall be modified by the
       * rtems_task_restart() call.
       */
      T_eq_u32( ctx->actual_argument, UNSET_ARGUMENT );
      T_eq_u32( ctx->restart_counter, 0 );
      break;
    }

    case RtemsTaskReqRestart_Post_Argument_NA:
      break;
  }
}

static void RtemsTaskReqRestart_Post_State_Check(
  RtemsTaskReqRestart_Context   *ctx,
  RtemsTaskReqRestart_Post_State state
)
{
  const T_scheduler_event *event;
  size_t                   index;

  index = 0;

  switch ( state ) {
    case RtemsTaskReqRestart_Post_State_Dormant: {
      /*
       * The state of the task specified by the ``id`` parameter shall be
       * dormant after the rtems_task_restart() call.
       */
      T_eq_u32( ctx->worker_state, STATES_DORMANT )

      event = T_scheduler_next_any( &ctx->scheduler_log.header, &index );
      T_eq_int( event->operation, T_SCHEDULER_NOP );
      break;
    }

    case RtemsTaskReqRestart_Post_State_DormantSuspended: {
      /*
       * The state of the task specified by the ``id`` parameter shall be
       * dormant and suspended after the rtems_task_restart() call.
       */
      T_eq_u32( ctx->worker_state, STATES_DORMANT | STATES_SUSPENDED )

      event = T_scheduler_next_any( &ctx->scheduler_log.header, &index );
      T_eq_int( event->operation, T_SCHEDULER_NOP );
      break;
    }

    case RtemsTaskReqRestart_Post_State_Blocked: {
      /*
       * The state of the task specified by the ``id`` parameter shall be
       * blocked after the rtems_task_restart() call.
       */
      T_ne_u32( ctx->worker_state & STATES_BLOCKED, 0 )
      T_eq_u32( ctx->worker_state & STATES_BLOCKED, ctx->worker_state )

      if ( ctx->suspended && !ctx->blocked ) {
        event = T_scheduler_next_any( &ctx->scheduler_log.header, &index );
        T_eq_int( event->operation, T_SCHEDULER_UNBLOCK );
        T_eq_ptr( event->thread, ctx->worker_tcb );
      }

      if ( !ctx->real_priority_is_initial && !ctx->terminating ) {
        event = T_scheduler_next_any( &ctx->scheduler_log.header, &index );
        T_eq_int( event->operation, T_SCHEDULER_UPDATE_PRIORITY );
        T_eq_ptr( event->thread, ctx->worker_tcb );
      }

      event = T_scheduler_next_any( &ctx->scheduler_log.header, &index );
      T_eq_int( event->operation, T_SCHEDULER_NOP );
      break;
    }

    case RtemsTaskReqRestart_Post_State_Ready: {
      /*
       * The state of the task specified by the ``id`` parameter shall be ready
       * after the rtems_task_restart() call.
       */
      T_eq_u32( ctx->worker_state, STATES_READY )

      if ( ctx->protected ) {
        if ( ctx->suspended ) {
          event = T_scheduler_next_any( &ctx->scheduler_log.header, &index );
          T_eq_int( event->operation, T_SCHEDULER_UNBLOCK );
          T_eq_ptr( event->thread, ctx->worker_tcb );
        }
      } else {
        if ( ctx->suspended || ctx->blocked ) {
          event = T_scheduler_next_any( &ctx->scheduler_log.header, &index );
          T_eq_int( event->operation, T_SCHEDULER_UNBLOCK );
          T_eq_ptr( event->thread, ctx->worker_tcb );
        } else {
          event = T_scheduler_next_any( &ctx->scheduler_log.header, &index );
          T_eq_int( event->operation, T_SCHEDULER_BLOCK );
          T_eq_ptr( event->thread, ctx->worker_tcb );

          event = T_scheduler_next_any( &ctx->scheduler_log.header, &index );
          T_eq_int( event->operation, T_SCHEDULER_UNBLOCK );
          T_eq_ptr( event->thread, ctx->worker_tcb );
        }
      }

      if ( !ctx->real_priority_is_initial && !ctx->terminating ) {
        event = T_scheduler_next_any( &ctx->scheduler_log.header, &index );
        T_eq_int( event->operation, T_SCHEDULER_UPDATE_PRIORITY );
        T_eq_ptr( event->thread, ctx->worker_tcb );
      }

      event = T_scheduler_next_any( &ctx->scheduler_log.header, &index );
      T_eq_int( event->operation, T_SCHEDULER_NOP );
      break;
    }

    case RtemsTaskReqRestart_Post_State_Zombie: {
      /*
       * The state of the task specified by the ``id`` parameter shall be the
       * zombie state after the rtems_task_restart() call.
       */
      T_eq_u32( ctx->worker_state, STATES_ZOMBIE )

      if ( ctx->protected ) {
        if ( ctx->suspended ) {
          event = T_scheduler_next_any( &ctx->scheduler_log.header, &index );
          T_eq_int( event->operation, T_SCHEDULER_UNBLOCK );
          T_eq_ptr( event->thread, ctx->worker_tcb );
        }
      } else {
        if ( ctx->suspended ) {
          event = T_scheduler_next_any( &ctx->scheduler_log.header, &index );
          T_eq_int( event->operation, T_SCHEDULER_UNBLOCK );
          T_eq_ptr( event->thread, ctx->worker_tcb );
        } else {
          event = T_scheduler_next_any( &ctx->scheduler_log.header, &index );
          T_eq_int( event->operation, T_SCHEDULER_BLOCK );
          T_eq_ptr( event->thread, ctx->worker_tcb );

          event = T_scheduler_next_any( &ctx->scheduler_log.header, &index );
          T_eq_int( event->operation, T_SCHEDULER_UNBLOCK );
          T_eq_ptr( event->thread, ctx->worker_tcb );
        }
      }

      if ( !ctx->real_priority_is_initial && !ctx->terminating ) {
        event = T_scheduler_next_any( &ctx->scheduler_log.header, &index );
        T_eq_int( event->operation, T_SCHEDULER_UPDATE_PRIORITY );
        T_eq_ptr( event->thread, ctx->worker_tcb );
      }

      /* Set zombie state */
      event = T_scheduler_next_any( &ctx->scheduler_log.header, &index );
      T_eq_int( event->operation, T_SCHEDULER_BLOCK );
      T_eq_ptr( event->thread, ctx->worker_tcb );

      /* Wake up deleter */
      event = T_scheduler_next_any( &ctx->scheduler_log.header, &index );
      T_eq_int( event->operation, T_SCHEDULER_UNBLOCK );
      T_eq_ptr( event->thread, ctx->deleter_tcb );

      event = T_scheduler_next_any( &ctx->scheduler_log.header, &index );
      T_eq_int( event->operation, T_SCHEDULER_NOP );
      break;
    }

    case RtemsTaskReqRestart_Post_State_Nop: {
      /*
       * The state of the task specified by the ``id`` parameter shall not be
       * modified by the rtems_task_restart() call.
       */
      T_ne_u32( ctx->worker_state & STATES_LIFE_IS_CHANGING, 0 )

      if ( !ctx->real_priority_is_initial ) {
        event = T_scheduler_next_any( &ctx->scheduler_log.header, &index );
        T_eq_int( event->operation, T_SCHEDULER_UPDATE_PRIORITY );
        T_eq_ptr( event->thread, ctx->worker_tcb );
      }

      event = T_scheduler_next_any( &ctx->scheduler_log.header, &index );
      T_eq_int( event->operation, T_SCHEDULER_NOP );
      break;
    }

    case RtemsTaskReqRestart_Post_State_NA:
      break;
  }
}

static void RtemsTaskReqRestart_Post_Enqueued_Check(
  RtemsTaskReqRestart_Context      *ctx,
  RtemsTaskReqRestart_Post_Enqueued state
)
{
  switch ( state ) {
    case RtemsTaskReqRestart_Post_Enqueued_Yes: {
      /*
       * The task specified by the ``id`` parameter shall be enqueued on a wait
       * queue after the rtems_task_restart() call.
       */
      T_not_null( ctx->worker_tcb->Wait.queue );
      break;
    }

    case RtemsTaskReqRestart_Post_Enqueued_No: {
      /*
       * The task specified by the ``id`` parameter shall not be enqueued on a
       * wait queue after the rtems_task_restart() call.
       */
      T_null( ctx->worker_tcb->Wait.queue );
      break;
    }

    case RtemsTaskReqRestart_Post_Enqueued_NA:
      break;
  }
}

static void RtemsTaskReqRestart_Post_Timer_Check(
  RtemsTaskReqRestart_Context   *ctx,
  RtemsTaskReqRestart_Post_Timer state
)
{
  TaskTimerInfo info;

  switch ( state ) {
    case RtemsTaskReqRestart_Post_Timer_Active: {
      /*
       * The timer of the task specified by the ``id`` parameter shall be
       * active after the rtems_task_restart() call.
       */
      GetTaskTimerInfoByThread( ctx->worker_tcb, &info);
      T_eq_int( info.state, TASK_TIMER_TICKS );
      break;
    }

    case RtemsTaskReqRestart_Post_Timer_Inactive: {
      /*
       * The timer of the task specified by the ``id`` parameter shall be
       * inactive after the rtems_task_restart() call.
       */
      GetTaskTimerInfoByThread( ctx->worker_tcb, &info);
      T_eq_int( info.state, TASK_TIMER_INACTIVE );
      break;
    }

    case RtemsTaskReqRestart_Post_Timer_NA:
      break;
  }
}

static void RtemsTaskReqRestart_Post_Restarting_Check(
  RtemsTaskReqRestart_Context        *ctx,
  RtemsTaskReqRestart_Post_Restarting state
)
{
  switch ( state ) {
    case RtemsTaskReqRestart_Post_Restarting_Yes: {
      /*
       * The task specified by the ``id`` parameter shall be restarting after
       * the rtems_task_restart() call.
       */
      T_ne_int( ctx->worker_life_state & THREAD_LIFE_RESTARTING, 0 );
      break;
    }

    case RtemsTaskReqRestart_Post_Restarting_No: {
      /*
       * The task specified by the ``id`` parameter shall not be restarting
       * after the rtems_task_restart() call.
       */
      T_eq_int( ctx->worker_life_state & THREAD_LIFE_RESTARTING, 0 );
      break;
    }

    case RtemsTaskReqRestart_Post_Restarting_NA:
      break;
  }
}

static void RtemsTaskReqRestart_Post_Terminating_Check(
  RtemsTaskReqRestart_Context         *ctx,
  RtemsTaskReqRestart_Post_Terminating state
)
{
  switch ( state ) {
    case RtemsTaskReqRestart_Post_Terminating_Yes: {
      /*
       * The task specified by the ``id`` parameter shall be terminating after
       * the rtems_task_restart() call.
       */
      T_ne_int( ctx->worker_life_state & THREAD_LIFE_TERMINATING, 0 );
      break;
    }

    case RtemsTaskReqRestart_Post_Terminating_No: {
      /*
       * The task specified by the ``id`` parameter shall not be terminating
       * after the rtems_task_restart() call.
       */
      T_eq_int( ctx->worker_life_state & THREAD_LIFE_TERMINATING, 0 );
      break;
    }

    case RtemsTaskReqRestart_Post_Terminating_NA:
      break;
  }
}

static void RtemsTaskReqRestart_Post_Protected_Check(
  RtemsTaskReqRestart_Context       *ctx,
  RtemsTaskReqRestart_Post_Protected state
)
{
  switch ( state ) {
    case RtemsTaskReqRestart_Post_Protected_Yes: {
      /*
       * The thread life of the task specified by the ``id`` parameter be
       * protected after the rtems_task_restart() call.
       */
      T_ne_int( ctx->worker_life_state & THREAD_LIFE_PROTECTED, 0 );
      break;
    }

    case RtemsTaskReqRestart_Post_Protected_No: {
      /*
       * The thread life of the task specified by the ``id`` parameter shall
       * not be protected after the rtems_task_restart() call.
       */
      T_eq_int( ctx->worker_life_state & THREAD_LIFE_PROTECTED, 0 );
      break;
    }

    case RtemsTaskReqRestart_Post_Protected_NA:
      break;
  }
}

static void RtemsTaskReqRestart_Post_RestartExtensions_Check(
  RtemsTaskReqRestart_Context               *ctx,
  RtemsTaskReqRestart_Post_RestartExtensions state
)
{
  switch ( state ) {
    case RtemsTaskReqRestart_Post_RestartExtensions_Yes: {
      /*
       * The thread restart user extensions shall be invoked by the
       * rtems_task_restart() call.
       */
      T_eq_u32( ctx->calls_after_restart.thread_restart, 1 );
      break;
    }

    case RtemsTaskReqRestart_Post_RestartExtensions_Nop: {
      /*
       * The thread restart user extensions shall not be invoked by the
       * rtems_task_restart() call.
       */
      T_eq_u32( ctx->calls_after_restart.thread_restart, 0 );
      break;
    }

    case RtemsTaskReqRestart_Post_RestartExtensions_NA:
      break;
  }
}

static void RtemsTaskReqRestart_Post_TerminateExtensions_Check(
  RtemsTaskReqRestart_Context                 *ctx,
  RtemsTaskReqRestart_Post_TerminateExtensions state
)
{
  switch ( state ) {
    case RtemsTaskReqRestart_Post_TerminateExtensions_Yes: {
      /*
       * The thread terminate user extensions shall be invoked by the
       * rtems_task_restart() call.
       */
      T_eq_u32( ctx->calls_after_restart.thread_terminate, 1 );
      break;
    }

    case RtemsTaskReqRestart_Post_TerminateExtensions_Nop: {
      /*
       * The thread terminate user extensions shall not be invoked by the
       * rtems_task_restart() call.
       */
      T_eq_u32( ctx->calls_after_restart.thread_terminate, 0 );
      break;
    }

    case RtemsTaskReqRestart_Post_TerminateExtensions_NA:
      break;
  }
}

static void RtemsTaskReqRestart_Setup( RtemsTaskReqRestart_Context *ctx )
{
  rtems_status_code sc;

  ctx->runner_id = rtems_task_self();
  ctx->scheduler_id = GetSelfScheduler();
  ctx->mutex_id = CreateMutexNoProtocol();
  ObtainMutex( ctx->mutex_id );
  WrapThreadQueueInitialize( &ctx->wrap_tq_ctx, Restart, ctx );

  sc = rtems_extension_create(
    rtems_build_name( 'T', 'E', 'S', 'T' ),
    &extensions,
    &ctx->extension_id
  );
  T_rsc_success( sc );

  SetFatalHandler( Fatal, ctx );
  SetSelfPriority( PRIO_NORMAL );

  ctx->deleter_id = CreateTask( "DELE", PRIO_HIGH );
  ctx->deleter_tcb = GetThread( ctx->deleter_id );
  StartTask( ctx->deleter_id, Deleter, NULL );
}

static void RtemsTaskReqRestart_Setup_Wrap( void *arg )
{
  RtemsTaskReqRestart_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsTaskReqRestart_Setup( ctx );
}

static void RtemsTaskReqRestart_Teardown( RtemsTaskReqRestart_Context *ctx )
{
  rtems_status_code sc;

  sc = rtems_extension_delete( ctx->extension_id );
  T_rsc_success( sc );

  SetFatalHandler( NULL, NULL );
  DeleteTask( ctx->deleter_id );
  ReleaseMutex( ctx->mutex_id );
  DeleteMutex( ctx->mutex_id );
  RestoreRunnerASR();
  RestoreRunnerPriority();
  WrapThreadQueueDestroy( &ctx->wrap_tq_ctx );
}

static void RtemsTaskReqRestart_Teardown_Wrap( void *arg )
{
  RtemsTaskReqRestart_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsTaskReqRestart_Teardown( ctx );
}

static void RtemsTaskReqRestart_Prepare( RtemsTaskReqRestart_Context *ctx )
{
  ctx->status = RTEMS_NOT_IMPLEMENTED;
  ctx->actual_argument = UNSET_ARGUMENT;
  ctx->restart_counter = 0;

  ctx->delete_worker_expected = false;
  ctx->worker_id = CreateTask( "WORK", PRIO_NORMAL );
  ctx->delete_worker_expected = true;

  ctx->worker_tcb = GetThread( ctx->worker_id );
  ctx->worker_state = UINT32_MAX;
  ctx->worker_life_state = INT_MAX;

  SetPriority( ctx->deleter_id, PRIO_HIGH );
}

static void RtemsTaskReqRestart_Action( RtemsTaskReqRestart_Context *ctx )
{
  rtems_status_code sc;

  if ( ctx->id != INVALID_ID ) {
    if ( ctx->dormant ) {
      PrepareRealPriority( ctx );
    } else {
      StartTask( ctx->worker_id, Worker, NULL );

      /* Let the worker catch signals and set the thread life protection state */
      Yield();

      sc = rtems_signal_send( ctx->worker_id, RTEMS_SIGNAL_0 );
      T_rsc_success( sc );

      if (
        ctx->restarting &&
        ( !ctx->nested_request || ( ctx->nested_request && ctx->terminating ) )
      ) {
        sc = rtems_task_restart( ctx->worker_id, (rtems_task_argument) ctx );
        T_rsc_success( sc );
      }

      if ( ctx->terminating && !ctx->nested_request ) {
        sc = rtems_task_restart( ctx->deleter_id, (rtems_task_argument) ctx );
        T_rsc_success( sc );
      } else {
        PrepareRealPriority( ctx );
        Yield();
      }
    }
  }

  if ( ctx->id == RTEMS_SELF ) {
    CaptureWorkerState( ctx );
  } else {
    if ( ctx->nested_request ) {
      if ( ctx->terminating ) {
        sc = rtems_task_restart( ctx->deleter_id, (rtems_task_argument) ctx );
        T_rsc_success( sc );
      } else {
        WrapThreadQueueExtract( &ctx->wrap_tq_ctx, ctx->worker_tcb );

        sc = rtems_task_restart( ctx->worker_id, (rtems_task_argument) ctx );
        T_rsc_success( sc );
      }
    } else {
      SetSelfPriority( PRIO_VERY_HIGH );

      if ( ctx->interrupt ) {
        CallWithinISR( Restart, ctx );
      } else {
        Restart( ctx );
      }
    }
  }
}

static void RtemsTaskReqRestart_Cleanup( RtemsTaskReqRestart_Context *ctx )
{
  SetSelfPriority( PRIO_VERY_LOW );

  if ( ctx->protected && ctx->blocked ) {
    if ( ctx->enqueued ) {
      ReleaseMutex( ctx->mutex_id );
      ObtainMutex( ctx->mutex_id );
    } else {
      SendEvents( ctx->worker_id, RTEMS_EVENT_0 );
    }
  }

  if ( ctx->id == INVALID_ID || ctx->dormant || !ctx->terminating ) {
    DeleteTask( ctx->worker_id );
  }

  SetSelfPriority( PRIO_NORMAL );
}

static const RtemsTaskReqRestart_Entry
RtemsTaskReqRestart_Entries[] = {
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqRestart_Post_Status_NA,
    RtemsTaskReqRestart_Post_FatalError_NA,
    RtemsTaskReqRestart_Post_Argument_NA, RtemsTaskReqRestart_Post_State_NA,
    RtemsTaskReqRestart_Post_Enqueued_NA, RtemsTaskReqRestart_Post_Timer_NA,
    RtemsTaskReqRestart_Post_Restarting_NA,
    RtemsTaskReqRestart_Post_Terminating_NA,
    RtemsTaskReqRestart_Post_Protected_NA,
    RtemsTaskReqRestart_Post_RestartExtensions_NA,
    RtemsTaskReqRestart_Post_TerminateExtensions_NA },
  { 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, RtemsTaskReqRestart_Post_Status_InvId,
    RtemsTaskReqRestart_Post_FatalError_Nop,
    RtemsTaskReqRestart_Post_Argument_NA, RtemsTaskReqRestart_Post_State_NA,
    RtemsTaskReqRestart_Post_Enqueued_NA, RtemsTaskReqRestart_Post_Timer_NA,
    RtemsTaskReqRestart_Post_Restarting_NA,
    RtemsTaskReqRestart_Post_Terminating_NA,
    RtemsTaskReqRestart_Post_Protected_NA,
    RtemsTaskReqRestart_Post_RestartExtensions_Nop,
    RtemsTaskReqRestart_Post_TerminateExtensions_Nop },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqRestart_Post_Status_NA,
    RtemsTaskReqRestart_Post_FatalError_NA,
    RtemsTaskReqRestart_Post_Argument_NA, RtemsTaskReqRestart_Post_State_NA,
    RtemsTaskReqRestart_Post_Enqueued_NA, RtemsTaskReqRestart_Post_Timer_NA,
    RtemsTaskReqRestart_Post_Restarting_NA,
    RtemsTaskReqRestart_Post_Terminating_NA,
    RtemsTaskReqRestart_Post_Protected_NA,
    RtemsTaskReqRestart_Post_RestartExtensions_NA,
    RtemsTaskReqRestart_Post_TerminateExtensions_NA },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqRestart_Post_Status_NA,
    RtemsTaskReqRestart_Post_FatalError_NA,
    RtemsTaskReqRestart_Post_Argument_NA, RtemsTaskReqRestart_Post_State_NA,
    RtemsTaskReqRestart_Post_Enqueued_NA, RtemsTaskReqRestart_Post_Timer_NA,
    RtemsTaskReqRestart_Post_Restarting_NA,
    RtemsTaskReqRestart_Post_Terminating_NA,
    RtemsTaskReqRestart_Post_Protected_NA,
    RtemsTaskReqRestart_Post_RestartExtensions_NA,
    RtemsTaskReqRestart_Post_TerminateExtensions_NA },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqRestart_Post_Status_NA,
    RtemsTaskReqRestart_Post_FatalError_NA,
    RtemsTaskReqRestart_Post_Argument_NA, RtemsTaskReqRestart_Post_State_NA,
    RtemsTaskReqRestart_Post_Enqueued_NA, RtemsTaskReqRestart_Post_Timer_NA,
    RtemsTaskReqRestart_Post_Restarting_NA,
    RtemsTaskReqRestart_Post_Terminating_NA,
    RtemsTaskReqRestart_Post_Protected_NA,
    RtemsTaskReqRestart_Post_RestartExtensions_NA,
    RtemsTaskReqRestart_Post_TerminateExtensions_NA },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqRestart_Post_Status_NA,
    RtemsTaskReqRestart_Post_FatalError_NA,
    RtemsTaskReqRestart_Post_Argument_NA, RtemsTaskReqRestart_Post_State_NA,
    RtemsTaskReqRestart_Post_Enqueued_NA, RtemsTaskReqRestart_Post_Timer_NA,
    RtemsTaskReqRestart_Post_Restarting_NA,
    RtemsTaskReqRestart_Post_Terminating_NA,
    RtemsTaskReqRestart_Post_Protected_NA,
    RtemsTaskReqRestart_Post_RestartExtensions_NA,
    RtemsTaskReqRestart_Post_TerminateExtensions_NA },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqRestart_Post_Status_NA,
    RtemsTaskReqRestart_Post_FatalError_NA,
    RtemsTaskReqRestart_Post_Argument_NA, RtemsTaskReqRestart_Post_State_NA,
    RtemsTaskReqRestart_Post_Enqueued_NA, RtemsTaskReqRestart_Post_Timer_NA,
    RtemsTaskReqRestart_Post_Restarting_NA,
    RtemsTaskReqRestart_Post_Terminating_NA,
    RtemsTaskReqRestart_Post_Protected_NA,
    RtemsTaskReqRestart_Post_RestartExtensions_NA,
    RtemsTaskReqRestart_Post_TerminateExtensions_NA },
  { 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0,
    RtemsTaskReqRestart_Post_Status_IncStat,
    RtemsTaskReqRestart_Post_FatalError_Nop,
    RtemsTaskReqRestart_Post_Argument_Nop,
    RtemsTaskReqRestart_Post_State_DormantSuspended,
    RtemsTaskReqRestart_Post_Enqueued_No,
    RtemsTaskReqRestart_Post_Timer_Inactive,
    RtemsTaskReqRestart_Post_Restarting_No,
    RtemsTaskReqRestart_Post_Terminating_No,
    RtemsTaskReqRestart_Post_Protected_No,
    RtemsTaskReqRestart_Post_RestartExtensions_Nop,
    RtemsTaskReqRestart_Post_TerminateExtensions_Nop },
  { 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0,
    RtemsTaskReqRestart_Post_Status_IncStat,
    RtemsTaskReqRestart_Post_FatalError_Nop,
    RtemsTaskReqRestart_Post_Argument_Nop,
    RtemsTaskReqRestart_Post_State_Dormant,
    RtemsTaskReqRestart_Post_Enqueued_No,
    RtemsTaskReqRestart_Post_Timer_Inactive,
    RtemsTaskReqRestart_Post_Restarting_No,
    RtemsTaskReqRestart_Post_Terminating_No,
    RtemsTaskReqRestart_Post_Protected_No,
    RtemsTaskReqRestart_Post_RestartExtensions_Nop,
    RtemsTaskReqRestart_Post_TerminateExtensions_Nop },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqRestart_Post_Status_NA,
    RtemsTaskReqRestart_Post_FatalError_NA,
    RtemsTaskReqRestart_Post_Argument_NA, RtemsTaskReqRestart_Post_State_NA,
    RtemsTaskReqRestart_Post_Enqueued_NA, RtemsTaskReqRestart_Post_Timer_NA,
    RtemsTaskReqRestart_Post_Restarting_NA,
    RtemsTaskReqRestart_Post_Terminating_NA,
    RtemsTaskReqRestart_Post_Protected_NA,
    RtemsTaskReqRestart_Post_RestartExtensions_NA,
    RtemsTaskReqRestart_Post_TerminateExtensions_NA },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqRestart_Post_Status_NA,
    RtemsTaskReqRestart_Post_FatalError_NA,
    RtemsTaskReqRestart_Post_Argument_NA, RtemsTaskReqRestart_Post_State_NA,
    RtemsTaskReqRestart_Post_Enqueued_NA, RtemsTaskReqRestart_Post_Timer_NA,
    RtemsTaskReqRestart_Post_Restarting_NA,
    RtemsTaskReqRestart_Post_Terminating_NA,
    RtemsTaskReqRestart_Post_Protected_NA,
    RtemsTaskReqRestart_Post_RestartExtensions_NA,
    RtemsTaskReqRestart_Post_TerminateExtensions_NA },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqRestart_Post_Status_NA,
    RtemsTaskReqRestart_Post_FatalError_NA,
    RtemsTaskReqRestart_Post_Argument_NA, RtemsTaskReqRestart_Post_State_NA,
    RtemsTaskReqRestart_Post_Enqueued_NA, RtemsTaskReqRestart_Post_Timer_NA,
    RtemsTaskReqRestart_Post_Restarting_NA,
    RtemsTaskReqRestart_Post_Terminating_NA,
    RtemsTaskReqRestart_Post_Protected_NA,
    RtemsTaskReqRestart_Post_RestartExtensions_NA,
    RtemsTaskReqRestart_Post_TerminateExtensions_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqRestart_Post_Status_Ok,
    RtemsTaskReqRestart_Post_FatalError_Nop,
    RtemsTaskReqRestart_Post_Argument_Set,
    RtemsTaskReqRestart_Post_State_Ready, RtemsTaskReqRestart_Post_Enqueued_No,
    RtemsTaskReqRestart_Post_Timer_Inactive,
    RtemsTaskReqRestart_Post_Restarting_Yes,
    RtemsTaskReqRestart_Post_Terminating_Yes,
    RtemsTaskReqRestart_Post_Protected_No,
    RtemsTaskReqRestart_Post_RestartExtensions_Nop,
    RtemsTaskReqRestart_Post_TerminateExtensions_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqRestart_Post_Status_Ok,
    RtemsTaskReqRestart_Post_FatalError_Nop,
    RtemsTaskReqRestart_Post_Argument_Set,
    RtemsTaskReqRestart_Post_State_Ready, RtemsTaskReqRestart_Post_Enqueued_No,
    RtemsTaskReqRestart_Post_Timer_Inactive,
    RtemsTaskReqRestart_Post_Restarting_Yes,
    RtemsTaskReqRestart_Post_Terminating_No,
    RtemsTaskReqRestart_Post_Protected_No,
    RtemsTaskReqRestart_Post_RestartExtensions_Nop,
    RtemsTaskReqRestart_Post_TerminateExtensions_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqRestart_Post_Status_Ok,
    RtemsTaskReqRestart_Post_FatalError_Nop,
    RtemsTaskReqRestart_Post_Argument_Set, RtemsTaskReqRestart_Post_State_Nop,
    RtemsTaskReqRestart_Post_Enqueued_No,
    RtemsTaskReqRestart_Post_Timer_Inactive,
    RtemsTaskReqRestart_Post_Restarting_Yes,
    RtemsTaskReqRestart_Post_Terminating_No,
    RtemsTaskReqRestart_Post_Protected_No,
    RtemsTaskReqRestart_Post_RestartExtensions_Nop,
    RtemsTaskReqRestart_Post_TerminateExtensions_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqRestart_Post_Status_Ok,
    RtemsTaskReqRestart_Post_FatalError_Nop,
    RtemsTaskReqRestart_Post_Argument_Set, RtemsTaskReqRestart_Post_State_Nop,
    RtemsTaskReqRestart_Post_Enqueued_No,
    RtemsTaskReqRestart_Post_Timer_Inactive,
    RtemsTaskReqRestart_Post_Restarting_Yes,
    RtemsTaskReqRestart_Post_Terminating_Yes,
    RtemsTaskReqRestart_Post_Protected_No,
    RtemsTaskReqRestart_Post_RestartExtensions_Nop,
    RtemsTaskReqRestart_Post_TerminateExtensions_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqRestart_Post_Status_Ok,
    RtemsTaskReqRestart_Post_FatalError_Nop,
    RtemsTaskReqRestart_Post_Argument_Set,
    RtemsTaskReqRestart_Post_State_Ready, RtemsTaskReqRestart_Post_Enqueued_No,
    RtemsTaskReqRestart_Post_Timer_Inactive,
    RtemsTaskReqRestart_Post_Restarting_Yes,
    RtemsTaskReqRestart_Post_Terminating_Yes,
    RtemsTaskReqRestart_Post_Protected_Yes,
    RtemsTaskReqRestart_Post_RestartExtensions_Nop,
    RtemsTaskReqRestart_Post_TerminateExtensions_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqRestart_Post_Status_Ok,
    RtemsTaskReqRestart_Post_FatalError_Nop,
    RtemsTaskReqRestart_Post_Argument_Set,
    RtemsTaskReqRestart_Post_State_Blocked,
    RtemsTaskReqRestart_Post_Enqueued_No,
    RtemsTaskReqRestart_Post_Timer_Inactive,
    RtemsTaskReqRestart_Post_Restarting_Yes,
    RtemsTaskReqRestart_Post_Terminating_Yes,
    RtemsTaskReqRestart_Post_Protected_Yes,
    RtemsTaskReqRestart_Post_RestartExtensions_Nop,
    RtemsTaskReqRestart_Post_TerminateExtensions_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqRestart_Post_Status_Ok,
    RtemsTaskReqRestart_Post_FatalError_Nop,
    RtemsTaskReqRestart_Post_Argument_Set,
    RtemsTaskReqRestart_Post_State_Blocked,
    RtemsTaskReqRestart_Post_Enqueued_No,
    RtemsTaskReqRestart_Post_Timer_Active,
    RtemsTaskReqRestart_Post_Restarting_Yes,
    RtemsTaskReqRestart_Post_Terminating_Yes,
    RtemsTaskReqRestart_Post_Protected_Yes,
    RtemsTaskReqRestart_Post_RestartExtensions_Nop,
    RtemsTaskReqRestart_Post_TerminateExtensions_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqRestart_Post_Status_Ok,
    RtemsTaskReqRestart_Post_FatalError_Nop,
    RtemsTaskReqRestart_Post_Argument_Set,
    RtemsTaskReqRestart_Post_State_Blocked,
    RtemsTaskReqRestart_Post_Enqueued_Yes,
    RtemsTaskReqRestart_Post_Timer_Inactive,
    RtemsTaskReqRestart_Post_Restarting_Yes,
    RtemsTaskReqRestart_Post_Terminating_Yes,
    RtemsTaskReqRestart_Post_Protected_Yes,
    RtemsTaskReqRestart_Post_RestartExtensions_Nop,
    RtemsTaskReqRestart_Post_TerminateExtensions_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqRestart_Post_Status_Ok,
    RtemsTaskReqRestart_Post_FatalError_Nop,
    RtemsTaskReqRestart_Post_Argument_Set,
    RtemsTaskReqRestart_Post_State_Blocked,
    RtemsTaskReqRestart_Post_Enqueued_Yes,
    RtemsTaskReqRestart_Post_Timer_Active,
    RtemsTaskReqRestart_Post_Restarting_Yes,
    RtemsTaskReqRestart_Post_Terminating_Yes,
    RtemsTaskReqRestart_Post_Protected_Yes,
    RtemsTaskReqRestart_Post_RestartExtensions_Nop,
    RtemsTaskReqRestart_Post_TerminateExtensions_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqRestart_Post_Status_Ok,
    RtemsTaskReqRestart_Post_FatalError_Nop,
    RtemsTaskReqRestart_Post_Argument_Set,
    RtemsTaskReqRestart_Post_State_Ready, RtemsTaskReqRestart_Post_Enqueued_No,
    RtemsTaskReqRestart_Post_Timer_Inactive,
    RtemsTaskReqRestart_Post_Restarting_Yes,
    RtemsTaskReqRestart_Post_Terminating_No,
    RtemsTaskReqRestart_Post_Protected_Yes,
    RtemsTaskReqRestart_Post_RestartExtensions_Nop,
    RtemsTaskReqRestart_Post_TerminateExtensions_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqRestart_Post_Status_Ok,
    RtemsTaskReqRestart_Post_FatalError_Nop,
    RtemsTaskReqRestart_Post_Argument_Set,
    RtemsTaskReqRestart_Post_State_Blocked,
    RtemsTaskReqRestart_Post_Enqueued_No,
    RtemsTaskReqRestart_Post_Timer_Inactive,
    RtemsTaskReqRestart_Post_Restarting_Yes,
    RtemsTaskReqRestart_Post_Terminating_No,
    RtemsTaskReqRestart_Post_Protected_Yes,
    RtemsTaskReqRestart_Post_RestartExtensions_Nop,
    RtemsTaskReqRestart_Post_TerminateExtensions_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqRestart_Post_Status_Ok,
    RtemsTaskReqRestart_Post_FatalError_Nop,
    RtemsTaskReqRestart_Post_Argument_Set,
    RtemsTaskReqRestart_Post_State_Blocked,
    RtemsTaskReqRestart_Post_Enqueued_No,
    RtemsTaskReqRestart_Post_Timer_Active,
    RtemsTaskReqRestart_Post_Restarting_Yes,
    RtemsTaskReqRestart_Post_Terminating_No,
    RtemsTaskReqRestart_Post_Protected_Yes,
    RtemsTaskReqRestart_Post_RestartExtensions_Nop,
    RtemsTaskReqRestart_Post_TerminateExtensions_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqRestart_Post_Status_Ok,
    RtemsTaskReqRestart_Post_FatalError_Nop,
    RtemsTaskReqRestart_Post_Argument_Set,
    RtemsTaskReqRestart_Post_State_Blocked,
    RtemsTaskReqRestart_Post_Enqueued_Yes,
    RtemsTaskReqRestart_Post_Timer_Inactive,
    RtemsTaskReqRestart_Post_Restarting_Yes,
    RtemsTaskReqRestart_Post_Terminating_No,
    RtemsTaskReqRestart_Post_Protected_Yes,
    RtemsTaskReqRestart_Post_RestartExtensions_Nop,
    RtemsTaskReqRestart_Post_TerminateExtensions_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqRestart_Post_Status_Ok,
    RtemsTaskReqRestart_Post_FatalError_Nop,
    RtemsTaskReqRestart_Post_Argument_Set,
    RtemsTaskReqRestart_Post_State_Blocked,
    RtemsTaskReqRestart_Post_Enqueued_Yes,
    RtemsTaskReqRestart_Post_Timer_Active,
    RtemsTaskReqRestart_Post_Restarting_Yes,
    RtemsTaskReqRestart_Post_Terminating_No,
    RtemsTaskReqRestart_Post_Protected_Yes,
    RtemsTaskReqRestart_Post_RestartExtensions_Nop,
    RtemsTaskReqRestart_Post_TerminateExtensions_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqRestart_Post_Status_NoReturn,
    RtemsTaskReqRestart_Post_FatalError_Nop,
    RtemsTaskReqRestart_Post_Argument_Set,
    RtemsTaskReqRestart_Post_State_Zombie,
    RtemsTaskReqRestart_Post_Enqueued_No,
    RtemsTaskReqRestart_Post_Timer_Inactive,
    RtemsTaskReqRestart_Post_Restarting_Yes,
    RtemsTaskReqRestart_Post_Terminating_Yes,
    RtemsTaskReqRestart_Post_Protected_Yes,
    RtemsTaskReqRestart_Post_RestartExtensions_Nop,
    RtemsTaskReqRestart_Post_TerminateExtensions_Yes },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqRestart_Post_Status_NoReturn,
    RtemsTaskReqRestart_Post_FatalError_Nop,
    RtemsTaskReqRestart_Post_Argument_Set,
    RtemsTaskReqRestart_Post_State_Ready, RtemsTaskReqRestart_Post_Enqueued_No,
    RtemsTaskReqRestart_Post_Timer_Inactive,
    RtemsTaskReqRestart_Post_Restarting_No,
    RtemsTaskReqRestart_Post_Terminating_No,
    RtemsTaskReqRestart_Post_Protected_No,
    RtemsTaskReqRestart_Post_RestartExtensions_Yes,
    RtemsTaskReqRestart_Post_TerminateExtensions_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqRestart_Post_Status_NoReturn,
    RtemsTaskReqRestart_Post_FatalError_Yes,
    RtemsTaskReqRestart_Post_Argument_Set,
    RtemsTaskReqRestart_Post_State_Ready, RtemsTaskReqRestart_Post_Enqueued_No,
    RtemsTaskReqRestart_Post_Timer_Inactive,
    RtemsTaskReqRestart_Post_Restarting_Yes,
    RtemsTaskReqRestart_Post_Terminating_Yes,
    RtemsTaskReqRestart_Post_Protected_Yes,
    RtemsTaskReqRestart_Post_RestartExtensions_Nop,
    RtemsTaskReqRestart_Post_TerminateExtensions_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqRestart_Post_Status_NoReturn,
    RtemsTaskReqRestart_Post_FatalError_Yes,
    RtemsTaskReqRestart_Post_Argument_Set,
    RtemsTaskReqRestart_Post_State_Ready, RtemsTaskReqRestart_Post_Enqueued_No,
    RtemsTaskReqRestart_Post_Timer_Inactive,
    RtemsTaskReqRestart_Post_Restarting_Yes,
    RtemsTaskReqRestart_Post_Terminating_Yes,
    RtemsTaskReqRestart_Post_Protected_No,
    RtemsTaskReqRestart_Post_RestartExtensions_Nop,
    RtemsTaskReqRestart_Post_TerminateExtensions_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqRestart_Post_Status_NoReturn,
    RtemsTaskReqRestart_Post_FatalError_Yes,
    RtemsTaskReqRestart_Post_Argument_Set,
    RtemsTaskReqRestart_Post_State_Ready, RtemsTaskReqRestart_Post_Enqueued_No,
    RtemsTaskReqRestart_Post_Timer_Inactive,
    RtemsTaskReqRestart_Post_Restarting_Yes,
    RtemsTaskReqRestart_Post_Terminating_No,
    RtemsTaskReqRestart_Post_Protected_Yes,
    RtemsTaskReqRestart_Post_RestartExtensions_Nop,
    RtemsTaskReqRestart_Post_TerminateExtensions_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqRestart_Post_Status_NoReturn,
    RtemsTaskReqRestart_Post_FatalError_Yes,
    RtemsTaskReqRestart_Post_Argument_Set,
    RtemsTaskReqRestart_Post_State_Ready, RtemsTaskReqRestart_Post_Enqueued_No,
    RtemsTaskReqRestart_Post_Timer_Inactive,
    RtemsTaskReqRestart_Post_Restarting_Yes,
    RtemsTaskReqRestart_Post_Terminating_No,
    RtemsTaskReqRestart_Post_Protected_No,
    RtemsTaskReqRestart_Post_RestartExtensions_Nop,
    RtemsTaskReqRestart_Post_TerminateExtensions_Nop }
};

static const uint8_t
RtemsTaskReqRestart_Map[] = {
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
  1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0,
  1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0,
  1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0,
  1, 0, 1, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0,
  1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0,
  1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0,
  1, 0, 1, 0, 1, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0,
  1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0,
  1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0,
  1, 0, 1, 0, 1, 0, 1, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0,
  1, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
  1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0,
  1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0,
  1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0,
  1, 0, 1, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0,
  1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0,
  1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0,
  1, 0, 1, 0, 1, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0,
  1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0,
  1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0,
  1, 0, 1, 0, 1, 0, 1, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0,
  1, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
  1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0,
  1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0,
  1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0,
  1, 0, 1, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0,
  1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0,
  3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0,
  3, 0, 3, 0, 3, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 3, 0,
  3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 11, 11, 11, 11,
  11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
  11, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 3, 0, 3, 0,
  3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0,
  3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0,
  3, 0, 3, 0, 3, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 3, 0,
  3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0,
  3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0,
  3, 0, 3, 0, 3, 0, 3, 0, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
  11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3,
  0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3,
  0, 3, 0, 3, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 3, 0, 3,
  0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 28, 26, 28, 26, 10, 10,
  10, 10, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 16, 0, 16, 0, 10, 0,
  10, 0, 17, 0, 17, 0, 18, 0, 18, 0, 19, 0, 19, 0, 20, 0, 20, 0, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 29, 26, 29, 26, 10,
  10, 10, 10, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 12, 0, 12, 0, 10,
  0, 10, 0, 12, 0, 12, 0, 12, 0, 12, 0, 12, 0, 12, 0, 12, 0, 12, 0, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 30, 27, 30, 27,
  10, 10, 10, 10, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 21, 0, 21, 0,
  10, 0, 10, 0, 22, 0, 22, 0, 23, 0, 23, 0, 24, 0, 24, 0, 25, 0, 25, 0, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 31, 27, 31,
  27, 10, 10, 10, 10, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 13, 0,
  13, 0, 10, 0, 10, 0, 13, 0, 13, 0, 13, 0, 13, 0, 13, 0, 13, 0, 13, 0, 13, 0,
  14, 0, 14, 0, 10, 0, 10, 0, 14, 0, 14, 0, 14, 0, 14, 0, 14, 0, 14, 0, 14, 0,
  14, 0, 28, 26, 28, 26, 10, 10, 10, 10, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
  9, 9, 9, 16, 0, 16, 0, 10, 0, 10, 0, 17, 0, 17, 0, 18, 0, 18, 0, 19, 0, 19,
  0, 20, 0, 20, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 29, 26, 29, 26, 10, 10, 10, 10, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
  9, 9, 9, 9, 9, 12, 0, 12, 0, 10, 0, 10, 0, 12, 0, 12, 0, 12, 0, 12, 0, 12, 0,
  12, 0, 12, 0, 12, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 30, 27, 30, 27, 10, 10, 10, 10, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
  9, 9, 9, 9, 9, 9, 21, 0, 21, 0, 10, 0, 10, 0, 22, 0, 22, 0, 23, 0, 23, 0, 24,
  0, 24, 0, 25, 0, 25, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 31, 27, 31, 27, 10, 10, 10, 10, 9, 9, 9, 9, 9, 9, 9, 9, 9,
  9, 9, 9, 9, 9, 9, 9, 13, 0, 13, 0, 10, 0, 10, 0, 13, 0, 13, 0, 13, 0, 13, 0,
  13, 0, 13, 0, 13, 0, 13, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 28, 26, 28, 26, 10, 10, 10, 10, 9, 9, 9, 9, 9, 9, 9,
  9, 9, 9, 9, 9, 9, 9, 9, 9, 16, 0, 16, 0, 10, 0, 10, 0, 17, 0, 17, 0, 18, 0,
  18, 0, 19, 0, 19, 0, 20, 0, 20, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 29, 26, 29, 26, 10, 10, 10, 10, 9, 9, 9, 9, 9,
  9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 12, 0, 12, 0, 10, 0, 10, 0, 12, 0, 12, 0,
  12, 0, 12, 0, 12, 0, 12, 0, 12, 0, 12, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 30, 27, 30, 27, 10, 10, 10, 10, 9, 9, 9,
  9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 21, 0, 21, 0, 10, 0, 10, 0, 22, 0, 22,
  0, 23, 0, 23, 0, 24, 0, 24, 0, 25, 0, 25, 0, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 31, 27, 31, 27, 10, 10, 10, 10, 9, 9,
  9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 13, 0, 13, 0, 10, 0, 10, 0, 13, 0,
  13, 0, 13, 0, 13, 0, 13, 0, 13, 0, 13, 0, 13, 0, 14, 0, 14, 0, 10, 0, 10, 0,
  14, 0, 14, 0, 14, 0, 14, 0, 14, 0, 14, 0, 14, 0, 14, 0, 28, 26, 28, 26, 10,
  10, 10, 10, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 16, 0, 16, 0, 10,
  0, 10, 0, 17, 0, 17, 0, 18, 0, 18, 0, 19, 0, 19, 0, 20, 0, 20, 0, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 29, 26, 29, 26,
  10, 10, 10, 10, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 12, 0, 12, 0,
  10, 0, 10, 0, 12, 0, 12, 0, 12, 0, 12, 0, 12, 0, 12, 0, 12, 0, 12, 0, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 30, 27, 30,
  27, 10, 10, 10, 10, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 21, 0,
  21, 0, 10, 0, 10, 0, 22, 0, 22, 0, 23, 0, 23, 0, 24, 0, 24, 0, 25, 0, 25, 0,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 31,
  27, 31, 27, 10, 10, 10, 10, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
  13, 0, 13, 0, 10, 0, 10, 0, 13, 0, 13, 0, 13, 0, 13, 0, 13, 0, 13, 0, 13, 0,
  13, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  0, 7, 0, 7, 0, 7, 0, 7, 0, 7, 0, 7, 0, 7, 0, 7, 0, 7, 0, 7, 0, 7, 0, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 0, 7, 0, 7, 0, 7,
  0, 7, 0, 7, 0, 7, 0, 7, 0, 7, 0, 7, 0, 7, 0, 7, 0, 11, 11, 11, 11, 11, 11,
  11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 0, 7, 0,
  7, 0, 7, 0, 7, 0, 7, 0, 7, 0, 7, 0, 7, 0, 7, 0, 7, 0, 7, 0, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 0, 7, 0, 7, 0, 7, 0, 7, 0,
  7, 0, 7, 0, 7, 0, 7, 0, 7, 0, 7, 0, 7, 0, 11, 11, 11, 11, 11, 11, 11, 11, 11,
  11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 0, 7, 0, 7, 0, 7, 0,
  7, 0, 7, 0, 7, 0, 7, 0, 7, 0, 7, 0, 7, 0, 7, 0, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 0, 7, 0, 7, 0, 7, 0, 7, 0, 7, 0, 7, 0,
  7, 0, 7, 0, 7, 0, 7, 0, 7, 0, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
  11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 0, 7, 0, 7, 0, 7, 0, 7, 0, 7, 0,
  7, 0, 7, 0, 7, 0, 7, 0, 7, 0, 7, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 0, 7, 0, 7, 0, 7, 0, 7, 0, 7, 0, 7, 0, 7, 0, 7, 0,
  7, 0, 7, 0, 7, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 0, 8, 0, 8, 0, 8, 0, 8, 0, 8, 0, 8, 0, 8, 0, 8, 0, 8, 0, 8, 0, 8, 0,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 0, 8, 0,
  8, 0, 8, 0, 8, 0, 8, 0, 8, 0, 8, 0, 8, 0, 8, 0, 8, 0, 8, 0, 11, 11, 11, 11,
  11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
  11, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 0, 8, 0, 8, 0, 8, 0, 8, 0, 8, 0, 8, 0, 8, 0, 8, 0, 8, 0, 8, 0, 8, 0, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 0, 8, 0, 8, 0,
  8, 0, 8, 0, 8, 0, 8, 0, 8, 0, 8, 0, 8, 0, 8, 0, 8, 0, 11, 11, 11, 11, 11, 11,
  11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 0, 8, 0,
  8, 0, 8, 0, 8, 0, 8, 0, 8, 0, 8, 0, 8, 0, 8, 0, 8, 0, 8, 0, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 0, 8, 0, 8, 0, 8, 0, 8, 0,
  8, 0, 8, 0, 8, 0, 8, 0, 8, 0, 8, 0, 8, 0, 11, 11, 11, 11, 11, 11, 11, 11, 11,
  11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 0, 8, 0, 8, 0, 8, 0,
  8, 0, 8, 0, 8, 0, 8, 0, 8, 0, 8, 0, 8, 0, 8, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 0, 8, 0, 8, 0, 8, 0, 8, 0, 8, 0, 8, 0,
  8, 0, 8, 0, 8, 0, 8, 0, 8, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 16, 16, 16, 16, 10, 10, 10, 10, 17, 17, 17, 17, 18,
  18, 18, 18, 19, 19, 19, 19, 20, 20, 20, 20, 16, 0, 16, 0, 10, 0, 10, 0, 17,
  0, 17, 0, 18, 0, 18, 0, 19, 0, 19, 0, 20, 0, 20, 0, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 12, 12, 12, 12, 10, 10, 10,
  10, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 0,
  12, 0, 10, 0, 10, 0, 12, 0, 12, 0, 12, 0, 12, 0, 12, 0, 12, 0, 12, 0, 12, 0,
  15, 0, 15, 0, 10, 0, 10, 0, 15, 0, 15, 0, 15, 0, 15, 0, 15, 0, 15, 0, 15, 0,
  15, 0, 21, 21, 21, 21, 10, 10, 10, 10, 22, 22, 22, 22, 23, 23, 23, 23, 24,
  24, 24, 24, 25, 25, 25, 25, 21, 0, 21, 0, 10, 0, 10, 0, 22, 0, 22, 0, 23, 0,
  23, 0, 24, 0, 24, 0, 25, 0, 25, 0, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 13, 13, 13, 13, 10, 10, 10, 10, 13, 13, 13, 13,
  13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 0, 13, 0, 10, 0, 10, 0,
  13, 0, 13, 0, 13, 0, 13, 0, 13, 0, 13, 0, 13, 0, 13, 0, 14, 0, 14, 0, 10, 0,
  10, 0, 14, 0, 14, 0, 14, 0, 14, 0, 14, 0, 14, 0, 14, 0, 14, 0, 16, 16, 16,
  16, 10, 10, 10, 10, 17, 17, 17, 17, 18, 18, 18, 18, 19, 19, 19, 19, 20, 20,
  20, 20, 16, 0, 16, 0, 10, 0, 10, 0, 17, 0, 17, 0, 18, 0, 18, 0, 19, 0, 19, 0,
  20, 0, 20, 0, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 12, 12, 12, 12, 10, 10, 10, 10, 12, 12, 12, 12, 12, 12, 12, 12, 12,
  12, 12, 12, 12, 12, 12, 12, 12, 0, 12, 0, 10, 0, 10, 0, 12, 0, 12, 0, 12, 0,
  12, 0, 12, 0, 12, 0, 12, 0, 12, 0, 15, 0, 15, 0, 10, 0, 10, 0, 15, 0, 15, 0,
  15, 0, 15, 0, 15, 0, 15, 0, 15, 0, 15, 0, 21, 21, 21, 21, 10, 10, 10, 10, 22,
  22, 22, 22, 23, 23, 23, 23, 24, 24, 24, 24, 25, 25, 25, 25, 21, 0, 21, 0, 10,
  0, 10, 0, 22, 0, 22, 0, 23, 0, 23, 0, 24, 0, 24, 0, 25, 0, 25, 0, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 13, 13, 13, 13,
  10, 10, 10, 10, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
  13, 13, 0, 13, 0, 10, 0, 10, 0, 13, 0, 13, 0, 13, 0, 13, 0, 13, 0, 13, 0, 13,
  0, 13, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 16, 16, 16, 16, 10, 10, 10, 10, 17, 17, 17, 17, 18, 18, 18, 18, 19, 19,
  19, 19, 20, 20, 20, 20, 16, 0, 16, 0, 10, 0, 10, 0, 17, 0, 17, 0, 18, 0, 18,
  0, 19, 0, 19, 0, 20, 0, 20, 0, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 12, 12, 12, 12, 10, 10, 10, 10, 12, 12, 12, 12,
  12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 0, 12, 0, 10, 0, 10, 0,
  12, 0, 12, 0, 12, 0, 12, 0, 12, 0, 12, 0, 12, 0, 12, 0, 15, 0, 15, 0, 10, 0,
  10, 0, 15, 0, 15, 0, 15, 0, 15, 0, 15, 0, 15, 0, 15, 0, 15, 0, 21, 21, 21,
  21, 10, 10, 10, 10, 22, 22, 22, 22, 23, 23, 23, 23, 24, 24, 24, 24, 25, 25,
  25, 25, 21, 0, 21, 0, 10, 0, 10, 0, 22, 0, 22, 0, 23, 0, 23, 0, 24, 0, 24, 0,
  25, 0, 25, 0, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 13, 13, 13, 13, 10, 10, 10, 10, 13, 13, 13, 13, 13, 13, 13, 13, 13,
  13, 13, 13, 13, 13, 13, 13, 13, 0, 13, 0, 10, 0, 10, 0, 13, 0, 13, 0, 13, 0,
  13, 0, 13, 0, 13, 0, 13, 0, 13, 0, 14, 0, 14, 0, 10, 0, 10, 0, 14, 0, 14, 0,
  14, 0, 14, 0, 14, 0, 14, 0, 14, 0, 14, 0, 16, 16, 16, 16, 10, 10, 10, 10, 17,
  17, 17, 17, 18, 18, 18, 18, 19, 19, 19, 19, 20, 20, 20, 20, 16, 0, 16, 0, 10,
  0, 10, 0, 17, 0, 17, 0, 18, 0, 18, 0, 19, 0, 19, 0, 20, 0, 20, 0, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 12, 12, 12, 12,
  10, 10, 10, 10, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
  12, 12, 0, 12, 0, 10, 0, 10, 0, 12, 0, 12, 0, 12, 0, 12, 0, 12, 0, 12, 0, 12,
  0, 12, 0, 15, 0, 15, 0, 10, 0, 10, 0, 15, 0, 15, 0, 15, 0, 15, 0, 15, 0, 15,
  0, 15, 0, 15, 0, 21, 21, 21, 21, 10, 10, 10, 10, 22, 22, 22, 22, 23, 23, 23,
  23, 24, 24, 24, 24, 25, 25, 25, 25, 21, 0, 21, 0, 10, 0, 10, 0, 22, 0, 22, 0,
  23, 0, 23, 0, 24, 0, 24, 0, 25, 0, 25, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 13, 13, 13, 13, 10, 10, 10, 10, 13, 13,
  13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 0, 13, 0, 10, 0,
  10, 0, 13, 0, 13, 0, 13, 0, 13, 0, 13, 0, 13, 0, 13, 0, 13, 0, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6
};

static size_t RtemsTaskReqRestart_Scope( void *arg, char *buf, size_t n )
{
  RtemsTaskReqRestart_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsTaskReqRestart_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsTaskReqRestart_Fixture = {
  .setup = RtemsTaskReqRestart_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsTaskReqRestart_Teardown_Wrap,
  .scope = RtemsTaskReqRestart_Scope,
  .initial_context = &RtemsTaskReqRestart_Instance
};

static inline RtemsTaskReqRestart_Entry RtemsTaskReqRestart_PopEntry(
  RtemsTaskReqRestart_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsTaskReqRestart_Entries[
    RtemsTaskReqRestart_Map[ index ]
  ];
}

static void RtemsTaskReqRestart_SetPreConditionStates(
  RtemsTaskReqRestart_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Dormant_NA ) {
    ctx->Map.pcs[ 1 ] = RtemsTaskReqRestart_Pre_Dormant_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  if ( ctx->Map.entry.Pre_Suspended_NA ) {
    ctx->Map.pcs[ 2 ] = RtemsTaskReqRestart_Pre_Suspended_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }

  if ( ctx->Map.entry.Pre_Restarting_NA ) {
    ctx->Map.pcs[ 3 ] = RtemsTaskReqRestart_Pre_Restarting_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }

  if ( ctx->Map.entry.Pre_Terminating_NA ) {
    ctx->Map.pcs[ 4 ] = RtemsTaskReqRestart_Pre_Terminating_NA;
  } else {
    ctx->Map.pcs[ 4 ] = ctx->Map.pci[ 4 ];
  }

  if ( ctx->Map.entry.Pre_Protected_NA ) {
    ctx->Map.pcs[ 5 ] = RtemsTaskReqRestart_Pre_Protected_NA;
  } else {
    ctx->Map.pcs[ 5 ] = ctx->Map.pci[ 5 ];
  }

  ctx->Map.pcs[ 6 ] = ctx->Map.pci[ 6 ];

  if ( ctx->Map.entry.Pre_State_NA ) {
    ctx->Map.pcs[ 7 ] = RtemsTaskReqRestart_Pre_State_NA;
  } else {
    ctx->Map.pcs[ 7 ] = ctx->Map.pci[ 7 ];
  }

  if ( ctx->Map.entry.Pre_Timer_NA ) {
    ctx->Map.pcs[ 8 ] = RtemsTaskReqRestart_Pre_Timer_NA;
  } else {
    ctx->Map.pcs[ 8 ] = ctx->Map.pci[ 8 ];
  }

  if ( ctx->Map.entry.Pre_RealPriority_NA ) {
    ctx->Map.pcs[ 9 ] = RtemsTaskReqRestart_Pre_RealPriority_NA;
  } else {
    ctx->Map.pcs[ 9 ] = ctx->Map.pci[ 9 ];
  }

  ctx->Map.pcs[ 10 ] = ctx->Map.pci[ 10 ];
}

static void RtemsTaskReqRestart_TestVariant( RtemsTaskReqRestart_Context *ctx )
{
  RtemsTaskReqRestart_Pre_Id_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsTaskReqRestart_Pre_Dormant_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsTaskReqRestart_Pre_Suspended_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsTaskReqRestart_Pre_Restarting_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  RtemsTaskReqRestart_Pre_Terminating_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  RtemsTaskReqRestart_Pre_Protected_Prepare( ctx, ctx->Map.pcs[ 5 ] );
  RtemsTaskReqRestart_Pre_Context_Prepare( ctx, ctx->Map.pcs[ 6 ] );
  RtemsTaskReqRestart_Pre_State_Prepare( ctx, ctx->Map.pcs[ 7 ] );
  RtemsTaskReqRestart_Pre_Timer_Prepare( ctx, ctx->Map.pcs[ 8 ] );
  RtemsTaskReqRestart_Pre_RealPriority_Prepare( ctx, ctx->Map.pcs[ 9 ] );
  RtemsTaskReqRestart_Pre_ThreadDispatch_Prepare( ctx, ctx->Map.pcs[ 10 ] );
  RtemsTaskReqRestart_Action( ctx );
  RtemsTaskReqRestart_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  RtemsTaskReqRestart_Post_FatalError_Check(
    ctx,
    ctx->Map.entry.Post_FatalError
  );
  RtemsTaskReqRestart_Post_Argument_Check( ctx, ctx->Map.entry.Post_Argument );
  RtemsTaskReqRestart_Post_State_Check( ctx, ctx->Map.entry.Post_State );
  RtemsTaskReqRestart_Post_Enqueued_Check( ctx, ctx->Map.entry.Post_Enqueued );
  RtemsTaskReqRestart_Post_Timer_Check( ctx, ctx->Map.entry.Post_Timer );
  RtemsTaskReqRestart_Post_Restarting_Check(
    ctx,
    ctx->Map.entry.Post_Restarting
  );
  RtemsTaskReqRestart_Post_Terminating_Check(
    ctx,
    ctx->Map.entry.Post_Terminating
  );
  RtemsTaskReqRestart_Post_Protected_Check(
    ctx,
    ctx->Map.entry.Post_Protected
  );
  RtemsTaskReqRestart_Post_RestartExtensions_Check(
    ctx,
    ctx->Map.entry.Post_RestartExtensions
  );
  RtemsTaskReqRestart_Post_TerminateExtensions_Check(
    ctx,
    ctx->Map.entry.Post_TerminateExtensions
  );
}

/**
 * @fn void T_case_body_RtemsTaskReqRestart( void )
 */
T_TEST_CASE_FIXTURE( RtemsTaskReqRestart, &RtemsTaskReqRestart_Fixture )
{
  RtemsTaskReqRestart_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = RtemsTaskReqRestart_Pre_Id_Invalid;
    ctx->Map.pci[ 0 ] < RtemsTaskReqRestart_Pre_Id_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = RtemsTaskReqRestart_Pre_Dormant_Yes;
      ctx->Map.pci[ 1 ] < RtemsTaskReqRestart_Pre_Dormant_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = RtemsTaskReqRestart_Pre_Suspended_Yes;
        ctx->Map.pci[ 2 ] < RtemsTaskReqRestart_Pre_Suspended_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = RtemsTaskReqRestart_Pre_Restarting_Yes;
          ctx->Map.pci[ 3 ] < RtemsTaskReqRestart_Pre_Restarting_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          for (
            ctx->Map.pci[ 4 ] = RtemsTaskReqRestart_Pre_Terminating_Yes;
            ctx->Map.pci[ 4 ] < RtemsTaskReqRestart_Pre_Terminating_NA;
            ++ctx->Map.pci[ 4 ]
          ) {
            for (
              ctx->Map.pci[ 5 ] = RtemsTaskReqRestart_Pre_Protected_Yes;
              ctx->Map.pci[ 5 ] < RtemsTaskReqRestart_Pre_Protected_NA;
              ++ctx->Map.pci[ 5 ]
            ) {
              for (
                ctx->Map.pci[ 6 ] = RtemsTaskReqRestart_Pre_Context_Task;
                ctx->Map.pci[ 6 ] < RtemsTaskReqRestart_Pre_Context_NA;
                ++ctx->Map.pci[ 6 ]
              ) {
                for (
                  ctx->Map.pci[ 7 ] = RtemsTaskReqRestart_Pre_State_Ready;
                  ctx->Map.pci[ 7 ] < RtemsTaskReqRestart_Pre_State_NA;
                  ++ctx->Map.pci[ 7 ]
                ) {
                  for (
                    ctx->Map.pci[ 8 ] = RtemsTaskReqRestart_Pre_Timer_Inactive;
                    ctx->Map.pci[ 8 ] < RtemsTaskReqRestart_Pre_Timer_NA;
                    ++ctx->Map.pci[ 8 ]
                  ) {
                    for (
                      ctx->Map.pci[ 9 ] = RtemsTaskReqRestart_Pre_RealPriority_Initial;
                      ctx->Map.pci[ 9 ] < RtemsTaskReqRestart_Pre_RealPriority_NA;
                      ++ctx->Map.pci[ 9 ]
                    ) {
                      for (
                        ctx->Map.pci[ 10 ] = RtemsTaskReqRestart_Pre_ThreadDispatch_Disabled;
                        ctx->Map.pci[ 10 ] < RtemsTaskReqRestart_Pre_ThreadDispatch_NA;
                        ++ctx->Map.pci[ 10 ]
                      ) {
                        ctx->Map.entry = RtemsTaskReqRestart_PopEntry( ctx );

                        if ( ctx->Map.entry.Skip ) {
                          continue;
                        }

                        RtemsTaskReqRestart_SetPreConditionStates( ctx );
                        RtemsTaskReqRestart_Prepare( ctx );
                        RtemsTaskReqRestart_TestVariant( ctx );
                        RtemsTaskReqRestart_Cleanup( ctx );
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
