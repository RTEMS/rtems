/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsTaskValTimeslice
 */

/*
 * Copyright (C) 2024 embedded brains GmbH & Co. KG
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

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsTaskValTimeslice spec:/rtems/task/val/timeslice
 *
 * @ingroup TestsuitesValidationOneCpu0
 *
 * @brief Tests the task timeslicing mode.
 *
 * This test case performs the following actions:
 *
 * - Create a worker task which forever increments a counter and yields.
 *
 *   - While the preemption mode is disabled and timeslicing is enabled for the
 *     executing task, elapse a full timeslice. Check that the executing task
 *     did not yield.
 *
 *   - While the preemption mode is enabled and timeslicing is enabled for the
 *     executing task, elapse a full timeslice. Check that the executing task
 *     did yield.
 *
 *   - While servicing an ISR, nearly elapse a timeslice while the executing
 *     task is ready. Go back to task context. Perform a clock tick. Check that
 *     the timeslice was exhausted.
 *
 *   - While servicing an ISR, nearly elapse a timeslice while the executing
 *     task is not ready. Go back to task context. Perform a clock tick. Check
 *     that the timeslice was not exhausted.
 *
 *   - Restore runner mode. Delete worker task.
 *
 * @{
 */

/**
 * @brief Test context for spec:/rtems/task/val/timeslice test case.
 */
typedef struct {
  /**
   * @brief This member contains the runner identifier.
   */
  rtems_id runner_id;

  /**
   * @brief This member contains the worker identifier.
   */
  rtems_id worker_id;

  /**
   * @brief This member provides an excution counter for another task.
   */
  uint32_t counter;
} RtemsTaskValTimeslice_Context;

static RtemsTaskValTimeslice_Context RtemsTaskValTimeslice_Instance;

typedef RtemsTaskValTimeslice_Context Context;

static void NearlyElapseWhileNotReady( void *arg )
{
  Context       *ctx;
  rtems_interval timeslice;
  rtems_interval tick;

  ctx = arg;
  SuspendTask( ctx->runner_id );
  timeslice = rtems_configuration_get_ticks_per_timeslice();

  for ( tick = 1; tick < timeslice; ++tick ) {
    ClockTick();
  }

  ResumeTask( ctx->runner_id );
  SuspendTask( ctx->worker_id );
  ResumeTask( ctx->worker_id );
}

static void NearlyElapseWhileReady( void *arg )
{
  rtems_interval timeslice;
  rtems_interval tick;

  (void) arg;
  timeslice = rtems_configuration_get_ticks_per_timeslice();

  for ( tick = 1; tick < timeslice; ++tick ) {
    ClockTick();
  }
}

static void Worker( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;

  while ( true ) {
    ++ctx->counter;
    Yield();
  }
}

static T_fixture RtemsTaskValTimeslice_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = NULL,
  .initial_context = &RtemsTaskValTimeslice_Instance
};

/**
 * @brief Create a worker task which forever increments a counter and yields.
 */
static void RtemsTaskValTimeslice_Action_0(
  RtemsTaskValTimeslice_Context *ctx
)
{
  rtems_mode        restore_mode;
  rtems_mode        unused_mode;
  rtems_status_code sc;
  rtems_interval    timeslice;
  rtems_interval    tick;

  timeslice = rtems_configuration_get_ticks_per_timeslice();
  ctx->runner_id = rtems_task_self();
  ctx->worker_id = CreateTask( "WORK", GetSelfPriority() );
  StartTask( ctx->worker_id, Worker, ctx );
  Yield();
  T_eq_u32( ctx->counter, 1 );

  /*
   * While the preemption mode is disabled and timeslicing is enabled for the
   * executing task, elapse a full timeslice. Check that the executing task did
   * not yield.
   */
  sc = rtems_task_mode(
    RTEMS_NO_PREEMPT | RTEMS_TIMESLICE,
    RTEMS_PREEMPT_MASK | RTEMS_TIMESLICE_MASK,
    &restore_mode
  );
  T_rsc_success( sc );

  for ( tick = 0; tick < timeslice; ++tick ) {
    ClockTick();
  }

  T_eq_u32( ctx->counter, 1 );

  sc = rtems_task_mode( RTEMS_PREEMPT, RTEMS_PREEMPT_MASK, &unused_mode );
  T_rsc_success( sc );

  /*
   * While the preemption mode is enabled and timeslicing is enabled for the
   * executing task, elapse a full timeslice. Check that the executing task did
   * yield.
   */
  T_eq_u32( ctx->counter, 1 );

  for ( tick = 1; tick < timeslice; ++tick ) {
    ClockTick();
    T_eq_u32( ctx->counter, 1 );
  }

  ClockTick();
  T_eq_u32( ctx->counter, 2 );

  /*
   * While servicing an ISR, nearly elapse a timeslice while the executing task
   * is ready. Go back to task context. Perform a clock tick. Check that the
   * timeslice was exhausted.
   */
  CallWithinISR( NearlyElapseWhileReady, ctx );

  ClockTick();
  T_eq_u32( ctx->counter, 3 );

  /*
   * While servicing an ISR, nearly elapse a timeslice while the executing task
   * is not ready. Go back to task context. Perform a clock tick. Check that
   * the timeslice was not exhausted.
   */
  CallWithinISR( NearlyElapseWhileNotReady, ctx );

  ClockTick();
  T_eq_u32( ctx->counter, 3 );

  /*
   * Restore runner mode. Delete worker task.
   */
  sc = rtems_task_mode(
    restore_mode,
    RTEMS_PREEMPT_MASK | RTEMS_TIMESLICE_MASK,
    &unused_mode
  );
  T_rsc_success( sc );

  DeleteTask( ctx->worker_id );
}

/**
 * @fn void T_case_body_RtemsTaskValTimeslice( void )
 */
T_TEST_CASE_FIXTURE( RtemsTaskValTimeslice, &RtemsTaskValTimeslice_Fixture )
{
  RtemsTaskValTimeslice_Context *ctx;

  ctx = T_fixture_context();

  RtemsTaskValTimeslice_Action_0( ctx );
}

/** @} */
