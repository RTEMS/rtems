/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsTaskReqExit
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
#include <rtems/score/apimutex.h>
#include <rtems/score/statesimpl.h>
#include <rtems/score/threaddispatch.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsTaskReqExit spec:/rtems/task/req/exit
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsTaskReqExit_Pre_Restarting_Yes,
  RtemsTaskReqExit_Pre_Restarting_No,
  RtemsTaskReqExit_Pre_Restarting_NA
} RtemsTaskReqExit_Pre_Restarting;

typedef enum {
  RtemsTaskReqExit_Pre_Terminating_Yes,
  RtemsTaskReqExit_Pre_Terminating_No,
  RtemsTaskReqExit_Pre_Terminating_NA
} RtemsTaskReqExit_Pre_Terminating;

typedef enum {
  RtemsTaskReqExit_Pre_Protected_Yes,
  RtemsTaskReqExit_Pre_Protected_No,
  RtemsTaskReqExit_Pre_Protected_NA
} RtemsTaskReqExit_Pre_Protected;

typedef enum {
  RtemsTaskReqExit_Pre_ThreadDispatch_Enabled,
  RtemsTaskReqExit_Pre_ThreadDispatch_Disabled,
  RtemsTaskReqExit_Pre_ThreadDispatch_NA
} RtemsTaskReqExit_Pre_ThreadDispatch;

typedef enum {
  RtemsTaskReqExit_Post_FatalError_Yes,
  RtemsTaskReqExit_Post_FatalError_Nop,
  RtemsTaskReqExit_Post_FatalError_NA
} RtemsTaskReqExit_Post_FatalError;

typedef enum {
  RtemsTaskReqExit_Post_DeleteExtensions_Nop,
  RtemsTaskReqExit_Post_DeleteExtensions_NA
} RtemsTaskReqExit_Post_DeleteExtensions;

typedef enum {
  RtemsTaskReqExit_Post_RestartExtensions_Nop,
  RtemsTaskReqExit_Post_RestartExtensions_NA
} RtemsTaskReqExit_Post_RestartExtensions;

typedef enum {
  RtemsTaskReqExit_Post_TerminateExtensions_Yes,
  RtemsTaskReqExit_Post_TerminateExtensions_Nop,
  RtemsTaskReqExit_Post_TerminateExtensions_NA
} RtemsTaskReqExit_Post_TerminateExtensions;

typedef enum {
  RtemsTaskReqExit_Post_Zombie_Yes,
  RtemsTaskReqExit_Post_Zombie_No,
  RtemsTaskReqExit_Post_Zombie_NA
} RtemsTaskReqExit_Post_Zombie;

typedef enum {
  RtemsTaskReqExit_Post_ID_Valid,
  RtemsTaskReqExit_Post_ID_Invalid,
  RtemsTaskReqExit_Post_ID_NA
} RtemsTaskReqExit_Post_ID;

typedef enum {
  RtemsTaskReqExit_Post_Delete_NextAllocate,
  RtemsTaskReqExit_Post_Delete_Nop,
  RtemsTaskReqExit_Post_Delete_NA
} RtemsTaskReqExit_Post_Delete;

typedef struct {
  uint32_t Skip : 1;
  uint32_t Pre_Restarting_NA : 1;
  uint32_t Pre_Terminating_NA : 1;
  uint32_t Pre_Protected_NA : 1;
  uint32_t Pre_ThreadDispatch_NA : 1;
  uint32_t Post_FatalError : 2;
  uint32_t Post_DeleteExtensions : 1;
  uint32_t Post_RestartExtensions : 1;
  uint32_t Post_TerminateExtensions : 2;
  uint32_t Post_Zombie : 2;
  uint32_t Post_ID : 2;
  uint32_t Post_Delete : 2;
} RtemsTaskReqExit_Entry;

/**
 * @brief Test context for spec:/rtems/task/req/exit test case.
 */
typedef struct {
  /**
   * @brief This member provides the scheduler operation records.
   */
  T_scheduler_log_4 scheduler_log;

  /**
   * @brief This member contains the identifier of the runner task.
   */
  rtems_id runner_id;

  /**
   * @brief This member contains the identifier of the worker task.
   */
  rtems_id worker_id;

  /**
   * @brief This member contains the identifier of the deleter task.
   */
  rtems_id deleter_id;

  /**
   * @brief This member contains the identifier of the test user extensions.
   */
  rtems_id extension_id;

  /**
   * @brief This member contains the count of fatal extension calls.
   */
  uint32_t fatal_extension_calls;

  /**
   * @brief This member contains the count of thread delete extension calls.
   */
  uint32_t delete_extension_calls;

  /**
   * @brief This member contains the count of thread restart extension calls.
   */
  uint32_t restart_extension_calls;

  /**
   * @brief This member contains the count of thread terminate extension calls.
   */
  uint32_t terminate_extension_calls;

  /**
   * @brief If this member is true, then the thread life of the worker is
   *   protected before the rtems_task_exit() call.
   */
  bool protected;

  /**
   * @brief If this member is true, then the worker locked the allocator.
   */
  bool allocator_locked;

  /**
   * @brief If this member is true, then the worker is restarting before the
   *   rtems_task_exit() call.
   */
  bool restarting;

  /**
   * @brief If this member is true, then the worker is terminating before the
   *   rtems_task_exit() call.
   */
  bool terminating;

  /**
   * @brief If this member is true, then thread dispatching is disabled by the
   *   worker task before the rtems_task_exit() call.
   */
  bool dispatch_disabled;

  /**
   * @brief If this member is true, then it is expected to delete the worker.
   */
  bool delete_worker_expected;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 4 ];

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
    RtemsTaskReqExit_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsTaskReqExit_Context;

static RtemsTaskReqExit_Context
  RtemsTaskReqExit_Instance;

static const char * const RtemsTaskReqExit_PreDesc_Restarting[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const RtemsTaskReqExit_PreDesc_Terminating[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const RtemsTaskReqExit_PreDesc_Protected[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const RtemsTaskReqExit_PreDesc_ThreadDispatch[] = {
  "Enabled",
  "Disabled",
  "NA"
};

static const char * const * const RtemsTaskReqExit_PreDesc[] = {
  RtemsTaskReqExit_PreDesc_Restarting,
  RtemsTaskReqExit_PreDesc_Terminating,
  RtemsTaskReqExit_PreDesc_Protected,
  RtemsTaskReqExit_PreDesc_ThreadDispatch,
  NULL
};

typedef RtemsTaskReqExit_Context Context;

static void Signal( rtems_signal_set signals )
{
  Context          *ctx;
  T_scheduler_log  *log;
  Thread_Life_state life_state;

  (void) signals;
  ctx = T_fixture_context();

  if ( ctx->dispatch_disabled ) {
    _Thread_Dispatch_disable();
  }

  /* Check that the thread life state was prepared correctly */
  life_state = GetExecuting()->Life.state;
  T_eq( ctx->protected, ( life_state & THREAD_LIFE_PROTECTED ) != 0 );
  T_eq( ctx->restarting, ( life_state & THREAD_LIFE_RESTARTING ) != 0 );
  T_eq( ctx->terminating, ( life_state & THREAD_LIFE_TERMINATING ) != 0 );

  log = T_scheduler_record_4( &ctx->scheduler_log );
  T_null( log );

  ctx->delete_extension_calls = 0;
  ctx->fatal_extension_calls = 0;
  ctx->restart_extension_calls = 0;
  ctx->terminate_extension_calls = 0;

  rtems_task_exit();
}

static void Deleter( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;

  if ( ctx != NULL ) {
    DeleteTask( ctx->worker_id );
  }

  SuspendSelf();
}

static void Worker( rtems_task_argument arg )
{
  Context          *ctx;
  rtems_status_code sc;

  ctx = (Context *) arg;

  sc = rtems_signal_catch( Signal, RTEMS_NO_ASR );
  T_rsc_success( sc );

  if ( ctx->protected ) {
    _RTEMS_Lock_allocator();
    ctx->allocator_locked = true;
  }

  Yield();
}

static void UnlockAllocator( Context *ctx )
{
  if ( ctx->allocator_locked ) {
    ctx->allocator_locked = false;
    _RTEMS_Unlock_allocator();
  }
}

static void Fatal(
  rtems_fatal_source source,
  rtems_fatal_code   code,
  void              *arg
)
{
  Context         *ctx;
  T_scheduler_log *log;
  Per_CPU_Control *cpu_self;

  ctx = arg;
  ++ctx->fatal_extension_calls;

  T_eq_int( source, INTERNAL_ERROR_CORE );
  T_eq_ulong( code, INTERNAL_ERROR_BAD_THREAD_DISPATCH_DISABLE_LEVEL );
  T_assert_eq_int( ctx->fatal_extension_calls, 1 );

  log = T_scheduler_record( NULL );
  T_eq_ptr( &log->header, &ctx->scheduler_log.header );

  UnlockAllocator( ctx );
  SuspendSelf();

  cpu_self = _Per_CPU_Get();
  _Thread_Dispatch_unnest( cpu_self );
  _Thread_Dispatch_direct_no_return( cpu_self );
}

static void ThreadDelete( rtems_tcb *executing, rtems_tcb *deleted )
{
  Context *ctx;

  ctx = T_fixture_context();
  ++ctx->delete_extension_calls;

  T_eq_u32( executing->Object.id, ctx->runner_id );

  if ( ctx->delete_worker_expected ) {
    T_eq_u32( deleted->Object.id, ctx->worker_id );
  }
}

static void ThreadRestart( rtems_tcb *executing, rtems_tcb *restarted )
{
  Context *ctx;

  ctx = T_fixture_context();
  ++ctx->restart_extension_calls;
}

static void ThreadTerminate( rtems_tcb *executing )
{
  Context *ctx;

  ctx = T_fixture_context();
  ++ctx->terminate_extension_calls;

  T_eq_u32( executing->Object.id, ctx->worker_id );

  UnlockAllocator( ctx );
}

static const rtems_extensions_table extensions = {
  .thread_delete = ThreadDelete,
  .thread_restart = ThreadRestart,
  .thread_terminate = ThreadTerminate
};

static void RtemsTaskReqExit_Pre_Restarting_Prepare(
  RtemsTaskReqExit_Context       *ctx,
  RtemsTaskReqExit_Pre_Restarting state
)
{
  switch ( state ) {
    case RtemsTaskReqExit_Pre_Restarting_Yes: {
      /*
       * While the calling task is restarting.
       */
      ctx->restarting = true;
      break;
    }

    case RtemsTaskReqExit_Pre_Restarting_No: {
      /*
       * While the calling task is not restarting.
       */
      ctx->restarting = false;
      break;
    }

    case RtemsTaskReqExit_Pre_Restarting_NA:
      break;
  }
}

static void RtemsTaskReqExit_Pre_Terminating_Prepare(
  RtemsTaskReqExit_Context        *ctx,
  RtemsTaskReqExit_Pre_Terminating state
)
{
  switch ( state ) {
    case RtemsTaskReqExit_Pre_Terminating_Yes: {
      /*
       * While the calling task is terminating.
       */
      ctx->terminating = true;
      break;
    }

    case RtemsTaskReqExit_Pre_Terminating_No: {
      /*
       * While the calling task is not terminating.
       */
      ctx->terminating = false;
      break;
    }

    case RtemsTaskReqExit_Pre_Terminating_NA:
      break;
  }
}

static void RtemsTaskReqExit_Pre_Protected_Prepare(
  RtemsTaskReqExit_Context      *ctx,
  RtemsTaskReqExit_Pre_Protected state
)
{
  switch ( state ) {
    case RtemsTaskReqExit_Pre_Protected_Yes: {
      /*
       * While the thread life of the calling task is protected.
       */
      ctx->protected = true;
      break;
    }

    case RtemsTaskReqExit_Pre_Protected_No: {
      /*
       * While the thread life of the calling task is not protected.
       */
      ctx->protected = false;
      break;
    }

    case RtemsTaskReqExit_Pre_Protected_NA:
      break;
  }
}

static void RtemsTaskReqExit_Pre_ThreadDispatch_Prepare(
  RtemsTaskReqExit_Context           *ctx,
  RtemsTaskReqExit_Pre_ThreadDispatch state
)
{
  switch ( state ) {
    case RtemsTaskReqExit_Pre_ThreadDispatch_Enabled: {
      /*
       * While thread dispatching is enabled for the calling task.
       */
      ctx->dispatch_disabled = false;
      break;
    }

    case RtemsTaskReqExit_Pre_ThreadDispatch_Disabled: {
      /*
       * While thread dispatching is disabled for the calling task.
       */
      ctx->dispatch_disabled = true;
      break;
    }

    case RtemsTaskReqExit_Pre_ThreadDispatch_NA:
      break;
  }
}

static void RtemsTaskReqExit_Post_FatalError_Check(
  RtemsTaskReqExit_Context        *ctx,
  RtemsTaskReqExit_Post_FatalError state
)
{
  switch ( state ) {
    case RtemsTaskReqExit_Post_FatalError_Yes: {
      /*
       * The fatal error with a fatal source of INTERNAL_ERROR_CORE and a fatal
       * code of INTERNAL_ERROR_BAD_THREAD_DISPATCH_DISABLE_LEVEL shall occur
       * by the rtems_task_exit() call.
       */
      T_eq_u32( ctx->fatal_extension_calls, 1 );
      break;
    }

    case RtemsTaskReqExit_Post_FatalError_Nop: {
      /*
       * No fatal error shall occur by the rtems_task_exit() call.
       */
      T_eq_u32( ctx->fatal_extension_calls, 0 );
      break;
    }

    case RtemsTaskReqExit_Post_FatalError_NA:
      break;
  }
}

static void RtemsTaskReqExit_Post_DeleteExtensions_Check(
  RtemsTaskReqExit_Context              *ctx,
  RtemsTaskReqExit_Post_DeleteExtensions state
)
{
  switch ( state ) {
    case RtemsTaskReqExit_Post_DeleteExtensions_Nop: {
      /*
       * The thread delete user extensions shall not be invoked by the
       * rtems_task_exit() call.
       */
      T_eq_u32( ctx->delete_extension_calls, 0 );
      break;
    }

    case RtemsTaskReqExit_Post_DeleteExtensions_NA:
      break;
  }
}

static void RtemsTaskReqExit_Post_RestartExtensions_Check(
  RtemsTaskReqExit_Context               *ctx,
  RtemsTaskReqExit_Post_RestartExtensions state
)
{
  switch ( state ) {
    case RtemsTaskReqExit_Post_RestartExtensions_Nop: {
      /*
       * The thread restart user extensions shall not be invoked by the
       * rtems_task_exit() call.
       */
      T_eq_u32( ctx->restart_extension_calls, 0 );
      break;
    }

    case RtemsTaskReqExit_Post_RestartExtensions_NA:
      break;
  }
}

static void RtemsTaskReqExit_Post_TerminateExtensions_Check(
  RtemsTaskReqExit_Context                 *ctx,
  RtemsTaskReqExit_Post_TerminateExtensions state
)
{
  switch ( state ) {
    case RtemsTaskReqExit_Post_TerminateExtensions_Yes: {
      /*
       * The thread terminate user extensions shall be invoked by the
       * rtems_task_exit() call.
       */
      if ( ctx->protected ) {
        T_eq_u32( ctx->terminate_extension_calls, 2 );
      } else {
        T_eq_u32( ctx->terminate_extension_calls, 1 );
      }
      break;
    }

    case RtemsTaskReqExit_Post_TerminateExtensions_Nop: {
      /*
       * The thread terminate user extensions shall not be invoked by the
       * rtems_task_exit() call.
       */
      T_eq_u32( ctx->terminate_extension_calls, 0 );
      break;
    }

    case RtemsTaskReqExit_Post_TerminateExtensions_NA:
      break;
  }
}

static void RtemsTaskReqExit_Post_Zombie_Check(
  RtemsTaskReqExit_Context    *ctx,
  RtemsTaskReqExit_Post_Zombie state
)
{
  const T_scheduler_event *event;
  size_t                   index;

  index = 0;

  switch ( state ) {
    case RtemsTaskReqExit_Post_Zombie_Yes: {
      /*
       * The thread state of the calling task shall be set to the zombie state
       * by the rtems_task_exit() call.
       */
      event = T_scheduler_next_any( &ctx->scheduler_log.header, &index );
      T_eq_int( event->operation, T_SCHEDULER_BLOCK );
      T_eq_u32( event->thread->Object.id, ctx->worker_id );
      T_eq_u32( event->thread->current_state, STATES_ZOMBIE );

      if ( ctx->terminating ) {
        /* The thread waiting for the worker exit was unblocked */
        event = T_scheduler_next_any( &ctx->scheduler_log.header, &index );
        T_eq_int( event->operation, T_SCHEDULER_UNBLOCK );
        T_eq_u32( event->thread->Object.id, ctx->deleter_id );

        /* Inherited priority was removed */
        event = T_scheduler_next_any( &ctx->scheduler_log.header, &index );
        T_eq_int( event->operation, T_SCHEDULER_UPDATE_PRIORITY );
        T_eq_u32( event->thread->Object.id, ctx->worker_id );

        /* The deleter task suspended itself */
        event = T_scheduler_next_any( &ctx->scheduler_log.header, &index );
        T_eq_int( event->operation, T_SCHEDULER_BLOCK );
        T_eq_u32( event->thread->Object.id, ctx->deleter_id );
      }

      event = T_scheduler_next_any( &ctx->scheduler_log.header, &index );
      T_eq_int( event->operation, T_SCHEDULER_NOP );
      break;
    }

    case RtemsTaskReqExit_Post_Zombie_No: {
      /*
       * The thread state of the calling task shall be not modified by the
       * rtems_task_exit() call.
       */
      T_eq_sz( ctx->scheduler_log.header.recorded, 0 );
      break;
    }

    case RtemsTaskReqExit_Post_Zombie_NA:
      break;
  }
}

static void RtemsTaskReqExit_Post_ID_Check(
  RtemsTaskReqExit_Context *ctx,
  RtemsTaskReqExit_Post_ID  state
)
{
  rtems_status_code sc;
  rtems_id          id;

  sc = rtems_task_get_scheduler( ctx->worker_id, &id );

  switch ( state ) {
    case RtemsTaskReqExit_Post_ID_Valid: {
      /*
       * The object identifier of the calling task shall be valid.
       */
      T_rsc_success( sc );
      break;
    }

    case RtemsTaskReqExit_Post_ID_Invalid: {
      /*
       * The object identifier of the calling task shall be invalid.
       */
      T_rsc( sc, RTEMS_INVALID_ID );
      break;
    }

    case RtemsTaskReqExit_Post_ID_NA:
      break;
  }
}

static void RtemsTaskReqExit_Post_Delete_Check(
  RtemsTaskReqExit_Context    *ctx,
  RtemsTaskReqExit_Post_Delete state
)
{
  rtems_id id;

  id = CreateTask( "TEMP", PRIO_LOW );

  switch ( state ) {
    case RtemsTaskReqExit_Post_Delete_NextAllocate: {
      /*
       * The calling task shall be deleted by the next directive which
       * allocates a task.
       */
      T_eq_u32( ctx->delete_extension_calls, 1 );
      break;
    }

    case RtemsTaskReqExit_Post_Delete_Nop: {
      /*
       * The calling task shall not be deleted by the next directive which
       * allocates a task.
       */
      T_eq_u32( ctx->delete_extension_calls, 0 );
      break;
    }

    case RtemsTaskReqExit_Post_Delete_NA:
      break;
  }

  DeleteTask( id );
}

static void RtemsTaskReqExit_Setup( RtemsTaskReqExit_Context *ctx )
{
  rtems_status_code sc;

  ctx->runner_id = rtems_task_self();

  sc = rtems_extension_create(
    rtems_build_name( 'T', 'E', 'S', 'T' ),
    &extensions,
    &ctx->extension_id
  );
  T_rsc_success( sc );

  SetFatalHandler( Fatal, ctx );
  SetSelfPriority( PRIO_NORMAL );

  ctx->deleter_id = CreateTask( "DELE", PRIO_HIGH );
  StartTask( ctx->deleter_id, Deleter, NULL );
}

static void RtemsTaskReqExit_Setup_Wrap( void *arg )
{
  RtemsTaskReqExit_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsTaskReqExit_Setup( ctx );
}

static void RtemsTaskReqExit_Teardown( RtemsTaskReqExit_Context *ctx )
{
  rtems_status_code sc;

  sc = rtems_extension_delete( ctx->extension_id );
  T_rsc_success( sc );

  SetFatalHandler( NULL, NULL );
  DeleteTask( ctx->deleter_id );
  RestoreRunnerASR();
  RestoreRunnerPriority();
}

static void RtemsTaskReqExit_Teardown_Wrap( void *arg )
{
  RtemsTaskReqExit_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsTaskReqExit_Teardown( ctx );
}

static void RtemsTaskReqExit_Action( RtemsTaskReqExit_Context *ctx )
{
  rtems_status_code sc;

  ctx->delete_worker_expected = false;
  ctx->worker_id = CreateTask( "WORK", PRIO_NORMAL );
  ctx->delete_worker_expected = true;

  StartTask( ctx->worker_id, Worker, ctx );

  /* Let the worker catch signals and set the thread life protection state */
  Yield();

  sc = rtems_signal_send( ctx->worker_id, RTEMS_SIGNAL_0 );
  T_rsc_success( sc );

  if ( ctx->restarting ) {
    sc = rtems_task_restart( ctx->worker_id, (rtems_task_argument) ctx );
    T_rsc_success( sc );
  }

  if ( ctx->terminating ) {
    sc = rtems_task_restart( ctx->deleter_id, (rtems_task_argument) ctx );
    T_rsc_success( sc );
  } else {
    Yield();
  }

  if ( !ctx->dispatch_disabled ) {
    T_scheduler_log *log;

    log = T_scheduler_record( NULL );
    T_eq_ptr( &log->header, &ctx->scheduler_log.header );
  }
}

static void RtemsTaskReqExit_Cleanup( RtemsTaskReqExit_Context *ctx )
{
  if ( ctx->dispatch_disabled ) {
    DeleteTask( ctx->worker_id );
  }
}

static const RtemsTaskReqExit_Entry
RtemsTaskReqExit_Entries[] = {
  { 0, 0, 0, 0, 0, RtemsTaskReqExit_Post_FatalError_Nop,
    RtemsTaskReqExit_Post_DeleteExtensions_Nop,
    RtemsTaskReqExit_Post_RestartExtensions_Nop,
    RtemsTaskReqExit_Post_TerminateExtensions_Yes,
    RtemsTaskReqExit_Post_Zombie_Yes, RtemsTaskReqExit_Post_ID_Invalid,
    RtemsTaskReqExit_Post_Delete_NextAllocate },
  { 0, 0, 0, 0, 0, RtemsTaskReqExit_Post_FatalError_Yes,
    RtemsTaskReqExit_Post_DeleteExtensions_Nop,
    RtemsTaskReqExit_Post_RestartExtensions_Nop,
    RtemsTaskReqExit_Post_TerminateExtensions_Nop,
    RtemsTaskReqExit_Post_Zombie_No, RtemsTaskReqExit_Post_ID_Valid,
    RtemsTaskReqExit_Post_Delete_Nop }
};

static const uint8_t
RtemsTaskReqExit_Map[] = {
  0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1
};

static size_t RtemsTaskReqExit_Scope( void *arg, char *buf, size_t n )
{
  RtemsTaskReqExit_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsTaskReqExit_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsTaskReqExit_Fixture = {
  .setup = RtemsTaskReqExit_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsTaskReqExit_Teardown_Wrap,
  .scope = RtemsTaskReqExit_Scope,
  .initial_context = &RtemsTaskReqExit_Instance
};

static inline RtemsTaskReqExit_Entry RtemsTaskReqExit_PopEntry(
  RtemsTaskReqExit_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsTaskReqExit_Entries[
    RtemsTaskReqExit_Map[ index ]
  ];
}

static void RtemsTaskReqExit_TestVariant( RtemsTaskReqExit_Context *ctx )
{
  RtemsTaskReqExit_Pre_Restarting_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsTaskReqExit_Pre_Terminating_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsTaskReqExit_Pre_Protected_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsTaskReqExit_Pre_ThreadDispatch_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  RtemsTaskReqExit_Action( ctx );
  RtemsTaskReqExit_Post_FatalError_Check(
    ctx,
    ctx->Map.entry.Post_FatalError
  );
  RtemsTaskReqExit_Post_DeleteExtensions_Check(
    ctx,
    ctx->Map.entry.Post_DeleteExtensions
  );
  RtemsTaskReqExit_Post_RestartExtensions_Check(
    ctx,
    ctx->Map.entry.Post_RestartExtensions
  );
  RtemsTaskReqExit_Post_TerminateExtensions_Check(
    ctx,
    ctx->Map.entry.Post_TerminateExtensions
  );
  RtemsTaskReqExit_Post_Zombie_Check( ctx, ctx->Map.entry.Post_Zombie );
  RtemsTaskReqExit_Post_ID_Check( ctx, ctx->Map.entry.Post_ID );
  RtemsTaskReqExit_Post_Delete_Check( ctx, ctx->Map.entry.Post_Delete );
}

/**
 * @fn void T_case_body_RtemsTaskReqExit( void )
 */
T_TEST_CASE_FIXTURE( RtemsTaskReqExit, &RtemsTaskReqExit_Fixture )
{
  RtemsTaskReqExit_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsTaskReqExit_Pre_Restarting_Yes;
    ctx->Map.pcs[ 0 ] < RtemsTaskReqExit_Pre_Restarting_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = RtemsTaskReqExit_Pre_Terminating_Yes;
      ctx->Map.pcs[ 1 ] < RtemsTaskReqExit_Pre_Terminating_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = RtemsTaskReqExit_Pre_Protected_Yes;
        ctx->Map.pcs[ 2 ] < RtemsTaskReqExit_Pre_Protected_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        for (
          ctx->Map.pcs[ 3 ] = RtemsTaskReqExit_Pre_ThreadDispatch_Enabled;
          ctx->Map.pcs[ 3 ] < RtemsTaskReqExit_Pre_ThreadDispatch_NA;
          ++ctx->Map.pcs[ 3 ]
        ) {
          ctx->Map.entry = RtemsTaskReqExit_PopEntry( ctx );
          RtemsTaskReqExit_TestVariant( ctx );
          RtemsTaskReqExit_Cleanup( ctx );
        }
      }
    }
  }
}

/** @} */
