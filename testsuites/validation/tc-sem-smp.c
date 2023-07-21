/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsSemValSmp
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
#include <rtems/score/threaddispatch.h>

#include "ts-config.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsSemValSmp spec:/rtems/sem/val/smp
 *
 * @ingroup TestsuitesValidationSmpOnly0
 *
 * @brief Tests SMP-specific semaphore behaviour.
 *
 * This test case performs the following actions:
 *
 * - Create a worker thread and a MrsP mutex.  Use the mutex and the worker to
 *   perform a bad sticky thread queue enqueue.
 *
 * - Create two worker threads, a MrsP mutex, and a priority inheritance mutex.
 *   Use the mutexes and the workers to raise the current priority to a higher
 *   priority than the ceiling priority of the mutex while one of the workers
 *   waits on the mutex.
 *
 *   - Let the first worker try to obtain the MrsP mutex.  Check that it
 *     acquired the ceiling priority.
 *
 *   - Let the second worker try to obtain the priority inheritance mutex.
 *     Check that the first worker inherited the priority from the second
 *     worker.
 *
 *   - Set the real priority of the first worker.  Check that it defines the
 *     current priority.
 *
 *   - Release the MrsP mutex so that the first worker can to obtain it.  It
 *     will replace a temporary priority node which is the maximum priority
 *     node.  This is the first scenario we want to test.
 *
 *   - Obtain the MrsP mutex for the runner thread to start the second scenario
 *     we would like to test.
 *
 *   - Let the first worker try to obtain the MrsP mutex.  Check that it
 *     acquired the ceiling priority.
 *
 *   - Let the second worker try to obtain the priority inheritance mutex.
 *     Check that the first worker inherited the priority from the second
 *     worker.
 *
 *   - Lower the priority of the second worker.  Check that the inherited
 *     priority of the first worker reflects this priority change.
 *
 *   - Change the real priority of the first worker so that it defines its
 *     current priority.
 *
 *   - Release the MrsP mutex so that the first worker can to obtain it.  It
 *     will replace a temporary priority node which is between the minimum and
 *     maximum priority node.  This is the second scenario we want to test.
 *
 *   - Clean up all used resources.
 *
 * @{
 */

/**
 * @brief Test context for spec:/rtems/sem/val/smp test case.
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
   * @brief If this member is true, then the worker is done.
   */
  volatile bool done;

  /**
   * @brief If this member is true, then the second worker is done.
   */
  volatile bool done_2;
} RtemsSemValSmp_Context;

static RtemsSemValSmp_Context
  RtemsSemValSmp_Instance;

typedef RtemsSemValSmp_Context Context;

static void BadEnqueueFatal(
  rtems_fatal_source source,
  rtems_fatal_code   code,
  void              *arg
)
{
  Per_CPU_Control *cpu_self;
  Context         *ctx;

  T_eq_int( source, INTERNAL_ERROR_CORE );
  T_eq_ulong(
    code,
    INTERNAL_ERROR_THREAD_QUEUE_ENQUEUE_STICKY_FROM_BAD_STATE
  );

  SetFatalHandler( NULL, NULL );

  cpu_self = _Per_CPU_Get();
  _Thread_Dispatch_unnest( cpu_self );
  _Thread_Dispatch_unnest( cpu_self );

  ctx = arg;
  ctx->done = true;
  SuspendSelf();
}

static void BadEnqueueTask( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;
  (void) _Thread_Dispatch_disable();
  ObtainMutex( ctx->mutex_id );
}

static void ObtainReleaseMrsPTask( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;
  ObtainMutex( ctx->mutex_2_id );
  ctx->done = true;
  ObtainMutex( ctx->mutex_id );
  ReleaseMutex( ctx->mutex_id );
  ReleaseMutex( ctx->mutex_2_id );
  ctx->done = true;
  SuspendSelf();
}

static void ObtainRelease2Task( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;
  ctx->done_2 = true;
  ObtainMutex( ctx->mutex_2_id );
  ReleaseMutex( ctx->mutex_2_id );
  ctx->done_2 = true;
  SuspendSelf();
}

static void RtemsSemValSmp_Setup( RtemsSemValSmp_Context *ctx )
{
  SetSelfPriority( PRIO_NORMAL );
}

static void RtemsSemValSmp_Setup_Wrap( void *arg )
{
  RtemsSemValSmp_Context *ctx;

  ctx = arg;
  RtemsSemValSmp_Setup( ctx );
}

static void RtemsSemValSmp_Teardown( RtemsSemValSmp_Context *ctx )
{
  RestoreRunnerPriority();
}

static void RtemsSemValSmp_Teardown_Wrap( void *arg )
{
  RtemsSemValSmp_Context *ctx;

  ctx = arg;
  RtemsSemValSmp_Teardown( ctx );
}

static T_fixture RtemsSemValSmp_Fixture = {
  .setup = RtemsSemValSmp_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsSemValSmp_Teardown_Wrap,
  .scope = NULL,
  .initial_context = &RtemsSemValSmp_Instance
};

/**
 * @brief Create a worker thread and a MrsP mutex.  Use the mutex and the
 *   worker to perform a bad sticky thread queue enqueue.
 */
static void RtemsSemValSmp_Action_0( RtemsSemValSmp_Context *ctx )
{
  rtems_status_code sc;
  rtems_id          worker_id;
  rtems_id          scheduler_b_id;

  ctx->done = false;

  sc = rtems_scheduler_ident( TEST_SCHEDULER_B_NAME, &scheduler_b_id );
  T_rsc_success( sc );

  sc = rtems_semaphore_create(
    rtems_build_name( 'M', 'U', 'T', 'X' ),
    1,
    RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY |
      RTEMS_MULTIPROCESSOR_RESOURCE_SHARING,
    PRIO_HIGH,
    &ctx->mutex_id
  );
  T_rsc_success( sc );

  worker_id = CreateTask( "WORK", PRIO_NORMAL );
  SetScheduler( worker_id, scheduler_b_id, PRIO_NORMAL );

  ObtainMutex( ctx->mutex_id );
  SetFatalHandler( BadEnqueueFatal, ctx );
  StartTask( worker_id, BadEnqueueTask, ctx );

  while ( !ctx->done ) {
    /* Wait */
  }

  DeleteTask( worker_id );
  ReleaseMutex( ctx->mutex_id );
  DeleteMutex( ctx->mutex_id );
}

/**
 * @brief Create two worker threads, a MrsP mutex, and a priority inheritance
 *   mutex. Use the mutexes and the workers to raise the current priority to a
 *   higher priority than the ceiling priority of the mutex while one of the
 *   workers waits on the mutex.
 */
static void RtemsSemValSmp_Action_1( RtemsSemValSmp_Context *ctx )
{
  rtems_status_code   sc;
  rtems_id            worker_id;
  rtems_id            worker_2_id;
  rtems_id            scheduler_b_id;
  rtems_task_priority prio;

  sc = rtems_scheduler_ident( TEST_SCHEDULER_B_NAME, &scheduler_b_id );
  T_rsc_success( sc );

  sc = rtems_semaphore_create(
    rtems_build_name( 'M', 'U', 'T', 'X' ),
    1,
    RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY |
      RTEMS_MULTIPROCESSOR_RESOURCE_SHARING,
    PRIO_HIGH,
    &ctx->mutex_id
  );
  T_rsc_success( sc );

  sc = rtems_semaphore_set_priority(
    ctx->mutex_id,
    scheduler_b_id,
    PRIO_HIGH,
    &prio
  );
  T_rsc_success( sc );

  ctx->mutex_2_id = CreateMutex();

  worker_id = CreateTask( "WORK", PRIO_NORMAL );
  SetScheduler( worker_id, scheduler_b_id, PRIO_NORMAL );

  worker_2_id = CreateTask( "WRK2", PRIO_NORMAL );
  SetScheduler( worker_2_id, scheduler_b_id, PRIO_VERY_HIGH );

  /*
   * Let the first worker try to obtain the MrsP mutex.  Check that it acquired
   * the ceiling priority.
   */
  ObtainMutex( ctx->mutex_id );
  ctx->done = false;
  StartTask( worker_id, ObtainReleaseMrsPTask, ctx );

  while ( !ctx->done ) {
    /* Wait */
  }

  ctx->done = false;
  WaitForIntendToBlock( worker_id );
  prio = GetPriorityByScheduler( worker_id, scheduler_b_id );
  T_eq_u32( prio, PRIO_HIGH );

  /*
   * Let the second worker try to obtain the priority inheritance mutex. Check
   * that the first worker inherited the priority from the second worker.
   */
  ctx->done_2 = false;
  StartTask( worker_2_id, ObtainRelease2Task, ctx );

  while ( !ctx->done_2 ) {
    /* Wait */
  }

  ctx->done_2 = false;
  WaitForExecutionStop( worker_2_id );
  prio = GetPriorityByScheduler( worker_id, scheduler_b_id );
  T_eq_u32( prio, PRIO_VERY_HIGH );

  /*
   * Set the real priority of the first worker.  Check that it defines the
   * current priority.
   */
  SetPriority( worker_id, PRIO_ULTRA_HIGH );

  prio = GetPriorityByScheduler( worker_id, scheduler_b_id );
  T_eq_u32( prio, PRIO_ULTRA_HIGH );

  /*
   * Release the MrsP mutex so that the first worker can to obtain it.  It will
   * replace a temporary priority node which is the maximum priority node.
   * This is the first scenario we want to test.
   */
  ReleaseMutex( ctx->mutex_id );

  while ( !ctx->done || !ctx->done_2 ) {
    /* Wait */
  }

  prio = GetPriorityByScheduler( worker_id, scheduler_b_id );
  T_eq_u32( prio, PRIO_ULTRA_HIGH );

  /*
   * Obtain the MrsP mutex for the runner thread to start the second scenario
   * we would like to test.
   */
  ObtainMutex( ctx->mutex_id );

  /*
   * Let the first worker try to obtain the MrsP mutex.  Check that it acquired
   * the ceiling priority.
   */
  ctx->done = false;
  sc = rtems_task_restart( worker_id, (rtems_task_argument) ctx );
  T_rsc_success( sc );

  while ( !ctx->done ) {
    /* Wait */
  }

  ctx->done = false;
  WaitForIntendToBlock( worker_id );
  prio = GetPriorityByScheduler( worker_id, scheduler_b_id );
  T_eq_u32( prio, PRIO_HIGH );

  /*
   * Let the second worker try to obtain the priority inheritance mutex. Check
   * that the first worker inherited the priority from the second worker.
   */
  ctx->done_2 = false;
  sc = rtems_task_restart( worker_2_id, (rtems_task_argument) ctx );
  T_rsc_success( sc );

  while ( !ctx->done_2 ) {
    /* Wait */
  }

  ctx->done_2 = false;
  WaitForExecutionStop( worker_2_id );
  prio = GetPriorityByScheduler( worker_id, scheduler_b_id );
  T_eq_u32( prio, PRIO_VERY_HIGH );

  /*
   * Lower the priority of the second worker.  Check that the inherited
   * priority of the first worker reflects this priority change.
   */
  SetPriority( worker_2_id, PRIO_LOW );

  prio = GetPriorityByScheduler( worker_id, scheduler_b_id );
  T_eq_u32( prio, PRIO_HIGH );

  /*
   * Change the real priority of the first worker so that it defines its
   * current priority.
   */
  SetPriority( worker_id, PRIO_ULTRA_HIGH );

  prio = GetPriorityByScheduler( worker_id, scheduler_b_id );
  T_eq_u32( prio, PRIO_ULTRA_HIGH );

  /*
   * Release the MrsP mutex so that the first worker can to obtain it.  It will
   * replace a temporary priority node which is between the minimum and maximum
   * priority node.  This is the second scenario we want to test.
   */
  ReleaseMutex( ctx->mutex_id );

  while ( !ctx->done || !ctx->done_2 ) {
    /* Wait */
  }

  prio = GetPriorityByScheduler( worker_id, scheduler_b_id );
  T_eq_u32( prio, PRIO_ULTRA_HIGH );

  /*
   * Clean up all used resources.
   */
  DeleteTask( worker_id );
  DeleteTask( worker_2_id );
  DeleteMutex( ctx->mutex_id );
  DeleteMutex( ctx->mutex_2_id );
}

/**
 * @fn void T_case_body_RtemsSemValSmp( void )
 */
T_TEST_CASE_FIXTURE( RtemsSemValSmp, &RtemsSemValSmp_Fixture )
{
  RtemsSemValSmp_Context *ctx;

  ctx = T_fixture_context();

  RtemsSemValSmp_Action_0( ctx );
  RtemsSemValSmp_Action_1( ctx );
}

/** @} */
