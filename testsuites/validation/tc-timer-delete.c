/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsTimerReqDelete
 */

/*
 * Copyright (C) 2021, 2026 embedded brains GmbH & Co. KG
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

#include <rtems.h>
#include <string.h>
#include <rtems/score/statesimpl.h>
#include <rtems/score/threadimpl.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsTimerReqDelete spec:/rtems/timer/req/delete
 *
 * @ingroup TestsuitesValidationNoClock1
 *
 * @{
 */

typedef enum {
  RtemsTimerReqDelete_Pre_Id_NoObj,
  RtemsTimerReqDelete_Pre_Id_Timer,
  RtemsTimerReqDelete_Pre_Id_NA
} RtemsTimerReqDelete_Pre_Id;

typedef enum {
  RtemsTimerReqDelete_Pre_Name_Unique,
  RtemsTimerReqDelete_Pre_Name_Found,
  RtemsTimerReqDelete_Pre_Name_Shadowed,
  RtemsTimerReqDelete_Pre_Name_NA
} RtemsTimerReqDelete_Pre_Name;

typedef enum {
  RtemsTimerReqDelete_Pre_Server_None,
  RtemsTimerReqDelete_Pre_Server_Idle,
  RtemsTimerReqDelete_Pre_Server_Tickle,
  RtemsTimerReqDelete_Pre_Server_NA
} RtemsTimerReqDelete_Pre_Server;

typedef enum {
  RtemsTimerReqDelete_Pre_Caller_Other,
  RtemsTimerReqDelete_Pre_Caller_Server,
  RtemsTimerReqDelete_Pre_Caller_NA
} RtemsTimerReqDelete_Pre_Caller;

typedef enum {
  RtemsTimerReqDelete_Post_Status_Ok,
  RtemsTimerReqDelete_Post_Status_InvId,
  RtemsTimerReqDelete_Post_Status_NA
} RtemsTimerReqDelete_Post_Status;

typedef enum {
  RtemsTimerReqDelete_Post_Name_None,
  RtemsTimerReqDelete_Post_Name_Other,
  RtemsTimerReqDelete_Post_Name_NA
} RtemsTimerReqDelete_Post_Name;

typedef enum {
  RtemsTimerReqDelete_Post_WaitForServer_Yes,
  RtemsTimerReqDelete_Post_WaitForServer_No,
  RtemsTimerReqDelete_Post_WaitForServer_NA
} RtemsTimerReqDelete_Post_WaitForServer;

typedef enum {
  RtemsTimerReqDelete_Post_YieldAllocMtx_Yes,
  RtemsTimerReqDelete_Post_YieldAllocMtx_No,
  RtemsTimerReqDelete_Post_YieldAllocMtx_NA
} RtemsTimerReqDelete_Post_YieldAllocMtx;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Id_NA : 1;
  uint16_t Pre_Name_NA : 1;
  uint16_t Pre_Server_NA : 1;
  uint16_t Pre_Caller_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_Name : 2;
  uint16_t Post_WaitForServer : 2;
  uint16_t Post_YieldAllocMtx : 2;
} RtemsTimerReqDelete_Entry;

/**
 * @brief Test context for spec:/rtems/timer/req/delete test case.
 */
typedef struct {
  rtems_id timer_id;

  rtems_id id;

  rtems_status_code status;

  /**
   * @brief If this member is true, then the parameter of the directive is the
   *   identifier of the timer.
   */
  Atomic_Uint valid_id;

  /**
   * @brief This member contains the identifier of a second timer which carries
   *   the object name of the timer.
   */
  rtems_id other_id;

  /**
   * @brief This member contains the identifier of the worker task.
   */
  rtems_id worker_id;

  /**
   * @brief This member contains the identifier of the Timer Server task.
   */
  rtems_id server_id;

  /**
   * @brief This member contains the identifier of the task which calls the
   *   directive.
   */
  rtems_id caller_id;

  /**
   * @brief If this member is true, then the Timer Server task runs a Timer
   *   Service Routine while the directive runs.
   */
  Atomic_Uint tickle;

  /**
   * @brief If this member is true, then the Timer Service Routine calls the
   *   directive.
   */
  Atomic_Uint self_delete;

  /**
   * @brief If this member is true, then the caller reached the wait for the
   *   Timer Server task.
   */
  Atomic_Uint waited;

  /**
   * @brief This member stays clear. A wait which takes no flag names it.
   */
  Atomic_Uint never;

  /**
   * @brief If this member is true, then the Timer Server task reached the
   *   window of the test.
   */
  Atomic_Uint window_reached;

  /**
   * @brief If this member is true, then a wait of the window reached its
   *   bound.
   */
  Atomic_Uint window_timed_out;

  /**
   * @brief If this member is true, then the delete of the worker returned.
   */
  Atomic_Uint delete_returned;

  /**
   * @brief If this member is true, then the delete of the timer ended.
   */
  Atomic_Uint deleted;

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 4 ];

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
    RtemsTimerReqDelete_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsTimerReqDelete_Context;

static RtemsTimerReqDelete_Context RtemsTimerReqDelete_Instance;

static const char *const RtemsTimerReqDelete_PreDesc_Id[] =
  { "NoObj", "Timer", "NA" };

static const char *const RtemsTimerReqDelete_PreDesc_Name[] =
  { "Unique", "Found", "Shadowed", "NA" };

static const char *const RtemsTimerReqDelete_PreDesc_Server[] =
  { "None", "Idle", "Tickle", "NA" };

static const char *const RtemsTimerReqDelete_PreDesc_Caller[] =
  { "Other", "Server", "NA" };

static const char *const *const RtemsTimerReqDelete_PreDesc[] = {
  RtemsTimerReqDelete_PreDesc_Id,
  RtemsTimerReqDelete_PreDesc_Name,
  RtemsTimerReqDelete_PreDesc_Server,
  RtemsTimerReqDelete_PreDesc_Caller,
  NULL
};

#define NAME rtems_build_name( 'T', 'E', 'S', 'T' )

typedef RtemsTimerReqDelete_Context Context;

static Context *delete_ctx;

/*
 * The window of the test.  The Timer Server task took the timer off its
 * chain and calls this routine outside the lock of the server.
 */
static void Routine( rtems_id id, void *arg )
{
  Context *ctx;

  (void) id;
  ctx = (Context *) arg;
  SetFlag( &ctx->window_reached, 1 );

  if ( GetFlag( &ctx->self_delete ) != 0 ) {
    ctx->status = rtems_timer_delete( ctx->id );

    /* The store publishes the status to the runner. */
    SetFlag( &ctx->deleted, 1 );
  } else {
    SendEvents( ctx->worker_id, RTEMS_EVENT_0 );
    (void) ReceiveAnyEvents();
  }
}

static void Worker( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;

  while ( true ) {
    /* The routine of the timer opens the window of the test. */
    (void) ReceiveAnyEvents();

    ctx->status = rtems_timer_delete( ctx->id );
    SetFlag( &ctx->delete_returned, 1 );

    /* The store publishes the status to the runner. */
    SetFlag( &ctx->deleted, 1 );
  }
}

/*
 * A timer which the Timer Server task held keeps a state which the cancel
 * of the directive reads through that task.  A case which runs without the
 * task therefore takes a timer which the task never held.
 */
static void FreshTimer( Context *ctx )
{
  rtems_status_code sc;

  if ( ctx->timer_id != 0 ) {
    sc = rtems_timer_delete( ctx->timer_id );
    T_rsc_success( sc );
  }

  sc = rtems_timer_create( NAME, &ctx->timer_id );
  T_rsc_success( sc );
}

static void DropOther( Context *ctx )
{
  if ( ctx->other_id != 0 ) {
    rtems_status_code sc;

    sc = rtems_timer_delete( ctx->other_id );
    T_rsc_success( sc );
    ctx->other_id = 0;
  }
}

/*
 * The two timers carry one object name and no other difference, so the
 * arrangement swaps their roles where the lookup returns the wrong one.
 * The lookup takes the first object of the table which carries the name,
 * and the allocator decides that order.
 */
static void NameDuplicate( Context *ctx, bool ours_found )
{
  rtems_status_code sc;
  rtems_id          id;

  if ( ctx->other_id == 0 ) {
    sc = rtems_timer_create( NAME, &ctx->other_id );
    T_rsc_success( sc );
  }

  id = 0;
  sc = rtems_timer_ident( NAME, &id );
  T_rsc_success( sc );

  if ( ( id == ctx->timer_id ) != ours_found ) {
    rtems_id tmp;

    tmp = ctx->timer_id;
    ctx->timer_id = ctx->other_id;
    ctx->other_id = tmp;
  }
}

static void UseServer( Context *ctx, bool wanted )
{
  if ( !wanted ) {
    (void) DeleteTimerServer();
    ctx->server_id = 0;

    return;
  }

  if ( ctx->server_id == 0 ) {
    rtems_status_code sc;

    sc = rtems_timer_initiate_server(
      RTEMS_TIMER_SERVER_DEFAULT_PRIORITY,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_ATTRIBUTES
    );
    T_rsc_success( sc );
    ctx->server_id = GetTimerServerTaskId();
  }
}

static void RtemsTimerReqDelete_Pre_Id_Prepare(
  RtemsTimerReqDelete_Context *ctx,
  RtemsTimerReqDelete_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsTimerReqDelete_Pre_Id_NoObj: {
      /*
       * While the `id` parameter is not associated with a timer.
       */
      SetFlag( &ctx->valid_id, 0 );
      break;
    }

    case RtemsTimerReqDelete_Pre_Id_Timer: {
      /*
       * While the `id` parameter is associated with a timer.
       */
      SetFlag( &ctx->valid_id, 1 );
      break;
    }

    case RtemsTimerReqDelete_Pre_Id_NA:
      break;
  }
}

static void RtemsTimerReqDelete_Pre_Name_Prepare(
  RtemsTimerReqDelete_Context *ctx,
  RtemsTimerReqDelete_Pre_Name state
)
{
  switch ( state ) {
    case RtemsTimerReqDelete_Pre_Name_Unique: {
      /*
       * While no other timer carries the object name of the timer.
       */
      DropOther( ctx );
      break;
    }

    case RtemsTimerReqDelete_Pre_Name_Found: {
      /*
       * While another timer carries the object name of the timer, while a
       * lookup of that name returns the timer.
       */
      NameDuplicate( ctx, true );
      break;
    }

    case RtemsTimerReqDelete_Pre_Name_Shadowed: {
      /*
       * While another timer carries the object name of the timer, while a
       * lookup of that name returns the other timer.
       */
      NameDuplicate( ctx, false );
      break;
    }

    case RtemsTimerReqDelete_Pre_Name_NA:
      break;
  }
}

static void RtemsTimerReqDelete_Pre_Server_Prepare(
  RtemsTimerReqDelete_Context   *ctx,
  RtemsTimerReqDelete_Pre_Server state
)
{
  switch ( state ) {
    case RtemsTimerReqDelete_Pre_Server_None: {
      /*
       * While the system has no Timer Server task.
       */
      FreshTimer( ctx );
      UseServer( ctx, false );
      SetFlag( &ctx->tickle, 0 );
      break;
    }

    case RtemsTimerReqDelete_Pre_Server_Idle: {
      /*
       * While the Timer Server task runs no Timer Service Routine.
       */
      UseServer( ctx, true );
      SetFlag( &ctx->tickle, 0 );
      break;
    }

    case RtemsTimerReqDelete_Pre_Server_Tickle: {
      /*
       * While the Timer Server task runs a Timer Service Routine.
       */
      UseServer( ctx, true );
      SetFlag( &ctx->tickle, 1 );
      break;
    }

    case RtemsTimerReqDelete_Pre_Server_NA:
      break;
  }
}

static void RtemsTimerReqDelete_Pre_Caller_Prepare(
  RtemsTimerReqDelete_Context   *ctx,
  RtemsTimerReqDelete_Pre_Caller state
)
{
  switch ( state ) {
    case RtemsTimerReqDelete_Pre_Caller_Other: {
      /*
       * While a task other than the Timer Server task calls the directive.
       */
      SetFlag( &ctx->self_delete, 0 );
      break;
    }

    case RtemsTimerReqDelete_Pre_Caller_Server: {
      /*
       * While the Timer Server task calls the directive.
       */
      SetFlag( &ctx->self_delete, 1 );
      break;
    }

    case RtemsTimerReqDelete_Pre_Caller_NA:
      break;
  }
}

static void RtemsTimerReqDelete_Post_Status_Check(
  RtemsTimerReqDelete_Context    *ctx,
  RtemsTimerReqDelete_Post_Status state
)
{
  switch ( state ) {
    case RtemsTimerReqDelete_Post_Status_Ok: {
      /*
       * The return status of rtems_timer_delete() shall be RTEMS_SUCCESSFUL.
       */
      ctx->timer_id = 0;
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsTimerReqDelete_Post_Status_InvId: {
      /*
       * The return status of rtems_timer_delete() shall be RTEMS_INVALID_ID.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ID );
      break;
    }

    case RtemsTimerReqDelete_Post_Status_NA:
      break;
  }
}

static void RtemsTimerReqDelete_Post_Name_Check(
  RtemsTimerReqDelete_Context  *ctx,
  RtemsTimerReqDelete_Post_Name state
)
{
  rtems_status_code sc;
  rtems_id          id;

  switch ( state ) {
    case RtemsTimerReqDelete_Post_Name_None: {
      /*
       * The object name shall identify no timer.
       */
      sc = rtems_timer_ident( NAME, &id );
      T_rsc( sc, RTEMS_INVALID_NAME );
      break;
    }

    case RtemsTimerReqDelete_Post_Name_Other: {
      /*
       * The object name shall identify the other timer.
       */
      id = 0;
      sc = rtems_timer_ident( NAME, &id );
      T_rsc_success( sc );
      T_eq_u32( id, ctx->other_id );
      break;
    }

    case RtemsTimerReqDelete_Post_Name_NA:
      break;
  }
}

static void RtemsTimerReqDelete_Post_WaitForServer_Check(
  RtemsTimerReqDelete_Context           *ctx,
  RtemsTimerReqDelete_Post_WaitForServer state
)
{
  switch ( state ) {
    case RtemsTimerReqDelete_Post_WaitForServer_Yes: {
      /*
       * The directive shall wait for the Timer Server task.
       */
      T_true( GetFlag( &ctx->waited ) != 0 );
      break;
    }

    case RtemsTimerReqDelete_Post_WaitForServer_No: {
      /*
       * The directive shall wait for no task.
       */
      T_false( GetFlag( &ctx->waited ) != 0 );
      break;
    }

    case RtemsTimerReqDelete_Post_WaitForServer_NA:
      break;
  }
}

static void RtemsTimerReqDelete_Post_YieldAllocMtx_Check(
  RtemsTimerReqDelete_Post_YieldAllocMtx state
)
{
  switch ( state ) {
    case RtemsTimerReqDelete_Post_YieldAllocMtx_Yes: {
      /*
       * The directive shall release the object allocator mutex before it
       * waits.
       */
      T_eq_uint( CallCounterGet( &AllocatorLockCounter ), 2 );
      break;
    }

    case RtemsTimerReqDelete_Post_YieldAllocMtx_No: {
      /*
       * The directive shall release the object allocator mutex when it ends
       * and at no other time.
       */
      T_eq_uint( CallCounterGet( &AllocatorLockCounter ), 1 );
      break;
    }

    case RtemsTimerReqDelete_Post_YieldAllocMtx_NA:
      break;
  }
}

static void RtemsTimerReqDelete_Setup( RtemsTimerReqDelete_Context *ctx )
{
  memset( ctx, 0, sizeof( *ctx ) );

  /*
   * The runner waits without a block, so the worker needs a priority above
   * the priority of the runner.
   */
  SetSelfPriority( PRIO_NORMAL );

  ctx->worker_id = CreateTask( "WORK", PRIO_HIGH );
  StartTask( ctx->worker_id, Worker, ctx );
}

static void RtemsTimerReqDelete_Setup_Wrap( void *arg )
{
  RtemsTimerReqDelete_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsTimerReqDelete_Setup( ctx );
}

static void RtemsTimerReqDelete_Teardown( RtemsTimerReqDelete_Context *ctx )
{
  if ( ctx->timer_id != 0 ) {
    rtems_status_code sc;

    sc = rtems_timer_delete( ctx->timer_id );
    T_rsc_success( sc );
  }

  CallCounterObserve( &AllocatorLockCounter, 0 );
  DropOther( ctx );
  DeleteTask( ctx->worker_id );
  (void) DeleteTimerServer();
  RestoreRunnerPriority();
  delete_ctx = NULL;
}

static void RtemsTimerReqDelete_Teardown_Wrap( void *arg )
{
  RtemsTimerReqDelete_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsTimerReqDelete_Teardown( ctx );
}

static void RtemsTimerReqDelete_Prepare( RtemsTimerReqDelete_Context *ctx )
{
  delete_ctx = ctx;
  ctx->caller_id = 0;
  SetFlag( &ctx->waited, 0 );
  SetFlag( &ctx->window_reached, 0 );
  SetFlag( &ctx->window_timed_out, 0 );
  SetFlag( &ctx->delete_returned, 0 );
  SetFlag( &ctx->deleted, 0 );

  if ( ctx->timer_id == 0 ) {
    rtems_status_code sc;

    sc = rtems_timer_create( NAME, &ctx->timer_id );
    T_rsc_success( sc );
  }
}

static void RtemsTimerReqDelete_Action( RtemsTimerReqDelete_Context *ctx )
{
  if ( GetFlag( &ctx->valid_id ) != 0 ) {
    ctx->id = ctx->timer_id;
  } else {
    ctx->id = 0;
  }

  if ( GetFlag( &ctx->tickle ) == 0 ) {
    ctx->caller_id = rtems_task_self();
    CallCounterObserve( &AllocatorLockCounter, ctx->caller_id );
    ctx->status = rtems_timer_delete( ctx->id );
  } else {
    rtems_status_code sc;

    if ( GetFlag( &ctx->self_delete ) != 0 ) {
      ctx->caller_id = ctx->server_id;
    } else {
      ctx->caller_id = ctx->worker_id;
    }

    CallCounterObserve( &AllocatorLockCounter, ctx->caller_id );

    sc = rtems_timer_server_fire_after( ctx->timer_id, 1, Routine, ctx );
    T_rsc_success( sc );

    /*
     * The tickle hands the timer to the Timer Server task.  That task has a
     * priority above every other task, so it runs the routine before this
     * directive returns.
     */
    ClockTick();

    if ( GetFlag( &ctx->self_delete ) == 0 ) {
      bool blocked;

      /* The condition variable of the Timer Server task blocks the delete. */
      blocked = WaitForBlockedState(
        ctx->worker_id,
        STATES_WAITING_FOR_CONDITION_VARIABLE,
        &ctx->delete_returned
      );
      SetFlag( &ctx->window_timed_out, !blocked );

      /*
       * The routine holds the window open, so a caller which waits is still
       * in the wait while this read happens.
       */
      SetFlag(
        &ctx->waited,
        ( GetThread( ctx->worker_id )->current_state &
          STATES_WAITING_FOR_CONDITION_VARIABLE ) != 0
      );

      /* Let the routine of the Timer Server task return. */
      SendEvents( ctx->server_id, RTEMS_EVENT_0 );
    }

    (void) WaitForFlag( &ctx->deleted );
  }
}

/* clang-format off */

static const RtemsTimerReqDelete_Entry
RtemsTimerReqDelete_Entries[] = {
  { 0, 0, 1, 0, 0, RtemsTimerReqDelete_Post_Status_InvId,
    RtemsTimerReqDelete_Post_Name_NA,
    RtemsTimerReqDelete_Post_WaitForServer_No,
    RtemsTimerReqDelete_Post_YieldAllocMtx_No },
  { 1, 0, 0, 0, 0, RtemsTimerReqDelete_Post_Status_NA,
    RtemsTimerReqDelete_Post_Name_NA,
    RtemsTimerReqDelete_Post_WaitForServer_NA,
    RtemsTimerReqDelete_Post_YieldAllocMtx_NA },
  { 0, 0, 0, 0, 0, RtemsTimerReqDelete_Post_Status_Ok,
    RtemsTimerReqDelete_Post_Name_Other,
    RtemsTimerReqDelete_Post_WaitForServer_No,
    RtemsTimerReqDelete_Post_YieldAllocMtx_No },
  { 0, 0, 0, 0, 0, RtemsTimerReqDelete_Post_Status_Ok,
    RtemsTimerReqDelete_Post_Name_None,
    RtemsTimerReqDelete_Post_WaitForServer_No,
    RtemsTimerReqDelete_Post_YieldAllocMtx_No },
  { 0, 0, 0, 0, 0, RtemsTimerReqDelete_Post_Status_Ok,
    RtemsTimerReqDelete_Post_Name_Other,
    RtemsTimerReqDelete_Post_WaitForServer_No,
    RtemsTimerReqDelete_Post_YieldAllocMtx_Yes },
  { 0, 0, 0, 0, 0, RtemsTimerReqDelete_Post_Status_Ok,
    RtemsTimerReqDelete_Post_Name_Other,
    RtemsTimerReqDelete_Post_WaitForServer_Yes,
    RtemsTimerReqDelete_Post_YieldAllocMtx_Yes },
  { 0, 0, 0, 0, 0, RtemsTimerReqDelete_Post_Status_Ok,
    RtemsTimerReqDelete_Post_Name_None,
    RtemsTimerReqDelete_Post_WaitForServer_No,
    RtemsTimerReqDelete_Post_YieldAllocMtx_Yes },
  { 0, 0, 0, 0, 0, RtemsTimerReqDelete_Post_Status_Ok,
    RtemsTimerReqDelete_Post_Name_None,
    RtemsTimerReqDelete_Post_WaitForServer_Yes,
    RtemsTimerReqDelete_Post_YieldAllocMtx_Yes }
};

static const uint8_t
RtemsTimerReqDelete_Map[] = {
  0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 3, 1, 6, 1, 7, 3, 2, 1,
  4, 1, 5, 2, 2, 1, 4, 1, 5, 2
};

/* clang-format on */

static size_t RtemsTimerReqDelete_Scope( void *arg, char *buf, size_t n )
{
  RtemsTimerReqDelete_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsTimerReqDelete_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsTimerReqDelete_Fixture = {
  .setup = RtemsTimerReqDelete_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsTimerReqDelete_Teardown_Wrap,
  .scope = RtemsTimerReqDelete_Scope,
  .initial_context = &RtemsTimerReqDelete_Instance
};

static inline RtemsTimerReqDelete_Entry RtemsTimerReqDelete_PopEntry(
  RtemsTimerReqDelete_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsTimerReqDelete_Entries[ RtemsTimerReqDelete_Map[ index ] ];
}

static void RtemsTimerReqDelete_SetPreConditionStates(
  RtemsTimerReqDelete_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Name_NA ) {
    ctx->Map.pcs[ 1 ] = RtemsTimerReqDelete_Pre_Name_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
}

static void RtemsTimerReqDelete_TestVariant( RtemsTimerReqDelete_Context *ctx )
{
  RtemsTimerReqDelete_Pre_Id_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsTimerReqDelete_Pre_Name_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsTimerReqDelete_Pre_Server_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsTimerReqDelete_Pre_Caller_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  RtemsTimerReqDelete_Action( ctx );
  RtemsTimerReqDelete_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  RtemsTimerReqDelete_Post_Name_Check( ctx, ctx->Map.entry.Post_Name );
  RtemsTimerReqDelete_Post_WaitForServer_Check(
    ctx,
    ctx->Map.entry.Post_WaitForServer
  );
  RtemsTimerReqDelete_Post_YieldAllocMtx_Check(
    ctx->Map.entry.Post_YieldAllocMtx
  );
}

/**
 * @fn void T_case_body_RtemsTimerReqDelete( void )
 */
T_TEST_CASE_FIXTURE( RtemsTimerReqDelete, &RtemsTimerReqDelete_Fixture )
{
  RtemsTimerReqDelete_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = RtemsTimerReqDelete_Pre_Id_NoObj;
    ctx->Map.pci[ 0 ] < RtemsTimerReqDelete_Pre_Id_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = RtemsTimerReqDelete_Pre_Name_Unique;
      ctx->Map.pci[ 1 ] < RtemsTimerReqDelete_Pre_Name_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = RtemsTimerReqDelete_Pre_Server_None;
        ctx->Map.pci[ 2 ] < RtemsTimerReqDelete_Pre_Server_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = RtemsTimerReqDelete_Pre_Caller_Other;
          ctx->Map.pci[ 3 ] < RtemsTimerReqDelete_Pre_Caller_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          ctx->Map.entry = RtemsTimerReqDelete_PopEntry( ctx );

          if ( ctx->Map.entry.Skip ) {
            continue;
          }

          RtemsTimerReqDelete_SetPreConditionStates( ctx );
          RtemsTimerReqDelete_Prepare( ctx );
          RtemsTimerReqDelete_TestVariant( ctx );
        }
      }
    }
  }
}

/** @} */
