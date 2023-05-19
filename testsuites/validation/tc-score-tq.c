/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreTqValTq
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

#include "tx-support.h"
#include "tx-thread-queue.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreTqValTq spec:/score/tq/val/tq
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @brief Tests special thread queue behaviour.
 *
 * This test case performs the following actions:
 *
 * - Use two worker threads to provoke a deadlock detection involving a thread
 *   queue with no owner.
 *
 *   - Let blocker A obtain mutex A.
 *
 *   - Let blocker A block on a counting semaphore.
 *
 *   - Let blocker B block on mutex A.  The deadlock detection will stop since
 *     blocker A blocks on the counting semaphore which has no owner.
 *
 *   - Clean up all used resources.
 *
 * @{
 */

/**
 * @brief Test context for spec:/score/tq/val/tq test case.
 */
typedef struct {
  /**
   * @brief This member contains the thread queue test context.
   */
  TQContext tq_ctx;
} ScoreTqValTq_Context;

static ScoreTqValTq_Context
  ScoreTqValTq_Instance;

static void ScoreTqValTq_Setup( ScoreTqValTq_Context *ctx )
{
  rtems_status_code sc;

  SetSelfPriority( PRIO_NORMAL );
  TQInitialize( &ctx->tq_ctx );

  /* Replace mutex D with a counting semaphore */
  DeleteMutex( ctx->tq_ctx.mutex_id[ TQ_MUTEX_D ] );
  sc = rtems_semaphore_create(
    rtems_build_name( 'C', 'S', 'E', 'M' ),
    0,
    RTEMS_COUNTING_SEMAPHORE | RTEMS_PRIORITY,
    0,
    &ctx->tq_ctx.mutex_id[ TQ_MUTEX_D ]
  );
  T_rsc_success( sc );
}

static void ScoreTqValTq_Setup_Wrap( void *arg )
{
  ScoreTqValTq_Context *ctx;

  ctx = arg;
  ScoreTqValTq_Setup( ctx );
}

static void ScoreTqValTq_Teardown( ScoreTqValTq_Context *ctx )
{
  TQDestroy( &ctx->tq_ctx );
  RestoreRunnerPriority();
}

static void ScoreTqValTq_Teardown_Wrap( void *arg )
{
  ScoreTqValTq_Context *ctx;

  ctx = arg;
  ScoreTqValTq_Teardown( ctx );
}

static T_fixture ScoreTqValTq_Fixture = {
  .setup = ScoreTqValTq_Setup_Wrap,
  .stop = NULL,
  .teardown = ScoreTqValTq_Teardown_Wrap,
  .scope = NULL,
  .initial_context = &ScoreTqValTq_Instance
};

/**
 * @brief Use two worker threads to provoke a deadlock detection involving a
 *   thread queue with no owner.
 */
static void ScoreTqValTq_Action_0( ScoreTqValTq_Context *ctx )
{
  TQReset( &ctx->tq_ctx );

  /*
   * Let blocker A obtain mutex A.
   */
  TQSend( &ctx->tq_ctx, TQ_BLOCKER_A, TQ_EVENT_MUTEX_A_OBTAIN );

  /*
   * Let blocker A block on a counting semaphore.
   */
  TQSend( &ctx->tq_ctx, TQ_BLOCKER_A, TQ_EVENT_MUTEX_D_OBTAIN );

  /*
   * Let blocker B block on mutex A.  The deadlock detection will stop since
   * blocker A blocks on the counting semaphore which has no owner.
   */
  TQSend( &ctx->tq_ctx, TQ_BLOCKER_B, TQ_EVENT_MUTEX_A_OBTAIN );

  /*
   * Clean up all used resources.
   */
  TQMutexRelease( &ctx->tq_ctx, TQ_MUTEX_D );
  TQSend( &ctx->tq_ctx, TQ_BLOCKER_A, TQ_EVENT_MUTEX_A_RELEASE );
  TQSend( &ctx->tq_ctx, TQ_BLOCKER_B, TQ_EVENT_MUTEX_A_RELEASE );
}

/**
 * @fn void T_case_body_ScoreTqValTq( void )
 */
T_TEST_CASE_FIXTURE( ScoreTqValTq, &ScoreTqValTq_Fixture )
{
  ScoreTqValTq_Context *ctx;

  ctx = T_fixture_context();

  ScoreTqValTq_Action_0( ctx );
}

/** @} */
