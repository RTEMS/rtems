/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqExit
 */

/*
 * Copyright (C) 2026 embedded brains GmbH & Co. KG
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <pthread.h>
#include <rtems.h>
#include <rtems/score/threaddispatch.h>
#include <rtems/score/threadimpl.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup CPthreadReqExit spec:/c/pthread/req/exit
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CPthreadReqExit_Pre_Value_Null,
  CPthreadReqExit_Pre_Value_Other,
  CPthreadReqExit_Pre_Value_NA
} CPthreadReqExit_Pre_Value;

typedef enum {
  CPthreadReqExit_Pre_Joinable_Yes,
  CPthreadReqExit_Pre_Joinable_No,
  CPthreadReqExit_Pre_Joinable_NA
} CPthreadReqExit_Pre_Joinable;

typedef enum {
  CPthreadReqExit_Pre_Joined_Yes,
  CPthreadReqExit_Pre_Joined_No,
  CPthreadReqExit_Pre_Joined_NA
} CPthreadReqExit_Pre_Joined;

typedef enum {
  CPthreadReqExit_Pre_CancelState_Enable,
  CPthreadReqExit_Pre_CancelState_Disable,
  CPthreadReqExit_Pre_CancelState_NA
} CPthreadReqExit_Pre_CancelState;

typedef enum {
  CPthreadReqExit_Pre_CancelType_Deferred,
  CPthreadReqExit_Pre_CancelType_Asynchronous,
  CPthreadReqExit_Pre_CancelType_NA
} CPthreadReqExit_Pre_CancelType;

typedef enum {
  CPthreadReqExit_Pre_CancelPending_Yes,
  CPthreadReqExit_Pre_CancelPending_No,
  CPthreadReqExit_Pre_CancelPending_NA
} CPthreadReqExit_Pre_CancelPending;

typedef enum {
  CPthreadReqExit_Pre_RestartPending_Yes,
  CPthreadReqExit_Pre_RestartPending_No,
  CPthreadReqExit_Pre_RestartPending_NA
} CPthreadReqExit_Pre_RestartPending;

typedef enum {
  CPthreadReqExit_Pre_ThreadDispatch_Enabled,
  CPthreadReqExit_Pre_ThreadDispatch_Disabled,
  CPthreadReqExit_Pre_ThreadDispatch_NA
} CPthreadReqExit_Pre_ThreadDispatch;

typedef enum {
  CPthreadReqExit_Post_FatalError_Yes,
  CPthreadReqExit_Post_FatalError_Nop,
  CPthreadReqExit_Post_FatalError_NA
} CPthreadReqExit_Post_FatalError;

typedef enum {
  CPthreadReqExit_Post_TerminateExtensions_Yes,
  CPthreadReqExit_Post_TerminateExtensions_Nop,
  CPthreadReqExit_Post_TerminateExtensions_NA
} CPthreadReqExit_Post_TerminateExtensions;

typedef enum {
  CPthreadReqExit_Post_Restart_Nop,
  CPthreadReqExit_Post_Restart_NA
} CPthreadReqExit_Post_Restart;

typedef enum {
  CPthreadReqExit_Post_Thread_Joinable,
  CPthreadReqExit_Post_Thread_Reclaimed,
  CPthreadReqExit_Post_Thread_Nop,
  CPthreadReqExit_Post_Thread_NA
} CPthreadReqExit_Post_Thread;

typedef enum {
  CPthreadReqExit_Post_Joiner_Joined,
  CPthreadReqExit_Post_Joiner_Nop,
  CPthreadReqExit_Post_Joiner_NA
} CPthreadReqExit_Post_Joiner;

typedef enum {
  CPthreadReqExit_Post_ExitValue_Value,
  CPthreadReqExit_Post_ExitValue_NA
} CPthreadReqExit_Post_ExitValue;

typedef struct {
  uint32_t Skip : 1;
  uint32_t Pre_Value_NA : 1;
  uint32_t Pre_Joinable_NA : 1;
  uint32_t Pre_Joined_NA : 1;
  uint32_t Pre_CancelState_NA : 1;
  uint32_t Pre_CancelType_NA : 1;
  uint32_t Pre_CancelPending_NA : 1;
  uint32_t Pre_RestartPending_NA : 1;
  uint32_t Pre_ThreadDispatch_NA : 1;
  uint32_t Post_FatalError : 2;
  uint32_t Post_TerminateExtensions : 2;
  uint32_t Post_Restart : 1;
  uint32_t Post_Thread : 2;
  uint32_t Post_Joiner : 2;
  uint32_t Post_ExitValue : 1;
} CPthreadReqExit_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/exit test case.
 */
typedef struct {
  /**
   * @brief This member contains the identifier of the worker task.
   */
  rtems_id worker_id;

  /**
   * @brief This member contains the thread identifier of the worker task.
   */
  pthread_t worker_thread;

  /**
   * @brief This member contains the identifier of the joiner task.
   */
  rtems_id joiner_id;

  /**
   * @brief This member contains the identifier of the user extension set.
   */
  rtems_id extension_id;

  /**
   * @brief This member contains the value of the `value_ptr` parameter.
   */
  void *value;

  /**
   * @brief This member is true, if the worker task shall be a joinable thread.
   */
  bool joinable;

  /**
   * @brief This member is true, if the joiner task shall wait to join the
   *   worker task.
   */
  bool joined;

  /**
   * @brief This member contains the thread cancel state attribute which the
   *   worker task sets before the call.
   */
  int cancel_state;

  /**
   * @brief This member contains the thread cancel type attribute which the
   *   worker task sets before the call.
   */
  int cancel_type;

  /**
   * @brief This member is true, if a thread cancellation request shall be
   *   pending.
   */
  bool cancel_pending;

  /**
   * @brief This member is true, if a restart request shall be pending.
   */
  bool restart_pending;

  /**
   * @brief This member is true, if the worker task shall disable thread
   *   dispatching before the call.
   */
  bool dispatch_disabled;

  /**
   * @brief This member is true, if the worker task started.
   */
  bool worker_started;

  /**
   * @brief This member is true, if the test reclaimed the worker task.
   */
  bool worker_reclaimed;

  /**
   * @brief This member contains the count of restarts of the worker task.
   */
  uint32_t restarts;

  /**
   * @brief This member contains the count of fatal extension calls.
   */
  uint32_t fatal_extension_calls;

  /**
   * @brief This member contains the count of thread terminate extension calls
   *   for the worker task.
   */
  uint32_t terminate_extension_calls;

  /**
   * @brief This member is true, if the pthread_join() call of the joiner task
   *   returned.
   */
  bool join_returned;

  /**
   * @brief This member contains the return value of the pthread_join() call of
   *   the joiner task.
   */
  int join_status;

  /**
   * @brief This member contains the value which the pthread_join() call of the
   *   joiner task stored.
   */
  void *join_value;

  /**
   * @brief This member contains the thread exit value of the worker task.
   */
  void *exit_value;

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
    CPthreadReqExit_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqExit_Context;

static CPthreadReqExit_Context CPthreadReqExit_Instance;

static const char *const CPthreadReqExit_PreDesc_Value[] =
  { "Null", "Other", "NA" };

static const char *const CPthreadReqExit_PreDesc_Joinable[] =
  { "Yes", "No", "NA" };

static const char *const CPthreadReqExit_PreDesc_Joined[] =
  { "Yes", "No", "NA" };

static const char *const CPthreadReqExit_PreDesc_CancelState[] =
  { "Enable", "Disable", "NA" };

static const char *const CPthreadReqExit_PreDesc_CancelType[] =
  { "Deferred", "Asynchronous", "NA" };

static const char *const CPthreadReqExit_PreDesc_CancelPending[] =
  { "Yes", "No", "NA" };

static const char *const CPthreadReqExit_PreDesc_RestartPending[] =
  { "Yes", "No", "NA" };

static const char *const CPthreadReqExit_PreDesc_ThreadDispatch[] =
  { "Enabled", "Disabled", "NA" };

static const char *const *const CPthreadReqExit_PreDesc[] = {
  CPthreadReqExit_PreDesc_Value,
  CPthreadReqExit_PreDesc_Joinable,
  CPthreadReqExit_PreDesc_Joined,
  CPthreadReqExit_PreDesc_CancelState,
  CPthreadReqExit_PreDesc_CancelType,
  CPthreadReqExit_PreDesc_CancelPending,
  CPthreadReqExit_PreDesc_RestartPending,
  CPthreadReqExit_PreDesc_ThreadDispatch,
  NULL
};

typedef CPthreadReqExit_Context Context;

static int exit_object;

static void Worker( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;

  if ( ctx->worker_started ) {
    ++ctx->restarts;
    SuspendSelf();
  }

  ctx->worker_started = true;
  ctx->worker_thread = pthread_self();
  DisableCancelability( NULL, NULL );
  SuspendSelf();

  if ( !ctx->joinable ) {
    T_eq_int( pthread_detach( ctx->worker_thread ), 0 );
  }

  RequestLifeChangesWithinISR(
    ctx->worker_id,
    ctx,
    ctx->restart_pending,
    ctx->cancel_pending
  );
  SetCancelability( ctx->cancel_state, ctx->cancel_type );

  if ( ctx->dispatch_disabled ) {
    _Thread_Dispatch_disable();
  }

  pthread_exit( ctx->value );
}

static void Joiner( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;
  ctx->join_status = pthread_join( ctx->worker_thread, &ctx->join_value );
  ctx->join_returned = true;
  SuspendSelf();
}

static void Fatal(
  rtems_fatal_source source,
  rtems_fatal_code   code,
  void              *arg
)
{
  Context         *ctx;
  Per_CPU_Control *cpu_self;

  ctx = arg;
  ++ctx->fatal_extension_calls;

  T_eq_int( source, INTERNAL_ERROR_CORE );
  T_eq_ulong( code, INTERNAL_ERROR_BAD_THREAD_DISPATCH_DISABLE_LEVEL );
  T_assert_eq_int( ctx->fatal_extension_calls, 1 );

  SuspendSelf();

  _ISR_Set_level( 0 );
  cpu_self = _Per_CPU_Get();
  _Thread_Dispatch_unnest( cpu_self );
  _Thread_Dispatch_direct_no_return( cpu_self );
}

static void ThreadTerminate( rtems_tcb *executing )
{
  Context *ctx;

  ctx = T_fixture_context();

  if ( executing->Object.id == ctx->worker_id ) {
    ++ctx->terminate_extension_calls;
  }
}

static const rtems_extensions_table extensions = {
  .thread_terminate = ThreadTerminate
};

static void CPthreadReqExit_Pre_Value_Prepare(
  CPthreadReqExit_Context  *ctx,
  CPthreadReqExit_Pre_Value state
)
{
  switch ( state ) {
    case CPthreadReqExit_Pre_Value_Null: {
      /*
       * While the `value_ptr` parameter is equal to NULL.
       */
      ctx->value = NULL;
      break;
    }

    case CPthreadReqExit_Pre_Value_Other: {
      /*
       * While the `value_ptr` parameter is not equal to NULL.
       */
      ctx->value = &exit_object;
      break;
    }

    case CPthreadReqExit_Pre_Value_NA:
      break;
  }
}

static void CPthreadReqExit_Pre_Joinable_Prepare(
  CPthreadReqExit_Context     *ctx,
  CPthreadReqExit_Pre_Joinable state
)
{
  switch ( state ) {
    case CPthreadReqExit_Pre_Joinable_Yes: {
      /*
       * While the executing task is a joinable thread.
       */
      ctx->joinable = true;
      break;
    }

    case CPthreadReqExit_Pre_Joinable_No: {
      /*
       * While the executing task is a detached thread.
       */
      ctx->joinable = false;
      break;
    }

    case CPthreadReqExit_Pre_Joinable_NA:
      break;
  }
}

static void CPthreadReqExit_Pre_Joined_Prepare(
  CPthreadReqExit_Context   *ctx,
  CPthreadReqExit_Pre_Joined state
)
{
  switch ( state ) {
    case CPthreadReqExit_Pre_Joined_Yes: {
      /*
       * While a thread waits to join the executing task.
       */
      ctx->joined = true;
      break;
    }

    case CPthreadReqExit_Pre_Joined_No: {
      /*
       * While no thread waits to join the executing task.
       */
      ctx->joined = false;
      break;
    }

    case CPthreadReqExit_Pre_Joined_NA:
      break;
  }
}

static void CPthreadReqExit_Pre_CancelState_Prepare(
  CPthreadReqExit_Context        *ctx,
  CPthreadReqExit_Pre_CancelState state
)
{
  switch ( state ) {
    case CPthreadReqExit_Pre_CancelState_Enable: {
      /*
       * While the thread cancel state attribute of the executing task is
       * PTHREAD_CANCEL_ENABLE.
       */
      ctx->cancel_state = PTHREAD_CANCEL_ENABLE;
      break;
    }

    case CPthreadReqExit_Pre_CancelState_Disable: {
      /*
       * While the thread cancel state attribute of the executing task is
       * PTHREAD_CANCEL_DISABLE.
       */
      ctx->cancel_state = PTHREAD_CANCEL_DISABLE;
      break;
    }

    case CPthreadReqExit_Pre_CancelState_NA:
      break;
  }
}

static void CPthreadReqExit_Pre_CancelType_Prepare(
  CPthreadReqExit_Context       *ctx,
  CPthreadReqExit_Pre_CancelType state
)
{
  switch ( state ) {
    case CPthreadReqExit_Pre_CancelType_Deferred: {
      /*
       * While the thread cancel type attribute of the executing task is
       * PTHREAD_CANCEL_DEFERRED.
       */
      ctx->cancel_type = PTHREAD_CANCEL_DEFERRED;
      break;
    }

    case CPthreadReqExit_Pre_CancelType_Asynchronous: {
      /*
       * While the thread cancel type attribute of the executing task is
       * PTHREAD_CANCEL_ASYNCHRONOUS.
       */
      ctx->cancel_type = PTHREAD_CANCEL_ASYNCHRONOUS;
      break;
    }

    case CPthreadReqExit_Pre_CancelType_NA:
      break;
  }
}

static void CPthreadReqExit_Pre_CancelPending_Prepare(
  CPthreadReqExit_Context          *ctx,
  CPthreadReqExit_Pre_CancelPending state
)
{
  switch ( state ) {
    case CPthreadReqExit_Pre_CancelPending_Yes: {
      /*
       * While a thread cancellation request is pending for the executing task.
       */
      ctx->cancel_pending = true;
      break;
    }

    case CPthreadReqExit_Pre_CancelPending_No: {
      /*
       * While no thread cancellation request is pending for the executing
       * task.
       */
      ctx->cancel_pending = false;
      break;
    }

    case CPthreadReqExit_Pre_CancelPending_NA:
      break;
  }
}

static void CPthreadReqExit_Pre_RestartPending_Prepare(
  CPthreadReqExit_Context           *ctx,
  CPthreadReqExit_Pre_RestartPending state
)
{
  switch ( state ) {
    case CPthreadReqExit_Pre_RestartPending_Yes: {
      /*
       * While a restart of the executing task by rtems_task_restart() is
       * pending.
       */
      ctx->restart_pending = true;
      break;
    }

    case CPthreadReqExit_Pre_RestartPending_No: {
      /*
       * While no restart of the executing task is pending.
       */
      ctx->restart_pending = false;
      break;
    }

    case CPthreadReqExit_Pre_RestartPending_NA:
      break;
  }
}

static void CPthreadReqExit_Pre_ThreadDispatch_Prepare(
  CPthreadReqExit_Context           *ctx,
  CPthreadReqExit_Pre_ThreadDispatch state
)
{
  switch ( state ) {
    case CPthreadReqExit_Pre_ThreadDispatch_Enabled: {
      /*
       * While thread dispatching is enabled for the executing task.
       */
      ctx->dispatch_disabled = false;
      break;
    }

    case CPthreadReqExit_Pre_ThreadDispatch_Disabled: {
      /*
       * While thread dispatching is disabled for the executing task.
       */
      ctx->dispatch_disabled = true;
      break;
    }

    case CPthreadReqExit_Pre_ThreadDispatch_NA:
      break;
  }
}

static void CPthreadReqExit_Post_FatalError_Check(
  CPthreadReqExit_Context        *ctx,
  CPthreadReqExit_Post_FatalError state
)
{
  switch ( state ) {
    case CPthreadReqExit_Post_FatalError_Yes: {
      /*
       * The fatal error with a fatal source of INTERNAL_ERROR_CORE and a fatal
       * code of INTERNAL_ERROR_BAD_THREAD_DISPATCH_DISABLE_LEVEL shall occur
       * by the pthread_exit() call.
       */
      T_eq_u32( ctx->fatal_extension_calls, 1 );
      break;
    }

    case CPthreadReqExit_Post_FatalError_Nop: {
      /*
       * The pthread_exit() call shall not cause a fatal error.
       */
      T_eq_u32( ctx->fatal_extension_calls, 0 );
      break;
    }

    case CPthreadReqExit_Post_FatalError_NA:
      break;
  }
}

static void CPthreadReqExit_Post_TerminateExtensions_Check(
  CPthreadReqExit_Context                 *ctx,
  CPthreadReqExit_Post_TerminateExtensions state
)
{
  switch ( state ) {
    case CPthreadReqExit_Post_TerminateExtensions_Yes: {
      /*
       * The thread terminate user extensions shall be invoked by the
       * pthread_exit() call.
       */
      T_eq_u32( ctx->terminate_extension_calls, 1 );
      break;
    }

    case CPthreadReqExit_Post_TerminateExtensions_Nop: {
      /*
       * The thread terminate user extensions shall not be invoked by the
       * pthread_exit() call.
       */
      T_eq_u32( ctx->terminate_extension_calls, 0 );
      break;
    }

    case CPthreadReqExit_Post_TerminateExtensions_NA:
      break;
  }
}

static void CPthreadReqExit_Post_Restart_Check(
  CPthreadReqExit_Context     *ctx,
  CPthreadReqExit_Post_Restart state
)
{
  switch ( state ) {
    case CPthreadReqExit_Post_Restart_Nop: {
      /*
       * The executing task shall not be restarted by the pthread_exit() call.
       */
      T_eq_u32( ctx->restarts, 0 );
      break;
    }

    case CPthreadReqExit_Post_Restart_NA:
      break;
  }
}

static void CPthreadReqExit_Post_Thread_Check(
  CPthreadReqExit_Context    *ctx,
  CPthreadReqExit_Post_Thread state
)
{
  switch ( state ) {
    case CPthreadReqExit_Post_Thread_Joinable: {
      /*
       * The executing task shall be a terminated joinable thread.
       */
      T_eq_int( pthread_join( ctx->worker_thread, &ctx->exit_value ), 0 );
      ctx->worker_reclaimed = true;
      break;
    }

    case CPthreadReqExit_Post_Thread_Reclaimed: {
      /*
       * The thread identifier of the executing task shall be associated with
       * no thread.
       */
      T_eq_int( pthread_join( ctx->worker_thread, NULL ), ESRCH );
      ctx->worker_reclaimed = true;
      break;
    }

    case CPthreadReqExit_Post_Thread_Nop: {
      /*
       * The executing task shall not be terminated by the pthread_exit() call.
       */
      T_true( IsTaskSuspended( ctx->worker_id ) );
      break;
    }

    case CPthreadReqExit_Post_Thread_NA:
      break;
  }
}

static void CPthreadReqExit_Post_Joiner_Check(
  CPthreadReqExit_Context    *ctx,
  CPthreadReqExit_Post_Joiner state
)
{
  switch ( state ) {
    case CPthreadReqExit_Post_Joiner_Joined: {
      /*
       * The thread which waits to join the executing task shall be joined with
       * that task.
       */
      T_true( ctx->join_returned );
      T_eq_int( ctx->join_status, 0 );
      ctx->exit_value = ctx->join_value;
      break;
    }

    case CPthreadReqExit_Post_Joiner_Nop: {
      /*
       * The thread which waits to join the executing task shall not be joined
       * with that task.
       */
      T_false( ctx->join_returned );
      break;
    }

    case CPthreadReqExit_Post_Joiner_NA:
      break;
  }
}

static void CPthreadReqExit_Post_ExitValue_Check(
  CPthreadReqExit_Context       *ctx,
  CPthreadReqExit_Post_ExitValue state
)
{
  switch ( state ) {
    case CPthreadReqExit_Post_ExitValue_Value: {
      /*
       * The thread exit value of the executing task shall be the value of the
       * `value_ptr` parameter.
       */
      T_eq_ptr( ctx->exit_value, ctx->value );
      break;
    }

    case CPthreadReqExit_Post_ExitValue_NA:
      break;
  }
}

static void CPthreadReqExit_Setup( CPthreadReqExit_Context *ctx )
{
  rtems_status_code sc;

  sc = rtems_extension_create(
    rtems_build_name( 'T', 'E', 'S', 'T' ),
    &extensions,
    &ctx->extension_id
  );
  T_rsc_success( sc );

  SetFatalHandler( Fatal, ctx );
  SetSelfPriority( PRIO_NORMAL );
}

static void CPthreadReqExit_Setup_Wrap( void *arg )
{
  CPthreadReqExit_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CPthreadReqExit_Setup( ctx );
}

static void CPthreadReqExit_Teardown( CPthreadReqExit_Context *ctx )
{
  rtems_status_code sc;

  sc = rtems_extension_delete( ctx->extension_id );
  T_rsc_success( sc );

  SetFatalHandler( NULL, NULL );
  RestoreRunnerPriority();
}

static void CPthreadReqExit_Teardown_Wrap( void *arg )
{
  CPthreadReqExit_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CPthreadReqExit_Teardown( ctx );
}

static void CPthreadReqExit_Prepare( CPthreadReqExit_Context *ctx )
{
  ctx->worker_started = false;
  ctx->worker_reclaimed = false;
  ctx->restarts = 0;
  ctx->fatal_extension_calls = 0;
  ctx->terminate_extension_calls = 0;
  ctx->join_returned = false;
  ctx->join_status = -1;
  ctx->join_value = NULL;
  ctx->exit_value = NULL;
}

static void CPthreadReqExit_Action( CPthreadReqExit_Context *ctx )
{
  ctx->worker_id = CreateTask( "WORK", PRIO_HIGH );
  StartTask( ctx->worker_id, Worker, ctx );

  if ( ctx->joined ) {
    ctx->joiner_id = CreateTask( "JOIN", PRIO_HIGH );
    StartTask( ctx->joiner_id, Joiner, ctx );
  }

  ResumeTask( ctx->worker_id );
}

static void CPthreadReqExit_Cleanup( CPthreadReqExit_Context *ctx )
{
  if ( ctx->joined ) {
    DeleteTask( ctx->joiner_id );
  }

  if ( !ctx->worker_reclaimed ) {
    DeleteTask( ctx->worker_id );
  }
}

/* clang-format off */

static const CPthreadReqExit_Entry
CPthreadReqExit_Entries[] = {
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqExit_Post_FatalError_Nop,
    CPthreadReqExit_Post_TerminateExtensions_Yes,
    CPthreadReqExit_Post_Restart_Nop, CPthreadReqExit_Post_Thread_Reclaimed,
    CPthreadReqExit_Post_Joiner_Joined, CPthreadReqExit_Post_ExitValue_Value },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqExit_Post_FatalError_Yes,
    CPthreadReqExit_Post_TerminateExtensions_Nop,
    CPthreadReqExit_Post_Restart_Nop, CPthreadReqExit_Post_Thread_Nop,
    CPthreadReqExit_Post_Joiner_Nop, CPthreadReqExit_Post_ExitValue_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqExit_Post_FatalError_Yes,
    CPthreadReqExit_Post_TerminateExtensions_Nop,
    CPthreadReqExit_Post_Restart_Nop, CPthreadReqExit_Post_Thread_Nop,
    CPthreadReqExit_Post_Joiner_NA, CPthreadReqExit_Post_ExitValue_NA },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqExit_Post_FatalError_NA,
    CPthreadReqExit_Post_TerminateExtensions_NA,
    CPthreadReqExit_Post_Restart_NA, CPthreadReqExit_Post_Thread_NA,
    CPthreadReqExit_Post_Joiner_NA, CPthreadReqExit_Post_ExitValue_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqExit_Post_FatalError_Nop,
    CPthreadReqExit_Post_TerminateExtensions_Yes,
    CPthreadReqExit_Post_Restart_Nop, CPthreadReqExit_Post_Thread_Joinable,
    CPthreadReqExit_Post_Joiner_NA, CPthreadReqExit_Post_ExitValue_Value },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqExit_Post_FatalError_Nop,
    CPthreadReqExit_Post_TerminateExtensions_Yes,
    CPthreadReqExit_Post_Restart_Nop, CPthreadReqExit_Post_Thread_Reclaimed,
    CPthreadReqExit_Post_Joiner_NA, CPthreadReqExit_Post_ExitValue_NA }
};

static const uint8_t
CPthreadReqExit_Map[] = {
  0, 1, 0, 1, 0, 1, 0, 1, 3, 3, 3, 3, 3, 3, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
  0, 1, 0, 1, 0, 1, 4, 2, 4, 2, 4, 2, 4, 2, 3, 3, 3, 3, 3, 3, 4, 2, 4, 2, 4, 2,
  4, 2, 4, 2, 4, 2, 4, 2, 4, 2, 4, 2, 0, 1, 0, 1, 0, 1, 0, 1, 3, 3, 3, 3, 3, 3,
  0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 5, 2, 5, 2, 5, 2, 5, 2,
  3, 3, 3, 3, 3, 3, 5, 2, 5, 2, 5, 2, 5, 2, 5, 2, 5, 2, 5, 2, 5, 2, 5, 2, 0, 1,
  0, 1, 0, 1, 0, 1, 3, 3, 3, 3, 3, 3, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
  0, 1, 0, 1, 4, 2, 4, 2, 4, 2, 4, 2, 3, 3, 3, 3, 3, 3, 4, 2, 4, 2, 4, 2, 4, 2,
  4, 2, 4, 2, 4, 2, 4, 2, 4, 2, 0, 1, 0, 1, 0, 1, 0, 1, 3, 3, 3, 3, 3, 3, 0, 1,
  0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 5, 2, 5, 2, 5, 2, 5, 2, 3, 3,
  3, 3, 3, 3, 5, 2, 5, 2, 5, 2, 5, 2, 5, 2, 5, 2, 5, 2, 5, 2, 5, 2
};

/* clang-format on */

static size_t CPthreadReqExit_Scope( void *arg, char *buf, size_t n )
{
  CPthreadReqExit_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CPthreadReqExit_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CPthreadReqExit_Fixture = {
  .setup = CPthreadReqExit_Setup_Wrap,
  .stop = NULL,
  .teardown = CPthreadReqExit_Teardown_Wrap,
  .scope = CPthreadReqExit_Scope,
  .initial_context = &CPthreadReqExit_Instance
};

static inline CPthreadReqExit_Entry CPthreadReqExit_PopEntry(
  CPthreadReqExit_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqExit_Entries[ CPthreadReqExit_Map[ index ] ];
}

static void CPthreadReqExit_TestVariant( CPthreadReqExit_Context *ctx )
{
  CPthreadReqExit_Pre_Value_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqExit_Pre_Joinable_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqExit_Pre_Joined_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CPthreadReqExit_Pre_CancelState_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CPthreadReqExit_Pre_CancelType_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  CPthreadReqExit_Pre_CancelPending_Prepare( ctx, ctx->Map.pcs[ 5 ] );
  CPthreadReqExit_Pre_RestartPending_Prepare( ctx, ctx->Map.pcs[ 6 ] );
  CPthreadReqExit_Pre_ThreadDispatch_Prepare( ctx, ctx->Map.pcs[ 7 ] );
  CPthreadReqExit_Action( ctx );
  CPthreadReqExit_Post_FatalError_Check( ctx, ctx->Map.entry.Post_FatalError );
  CPthreadReqExit_Post_TerminateExtensions_Check(
    ctx,
    ctx->Map.entry.Post_TerminateExtensions
  );
  CPthreadReqExit_Post_Restart_Check( ctx, ctx->Map.entry.Post_Restart );
  CPthreadReqExit_Post_Thread_Check( ctx, ctx->Map.entry.Post_Thread );
  CPthreadReqExit_Post_Joiner_Check( ctx, ctx->Map.entry.Post_Joiner );
  CPthreadReqExit_Post_ExitValue_Check( ctx, ctx->Map.entry.Post_ExitValue );
}

/**
 * @fn void T_case_body_CPthreadReqExit( void )
 */
T_TEST_CASE_FIXTURE( CPthreadReqExit, &CPthreadReqExit_Fixture )
{
  CPthreadReqExit_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CPthreadReqExit_Pre_Value_Null;
    ctx->Map.pcs[ 0 ] < CPthreadReqExit_Pre_Value_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CPthreadReqExit_Pre_Joinable_Yes;
      ctx->Map.pcs[ 1 ] < CPthreadReqExit_Pre_Joinable_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = CPthreadReqExit_Pre_Joined_Yes;
        ctx->Map.pcs[ 2 ] < CPthreadReqExit_Pre_Joined_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        for (
          ctx->Map.pcs[ 3 ] = CPthreadReqExit_Pre_CancelState_Enable;
          ctx->Map.pcs[ 3 ] < CPthreadReqExit_Pre_CancelState_NA;
          ++ctx->Map.pcs[ 3 ]
        ) {
          for (
            ctx->Map.pcs[ 4 ] = CPthreadReqExit_Pre_CancelType_Deferred;
            ctx->Map.pcs[ 4 ] < CPthreadReqExit_Pre_CancelType_NA;
            ++ctx->Map.pcs[ 4 ]
          ) {
            for (
              ctx->Map.pcs[ 5 ] = CPthreadReqExit_Pre_CancelPending_Yes;
              ctx->Map.pcs[ 5 ] < CPthreadReqExit_Pre_CancelPending_NA;
              ++ctx->Map.pcs[ 5 ]
            ) {
              for (
                ctx->Map.pcs[ 6 ] = CPthreadReqExit_Pre_RestartPending_Yes;
                ctx->Map.pcs[ 6 ] < CPthreadReqExit_Pre_RestartPending_NA;
                ++ctx->Map.pcs[ 6 ]
              ) {
                for (
                  ctx->Map.pcs[ 7 ] =
                    CPthreadReqExit_Pre_ThreadDispatch_Enabled;
                  ctx->Map.pcs[ 7 ] < CPthreadReqExit_Pre_ThreadDispatch_NA;
                  ++ctx->Map.pcs[ 7 ]
                ) {
                  ctx->Map.entry = CPthreadReqExit_PopEntry( ctx );

                  if ( ctx->Map.entry.Skip ) {
                    continue;
                  }

                  CPthreadReqExit_Prepare( ctx );
                  CPthreadReqExit_TestVariant( ctx );
                  CPthreadReqExit_Cleanup( ctx );
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
