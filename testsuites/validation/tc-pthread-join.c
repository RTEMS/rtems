/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqJoin
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
 * @defgroup CPthreadReqJoin spec:/c/pthread/req/join
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CPthreadReqJoin_Pre_Thread_Invalid,
  CPthreadReqJoin_Pre_Thread_Executing,
  CPthreadReqJoin_Pre_Thread_Other,
  CPthreadReqJoin_Pre_Thread_NA
} CPthreadReqJoin_Pre_Thread;

typedef enum {
  CPthreadReqJoin_Pre_Joinable_Yes,
  CPthreadReqJoin_Pre_Joinable_No,
  CPthreadReqJoin_Pre_Joinable_NA
} CPthreadReqJoin_Pre_Joinable;

typedef enum {
  CPthreadReqJoin_Pre_Terminated_Yes,
  CPthreadReqJoin_Pre_Terminated_No,
  CPthreadReqJoin_Pre_Terminated_NA
} CPthreadReqJoin_Pre_Terminated;

typedef enum {
  CPthreadReqJoin_Pre_Joining_Yes,
  CPthreadReqJoin_Pre_Joining_No,
  CPthreadReqJoin_Pre_Joining_NA
} CPthreadReqJoin_Pre_Joining;

typedef enum {
  CPthreadReqJoin_Pre_Canceled_Yes,
  CPthreadReqJoin_Pre_Canceled_No,
  CPthreadReqJoin_Pre_Canceled_NA
} CPthreadReqJoin_Pre_Canceled;

typedef enum {
  CPthreadReqJoin_Pre_Restarted_Yes,
  CPthreadReqJoin_Pre_Restarted_No,
  CPthreadReqJoin_Pre_Restarted_NA
} CPthreadReqJoin_Pre_Restarted;

typedef enum {
  CPthreadReqJoin_Pre_TargetRestarted_Yes,
  CPthreadReqJoin_Pre_TargetRestarted_No,
  CPthreadReqJoin_Pre_TargetRestarted_NA
} CPthreadReqJoin_Pre_TargetRestarted;

typedef enum {
  CPthreadReqJoin_Pre_ValuePtr_Valid,
  CPthreadReqJoin_Pre_ValuePtr_Null,
  CPthreadReqJoin_Pre_ValuePtr_NA
} CPthreadReqJoin_Pre_ValuePtr;

typedef enum {
  CPthreadReqJoin_Post_Status_Ok,
  CPthreadReqJoin_Post_Status_Deadlk,
  CPthreadReqJoin_Post_Status_Inval,
  CPthreadReqJoin_Post_Status_Srch,
  CPthreadReqJoin_Post_Status_NA
} CPthreadReqJoin_Post_Status;

typedef enum {
  CPthreadReqJoin_Post_Value_Set,
  CPthreadReqJoin_Post_Value_Nop,
  CPthreadReqJoin_Post_Value_NA
} CPthreadReqJoin_Post_Value;

typedef enum {
  CPthreadReqJoin_Post_Wait_Yes,
  CPthreadReqJoin_Post_Wait_No,
  CPthreadReqJoin_Post_Wait_NA
} CPthreadReqJoin_Post_Wait;

typedef enum {
  CPthreadReqJoin_Post_Return_Yes,
  CPthreadReqJoin_Post_Return_No,
  CPthreadReqJoin_Post_Return_NA
} CPthreadReqJoin_Post_Return;

typedef enum {
  CPthreadReqJoin_Post_Target_Reclaimed,
  CPthreadReqJoin_Post_Target_Joinable,
  CPthreadReqJoin_Post_Target_Detached,
  CPthreadReqJoin_Post_Target_NA
} CPthreadReqJoin_Post_Target;

typedef enum {
  CPthreadReqJoin_Post_Caller_Nop,
  CPthreadReqJoin_Post_Caller_Terminated,
  CPthreadReqJoin_Post_Caller_Restarted,
  CPthreadReqJoin_Post_Caller_NA
} CPthreadReqJoin_Post_Caller;

typedef struct {
  uint32_t Skip : 1;
  uint32_t Pre_Thread_NA : 1;
  uint32_t Pre_Joinable_NA : 1;
  uint32_t Pre_Terminated_NA : 1;
  uint32_t Pre_Joining_NA : 1;
  uint32_t Pre_Canceled_NA : 1;
  uint32_t Pre_Restarted_NA : 1;
  uint32_t Pre_TargetRestarted_NA : 1;
  uint32_t Pre_ValuePtr_NA : 1;
  uint32_t Post_Status : 3;
  uint32_t Post_Value : 2;
  uint32_t Post_Wait : 2;
  uint32_t Post_Return : 2;
  uint32_t Post_Target : 2;
  uint32_t Post_Caller : 2;
} CPthreadReqJoin_Entry;

typedef enum { THREAD_INVALID, THREAD_EXECUTING, THREAD_OTHER } ThreadKind;

/**
 * @brief Test context for spec:/c/pthread/req/join test case.
 */
typedef struct {
  /**
   * @brief This member contains the identifier of the calling task.
   */
  rtems_id caller_id;

  /**
   * @brief This member contains the thread identifier of the calling task.
   */
  pthread_t caller_thread;

  /**
   * @brief This member is true, if the calling task started.
   */
  bool caller_started;

  /**
   * @brief This member contains the identifier of the target task.
   */
  rtems_id target_id;

  /**
   * @brief This member contains the thread identifier of the target task.
   */
  pthread_t target_thread;

  /**
   * @brief This member is true, if the target task started.
   */
  bool target_started;

  /**
   * @brief This member specifies the thread which the call joins.
   */
  ThreadKind thread_kind;

  /**
   * @brief This member is true, if the target task is a joinable thread.
   */
  bool joinable;

  /**
   * @brief This member is true, if the target task terminates before the call.
   */
  bool terminated;

  /**
   * @brief This member is true, if the target task waits to join the calling
   *   task.
   */
  bool joining;

  /**
   * @brief This member is true, if a thread cancellation of the calling task
   *   is requested during the call.
   */
  bool canceled;

  /**
   * @brief This member is true, if a restart of the calling task is requested
   *   during the call.
   */
  bool restarted;

  /**
   * @brief This member is true, if the target task is restarted during the
   *   call.
   */
  bool target_restarted;

  /**
   * @brief This member is true, if the call returned to the caller.
   */
  bool returned;

  /**
   * @brief This member is true, if the call returned before the target task
   *   was resumed.
   */
  bool returned_early;

  /**
   * @brief This member is true, if the call returned before the restart of the
   *   target task ended.
   */
  bool returned_after_restart;

  /**
   * @brief This member is true, if the calling task exited.
   */
  bool caller_exited;

  /**
   * @brief This member is true, if the test joined the calling task.
   */
  bool caller_joined;

  /**
   * @brief This member contains the count of restarts of the calling task.
   */
  uint32_t caller_restarts;

  /**
   * @brief This member is true, if the target task exited.
   */
  bool target_exited;

  /**
   * @brief This member provides the object referenced by the `value_ptr`
   *   parameter.
   */
  void *value;

  /**
   * @brief This member contains the return value of the pthread_join() call.
   */
  int status;

  /**
   * @brief This member specifies the `thread` parameter value.
   */
  pthread_t thread;

  /**
   * @brief This member specifies the `value_ptr` parameter value.
   */
  void **value_ptr;

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
    CPthreadReqJoin_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqJoin_Context;

static CPthreadReqJoin_Context CPthreadReqJoin_Instance;

static const char *const CPthreadReqJoin_PreDesc_Thread[] =
  { "Invalid", "Executing", "Other", "NA" };

static const char *const CPthreadReqJoin_PreDesc_Joinable[] =
  { "Yes", "No", "NA" };

static const char *const CPthreadReqJoin_PreDesc_Terminated[] =
  { "Yes", "No", "NA" };

static const char *const CPthreadReqJoin_PreDesc_Joining[] =
  { "Yes", "No", "NA" };

static const char *const CPthreadReqJoin_PreDesc_Canceled[] =
  { "Yes", "No", "NA" };

static const char *const CPthreadReqJoin_PreDesc_Restarted[] =
  { "Yes", "No", "NA" };

static const char *const CPthreadReqJoin_PreDesc_TargetRestarted[] =
  { "Yes", "No", "NA" };

static const char *const CPthreadReqJoin_PreDesc_ValuePtr[] =
  { "Valid", "Null", "NA" };

static const char *const *const CPthreadReqJoin_PreDesc[] = {
  CPthreadReqJoin_PreDesc_Thread,
  CPthreadReqJoin_PreDesc_Joinable,
  CPthreadReqJoin_PreDesc_Terminated,
  CPthreadReqJoin_PreDesc_Joining,
  CPthreadReqJoin_PreDesc_Canceled,
  CPthreadReqJoin_PreDesc_Restarted,
  CPthreadReqJoin_PreDesc_TargetRestarted,
  CPthreadReqJoin_PreDesc_ValuePtr,
  NULL
};

typedef CPthreadReqJoin_Context Context;

static int exit_object;

static int initial_object;

static void EndTarget( Context *ctx )
{
  if ( ctx->target_restarted ) {
    SendEvents( ctx->target_id, RTEMS_EVENT_0 );
  } else {
    ResumeTask( ctx->target_id );
  }
}

static void Caller( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;

  if ( ctx->caller_started ) {
    ++ctx->caller_restarts;
  } else {
    ctx->caller_started = true;
    ctx->caller_thread = pthread_self();
    SuspendSelf();
    ctx->status = pthread_join( ctx->thread, ctx->value_ptr );
    ctx->returned = true;
  }

  SuspendSelf();
  ctx->caller_exited = true;
  pthread_exit( NULL );
}

static void Target( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;

  if ( ctx->target_started ) {
    (void) ReceiveAnyEvents();
  } else {
    ctx->target_started = true;
    ctx->target_thread = pthread_self();

    if ( ctx->joining ) {
      (void) pthread_join( ctx->caller_thread, NULL );
    }

    if ( !ctx->terminated ) {
      SuspendSelf();
    }
  }

  ctx->target_exited = true;
  pthread_exit( &exit_object );
}

static void CPthreadReqJoin_Pre_Thread_Prepare(
  CPthreadReqJoin_Context   *ctx,
  CPthreadReqJoin_Pre_Thread state
)
{
  switch ( state ) {
    case CPthreadReqJoin_Pre_Thread_Invalid: {
      /*
       * While the `thread` parameter is not associated with a thread.
       */
      ctx->thread_kind = THREAD_INVALID;
      break;
    }

    case CPthreadReqJoin_Pre_Thread_Executing: {
      /*
       * While the `thread` parameter is associated with the executing task.
       */
      ctx->thread_kind = THREAD_EXECUTING;
      break;
    }

    case CPthreadReqJoin_Pre_Thread_Other: {
      /*
       * While the `thread` parameter is associated with a thread other than
       * the executing task.
       */
      ctx->thread_kind = THREAD_OTHER;
      break;
    }

    case CPthreadReqJoin_Pre_Thread_NA:
      break;
  }
}

static void CPthreadReqJoin_Pre_Joinable_Prepare(
  CPthreadReqJoin_Context     *ctx,
  CPthreadReqJoin_Pre_Joinable state
)
{
  switch ( state ) {
    case CPthreadReqJoin_Pre_Joinable_Yes: {
      /*
       * While the thread specified by the `thread` parameter is a joinable
       * thread.
       */
      ctx->joinable = true;
      break;
    }

    case CPthreadReqJoin_Pre_Joinable_No: {
      /*
       * While the thread specified by the `thread` parameter is a detached
       * thread.
       */
      ctx->joinable = false;
      break;
    }

    case CPthreadReqJoin_Pre_Joinable_NA:
      break;
  }
}

static void CPthreadReqJoin_Pre_Terminated_Prepare(
  CPthreadReqJoin_Context       *ctx,
  CPthreadReqJoin_Pre_Terminated state
)
{
  switch ( state ) {
    case CPthreadReqJoin_Pre_Terminated_Yes: {
      /*
       * While the thread specified by the `thread` parameter is terminated.
       */
      ctx->terminated = true;
      break;
    }

    case CPthreadReqJoin_Pre_Terminated_No: {
      /*
       * While the thread specified by the `thread` parameter is not
       * terminated.
       */
      ctx->terminated = false;
      break;
    }

    case CPthreadReqJoin_Pre_Terminated_NA:
      break;
  }
}

static void CPthreadReqJoin_Pre_Joining_Prepare(
  CPthreadReqJoin_Context    *ctx,
  CPthreadReqJoin_Pre_Joining state
)
{
  switch ( state ) {
    case CPthreadReqJoin_Pre_Joining_Yes: {
      /*
       * While the thread specified by the `thread` parameter waits to join the
       * executing task.
       */
      ctx->joining = true;
      break;
    }

    case CPthreadReqJoin_Pre_Joining_No: {
      /*
       * While the thread specified by the `thread` parameter does not wait to
       * join the executing task.
       */
      ctx->joining = false;
      break;
    }

    case CPthreadReqJoin_Pre_Joining_NA:
      break;
  }
}

static void CPthreadReqJoin_Pre_Canceled_Prepare(
  CPthreadReqJoin_Context     *ctx,
  CPthreadReqJoin_Pre_Canceled state
)
{
  switch ( state ) {
    case CPthreadReqJoin_Pre_Canceled_Yes: {
      /*
       * While a thread cancellation of the executing task is requested during
       * the pthread_join() call.
       */
      ctx->canceled = true;
      break;
    }

    case CPthreadReqJoin_Pre_Canceled_No: {
      /*
       * While no thread cancellation of the executing task is requested during
       * the pthread_join() call.
       */
      ctx->canceled = false;
      break;
    }

    case CPthreadReqJoin_Pre_Canceled_NA:
      break;
  }
}

static void CPthreadReqJoin_Pre_Restarted_Prepare(
  CPthreadReqJoin_Context      *ctx,
  CPthreadReqJoin_Pre_Restarted state
)
{
  switch ( state ) {
    case CPthreadReqJoin_Pre_Restarted_Yes: {
      /*
       * While a restart of the executing task by rtems_task_restart() is
       * requested during the pthread_join() call.
       */
      ctx->restarted = true;
      break;
    }

    case CPthreadReqJoin_Pre_Restarted_No: {
      /*
       * While no restart of the executing task is requested during the
       * pthread_join() call.
       */
      ctx->restarted = false;
      break;
    }

    case CPthreadReqJoin_Pre_Restarted_NA:
      break;
  }
}

static void CPthreadReqJoin_Pre_TargetRestarted_Prepare(
  CPthreadReqJoin_Context            *ctx,
  CPthreadReqJoin_Pre_TargetRestarted state
)
{
  switch ( state ) {
    case CPthreadReqJoin_Pre_TargetRestarted_Yes: {
      /*
       * While the thread specified by the `thread` parameter is restarted by
       * rtems_task_restart() during the pthread_join() call.
       */
      ctx->target_restarted = true;
      break;
    }

    case CPthreadReqJoin_Pre_TargetRestarted_No: {
      /*
       * While the thread specified by the `thread` parameter is not restarted
       * during the pthread_join() call.
       */
      ctx->target_restarted = false;
      break;
    }

    case CPthreadReqJoin_Pre_TargetRestarted_NA:
      break;
  }
}

static void CPthreadReqJoin_Pre_ValuePtr_Prepare(
  CPthreadReqJoin_Context     *ctx,
  CPthreadReqJoin_Pre_ValuePtr state
)
{
  switch ( state ) {
    case CPthreadReqJoin_Pre_ValuePtr_Valid: {
      /*
       * While the `value_ptr` parameter references an object of type void
       * pointer.
       */
      ctx->value_ptr = &ctx->value;
      break;
    }

    case CPthreadReqJoin_Pre_ValuePtr_Null: {
      /*
       * While the `value_ptr` parameter is equal to NULL.
       */
      ctx->value_ptr = NULL;
      break;
    }

    case CPthreadReqJoin_Pre_ValuePtr_NA:
      break;
  }
}

static void CPthreadReqJoin_Post_Status_Check(
  CPthreadReqJoin_Context    *ctx,
  CPthreadReqJoin_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqJoin_Post_Status_Ok: {
      /*
       * The return value of pthread_join() shall be zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CPthreadReqJoin_Post_Status_Deadlk: {
      /*
       * The return value of pthread_join() shall be EDEADLK.
       */
      T_eq_int( ctx->status, EDEADLK );
      break;
    }

    case CPthreadReqJoin_Post_Status_Inval: {
      /*
       * The return value of pthread_join() shall be EINVAL.
       */
      T_eq_int( ctx->status, EINVAL );
      break;
    }

    case CPthreadReqJoin_Post_Status_Srch: {
      /*
       * The return value of pthread_join() shall be ESRCH.
       */
      T_eq_int( ctx->status, ESRCH );
      break;
    }

    case CPthreadReqJoin_Post_Status_NA:
      break;
  }
}

static void CPthreadReqJoin_Post_Value_Check(
  CPthreadReqJoin_Context   *ctx,
  CPthreadReqJoin_Post_Value state
)
{
  switch ( state ) {
    case CPthreadReqJoin_Post_Value_Set: {
      /*
       * The value of the object referenced by the `value_ptr` parameter shall
       * be the thread exit value of the thread specified by the `thread`
       * parameter.
       */
      T_eq_ptr( ctx->value, &exit_object );
      break;
    }

    case CPthreadReqJoin_Post_Value_Nop: {
      /*
       * The object referenced by the `value_ptr` parameter shall not be
       * modified by the pthread_join() call.
       */
      T_eq_ptr( ctx->value, &initial_object );
      break;
    }

    case CPthreadReqJoin_Post_Value_NA:
      break;
  }
}

static void CPthreadReqJoin_Post_Wait_Check(
  CPthreadReqJoin_Context  *ctx,
  CPthreadReqJoin_Post_Wait state
)
{
  switch ( state ) {
    case CPthreadReqJoin_Post_Wait_Yes: {
      /*
       * The executing task shall be blocked by the pthread_join() call until
       * the thread specified by the `thread` parameter terminates.
       */
      T_false( ctx->returned_early );

      if ( ctx->target_restarted ) {
        T_false( ctx->returned_after_restart );
      }
      break;
    }

    case CPthreadReqJoin_Post_Wait_No: {
      /*
       * The executing task shall not be blocked by the pthread_join() call.
       */
      T_true( ctx->returned_early );
      break;
    }

    case CPthreadReqJoin_Post_Wait_NA:
      break;
  }
}

static void CPthreadReqJoin_Post_Return_Check(
  CPthreadReqJoin_Context    *ctx,
  CPthreadReqJoin_Post_Return state
)
{
  switch ( state ) {
    case CPthreadReqJoin_Post_Return_Yes: {
      /*
       * The pthread_join() call shall return to the caller.
       */
      T_true( ctx->returned );
      break;
    }

    case CPthreadReqJoin_Post_Return_No: {
      /*
       * The pthread_join() call shall not return to the caller.
       */
      T_false( ctx->returned );
      break;
    }

    case CPthreadReqJoin_Post_Return_NA:
      break;
  }
}

static void CPthreadReqJoin_Post_Target_Check(
  CPthreadReqJoin_Context    *ctx,
  CPthreadReqJoin_Post_Target state
)
{
  switch ( state ) {
    case CPthreadReqJoin_Post_Target_Reclaimed: {
      /*
       * The identifier specified by the `thread` parameter shall be associated
       * with no thread.
       */
      T_eq_int( pthread_detach( ctx->target_thread ), ESRCH );
      break;
    }

    case CPthreadReqJoin_Post_Target_Joinable: {
      /*
       * The thread specified by the `thread` parameter shall be a joinable
       * thread.
       */
      T_eq_int( pthread_detach( ctx->target_thread ), 0 );
      break;
    }

    case CPthreadReqJoin_Post_Target_Detached: {
      /*
       * The thread specified by the `thread` parameter shall be a detached
       * thread.
       */
      T_eq_int( pthread_detach( ctx->target_thread ), EINVAL );
      break;
    }

    case CPthreadReqJoin_Post_Target_NA:
      break;
  }
}

static void CPthreadReqJoin_Post_Caller_Check(
  CPthreadReqJoin_Context    *ctx,
  CPthreadReqJoin_Post_Caller state
)
{
  void *value;

  switch ( state ) {
    case CPthreadReqJoin_Post_Caller_Nop: {
      /*
       * The executing task shall be neither terminated nor restarted during
       * the pthread_join() call.
       */
      T_eq_u32( ctx->caller_restarts, 0 );
      T_false( ctx->caller_exited );
      break;
    }

    case CPthreadReqJoin_Post_Caller_Terminated: {
      /*
       * The executing task shall be terminated by a thread cancellation.
       */
      T_eq_int( pthread_join( ctx->caller_thread, &value ), 0 );
      ctx->caller_joined = true;
      T_eq_ptr( value, PTHREAD_CANCELED );
      T_eq_u32( ctx->caller_restarts, 0 );
      break;
    }

    case CPthreadReqJoin_Post_Caller_Restarted: {
      /*
       * The executing task shall be restarted.
       */
      T_eq_u32( ctx->caller_restarts, 1 );
      T_false( ctx->caller_exited );
      break;
    }

    case CPthreadReqJoin_Post_Caller_NA:
      break;
  }
}

static void CPthreadReqJoin_Setup( void )
{
  SetSelfPriority( PRIO_NORMAL );
}

static void CPthreadReqJoin_Setup_Wrap( void *arg )
{
  CPthreadReqJoin_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CPthreadReqJoin_Setup();
}

static void CPthreadReqJoin_Teardown( void )
{
  RestoreRunnerPriority();
}

static void CPthreadReqJoin_Teardown_Wrap( void *arg )
{
  CPthreadReqJoin_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CPthreadReqJoin_Teardown();
}

static void CPthreadReqJoin_Prepare( CPthreadReqJoin_Context *ctx )
{
  ctx->joinable = true;
  ctx->terminated = false;
  ctx->joining = false;
  ctx->canceled = false;
  ctx->restarted = false;
  ctx->target_restarted = false;
  ctx->returned = false;
  ctx->returned_early = false;
  ctx->returned_after_restart = false;
  ctx->caller_started = false;
  ctx->caller_exited = false;
  ctx->caller_joined = false;
  ctx->caller_restarts = 0;
  ctx->target_started = false;
  ctx->target_exited = false;
  ctx->status = -1;
  ctx->value = &initial_object;
}

static void CPthreadReqJoin_Action( CPthreadReqJoin_Context *ctx )
{
  ctx->caller_id = CreateTask( "CALL", PRIO_HIGH );
  StartTask( ctx->caller_id, Caller, ctx );
  ctx->target_id = CreateTask( "TARG", PRIO_HIGH );
  StartTask( ctx->target_id, Target, ctx );

  if ( !ctx->joinable ) {
    T_eq_int( pthread_detach( ctx->target_thread ), 0 );
  }

  switch ( ctx->thread_kind ) {
    case THREAD_INVALID:
      ctx->thread = INVALID_ID;
      break;
    case THREAD_EXECUTING:
      ctx->thread = ctx->caller_thread;
      break;
    default:
      ctx->thread = ctx->target_thread;
      break;
  }

  ResumeTask( ctx->caller_id );
  ctx->returned_early = ctx->returned;

  if ( ctx->target_restarted ) {
    RestartTask( ctx->target_id, ctx );
    (void) rtems_task_resume( ctx->target_id );
    ctx->returned_after_restart = ctx->returned;
  }

  if ( ctx->canceled || ctx->restarted ) {
    RequestLifeChangesWithinISR(
      ctx->caller_id,
      ctx,
      ctx->restarted,
      ctx->canceled
    );
  } else if ( !ctx->returned ) {
    EndTarget( ctx );
  }
}

static void CPthreadReqJoin_Cleanup( CPthreadReqJoin_Context *ctx )
{
  if ( !ctx->caller_exited && !ctx->canceled ) {
    ResumeTask( ctx->caller_id );
  }

  if ( !ctx->joining && !ctx->caller_joined ) {
    T_eq_int( pthread_join( ctx->caller_thread, NULL ), 0 );
  }

  (void) pthread_detach( ctx->target_thread );

  if ( !ctx->target_exited ) {
    EndTarget( ctx );
  }
}

/* clang-format off */

static const CPthreadReqJoin_Entry
CPthreadReqJoin_Entries[] = {
  { 0, 0, 1, 1, 1, 1, 1, 1, 0, CPthreadReqJoin_Post_Status_Srch,
    CPthreadReqJoin_Post_Value_Nop, CPthreadReqJoin_Post_Wait_No,
    CPthreadReqJoin_Post_Return_Yes, CPthreadReqJoin_Post_Target_NA,
    CPthreadReqJoin_Post_Caller_Nop },
  { 0, 0, 1, 1, 1, 1, 1, 1, 0, CPthreadReqJoin_Post_Status_Srch,
    CPthreadReqJoin_Post_Value_NA, CPthreadReqJoin_Post_Wait_No,
    CPthreadReqJoin_Post_Return_Yes, CPthreadReqJoin_Post_Target_NA,
    CPthreadReqJoin_Post_Caller_Nop },
  { 0, 0, 1, 1, 1, 1, 1, 1, 0, CPthreadReqJoin_Post_Status_Deadlk,
    CPthreadReqJoin_Post_Value_Nop, CPthreadReqJoin_Post_Wait_No,
    CPthreadReqJoin_Post_Return_Yes, CPthreadReqJoin_Post_Target_NA,
    CPthreadReqJoin_Post_Caller_Nop },
  { 0, 0, 1, 1, 1, 1, 1, 1, 0, CPthreadReqJoin_Post_Status_Deadlk,
    CPthreadReqJoin_Post_Value_NA, CPthreadReqJoin_Post_Wait_No,
    CPthreadReqJoin_Post_Return_Yes, CPthreadReqJoin_Post_Target_NA,
    CPthreadReqJoin_Post_Caller_Nop },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqJoin_Post_Status_NA,
    CPthreadReqJoin_Post_Value_NA, CPthreadReqJoin_Post_Wait_NA,
    CPthreadReqJoin_Post_Return_NA, CPthreadReqJoin_Post_Target_NA,
    CPthreadReqJoin_Post_Caller_NA },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqJoin_Post_Status_NA,
    CPthreadReqJoin_Post_Value_NA, CPthreadReqJoin_Post_Wait_NA,
    CPthreadReqJoin_Post_Return_NA, CPthreadReqJoin_Post_Target_NA,
    CPthreadReqJoin_Post_Caller_NA },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqJoin_Post_Status_NA,
    CPthreadReqJoin_Post_Value_NA, CPthreadReqJoin_Post_Wait_NA,
    CPthreadReqJoin_Post_Return_NA, CPthreadReqJoin_Post_Target_NA,
    CPthreadReqJoin_Post_Caller_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqJoin_Post_Status_NA,
    CPthreadReqJoin_Post_Value_NA, CPthreadReqJoin_Post_Wait_Yes,
    CPthreadReqJoin_Post_Return_No, CPthreadReqJoin_Post_Target_Joinable,
    CPthreadReqJoin_Post_Caller_Terminated },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqJoin_Post_Status_NA,
    CPthreadReqJoin_Post_Value_NA, CPthreadReqJoin_Post_Wait_Yes,
    CPthreadReqJoin_Post_Return_No, CPthreadReqJoin_Post_Target_Joinable,
    CPthreadReqJoin_Post_Caller_Restarted },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqJoin_Post_Status_Ok,
    CPthreadReqJoin_Post_Value_Set, CPthreadReqJoin_Post_Wait_Yes,
    CPthreadReqJoin_Post_Return_Yes, CPthreadReqJoin_Post_Target_Reclaimed,
    CPthreadReqJoin_Post_Caller_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqJoin_Post_Status_Ok,
    CPthreadReqJoin_Post_Value_NA, CPthreadReqJoin_Post_Wait_Yes,
    CPthreadReqJoin_Post_Return_Yes, CPthreadReqJoin_Post_Target_Reclaimed,
    CPthreadReqJoin_Post_Caller_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqJoin_Post_Status_Inval,
    CPthreadReqJoin_Post_Value_Nop, CPthreadReqJoin_Post_Wait_No,
    CPthreadReqJoin_Post_Return_Yes, CPthreadReqJoin_Post_Target_Detached,
    CPthreadReqJoin_Post_Caller_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqJoin_Post_Status_Inval,
    CPthreadReqJoin_Post_Value_NA, CPthreadReqJoin_Post_Wait_No,
    CPthreadReqJoin_Post_Return_Yes, CPthreadReqJoin_Post_Target_Detached,
    CPthreadReqJoin_Post_Caller_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqJoin_Post_Status_Ok,
    CPthreadReqJoin_Post_Value_Set, CPthreadReqJoin_Post_Wait_No,
    CPthreadReqJoin_Post_Return_Yes, CPthreadReqJoin_Post_Target_Reclaimed,
    CPthreadReqJoin_Post_Caller_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqJoin_Post_Status_Ok,
    CPthreadReqJoin_Post_Value_NA, CPthreadReqJoin_Post_Wait_No,
    CPthreadReqJoin_Post_Return_Yes, CPthreadReqJoin_Post_Target_Reclaimed,
    CPthreadReqJoin_Post_Caller_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqJoin_Post_Status_Deadlk,
    CPthreadReqJoin_Post_Value_Nop, CPthreadReqJoin_Post_Wait_No,
    CPthreadReqJoin_Post_Return_Yes, CPthreadReqJoin_Post_Target_Joinable,
    CPthreadReqJoin_Post_Caller_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqJoin_Post_Status_Deadlk,
    CPthreadReqJoin_Post_Value_NA, CPthreadReqJoin_Post_Wait_No,
    CPthreadReqJoin_Post_Return_Yes, CPthreadReqJoin_Post_Target_Joinable,
    CPthreadReqJoin_Post_Caller_Nop }
};

static const uint8_t
CPthreadReqJoin_Map[] = {
  0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
  0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
  0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
  0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
  0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 2, 3,
  2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3,
  2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3,
  2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3,
  2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3,
  2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  13, 14, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 15, 16, 7, 7, 7, 7, 7, 7,
  7, 7, 8, 8, 8, 8, 9, 10, 9, 10, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 11, 12, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 11, 12
};

/* clang-format on */

static size_t CPthreadReqJoin_Scope( void *arg, char *buf, size_t n )
{
  CPthreadReqJoin_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CPthreadReqJoin_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CPthreadReqJoin_Fixture = {
  .setup = CPthreadReqJoin_Setup_Wrap,
  .stop = NULL,
  .teardown = CPthreadReqJoin_Teardown_Wrap,
  .scope = CPthreadReqJoin_Scope,
  .initial_context = &CPthreadReqJoin_Instance
};

static inline CPthreadReqJoin_Entry CPthreadReqJoin_PopEntry(
  CPthreadReqJoin_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqJoin_Entries[ CPthreadReqJoin_Map[ index ] ];
}

static void CPthreadReqJoin_SetPreConditionStates(
  CPthreadReqJoin_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Joinable_NA ) {
    ctx->Map.pcs[ 1 ] = CPthreadReqJoin_Pre_Joinable_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  if ( ctx->Map.entry.Pre_Terminated_NA ) {
    ctx->Map.pcs[ 2 ] = CPthreadReqJoin_Pre_Terminated_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }

  if ( ctx->Map.entry.Pre_Joining_NA ) {
    ctx->Map.pcs[ 3 ] = CPthreadReqJoin_Pre_Joining_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }

  if ( ctx->Map.entry.Pre_Canceled_NA ) {
    ctx->Map.pcs[ 4 ] = CPthreadReqJoin_Pre_Canceled_NA;
  } else {
    ctx->Map.pcs[ 4 ] = ctx->Map.pci[ 4 ];
  }

  if ( ctx->Map.entry.Pre_Restarted_NA ) {
    ctx->Map.pcs[ 5 ] = CPthreadReqJoin_Pre_Restarted_NA;
  } else {
    ctx->Map.pcs[ 5 ] = ctx->Map.pci[ 5 ];
  }

  if ( ctx->Map.entry.Pre_TargetRestarted_NA ) {
    ctx->Map.pcs[ 6 ] = CPthreadReqJoin_Pre_TargetRestarted_NA;
  } else {
    ctx->Map.pcs[ 6 ] = ctx->Map.pci[ 6 ];
  }

  ctx->Map.pcs[ 7 ] = ctx->Map.pci[ 7 ];
}

static void CPthreadReqJoin_TestVariant( CPthreadReqJoin_Context *ctx )
{
  CPthreadReqJoin_Pre_Thread_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqJoin_Pre_Joinable_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqJoin_Pre_Terminated_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CPthreadReqJoin_Pre_Joining_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CPthreadReqJoin_Pre_Canceled_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  CPthreadReqJoin_Pre_Restarted_Prepare( ctx, ctx->Map.pcs[ 5 ] );
  CPthreadReqJoin_Pre_TargetRestarted_Prepare( ctx, ctx->Map.pcs[ 6 ] );
  CPthreadReqJoin_Pre_ValuePtr_Prepare( ctx, ctx->Map.pcs[ 7 ] );
  CPthreadReqJoin_Action( ctx );
  CPthreadReqJoin_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CPthreadReqJoin_Post_Value_Check( ctx, ctx->Map.entry.Post_Value );
  CPthreadReqJoin_Post_Wait_Check( ctx, ctx->Map.entry.Post_Wait );
  CPthreadReqJoin_Post_Return_Check( ctx, ctx->Map.entry.Post_Return );
  CPthreadReqJoin_Post_Target_Check( ctx, ctx->Map.entry.Post_Target );
  CPthreadReqJoin_Post_Caller_Check( ctx, ctx->Map.entry.Post_Caller );
}

/**
 * @fn void T_case_body_CPthreadReqJoin( void )
 */
T_TEST_CASE_FIXTURE( CPthreadReqJoin, &CPthreadReqJoin_Fixture )
{
  CPthreadReqJoin_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CPthreadReqJoin_Pre_Thread_Invalid;
    ctx->Map.pci[ 0 ] < CPthreadReqJoin_Pre_Thread_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CPthreadReqJoin_Pre_Joinable_Yes;
      ctx->Map.pci[ 1 ] < CPthreadReqJoin_Pre_Joinable_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CPthreadReqJoin_Pre_Terminated_Yes;
        ctx->Map.pci[ 2 ] < CPthreadReqJoin_Pre_Terminated_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = CPthreadReqJoin_Pre_Joining_Yes;
          ctx->Map.pci[ 3 ] < CPthreadReqJoin_Pre_Joining_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          for (
            ctx->Map.pci[ 4 ] = CPthreadReqJoin_Pre_Canceled_Yes;
            ctx->Map.pci[ 4 ] < CPthreadReqJoin_Pre_Canceled_NA;
            ++ctx->Map.pci[ 4 ]
          ) {
            for (
              ctx->Map.pci[ 5 ] = CPthreadReqJoin_Pre_Restarted_Yes;
              ctx->Map.pci[ 5 ] < CPthreadReqJoin_Pre_Restarted_NA;
              ++ctx->Map.pci[ 5 ]
            ) {
              for (
                ctx->Map.pci[ 6 ] = CPthreadReqJoin_Pre_TargetRestarted_Yes;
                ctx->Map.pci[ 6 ] < CPthreadReqJoin_Pre_TargetRestarted_NA;
                ++ctx->Map.pci[ 6 ]
              ) {
                for (
                  ctx->Map.pci[ 7 ] = CPthreadReqJoin_Pre_ValuePtr_Valid;
                  ctx->Map.pci[ 7 ] < CPthreadReqJoin_Pre_ValuePtr_NA;
                  ++ctx->Map.pci[ 7 ]
                ) {
                  ctx->Map.entry = CPthreadReqJoin_PopEntry( ctx );

                  if ( ctx->Map.entry.Skip ) {
                    continue;
                  }

                  CPthreadReqJoin_SetPreConditionStates( ctx );
                  CPthreadReqJoin_Prepare( ctx );
                  CPthreadReqJoin_TestVariant( ctx );
                  CPthreadReqJoin_Cleanup( ctx );
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
