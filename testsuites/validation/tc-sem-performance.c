/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsSemValPerf
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
 * @defgroup RtemsSemValPerf spec:/rtems/sem/val/perf
 *
 * @ingroup TestsuitesPerformanceNoClock0
 *
 * @brief This test case provides a context to run @ref RTEMSAPIClassicSem
 *   performance tests.
 *
 * @{
 */

/**
 * @brief Test context for spec:/rtems/sem/val/perf test case.
 */
typedef struct {
  /**
   * @brief This member provides a mutex identifier.
   */
  rtems_id mutex_id;

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
} RtemsSemValPerf_Context;

static RtemsSemValPerf_Context
  RtemsSemValPerf_Instance;

#define EVENT_END RTEMS_EVENT_0

#define EVENT_OBTAIN RTEMS_EVENT_1

#define EVENT_OBTAIN_END RTEMS_EVENT_2

#define EVENT_RELEASE RTEMS_EVENT_3

#define EVENT_RELEASE_END RTEMS_EVENT_4

typedef RtemsSemValPerf_Context Context;

static void Send( const Context *ctx, rtems_event_set events )
{
  SendEvents( ctx->worker_id, events );
}

static void Worker( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;

  while ( true ) {
    rtems_event_set   events;
    rtems_status_code sc;
    T_ticks           ticks;

    sc = rtems_event_receive(
      RTEMS_ALL_EVENTS,
      RTEMS_EVENT_ANY | RTEMS_WAIT,
      RTEMS_NO_TIMEOUT,
      &events
    );
    ticks = T_tick();
    T_quiet_rsc_success( sc );

    if ( ( events & EVENT_END ) != 0 ) {
      ctx->end = ticks;
    }

    if ( ( events & EVENT_OBTAIN ) != 0 ) {
      sc = rtems_semaphore_obtain(
        ctx->mutex_id,
        RTEMS_WAIT,
        RTEMS_NO_TIMEOUT
      );
      ticks = T_tick();
      T_quiet_rsc_success( sc );

      if ( ( events & EVENT_OBTAIN_END ) != 0 ) {
        ctx->end = ticks;
      }
    }

    if ( ( events & EVENT_RELEASE ) != 0 ) {
      sc = rtems_semaphore_release( ctx->mutex_id );
      ticks = T_tick();
      T_quiet_rsc_success( sc );

      if ( ( events & EVENT_RELEASE_END ) != 0 ) {
        ctx->end = ticks;
      }
    }
  }
}

static void RtemsSemValPerf_Setup_Context( RtemsSemValPerf_Context *ctx )
{
  T_measure_runtime_config config;

  memset( &config, 0, sizeof( config ) );
  config.sample_count = 100;
  ctx->request.arg = ctx;
  ctx->request.flags = T_MEASURE_RUNTIME_REPORT_SAMPLES;
  ctx->context = T_measure_runtime_create( &config );
  T_assert_not_null( ctx->context );
}

/**
 * @brief Create a mutex and a worker task.
 */
static void RtemsSemValPerf_Setup( RtemsSemValPerf_Context *ctx )
{
  SetSelfPriority( PRIO_NORMAL );
  ctx->mutex_id = CreateMutex();
  ctx->worker_id = CreateTask( "WORK", PRIO_HIGH );
  StartTask( ctx->worker_id, Worker, ctx );
}

static void RtemsSemValPerf_Setup_Wrap( void *arg )
{
  RtemsSemValPerf_Context *ctx;

  ctx = arg;
  RtemsSemValPerf_Setup_Context( ctx );
  RtemsSemValPerf_Setup( ctx );
}

/**
 * @brief Delete the worker task and the mutex.
 */
static void RtemsSemValPerf_Teardown( RtemsSemValPerf_Context *ctx )
{
  DeleteTask( ctx->worker_id );
  DeleteMutex( ctx->mutex_id );
  RestoreRunnerPriority();
}

static void RtemsSemValPerf_Teardown_Wrap( void *arg )
{
  RtemsSemValPerf_Context *ctx;

  ctx = arg;
  RtemsSemValPerf_Teardown( ctx );
}

static T_fixture RtemsSemValPerf_Fixture = {
  .setup = RtemsSemValPerf_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsSemValPerf_Teardown_Wrap,
  .scope = NULL,
  .initial_context = &RtemsSemValPerf_Instance
};

/**
 * @defgroup RtemsSemReqPerfMtxPiObtain spec:/rtems/sem/req/perf-mtx-pi-obtain
 *
 * @{
 */

/**
 * @brief Obtain the available mutex.
 */
static void RtemsSemReqPerfMtxPiObtain_Body( RtemsSemValPerf_Context *ctx )
{
  ctx->status = rtems_semaphore_obtain(
    ctx->mutex_id,
    RTEMS_WAIT,
    RTEMS_NO_TIMEOUT
  );
}

static void RtemsSemReqPerfMtxPiObtain_Body_Wrap( void *arg )
{
  RtemsSemValPerf_Context *ctx;

  ctx = arg;
  RtemsSemReqPerfMtxPiObtain_Body( ctx );
}

/**
 * @brief Release the mutex.  Discard samples interrupted by a clock tick.
 */
static bool RtemsSemReqPerfMtxPiObtain_Teardown(
  RtemsSemValPerf_Context *ctx,
  T_ticks                 *delta,
  uint32_t                 tic,
  uint32_t                 toc,
  unsigned int             retry
)
{
  T_quiet_rsc_success( ctx->status );

  ReleaseMutex( ctx->mutex_id );

  return tic == toc;
}

static bool RtemsSemReqPerfMtxPiObtain_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  RtemsSemValPerf_Context *ctx;

  ctx = arg;
  return RtemsSemReqPerfMtxPiObtain_Teardown( ctx, delta, tic, toc, retry );
}

/** @} */

/**
 * @defgroup RtemsSemReqPerfMtxPiRelease \
 *   spec:/rtems/sem/req/perf-mtx-pi-release
 *
 * @{
 */

/**
 * @brief Obtain the mutex.
 */
static void RtemsSemReqPerfMtxPiRelease_Setup( RtemsSemValPerf_Context *ctx )
{
  ObtainMutex( ctx->mutex_id );
}

static void RtemsSemReqPerfMtxPiRelease_Setup_Wrap( void *arg )
{
  RtemsSemValPerf_Context *ctx;

  ctx = arg;
  RtemsSemReqPerfMtxPiRelease_Setup( ctx );
}

/**
 * @brief Release the mutex.
 */
static void RtemsSemReqPerfMtxPiRelease_Body( RtemsSemValPerf_Context *ctx )
{
  ctx->status = rtems_semaphore_release( ctx->mutex_id );
}

static void RtemsSemReqPerfMtxPiRelease_Body_Wrap( void *arg )
{
  RtemsSemValPerf_Context *ctx;

  ctx = arg;
  RtemsSemReqPerfMtxPiRelease_Body( ctx );
}

/**
 * @brief Discard samples interrupted by a clock tick.
 */
static bool RtemsSemReqPerfMtxPiRelease_Teardown(
  RtemsSemValPerf_Context *ctx,
  T_ticks                 *delta,
  uint32_t                 tic,
  uint32_t                 toc,
  unsigned int             retry
)
{
  T_quiet_rsc_success( ctx->status );

  return tic == toc;
}

static bool RtemsSemReqPerfMtxPiRelease_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  RtemsSemValPerf_Context *ctx;

  ctx = arg;
  return RtemsSemReqPerfMtxPiRelease_Teardown( ctx, delta, tic, toc, retry );
}

/** @} */

/**
 * @defgroup RtemsSemReqPerfMtxPiReleaseOne \
 *   spec:/rtems/sem/req/perf-mtx-pi-release-one
 *
 * @{
 */

/**
 * @brief Let one task wait on the mutex.
 */
static void RtemsSemReqPerfMtxPiReleaseOne_Setup(
  RtemsSemValPerf_Context *ctx
)
{
  SetSelfPriority( PRIO_HIGH );
  ObtainMutex( ctx->mutex_id );
  Send( ctx, EVENT_OBTAIN );
  Yield();
  Send( ctx, EVENT_RELEASE );
}

static void RtemsSemReqPerfMtxPiReleaseOne_Setup_Wrap( void *arg )
{
  RtemsSemValPerf_Context *ctx;

  ctx = arg;
  RtemsSemReqPerfMtxPiReleaseOne_Setup( ctx );
}

/**
 * @brief Release the mutex.
 */
static void RtemsSemReqPerfMtxPiReleaseOne_Body( RtemsSemValPerf_Context *ctx )
{
  /*
   * The release will unblock the worker task which has our priority.  The
   * scheduler ensures FIFO ordering for ready threads of the same priority, so
   * the release will not preempt us.
   */
  ctx->status = rtems_semaphore_release( ctx->mutex_id );
}

static void RtemsSemReqPerfMtxPiReleaseOne_Body_Wrap( void *arg )
{
  RtemsSemValPerf_Context *ctx;

  ctx = arg;
  RtemsSemReqPerfMtxPiReleaseOne_Body( ctx );
}

/**
 * @brief Restore the worker priority.  Discard samples interrupted by a clock
 *   tick.
 */
static bool RtemsSemReqPerfMtxPiReleaseOne_Teardown(
  RtemsSemValPerf_Context *ctx,
  T_ticks                 *delta,
  uint32_t                 tic,
  uint32_t                 toc,
  unsigned int             retry
)
{
  T_quiet_rsc( ctx->status, RTEMS_SUCCESSFUL );

  SetSelfPriority( PRIO_NORMAL );

  return tic == toc;
}

static bool RtemsSemReqPerfMtxPiReleaseOne_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  RtemsSemValPerf_Context *ctx;

  ctx = arg;
  return RtemsSemReqPerfMtxPiReleaseOne_Teardown(
    ctx,
    delta,
    tic,
    toc,
    retry
  );
}

/** @} */

#if defined(RTEMS_SMP)
/**
 * @defgroup RtemsSemReqPerfMtxPiReleaseOtherCpu \
 *   spec:/rtems/sem/req/perf-mtx-pi-release-other-cpu
 *
 * @{
 */

/**
 * @brief Move worker to scheduler B.
 */
static void RtemsSemReqPerfMtxPiReleaseOtherCpu_Prepare(
  RtemsSemValPerf_Context *ctx
)
{
  SetScheduler( ctx->worker_id, SCHEDULER_B_ID, PRIO_NORMAL );
}

/**
 * @brief Let one task wait on the mutex.
 */
static void RtemsSemReqPerfMtxPiReleaseOtherCpu_Setup(
  RtemsSemValPerf_Context *ctx
)
{
  ObtainMutex( ctx->mutex_id );
  Send( ctx, EVENT_OBTAIN | EVENT_OBTAIN_END | EVENT_RELEASE );
  WaitForNextTask( 1, ctx->worker_id );
}

static void RtemsSemReqPerfMtxPiReleaseOtherCpu_Setup_Wrap( void *arg )
{
  RtemsSemValPerf_Context *ctx;

  ctx = arg;
  RtemsSemReqPerfMtxPiReleaseOtherCpu_Setup( ctx );
}

/**
 * @brief Release the mutex.
 */
static void RtemsSemReqPerfMtxPiReleaseOtherCpu_Body(
  RtemsSemValPerf_Context *ctx
)
{
  ctx->begin = T_tick();
  ctx->status = rtems_semaphore_release( ctx->mutex_id );
}

static void RtemsSemReqPerfMtxPiReleaseOtherCpu_Body_Wrap( void *arg )
{
  RtemsSemValPerf_Context *ctx;

  ctx = arg;
  RtemsSemReqPerfMtxPiReleaseOtherCpu_Body( ctx );
}

/**
 * @brief Make sure the worker waits for the next event.  Set the measured
 *   runtime. Discard samples interrupted by a clock tick.
 */
static bool RtemsSemReqPerfMtxPiReleaseOtherCpu_Teardown(
  RtemsSemValPerf_Context *ctx,
  T_ticks                 *delta,
  uint32_t                 tic,
  uint32_t                 toc,
  unsigned int             retry
)
{
  T_quiet_rsc( ctx->status, RTEMS_SUCCESSFUL );

  WaitForNextTask( 1, ctx->worker_id );
  *delta = ctx->end - ctx->begin;

  return tic == toc;
}

static bool RtemsSemReqPerfMtxPiReleaseOtherCpu_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  RtemsSemValPerf_Context *ctx;

  ctx = arg;
  return RtemsSemReqPerfMtxPiReleaseOtherCpu_Teardown(
    ctx,
    delta,
    tic,
    toc,
    retry
  );
}

/**
 * @brief Move worker to scheduler A.
 */
static void RtemsSemReqPerfMtxPiReleaseOtherCpu_Cleanup(
  RtemsSemValPerf_Context *ctx
)
{
  SetScheduler( ctx->worker_id, SCHEDULER_A_ID, PRIO_HIGH );
}

/** @} */
#endif

/**
 * @defgroup RtemsSemReqPerfMtxPiReleasePreempt \
 *   spec:/rtems/sem/req/perf-mtx-pi-release-preempt
 *
 * @{
 */

/**
 * @brief Let one task wait on the mutex.
 */
static void RtemsSemReqPerfMtxPiReleasePreempt_Setup(
  RtemsSemValPerf_Context *ctx
)
{
  ObtainMutex( ctx->mutex_id );
  Send( ctx, EVENT_OBTAIN | EVENT_OBTAIN_END | EVENT_RELEASE );
}

static void RtemsSemReqPerfMtxPiReleasePreempt_Setup_Wrap( void *arg )
{
  RtemsSemValPerf_Context *ctx;

  ctx = arg;
  RtemsSemReqPerfMtxPiReleasePreempt_Setup( ctx );
}

/**
 * @brief Release the mutex.
 */
static void RtemsSemReqPerfMtxPiReleasePreempt_Body(
  RtemsSemValPerf_Context *ctx
)
{
  ctx->begin = T_tick();
  ctx->status = rtems_semaphore_release( ctx->mutex_id );
}

static void RtemsSemReqPerfMtxPiReleasePreempt_Body_Wrap( void *arg )
{
  RtemsSemValPerf_Context *ctx;

  ctx = arg;
  RtemsSemReqPerfMtxPiReleasePreempt_Body( ctx );
}

/**
 * @brief Set the measured runtime.  Discard samples interrupted by a clock
 *   tick.
 */
static bool RtemsSemReqPerfMtxPiReleasePreempt_Teardown(
  RtemsSemValPerf_Context *ctx,
  T_ticks                 *delta,
  uint32_t                 tic,
  uint32_t                 toc,
  unsigned int             retry
)
{
  T_quiet_rsc( ctx->status, RTEMS_SUCCESSFUL );

  *delta = ctx->end - ctx->begin;

  return tic == toc;
}

static bool RtemsSemReqPerfMtxPiReleasePreempt_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  RtemsSemValPerf_Context *ctx;

  ctx = arg;
  return RtemsSemReqPerfMtxPiReleasePreempt_Teardown(
    ctx,
    delta,
    tic,
    toc,
    retry
  );
}

/** @} */

/**
 * @defgroup RtemsSemReqPerfMtxPiTry spec:/rtems/sem/req/perf-mtx-pi-try
 *
 * @{
 */

/**
 * @brief Make the mutex unavailable.
 */
static void RtemsSemReqPerfMtxPiTry_Prepare( RtemsSemValPerf_Context *ctx )
{
  Send( ctx, EVENT_OBTAIN );
}

/**
 * @brief Try to obtain the unavailable mutex.
 */
static void RtemsSemReqPerfMtxPiTry_Body( RtemsSemValPerf_Context *ctx )
{
  ctx->status = rtems_semaphore_obtain( ctx->mutex_id, RTEMS_NO_WAIT, 0 );
}

static void RtemsSemReqPerfMtxPiTry_Body_Wrap( void *arg )
{
  RtemsSemValPerf_Context *ctx;

  ctx = arg;
  RtemsSemReqPerfMtxPiTry_Body( ctx );
}

/**
 * @brief Discard samples interrupted by a clock tick.
 */
static bool RtemsSemReqPerfMtxPiTry_Teardown(
  RtemsSemValPerf_Context *ctx,
  T_ticks                 *delta,
  uint32_t                 tic,
  uint32_t                 toc,
  unsigned int             retry
)
{
  T_quiet_rsc( ctx->status, RTEMS_UNSATISFIED );

  return tic == toc;
}

static bool RtemsSemReqPerfMtxPiTry_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  RtemsSemValPerf_Context *ctx;

  ctx = arg;
  return RtemsSemReqPerfMtxPiTry_Teardown( ctx, delta, tic, toc, retry );
}

/**
 * @brief Make the mutex available.
 */
static void RtemsSemReqPerfMtxPiTry_Cleanup( RtemsSemValPerf_Context *ctx )
{
  Send( ctx, EVENT_RELEASE );
}

/** @} */

/**
 * @defgroup RtemsSemReqPerfMtxPiWaitForever \
 *   spec:/rtems/sem/req/perf-mtx-pi-wait-forever
 *
 * @{
 */

/**
 * @brief Make the mutex unavailable.
 */
static void RtemsSemReqPerfMtxPiWaitForever_Setup(
  RtemsSemValPerf_Context *ctx
)
{
  Send( ctx, EVENT_OBTAIN );
  SetSelfPriority( PRIO_VERY_HIGH );
  Send( ctx, EVENT_END | EVENT_RELEASE );
}

static void RtemsSemReqPerfMtxPiWaitForever_Setup_Wrap( void *arg )
{
  RtemsSemValPerf_Context *ctx;

  ctx = arg;
  RtemsSemReqPerfMtxPiWaitForever_Setup( ctx );
}

/**
 * @brief Obtain the unavailable mutex and wait forever.
 */
static void RtemsSemReqPerfMtxPiWaitForever_Body(
  RtemsSemValPerf_Context *ctx
)
{
  ctx->begin = T_tick();
  ctx->status = rtems_semaphore_obtain(
    ctx->mutex_id,
    RTEMS_WAIT,
    RTEMS_NO_TIMEOUT
  );
}

static void RtemsSemReqPerfMtxPiWaitForever_Body_Wrap( void *arg )
{
  RtemsSemValPerf_Context *ctx;

  ctx = arg;
  RtemsSemReqPerfMtxPiWaitForever_Body( ctx );
}

/**
 * @brief Set the measured runtime.  Restore the worker priority.  Release the
 *   mutex. Discard samples interrupted by a clock tick.
 */
static bool RtemsSemReqPerfMtxPiWaitForever_Teardown(
  RtemsSemValPerf_Context *ctx,
  T_ticks                 *delta,
  uint32_t                 tic,
  uint32_t                 toc,
  unsigned int             retry
)
{
  T_quiet_rsc( ctx->status, RTEMS_SUCCESSFUL );

  *delta = ctx->end - ctx->begin;
  ReleaseMutex( ctx->mutex_id );
  SetSelfPriority( PRIO_NORMAL );

  return tic == toc;
}

static bool RtemsSemReqPerfMtxPiWaitForever_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  RtemsSemValPerf_Context *ctx;

  ctx = arg;
  return RtemsSemReqPerfMtxPiWaitForever_Teardown(
    ctx,
    delta,
    tic,
    toc,
    retry
  );
}

/** @} */

/**
 * @defgroup RtemsSemReqPerfMtxPiWaitTimed \
 *   spec:/rtems/sem/req/perf-mtx-pi-wait-timed
 *
 * @{
 */

/**
 * @brief Make the mutex unavailable.
 */
static void RtemsSemReqPerfMtxPiWaitTimed_Setup( RtemsSemValPerf_Context *ctx )
{
  Send( ctx, EVENT_OBTAIN );
  SetSelfPriority( PRIO_VERY_HIGH );
  Send( ctx, EVENT_END | EVENT_RELEASE );
}

static void RtemsSemReqPerfMtxPiWaitTimed_Setup_Wrap( void *arg )
{
  RtemsSemValPerf_Context *ctx;

  ctx = arg;
  RtemsSemReqPerfMtxPiWaitTimed_Setup( ctx );
}

/**
 * @brief Obtain the unavailable mutex and wait forever.
 */
static void RtemsSemReqPerfMtxPiWaitTimed_Body( RtemsSemValPerf_Context *ctx )
{
  ctx->begin = T_tick();
  ctx->status = rtems_semaphore_obtain(
    ctx->mutex_id,
    RTEMS_WAIT,
    UINT32_MAX
  );
}

static void RtemsSemReqPerfMtxPiWaitTimed_Body_Wrap( void *arg )
{
  RtemsSemValPerf_Context *ctx;

  ctx = arg;
  RtemsSemReqPerfMtxPiWaitTimed_Body( ctx );
}

/**
 * @brief Set the measured runtime.  Restore the worker priority.  Release the
 *   mutex. Discard samples interrupted by a clock tick.
 */
static bool RtemsSemReqPerfMtxPiWaitTimed_Teardown(
  RtemsSemValPerf_Context *ctx,
  T_ticks                 *delta,
  uint32_t                 tic,
  uint32_t                 toc,
  unsigned int             retry
)
{
  T_quiet_rsc( ctx->status, RTEMS_SUCCESSFUL );

  *delta = ctx->end - ctx->begin;
  ReleaseMutex( ctx->mutex_id );
  SetSelfPriority( PRIO_NORMAL );

  return tic == toc;
}

static bool RtemsSemReqPerfMtxPiWaitTimed_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  RtemsSemValPerf_Context *ctx;

  ctx = arg;
  return RtemsSemReqPerfMtxPiWaitTimed_Teardown( ctx, delta, tic, toc, retry );
}

/** @} */

/**
 * @fn void T_case_body_RtemsSemValPerf( void )
 */
T_TEST_CASE_FIXTURE( RtemsSemValPerf, &RtemsSemValPerf_Fixture )
{
  RtemsSemValPerf_Context *ctx;

  ctx = T_fixture_context();

  ctx->request.name = "RtemsSemReqPerfMtxPiObtain";
  ctx->request.setup = NULL;
  ctx->request.body = RtemsSemReqPerfMtxPiObtain_Body_Wrap;
  ctx->request.teardown = RtemsSemReqPerfMtxPiObtain_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  ctx->request.name = "RtemsSemReqPerfMtxPiRelease";
  ctx->request.setup = RtemsSemReqPerfMtxPiRelease_Setup_Wrap;
  ctx->request.body = RtemsSemReqPerfMtxPiRelease_Body_Wrap;
  ctx->request.teardown = RtemsSemReqPerfMtxPiRelease_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  ctx->request.name = "RtemsSemReqPerfMtxPiReleaseOne";
  ctx->request.setup = RtemsSemReqPerfMtxPiReleaseOne_Setup_Wrap;
  ctx->request.body = RtemsSemReqPerfMtxPiReleaseOne_Body_Wrap;
  ctx->request.teardown = RtemsSemReqPerfMtxPiReleaseOne_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  #if defined(RTEMS_SMP)
  RtemsSemReqPerfMtxPiReleaseOtherCpu_Prepare( ctx );
  ctx->request.name = "RtemsSemReqPerfMtxPiReleaseOtherCpu";
  ctx->request.setup = RtemsSemReqPerfMtxPiReleaseOtherCpu_Setup_Wrap;
  ctx->request.body = RtemsSemReqPerfMtxPiReleaseOtherCpu_Body_Wrap;
  ctx->request.teardown = RtemsSemReqPerfMtxPiReleaseOtherCpu_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );
  RtemsSemReqPerfMtxPiReleaseOtherCpu_Cleanup( ctx );
  #endif

  ctx->request.name = "RtemsSemReqPerfMtxPiReleasePreempt";
  ctx->request.setup = RtemsSemReqPerfMtxPiReleasePreempt_Setup_Wrap;
  ctx->request.body = RtemsSemReqPerfMtxPiReleasePreempt_Body_Wrap;
  ctx->request.teardown = RtemsSemReqPerfMtxPiReleasePreempt_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  RtemsSemReqPerfMtxPiTry_Prepare( ctx );
  ctx->request.name = "RtemsSemReqPerfMtxPiTry";
  ctx->request.setup = NULL;
  ctx->request.body = RtemsSemReqPerfMtxPiTry_Body_Wrap;
  ctx->request.teardown = RtemsSemReqPerfMtxPiTry_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );
  RtemsSemReqPerfMtxPiTry_Cleanup( ctx );

  ctx->request.name = "RtemsSemReqPerfMtxPiWaitForever";
  ctx->request.setup = RtemsSemReqPerfMtxPiWaitForever_Setup_Wrap;
  ctx->request.body = RtemsSemReqPerfMtxPiWaitForever_Body_Wrap;
  ctx->request.teardown = RtemsSemReqPerfMtxPiWaitForever_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  ctx->request.name = "RtemsSemReqPerfMtxPiWaitTimed";
  ctx->request.setup = RtemsSemReqPerfMtxPiWaitTimed_Setup_Wrap;
  ctx->request.body = RtemsSemReqPerfMtxPiWaitTimed_Body_Wrap;
  ctx->request.teardown = RtemsSemReqPerfMtxPiWaitTimed_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );
}

/** @} */
