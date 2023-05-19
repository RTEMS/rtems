/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsBarrierValPerf
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
 * @defgroup RtemsBarrierValPerf spec:/rtems/barrier/val/perf
 *
 * @ingroup TestsuitesPerformanceNoClock0
 *
 * @brief This test case provides a context to run @ref RTEMSAPIClassicBarrier
 *   performance tests.
 *
 * @{
 */

/**
 * @brief Test context for spec:/rtems/barrier/val/perf test case.
 */
typedef struct {
  /**
   * @brief This member provides a barrier identifier.
   */
  rtems_id barrier_id;

  /**
   * @brief This member provides a worker identifier.
   */
  rtems_id worker_id;

  /**
   * @brief This member provides a status code.
   */
  rtems_status_code status;

  /**
   * @brief This member references the measure runtime context.
   */
  T_measure_runtime_context *context;

  /**
   * @brief This member provides the measure runtime request.
   */
  T_measure_runtime_request request;

  /**
   * @brief This member provides an optional measurement begin time point.
   */
  T_ticks begin;

  /**
   * @brief This member provides an optional measurement end time point.
   */
  T_ticks end;
} RtemsBarrierValPerf_Context;

static RtemsBarrierValPerf_Context
  RtemsBarrierValPerf_Instance;

typedef RtemsBarrierValPerf_Context Context;

static void BarrierWaitWorker( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;

  while ( true ) {
    rtems_status_code sc;

    sc = rtems_barrier_wait( ctx->barrier_id, RTEMS_NO_TIMEOUT );
    ctx->end = T_tick();
    T_quiet_rsc_success( sc );
  }
}

static void RtemsBarrierValPerf_Setup_Context(
  RtemsBarrierValPerf_Context *ctx
)
{
  T_measure_runtime_config config;

  memset( &config, 0, sizeof( config ) );
  config.sample_count = 100;
  ctx->request.arg = ctx;
  ctx->request.flags = T_MEASURE_RUNTIME_REPORT_SAMPLES;
  ctx->context = T_measure_runtime_create( &config );
  T_assert_not_null( ctx->context );
}

static void RtemsBarrierValPerf_Setup_Wrap( void *arg )
{
  RtemsBarrierValPerf_Context *ctx;

  ctx = arg;
  RtemsBarrierValPerf_Setup_Context( ctx );
}

static T_fixture RtemsBarrierValPerf_Fixture = {
  .setup = RtemsBarrierValPerf_Setup_Wrap,
  .stop = NULL,
  .teardown = NULL,
  .scope = NULL,
  .initial_context = &RtemsBarrierValPerf_Instance
};

/**
 * @defgroup RtemsBarrierReqPerfReleaseAuto \
 *   spec:/rtems/barrier/req/perf-release-auto
 *
 * @{
 */

/**
 * @brief Create an automatic release barrier.
 */
static void RtemsBarrierReqPerfReleaseAuto_Prepare(
  RtemsBarrierValPerf_Context *ctx
)
{
  rtems_status_code sc;

  sc = rtems_barrier_create(
    OBJECT_NAME,
    RTEMS_BARRIER_AUTOMATIC_RELEASE,
    1,
    &ctx->barrier_id
  );
  T_rsc_success( sc );
}

/**
 * @brief Automatically release the barrier.
 */
static void RtemsBarrierReqPerfReleaseAuto_Body(
  RtemsBarrierValPerf_Context *ctx
)
{
  ctx->status = rtems_barrier_wait( ctx->barrier_id, RTEMS_NO_TIMEOUT );
}

static void RtemsBarrierReqPerfReleaseAuto_Body_Wrap( void *arg )
{
  RtemsBarrierValPerf_Context *ctx;

  ctx = arg;
  RtemsBarrierReqPerfReleaseAuto_Body( ctx );
}

/**
 * @brief Discard samples interrupted by a clock tick.
 */
static bool RtemsBarrierReqPerfReleaseAuto_Teardown(
  RtemsBarrierValPerf_Context *ctx,
  T_ticks                     *delta,
  uint32_t                     tic,
  uint32_t                     toc,
  unsigned int                 retry
)
{
  T_quiet_rsc_success( ctx->status );

  return tic == toc;
}

static bool RtemsBarrierReqPerfReleaseAuto_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  RtemsBarrierValPerf_Context *ctx;

  ctx = arg;
  return RtemsBarrierReqPerfReleaseAuto_Teardown(
    ctx,
    delta,
    tic,
    toc,
    retry
  );
}

/**
 * @brief Delete the barrier and the worker.
 */
static void RtemsBarrierReqPerfReleaseAuto_Cleanup(
  RtemsBarrierValPerf_Context *ctx
)
{
  rtems_status_code sc;

  sc = rtems_barrier_delete( ctx->barrier_id );
  T_rsc_success( sc );
}

/** @} */

#if defined(RTEMS_SMP)
/**
 * @defgroup RtemsBarrierReqPerfReleaseAutoOtherCpu \
 *   spec:/rtems/barrier/req/perf-release-auto-other-cpu
 *
 * @{
 */

/**
 * @brief Create an automatic release barrier.  Create and start a worker task.
 */
static void RtemsBarrierReqPerfReleaseAutoOtherCpu_Prepare(
  RtemsBarrierValPerf_Context *ctx
)
{
  rtems_status_code sc;

  sc = rtems_barrier_create(
    OBJECT_NAME,
    RTEMS_BARRIER_AUTOMATIC_RELEASE,
    2,
    &ctx->barrier_id
  );
  T_rsc_success( sc );

  ctx->worker_id = CreateTask( "WORK", PRIO_NORMAL );
  SetScheduler( ctx->worker_id, SCHEDULER_B_ID, PRIO_NORMAL );
  StartTask( ctx->worker_id, BarrierWaitWorker, ctx );
  WaitForNextTask( 1, ctx->worker_id );
}

/**
 * @brief Automatically release the barrier.
 */
static void RtemsBarrierReqPerfReleaseAutoOtherCpu_Body(
  RtemsBarrierValPerf_Context *ctx
)
{
  ctx->begin = T_tick();
  ctx->status = rtems_barrier_wait( ctx->barrier_id, RTEMS_NO_TIMEOUT );
}

static void RtemsBarrierReqPerfReleaseAutoOtherCpu_Body_Wrap( void *arg )
{
  RtemsBarrierValPerf_Context *ctx;

  ctx = arg;
  RtemsBarrierReqPerfReleaseAutoOtherCpu_Body( ctx );
}

/**
 * @brief Make sure the worker waits for the next event.  Set the measured
 *   runtime. Discard samples interrupted by a clock tick.
 */
static bool RtemsBarrierReqPerfReleaseAutoOtherCpu_Teardown(
  RtemsBarrierValPerf_Context *ctx,
  T_ticks                     *delta,
  uint32_t                     tic,
  uint32_t                     toc,
  unsigned int                 retry
)
{
  T_quiet_rsc_success( ctx->status );

  WaitForNextTask( 1, ctx->worker_id );
  *delta = ctx->end - ctx->begin;

  return tic == toc;
}

static bool RtemsBarrierReqPerfReleaseAutoOtherCpu_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  RtemsBarrierValPerf_Context *ctx;

  ctx = arg;
  return RtemsBarrierReqPerfReleaseAutoOtherCpu_Teardown(
    ctx,
    delta,
    tic,
    toc,
    retry
  );
}

/**
 * @brief Delete the barrier and the worker.
 */
static void RtemsBarrierReqPerfReleaseAutoOtherCpu_Cleanup(
  RtemsBarrierValPerf_Context *ctx
)
{
  rtems_status_code sc;

  DeleteTask( ctx->worker_id );

  sc = rtems_barrier_delete( ctx->barrier_id );
  T_rsc_success( sc );
}

/** @} */
#endif

/**
 * @defgroup RtemsBarrierReqPerfReleaseManual \
 *   spec:/rtems/barrier/req/perf-release-manual
 *
 * @{
 */

/**
 * @brief Create a manual release barrier.  Create and start a worker task.
 */
static void RtemsBarrierReqPerfReleaseManual_Prepare(
  RtemsBarrierValPerf_Context *ctx
)
{
  rtems_status_code sc;

  sc = rtems_barrier_create(
    OBJECT_NAME,
    RTEMS_BARRIER_MANUAL_RELEASE,
    0,
    &ctx->barrier_id
  );
  T_rsc_success( sc );

  SetSelfPriority( PRIO_NORMAL );
  ctx->worker_id = CreateTask( "WORK", PRIO_NORMAL );
  StartTask( ctx->worker_id, BarrierWaitWorker, ctx );
}

/**
 * @brief Make sure the worker task is fully blocked on the barrier.
 */
static void RtemsBarrierReqPerfReleaseManual_Setup(
  RtemsBarrierValPerf_Context *ctx
)
{
  Yield();
}

static void RtemsBarrierReqPerfReleaseManual_Setup_Wrap( void *arg )
{
  RtemsBarrierValPerf_Context *ctx;

  ctx = arg;
  RtemsBarrierReqPerfReleaseManual_Setup( ctx );
}

/**
 * @brief Release the barrier.
 */
static void RtemsBarrierReqPerfReleaseManual_Body(
  RtemsBarrierValPerf_Context *ctx
)
{
  uint32_t count;

  ctx->status = rtems_barrier_release( ctx->barrier_id, &count );
}

static void RtemsBarrierReqPerfReleaseManual_Body_Wrap( void *arg )
{
  RtemsBarrierValPerf_Context *ctx;

  ctx = arg;
  RtemsBarrierReqPerfReleaseManual_Body( ctx );
}

/**
 * @brief Discard samples interrupted by a clock tick.
 */
static bool RtemsBarrierReqPerfReleaseManual_Teardown(
  RtemsBarrierValPerf_Context *ctx,
  T_ticks                     *delta,
  uint32_t                     tic,
  uint32_t                     toc,
  unsigned int                 retry
)
{
  T_quiet_rsc_success( ctx->status );

  return tic == toc;
}

static bool RtemsBarrierReqPerfReleaseManual_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  RtemsBarrierValPerf_Context *ctx;

  ctx = arg;
  return RtemsBarrierReqPerfReleaseManual_Teardown(
    ctx,
    delta,
    tic,
    toc,
    retry
  );
}

/**
 * @brief Delete the barrier and the worker.
 */
static void RtemsBarrierReqPerfReleaseManual_Cleanup(
  RtemsBarrierValPerf_Context *ctx
)
{
  rtems_status_code sc;

  DeleteTask( ctx->worker_id );

  sc = rtems_barrier_delete( ctx->barrier_id );
  T_rsc_success( sc );

  RestoreRunnerPriority();
}

/** @} */

/**
 * @defgroup RtemsBarrierReqPerfReleaseManualPreempt \
 *   spec:/rtems/barrier/req/perf-release-manual-preempt
 *
 * @{
 */

/**
 * @brief Create a manual release barrier.  Create and start a worker task.
 */
static void RtemsBarrierReqPerfReleaseManualPreempt_Prepare(
  RtemsBarrierValPerf_Context *ctx
)
{
  rtems_status_code sc;

  sc = rtems_barrier_create(
    OBJECT_NAME,
    RTEMS_BARRIER_MANUAL_RELEASE,
    0,
    &ctx->barrier_id
  );
  T_rsc_success( sc );

  SetSelfPriority( PRIO_NORMAL );
  ctx->worker_id = CreateTask( "WORK", PRIO_HIGH );
  StartTask( ctx->worker_id, BarrierWaitWorker, ctx );
}

/**
 * @brief Release the barrier.
 */
static void RtemsBarrierReqPerfReleaseManualPreempt_Body(
  RtemsBarrierValPerf_Context *ctx
)
{
  uint32_t count;

  ctx->begin = T_tick();
  ctx->status = rtems_barrier_release( ctx->barrier_id, &count );
}

static void RtemsBarrierReqPerfReleaseManualPreempt_Body_Wrap( void *arg )
{
  RtemsBarrierValPerf_Context *ctx;

  ctx = arg;
  RtemsBarrierReqPerfReleaseManualPreempt_Body( ctx );
}

/**
 * @brief Set the measured runtime.  Discard samples interrupted by a clock
 *   tick.
 */
static bool RtemsBarrierReqPerfReleaseManualPreempt_Teardown(
  RtemsBarrierValPerf_Context *ctx,
  T_ticks                     *delta,
  uint32_t                     tic,
  uint32_t                     toc,
  unsigned int                 retry
)
{
  T_quiet_rsc_success( ctx->status );

  *delta = ctx->end - ctx->begin;

  return tic == toc;
}

static bool RtemsBarrierReqPerfReleaseManualPreempt_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  RtemsBarrierValPerf_Context *ctx;

  ctx = arg;
  return RtemsBarrierReqPerfReleaseManualPreempt_Teardown(
    ctx,
    delta,
    tic,
    toc,
    retry
  );
}

/**
 * @brief Delete the barrier and the worker.
 */
static void RtemsBarrierReqPerfReleaseManualPreempt_Cleanup(
  RtemsBarrierValPerf_Context *ctx
)
{
  rtems_status_code sc;

  DeleteTask( ctx->worker_id );

  sc = rtems_barrier_delete( ctx->barrier_id );
  T_rsc_success( sc );

  RestoreRunnerPriority();
}

/** @} */

/**
 * @fn void T_case_body_RtemsBarrierValPerf( void )
 */
T_TEST_CASE_FIXTURE( RtemsBarrierValPerf, &RtemsBarrierValPerf_Fixture )
{
  RtemsBarrierValPerf_Context *ctx;

  ctx = T_fixture_context();

  RtemsBarrierReqPerfReleaseAuto_Prepare( ctx );
  ctx->request.name = "RtemsBarrierReqPerfReleaseAuto";
  ctx->request.setup = NULL;
  ctx->request.body = RtemsBarrierReqPerfReleaseAuto_Body_Wrap;
  ctx->request.teardown = RtemsBarrierReqPerfReleaseAuto_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );
  RtemsBarrierReqPerfReleaseAuto_Cleanup( ctx );

  #if defined(RTEMS_SMP)
  RtemsBarrierReqPerfReleaseAutoOtherCpu_Prepare( ctx );
  ctx->request.name = "RtemsBarrierReqPerfReleaseAutoOtherCpu";
  ctx->request.setup = NULL;
  ctx->request.body = RtemsBarrierReqPerfReleaseAutoOtherCpu_Body_Wrap;
  ctx->request.teardown = RtemsBarrierReqPerfReleaseAutoOtherCpu_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );
  RtemsBarrierReqPerfReleaseAutoOtherCpu_Cleanup( ctx );
  #endif

  RtemsBarrierReqPerfReleaseManual_Prepare( ctx );
  ctx->request.name = "RtemsBarrierReqPerfReleaseManual";
  ctx->request.setup = RtemsBarrierReqPerfReleaseManual_Setup_Wrap;
  ctx->request.body = RtemsBarrierReqPerfReleaseManual_Body_Wrap;
  ctx->request.teardown = RtemsBarrierReqPerfReleaseManual_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );
  RtemsBarrierReqPerfReleaseManual_Cleanup( ctx );

  RtemsBarrierReqPerfReleaseManualPreempt_Prepare( ctx );
  ctx->request.name = "RtemsBarrierReqPerfReleaseManualPreempt";
  ctx->request.setup = NULL;
  ctx->request.body = RtemsBarrierReqPerfReleaseManualPreempt_Body_Wrap;
  ctx->request.teardown = RtemsBarrierReqPerfReleaseManualPreempt_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );
  RtemsBarrierReqPerfReleaseManualPreempt_Cleanup( ctx );
}

/** @} */
