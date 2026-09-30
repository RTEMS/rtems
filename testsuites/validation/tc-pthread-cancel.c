/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqCancel
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

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup CPthreadReqCancel spec:/c/pthread/req/cancel
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CPthreadReqCancel_Pre_Thread_Invalid,
  CPthreadReqCancel_Pre_Thread_Executing,
  CPthreadReqCancel_Pre_Thread_Other,
  CPthreadReqCancel_Pre_Thread_NA
} CPthreadReqCancel_Pre_Thread;

typedef enum {
  CPthreadReqCancel_Pre_Terminated_Yes,
  CPthreadReqCancel_Pre_Terminated_No,
  CPthreadReqCancel_Pre_Terminated_NA
} CPthreadReqCancel_Pre_Terminated;

typedef enum {
  CPthreadReqCancel_Pre_CancelState_Enable,
  CPthreadReqCancel_Pre_CancelState_Disable,
  CPthreadReqCancel_Pre_CancelState_NA
} CPthreadReqCancel_Pre_CancelState;

typedef enum {
  CPthreadReqCancel_Pre_CancelType_Deferred,
  CPthreadReqCancel_Pre_CancelType_Asynchronous,
  CPthreadReqCancel_Pre_CancelType_NA
} CPthreadReqCancel_Pre_CancelType;

typedef enum {
  CPthreadReqCancel_Pre_CancelPending_Yes,
  CPthreadReqCancel_Pre_CancelPending_No,
  CPthreadReqCancel_Pre_CancelPending_NA
} CPthreadReqCancel_Pre_CancelPending;

typedef enum {
  CPthreadReqCancel_Pre_RestartPending_Yes,
  CPthreadReqCancel_Pre_RestartPending_No,
  CPthreadReqCancel_Pre_RestartPending_NA
} CPthreadReqCancel_Pre_RestartPending;

typedef enum {
  CPthreadReqCancel_Pre_Blocked_Yes,
  CPthreadReqCancel_Pre_Blocked_No,
  CPthreadReqCancel_Pre_Blocked_NA
} CPthreadReqCancel_Pre_Blocked;

typedef enum {
  CPthreadReqCancel_Post_Status_Ok,
  CPthreadReqCancel_Post_Status_Srch,
  CPthreadReqCancel_Post_Status_NA
} CPthreadReqCancel_Post_Status;

typedef enum {
  CPthreadReqCancel_Post_Return_Yes,
  CPthreadReqCancel_Post_Return_No,
  CPthreadReqCancel_Post_Return_NA
} CPthreadReqCancel_Post_Return;

typedef enum {
  CPthreadReqCancel_Post_Wait_Nop,
  CPthreadReqCancel_Post_Wait_NA
} CPthreadReqCancel_Post_Wait;

typedef enum {
  CPthreadReqCancel_Post_Life_Terminate,
  CPthreadReqCancel_Post_Life_Nop,
  CPthreadReqCancel_Post_Life_NA
} CPthreadReqCancel_Post_Life;

typedef enum {
  CPthreadReqCancel_Post_CancelState_Nop,
  CPthreadReqCancel_Post_CancelState_NA
} CPthreadReqCancel_Post_CancelState;

typedef enum {
  CPthreadReqCancel_Post_CancelType_Nop,
  CPthreadReqCancel_Post_CancelType_NA
} CPthreadReqCancel_Post_CancelType;

typedef enum {
  CPthreadReqCancel_Post_ExitValue_Canceled,
  CPthreadReqCancel_Post_ExitValue_Exited,
  CPthreadReqCancel_Post_ExitValue_NA
} CPthreadReqCancel_Post_ExitValue;

typedef enum {
  CPthreadReqCancel_Post_Pending_Yes,
  CPthreadReqCancel_Post_Pending_NA
} CPthreadReqCancel_Post_Pending;

typedef struct {
  uint32_t Skip : 1;
  uint32_t Pre_Thread_NA : 1;
  uint32_t Pre_Terminated_NA : 1;
  uint32_t Pre_CancelState_NA : 1;
  uint32_t Pre_CancelType_NA : 1;
  uint32_t Pre_CancelPending_NA : 1;
  uint32_t Pre_RestartPending_NA : 1;
  uint32_t Pre_Blocked_NA : 1;
  uint32_t Post_Status : 2;
  uint32_t Post_Return : 2;
  uint32_t Post_Wait : 1;
  uint32_t Post_Life : 2;
  uint32_t Post_CancelState : 1;
  uint32_t Post_CancelType : 1;
  uint32_t Post_ExitValue : 2;
  uint32_t Post_Pending : 1;
} CPthreadReqCancel_Entry;

typedef enum { THREAD_INVALID, THREAD_EXECUTING, THREAD_OTHER } ThreadKind;

typedef enum { PHASE_ACTION, PHASE_LATER, PHASE_CLEANUP } Phase;

/**
 * @brief Test context for spec:/c/pthread/req/cancel test case.
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
   * @brief This member contains the phase of the variant.
   */
  Phase phase;

  /**
   * @brief This member specifies the thread which the call cancels.
   */
  ThreadKind thread_kind;

  /**
   * @brief This member is true, if the worker task terminates before the call.
   */
  bool terminated;

  /**
   * @brief This member contains the thread cancel state attribute which the
   *   worker task sets.
   */
  int cancel_state;

  /**
   * @brief This member contains the thread cancel type attribute which the
   *   worker task sets.
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
   * @brief This member is true, if the worker task is blocked.
   */
  bool blocked;

  /**
   * @brief This member contains the thread cancel state attribute of the
   *   worker task after the call.
   */
  int state_after;

  /**
   * @brief This member contains the thread cancel type attribute of the worker
   *   task after the call.
   */
  int type_after;

  /**
   * @brief This member is true, if the worker task started.
   */
  bool worker_started;

  /**
   * @brief This member is true, if the worker task continued after the call.
   */
  bool worker_continued;

  /**
   * @brief This member is true, if the wait of the worker task returned.
   */
  bool wait_returned;

  /**
   * @brief This member is true, if the wait of the worker task returned during
   *   the call.
   */
  bool wait_returned_early;

  /**
   * @brief This member is true, if the enabled asynchronous cancellation of
   *   the later phase returned to the worker task.
   */
  bool late_returned;

  /**
   * @brief This member contains the count of restarts of the worker task.
   */
  uint32_t restarts;

  /**
   * @brief This member is true, if the test joined the worker task.
   */
  bool joined;

  /**
   * @brief This member contains the thread exit value of the worker task.
   */
  void *exit_value;

  /**
   * @brief This member is true, if the call returned to the caller.
   */
  bool returned;

  /**
   * @brief This member contains the return value of the pthread_cancel() call.
   */
  int status;

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
    CPthreadReqCancel_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqCancel_Context;

static CPthreadReqCancel_Context CPthreadReqCancel_Instance;

static const char *const CPthreadReqCancel_PreDesc_Thread[] =
  { "Invalid", "Executing", "Other", "NA" };

static const char *const CPthreadReqCancel_PreDesc_Terminated[] =
  { "Yes", "No", "NA" };

static const char *const CPthreadReqCancel_PreDesc_CancelState[] =
  { "Enable", "Disable", "NA" };

static const char *const CPthreadReqCancel_PreDesc_CancelType[] =
  { "Deferred", "Asynchronous", "NA" };

static const char *const CPthreadReqCancel_PreDesc_CancelPending[] =
  { "Yes", "No", "NA" };

static const char *const CPthreadReqCancel_PreDesc_RestartPending[] =
  { "Yes", "No", "NA" };

static const char *const CPthreadReqCancel_PreDesc_Blocked[] =
  { "Yes", "No", "NA" };

static const char *const *const CPthreadReqCancel_PreDesc[] = {
  CPthreadReqCancel_PreDesc_Thread,
  CPthreadReqCancel_PreDesc_Terminated,
  CPthreadReqCancel_PreDesc_CancelState,
  CPthreadReqCancel_PreDesc_CancelType,
  CPthreadReqCancel_PreDesc_CancelPending,
  CPthreadReqCancel_PreDesc_RestartPending,
  CPthreadReqCancel_PreDesc_Blocked,
  NULL
};

typedef CPthreadReqCancel_Context Context;

static int exit_object;

static void KickWorker( Context *ctx )
{
  SendEvents( ctx->worker_id, RTEMS_EVENT_0 );
  (void) SetPriority( ctx->worker_id, PRIO_HIGH );
}

static void JoinWorker( Context *ctx )
{
  ctx->phase = PHASE_CLEANUP;
  KickWorker( ctx );
  T_false( IsTaskSuspended( ctx->worker_id ) );
  (void) rtems_task_resume( ctx->worker_id );
  T_eq_int( pthread_join( ctx->worker_thread, &ctx->exit_value ), 0 );
  ctx->joined = true;
}

static void Worker( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;

  if ( ctx->worker_started ) {
    ++ctx->restarts;
    pthread_exit( NULL );
  }

  ctx->worker_started = true;
  ctx->worker_thread = pthread_self();
  DisableCancelability( NULL, NULL );
  RequestLifeChangesWithinISR(
    ctx->worker_id,
    ctx,
    ctx->restart_pending,
    ctx->cancel_pending
  );
  SetCancelability( ctx->cancel_state, ctx->cancel_type );

  if ( ctx->thread_kind == THREAD_EXECUTING ) {
    ctx->status = pthread_cancel( pthread_self() );
    ctx->returned = true;
  } else if ( ctx->terminated ) {
    pthread_exit( &exit_object );
  } else if ( ctx->blocked ) {
    (void) ReceiveAnyEvents();
    ctx->wait_returned = true;
  } else {
    (void) SetSelfPriority( PRIO_LOW );
  }

  ctx->worker_continued = true;
  DisableCancelability( &ctx->state_after, &ctx->type_after );
  SuspendSelf();
  SetCancelability( PTHREAD_CANCEL_ENABLE, PTHREAD_CANCEL_ASYNCHRONOUS );

  if ( ctx->phase == PHASE_LATER ) {
    ctx->late_returned = true;
  }

  pthread_exit( &exit_object );
}

static void CPthreadReqCancel_Pre_Thread_Prepare(
  CPthreadReqCancel_Context   *ctx,
  CPthreadReqCancel_Pre_Thread state
)
{
  switch ( state ) {
    case CPthreadReqCancel_Pre_Thread_Invalid: {
      /*
       * While the `thread` parameter is not associated with a thread.
       */
      ctx->thread_kind = THREAD_INVALID;
      break;
    }

    case CPthreadReqCancel_Pre_Thread_Executing: {
      /*
       * While the `thread` parameter is associated with the executing task.
       */
      ctx->thread_kind = THREAD_EXECUTING;
      break;
    }

    case CPthreadReqCancel_Pre_Thread_Other: {
      /*
       * While the `thread` parameter is associated with a thread other than
       * the executing task.
       */
      ctx->thread_kind = THREAD_OTHER;
      break;
    }

    case CPthreadReqCancel_Pre_Thread_NA:
      break;
  }
}

static void CPthreadReqCancel_Pre_Terminated_Prepare(
  CPthreadReqCancel_Context       *ctx,
  CPthreadReqCancel_Pre_Terminated state
)
{
  switch ( state ) {
    case CPthreadReqCancel_Pre_Terminated_Yes: {
      /*
       * While the thread specified by the `thread` parameter is terminated.
       */
      ctx->terminated = true;
      break;
    }

    case CPthreadReqCancel_Pre_Terminated_No: {
      /*
       * While the thread specified by the `thread` parameter is not
       * terminated.
       */
      ctx->terminated = false;
      break;
    }

    case CPthreadReqCancel_Pre_Terminated_NA:
      break;
  }
}

static void CPthreadReqCancel_Pre_CancelState_Prepare(
  CPthreadReqCancel_Context        *ctx,
  CPthreadReqCancel_Pre_CancelState state
)
{
  switch ( state ) {
    case CPthreadReqCancel_Pre_CancelState_Enable: {
      /*
       * While the thread cancel state attribute of the thread specified by the
       * `thread` parameter is PTHREAD_CANCEL_ENABLE.
       */
      ctx->cancel_state = PTHREAD_CANCEL_ENABLE;
      break;
    }

    case CPthreadReqCancel_Pre_CancelState_Disable: {
      /*
       * While the thread cancel state attribute of the thread specified by the
       * `thread` parameter is PTHREAD_CANCEL_DISABLE.
       */
      ctx->cancel_state = PTHREAD_CANCEL_DISABLE;
      break;
    }

    case CPthreadReqCancel_Pre_CancelState_NA:
      break;
  }
}

static void CPthreadReqCancel_Pre_CancelType_Prepare(
  CPthreadReqCancel_Context       *ctx,
  CPthreadReqCancel_Pre_CancelType state
)
{
  switch ( state ) {
    case CPthreadReqCancel_Pre_CancelType_Deferred: {
      /*
       * While the thread cancel type attribute of the thread specified by the
       * `thread` parameter is PTHREAD_CANCEL_DEFERRED.
       */
      ctx->cancel_type = PTHREAD_CANCEL_DEFERRED;
      break;
    }

    case CPthreadReqCancel_Pre_CancelType_Asynchronous: {
      /*
       * While the thread cancel type attribute of the thread specified by the
       * `thread` parameter is PTHREAD_CANCEL_ASYNCHRONOUS.
       */
      ctx->cancel_type = PTHREAD_CANCEL_ASYNCHRONOUS;
      break;
    }

    case CPthreadReqCancel_Pre_CancelType_NA:
      break;
  }
}

static void CPthreadReqCancel_Pre_CancelPending_Prepare(
  CPthreadReqCancel_Context          *ctx,
  CPthreadReqCancel_Pre_CancelPending state
)
{
  switch ( state ) {
    case CPthreadReqCancel_Pre_CancelPending_Yes: {
      /*
       * While a thread cancellation request is pending for the thread
       * specified by the `thread` parameter.
       */
      ctx->cancel_pending = true;
      break;
    }

    case CPthreadReqCancel_Pre_CancelPending_No: {
      /*
       * While no thread cancellation request is pending for the thread
       * specified by the `thread` parameter.
       */
      ctx->cancel_pending = false;
      break;
    }

    case CPthreadReqCancel_Pre_CancelPending_NA:
      break;
  }
}

static void CPthreadReqCancel_Pre_RestartPending_Prepare(
  CPthreadReqCancel_Context           *ctx,
  CPthreadReqCancel_Pre_RestartPending state
)
{
  switch ( state ) {
    case CPthreadReqCancel_Pre_RestartPending_Yes: {
      /*
       * While a restart of the thread specified by the `thread` parameter by
       * rtems_task_restart() is pending.
       */
      ctx->restart_pending = true;
      break;
    }

    case CPthreadReqCancel_Pre_RestartPending_No: {
      /*
       * While no restart of the thread specified by the `thread` parameter is
       * pending.
       */
      ctx->restart_pending = false;
      break;
    }

    case CPthreadReqCancel_Pre_RestartPending_NA:
      break;
  }
}

static void CPthreadReqCancel_Pre_Blocked_Prepare(
  CPthreadReqCancel_Context    *ctx,
  CPthreadReqCancel_Pre_Blocked state
)
{
  switch ( state ) {
    case CPthreadReqCancel_Pre_Blocked_Yes: {
      /*
       * While the thread specified by the `thread` parameter is a blocked
       * task.
       */
      ctx->blocked = true;
      break;
    }

    case CPthreadReqCancel_Pre_Blocked_No: {
      /*
       * While the thread specified by the `thread` parameter is a ready task.
       */
      ctx->blocked = false;
      break;
    }

    case CPthreadReqCancel_Pre_Blocked_NA:
      break;
  }
}

static void CPthreadReqCancel_Post_Status_Check(
  CPthreadReqCancel_Context    *ctx,
  CPthreadReqCancel_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqCancel_Post_Status_Ok: {
      /*
       * The return value of pthread_cancel() shall be zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CPthreadReqCancel_Post_Status_Srch: {
      /*
       * The return value of pthread_cancel() shall be ESRCH.
       */
      T_eq_int( ctx->status, ESRCH );
      break;
    }

    case CPthreadReqCancel_Post_Status_NA:
      break;
  }
}

static void CPthreadReqCancel_Post_Return_Check(
  CPthreadReqCancel_Context    *ctx,
  CPthreadReqCancel_Post_Return state
)
{
  switch ( state ) {
    case CPthreadReqCancel_Post_Return_Yes: {
      /*
       * The pthread_cancel() call shall return to the caller.
       */
      T_true( ctx->returned );
      break;
    }

    case CPthreadReqCancel_Post_Return_No: {
      /*
       * The pthread_cancel() call shall not return to the caller.
       */
      T_false( ctx->returned );
      break;
    }

    case CPthreadReqCancel_Post_Return_NA:
      break;
  }
}

static void CPthreadReqCancel_Post_Wait_Check(
  CPthreadReqCancel_Context  *ctx,
  CPthreadReqCancel_Post_Wait state
)
{
  switch ( state ) {
    case CPthreadReqCancel_Post_Wait_Nop: {
      /*
       * The thread specified by the `thread` parameter shall not be unblocked
       * by the pthread_cancel() call.
       */
      T_false( ctx->wait_returned_early );
      break;
    }

    case CPthreadReqCancel_Post_Wait_NA:
      break;
  }
}

static void CPthreadReqCancel_Post_Life_Check(
  CPthreadReqCancel_Context  *ctx,
  CPthreadReqCancel_Post_Life state
)
{
  switch ( state ) {
    case CPthreadReqCancel_Post_Life_Terminate: {
      /*
       * The thread specified by the `thread` parameter shall be terminated by
       * a thread cancellation.
       */
      JoinWorker( ctx );
      T_false( ctx->worker_continued );
      T_eq_u32( ctx->restarts, 0 );
      break;
    }

    case CPthreadReqCancel_Post_Life_Nop: {
      /*
       * The thread specified by the `thread` parameter shall be neither
       * terminated nor restarted by the pthread_cancel() call.
       */
      KickWorker( ctx );
      T_true( ctx->worker_continued );
      T_eq_u32( ctx->restarts, 0 );
      break;
    }

    case CPthreadReqCancel_Post_Life_NA:
      break;
  }
}

static void CPthreadReqCancel_Post_CancelState_Check(
  CPthreadReqCancel_Context         *ctx,
  CPthreadReqCancel_Post_CancelState state
)
{
  switch ( state ) {
    case CPthreadReqCancel_Post_CancelState_Nop: {
      /*
       * The thread cancel state attribute of the thread specified by the
       * `thread` parameter shall not be modified by the pthread_cancel() call.
       */
      T_eq_int( ctx->state_after, ctx->cancel_state );
      break;
    }

    case CPthreadReqCancel_Post_CancelState_NA:
      break;
  }
}

static void CPthreadReqCancel_Post_CancelType_Check(
  CPthreadReqCancel_Context        *ctx,
  CPthreadReqCancel_Post_CancelType state
)
{
  switch ( state ) {
    case CPthreadReqCancel_Post_CancelType_Nop: {
      /*
       * The thread cancel type attribute of the thread specified by the
       * `thread` parameter shall not be modified by the pthread_cancel() call.
       */
      T_eq_int( ctx->type_after, ctx->cancel_type );
      break;
    }

    case CPthreadReqCancel_Post_CancelType_NA:
      break;
  }
}

static void CPthreadReqCancel_Post_ExitValue_Check(
  CPthreadReqCancel_Context       *ctx,
  CPthreadReqCancel_Post_ExitValue state
)
{
  switch ( state ) {
    case CPthreadReqCancel_Post_ExitValue_Canceled: {
      /*
       * The thread exit value of the thread specified by the `thread`
       * parameter shall be PTHREAD_CANCELED.
       */
      T_eq_ptr( ctx->exit_value, PTHREAD_CANCELED );
      break;
    }

    case CPthreadReqCancel_Post_ExitValue_Exited: {
      /*
       * The thread exit value of the thread specified by the `thread`
       * parameter shall be the value which the thread passed at its
       * termination.
       */
      JoinWorker( ctx );
      T_eq_ptr( ctx->exit_value, &exit_object );
      break;
    }

    case CPthreadReqCancel_Post_ExitValue_NA:
      break;
  }
}

static void CPthreadReqCancel_Post_Pending_Check(
  CPthreadReqCancel_Context     *ctx,
  CPthreadReqCancel_Post_Pending state
)
{
  switch ( state ) {
    case CPthreadReqCancel_Post_Pending_Yes: {
      /*
       * A thread cancellation request shall be pending for the thread
       * specified by the `thread` parameter.
       */
      ctx->phase = PHASE_LATER;
      ResumeTask( ctx->worker_id );
      T_eq_int( pthread_join( ctx->worker_thread, &ctx->exit_value ), 0 );
      ctx->joined = true;
      T_eq_ptr( ctx->exit_value, PTHREAD_CANCELED );
      T_false( ctx->late_returned );
      T_eq_u32( ctx->restarts, 0 );
      break;
    }

    case CPthreadReqCancel_Post_Pending_NA:
      break;
  }
}

static void CPthreadReqCancel_Setup( void )
{
  SetSelfPriority( PRIO_NORMAL );
}

static void CPthreadReqCancel_Setup_Wrap( void *arg )
{
  CPthreadReqCancel_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CPthreadReqCancel_Setup();
}

static void CPthreadReqCancel_Teardown( void )
{
  RestoreRunnerPriority();
}

static void CPthreadReqCancel_Teardown_Wrap( void *arg )
{
  CPthreadReqCancel_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CPthreadReqCancel_Teardown();
}

static void CPthreadReqCancel_Prepare( CPthreadReqCancel_Context *ctx )
{
  ctx->worker_id = RTEMS_ID_NONE;
  ctx->phase = PHASE_ACTION;
  ctx->terminated = false;
  ctx->cancel_state = PTHREAD_CANCEL_ENABLE;
  ctx->cancel_type = PTHREAD_CANCEL_DEFERRED;
  ctx->cancel_pending = false;
  ctx->restart_pending = false;
  ctx->blocked = false;
  ctx->worker_started = false;
  ctx->worker_continued = false;
  ctx->wait_returned = false;
  ctx->wait_returned_early = false;
  ctx->late_returned = false;
  ctx->restarts = 0;
  ctx->joined = false;
  ctx->exit_value = NULL;
  ctx->returned = false;
  ctx->status = -1;
  ctx->state_after = -1;
  ctx->type_after = -1;
}

static void CPthreadReqCancel_Action( CPthreadReqCancel_Context *ctx )
{
  if ( ctx->thread_kind == THREAD_INVALID ) {
    ctx->status = pthread_cancel( INVALID_ID );
    ctx->returned = true;
  } else {
    ctx->worker_id = CreateTask( "WORK", PRIO_HIGH );
    StartTask( ctx->worker_id, Worker, ctx );

    if ( ctx->thread_kind == THREAD_OTHER ) {
      ctx->status = pthread_cancel( ctx->worker_thread );
      ctx->returned = true;
      ctx->wait_returned_early = ctx->wait_returned;
    }
  }
}

static void CPthreadReqCancel_Cleanup( CPthreadReqCancel_Context *ctx )
{
  if ( ctx->worker_id != RTEMS_ID_NONE && !ctx->joined ) {
    ctx->phase = PHASE_CLEANUP;
    KickWorker( ctx );
    (void) rtems_task_resume( ctx->worker_id );
    T_eq_int( pthread_join( ctx->worker_thread, NULL ), 0 );
  }
}

/* clang-format off */

static const CPthreadReqCancel_Entry
CPthreadReqCancel_Entries[] = {
  { 0, 0, 1, 1, 1, 1, 1, 1, CPthreadReqCancel_Post_Status_Srch,
    CPthreadReqCancel_Post_Return_Yes, CPthreadReqCancel_Post_Wait_NA,
    CPthreadReqCancel_Post_Life_NA, CPthreadReqCancel_Post_CancelState_NA,
    CPthreadReqCancel_Post_CancelType_NA, CPthreadReqCancel_Post_ExitValue_NA,
    CPthreadReqCancel_Post_Pending_NA },
  { 0, 0, 1, 0, 0, 0, 0, 1, CPthreadReqCancel_Post_Status_Ok,
    CPthreadReqCancel_Post_Return_Yes, CPthreadReqCancel_Post_Wait_NA,
    CPthreadReqCancel_Post_Life_Nop, CPthreadReqCancel_Post_CancelState_Nop,
    CPthreadReqCancel_Post_CancelType_Nop, CPthreadReqCancel_Post_ExitValue_NA,
    CPthreadReqCancel_Post_Pending_Yes },
  { 0, 0, 0, 1, 1, 1, 1, 1, CPthreadReqCancel_Post_Status_Ok,
    CPthreadReqCancel_Post_Return_Yes, CPthreadReqCancel_Post_Wait_NA,
    CPthreadReqCancel_Post_Life_NA, CPthreadReqCancel_Post_CancelState_NA,
    CPthreadReqCancel_Post_CancelType_NA,
    CPthreadReqCancel_Post_ExitValue_Exited, CPthreadReqCancel_Post_Pending_NA },
  { 1, 0, 1, 0, 0, 0, 0, 1, CPthreadReqCancel_Post_Status_NA,
    CPthreadReqCancel_Post_Return_NA, CPthreadReqCancel_Post_Wait_NA,
    CPthreadReqCancel_Post_Life_NA, CPthreadReqCancel_Post_CancelState_NA,
    CPthreadReqCancel_Post_CancelType_NA, CPthreadReqCancel_Post_ExitValue_NA,
    CPthreadReqCancel_Post_Pending_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqCancel_Post_Status_Ok,
    CPthreadReqCancel_Post_Return_Yes, CPthreadReqCancel_Post_Wait_Nop,
    CPthreadReqCancel_Post_Life_Nop, CPthreadReqCancel_Post_CancelState_Nop,
    CPthreadReqCancel_Post_CancelType_Nop, CPthreadReqCancel_Post_ExitValue_NA,
    CPthreadReqCancel_Post_Pending_Yes },
  { 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqCancel_Post_Status_Ok,
    CPthreadReqCancel_Post_Return_Yes, CPthreadReqCancel_Post_Wait_NA,
    CPthreadReqCancel_Post_Life_Nop, CPthreadReqCancel_Post_CancelState_Nop,
    CPthreadReqCancel_Post_CancelType_Nop, CPthreadReqCancel_Post_ExitValue_NA,
    CPthreadReqCancel_Post_Pending_Yes },
  { 1, 0, 0, 0, 0, 0, 0, 0, CPthreadReqCancel_Post_Status_NA,
    CPthreadReqCancel_Post_Return_NA, CPthreadReqCancel_Post_Wait_NA,
    CPthreadReqCancel_Post_Life_NA, CPthreadReqCancel_Post_CancelState_NA,
    CPthreadReqCancel_Post_CancelType_NA, CPthreadReqCancel_Post_ExitValue_NA,
    CPthreadReqCancel_Post_Pending_NA },
  { 0, 0, 1, 0, 0, 0, 0, 1, CPthreadReqCancel_Post_Status_NA,
    CPthreadReqCancel_Post_Return_No, CPthreadReqCancel_Post_Wait_NA,
    CPthreadReqCancel_Post_Life_Terminate,
    CPthreadReqCancel_Post_CancelState_NA,
    CPthreadReqCancel_Post_CancelType_NA,
    CPthreadReqCancel_Post_ExitValue_Canceled,
    CPthreadReqCancel_Post_Pending_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqCancel_Post_Status_Ok,
    CPthreadReqCancel_Post_Return_Yes, CPthreadReqCancel_Post_Wait_NA,
    CPthreadReqCancel_Post_Life_Terminate,
    CPthreadReqCancel_Post_CancelState_NA,
    CPthreadReqCancel_Post_CancelType_NA,
    CPthreadReqCancel_Post_ExitValue_Canceled,
    CPthreadReqCancel_Post_Pending_NA }
};

static const uint8_t
CPthreadReqCancel_Map[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 3, 3, 3, 3,
  7, 7, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  3, 3, 3, 3, 3, 3, 7, 7, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 4, 5, 4, 5, 4, 5, 4, 5, 6, 6, 6, 6, 6, 6, 8, 8, 4, 5, 4, 5, 4, 5,
  4, 5, 4, 5, 4, 5, 4, 5, 4, 5
};

/* clang-format on */

static size_t CPthreadReqCancel_Scope( void *arg, char *buf, size_t n )
{
  CPthreadReqCancel_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CPthreadReqCancel_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CPthreadReqCancel_Fixture = {
  .setup = CPthreadReqCancel_Setup_Wrap,
  .stop = NULL,
  .teardown = CPthreadReqCancel_Teardown_Wrap,
  .scope = CPthreadReqCancel_Scope,
  .initial_context = &CPthreadReqCancel_Instance
};

static inline CPthreadReqCancel_Entry CPthreadReqCancel_PopEntry(
  CPthreadReqCancel_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqCancel_Entries[ CPthreadReqCancel_Map[ index ] ];
}

static void CPthreadReqCancel_SetPreConditionStates(
  CPthreadReqCancel_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Terminated_NA ) {
    ctx->Map.pcs[ 1 ] = CPthreadReqCancel_Pre_Terminated_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  if ( ctx->Map.entry.Pre_CancelState_NA ) {
    ctx->Map.pcs[ 2 ] = CPthreadReqCancel_Pre_CancelState_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }

  if ( ctx->Map.entry.Pre_CancelType_NA ) {
    ctx->Map.pcs[ 3 ] = CPthreadReqCancel_Pre_CancelType_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }

  if ( ctx->Map.entry.Pre_CancelPending_NA ) {
    ctx->Map.pcs[ 4 ] = CPthreadReqCancel_Pre_CancelPending_NA;
  } else {
    ctx->Map.pcs[ 4 ] = ctx->Map.pci[ 4 ];
  }

  if ( ctx->Map.entry.Pre_RestartPending_NA ) {
    ctx->Map.pcs[ 5 ] = CPthreadReqCancel_Pre_RestartPending_NA;
  } else {
    ctx->Map.pcs[ 5 ] = ctx->Map.pci[ 5 ];
  }

  if ( ctx->Map.entry.Pre_Blocked_NA ) {
    ctx->Map.pcs[ 6 ] = CPthreadReqCancel_Pre_Blocked_NA;
  } else {
    ctx->Map.pcs[ 6 ] = ctx->Map.pci[ 6 ];
  }
}

static void CPthreadReqCancel_TestVariant( CPthreadReqCancel_Context *ctx )
{
  CPthreadReqCancel_Pre_Thread_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqCancel_Pre_Terminated_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqCancel_Pre_CancelState_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CPthreadReqCancel_Pre_CancelType_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CPthreadReqCancel_Pre_CancelPending_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  CPthreadReqCancel_Pre_RestartPending_Prepare( ctx, ctx->Map.pcs[ 5 ] );
  CPthreadReqCancel_Pre_Blocked_Prepare( ctx, ctx->Map.pcs[ 6 ] );
  CPthreadReqCancel_Action( ctx );
  CPthreadReqCancel_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CPthreadReqCancel_Post_Return_Check( ctx, ctx->Map.entry.Post_Return );
  CPthreadReqCancel_Post_Wait_Check( ctx, ctx->Map.entry.Post_Wait );
  CPthreadReqCancel_Post_Life_Check( ctx, ctx->Map.entry.Post_Life );
  CPthreadReqCancel_Post_CancelState_Check(
    ctx,
    ctx->Map.entry.Post_CancelState
  );
  CPthreadReqCancel_Post_CancelType_Check(
    ctx,
    ctx->Map.entry.Post_CancelType
  );
  CPthreadReqCancel_Post_ExitValue_Check( ctx, ctx->Map.entry.Post_ExitValue );
  CPthreadReqCancel_Post_Pending_Check( ctx, ctx->Map.entry.Post_Pending );
}

/**
 * @fn void T_case_body_CPthreadReqCancel( void )
 */
T_TEST_CASE_FIXTURE( CPthreadReqCancel, &CPthreadReqCancel_Fixture )
{
  CPthreadReqCancel_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CPthreadReqCancel_Pre_Thread_Invalid;
    ctx->Map.pci[ 0 ] < CPthreadReqCancel_Pre_Thread_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CPthreadReqCancel_Pre_Terminated_Yes;
      ctx->Map.pci[ 1 ] < CPthreadReqCancel_Pre_Terminated_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CPthreadReqCancel_Pre_CancelState_Enable;
        ctx->Map.pci[ 2 ] < CPthreadReqCancel_Pre_CancelState_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = CPthreadReqCancel_Pre_CancelType_Deferred;
          ctx->Map.pci[ 3 ] < CPthreadReqCancel_Pre_CancelType_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          for (
            ctx->Map.pci[ 4 ] = CPthreadReqCancel_Pre_CancelPending_Yes;
            ctx->Map.pci[ 4 ] < CPthreadReqCancel_Pre_CancelPending_NA;
            ++ctx->Map.pci[ 4 ]
          ) {
            for (
              ctx->Map.pci[ 5 ] = CPthreadReqCancel_Pre_RestartPending_Yes;
              ctx->Map.pci[ 5 ] < CPthreadReqCancel_Pre_RestartPending_NA;
              ++ctx->Map.pci[ 5 ]
            ) {
              for (
                ctx->Map.pci[ 6 ] = CPthreadReqCancel_Pre_Blocked_Yes;
                ctx->Map.pci[ 6 ] < CPthreadReqCancel_Pre_Blocked_NA;
                ++ctx->Map.pci[ 6 ]
              ) {
                ctx->Map.entry = CPthreadReqCancel_PopEntry( ctx );

                if ( ctx->Map.entry.Skip ) {
                  continue;
                }

                CPthreadReqCancel_SetPreConditionStates( ctx );
                CPthreadReqCancel_Prepare( ctx );
                CPthreadReqCancel_TestVariant( ctx );
                CPthreadReqCancel_Cleanup( ctx );
              }
            }
          }
        }
      }
    }
  }
}

/** @} */
