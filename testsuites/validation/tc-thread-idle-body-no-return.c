/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreThreadValIdleBodyNoReturn
 */

/*
 * Copyright (C) 2022 embedded brains GmbH & Co. KG
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

#include <bsp.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreThreadValIdleBodyNoReturn \
 *   spec:/score/thread/val/idle-body-no-return
 *
 * @ingroup TestsuitesValidation0
 *
 * @brief Tests thread idle body behaviour.
 *
 * This test case performs the following actions:
 *
 * - Create threads which execute an thread idle body.  Check that the thread
 *   idle body does not return.  If it would return, then an
 *   INTERNAL_ERROR_THREAD_EXITTED fatal error would occur.
 *
 *   - Check that the CPU port thread idle body does not return.
 *
 *   - Where the BSP provides an idle thread body, check that it does not
 *     return.
 *
 *   - Clean up all used resources.
 *
 * @{
 */

/**
 * @brief Test context for spec:/score/thread/val/idle-body-no-return test
 *   case.
 */
typedef struct {
  /**
   * @brief This member contains a counter.
   */
  uint32_t counter;
} ScoreThreadValIdleBodyNoReturn_Context;

static ScoreThreadValIdleBodyNoReturn_Context
  ScoreThreadValIdleBodyNoReturn_Instance;

typedef ScoreThreadValIdleBodyNoReturn_Context Context;

static void CheckIdleBody( Context *ctx, rtems_task_entry entry )
{
  rtems_id          id;
  rtems_interval    interval;
  rtems_status_code sc;

  ctx->counter = 0;
  id = CreateTask( "WORK", PRIO_LOW );
  StartTask( id, entry, ctx );

  /*
   * With optimization disabled, coverage enabled, SMP enabled and a slow
   * target, things may take some time.
   */
  interval = 1;
  while ( ctx->counter == 0 && interval <= 1024 ) {

    sc = rtems_task_wake_after( interval );
    T_rsc_success( sc );

    interval *= 2;
  }

  sc = rtems_task_wake_after( interval );
  T_rsc_success( sc );

  T_eq_u32( ctx->counter, 1 );
  DeleteTask( id );
}

static void CPUThreadIdleBody( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;
  ++ctx->counter;

  (void) _CPU_Thread_Idle_body( 0 );
}

#if defined(BSP_IDLE_TASK_BODY)
static void BSPIdleTaskBody( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;
  ++ctx->counter;

  (void) BSP_IDLE_TASK_BODY( 0 );
}
#endif

static T_fixture ScoreThreadValIdleBodyNoReturn_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = NULL,
  .initial_context = &ScoreThreadValIdleBodyNoReturn_Instance
};

/**
 * @brief Create threads which execute an thread idle body.  Check that the
 *   thread idle body does not return.  If it would return, then an
 *   INTERNAL_ERROR_THREAD_EXITTED fatal error would occur.
 */
static void ScoreThreadValIdleBodyNoReturn_Action_0(
  ScoreThreadValIdleBodyNoReturn_Context *ctx
)
{
  SetSelfPriority( PRIO_NORMAL );

  /*
   * Check that the CPU port thread idle body does not return.
   */
  CheckIdleBody( ctx, CPUThreadIdleBody );

  /*
   * Where the BSP provides an idle thread body, check that it does not return.
   */
  #if defined(BSP_IDLE_TASK_BODY)
  CheckIdleBody( ctx, BSPIdleTaskBody );
  #endif

  /*
   * Clean up all used resources.
   */
  RestoreRunnerPriority();
}

/**
 * @fn void T_case_body_ScoreThreadValIdleBodyNoReturn( void )
 */
T_TEST_CASE_FIXTURE(
  ScoreThreadValIdleBodyNoReturn,
  &ScoreThreadValIdleBodyNoReturn_Fixture
)
{
  ScoreThreadValIdleBodyNoReturn_Context *ctx;

  ctx = T_fixture_context();

  ScoreThreadValIdleBodyNoReturn_Action_0( ctx );
}

/** @} */
