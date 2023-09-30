/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreThreadValThread
 */

/*
 * Copyright (C) 2021, 2023 embedded brains GmbH & Co. KG
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
#include <rtems/score/statesimpl.h>
#include <rtems/score/threadimpl.h>

#include "ts-config.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreThreadValThread spec:/score/thread/val/thread
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @brief Tests general thread behaviour.
 *
 * This test case performs the following actions:
 *
 * - Create an extension set with a thread terminate extension which deletes
 *   the killer task if it is invoked for the worker task.  Create and start
 *   the worker task.  Create and start the killer task.  The killer task
 *   deletes the worker task.
 *
 *   - Check that the killer task was deleted.
 *
 *   - Check that the worker task still exists.
 *
 *   - Check that the life of the worker task is protected and terminating.
 *
 *   - Check that the worker task is waiting for a joining thread.
 *
 *   - Delete the worker task using brute force.
 *
 *   - Clean up all used resources.
 *
 * - Delete a thread which least recently used the floating point coprocessor.
 *
 *   - Start the worker thread.  Let it use the floating point coprocessor.
 *
 *   - Delete the worker thread and free the thread resources.
 *
 *   - Clean up all used resources.
 *
 * - Validate the global construction.  Mark that the test case executed.
 *
 *   - Check that the global constructor was called exactly once.
 *
 *   - Check that the global construction was done by the Classic API user
 *     initialization task.
 *
 *   - Check that the global constructor was called before the task entry.
 *
 * - Validate that thread dispatching does not recurse.  Issue a couple of
 *   thread context switches during a thread dispatch.  Record the stack
 *   pointers of the heir threads.
 *
 *   - Check that the thread dispatching did not recurse through the recorded
 *     stack pointers.
 *
 * @{
 */

/**
 * @brief Test context for spec:/score/thread/val/thread test case.
 */
typedef struct {
  /**
   * @brief This member contains the worker task identifier.
   */
  rtems_id worker_id;

  /**
   * @brief This member contains the killer task identifier.
   */
  rtems_id killer_id;

  /**
   * @brief This member contains a floating-point object.
   */
  volatile double fp_obj;

  /**
   * @brief This member indicates the thread switch state.
   */
  int thread_switch_state;

  /**
   * @brief This member contain the runner stack pointer at the context switch.
   */
  uintptr_t runner_stack[ 2 ];

  /**
   * @brief This member contain the worker stack pointer at the context switch.
   */
  uintptr_t worker_stack[ 2 ];
} ScoreThreadValThread_Context;

static ScoreThreadValThread_Context
  ScoreThreadValThread_Instance;

typedef ScoreThreadValThread_Context Context;

static bool test_case_executed;

static bool constructor_test_case_executed;

static uint32_t constructor_calls;

static rtems_id constructor_id;

static __attribute__(( __constructor__ )) void Constructor( void )
{
  constructor_test_case_executed = test_case_executed;
  ++constructor_calls;
  constructor_id = rtems_task_self();
}

static void TaskTerminate( rtems_tcb *executing )
{
  Context *ctx;

  ctx = T_fixture_context();

  if ( ctx->worker_id == executing->Object.id ) {
    DeleteTask( ctx->killer_id );
  }
}

static void WorkerTask( rtems_task_argument arg )
{
  (void) arg;
  SuspendSelf();
}

static void GoBackToRunner( void *arg )
{
  Context *ctx;

  ctx = arg;
  SetPriority( ctx->worker_id, PRIO_LOW );
}

static void FloatingPointTask( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;
  ctx->fp_obj *= 1.23;

  /*
   * We use an interrupt to go back to the runner since on some
   * architectures, the floating-point context is only saved during interrupt
   * processing and not for synchronous thread switches.
   */
  CallWithinISR( GoBackToRunner, ctx );
}

static void KillerTask( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;
  DeleteTask( ctx->worker_id );
}

static void TaskSwitch( rtems_tcb *executing, rtems_tcb *heir )
{
  Context  *ctx;
  rtems_id  worker_id;
  int       state;
  uintptr_t heir_stack;

  ctx = T_fixture_context();
  worker_id = ctx->worker_id;
  state = ctx->thread_switch_state;
  ctx->thread_switch_state = state + 1;
  heir_stack = _CPU_Context_Get_SP( &heir->Registers );

  switch ( state ) {
    case 0:
      T_eq_u32( heir->Object.id, worker_id );
      SuspendTask( worker_id );
      ctx->worker_stack[ 0 ] = heir_stack;
      break;
    case 1:
      T_eq_u32( executing->Object.id, worker_id );
      ResumeTask( worker_id );
      ctx->runner_stack[ 0 ] = heir_stack;
      break;
    case 2:
      T_eq_u32( heir->Object.id, worker_id );
      SuspendTask( worker_id );
      ctx->worker_stack[ 1 ] = heir_stack;
      break;
    case 3:
      T_eq_u32( executing->Object.id, worker_id );
      ctx->runner_stack[ 1 ] = heir_stack;
      break;
    default:
      T_unreachable();
  }
}

static T_fixture ScoreThreadValThread_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = NULL,
  .initial_context = &ScoreThreadValThread_Instance
};

/**
 * @brief Create an extension set with a thread terminate extension which
 *   deletes the killer task if it is invoked for the worker task.  Create and
 *   start the worker task.  Create and start the killer task.  The killer task
 *   deletes the worker task.
 */
static void ScoreThreadValThread_Action_0( ScoreThreadValThread_Context *ctx )
{
  rtems_extensions_table table = {
    .thread_terminate = TaskTerminate
  };
  rtems_status_code sc;
  rtems_id          id;
  rtems_tcb        *worker_tcb;

  sc = rtems_extension_create(
    rtems_build_name( 'T', 'E', 'S', 'T' ),
    &table,
    &id
  );
  T_rsc_success( sc );

  SetSelfPriority( PRIO_NORMAL );
  ctx->worker_id = CreateTask( "WORK", PRIO_HIGH );
  worker_tcb = GetThread( ctx->worker_id );
  StartTask( ctx->worker_id, WorkerTask, NULL );
  ctx->killer_id = CreateTask( "KILL", PRIO_HIGH );
  StartTask( ctx->killer_id, KillerTask, ctx );

  /*
   * Check that the killer task was deleted.
   */
  sc = rtems_event_send( ctx->killer_id, RTEMS_EVENT_0 );
  T_rsc( sc, RTEMS_INVALID_ID );

  /*
   * Check that the worker task still exists.
   */
  sc = rtems_event_send( ctx->worker_id, RTEMS_EVENT_0 );
  T_rsc_success( sc );

  /*
   * Check that the life of the worker task is protected and terminating.
   */
  T_eq_int(
    worker_tcb->Life.state,
    THREAD_LIFE_PROTECTED | THREAD_LIFE_TERMINATING
  );

  /*
   * Check that the worker task is waiting for a joining thread.
   */
  T_eq_u32(
    worker_tcb->current_state,
    STATES_WAITING_FOR_JOIN_AT_EXIT
  );

  /*
   * Delete the worker task using brute force.
   */
  worker_tcb->Life.state = THREAD_LIFE_DETACHED |
    THREAD_LIFE_PROTECTED | THREAD_LIFE_TERMINATING;
  _Thread_Clear_state( worker_tcb, STATES_WAITING_FOR_JOIN_AT_EXIT );

  /*
   * Clean up all used resources.
   */
  KillZombies();
  RestoreRunnerPriority();

  sc = rtems_extension_delete( id );
  T_rsc_success( sc );
}

/**
 * @brief Delete a thread which least recently used the floating point
 *   coprocessor.
 */
static void ScoreThreadValThread_Action_1( ScoreThreadValThread_Context *ctx )
{
  rtems_status_code sc;

  SetSelfPriority( PRIO_NORMAL );
  sc = rtems_task_create(
    rtems_build_name( 'W', 'O', 'R', 'K'),
    PRIO_HIGH,
    TEST_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_FLOATING_POINT,
    &ctx->worker_id
  );
  T_rsc_success( sc );

  /*
   * Start the worker thread.  Let it use the floating point coprocessor.
   */
  StartTask( ctx->worker_id, FloatingPointTask, ctx );

  /*
   * Delete the worker thread and free the thread resources.
   */
  DeleteTask( ctx->worker_id );
  KillZombies();

  /*
   * Clean up all used resources.
   */
  RestoreRunnerPriority();
}

/**
 * @brief Validate the global construction.  Mark that the test case executed.
 */
static void ScoreThreadValThread_Action_2( ScoreThreadValThread_Context *ctx )
{
  test_case_executed = true;

  /*
   * Check that the global constructor was called exactly once.
   */
  T_eq_u32( constructor_calls, 1 );

  /*
   * Check that the global construction was done by the Classic API user
   * initialization task.
   */
  T_eq_u32( constructor_id, rtems_task_self() );

  /*
   * Check that the global constructor was called before the task entry.
   */
  T_false( constructor_test_case_executed );
}

/**
 * @brief Validate that thread dispatching does not recurse.  Issue a couple of
 *   thread context switches during a thread dispatch.  Record the stack
 *   pointers of the heir threads.
 */
static void ScoreThreadValThread_Action_3( ScoreThreadValThread_Context *ctx )
{
  SetSelfPriority( PRIO_NORMAL );
  ctx->worker_id = CreateTask( "WORK", PRIO_HIGH );
  StartTask( ctx->worker_id, WorkerTask, NULL );

  ctx->thread_switch_state = 0;
  ctx->runner_stack[ 0 ] = 0;
  ctx->runner_stack[ 1 ] = 1;
  ctx->worker_stack[ 0 ] = 0;
  ctx->worker_stack[ 1 ] = 1;
  SetTaskSwitchExtension( TaskSwitch );
  ResumeTask( ctx->worker_id );

  SetTaskSwitchExtension( NULL );
  DeleteTask( ctx->worker_id );
  RestoreRunnerPriority();

  /*
   * Check that the thread dispatching did not recurse through the recorded
   * stack pointers.
   */
  T_eq_uptr( ctx->runner_stack[ 0 ], ctx->runner_stack[ 1 ] );
  T_eq_uptr( ctx->worker_stack[ 0 ], ctx->worker_stack[ 1 ] );
}

/**
 * @fn void T_case_body_ScoreThreadValThread( void )
 */
T_TEST_CASE_FIXTURE( ScoreThreadValThread, &ScoreThreadValThread_Fixture )
{
  ScoreThreadValThread_Context *ctx;

  ctx = T_fixture_context();

  ScoreThreadValThread_Action_0( ctx );
  ScoreThreadValThread_Action_1( ctx );
  ScoreThreadValThread_Action_2( ctx );
  ScoreThreadValThread_Action_3( ctx );
}

/** @} */
