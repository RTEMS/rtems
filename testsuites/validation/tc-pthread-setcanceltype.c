/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqSetcanceltype
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
 * @defgroup CPthreadReqSetcanceltype spec:/c/pthread/req/setcanceltype
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CPthreadReqSetcanceltype_Pre_Context_Task,
  CPthreadReqSetcanceltype_Pre_Context_Interrupt,
  CPthreadReqSetcanceltype_Pre_Context_NA
} CPthreadReqSetcanceltype_Pre_Context;

typedef enum {
  CPthreadReqSetcanceltype_Pre_Type_Deferred,
  CPthreadReqSetcanceltype_Pre_Type_Asynchronous,
  CPthreadReqSetcanceltype_Pre_Type_Invalid,
  CPthreadReqSetcanceltype_Pre_Type_NA
} CPthreadReqSetcanceltype_Pre_Type;

typedef enum {
  CPthreadReqSetcanceltype_Pre_OldType_Valid,
  CPthreadReqSetcanceltype_Pre_OldType_Null,
  CPthreadReqSetcanceltype_Pre_OldType_NA
} CPthreadReqSetcanceltype_Pre_OldType;

typedef enum {
  CPthreadReqSetcanceltype_Pre_CancelState_Enable,
  CPthreadReqSetcanceltype_Pre_CancelState_Disable,
  CPthreadReqSetcanceltype_Pre_CancelState_NA
} CPthreadReqSetcanceltype_Pre_CancelState;

typedef enum {
  CPthreadReqSetcanceltype_Pre_CancelType_Deferred,
  CPthreadReqSetcanceltype_Pre_CancelType_Asynchronous,
  CPthreadReqSetcanceltype_Pre_CancelType_NA
} CPthreadReqSetcanceltype_Pre_CancelType;

typedef enum {
  CPthreadReqSetcanceltype_Pre_CancelPending_Yes,
  CPthreadReqSetcanceltype_Pre_CancelPending_No,
  CPthreadReqSetcanceltype_Pre_CancelPending_NA
} CPthreadReqSetcanceltype_Pre_CancelPending;

typedef enum {
  CPthreadReqSetcanceltype_Pre_RestartPending_Yes,
  CPthreadReqSetcanceltype_Pre_RestartPending_No,
  CPthreadReqSetcanceltype_Pre_RestartPending_NA
} CPthreadReqSetcanceltype_Pre_RestartPending;

typedef enum {
  CPthreadReqSetcanceltype_Post_Status_Ok,
  CPthreadReqSetcanceltype_Post_Status_Inval,
  CPthreadReqSetcanceltype_Post_Status_Proto,
  CPthreadReqSetcanceltype_Post_Status_NA
} CPthreadReqSetcanceltype_Post_Status;

typedef enum {
  CPthreadReqSetcanceltype_Post_CancelType_Deferred,
  CPthreadReqSetcanceltype_Post_CancelType_Asynchronous,
  CPthreadReqSetcanceltype_Post_CancelType_Nop,
  CPthreadReqSetcanceltype_Post_CancelType_NA
} CPthreadReqSetcanceltype_Post_CancelType;

typedef enum {
  CPthreadReqSetcanceltype_Post_OldType_Set,
  CPthreadReqSetcanceltype_Post_OldType_Nop,
  CPthreadReqSetcanceltype_Post_OldType_NA
} CPthreadReqSetcanceltype_Post_OldType;

typedef enum {
  CPthreadReqSetcanceltype_Post_CancelState_Nop,
  CPthreadReqSetcanceltype_Post_CancelState_NA
} CPthreadReqSetcanceltype_Post_CancelState;

typedef enum {
  CPthreadReqSetcanceltype_Post_Life_Nop,
  CPthreadReqSetcanceltype_Post_Life_Terminate,
  CPthreadReqSetcanceltype_Post_Life_Restart,
  CPthreadReqSetcanceltype_Post_Life_NA
} CPthreadReqSetcanceltype_Post_Life;

typedef enum {
  CPthreadReqSetcanceltype_Post_Return_Yes,
  CPthreadReqSetcanceltype_Post_Return_No,
  CPthreadReqSetcanceltype_Post_Return_NA
} CPthreadReqSetcanceltype_Post_Return;

typedef enum {
  CPthreadReqSetcanceltype_Post_ExitValue_Canceled,
  CPthreadReqSetcanceltype_Post_ExitValue_NA
} CPthreadReqSetcanceltype_Post_ExitValue;

typedef enum {
  CPthreadReqSetcanceltype_Post_Requests_Nop,
  CPthreadReqSetcanceltype_Post_Requests_NA
} CPthreadReqSetcanceltype_Post_Requests;

typedef struct {
  uint32_t Skip : 1;
  uint32_t Pre_Context_NA : 1;
  uint32_t Pre_Type_NA : 1;
  uint32_t Pre_OldType_NA : 1;
  uint32_t Pre_CancelState_NA : 1;
  uint32_t Pre_CancelType_NA : 1;
  uint32_t Pre_CancelPending_NA : 1;
  uint32_t Pre_RestartPending_NA : 1;
  uint32_t Post_Status : 2;
  uint32_t Post_CancelType : 2;
  uint32_t Post_OldType : 2;
  uint32_t Post_CancelState : 1;
  uint32_t Post_Life : 2;
  uint32_t Post_Return : 2;
  uint32_t Post_ExitValue : 1;
  uint32_t Post_Requests : 1;
} CPthreadReqSetcanceltype_Entry;

typedef enum { PHASE_ACTION, PHASE_LATER, PHASE_CLEANUP } Phase;

/**
 * @brief Test context for spec:/c/pthread/req/setcanceltype test case.
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
   * @brief This member provides the object referenced by the `oldtype`
   *   parameter.
   */
  int oldtype_obj;

  /**
   * @brief This member contains the return value of the
   *   pthread_setcanceltype() call.
   */
  int status;

  /**
   * @brief This member specifies the `type` parameter value.
   */
  int type;

  /**
   * @brief This member specifies the `oldtype` parameter value.
   */
  int *oldtype;

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
    CPthreadReqSetcanceltype_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqSetcanceltype_Context;

static CPthreadReqSetcanceltype_Context CPthreadReqSetcanceltype_Instance;

static const char *const CPthreadReqSetcanceltype_PreDesc_Context[] =
  { "Task", "Interrupt", "NA" };

static const char *const CPthreadReqSetcanceltype_PreDesc_Type[] =
  { "Deferred", "Asynchronous", "Invalid", "NA" };

static const char *const CPthreadReqSetcanceltype_PreDesc_OldType[] =
  { "Valid", "Null", "NA" };

static const char *const CPthreadReqSetcanceltype_PreDesc_CancelState[] =
  { "Enable", "Disable", "NA" };

static const char *const CPthreadReqSetcanceltype_PreDesc_CancelType[] =
  { "Deferred", "Asynchronous", "NA" };

static const char *const CPthreadReqSetcanceltype_PreDesc_CancelPending[] =
  { "Yes", "No", "NA" };

static const char *const CPthreadReqSetcanceltype_PreDesc_RestartPending[] =
  { "Yes", "No", "NA" };

static const char *const *const CPthreadReqSetcanceltype_PreDesc[] = {
  CPthreadReqSetcanceltype_PreDesc_Context,
  CPthreadReqSetcanceltype_PreDesc_Type,
  CPthreadReqSetcanceltype_PreDesc_OldType,
  CPthreadReqSetcanceltype_PreDesc_CancelState,
  CPthreadReqSetcanceltype_PreDesc_CancelType,
  CPthreadReqSetcanceltype_PreDesc_CancelPending,
  CPthreadReqSetcanceltype_PreDesc_RestartPending,
  NULL
};

typedef CPthreadReqSetcanceltype_Context Context;

static int exit_object;

static void Action( void *arg )
{
  Context *ctx;

  ctx = arg;
  ctx->status = pthread_setcanceltype( ctx->type, ctx->oldtype );
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

static void CPthreadReqSetcanceltype_Pre_Context_Prepare(
  CPthreadReqSetcanceltype_Context    *ctx,
  CPthreadReqSetcanceltype_Pre_Context state
)
{
  switch ( state ) {
    case CPthreadReqSetcanceltype_Pre_Context_Task: {
      /*
       * While pthread_setcanceltype() is called from within task context.
       */
      ctx->in_interrupt = false;
      break;
    }

    case CPthreadReqSetcanceltype_Pre_Context_Interrupt: {
      /*
       * While pthread_setcanceltype() is called from within interrupt context.
       */
      ctx->in_interrupt = true;
      break;
    }

    case CPthreadReqSetcanceltype_Pre_Context_NA:
      break;
  }
}

static void CPthreadReqSetcanceltype_Pre_Type_Prepare(
  CPthreadReqSetcanceltype_Context *ctx,
  CPthreadReqSetcanceltype_Pre_Type state
)
{
  switch ( state ) {
    case CPthreadReqSetcanceltype_Pre_Type_Deferred: {
      /*
       * While the `type` parameter is equal to PTHREAD_CANCEL_DEFERRED.
       */
      ctx->type = PTHREAD_CANCEL_DEFERRED;
      break;
    }

    case CPthreadReqSetcanceltype_Pre_Type_Asynchronous: {
      /*
       * While the `type` parameter is equal to PTHREAD_CANCEL_ASYNCHRONOUS.
       */
      ctx->type = PTHREAD_CANCEL_ASYNCHRONOUS;
      break;
    }

    case CPthreadReqSetcanceltype_Pre_Type_Invalid: {
      /*
       * While the `type` parameter is neither equal to PTHREAD_CANCEL_DEFERRED
       * nor equal to PTHREAD_CANCEL_ASYNCHRONOUS.
       */
      ctx->type = -1;
      break;
    }

    case CPthreadReqSetcanceltype_Pre_Type_NA:
      break;
  }
}

static void CPthreadReqSetcanceltype_Pre_OldType_Prepare(
  CPthreadReqSetcanceltype_Context    *ctx,
  CPthreadReqSetcanceltype_Pre_OldType state
)
{
  switch ( state ) {
    case CPthreadReqSetcanceltype_Pre_OldType_Valid: {
      /*
       * While the `oldtype` parameter references an object of type int.
       */
      ctx->oldtype = &ctx->oldtype_obj;
      break;
    }

    case CPthreadReqSetcanceltype_Pre_OldType_Null: {
      /*
       * While the `oldtype` parameter is equal to NULL.
       */
      ctx->oldtype = NULL;
      break;
    }

    case CPthreadReqSetcanceltype_Pre_OldType_NA:
      break;
  }
}

static void CPthreadReqSetcanceltype_Pre_CancelState_Prepare(
  CPthreadReqSetcanceltype_Context        *ctx,
  CPthreadReqSetcanceltype_Pre_CancelState state
)
{
  switch ( state ) {
    case CPthreadReqSetcanceltype_Pre_CancelState_Enable: {
      /*
       * While the thread cancel state attribute of the executing task is
       * PTHREAD_CANCEL_ENABLE.
       */
      ctx->cancel_state = PTHREAD_CANCEL_ENABLE;
      break;
    }

    case CPthreadReqSetcanceltype_Pre_CancelState_Disable: {
      /*
       * While the thread cancel state attribute of the executing task is
       * PTHREAD_CANCEL_DISABLE.
       */
      ctx->cancel_state = PTHREAD_CANCEL_DISABLE;
      break;
    }

    case CPthreadReqSetcanceltype_Pre_CancelState_NA:
      break;
  }
}

static void CPthreadReqSetcanceltype_Pre_CancelType_Prepare(
  CPthreadReqSetcanceltype_Context       *ctx,
  CPthreadReqSetcanceltype_Pre_CancelType state
)
{
  switch ( state ) {
    case CPthreadReqSetcanceltype_Pre_CancelType_Deferred: {
      /*
       * While the thread cancel type attribute of the executing task is
       * PTHREAD_CANCEL_DEFERRED.
       */
      ctx->cancel_type = PTHREAD_CANCEL_DEFERRED;
      break;
    }

    case CPthreadReqSetcanceltype_Pre_CancelType_Asynchronous: {
      /*
       * While the thread cancel type attribute of the executing task is
       * PTHREAD_CANCEL_ASYNCHRONOUS.
       */
      ctx->cancel_type = PTHREAD_CANCEL_ASYNCHRONOUS;
      break;
    }

    case CPthreadReqSetcanceltype_Pre_CancelType_NA:
      break;
  }
}

static void CPthreadReqSetcanceltype_Pre_CancelPending_Prepare(
  CPthreadReqSetcanceltype_Context          *ctx,
  CPthreadReqSetcanceltype_Pre_CancelPending state
)
{
  switch ( state ) {
    case CPthreadReqSetcanceltype_Pre_CancelPending_Yes: {
      /*
       * While a thread cancellation request is pending for the executing task.
       */
      ctx->cancel_pending = true;
      break;
    }

    case CPthreadReqSetcanceltype_Pre_CancelPending_No: {
      /*
       * While no thread cancellation request is pending for the executing
       * task.
       */
      ctx->cancel_pending = false;
      break;
    }

    case CPthreadReqSetcanceltype_Pre_CancelPending_NA:
      break;
  }
}

static void CPthreadReqSetcanceltype_Pre_RestartPending_Prepare(
  CPthreadReqSetcanceltype_Context           *ctx,
  CPthreadReqSetcanceltype_Pre_RestartPending state
)
{
  switch ( state ) {
    case CPthreadReqSetcanceltype_Pre_RestartPending_Yes: {
      /*
       * While a restart of the executing task by rtems_task_restart() is
       * pending.
       */
      ctx->restart_pending = true;
      break;
    }

    case CPthreadReqSetcanceltype_Pre_RestartPending_No: {
      /*
       * While no restart of the executing task is pending.
       */
      ctx->restart_pending = false;
      break;
    }

    case CPthreadReqSetcanceltype_Pre_RestartPending_NA:
      break;
  }
}

static void CPthreadReqSetcanceltype_Post_Status_Check(
  CPthreadReqSetcanceltype_Context    *ctx,
  CPthreadReqSetcanceltype_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqSetcanceltype_Post_Status_Ok: {
      /*
       * The return value of pthread_setcanceltype() shall be zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CPthreadReqSetcanceltype_Post_Status_Inval: {
      /*
       * The return value of pthread_setcanceltype() shall be EINVAL.
       */
      T_eq_int( ctx->status, EINVAL );
      break;
    }

    case CPthreadReqSetcanceltype_Post_Status_Proto: {
      /*
       * The return value of pthread_setcanceltype() shall be EPROTO.
       */
      T_eq_int( ctx->status, EPROTO );
      break;
    }

    case CPthreadReqSetcanceltype_Post_Status_NA:
      break;
  }
}

static void CPthreadReqSetcanceltype_Post_CancelType_Check(
  CPthreadReqSetcanceltype_Context        *ctx,
  CPthreadReqSetcanceltype_Post_CancelType state
)
{
  switch ( state ) {
    case CPthreadReqSetcanceltype_Post_CancelType_Deferred: {
      /*
       * The thread cancel type attribute of the executing task shall be
       * PTHREAD_CANCEL_DEFERRED.
       */
      T_eq_int( ctx->type_after, PTHREAD_CANCEL_DEFERRED );
      break;
    }

    case CPthreadReqSetcanceltype_Post_CancelType_Asynchronous: {
      /*
       * The thread cancel type attribute of the executing task shall be
       * PTHREAD_CANCEL_ASYNCHRONOUS.
       */
      T_eq_int( ctx->type_after, PTHREAD_CANCEL_ASYNCHRONOUS );
      break;
    }

    case CPthreadReqSetcanceltype_Post_CancelType_Nop: {
      /*
       * The thread cancel type attribute of the executing task shall not be
       * modified by the pthread_setcanceltype() call.
       */
      T_eq_int( ctx->type_after, ctx->cancel_type );
      break;
    }

    case CPthreadReqSetcanceltype_Post_CancelType_NA:
      break;
  }
}

static void CPthreadReqSetcanceltype_Post_OldType_Check(
  CPthreadReqSetcanceltype_Context     *ctx,
  CPthreadReqSetcanceltype_Post_OldType state
)
{
  switch ( state ) {
    case CPthreadReqSetcanceltype_Post_OldType_Set: {
      /*
       * The value of the object referenced by the `oldtype` parameter shall be
       * set to the thread cancel type attribute of the executing task before
       * the call.
       */
      T_eq_int( ctx->oldtype_obj, ctx->cancel_type );
      break;
    }

    case CPthreadReqSetcanceltype_Post_OldType_Nop: {
      /*
       * The object referenced by the `oldtype` parameter shall not be modified
       * by the pthread_setcanceltype() call.
       */
      T_eq_int( ctx->oldtype_obj, -1 );
      break;
    }

    case CPthreadReqSetcanceltype_Post_OldType_NA:
      break;
  }
}

static void CPthreadReqSetcanceltype_Post_CancelState_Check(
  CPthreadReqSetcanceltype_Context         *ctx,
  CPthreadReqSetcanceltype_Post_CancelState state
)
{
  switch ( state ) {
    case CPthreadReqSetcanceltype_Post_CancelState_Nop: {
      /*
       * The thread cancel state attribute of the executing task shall not be
       * modified by the pthread_setcanceltype() call.
       */
      T_eq_int( ctx->state_after, ctx->cancel_state );
      break;
    }

    case CPthreadReqSetcanceltype_Post_CancelState_NA:
      break;
  }
}

static void CPthreadReqSetcanceltype_Post_Life_Check(
  CPthreadReqSetcanceltype_Context  *ctx,
  CPthreadReqSetcanceltype_Post_Life state
)
{
  switch ( state ) {
    case CPthreadReqSetcanceltype_Post_Life_Nop: {
      /*
       * The executing task shall be neither terminated nor restarted by the
       * pthread_setcanceltype() call.
       */
      T_true( IsTaskSuspended( ctx->worker_id ) );
      T_eq_u32( ctx->restarts, 0 );
      break;
    }

    case CPthreadReqSetcanceltype_Post_Life_Terminate: {
      /*
       * The executing task shall be terminated by a thread cancellation.
       */
      JoinWorker( ctx );
      T_eq_u32( ctx->restarts, 0 );
      break;
    }

    case CPthreadReqSetcanceltype_Post_Life_Restart: {
      /*
       * The executing task shall be restarted.
       */
      JoinWorker( ctx );
      T_eq_ptr( ctx->exit_value, &exit_object );
      T_eq_u32( ctx->restarts, 1 );
      break;
    }

    case CPthreadReqSetcanceltype_Post_Life_NA:
      break;
  }
}

static void CPthreadReqSetcanceltype_Post_Return_Check(
  CPthreadReqSetcanceltype_Context    *ctx,
  CPthreadReqSetcanceltype_Post_Return state
)
{
  switch ( state ) {
    case CPthreadReqSetcanceltype_Post_Return_Yes: {
      /*
       * The pthread_setcanceltype() call shall return to the caller.
       */
      T_true( ctx->returned );
      break;
    }

    case CPthreadReqSetcanceltype_Post_Return_No: {
      /*
       * The pthread_setcanceltype() call shall not return to the caller.
       */
      T_false( ctx->returned );
      break;
    }

    case CPthreadReqSetcanceltype_Post_Return_NA:
      break;
  }
}

static void CPthreadReqSetcanceltype_Post_ExitValue_Check(
  CPthreadReqSetcanceltype_Context       *ctx,
  CPthreadReqSetcanceltype_Post_ExitValue state
)
{
  switch ( state ) {
    case CPthreadReqSetcanceltype_Post_ExitValue_Canceled: {
      /*
       * The thread exit value of the executing task shall be PTHREAD_CANCELED.
       */
      T_eq_ptr( ctx->exit_value, PTHREAD_CANCELED );
      break;
    }

    case CPthreadReqSetcanceltype_Post_ExitValue_NA:
      break;
  }
}

static void CPthreadReqSetcanceltype_Post_Requests_Check(
  CPthreadReqSetcanceltype_Context      *ctx,
  CPthreadReqSetcanceltype_Post_Requests state
)
{
  switch ( state ) {
    case CPthreadReqSetcanceltype_Post_Requests_Nop: {
      /*
       * The pending thread cancellation request and the pending restart by
       * rtems_task_restart() of the executing task shall not be modified by
       * the pthread_setcanceltype() call.
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

    case CPthreadReqSetcanceltype_Post_Requests_NA:
      break;
  }
}

static void CPthreadReqSetcanceltype_Setup( void )
{
  SetSelfPriority( PRIO_NORMAL );
}

static void CPthreadReqSetcanceltype_Setup_Wrap( void *arg )
{
  CPthreadReqSetcanceltype_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CPthreadReqSetcanceltype_Setup();
}

static void CPthreadReqSetcanceltype_Teardown( void )
{
  RestoreRunnerPriority();
}

static void CPthreadReqSetcanceltype_Teardown_Wrap( void *arg )
{
  CPthreadReqSetcanceltype_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CPthreadReqSetcanceltype_Teardown();
}

static void CPthreadReqSetcanceltype_Prepare(
  CPthreadReqSetcanceltype_Context *ctx
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
  ctx->oldtype_obj = -1;
  ctx->state_after = -1;
  ctx->type_after = -1;
}

static void CPthreadReqSetcanceltype_Action(
  CPthreadReqSetcanceltype_Context *ctx
)
{
  ctx->worker_id = CreateTask( "WORK", PRIO_HIGH );
  StartTask( ctx->worker_id, Worker, ctx );
}

static void CPthreadReqSetcanceltype_Cleanup(
  CPthreadReqSetcanceltype_Context *ctx
)
{
  if ( !ctx->joined ) {
    ctx->phase = PHASE_CLEANUP;
    ResumeTask( ctx->worker_id );
    T_eq_int( pthread_join( ctx->worker_thread, NULL ), 0 );
  }
}

/* clang-format off */

static const CPthreadReqSetcanceltype_Entry
CPthreadReqSetcanceltype_Entries[] = {
  { 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqSetcanceltype_Post_Status_Proto,
    CPthreadReqSetcanceltype_Post_CancelType_Nop,
    CPthreadReqSetcanceltype_Post_OldType_Nop,
    CPthreadReqSetcanceltype_Post_CancelState_Nop,
    CPthreadReqSetcanceltype_Post_Life_Nop,
    CPthreadReqSetcanceltype_Post_Return_Yes,
    CPthreadReqSetcanceltype_Post_ExitValue_NA,
    CPthreadReqSetcanceltype_Post_Requests_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqSetcanceltype_Post_Status_Proto,
    CPthreadReqSetcanceltype_Post_CancelType_Nop,
    CPthreadReqSetcanceltype_Post_OldType_NA,
    CPthreadReqSetcanceltype_Post_CancelState_Nop,
    CPthreadReqSetcanceltype_Post_Life_Nop,
    CPthreadReqSetcanceltype_Post_Return_Yes,
    CPthreadReqSetcanceltype_Post_ExitValue_NA,
    CPthreadReqSetcanceltype_Post_Requests_Nop },
  { 1, 0, 0, 0, 0, 0, 0, 0, CPthreadReqSetcanceltype_Post_Status_NA,
    CPthreadReqSetcanceltype_Post_CancelType_NA,
    CPthreadReqSetcanceltype_Post_OldType_NA,
    CPthreadReqSetcanceltype_Post_CancelState_NA,
    CPthreadReqSetcanceltype_Post_Life_NA,
    CPthreadReqSetcanceltype_Post_Return_NA,
    CPthreadReqSetcanceltype_Post_ExitValue_NA,
    CPthreadReqSetcanceltype_Post_Requests_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqSetcanceltype_Post_Status_Ok,
    CPthreadReqSetcanceltype_Post_CancelType_Deferred,
    CPthreadReqSetcanceltype_Post_OldType_Set,
    CPthreadReqSetcanceltype_Post_CancelState_Nop,
    CPthreadReqSetcanceltype_Post_Life_Nop,
    CPthreadReqSetcanceltype_Post_Return_Yes,
    CPthreadReqSetcanceltype_Post_ExitValue_NA,
    CPthreadReqSetcanceltype_Post_Requests_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqSetcanceltype_Post_Status_Ok,
    CPthreadReqSetcanceltype_Post_CancelType_Deferred,
    CPthreadReqSetcanceltype_Post_OldType_NA,
    CPthreadReqSetcanceltype_Post_CancelState_Nop,
    CPthreadReqSetcanceltype_Post_Life_Nop,
    CPthreadReqSetcanceltype_Post_Return_Yes,
    CPthreadReqSetcanceltype_Post_ExitValue_NA,
    CPthreadReqSetcanceltype_Post_Requests_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqSetcanceltype_Post_Status_Inval,
    CPthreadReqSetcanceltype_Post_CancelType_Nop,
    CPthreadReqSetcanceltype_Post_OldType_Nop,
    CPthreadReqSetcanceltype_Post_CancelState_Nop,
    CPthreadReqSetcanceltype_Post_Life_Nop,
    CPthreadReqSetcanceltype_Post_Return_Yes,
    CPthreadReqSetcanceltype_Post_ExitValue_NA,
    CPthreadReqSetcanceltype_Post_Requests_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqSetcanceltype_Post_Status_Inval,
    CPthreadReqSetcanceltype_Post_CancelType_Nop,
    CPthreadReqSetcanceltype_Post_OldType_NA,
    CPthreadReqSetcanceltype_Post_CancelState_Nop,
    CPthreadReqSetcanceltype_Post_Life_Nop,
    CPthreadReqSetcanceltype_Post_Return_Yes,
    CPthreadReqSetcanceltype_Post_ExitValue_NA,
    CPthreadReqSetcanceltype_Post_Requests_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqSetcanceltype_Post_Status_Ok,
    CPthreadReqSetcanceltype_Post_CancelType_Asynchronous,
    CPthreadReqSetcanceltype_Post_OldType_Set,
    CPthreadReqSetcanceltype_Post_CancelState_Nop,
    CPthreadReqSetcanceltype_Post_Life_Nop,
    CPthreadReqSetcanceltype_Post_Return_Yes,
    CPthreadReqSetcanceltype_Post_ExitValue_NA,
    CPthreadReqSetcanceltype_Post_Requests_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqSetcanceltype_Post_Status_Ok,
    CPthreadReqSetcanceltype_Post_CancelType_Asynchronous,
    CPthreadReqSetcanceltype_Post_OldType_NA,
    CPthreadReqSetcanceltype_Post_CancelState_Nop,
    CPthreadReqSetcanceltype_Post_Life_Nop,
    CPthreadReqSetcanceltype_Post_Return_Yes,
    CPthreadReqSetcanceltype_Post_ExitValue_NA,
    CPthreadReqSetcanceltype_Post_Requests_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqSetcanceltype_Post_Status_NA,
    CPthreadReqSetcanceltype_Post_CancelType_NA,
    CPthreadReqSetcanceltype_Post_OldType_NA,
    CPthreadReqSetcanceltype_Post_CancelState_NA,
    CPthreadReqSetcanceltype_Post_Life_Terminate,
    CPthreadReqSetcanceltype_Post_Return_No,
    CPthreadReqSetcanceltype_Post_ExitValue_Canceled,
    CPthreadReqSetcanceltype_Post_Requests_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, CPthreadReqSetcanceltype_Post_Status_NA,
    CPthreadReqSetcanceltype_Post_CancelType_NA,
    CPthreadReqSetcanceltype_Post_OldType_NA,
    CPthreadReqSetcanceltype_Post_CancelState_NA,
    CPthreadReqSetcanceltype_Post_Life_Restart,
    CPthreadReqSetcanceltype_Post_Return_No,
    CPthreadReqSetcanceltype_Post_ExitValue_NA,
    CPthreadReqSetcanceltype_Post_Requests_NA }
};

static const uint8_t
CPthreadReqSetcanceltype_Map[] = {
  3, 3, 3, 3, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 2, 2, 2, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 9, 9, 10, 7, 2, 2, 2, 7, 7, 7, 7, 7, 7, 7, 7, 7, 9, 9, 10,
  8, 2, 2, 2, 8, 8, 8, 8, 8, 8, 8, 8, 8, 5, 5, 5, 5, 2, 2, 2, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 6, 6, 6, 6, 2, 2, 2, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 2, 2, 2,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
  0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2,
  2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

/* clang-format on */

static size_t CPthreadReqSetcanceltype_Scope( void *arg, char *buf, size_t n )
{
  CPthreadReqSetcanceltype_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      CPthreadReqSetcanceltype_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture CPthreadReqSetcanceltype_Fixture = {
  .setup = CPthreadReqSetcanceltype_Setup_Wrap,
  .stop = NULL,
  .teardown = CPthreadReqSetcanceltype_Teardown_Wrap,
  .scope = CPthreadReqSetcanceltype_Scope,
  .initial_context = &CPthreadReqSetcanceltype_Instance
};

static inline CPthreadReqSetcanceltype_Entry CPthreadReqSetcanceltype_PopEntry(
  CPthreadReqSetcanceltype_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqSetcanceltype_Entries
    [ CPthreadReqSetcanceltype_Map[ index ] ];
}

static void CPthreadReqSetcanceltype_TestVariant(
  CPthreadReqSetcanceltype_Context *ctx
)
{
  CPthreadReqSetcanceltype_Pre_Context_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqSetcanceltype_Pre_Type_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqSetcanceltype_Pre_OldType_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CPthreadReqSetcanceltype_Pre_CancelState_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CPthreadReqSetcanceltype_Pre_CancelType_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  CPthreadReqSetcanceltype_Pre_CancelPending_Prepare( ctx, ctx->Map.pcs[ 5 ] );
  CPthreadReqSetcanceltype_Pre_RestartPending_Prepare(
    ctx,
    ctx->Map.pcs[ 6 ]
  );
  CPthreadReqSetcanceltype_Action( ctx );
  CPthreadReqSetcanceltype_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  CPthreadReqSetcanceltype_Post_CancelType_Check(
    ctx,
    ctx->Map.entry.Post_CancelType
  );
  CPthreadReqSetcanceltype_Post_OldType_Check(
    ctx,
    ctx->Map.entry.Post_OldType
  );
  CPthreadReqSetcanceltype_Post_CancelState_Check(
    ctx,
    ctx->Map.entry.Post_CancelState
  );
  CPthreadReqSetcanceltype_Post_Life_Check( ctx, ctx->Map.entry.Post_Life );
  CPthreadReqSetcanceltype_Post_Return_Check(
    ctx,
    ctx->Map.entry.Post_Return
  );
  CPthreadReqSetcanceltype_Post_ExitValue_Check(
    ctx,
    ctx->Map.entry.Post_ExitValue
  );
  CPthreadReqSetcanceltype_Post_Requests_Check(
    ctx,
    ctx->Map.entry.Post_Requests
  );
}

/**
 * @fn void T_case_body_CPthreadReqSetcanceltype( void )
 */
T_TEST_CASE_FIXTURE(
  CPthreadReqSetcanceltype,
  &CPthreadReqSetcanceltype_Fixture
)
{
  CPthreadReqSetcanceltype_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CPthreadReqSetcanceltype_Pre_Context_Task;
    ctx->Map.pcs[ 0 ] < CPthreadReqSetcanceltype_Pre_Context_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CPthreadReqSetcanceltype_Pre_Type_Deferred;
      ctx->Map.pcs[ 1 ] < CPthreadReqSetcanceltype_Pre_Type_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = CPthreadReqSetcanceltype_Pre_OldType_Valid;
        ctx->Map.pcs[ 2 ] < CPthreadReqSetcanceltype_Pre_OldType_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        for (
          ctx->Map.pcs[ 3 ] = CPthreadReqSetcanceltype_Pre_CancelState_Enable;
          ctx->Map.pcs[ 3 ] < CPthreadReqSetcanceltype_Pre_CancelState_NA;
          ++ctx->Map.pcs[ 3 ]
        ) {
          for (
            ctx->Map.pcs[ 4 ] =
              CPthreadReqSetcanceltype_Pre_CancelType_Deferred;
            ctx->Map.pcs[ 4 ] < CPthreadReqSetcanceltype_Pre_CancelType_NA;
            ++ctx->Map.pcs[ 4 ]
          ) {
            for (
              ctx->Map.pcs[ 5 ] =
                CPthreadReqSetcanceltype_Pre_CancelPending_Yes;
              ctx->Map.pcs[ 5 ] <
              CPthreadReqSetcanceltype_Pre_CancelPending_NA;
              ++ctx->Map.pcs[ 5 ]
            ) {
              for (
                ctx->Map.pcs[ 6 ] =
                  CPthreadReqSetcanceltype_Pre_RestartPending_Yes;
                ctx->Map.pcs[ 6 ] <
                CPthreadReqSetcanceltype_Pre_RestartPending_NA;
                ++ctx->Map.pcs[ 6 ]
              ) {
                ctx->Map.entry = CPthreadReqSetcanceltype_PopEntry( ctx );

                if ( ctx->Map.entry.Skip ) {
                  continue;
                }

                CPthreadReqSetcanceltype_Prepare( ctx );
                CPthreadReqSetcanceltype_TestVariant( ctx );
                CPthreadReqSetcanceltype_Cleanup( ctx );
              }
            }
          }
        }
      }
    }
  }
}

/** @} */
