/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsEventValPerf
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
 * @defgroup RtemsEventValPerf spec:/rtems/event/val/perf
 *
 * @ingroup TestsuitesPerformanceNoClock0
 *
 * @brief This test case provides a context to run @ref RTEMSAPIClassicEvent
 *   performance tests.
 *
 * @{
 */

/**
 * @brief Test context for spec:/rtems/event/val/perf test case.
 */
typedef struct {
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
} RtemsEventValPerf_Context;

static RtemsEventValPerf_Context
  RtemsEventValPerf_Instance;

#define EVENT_END RTEMS_EVENT_0

#define EVENT_OTHER RTEMS_EVENT_1

typedef RtemsEventValPerf_Context Context;

static void Send( const Context *ctx, rtems_event_set events )
{
  SendEvents( ctx->worker_id, events );
}

static void Satisfy( void *arg )
{
  Context *ctx;

  ctx = arg;
  ctx->begin = T_tick();
  ctx->status = rtems_event_send( ctx->worker_id, EVENT_END | EVENT_OTHER );
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
      EVENT_END | EVENT_OTHER,
      RTEMS_EVENT_ALL | RTEMS_WAIT,
      RTEMS_NO_TIMEOUT,
      &events
    );
    ticks = T_tick();
    T_quiet_rsc_success( sc );

    if ( ( events & EVENT_END ) != 0 ) {
      ctx->end = ticks;
    }
  }
}

static void RtemsEventValPerf_Setup_Context( RtemsEventValPerf_Context *ctx )
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
static void RtemsEventValPerf_Setup( RtemsEventValPerf_Context *ctx )
{
  SetSelfPriority( PRIO_NORMAL );
  ctx->worker_id = CreateTask( "WORK", PRIO_HIGH );
  StartTask( ctx->worker_id, Worker, ctx );
}

static void RtemsEventValPerf_Setup_Wrap( void *arg )
{
  RtemsEventValPerf_Context *ctx;

  ctx = arg;
  RtemsEventValPerf_Setup_Context( ctx );
  RtemsEventValPerf_Setup( ctx );
}

/**
 * @brief Delete the worker task and the mutex.
 */
static void RtemsEventValPerf_Teardown( RtemsEventValPerf_Context *ctx )
{
  DeleteTask( ctx->worker_id );
  RestoreRunnerPriority();
}

static void RtemsEventValPerf_Teardown_Wrap( void *arg )
{
  RtemsEventValPerf_Context *ctx;

  ctx = arg;
  RtemsEventValPerf_Teardown( ctx );
}

static T_fixture RtemsEventValPerf_Fixture = {
  .setup = RtemsEventValPerf_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsEventValPerf_Teardown_Wrap,
  .scope = NULL,
  .initial_context = &RtemsEventValPerf_Instance
};

/**
 * @defgroup RtemsEventReqPerfIsrPreempt spec:/rtems/event/req/perf-isr-preempt
 *
 * @{
 */

/**
 * @brief Send two events from with interrupt context.  Satisfy the event
 *   condition.
 */
static void RtemsEventReqPerfIsrPreempt_Body( RtemsEventValPerf_Context *ctx )
{
  CallWithinISR( Satisfy, ctx );
}

static void RtemsEventReqPerfIsrPreempt_Body_Wrap( void *arg )
{
  RtemsEventValPerf_Context *ctx;

  ctx = arg;
  RtemsEventReqPerfIsrPreempt_Body( ctx );
}

/**
 * @brief Set the measured runtime.  Discard samples interrupted by a clock
 *   tick.
 */
static bool RtemsEventReqPerfIsrPreempt_Teardown(
  RtemsEventValPerf_Context *ctx,
  T_ticks                   *delta,
  uint32_t                   tic,
  uint32_t                   toc,
  unsigned int               retry
)
{
  T_quiet_rsc_success( ctx->status );

  *delta = ctx->end - ctx->begin;

  return tic == toc;
}

static bool RtemsEventReqPerfIsrPreempt_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  RtemsEventValPerf_Context *ctx;

  ctx = arg;
  return RtemsEventReqPerfIsrPreempt_Teardown( ctx, delta, tic, toc, retry );
}

/** @} */

/**
 * @defgroup RtemsEventReqPerfOther spec:/rtems/event/req/perf-other
 *
 * @{
 */

/**
 * @brief Lower the worker priority.
 */
static void RtemsEventReqPerfOther_Setup( RtemsEventValPerf_Context *ctx )
{
  SetPriority( ctx->worker_id, PRIO_LOW );
}

static void RtemsEventReqPerfOther_Setup_Wrap( void *arg )
{
  RtemsEventValPerf_Context *ctx;

  ctx = arg;
  RtemsEventReqPerfOther_Setup( ctx );
}

/**
 * @brief Send two events.  Satisfy the event condition.
 */
static void RtemsEventReqPerfOther_Body( RtemsEventValPerf_Context *ctx )
{
  ctx->status = rtems_event_send( ctx->worker_id, EVENT_END | EVENT_OTHER );
}

static void RtemsEventReqPerfOther_Body_Wrap( void *arg )
{
  RtemsEventValPerf_Context *ctx;

  ctx = arg;
  RtemsEventReqPerfOther_Body( ctx );
}

/**
 * @brief Restore the worker priority.  Discard samples interrupted by a clock
 *   tick.
 */
static bool RtemsEventReqPerfOther_Teardown(
  RtemsEventValPerf_Context *ctx,
  T_ticks                   *delta,
  uint32_t                   tic,
  uint32_t                   toc,
  unsigned int               retry
)
{
  T_quiet_rsc_success( ctx->status );

  SetPriority( ctx->worker_id, PRIO_HIGH );

  return tic == toc;
}

static bool RtemsEventReqPerfOther_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  RtemsEventValPerf_Context *ctx;

  ctx = arg;
  return RtemsEventReqPerfOther_Teardown( ctx, delta, tic, toc, retry );
}

/** @} */

#if defined(RTEMS_SMP)
/**
 * @defgroup RtemsEventReqPerfOtherCpu spec:/rtems/event/req/perf-other-cpu
 *
 * @{
 */

/**
 * @brief Move worker to scheduler B.
 */
static void RtemsEventReqPerfOtherCpu_Prepare( RtemsEventValPerf_Context *ctx )
{
  SetScheduler( ctx->worker_id, SCHEDULER_B_ID, PRIO_NORMAL );
}

/**
 * @brief Send two events.  Satisfy the event condition.
 */
static void RtemsEventReqPerfOtherCpu_Body( RtemsEventValPerf_Context *ctx )
{
  ctx->begin = T_tick();
  ctx->status = rtems_event_send( ctx->worker_id, EVENT_END | EVENT_OTHER );
}

static void RtemsEventReqPerfOtherCpu_Body_Wrap( void *arg )
{
  RtemsEventValPerf_Context *ctx;

  ctx = arg;
  RtemsEventReqPerfOtherCpu_Body( ctx );
}

/**
 * @brief Make sure the worker waits for the next event.  Set the measured
 *   runtime. Discard samples interrupted by a clock tick.
 */
static bool RtemsEventReqPerfOtherCpu_Teardown(
  RtemsEventValPerf_Context *ctx,
  T_ticks                   *delta,
  uint32_t                   tic,
  uint32_t                   toc,
  unsigned int               retry
)
{
  T_quiet_rsc_success( ctx->status );

  WaitForNextTask( 1, ctx->worker_id );
  *delta = ctx->end - ctx->begin;

  return tic == toc;
}

static bool RtemsEventReqPerfOtherCpu_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  RtemsEventValPerf_Context *ctx;

  ctx = arg;
  return RtemsEventReqPerfOtherCpu_Teardown( ctx, delta, tic, toc, retry );
}

/**
 * @brief Move worker to scheduler A.
 */
static void RtemsEventReqPerfOtherCpu_Cleanup( RtemsEventValPerf_Context *ctx )
{
  SetScheduler( ctx->worker_id, SCHEDULER_A_ID, PRIO_HIGH );
}

/** @} */
#endif

/**
 * @defgroup RtemsEventReqPerfOtherNotSatisfied \
 *   spec:/rtems/event/req/perf-other-not-satisfied
 *
 * @{
 */

/**
 * @brief Send an event.  Do not satisfy the event condition.
 */
static void RtemsEventReqPerfOtherNotSatisfied_Body(
  RtemsEventValPerf_Context *ctx
)
{
  ctx->status = rtems_event_send( ctx->worker_id, EVENT_OTHER );
}

static void RtemsEventReqPerfOtherNotSatisfied_Body_Wrap( void *arg )
{
  RtemsEventValPerf_Context *ctx;

  ctx = arg;
  RtemsEventReqPerfOtherNotSatisfied_Body( ctx );
}

/**
 * @brief Let the worker wait for the next set of events.  Discard samples
 *   interrupted by a clock tick.
 */
static bool RtemsEventReqPerfOtherNotSatisfied_Teardown(
  RtemsEventValPerf_Context *ctx,
  T_ticks                   *delta,
  uint32_t                   tic,
  uint32_t                   toc,
  unsigned int               retry
)
{
  T_quiet_rsc_success( ctx->status );

  Send( ctx, EVENT_END );

  return tic == toc;
}

static bool RtemsEventReqPerfOtherNotSatisfied_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  RtemsEventValPerf_Context *ctx;

  ctx = arg;
  return RtemsEventReqPerfOtherNotSatisfied_Teardown(
    ctx,
    delta,
    tic,
    toc,
    retry
  );
}

/** @} */

/**
 * @defgroup RtemsEventReqPerfOtherPreempt \
 *   spec:/rtems/event/req/perf-other-preempt
 *
 * @{
 */

/**
 * @brief Send two events.  Satisfy the event condition.
 */
static void RtemsEventReqPerfOtherPreempt_Body(
  RtemsEventValPerf_Context *ctx
)
{
  ctx->begin = T_tick();
  ctx->status = rtems_event_send( ctx->worker_id, EVENT_END | EVENT_OTHER );
}

static void RtemsEventReqPerfOtherPreempt_Body_Wrap( void *arg )
{
  RtemsEventValPerf_Context *ctx;

  ctx = arg;
  RtemsEventReqPerfOtherPreempt_Body( ctx );
}

/**
 * @brief Set the measured runtime.  Discard samples interrupted by a clock
 *   tick.
 */
static bool RtemsEventReqPerfOtherPreempt_Teardown(
  RtemsEventValPerf_Context *ctx,
  T_ticks                   *delta,
  uint32_t                   tic,
  uint32_t                   toc,
  unsigned int               retry
)
{
  T_quiet_rsc_success( ctx->status );

  *delta = ctx->end - ctx->begin;

  return tic == toc;
}

static bool RtemsEventReqPerfOtherPreempt_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  RtemsEventValPerf_Context *ctx;

  ctx = arg;
  return RtemsEventReqPerfOtherPreempt_Teardown( ctx, delta, tic, toc, retry );
}

/** @} */

/**
 * @fn void T_case_body_RtemsEventValPerf( void )
 */
T_TEST_CASE_FIXTURE( RtemsEventValPerf, &RtemsEventValPerf_Fixture )
{
  RtemsEventValPerf_Context *ctx;

  ctx = T_fixture_context();

  ctx->request.name = "RtemsEventReqPerfIsrPreempt";
  ctx->request.setup = NULL;
  ctx->request.body = RtemsEventReqPerfIsrPreempt_Body_Wrap;
  ctx->request.teardown = RtemsEventReqPerfIsrPreempt_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  ctx->request.name = "RtemsEventReqPerfOther";
  ctx->request.setup = RtemsEventReqPerfOther_Setup_Wrap;
  ctx->request.body = RtemsEventReqPerfOther_Body_Wrap;
  ctx->request.teardown = RtemsEventReqPerfOther_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  #if defined(RTEMS_SMP)
  RtemsEventReqPerfOtherCpu_Prepare( ctx );
  ctx->request.name = "RtemsEventReqPerfOtherCpu";
  ctx->request.setup = NULL;
  ctx->request.body = RtemsEventReqPerfOtherCpu_Body_Wrap;
  ctx->request.teardown = RtemsEventReqPerfOtherCpu_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );
  RtemsEventReqPerfOtherCpu_Cleanup( ctx );
  #endif

  ctx->request.name = "RtemsEventReqPerfOtherNotSatisfied";
  ctx->request.setup = NULL;
  ctx->request.body = RtemsEventReqPerfOtherNotSatisfied_Body_Wrap;
  ctx->request.teardown = RtemsEventReqPerfOtherNotSatisfied_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  ctx->request.name = "RtemsEventReqPerfOtherPreempt";
  ctx->request.setup = NULL;
  ctx->request.body = RtemsEventReqPerfOtherPreempt_Body_Wrap;
  ctx->request.teardown = RtemsEventReqPerfOtherPreempt_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );
}

/** @} */
