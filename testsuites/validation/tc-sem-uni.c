/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsSemValUni
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

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsSemValUni spec:/rtems/sem/val/uni
 *
 * @ingroup TestsuitesValidationOneCpu0
 *
 * @brief Tests uniprocessor-specific semaphore behaviour.
 *
 * This test case performs the following actions:
 *
 * - Create a worker thread and two MrsP mutexes.  Obtain the Mrsp mutexes and
 *   check that a task yield works (owner is not sticky).  We need two mutexes
 *   since the uniprocessor schedulers do not increment the stick level in the
 *   scheduler unblock operation.
 *
 *   - Yield and let the worker obtain the MrsP mutexes.
 *
 *   - Yield and let the worker release the MrsP mutexes.
 *
 *   - Clean up all used resources.
 *
 * @{
 */

/**
 * @brief Test context for spec:/rtems/sem/val/uni test case.
 */
typedef struct {
  /**
   * @brief This member contains the mutex identifier.
   */
  rtems_id mutex_id;

  /**
   * @brief This member contains the second mutex identifier.
   */
  rtems_id mutex_2_id;

  /**
   * @brief This member contains a progress counter.
   */
  uint32_t counter;
} RtemsSemValUni_Context;

static RtemsSemValUni_Context
  RtemsSemValUni_Instance;

typedef RtemsSemValUni_Context Context;

static void ObtainReleaseMrsPTask( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;
  ObtainMutex( ctx->mutex_id );
  ObtainMutex( ctx->mutex_2_id );
  ctx->counter = 1;
  Yield();
  ReleaseMutex( ctx->mutex_2_id );
  ReleaseMutex( ctx->mutex_id );
  ctx->counter = 2;
  (void) ReceiveAnyEvents();
}

static void RtemsSemValUni_Setup( RtemsSemValUni_Context *ctx )
{
  SetSelfPriority( PRIO_NORMAL );
}

static void RtemsSemValUni_Setup_Wrap( void *arg )
{
  RtemsSemValUni_Context *ctx;

  ctx = arg;
  RtemsSemValUni_Setup( ctx );
}

static void RtemsSemValUni_Teardown( RtemsSemValUni_Context *ctx )
{
  RestoreRunnerPriority();
}

static void RtemsSemValUni_Teardown_Wrap( void *arg )
{
  RtemsSemValUni_Context *ctx;

  ctx = arg;
  RtemsSemValUni_Teardown( ctx );
}

static T_fixture RtemsSemValUni_Fixture = {
  .setup = RtemsSemValUni_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsSemValUni_Teardown_Wrap,
  .scope = NULL,
  .initial_context = &RtemsSemValUni_Instance
};

/**
 * @brief Create a worker thread and two MrsP mutexes.  Obtain the Mrsp mutexes
 *   and check that a task yield works (owner is not sticky).  We need two
 *   mutexes since the uniprocessor schedulers do not increment the stick level
 *   in the scheduler unblock operation.
 */
static void RtemsSemValUni_Action_0( RtemsSemValUni_Context *ctx )
{
  rtems_status_code   sc;
  rtems_id            worker_id;

  sc = rtems_semaphore_create(
    rtems_build_name( 'M', 'T', 'X', '1' ),
    1,
    RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY |
      RTEMS_MULTIPROCESSOR_RESOURCE_SHARING,
    PRIO_NORMAL,
    &ctx->mutex_id
  );
  T_rsc_success( sc );

  sc = rtems_semaphore_create(
    rtems_build_name( 'M', 'T', 'X', '2' ),
    1,
    RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY |
      RTEMS_MULTIPROCESSOR_RESOURCE_SHARING,
    PRIO_NORMAL,
    &ctx->mutex_2_id
  );
  T_rsc_success( sc );

  ctx->counter = 0;

  worker_id = CreateTask( "WORK", PRIO_NORMAL );
  StartTask( worker_id, ObtainReleaseMrsPTask, ctx );

  /*
   * Yield and let the worker obtain the MrsP mutexes.
   */
  Yield();
  T_eq_u32( ctx->counter, 1 );

  /*
   * Yield and let the worker release the MrsP mutexes.
   */
  Yield();
  T_eq_u32( ctx->counter, 2 );

  /*
   * Clean up all used resources.
   */
  DeleteTask( worker_id );
  DeleteMutex( ctx->mutex_2_id );
  DeleteMutex( ctx->mutex_id );
}

/**
 * @fn void T_case_body_RtemsSemValUni( void )
 */
T_TEST_CASE_FIXTURE( RtemsSemValUni, &RtemsSemValUni_Fixture )
{
  RtemsSemValUni_Context *ctx;

  ctx = T_fixture_context();

  RtemsSemValUni_Action_0( ctx );
}

/** @} */
