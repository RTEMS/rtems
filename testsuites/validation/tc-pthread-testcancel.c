/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqTestcancel
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

#include <pthread.h>
#include <rtems.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup CPthreadReqTestcancel spec:/c/pthread/req/testcancel
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CPthreadReqTestcancel_Pre_Context_Task,
  CPthreadReqTestcancel_Pre_Context_Interrupt,
  CPthreadReqTestcancel_Pre_Context_NA
} CPthreadReqTestcancel_Pre_Context;

typedef enum {
  CPthreadReqTestcancel_Pre_CancelState_Enable,
  CPthreadReqTestcancel_Pre_CancelState_Disable,
  CPthreadReqTestcancel_Pre_CancelState_NA
} CPthreadReqTestcancel_Pre_CancelState;

typedef enum {
  CPthreadReqTestcancel_Pre_CancelType_Deferred,
  CPthreadReqTestcancel_Pre_CancelType_Asynchronous,
  CPthreadReqTestcancel_Pre_CancelType_NA
} CPthreadReqTestcancel_Pre_CancelType;

typedef enum {
  CPthreadReqTestcancel_Pre_CancelPending_Yes,
  CPthreadReqTestcancel_Pre_CancelPending_No,
  CPthreadReqTestcancel_Pre_CancelPending_NA
} CPthreadReqTestcancel_Pre_CancelPending;

typedef enum {
  CPthreadReqTestcancel_Pre_RestartPending_Yes,
  CPthreadReqTestcancel_Pre_RestartPending_No,
  CPthreadReqTestcancel_Pre_RestartPending_NA
} CPthreadReqTestcancel_Pre_RestartPending;

typedef enum {
  CPthreadReqTestcancel_Post_Return_Yes,
  CPthreadReqTestcancel_Post_Return_No,
  CPthreadReqTestcancel_Post_Return_NA
} CPthreadReqTestcancel_Post_Return;

typedef enum {
  CPthreadReqTestcancel_Post_Life_Terminate,
  CPthreadReqTestcancel_Post_Life_Restart,
  CPthreadReqTestcancel_Post_Life_Nop,
  CPthreadReqTestcancel_Post_Life_NA
} CPthreadReqTestcancel_Post_Life;

typedef enum {
  CPthreadReqTestcancel_Post_ExitValue_Canceled,
  CPthreadReqTestcancel_Post_ExitValue_NA
} CPthreadReqTestcancel_Post_ExitValue;

typedef enum {
  CPthreadReqTestcancel_Post_CancelState_Nop,
  CPthreadReqTestcancel_Post_CancelState_NA
} CPthreadReqTestcancel_Post_CancelState;

typedef enum {
  CPthreadReqTestcancel_Post_CancelType_Nop,
  CPthreadReqTestcancel_Post_CancelType_NA
} CPthreadReqTestcancel_Post_CancelType;

typedef enum {
  CPthreadReqTestcancel_Post_Requests_Nop,
  CPthreadReqTestcancel_Post_Requests_NA
} CPthreadReqTestcancel_Post_Requests;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Context_NA : 1;
  uint16_t Pre_CancelState_NA : 1;
  uint16_t Pre_CancelType_NA : 1;
  uint16_t Pre_CancelPending_NA : 1;
  uint16_t Pre_RestartPending_NA : 1;
  uint16_t Post_Return : 2;
  uint16_t Post_Life : 2;
  uint16_t Post_ExitValue : 1;
  uint16_t Post_CancelState : 1;
  uint16_t Post_CancelType : 1;
  uint16_t Post_Requests : 1;
} CPthreadReqTestcancel_Entry;

typedef enum { PHASE_ACTION, PHASE_LATER, PHASE_CLEANUP } Phase;

/**
 * @brief Test context for spec:/c/pthread/req/testcancel test case.
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
   * @brief This member is true, if the worker task started.
   */
  bool worker_started;

  /**
   * @brief This member contains the count of restarts of the worker task.
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
   * @brief This member is true, if the test joined the worker task.
   */
  bool joined;

  /**
   * @brief This member contains the thread exit value of the worker task.
   */
  void *exit_value;

  struct {
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
    CPthreadReqTestcancel_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqTestcancel_Context;

static CPthreadReqTestcancel_Context CPthreadReqTestcancel_Instance;

static const char *const CPthreadReqTestcancel_PreDesc_Context[] =
  { "Task", "Interrupt", "NA" };

static const char *const CPthreadReqTestcancel_PreDesc_CancelState[] =
  { "Enable", "Disable", "NA" };

static const char *const CPthreadReqTestcancel_PreDesc_CancelType[] =
  { "Deferred", "Asynchronous", "NA" };

static const char *const CPthreadReqTestcancel_PreDesc_CancelPending[] =
  { "Yes", "No", "NA" };

static const char *const CPthreadReqTestcancel_PreDesc_RestartPending[] =
  { "Yes", "No", "NA" };

static const char *const *const CPthreadReqTestcancel_PreDesc[] = {
  CPthreadReqTestcancel_PreDesc_Context,
  CPthreadReqTestcancel_PreDesc_CancelState,
  CPthreadReqTestcancel_PreDesc_CancelType,
  CPthreadReqTestcancel_PreDesc_CancelPending,
  CPthreadReqTestcancel_PreDesc_RestartPending,
  NULL
};

typedef CPthreadReqTestcancel_Context Context;

static int exit_object;

static void TestCancel( void *arg )
{
  Context *ctx;

  ctx = arg;
  pthread_testcancel();
  ctx->returned = true;
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
    CallWithinISR( TestCancel, ctx );
  } else {
    TestCancel( ctx );
  }

  DisableCancelability( &ctx->state_after, &ctx->type_after );
  SuspendSelf();
  SetCancelability( PTHREAD_CANCEL_ENABLE, PTHREAD_CANCEL_ASYNCHRONOUS );

  if ( ctx->phase == PHASE_LATER ) {
    ctx->late_returned = true;
  }

  pthread_exit( &exit_object );
}

static void CPthreadReqTestcancel_Pre_Context_Prepare(
  CPthreadReqTestcancel_Context    *ctx,
  CPthreadReqTestcancel_Pre_Context state
)
{
  switch ( state ) {
    case CPthreadReqTestcancel_Pre_Context_Task: {
      /*
       * While pthread_testcancel() is called from within task context.
       */
      ctx->in_interrupt = false;
      break;
    }

    case CPthreadReqTestcancel_Pre_Context_Interrupt: {
      /*
       * While pthread_testcancel() is called from within interrupt context.
       */
      ctx->in_interrupt = true;
      break;
    }

    case CPthreadReqTestcancel_Pre_Context_NA:
      break;
  }
}

static void CPthreadReqTestcancel_Pre_CancelState_Prepare(
  CPthreadReqTestcancel_Context        *ctx,
  CPthreadReqTestcancel_Pre_CancelState state
)
{
  switch ( state ) {
    case CPthreadReqTestcancel_Pre_CancelState_Enable: {
      /*
       * While the thread cancel state attribute of the executing task is
       * PTHREAD_CANCEL_ENABLE.
       */
      ctx->cancel_state = PTHREAD_CANCEL_ENABLE;
      break;
    }

    case CPthreadReqTestcancel_Pre_CancelState_Disable: {
      /*
       * While the thread cancel state attribute of the executing task is
       * PTHREAD_CANCEL_DISABLE.
       */
      ctx->cancel_state = PTHREAD_CANCEL_DISABLE;
      break;
    }

    case CPthreadReqTestcancel_Pre_CancelState_NA:
      break;
  }
}

static void CPthreadReqTestcancel_Pre_CancelType_Prepare(
  CPthreadReqTestcancel_Context       *ctx,
  CPthreadReqTestcancel_Pre_CancelType state
)
{
  switch ( state ) {
    case CPthreadReqTestcancel_Pre_CancelType_Deferred: {
      /*
       * While the thread cancel type attribute of the executing task is
       * PTHREAD_CANCEL_DEFERRED.
       */
      ctx->cancel_type = PTHREAD_CANCEL_DEFERRED;
      break;
    }

    case CPthreadReqTestcancel_Pre_CancelType_Asynchronous: {
      /*
       * While the thread cancel type attribute of the executing task is
       * PTHREAD_CANCEL_ASYNCHRONOUS.
       */
      ctx->cancel_type = PTHREAD_CANCEL_ASYNCHRONOUS;
      break;
    }

    case CPthreadReqTestcancel_Pre_CancelType_NA:
      break;
  }
}

static void CPthreadReqTestcancel_Pre_CancelPending_Prepare(
  CPthreadReqTestcancel_Context          *ctx,
  CPthreadReqTestcancel_Pre_CancelPending state
)
{
  switch ( state ) {
    case CPthreadReqTestcancel_Pre_CancelPending_Yes: {
      /*
       * While a thread cancellation request is pending for the executing task.
       */
      ctx->cancel_pending = true;
      break;
    }

    case CPthreadReqTestcancel_Pre_CancelPending_No: {
      /*
       * While no thread cancellation request is pending for the executing
       * task.
       */
      ctx->cancel_pending = false;
      break;
    }

    case CPthreadReqTestcancel_Pre_CancelPending_NA:
      break;
  }
}

static void CPthreadReqTestcancel_Pre_RestartPending_Prepare(
  CPthreadReqTestcancel_Context           *ctx,
  CPthreadReqTestcancel_Pre_RestartPending state
)
{
  switch ( state ) {
    case CPthreadReqTestcancel_Pre_RestartPending_Yes: {
      /*
       * While a restart of the executing task by rtems_task_restart() is
       * pending.
       */
      ctx->restart_pending = true;
      break;
    }

    case CPthreadReqTestcancel_Pre_RestartPending_No: {
      /*
       * While no restart of the executing task is pending.
       */
      ctx->restart_pending = false;
      break;
    }

    case CPthreadReqTestcancel_Pre_RestartPending_NA:
      break;
  }
}

static void CPthreadReqTestcancel_Post_Return_Check(
  CPthreadReqTestcancel_Context    *ctx,
  CPthreadReqTestcancel_Post_Return state
)
{
  switch ( state ) {
    case CPthreadReqTestcancel_Post_Return_Yes: {
      /*
       * The pthread_testcancel() call shall return to the caller.
       */
      T_true( ctx->returned );
      break;
    }

    case CPthreadReqTestcancel_Post_Return_No: {
      /*
       * The pthread_testcancel() call shall not return to the caller.
       */
      T_false( ctx->returned );
      break;
    }

    case CPthreadReqTestcancel_Post_Return_NA:
      break;
  }
}

static void CPthreadReqTestcancel_Post_Life_Check(
  CPthreadReqTestcancel_Context  *ctx,
  CPthreadReqTestcancel_Post_Life state
)
{
  switch ( state ) {
    case CPthreadReqTestcancel_Post_Life_Terminate: {
      /*
       * The executing task shall be terminated by a thread cancellation.
       */
      JoinWorker( ctx );
      T_eq_u32( ctx->restarts, 0 );
      break;
    }

    case CPthreadReqTestcancel_Post_Life_Restart: {
      /*
       * The executing task shall be restarted.
       */
      JoinWorker( ctx );
      T_eq_ptr( ctx->exit_value, &exit_object );
      T_eq_u32( ctx->restarts, 1 );
      break;
    }

    case CPthreadReqTestcancel_Post_Life_Nop: {
      /*
       * The executing task shall be neither terminated nor restarted by the
       * pthread_testcancel() call.
       */
      T_true( IsTaskSuspended( ctx->worker_id ) );
      T_eq_u32( ctx->restarts, 0 );
      break;
    }

    case CPthreadReqTestcancel_Post_Life_NA:
      break;
  }
}

static void CPthreadReqTestcancel_Post_ExitValue_Check(
  CPthreadReqTestcancel_Context       *ctx,
  CPthreadReqTestcancel_Post_ExitValue state
)
{
  switch ( state ) {
    case CPthreadReqTestcancel_Post_ExitValue_Canceled: {
      /*
       * The thread exit value of the executing task shall be PTHREAD_CANCELED.
       */
      T_eq_ptr( ctx->exit_value, PTHREAD_CANCELED );
      break;
    }

    case CPthreadReqTestcancel_Post_ExitValue_NA:
      break;
  }
}

static void CPthreadReqTestcancel_Post_CancelState_Check(
  CPthreadReqTestcancel_Context         *ctx,
  CPthreadReqTestcancel_Post_CancelState state
)
{
  switch ( state ) {
    case CPthreadReqTestcancel_Post_CancelState_Nop: {
      /*
       * The thread cancel state attribute of the executing task shall not be
       * modified by the pthread_testcancel() call.
       */
      T_eq_int( ctx->state_after, ctx->cancel_state );
      break;
    }

    case CPthreadReqTestcancel_Post_CancelState_NA:
      break;
  }
}

static void CPthreadReqTestcancel_Post_CancelType_Check(
  CPthreadReqTestcancel_Context        *ctx,
  CPthreadReqTestcancel_Post_CancelType state
)
{
  switch ( state ) {
    case CPthreadReqTestcancel_Post_CancelType_Nop: {
      /*
       * The thread cancel type attribute of the executing task shall not be
       * modified by the pthread_testcancel() call.
       */
      T_eq_int( ctx->type_after, ctx->cancel_type );
      break;
    }

    case CPthreadReqTestcancel_Post_CancelType_NA:
      break;
  }
}

static void CPthreadReqTestcancel_Post_Requests_Check(
  CPthreadReqTestcancel_Context      *ctx,
  CPthreadReqTestcancel_Post_Requests state
)
{
  switch ( state ) {
    case CPthreadReqTestcancel_Post_Requests_Nop: {
      /*
       * The pending thread cancellation request and the pending restart by
       * rtems_task_restart() of the executing task shall not be modified by
       * the pthread_testcancel() call.
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

    case CPthreadReqTestcancel_Post_Requests_NA:
      break;
  }
}

static void CPthreadReqTestcancel_Setup( void )
{
  SetSelfPriority( PRIO_NORMAL );
}

static void CPthreadReqTestcancel_Setup_Wrap( void *arg )
{
  CPthreadReqTestcancel_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CPthreadReqTestcancel_Setup();
}

static void CPthreadReqTestcancel_Teardown( void )
{
  RestoreRunnerPriority();
}

static void CPthreadReqTestcancel_Teardown_Wrap( void *arg )
{
  CPthreadReqTestcancel_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CPthreadReqTestcancel_Teardown();
}

static void CPthreadReqTestcancel_Prepare( CPthreadReqTestcancel_Context *ctx )
{
  ctx->phase = PHASE_ACTION;
  ctx->worker_started = false;
  ctx->restarts = 0;
  ctx->returned = false;
  ctx->late_returned = false;
  ctx->joined = false;
  ctx->exit_value = NULL;
  ctx->state_after = -1;
  ctx->type_after = -1;
}

static void CPthreadReqTestcancel_Action( CPthreadReqTestcancel_Context *ctx )
{
  ctx->worker_id = CreateTask( "WORK", PRIO_HIGH );
  StartTask( ctx->worker_id, Worker, ctx );
}

static void CPthreadReqTestcancel_Cleanup( CPthreadReqTestcancel_Context *ctx )
{
  if ( !ctx->joined ) {
    ctx->phase = PHASE_CLEANUP;
    ResumeTask( ctx->worker_id );
    T_eq_int( pthread_join( ctx->worker_thread, NULL ), 0 );
  }
}

/* clang-format off */

static const CPthreadReqTestcancel_Entry
CPthreadReqTestcancel_Entries[] = {
  { 0, 0, 0, 0, 0, 0, CPthreadReqTestcancel_Post_Return_Yes,
    CPthreadReqTestcancel_Post_Life_Nop,
    CPthreadReqTestcancel_Post_ExitValue_NA,
    CPthreadReqTestcancel_Post_CancelState_Nop,
    CPthreadReqTestcancel_Post_CancelType_Nop,
    CPthreadReqTestcancel_Post_Requests_Nop },
  { 1, 0, 0, 0, 0, 0, CPthreadReqTestcancel_Post_Return_NA,
    CPthreadReqTestcancel_Post_Life_NA,
    CPthreadReqTestcancel_Post_ExitValue_NA,
    CPthreadReqTestcancel_Post_CancelState_NA,
    CPthreadReqTestcancel_Post_CancelType_NA,
    CPthreadReqTestcancel_Post_Requests_NA },
  { 0, 0, 0, 0, 0, 0, CPthreadReqTestcancel_Post_Return_No,
    CPthreadReqTestcancel_Post_Life_Terminate,
    CPthreadReqTestcancel_Post_ExitValue_Canceled,
    CPthreadReqTestcancel_Post_CancelState_NA,
    CPthreadReqTestcancel_Post_CancelType_NA,
    CPthreadReqTestcancel_Post_Requests_NA },
  { 0, 0, 0, 0, 0, 0, CPthreadReqTestcancel_Post_Return_No,
    CPthreadReqTestcancel_Post_Life_Restart,
    CPthreadReqTestcancel_Post_ExitValue_NA,
    CPthreadReqTestcancel_Post_CancelState_NA,
    CPthreadReqTestcancel_Post_CancelType_NA,
    CPthreadReqTestcancel_Post_Requests_NA }
};

static const uint8_t
CPthreadReqTestcancel_Map[] = {
  2, 2, 3, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0,
  0, 0, 0, 0, 0, 0
};

/* clang-format on */

static size_t CPthreadReqTestcancel_Scope( void *arg, char *buf, size_t n )
{
  CPthreadReqTestcancel_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CPthreadReqTestcancel_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CPthreadReqTestcancel_Fixture = {
  .setup = CPthreadReqTestcancel_Setup_Wrap,
  .stop = NULL,
  .teardown = CPthreadReqTestcancel_Teardown_Wrap,
  .scope = CPthreadReqTestcancel_Scope,
  .initial_context = &CPthreadReqTestcancel_Instance
};

static inline CPthreadReqTestcancel_Entry CPthreadReqTestcancel_PopEntry(
  CPthreadReqTestcancel_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqTestcancel_Entries[ CPthreadReqTestcancel_Map[ index ] ];
}

static void CPthreadReqTestcancel_TestVariant(
  CPthreadReqTestcancel_Context *ctx
)
{
  CPthreadReqTestcancel_Pre_Context_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqTestcancel_Pre_CancelState_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqTestcancel_Pre_CancelType_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CPthreadReqTestcancel_Pre_CancelPending_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CPthreadReqTestcancel_Pre_RestartPending_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  CPthreadReqTestcancel_Action( ctx );
  CPthreadReqTestcancel_Post_Return_Check( ctx, ctx->Map.entry.Post_Return );
  CPthreadReqTestcancel_Post_Life_Check( ctx, ctx->Map.entry.Post_Life );
  CPthreadReqTestcancel_Post_ExitValue_Check(
    ctx,
    ctx->Map.entry.Post_ExitValue
  );
  CPthreadReqTestcancel_Post_CancelState_Check(
    ctx,
    ctx->Map.entry.Post_CancelState
  );
  CPthreadReqTestcancel_Post_CancelType_Check(
    ctx,
    ctx->Map.entry.Post_CancelType
  );
  CPthreadReqTestcancel_Post_Requests_Check(
    ctx,
    ctx->Map.entry.Post_Requests
  );
}

/**
 * @fn void T_case_body_CPthreadReqTestcancel( void )
 */
T_TEST_CASE_FIXTURE( CPthreadReqTestcancel, &CPthreadReqTestcancel_Fixture )
{
  CPthreadReqTestcancel_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CPthreadReqTestcancel_Pre_Context_Task;
    ctx->Map.pcs[ 0 ] < CPthreadReqTestcancel_Pre_Context_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CPthreadReqTestcancel_Pre_CancelState_Enable;
      ctx->Map.pcs[ 1 ] < CPthreadReqTestcancel_Pre_CancelState_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = CPthreadReqTestcancel_Pre_CancelType_Deferred;
        ctx->Map.pcs[ 2 ] < CPthreadReqTestcancel_Pre_CancelType_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        for (
          ctx->Map.pcs[ 3 ] = CPthreadReqTestcancel_Pre_CancelPending_Yes;
          ctx->Map.pcs[ 3 ] < CPthreadReqTestcancel_Pre_CancelPending_NA;
          ++ctx->Map.pcs[ 3 ]
        ) {
          for (
            ctx->Map.pcs[ 4 ] = CPthreadReqTestcancel_Pre_RestartPending_Yes;
            ctx->Map.pcs[ 4 ] < CPthreadReqTestcancel_Pre_RestartPending_NA;
            ++ctx->Map.pcs[ 4 ]
          ) {
            ctx->Map.entry = CPthreadReqTestcancel_PopEntry( ctx );

            if ( ctx->Map.entry.Skip ) {
              continue;
            }

            CPthreadReqTestcancel_Prepare( ctx );
            CPthreadReqTestcancel_TestVariant( ctx );
            CPthreadReqTestcancel_Cleanup( ctx );
          }
        }
      }
    }
  }
}

/** @} */
