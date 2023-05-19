/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreThreadValSmpOneCpu
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

#include <rtems/score/threadimpl.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreThreadValSmpOneCpu spec:/score/thread/val/smp-one-cpu
 *
 * @ingroup TestsuitesValidationSmpOneCpu0
 *
 * @brief Tests SMP-specific thread behaviour using only one processor and a
 *   uniprocessor scheduler.
 *
 * This test case performs the following actions:
 *
 * - Create one worker thread to validate the thread pinning on only one
 *   processor using a uniprocessor scheduler.
 *
 *   - Pin the runner thread.  Preempt the runner thread.  Unpin the runner
 *     thread.
 *
 *   - Clean up all used resources.
 *
 * @{
 */

/**
 * @brief Test context for spec:/score/thread/val/smp-one-cpu test case.
 */
typedef struct {
  /**
   * @brief This member contains the worker thread identifier.
   */
  rtems_id worker_id;

  /**
   * @brief This member contains a counter for EVENT_COUNT.
   */
  volatile uint32_t counter;
} ScoreThreadValSmpOneCpu_Context;

static ScoreThreadValSmpOneCpu_Context
  ScoreThreadValSmpOneCpu_Instance;

#define EVENT_COUNT RTEMS_EVENT_0

typedef ScoreThreadValSmpOneCpu_Context Context;

static void WorkerTask( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;

  while ( true ) {
    rtems_event_set events;

    events = ReceiveAnyEvents();

    if ( ( events & EVENT_COUNT ) != 0 ) {
      ++ctx->counter;
    }
  }
}

static T_fixture ScoreThreadValSmpOneCpu_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = NULL,
  .initial_context = &ScoreThreadValSmpOneCpu_Instance
};

/**
 * @brief Create one worker thread to validate the thread pinning on only one
 *   processor using a uniprocessor scheduler.
 */
static void ScoreThreadValSmpOneCpu_Action_0(
  ScoreThreadValSmpOneCpu_Context *ctx
)
{
  Per_CPU_Control *cpu_self;
  Thread_Control  *executing;

  executing = _Thread_Get_executing();
  SetSelfPriority( PRIO_NORMAL );
  ctx->counter = 0;

  ctx->worker_id = CreateTask( "WORK", PRIO_HIGH );
  StartTask( ctx->worker_id, WorkerTask, ctx );

  /*
   * Pin the runner thread.  Preempt the runner thread.  Unpin the runner
   * thread.
   */
  _Thread_Pin( executing );

  /* We have to preempt the runner to end up in _Thread_Do_unpin() */
  SendEvents( ctx->worker_id, EVENT_COUNT );
  T_eq_u32( ctx->counter, 1 );

  cpu_self = _Thread_Dispatch_disable();
  _Thread_Unpin( executing, cpu_self );
  _Thread_Dispatch_direct( cpu_self );

  /*
   * Clean up all used resources.
   */
  DeleteTask( ctx->worker_id );
  RestoreRunnerPriority();
}

/**
 * @fn void T_case_body_ScoreThreadValSmpOneCpu( void )
 */
T_TEST_CASE_FIXTURE(
  ScoreThreadValSmpOneCpu,
  &ScoreThreadValSmpOneCpu_Fixture
)
{
  ScoreThreadValSmpOneCpu_Context *ctx;

  ctx = T_fixture_context();

  ScoreThreadValSmpOneCpu_Action_0( ctx );
}

/** @} */
