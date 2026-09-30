/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqDetach
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
 * @defgroup CPthreadReqDetach spec:/c/pthread/req/detach
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CPthreadReqDetach_Pre_Thread_Invalid,
  CPthreadReqDetach_Pre_Thread_Executing,
  CPthreadReqDetach_Pre_Thread_Other,
  CPthreadReqDetach_Pre_Thread_NA
} CPthreadReqDetach_Pre_Thread;

typedef enum {
  CPthreadReqDetach_Pre_Joinable_Yes,
  CPthreadReqDetach_Pre_Joinable_No,
  CPthreadReqDetach_Pre_Joinable_NA
} CPthreadReqDetach_Pre_Joinable;

typedef enum {
  CPthreadReqDetach_Pre_Terminated_Yes,
  CPthreadReqDetach_Pre_Terminated_No,
  CPthreadReqDetach_Pre_Terminated_NA
} CPthreadReqDetach_Pre_Terminated;

typedef enum {
  CPthreadReqDetach_Pre_Joined_Yes,
  CPthreadReqDetach_Pre_Joined_No,
  CPthreadReqDetach_Pre_Joined_NA
} CPthreadReqDetach_Pre_Joined;

typedef enum {
  CPthreadReqDetach_Pre_Restarted_Yes,
  CPthreadReqDetach_Pre_Restarted_No,
  CPthreadReqDetach_Pre_Restarted_NA
} CPthreadReqDetach_Pre_Restarted;

typedef enum {
  CPthreadReqDetach_Post_Status_Ok,
  CPthreadReqDetach_Post_Status_Inval,
  CPthreadReqDetach_Post_Status_Srch,
  CPthreadReqDetach_Post_Status_NA
} CPthreadReqDetach_Post_Status;

typedef enum {
  CPthreadReqDetach_Post_Target_Detached,
  CPthreadReqDetach_Post_Target_Reclaimed,
  CPthreadReqDetach_Post_Target_NA
} CPthreadReqDetach_Post_Target;

typedef enum {
  CPthreadReqDetach_Post_Reclaim_Yes,
  CPthreadReqDetach_Post_Reclaim_NA
} CPthreadReqDetach_Post_Reclaim;

typedef enum {
  CPthreadReqDetach_Post_Joiner_Joined,
  CPthreadReqDetach_Post_Joiner_NA
} CPthreadReqDetach_Post_Joiner;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Thread_NA : 1;
  uint16_t Pre_Joinable_NA : 1;
  uint16_t Pre_Terminated_NA : 1;
  uint16_t Pre_Joined_NA : 1;
  uint16_t Pre_Restarted_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_Target : 2;
  uint16_t Post_Reclaim : 1;
  uint16_t Post_Joiner : 1;
} CPthreadReqDetach_Entry;

typedef enum { THREAD_INVALID, THREAD_EXECUTING, THREAD_OTHER } ThreadKind;

/**
 * @brief Test context for spec:/c/pthread/req/detach test case.
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
   * @brief This member contains the identifier of the target task.
   */
  rtems_id target_id;

  /**
   * @brief This member contains the thread identifier of the target task.
   */
  pthread_t target_thread;

  /**
   * @brief This member contains the identifier of the joining task.
   */
  rtems_id joiner_id;

  /**
   * @brief This member contains the thread identifier of the joining task.
   */
  pthread_t joiner_thread;

  /**
   * @brief This member contains the identifier of the task which the call
   *   detaches.
   */
  rtems_id thread_id;

  /**
   * @brief This member specifies the thread which the call detaches.
   */
  ThreadKind thread_kind;

  /**
   * @brief This member is true, if the thread is a joinable thread.
   */
  bool joinable;

  /**
   * @brief This member is true, if the target task terminates before the call.
   */
  bool terminated;

  /**
   * @brief This member is true, if a task waits to join the thread.
   */
  bool joined;

  /**
   * @brief This member is true, if the thread is restarted before the call.
   */
  bool restarted;

  /**
   * @brief This member is true, if the calling task started.
   */
  bool caller_started;

  /**
   * @brief This member is true, if the target task started.
   */
  bool target_started;

  /**
   * @brief This member is true, if the calling task exited.
   */
  bool caller_exited;

  /**
   * @brief This member is true, if the target task exited.
   */
  bool target_exited;

  /**
   * @brief This member is true, if the joining task exited.
   */
  bool joiner_exited;

  /**
   * @brief This member is true, if the join of the joining task returned.
   */
  bool joiner_returned;

  /**
   * @brief This member is true, if the join of the joining task returned
   *   before the thread terminated.
   */
  bool joiner_returned_early;

  /**
   * @brief This member contains the return value of the join of the joining
   *   task.
   */
  int joiner_status;

  /**
   * @brief This member contains the value which the join of the joining task
   *   stored.
   */
  void *joiner_value;

  /**
   * @brief This member contains the return value of the pthread_detach() call.
   */
  int status;

  /**
   * @brief This member specifies the `thread` parameter value.
   */
  pthread_t thread;

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 5 ];

    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 5 ];

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
    CPthreadReqDetach_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqDetach_Context;

static CPthreadReqDetach_Context CPthreadReqDetach_Instance;

static const char *const CPthreadReqDetach_PreDesc_Thread[] =
  { "Invalid", "Executing", "Other", "NA" };

static const char *const CPthreadReqDetach_PreDesc_Joinable[] =
  { "Yes", "No", "NA" };

static const char *const CPthreadReqDetach_PreDesc_Terminated[] =
  { "Yes", "No", "NA" };

static const char *const CPthreadReqDetach_PreDesc_Joined[] =
  { "Yes", "No", "NA" };

static const char *const CPthreadReqDetach_PreDesc_Restarted[] =
  { "Yes", "No", "NA" };

static const char *const *const CPthreadReqDetach_PreDesc[] = {
  CPthreadReqDetach_PreDesc_Thread,
  CPthreadReqDetach_PreDesc_Joinable,
  CPthreadReqDetach_PreDesc_Terminated,
  CPthreadReqDetach_PreDesc_Joined,
  CPthreadReqDetach_PreDesc_Restarted,
  NULL
};

typedef CPthreadReqDetach_Context Context;

static int exit_object;

static void EndThread( Context *ctx )
{
  SendEvents( ctx->thread_id, RTEMS_EVENT_0 );
}

static void Caller( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;

  if ( !ctx->caller_started ) {
    ctx->caller_started = true;
    ctx->caller_thread = pthread_self();
  }

  (void) ReceiveAnyEvents();
  ctx->status = pthread_detach( ctx->thread );
  (void) ReceiveAnyEvents();
  ctx->caller_exited = true;
  pthread_exit( &exit_object );
}

static void Target( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;

  if ( !ctx->target_started ) {
    ctx->target_started = true;
    ctx->target_thread = pthread_self();
  }

  if ( !ctx->terminated ) {
    (void) ReceiveAnyEvents();
  }

  ctx->target_exited = true;
  pthread_exit( &exit_object );
}

static void Joiner( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;
  ctx->joiner_thread = pthread_self();
  ctx->joiner_status = pthread_join( ctx->thread, &ctx->joiner_value );
  ctx->joiner_returned = true;
  SuspendSelf();
  ctx->joiner_exited = true;
  pthread_exit( NULL );
}

static void CPthreadReqDetach_Pre_Thread_Prepare(
  CPthreadReqDetach_Context   *ctx,
  CPthreadReqDetach_Pre_Thread state
)
{
  switch ( state ) {
    case CPthreadReqDetach_Pre_Thread_Invalid: {
      /*
       * While the `thread` parameter is not associated with a thread.
       */
      ctx->thread_kind = THREAD_INVALID;
      break;
    }

    case CPthreadReqDetach_Pre_Thread_Executing: {
      /*
       * While the `thread` parameter is associated with the executing task.
       */
      ctx->thread_kind = THREAD_EXECUTING;
      break;
    }

    case CPthreadReqDetach_Pre_Thread_Other: {
      /*
       * While the `thread` parameter is associated with a thread other than
       * the executing task.
       */
      ctx->thread_kind = THREAD_OTHER;
      break;
    }

    case CPthreadReqDetach_Pre_Thread_NA:
      break;
  }
}

static void CPthreadReqDetach_Pre_Joinable_Prepare(
  CPthreadReqDetach_Context     *ctx,
  CPthreadReqDetach_Pre_Joinable state
)
{
  switch ( state ) {
    case CPthreadReqDetach_Pre_Joinable_Yes: {
      /*
       * While the thread specified by the `thread` parameter is a joinable
       * thread.
       */
      ctx->joinable = true;
      break;
    }

    case CPthreadReqDetach_Pre_Joinable_No: {
      /*
       * While the thread specified by the `thread` parameter is a detached
       * thread.
       */
      ctx->joinable = false;
      break;
    }

    case CPthreadReqDetach_Pre_Joinable_NA:
      break;
  }
}

static void CPthreadReqDetach_Pre_Terminated_Prepare(
  CPthreadReqDetach_Context       *ctx,
  CPthreadReqDetach_Pre_Terminated state
)
{
  switch ( state ) {
    case CPthreadReqDetach_Pre_Terminated_Yes: {
      /*
       * While the thread specified by the `thread` parameter is terminated.
       */
      ctx->terminated = true;
      break;
    }

    case CPthreadReqDetach_Pre_Terminated_No: {
      /*
       * While the thread specified by the `thread` parameter is not
       * terminated.
       */
      ctx->terminated = false;
      break;
    }

    case CPthreadReqDetach_Pre_Terminated_NA:
      break;
  }
}

static void CPthreadReqDetach_Pre_Joined_Prepare(
  CPthreadReqDetach_Context   *ctx,
  CPthreadReqDetach_Pre_Joined state
)
{
  switch ( state ) {
    case CPthreadReqDetach_Pre_Joined_Yes: {
      /*
       * While a thread waits to join the thread specified by the `thread`
       * parameter.
       */
      ctx->joined = true;
      break;
    }

    case CPthreadReqDetach_Pre_Joined_No: {
      /*
       * While no thread waits to join the thread specified by the `thread`
       * parameter.
       */
      ctx->joined = false;
      break;
    }

    case CPthreadReqDetach_Pre_Joined_NA:
      break;
  }
}

static void CPthreadReqDetach_Pre_Restarted_Prepare(
  CPthreadReqDetach_Context      *ctx,
  CPthreadReqDetach_Pre_Restarted state
)
{
  switch ( state ) {
    case CPthreadReqDetach_Pre_Restarted_Yes: {
      /*
       * While the thread specified by the `thread` parameter is restarted by
       * rtems_task_restart() before the call.
       */
      ctx->restarted = true;
      break;
    }

    case CPthreadReqDetach_Pre_Restarted_No: {
      /*
       * While the thread specified by the `thread` parameter is not restarted
       * before the call.
       */
      ctx->restarted = false;
      break;
    }

    case CPthreadReqDetach_Pre_Restarted_NA:
      break;
  }
}

static void CPthreadReqDetach_Post_Status_Check(
  CPthreadReqDetach_Context    *ctx,
  CPthreadReqDetach_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqDetach_Post_Status_Ok: {
      /*
       * The return value of pthread_detach() shall be zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CPthreadReqDetach_Post_Status_Inval: {
      /*
       * The return value of pthread_detach() shall be EINVAL.
       */
      T_eq_int( ctx->status, EINVAL );
      break;
    }

    case CPthreadReqDetach_Post_Status_Srch: {
      /*
       * The return value of pthread_detach() shall be ESRCH.
       */
      T_eq_int( ctx->status, ESRCH );
      break;
    }

    case CPthreadReqDetach_Post_Status_NA:
      break;
  }
}

static void CPthreadReqDetach_Post_Target_Check(
  CPthreadReqDetach_Context    *ctx,
  CPthreadReqDetach_Post_Target state
)
{
  switch ( state ) {
    case CPthreadReqDetach_Post_Target_Detached: {
      /*
       * The thread specified by the `thread` parameter shall be a detached
       * thread.
       */
      T_eq_int( pthread_join( ctx->thread, NULL ), EINVAL );
      break;
    }

    case CPthreadReqDetach_Post_Target_Reclaimed: {
      /*
       * The identifier specified by the `thread` parameter shall be associated
       * with no thread.
       */
      T_eq_int( pthread_join( ctx->thread, NULL ), ESRCH );
      break;
    }

    case CPthreadReqDetach_Post_Target_NA:
      break;
  }
}

static void CPthreadReqDetach_Post_Reclaim_Check(
  CPthreadReqDetach_Context     *ctx,
  CPthreadReqDetach_Post_Reclaim state
)
{
  switch ( state ) {
    case CPthreadReqDetach_Post_Reclaim_Yes: {
      /*
       * The identifier specified by the `thread` parameter shall be associated
       * with no thread after the termination of the thread.
       */
      EndThread( ctx );
      T_eq_int( pthread_join( ctx->thread, NULL ), ESRCH );
      break;
    }

    case CPthreadReqDetach_Post_Reclaim_NA:
      break;
  }
}

static void CPthreadReqDetach_Post_Joiner_Check(
  CPthreadReqDetach_Context    *ctx,
  CPthreadReqDetach_Post_Joiner state
)
{
  switch ( state ) {
    case CPthreadReqDetach_Post_Joiner_Joined: {
      /*
       * The thread which waits to join the thread specified by the `thread`
       * parameter shall be joined with that thread.
       */
      T_false( ctx->joiner_returned_early );
      T_true( ctx->joiner_returned );
      T_eq_int( ctx->joiner_status, 0 );
      T_eq_ptr( ctx->joiner_value, &exit_object );
      break;
    }

    case CPthreadReqDetach_Post_Joiner_NA:
      break;
  }
}

static void CPthreadReqDetach_Setup( void )
{
  SetSelfPriority( PRIO_NORMAL );
}

static void CPthreadReqDetach_Setup_Wrap( void *arg )
{
  CPthreadReqDetach_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CPthreadReqDetach_Setup();
}

static void CPthreadReqDetach_Teardown( void )
{
  RestoreRunnerPriority();
}

static void CPthreadReqDetach_Teardown_Wrap( void *arg )
{
  CPthreadReqDetach_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CPthreadReqDetach_Teardown();
}

static void CPthreadReqDetach_Prepare( CPthreadReqDetach_Context *ctx )
{
  ctx->joinable = true;
  ctx->terminated = false;
  ctx->joined = false;
  ctx->restarted = false;
  ctx->caller_started = false;
  ctx->target_started = false;
  ctx->joiner_id = 0;
  ctx->caller_exited = false;
  ctx->target_exited = false;
  ctx->joiner_exited = false;
  ctx->joiner_returned = false;
  ctx->joiner_returned_early = false;
  ctx->joiner_status = -1;
  ctx->joiner_value = NULL;
  ctx->status = -1;
}

static void CPthreadReqDetach_Action( CPthreadReqDetach_Context *ctx )
{
  ctx->caller_id = CreateTask( "CALL", PRIO_HIGH );
  StartTask( ctx->caller_id, Caller, ctx );
  ctx->target_id = CreateTask( "TARG", PRIO_HIGH );
  StartTask( ctx->target_id, Target, ctx );

  switch ( ctx->thread_kind ) {
    case THREAD_INVALID:
      ctx->thread = INVALID_ID;
      break;
    case THREAD_EXECUTING:
      ctx->thread = ctx->caller_thread;
      ctx->thread_id = ctx->caller_id;
      break;
    default:
      ctx->thread = ctx->target_thread;
      ctx->thread_id = ctx->target_id;
      break;
  }

  if ( ctx->joined ) {
    ctx->joiner_id = CreateTask( "JOIN", PRIO_HIGH );
    StartTask( ctx->joiner_id, Joiner, ctx );
  }

  if ( !ctx->joinable ) {
    T_eq_int( pthread_detach( ctx->thread ), 0 );
  }

  if ( ctx->restarted ) {
    RestartTask( ctx->thread_id, ctx );
  }

  SendEvents( ctx->caller_id, RTEMS_EVENT_0 );
  ctx->joiner_returned_early = ctx->joiner_returned;
}

static void CPthreadReqDetach_Cleanup( CPthreadReqDetach_Context *ctx )
{
  (void) pthread_detach( ctx->caller_thread );
  (void) pthread_detach( ctx->target_thread );

  if ( !ctx->caller_exited ) {
    SendEvents( ctx->caller_id, RTEMS_EVENT_0 );
  }

  if ( !ctx->target_exited ) {
    SendEvents( ctx->target_id, RTEMS_EVENT_0 );
  }

  if ( ctx->joiner_id != 0 ) {
    (void) pthread_detach( ctx->joiner_thread );

    if ( !ctx->joiner_exited ) {
      ResumeTask( ctx->joiner_id );
    }
  }
}

/* clang-format off */

static const CPthreadReqDetach_Entry
CPthreadReqDetach_Entries[] = {
  { 0, 0, 1, 1, 1, 1, CPthreadReqDetach_Post_Status_Srch,
    CPthreadReqDetach_Post_Target_NA, CPthreadReqDetach_Post_Reclaim_NA,
    CPthreadReqDetach_Post_Joiner_NA },
  { 0, 0, 0, 1, 0, 0, CPthreadReqDetach_Post_Status_Ok,
    CPthreadReqDetach_Post_Target_Detached, CPthreadReqDetach_Post_Reclaim_Yes,
    CPthreadReqDetach_Post_Joiner_Joined },
  { 0, 0, 0, 1, 0, 0, CPthreadReqDetach_Post_Status_Ok,
    CPthreadReqDetach_Post_Target_Detached, CPthreadReqDetach_Post_Reclaim_Yes,
    CPthreadReqDetach_Post_Joiner_NA },
  { 0, 0, 0, 1, 0, 0, CPthreadReqDetach_Post_Status_Inval,
    CPthreadReqDetach_Post_Target_Detached, CPthreadReqDetach_Post_Reclaim_Yes,
    CPthreadReqDetach_Post_Joiner_Joined },
  { 0, 0, 0, 1, 0, 0, CPthreadReqDetach_Post_Status_Inval,
    CPthreadReqDetach_Post_Target_Detached, CPthreadReqDetach_Post_Reclaim_Yes,
    CPthreadReqDetach_Post_Joiner_NA },
  { 1, 0, 0, 0, 0, 0, CPthreadReqDetach_Post_Status_NA,
    CPthreadReqDetach_Post_Target_NA, CPthreadReqDetach_Post_Reclaim_NA,
    CPthreadReqDetach_Post_Joiner_NA },
  { 1, 0, 0, 0, 0, 0, CPthreadReqDetach_Post_Status_NA,
    CPthreadReqDetach_Post_Target_NA, CPthreadReqDetach_Post_Reclaim_NA,
    CPthreadReqDetach_Post_Joiner_NA },
  { 0, 0, 0, 0, 0, 0, CPthreadReqDetach_Post_Status_Ok,
    CPthreadReqDetach_Post_Target_Reclaimed, CPthreadReqDetach_Post_Reclaim_NA,
    CPthreadReqDetach_Post_Joiner_NA },
  { 0, 0, 0, 0, 0, 0, CPthreadReqDetach_Post_Status_Ok,
    CPthreadReqDetach_Post_Target_Detached, CPthreadReqDetach_Post_Reclaim_Yes,
    CPthreadReqDetach_Post_Joiner_Joined },
  { 0, 0, 0, 0, 0, 0, CPthreadReqDetach_Post_Status_Ok,
    CPthreadReqDetach_Post_Target_Detached, CPthreadReqDetach_Post_Reclaim_Yes,
    CPthreadReqDetach_Post_Joiner_NA },
  { 0, 0, 0, 0, 0, 0, CPthreadReqDetach_Post_Status_Inval,
    CPthreadReqDetach_Post_Target_Detached, CPthreadReqDetach_Post_Reclaim_Yes,
    CPthreadReqDetach_Post_Joiner_Joined },
  { 0, 0, 0, 0, 0, 0, CPthreadReqDetach_Post_Status_Inval,
    CPthreadReqDetach_Post_Target_Detached, CPthreadReqDetach_Post_Reclaim_Yes,
    CPthreadReqDetach_Post_Joiner_NA }
};

static const uint8_t
CPthreadReqDetach_Map[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 1, 1, 2, 2, 3, 3,
  4, 4, 3, 3, 4, 4, 6, 6, 7, 7, 8, 8, 9, 9, 5, 5, 5, 5, 10, 10, 11, 11
};

/* clang-format on */

static size_t CPthreadReqDetach_Scope( void *arg, char *buf, size_t n )
{
  CPthreadReqDetach_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CPthreadReqDetach_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CPthreadReqDetach_Fixture = {
  .setup = CPthreadReqDetach_Setup_Wrap,
  .stop = NULL,
  .teardown = CPthreadReqDetach_Teardown_Wrap,
  .scope = CPthreadReqDetach_Scope,
  .initial_context = &CPthreadReqDetach_Instance
};

static inline CPthreadReqDetach_Entry CPthreadReqDetach_PopEntry(
  CPthreadReqDetach_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqDetach_Entries[ CPthreadReqDetach_Map[ index ] ];
}

static void CPthreadReqDetach_SetPreConditionStates(
  CPthreadReqDetach_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Joinable_NA ) {
    ctx->Map.pcs[ 1 ] = CPthreadReqDetach_Pre_Joinable_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  if ( ctx->Map.entry.Pre_Terminated_NA ) {
    ctx->Map.pcs[ 2 ] = CPthreadReqDetach_Pre_Terminated_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }

  if ( ctx->Map.entry.Pre_Joined_NA ) {
    ctx->Map.pcs[ 3 ] = CPthreadReqDetach_Pre_Joined_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }

  if ( ctx->Map.entry.Pre_Restarted_NA ) {
    ctx->Map.pcs[ 4 ] = CPthreadReqDetach_Pre_Restarted_NA;
  } else {
    ctx->Map.pcs[ 4 ] = ctx->Map.pci[ 4 ];
  }
}

static void CPthreadReqDetach_TestVariant( CPthreadReqDetach_Context *ctx )
{
  CPthreadReqDetach_Pre_Thread_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqDetach_Pre_Joinable_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqDetach_Pre_Terminated_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CPthreadReqDetach_Pre_Joined_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CPthreadReqDetach_Pre_Restarted_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  CPthreadReqDetach_Action( ctx );
  CPthreadReqDetach_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CPthreadReqDetach_Post_Target_Check( ctx, ctx->Map.entry.Post_Target );
  CPthreadReqDetach_Post_Reclaim_Check( ctx, ctx->Map.entry.Post_Reclaim );
  CPthreadReqDetach_Post_Joiner_Check( ctx, ctx->Map.entry.Post_Joiner );
}

/**
 * @fn void T_case_body_CPthreadReqDetach( void )
 */
T_TEST_CASE_FIXTURE( CPthreadReqDetach, &CPthreadReqDetach_Fixture )
{
  CPthreadReqDetach_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CPthreadReqDetach_Pre_Thread_Invalid;
    ctx->Map.pci[ 0 ] < CPthreadReqDetach_Pre_Thread_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CPthreadReqDetach_Pre_Joinable_Yes;
      ctx->Map.pci[ 1 ] < CPthreadReqDetach_Pre_Joinable_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CPthreadReqDetach_Pre_Terminated_Yes;
        ctx->Map.pci[ 2 ] < CPthreadReqDetach_Pre_Terminated_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = CPthreadReqDetach_Pre_Joined_Yes;
          ctx->Map.pci[ 3 ] < CPthreadReqDetach_Pre_Joined_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          for (
            ctx->Map.pci[ 4 ] = CPthreadReqDetach_Pre_Restarted_Yes;
            ctx->Map.pci[ 4 ] < CPthreadReqDetach_Pre_Restarted_NA;
            ++ctx->Map.pci[ 4 ]
          ) {
            ctx->Map.entry = CPthreadReqDetach_PopEntry( ctx );

            if ( ctx->Map.entry.Skip ) {
              continue;
            }

            CPthreadReqDetach_SetPreConditionStates( ctx );
            CPthreadReqDetach_Prepare( ctx );
            CPthreadReqDetach_TestVariant( ctx );
            CPthreadReqDetach_Cleanup( ctx );
          }
        }
      }
    }
  }
}

/** @} */
