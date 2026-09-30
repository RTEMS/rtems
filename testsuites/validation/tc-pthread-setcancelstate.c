/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqSetcancelstate
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
 * @defgroup CPthreadReqSetcancelstate spec:/c/pthread/req/setcancelstate
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CPthreadReqSetcancelstate_Pre_Context_Task,
  CPthreadReqSetcancelstate_Pre_Context_Interrupt,
  CPthreadReqSetcancelstate_Pre_Context_NA
} CPthreadReqSetcancelstate_Pre_Context;

typedef enum {
  CPthreadReqSetcancelstate_Pre_State_Enable,
  CPthreadReqSetcancelstate_Pre_State_Disable,
  CPthreadReqSetcancelstate_Pre_State_Invalid,
  CPthreadReqSetcancelstate_Pre_State_NA
} CPthreadReqSetcancelstate_Pre_State;

typedef enum {
  CPthreadReqSetcancelstate_Pre_OldState_Valid,
  CPthreadReqSetcancelstate_Pre_OldState_Null,
  CPthreadReqSetcancelstate_Pre_OldState_NA
} CPthreadReqSetcancelstate_Pre_OldState;

typedef enum {
  CPthreadReqSetcancelstate_Pre_CancelState_Enable,
  CPthreadReqSetcancelstate_Pre_CancelState_Disable,
  CPthreadReqSetcancelstate_Pre_CancelState_NA
} CPthreadReqSetcancelstate_Pre_CancelState;

typedef enum {
  CPthreadReqSetcancelstate_Pre_CancelType_Deferred,
  CPthreadReqSetcancelstate_Pre_CancelType_Asynchronous,
  CPthreadReqSetcancelstate_Pre_CancelType_NA
} CPthreadReqSetcancelstate_Pre_CancelType;

typedef enum {
  CPthreadReqSetcancelstate_Pre_CancelPending_Yes,
  CPthreadReqSetcancelstate_Pre_CancelPending_No,
  CPthreadReqSetcancelstate_Pre_CancelPending_NA
} CPthreadReqSetcancelstate_Pre_CancelPending;

typedef enum {
  CPthreadReqSetcancelstate_Pre_RestartPending_Yes,
  CPthreadReqSetcancelstate_Pre_RestartPending_No,
  CPthreadReqSetcancelstate_Pre_RestartPending_NA
} CPthreadReqSetcancelstate_Pre_RestartPending;

typedef enum {
  CPthreadReqSetcancelstate_Post_Status_Ok,
  CPthreadReqSetcancelstate_Post_Status_Inval,
  CPthreadReqSetcancelstate_Post_Status_Proto,
  CPthreadReqSetcancelstate_Post_Status_NA
} CPthreadReqSetcancelstate_Post_Status;

typedef enum {
  CPthreadReqSetcancelstate_Post_CancelState_Enable,
  CPthreadReqSetcancelstate_Post_CancelState_Disable,
  CPthreadReqSetcancelstate_Post_CancelState_Nop,
  CPthreadReqSetcancelstate_Post_CancelState_NA
} CPthreadReqSetcancelstate_Post_CancelState;

typedef enum {
  CPthreadReqSetcancelstate_Post_OldState_Set,
  CPthreadReqSetcancelstate_Post_OldState_Nop,
  CPthreadReqSetcancelstate_Post_OldState_NA
} CPthreadReqSetcancelstate_Post_OldState;

typedef enum {
  CPthreadReqSetcancelstate_Post_CancelType_Nop,
  CPthreadReqSetcancelstate_Post_CancelType_NA
} CPthreadReqSetcancelstate_Post_CancelType;

typedef enum {
  CPthreadReqSetcancelstate_Post_Life_Nop,
  CPthreadReqSetcancelstate_Post_Life_Terminate,
  CPthreadReqSetcancelstate_Post_Life_Restart,
  CPthreadReqSetcancelstate_Post_Life_NA
} CPthreadReqSetcancelstate_Post_Life;

typedef enum {
  CPthreadReqSetcancelstate_Post_Return_Yes,
  CPthreadReqSetcancelstate_Post_Return_No,
  CPthreadReqSetcancelstate_Post_Return_NA
} CPthreadReqSetcancelstate_Post_Return;

typedef enum {
  CPthreadReqSetcancelstate_Post_ExitValue_Canceled,
  CPthreadReqSetcancelstate_Post_ExitValue_NA
} CPthreadReqSetcancelstate_Post_ExitValue;

typedef enum {
  CPthreadReqSetcancelstate_Post_Requests_Nop,
  CPthreadReqSetcancelstate_Post_Requests_NA
} CPthreadReqSetcancelstate_Post_Requests;

typedef struct {
  uint32_t Skip : 1;
  uint32_t Pre_Context_NA : 1;
  uint32_t Pre_State_NA : 1;
  uint32_t Pre_OldState_NA : 1;
  uint32_t Pre_CancelState_NA : 1;
  uint32_t Pre_CancelType_NA : 1;
  uint32_t Pre_CancelPending_NA : 1;
  uint32_t Pre_RestartPending_NA : 1;
  uint32_t Post_Status : 2;
  uint32_t Post_CancelState : 2;
  uint32_t Post_OldState : 2;
  uint32_t Post_CancelType : 1;
  uint32_t Post_Life : 2;
  uint32_t Post_Return : 2;
  uint32_t Post_ExitValue : 1;
  uint32_t Post_Requests : 1;
} CPthreadReqSetcancelstate_Entry;

typedef enum { PHASE_ACTION, PHASE_LATER, PHASE_CLEANUP } Phase;

/**
 * @brief Test context for spec:/c/pthread/req/setcancelstate test case.
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
   * @brief This member is true, if the worker task started.
   */
  bool worker_started;

  /**
   * @brief This member is true, if the test joined the worker task.
   */
  bool joined;

  /**
   * @brief This member contains the thread exit value of the worker task.
   */
  void *exit_value;

  /**
   * @brief This member contains the count of restarts of the worker task in
   *   the action phase.
   */
  uint32_t restarts;

  /**
   * @brief This member is true, if the call returned to the caller.
   */
  bool returned;

  /**
   * @brief This member is true, if the enabled asynchronous cancellation of
   *   the later phase returned to the worker task.
   */
  bool late_returned;

  /**
   * @brief This member is true, if the call is made from within interrupt
   *   context.
   */
  bool in_interrupt;

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
   * @brief This member contains the thread cancel state attribute after the
   *   call.
   */
  int state_after;

  /**
   * @brief This member contains the thread cancel type attribute after the
   *   call.
   */
  int type_after;

  /**
   * @brief This member provides the object referenced by the `oldstate`
   *   parameter.
   */
  int oldstate_obj;

  /**
   * @brief This member contains the return value of the
   *   pthread_setcancelstate() call.
   */
  int status;

  /**
   * @brief This member specifies the `state` parameter value.
   */
  int state;

  /**
   * @brief This member specifies the `oldstate` parameter value.
   */
  int *oldstate;

  struct {
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
    CPthreadReqSetcancelstate_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqSetcancelstate_Context;

static CPthreadReqSetcancelstate_Context CPthreadReqSetcancelstate_Instance;

static const char *const CPthreadReqSetcancelstate_PreDesc_Context[] =
  { "Task", "Interrupt", "NA" };

static const char *const CPthreadReqSetcancelstate_PreDesc_State[] =
  { "Enable", "Disable", "Invalid", "NA" };

static const char *const CPthreadReqSetcancelstate_PreDesc_OldState[] =
  { "Valid", "Null", "NA" };

static const char *const CPthreadReqSetcancelstate_PreDesc_CancelState[] =
  { "Enable", "Disable", "NA" };

static const char *const CPthreadReqSetcancelstate_PreDesc_CancelType[] =
  { "Deferred", "Asynchronous", "NA" };

static const char *const CPthreadReqSetcancelstate_PreDesc_CancelPending[] =
  { "Yes", "No", "NA" };

static const char *const CPthreadReqSetcancelstate_PreDesc_RestartPending[] =
  { "Yes", "No", "NA" };

static const char *const *const CPthreadReqSetcancelstate_PreDesc[] = {
  CPthreadReqSetcancelstate_PreDesc_Context,
  CPthreadReqSetcancelstate_PreDesc_State,
  CPthreadReqSetcancelstate_PreDesc_OldState,
  CPthreadReqSetcancelstate_PreDesc_CancelState,
  CPthreadReqSetcancelstate_PreDesc_CancelType,
  CPthreadReqSetcancelstate_PreDesc_CancelPending,
  CPthreadReqSetcancelstate_PreDesc_RestartPending,
  NULL
};

typedef CPthreadReqSetcancelstate_Context Context;

static int exit_object;

static void Action( void *arg )
{
  Context *ctx;

  ctx = arg;
  ctx->status = pthread_setcancelstate( ctx->state, ctx->oldstate );
}

static void JoinWorker( Context *ctx )
{
  bool suspended;

  suspended = IsTaskSuspended( ctx->worker_id );
  T_false( suspended );

  if ( suspended ) {
    ctx->phase = PHASE_CLEANUP;
    ResumeTask( ctx->worker_id );
  }

  T_eq_int( pthread_join( ctx->worker_thread, &ctx->exit_value ), 0 );
  ctx->joined = true;
}

static void Worker( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;

  if ( ctx->worker_started ) {
    ++ctx->restarts;
    pthread_exit( &exit_object );
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

  if ( ctx->in_interrupt ) {
    CallWithinISR( Action, ctx );
  } else {
    Action( ctx );
  }

  ctx->returned = true;
  DisableCancelability( &ctx->state_after, &ctx->type_after );
  SuspendSelf();
  SetCancelability( PTHREAD_CANCEL_ENABLE, PTHREAD_CANCEL_ASYNCHRONOUS );

  if ( ctx->phase == PHASE_LATER ) {
    ctx->late_returned = true;
  }

  pthread_exit( &exit_object );
}

static void CPthreadReqSetcancelstate_Pre_Context_Prepare(
  CPthreadReqSetcancelstate_Context    *ctx,
  CPthreadReqSetcancelstate_Pre_Context state
)
{
  switch ( state ) {
    case CPthreadReqSetcancelstate_Pre_Context_Task: {
      /*
       * While pthread_setcancelstate() is called from within task context.
       */
      ctx->in_interrupt = false;
      break;
    }

    case CPthreadReqSetcancelstate_Pre_Context_Interrupt: {
      /*
       * While pthread_setcancelstate() is called from within interrupt
       * context.
       */
      ctx->in_interrupt = true;
      break;
    }

    case CPthreadReqSetcancelstate_Pre_Context_NA:
      break;
  }
}

static void CPthreadReqSetcancelstate_Pre_State_Prepare(
  CPthreadReqSetcancelstate_Context  *ctx,
  CPthreadReqSetcancelstate_Pre_State state
)
{
  switch ( state ) {
    case CPthreadReqSetcancelstate_Pre_State_Enable: {
      /*
       * While the `state` parameter is equal to PTHREAD_CANCEL_ENABLE.
       */
      ctx->state = PTHREAD_CANCEL_ENABLE;
      break;
    }

    case CPthreadReqSetcancelstate_Pre_State_Disable: {
      /*
       * While the `state` parameter is equal to PTHREAD_CANCEL_DISABLE.
       */
      ctx->state = PTHREAD_CANCEL_DISABLE;
      break;
    }

    case CPthreadReqSetcancelstate_Pre_State_Invalid: {
      /*
       * While the `state` parameter is neither equal to PTHREAD_CANCEL_ENABLE
       * nor equal to PTHREAD_CANCEL_DISABLE.
       */
      ctx->state = -1;
      break;
    }

    case CPthreadReqSetcancelstate_Pre_State_NA:
      break;
  }
}

static void CPthreadReqSetcancelstate_Pre_OldState_Prepare(
  CPthreadReqSetcancelstate_Context     *ctx,
  CPthreadReqSetcancelstate_Pre_OldState state
)
{
  switch ( state ) {
    case CPthreadReqSetcancelstate_Pre_OldState_Valid: {
      /*
       * While the `oldstate` parameter references an object of type int.
       */
      ctx->oldstate = &ctx->oldstate_obj;
      break;
    }

    case CPthreadReqSetcancelstate_Pre_OldState_Null: {
      /*
       * While the `oldstate` parameter is equal to NULL.
       */
      ctx->oldstate = NULL;
      break;
    }

    case CPthreadReqSetcancelstate_Pre_OldState_NA:
      break;
  }
}

static void CPthreadReqSetcancelstate_Pre_CancelState_Prepare(
  CPthreadReqSetcancelstate_Context        *ctx,
  CPthreadReqSetcancelstate_Pre_CancelState state
)
{
  switch ( state ) {
    case CPthreadReqSetcancelstate_Pre_CancelState_Enable: {
      /*
       * While the thread cancel state attribute of the executing task is
       * PTHREAD_CANCEL_ENABLE.
       */
      ctx->cancel_state = PTHREAD_CANCEL_ENABLE;
      break;
    }

    case CPthreadReqSetcancelstate_Pre_CancelState_Disable: {
      /*
       * While the thread cancel state attribute of the executing task is
       * PTHREAD_CANCEL_DISABLE.
       */
      ctx->cancel_state = PTHREAD_CANCEL_DISABLE;
      break;
    }

    case CPthreadReqSetcancelstate_Pre_CancelState_NA:
      break;
  }
}

static void CPthreadReqSetcancelstate_Pre_CancelType_Prepare(
  CPthreadReqSetcancelstate_Context       *ctx,
  CPthreadReqSetcancelstate_Pre_CancelType state
)
{
  switch ( state ) {
    case CPthreadReqSetcancelstate_Pre_CancelType_Deferred: {
      /*
       * While the thread cancel type attribute of the executing task is
       * PTHREAD_CANCEL_DEFERRED.
       */
      ctx->cancel_type = PTHREAD_CANCEL_DEFERRED;
      break;
    }

    case CPthreadReqSetcancelstate_Pre_CancelType_Asynchronous: {
      /*
       * While the thread cancel type attribute of the executing task is
       * PTHREAD_CANCEL_ASYNCHRONOUS.
       */
      ctx->cancel_type = PTHREAD_CANCEL_ASYNCHRONOUS;
      break;
    }

    case CPthreadReqSetcancelstate_Pre_CancelType_NA:
      break;
  }
}

static void CPthreadReqSetcancelstate_Pre_CancelPending_Prepare(
  CPthreadReqSetcancelstate_Context          *ctx,
  CPthreadReqSetcancelstate_Pre_CancelPending state
)
{
  switch ( state ) {
    case CPthreadReqSetcancelstate_Pre_CancelPending_Yes: {
      /*
       * While a thread cancellation request is pending for the executing task.
       */
      ctx->cancel_pending = true;
      break;
    }

    case CPthreadReqSetcancelstate_Pre_CancelPending_No: {
      /*
       * While no thread cancellation request is pending for the executing
       * task.
       */
      ctx->cancel_pending = false;
      break;
    }

    case CPthreadReqSetcancelstate_Pre_CancelPending_NA:
      break;
  }
}

static void CPthreadReqSetcancelstate_Pre_RestartPending_Prepare(
  CPthreadReqSetcancelstate_Context           *ctx,
  CPthreadReqSetcancelstate_Pre_RestartPending state
)
{
  switch ( state ) {
    case CPthreadReqSetcancelstate_Pre_RestartPending_Yes: {
      /*
       * While a restart of the executing task by rtems_task_restart() is
       * pending.
       */
      ctx->restart_pending = true;
      break;
    }

    case CPthreadReqSetcancelstate_Pre_RestartPending_No: {
      /*
       * While no restart of the executing task is pending.
       */
      ctx->restart_pending = false;
      break;
    }

    case CPthreadReqSetcancelstate_Pre_RestartPending_NA:
      break;
  }
}

static void CPthreadReqSetcancelstate_Post_Status_Check(
  CPthreadReqSetcancelstate_Context    *ctx,
  CPthreadReqSetcancelstate_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqSetcancelstate_Post_Status_Ok: {
      /*
       * The return value of pthread_setcancelstate() shall be zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CPthreadReqSetcancelstate_Post_Status_Inval: {
      /*
       * The return value of pthread_setcancelstate() shall be EINVAL.
       */
      T_eq_int( ctx->status, EINVAL );
      break;
    }

    case CPthreadReqSetcancelstate_Post_Status_Proto: {
      /*
       * The return value of pthread_setcancelstate() shall be EPROTO.
       */
      T_eq_int( ctx->status, EPROTO );
      break;
    }

    case CPthreadReqSetcancelstate_Post_Status_NA:
      break;
  }
}

static void CPthreadReqSetcancelstate_Post_CancelState_Check(
  CPthreadReqSetcancelstate_Context         *ctx,
  CPthreadReqSetcancelstate_Post_CancelState state
)
{
  switch ( state ) {
    case CPthreadReqSetcancelstate_Post_CancelState_Enable: {
      /*
       * The thread cancel state attribute of the executing task shall be
       * PTHREAD_CANCEL_ENABLE.
       */
      T_eq_int( ctx->state_after, PTHREAD_CANCEL_ENABLE );
      break;
    }

    case CPthreadReqSetcancelstate_Post_CancelState_Disable: {
      /*
       * The thread cancel state attribute of the executing task shall be
       * PTHREAD_CANCEL_DISABLE.
       */
      T_eq_int( ctx->state_after, PTHREAD_CANCEL_DISABLE );
      break;
    }

    case CPthreadReqSetcancelstate_Post_CancelState_Nop: {
      /*
       * The thread cancel state attribute of the executing task shall not be
       * modified by the pthread_setcancelstate() call.
       */
      T_eq_int( ctx->state_after, ctx->cancel_state );
      break;
    }

    case CPthreadReqSetcancelstate_Post_CancelState_NA:
      break;
  }
}

static void CPthreadReqSetcancelstate_Post_OldState_Check(
  CPthreadReqSetcancelstate_Context      *ctx,
  CPthreadReqSetcancelstate_Post_OldState state
)
{
  switch ( state ) {
    case CPthreadReqSetcancelstate_Post_OldState_Set: {
      /*
       * The value of the object referenced by the `oldstate` parameter shall
       * be set to the thread cancel state attribute of the executing task
       * before the call.
       */
      T_eq_int( ctx->oldstate_obj, ctx->cancel_state );
      break;
    }

    case CPthreadReqSetcancelstate_Post_OldState_Nop: {
      /*
       * The object referenced by the `oldstate` parameter shall not be
       * modified by the pthread_setcancelstate() call.
       */
      T_eq_int( ctx->oldstate_obj, -1 );
      break;
    }

    case CPthreadReqSetcancelstate_Post_OldState_NA:
      break;
  }
}

static void CPthreadReqSetcancelstate_Post_CancelType_Check(
  CPthreadReqSetcancelstate_Context        *ctx,
  CPthreadReqSetcancelstate_Post_CancelType state
)
{
  switch ( state ) {
    case CPthreadReqSetcancelstate_Post_CancelType_Nop: {
      /*
       * The thread cancel type attribute of the executing task shall not be
       * modified by the pthread_setcancelstate() call.
       */
      T_eq_int( ctx->type_after, ctx->cancel_type );
      break;
    }

    case CPthreadReqSetcancelstate_Post_CancelType_NA:
      break;
  }
}

static void CPthreadReqSetcancelstate_Post_Life_Check(
  CPthreadReqSetcancelstate_Context  *ctx,
  CPthreadReqSetcancelstate_Post_Life state
)
{
  switch ( state ) {
    case CPthreadReqSetcancelstate_Post_Life_Nop: {
      /*
       * The executing task shall be neither terminated nor restarted by the
       * pthread_setcancelstate() call.
       */
      T_true( IsTaskSuspended( ctx->worker_id ) );
      T_eq_u32( ctx->restarts, 0 );
      break;
    }

    case CPthreadReqSetcancelstate_Post_Life_Terminate: {
      /*
       * The executing task shall be terminated by a thread cancellation.
       */
      JoinWorker( ctx );
      T_eq_u32( ctx->restarts, 0 );
      break;
    }

    case CPthreadReqSetcancelstate_Post_Life_Restart: {
      /*
       * The executing task shall be restarted.
       */
      JoinWorker( ctx );
      T_eq_ptr( ctx->exit_value, &exit_object );
      T_eq_u32( ctx->restarts, 1 );
      break;
    }

    case CPthreadReqSetcancelstate_Post_Life_NA:
      break;
  }
}

static void CPthreadReqSetcancelstate_Post_Return_Check(
  CPthreadReqSetcancelstate_Context    *ctx,
  CPthreadReqSetcancelstate_Post_Return state
)
{
  switch ( state ) {
    case CPthreadReqSetcancelstate_Post_Return_Yes: {
      /*
       * The pthread_setcancelstate() call shall return to the caller.
       */
      T_true( ctx->returned );
      break;
    }

    case CPthreadReqSetcancelstate_Post_Return_No: {
      /*
       * The pthread_setcancelstate() call shall not return to the caller.
       */
      T_false( ctx->returned );
      break;
    }

    case CPthreadReqSetcancelstate_Post_Return_NA:
      break;
  }
}

static void CPthreadReqSetcancelstate_Post_ExitValue_Check(
  CPthreadReqSetcancelstate_Context       *ctx,
  CPthreadReqSetcancelstate_Post_ExitValue state
)
{
  switch ( state ) {
    case CPthreadReqSetcancelstate_Post_ExitValue_Canceled: {
      /*
       * The thread exit value of the executing task shall be PTHREAD_CANCELED.
       */
      T_eq_ptr( ctx->exit_value, PTHREAD_CANCELED );
      break;
    }

    case CPthreadReqSetcancelstate_Post_ExitValue_NA:
      break;
  }
}

static void CPthreadReqSetcancelstate_Post_Requests_Check(
  CPthreadReqSetcancelstate_Context      *ctx,
  CPthreadReqSetcancelstate_Post_Requests state
)
{
  switch ( state ) {
    case CPthreadReqSetcancelstate_Post_Requests_Nop: {
      /*
       * The pending thread cancellation request and the pending restart by
       * rtems_task_restart() of the executing task shall not be modified by
       * the pthread_setcancelstate() call.
       */
      ctx->phase = PHASE_LATER;
      ResumeTask( ctx->worker_id );
      T_eq_int( pthread_join( ctx->worker_thread, &ctx->exit_value ), 0 );
      ctx->joined = true;

      if ( ctx->cancel_pending ) {
        T_eq_ptr( ctx->exit_value, PTHREAD_CANCELED );
        T_false( ctx->late_returned );
        T_eq_u32( ctx->restarts, 0 );
      } else if ( ctx->restart_pending ) {
        T_eq_ptr( ctx->exit_value, &exit_object );
        T_eq_u32( ctx->restarts, 1 );
      } else {
        T_eq_ptr( ctx->exit_value, &exit_object );
        T_true( ctx->late_returned );
        T_eq_u32( ctx->restarts, 0 );
      }
      break;
    }

    case CPthreadReqSetcancelstate_Post_Requests_NA:
      break;
  }
}

static void CPthreadReqSetcancelstate_Setup( void )
{
  SetSelfPriority( PRIO_NORMAL );
}

static void CPthreadReqSetcancelstate_Setup_Wrap( void *arg )
{
  CPthreadReqSetcancelstate_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CPthreadReqSetcancelstate_Setup();
}

static void CPthreadReqSetcancelstate_Teardown( void )
{
  RestoreRunnerPriority();
}

static void CPthreadReqSetcancelstate_Teardown_Wrap( void *arg )
{
  CPthreadReqSetcancelstate_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CPthreadReqSetcancelstate_Teardown();
}

static void CPthreadReqSetcancelstate_Prepare(
  CPthreadReqSetcancelstate_Context *ctx
)
{
  ctx->phase = PHASE_ACTION;
  ctx->worker_started = false;
  ctx->joined = false;
  ctx->restarts = 0;
  ctx->returned = false;
  ctx->late_returned = false;
  ctx->exit_value = NULL;
  ctx->status = -1;
  ctx->oldstate_obj = -1;
  ctx->state_after = -1;
  ctx->type_after = -1;
}

static void CPthreadReqSetcancelstate_Action(
  CPthreadReqSetcancelstate_Context *ctx
)
{
  ctx->worker_id = CreateTask( "WORK", PRIO_HIGH );
  StartTask( ctx->worker_id, Worker, ctx );
}

static void CPthreadReqSetcancelstate_Cleanup(
  CPthreadReqSetcancelstate_Context *ctx
)
{
  if ( !ctx->joined ) {
    ctx->phase = PHASE_CLEANUP;
    ResumeTask( ctx->worker_id );
    T_eq_int( pthread_join( ctx->worker_thread, NULL ), 0 );
  }
}

/* clang-format off */

static const CPthreadReqSetcancelstate_Entry
CPthreadReqSetcancelstate_Entries[] = {
  { 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqSetcancelstate_Post_Status_Proto,
    CPthreadReqSetcancelstate_Post_CancelState_Nop,
    CPthreadReqSetcancelstate_Post_OldState_Nop,
    CPthreadReqSetcancelstate_Post_CancelType_Nop,
    CPthreadReqSetcancelstate_Post_Life_Nop,
    CPthreadReqSetcancelstate_Post_Return_Yes,
    CPthreadReqSetcancelstate_Post_ExitValue_NA,
    CPthreadReqSetcancelstate_Post_Requests_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqSetcancelstate_Post_Status_Proto,
    CPthreadReqSetcancelstate_Post_CancelState_Nop,
    CPthreadReqSetcancelstate_Post_OldState_NA,
    CPthreadReqSetcancelstate_Post_CancelType_Nop,
    CPthreadReqSetcancelstate_Post_Life_Nop,
    CPthreadReqSetcancelstate_Post_Return_Yes,
    CPthreadReqSetcancelstate_Post_ExitValue_NA,
    CPthreadReqSetcancelstate_Post_Requests_Nop },
  { 1, 0, 0, 0, 0, 0, 0, 0, CPthreadReqSetcancelstate_Post_Status_NA,
    CPthreadReqSetcancelstate_Post_CancelState_NA,
    CPthreadReqSetcancelstate_Post_OldState_NA,
    CPthreadReqSetcancelstate_Post_CancelType_NA,
    CPthreadReqSetcancelstate_Post_Life_NA,
    CPthreadReqSetcancelstate_Post_Return_NA,
    CPthreadReqSetcancelstate_Post_ExitValue_NA,
    CPthreadReqSetcancelstate_Post_Requests_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqSetcancelstate_Post_Status_Ok,
    CPthreadReqSetcancelstate_Post_CancelState_Disable,
    CPthreadReqSetcancelstate_Post_OldState_Set,
    CPthreadReqSetcancelstate_Post_CancelType_Nop,
    CPthreadReqSetcancelstate_Post_Life_Nop,
    CPthreadReqSetcancelstate_Post_Return_Yes,
    CPthreadReqSetcancelstate_Post_ExitValue_NA,
    CPthreadReqSetcancelstate_Post_Requests_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqSetcancelstate_Post_Status_Ok,
    CPthreadReqSetcancelstate_Post_CancelState_Disable,
    CPthreadReqSetcancelstate_Post_OldState_NA,
    CPthreadReqSetcancelstate_Post_CancelType_Nop,
    CPthreadReqSetcancelstate_Post_Life_Nop,
    CPthreadReqSetcancelstate_Post_Return_Yes,
    CPthreadReqSetcancelstate_Post_ExitValue_NA,
    CPthreadReqSetcancelstate_Post_Requests_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqSetcancelstate_Post_Status_Inval,
    CPthreadReqSetcancelstate_Post_CancelState_Nop,
    CPthreadReqSetcancelstate_Post_OldState_Nop,
    CPthreadReqSetcancelstate_Post_CancelType_Nop,
    CPthreadReqSetcancelstate_Post_Life_Nop,
    CPthreadReqSetcancelstate_Post_Return_Yes,
    CPthreadReqSetcancelstate_Post_ExitValue_NA,
    CPthreadReqSetcancelstate_Post_Requests_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqSetcancelstate_Post_Status_Inval,
    CPthreadReqSetcancelstate_Post_CancelState_Nop,
    CPthreadReqSetcancelstate_Post_OldState_NA,
    CPthreadReqSetcancelstate_Post_CancelType_Nop,
    CPthreadReqSetcancelstate_Post_Life_Nop,
    CPthreadReqSetcancelstate_Post_Return_Yes,
    CPthreadReqSetcancelstate_Post_ExitValue_NA,
    CPthreadReqSetcancelstate_Post_Requests_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqSetcancelstate_Post_Status_Ok,
    CPthreadReqSetcancelstate_Post_CancelState_Enable,
    CPthreadReqSetcancelstate_Post_OldState_Set,
    CPthreadReqSetcancelstate_Post_CancelType_Nop,
    CPthreadReqSetcancelstate_Post_Life_Nop,
    CPthreadReqSetcancelstate_Post_Return_Yes,
    CPthreadReqSetcancelstate_Post_ExitValue_NA,
    CPthreadReqSetcancelstate_Post_Requests_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqSetcancelstate_Post_Status_Ok,
    CPthreadReqSetcancelstate_Post_CancelState_Enable,
    CPthreadReqSetcancelstate_Post_OldState_NA,
    CPthreadReqSetcancelstate_Post_CancelType_Nop,
    CPthreadReqSetcancelstate_Post_Life_Nop,
    CPthreadReqSetcancelstate_Post_Return_Yes,
    CPthreadReqSetcancelstate_Post_ExitValue_NA,
    CPthreadReqSetcancelstate_Post_Requests_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqSetcancelstate_Post_Status_NA,
    CPthreadReqSetcancelstate_Post_CancelState_NA,
    CPthreadReqSetcancelstate_Post_OldState_NA,
    CPthreadReqSetcancelstate_Post_CancelType_NA,
    CPthreadReqSetcancelstate_Post_Life_Terminate,
    CPthreadReqSetcancelstate_Post_Return_No,
    CPthreadReqSetcancelstate_Post_ExitValue_Canceled,
    CPthreadReqSetcancelstate_Post_Requests_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqSetcancelstate_Post_Status_NA,
    CPthreadReqSetcancelstate_Post_CancelState_NA,
    CPthreadReqSetcancelstate_Post_OldState_NA,
    CPthreadReqSetcancelstate_Post_CancelType_NA,
    CPthreadReqSetcancelstate_Post_Life_Restart,
    CPthreadReqSetcancelstate_Post_Return_No,
    CPthreadReqSetcancelstate_Post_ExitValue_NA,
    CPthreadReqSetcancelstate_Post_Requests_NA }
};

static const uint8_t
CPthreadReqSetcancelstate_Map[] = {
  7, 7, 7, 7, 2, 2, 2, 7, 7, 7, 7, 7, 9, 9, 10, 7, 8, 8, 8, 8, 2, 2, 2, 8, 8,
  8, 8, 8, 9, 9, 10, 8, 3, 3, 3, 3, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4,
  4, 4, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 2, 2, 2, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 6, 6, 6, 6, 2, 2, 2, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 2, 2,
  2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
  2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

/* clang-format on */

static size_t CPthreadReqSetcancelstate_Scope( void *arg, char *buf, size_t n )
{
  CPthreadReqSetcancelstate_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      CPthreadReqSetcancelstate_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture CPthreadReqSetcancelstate_Fixture = {
  .setup = CPthreadReqSetcancelstate_Setup_Wrap,
  .stop = NULL,
  .teardown = CPthreadReqSetcancelstate_Teardown_Wrap,
  .scope = CPthreadReqSetcancelstate_Scope,
  .initial_context = &CPthreadReqSetcancelstate_Instance
};

static inline CPthreadReqSetcancelstate_Entry
CPthreadReqSetcancelstate_PopEntry( CPthreadReqSetcancelstate_Context *ctx )
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqSetcancelstate_Entries
    [ CPthreadReqSetcancelstate_Map[ index ] ];
}

static void CPthreadReqSetcancelstate_TestVariant(
  CPthreadReqSetcancelstate_Context *ctx
)
{
  CPthreadReqSetcancelstate_Pre_Context_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqSetcancelstate_Pre_State_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqSetcancelstate_Pre_OldState_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CPthreadReqSetcancelstate_Pre_CancelState_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CPthreadReqSetcancelstate_Pre_CancelType_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  CPthreadReqSetcancelstate_Pre_CancelPending_Prepare(
    ctx,
    ctx->Map.pcs[ 5 ]
  );
  CPthreadReqSetcancelstate_Pre_RestartPending_Prepare(
    ctx,
    ctx->Map.pcs[ 6 ]
  );
  CPthreadReqSetcancelstate_Action( ctx );
  CPthreadReqSetcancelstate_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  CPthreadReqSetcancelstate_Post_CancelState_Check(
    ctx,
    ctx->Map.entry.Post_CancelState
  );
  CPthreadReqSetcancelstate_Post_OldState_Check(
    ctx,
    ctx->Map.entry.Post_OldState
  );
  CPthreadReqSetcancelstate_Post_CancelType_Check(
    ctx,
    ctx->Map.entry.Post_CancelType
  );
  CPthreadReqSetcancelstate_Post_Life_Check( ctx, ctx->Map.entry.Post_Life );
  CPthreadReqSetcancelstate_Post_Return_Check(
    ctx,
    ctx->Map.entry.Post_Return
  );
  CPthreadReqSetcancelstate_Post_ExitValue_Check(
    ctx,
    ctx->Map.entry.Post_ExitValue
  );
  CPthreadReqSetcancelstate_Post_Requests_Check(
    ctx,
    ctx->Map.entry.Post_Requests
  );
}

/**
 * @fn void T_case_body_CPthreadReqSetcancelstate( void )
 */
T_TEST_CASE_FIXTURE(
  CPthreadReqSetcancelstate,
  &CPthreadReqSetcancelstate_Fixture
)
{
  CPthreadReqSetcancelstate_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CPthreadReqSetcancelstate_Pre_Context_Task;
    ctx->Map.pcs[ 0 ] < CPthreadReqSetcancelstate_Pre_Context_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CPthreadReqSetcancelstate_Pre_State_Enable;
      ctx->Map.pcs[ 1 ] < CPthreadReqSetcancelstate_Pre_State_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = CPthreadReqSetcancelstate_Pre_OldState_Valid;
        ctx->Map.pcs[ 2 ] < CPthreadReqSetcancelstate_Pre_OldState_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        for (
          ctx->Map.pcs[ 3 ] = CPthreadReqSetcancelstate_Pre_CancelState_Enable;
          ctx->Map.pcs[ 3 ] < CPthreadReqSetcancelstate_Pre_CancelState_NA;
          ++ctx->Map.pcs[ 3 ]
        ) {
          for (
            ctx->Map.pcs[ 4 ] =
              CPthreadReqSetcancelstate_Pre_CancelType_Deferred;
            ctx->Map.pcs[ 4 ] < CPthreadReqSetcancelstate_Pre_CancelType_NA;
            ++ctx->Map.pcs[ 4 ]
          ) {
            for (
              ctx->Map.pcs[ 5 ] =
                CPthreadReqSetcancelstate_Pre_CancelPending_Yes;
              ctx->Map.pcs[ 5 ] <
              CPthreadReqSetcancelstate_Pre_CancelPending_NA;
              ++ctx->Map.pcs[ 5 ]
            ) {
              for (
                ctx->Map.pcs[ 6 ] =
                  CPthreadReqSetcancelstate_Pre_RestartPending_Yes;
                ctx->Map.pcs[ 6 ] <
                CPthreadReqSetcancelstate_Pre_RestartPending_NA;
                ++ctx->Map.pcs[ 6 ]
              ) {
                ctx->Map.entry = CPthreadReqSetcancelstate_PopEntry( ctx );

                if ( ctx->Map.entry.Skip ) {
                  continue;
                }

                CPthreadReqSetcancelstate_Prepare( ctx );
                CPthreadReqSetcancelstate_TestVariant( ctx );
                CPthreadReqSetcancelstate_Cleanup( ctx );
              }
            }
          }
        }
      }
    }
  }
}

/** @} */
