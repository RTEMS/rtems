/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestCaseScoreThreadValThread
 */

/*
 * Copyright (C) 2021 embedded brains GmbH (http://www.embedded-brains.de)
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

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RTEMSTestCaseScoreThreadValThread spec:/score/thread/val/thread
 *
 * @ingroup RTEMSTestSuiteTestsuitesValidationNoClock0
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
} ScoreThreadValThread_Context;

static ScoreThreadValThread_Context
  ScoreThreadValThread_Instance;

typedef ScoreThreadValThread_Context Context;

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

static void KillerTask( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;
  DeleteTask( ctx->worker_id );
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
 * @fn void T_case_body_ScoreThreadValThread( void )
 */
T_TEST_CASE_FIXTURE( ScoreThreadValThread, &ScoreThreadValThread_Fixture )
{
  ScoreThreadValThread_Context *ctx;

  ctx = T_fixture_context();

  ScoreThreadValThread_Action_0( ctx );
}

/** @} */
