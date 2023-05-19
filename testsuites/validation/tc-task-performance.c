/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsTaskValPerf
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

#include "ts-config.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsTaskValPerf spec:/rtems/task/val/perf
 *
 * @ingroup TestsuitesPerformanceNoClock0
 *
 * @brief This test case provides a context to run @ref RTEMSAPIClassicTasks
 *   performance tests.
 *
 * @{
 */

/**
 * @brief Test context for spec:/rtems/task/val/perf test case.
 */
typedef struct {
  /**
   * @brief This member provides a worker identifier.
   */
  rtems_id worker_id;

  /**
   * @brief This member provides a second worker identifier.
   */
  rtems_id worker_2_id;

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
} RtemsTaskValPerf_Context;

static RtemsTaskValPerf_Context
  RtemsTaskValPerf_Instance;

#define EVENT_RESTART RTEMS_EVENT_0

#define EVENT_SET_END RTEMS_EVENT_1

#define EVENT_BUSY RTEMS_EVENT_2

typedef RtemsTaskValPerf_Context Context;

RTEMS_ALIGNED( RTEMS_TASK_STORAGE_ALIGNMENT ) static char task_storage[
  RTEMS_TASK_STORAGE_SIZE(
    TEST_MAXIMUM_TLS_SIZE + TEST_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_ATTRIBUTES
  )
];

static const rtems_task_config config = {
  .name = OBJECT_NAME,
  .initial_priority = PRIO_NORMAL,
  .storage_area = task_storage,
  .storage_size = sizeof( task_storage ),
  .maximum_thread_local_storage_size = 0,
  .initial_modes = RTEMS_DEFAULT_MODES,
  .attributes = RTEMS_DEFAULT_ATTRIBUTES
};

static void Send( const Context *ctx, rtems_event_set events )
{
  SendEvents( ctx->worker_id, events );
}

static void Worker( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;
  ctx->end = T_tick();

  while ( true ) {
    rtems_event_set events;
    T_ticks         ticks;

    events = ReceiveAnyEvents();
    ticks = T_tick();

    if ( ( events & EVENT_RESTART ) != 0 ) {
      ctx->begin = T_tick();
      (void) rtems_task_restart( RTEMS_SELF, (rtems_task_argument) ctx );
    }

    if ( ( events & EVENT_SET_END ) != 0 ) {
      ctx->end = ticks;
    }

    if ( ( events & EVENT_BUSY ) != 0 ) {
      (void) _CPU_Thread_Idle_body( 0 );
    }
  }
}

static void RtemsTaskValPerf_Setup_Context( RtemsTaskValPerf_Context *ctx )
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
 * @brief Set the runner priority.
 */
static void RtemsTaskValPerf_Setup( RtemsTaskValPerf_Context *ctx )
{
  SetSelfPriority( PRIO_NORMAL );
}

static void RtemsTaskValPerf_Setup_Wrap( void *arg )
{
  RtemsTaskValPerf_Context *ctx;

  ctx = arg;
  RtemsTaskValPerf_Setup_Context( ctx );
  RtemsTaskValPerf_Setup( ctx );
}

/**
 * @brief Restore the runner priority.
 */
static void RtemsTaskValPerf_Teardown( RtemsTaskValPerf_Context *ctx )
{
  RestoreRunnerPriority();
}

static void RtemsTaskValPerf_Teardown_Wrap( void *arg )
{
  RtemsTaskValPerf_Context *ctx;

  ctx = arg;
  RtemsTaskValPerf_Teardown( ctx );
}

static T_fixture RtemsTaskValPerf_Fixture = {
  .setup = RtemsTaskValPerf_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsTaskValPerf_Teardown_Wrap,
  .scope = NULL,
  .initial_context = &RtemsTaskValPerf_Instance
};

/**
 * @defgroup RtemsTaskReqPerfConstruct spec:/rtems/task/req/perf-construct
 *
 * @{
 */

/**
 * @brief Construct a worker task.
 */
static void RtemsTaskReqPerfConstruct_Body( RtemsTaskValPerf_Context *ctx )
{
  ctx->status = rtems_task_construct( &config, &ctx->worker_id );
}

static void RtemsTaskReqPerfConstruct_Body_Wrap( void *arg )
{
  RtemsTaskValPerf_Context *ctx;

  ctx = arg;
  RtemsTaskReqPerfConstruct_Body( ctx );
}

/**
 * @brief Delete the worker.  Discard samples interrupted by a clock tick.
 */
static bool RtemsTaskReqPerfConstruct_Teardown(
  RtemsTaskValPerf_Context *ctx,
  T_ticks                  *delta,
  uint32_t                  tic,
  uint32_t                  toc,
  unsigned int              retry
)
{
  T_quiet_rsc_success( ctx->status );

  DeleteTask( ctx->worker_id );
  KillZombies();

  return tic == toc;
}

static bool RtemsTaskReqPerfConstruct_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  RtemsTaskValPerf_Context *ctx;

  ctx = arg;
  return RtemsTaskReqPerfConstruct_Teardown( ctx, delta, tic, toc, retry );
}

/** @} */

/**
 * @defgroup RtemsTaskReqPerfRestart spec:/rtems/task/req/perf-restart
 *
 * @{
 */

/**
 * @brief Create and start a worker task.
 */
static void RtemsTaskReqPerfRestart_Prepare( RtemsTaskValPerf_Context *ctx )
{
  ctx->worker_id = CreateTask( "WORK", PRIO_LOW );
  StartTask( ctx->worker_id, Worker, ctx );
}

/**
 * @brief Restart the worker task.
 */
static void RtemsTaskReqPerfRestart_Body( RtemsTaskValPerf_Context *ctx )
{
  ctx->status = rtems_task_restart(
    ctx->worker_id,
    (rtems_task_argument) ctx
  );
}

static void RtemsTaskReqPerfRestart_Body_Wrap( void *arg )
{
  RtemsTaskValPerf_Context *ctx;

  ctx = arg;
  RtemsTaskReqPerfRestart_Body( ctx );
}

/**
 * @brief Discard samples interrupted by a clock tick.
 */
static bool RtemsTaskReqPerfRestart_Teardown(
  RtemsTaskValPerf_Context *ctx,
  T_ticks                  *delta,
  uint32_t                  tic,
  uint32_t                  toc,
  unsigned int              retry
)
{
  T_quiet_rsc_success( ctx->status );

  return tic == toc;
}

static bool RtemsTaskReqPerfRestart_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  RtemsTaskValPerf_Context *ctx;

  ctx = arg;
  return RtemsTaskReqPerfRestart_Teardown( ctx, delta, tic, toc, retry );
}

/**
 * @brief Delete the worker task.
 */
static void RtemsTaskReqPerfRestart_Cleanup( RtemsTaskValPerf_Context *ctx )
{
  DeleteTask( ctx->worker_id );
}

/** @} */

/**
 * @defgroup RtemsTaskReqPerfRestartPreempt \
 *   spec:/rtems/task/req/perf-restart-preempt
 *
 * @{
 */

/**
 * @brief Create and start a worker task.
 */
static void RtemsTaskReqPerfRestartPreempt_Prepare(
  RtemsTaskValPerf_Context *ctx
)
{
  ctx->worker_id = CreateTask( "WORK", PRIO_HIGH );
  StartTask( ctx->worker_id, Worker, ctx );
}

/**
 * @brief Restart the worker task.
 */
static void RtemsTaskReqPerfRestartPreempt_Body(
  RtemsTaskValPerf_Context *ctx
)
{
  ctx->begin = T_tick();
  ctx->status = rtems_task_restart(
    ctx->worker_id,
    (rtems_task_argument) ctx
  );
}

static void RtemsTaskReqPerfRestartPreempt_Body_Wrap( void *arg )
{
  RtemsTaskValPerf_Context *ctx;

  ctx = arg;
  RtemsTaskReqPerfRestartPreempt_Body( ctx );
}

/**
 * @brief Set the measured runtime.  Discard samples interrupted by a clock
 *   tick.
 */
static bool RtemsTaskReqPerfRestartPreempt_Teardown(
  RtemsTaskValPerf_Context *ctx,
  T_ticks                  *delta,
  uint32_t                  tic,
  uint32_t                  toc,
  unsigned int              retry
)
{
  T_quiet_rsc_success( ctx->status );

  *delta = ctx->end - ctx->begin;

  return tic == toc;
}

static bool RtemsTaskReqPerfRestartPreempt_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  RtemsTaskValPerf_Context *ctx;

  ctx = arg;
  return RtemsTaskReqPerfRestartPreempt_Teardown(
    ctx,
    delta,
    tic,
    toc,
    retry
  );
}

/**
 * @brief Delete the worker task.
 */
static void RtemsTaskReqPerfRestartPreempt_Cleanup(
  RtemsTaskValPerf_Context *ctx
)
{
  DeleteTask( ctx->worker_id );
}

/** @} */

/**
 * @defgroup RtemsTaskReqPerfRestartSelf spec:/rtems/task/req/perf-restart-self
 *
 * @{
 */

/**
 * @brief Create and start a worker task.
 */
static void RtemsTaskReqPerfRestartSelf_Prepare(
  RtemsTaskValPerf_Context *ctx
)
{
  ctx->worker_id = CreateTask( "WORK", PRIO_HIGH );
  StartTask( ctx->worker_id, Worker, ctx );
}

/**
 * @brief Restart the worker task.
 */
static void RtemsTaskReqPerfRestartSelf_Body( RtemsTaskValPerf_Context *ctx )
{
  Send( ctx, EVENT_RESTART );
}

static void RtemsTaskReqPerfRestartSelf_Body_Wrap( void *arg )
{
  RtemsTaskValPerf_Context *ctx;

  ctx = arg;
  RtemsTaskReqPerfRestartSelf_Body( ctx );
}

/**
 * @brief Set the measured runtime.  Discard samples interrupted by a clock
 *   tick.
 */
static bool RtemsTaskReqPerfRestartSelf_Teardown(
  RtemsTaskValPerf_Context *ctx,
  T_ticks                  *delta,
  uint32_t                  tic,
  uint32_t                  toc,
  unsigned int              retry
)
{
  T_quiet_rsc_success( ctx->status );

  *delta = ctx->end - ctx->begin;

  return tic == toc;
}

static bool RtemsTaskReqPerfRestartSelf_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  RtemsTaskValPerf_Context *ctx;

  ctx = arg;
  return RtemsTaskReqPerfRestartSelf_Teardown( ctx, delta, tic, toc, retry );
}

/**
 * @brief Delete the worker task.
 */
static void RtemsTaskReqPerfRestartSelf_Cleanup(
  RtemsTaskValPerf_Context *ctx
)
{
  DeleteTask( ctx->worker_id );
}

/** @} */

#if defined(RTEMS_SMP)
/**
 * @defgroup RtemsTaskReqPerfSetSchedulerMove \
 *   spec:/rtems/task/req/perf-set-scheduler-move
 *
 * @{
 */

/**
 * @brief Set the runner affinity.
 */
static void RtemsTaskReqPerfSetSchedulerMove_Prepare(
  RtemsTaskValPerf_Context *ctx
)
{
  SetSelfAffinityAll();
}

/**
 * @brief Set the scheduler of the runner.
 */
static void RtemsTaskReqPerfSetSchedulerMove_Body(
  RtemsTaskValPerf_Context *ctx
)
{
  ctx->status = rtems_task_set_scheduler(
    RTEMS_SELF,
    SCHEDULER_B_ID,
    PRIO_NORMAL
  );
}

static void RtemsTaskReqPerfSetSchedulerMove_Body_Wrap( void *arg )
{
  RtemsTaskValPerf_Context *ctx;

  ctx = arg;
  RtemsTaskReqPerfSetSchedulerMove_Body( ctx );
}

/**
 * @brief Discard samples interrupted by a clock tick.
 */
static bool RtemsTaskReqPerfSetSchedulerMove_Teardown(
  RtemsTaskValPerf_Context *ctx,
  T_ticks                  *delta,
  uint32_t                  tic,
  uint32_t                  toc,
  unsigned int              retry
)
{
  T_quiet_rsc_success( ctx->status );

  SetSelfScheduler( SCHEDULER_A_ID, PRIO_NORMAL );

  return tic == toc;
}

static bool RtemsTaskReqPerfSetSchedulerMove_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  RtemsTaskValPerf_Context *ctx;

  ctx = arg;
  return RtemsTaskReqPerfSetSchedulerMove_Teardown(
    ctx,
    delta,
    tic,
    toc,
    retry
  );
}

/**
 * @brief Restore the runner affinity.
 */
static void RtemsTaskReqPerfSetSchedulerMove_Cleanup(
  RtemsTaskValPerf_Context *ctx
)
{
  SetSelfAffinityOne( 0 );
}

/** @} */
#endif

/**
 * @defgroup RtemsTaskReqPerfSetSchedulerNop \
 *   spec:/rtems/task/req/perf-set-scheduler-nop
 *
 * @{
 */

/**
 * @brief Set the scheduler of the runner.
 */
static void RtemsTaskReqPerfSetSchedulerNop_Body(
  RtemsTaskValPerf_Context *ctx
)
{
  ctx->status = rtems_task_set_scheduler(
    RTEMS_SELF,
    SCHEDULER_A_ID,
    PRIO_NORMAL
  );
}

static void RtemsTaskReqPerfSetSchedulerNop_Body_Wrap( void *arg )
{
  RtemsTaskValPerf_Context *ctx;

  ctx = arg;
  RtemsTaskReqPerfSetSchedulerNop_Body( ctx );
}

/**
 * @brief Discard samples interrupted by a clock tick.
 */
static bool RtemsTaskReqPerfSetSchedulerNop_Teardown(
  RtemsTaskValPerf_Context *ctx,
  T_ticks                  *delta,
  uint32_t                  tic,
  uint32_t                  toc,
  unsigned int              retry
)
{
  T_quiet_rsc_success( ctx->status );

  return tic == toc;
}

static bool RtemsTaskReqPerfSetSchedulerNop_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  RtemsTaskValPerf_Context *ctx;

  ctx = arg;
  return RtemsTaskReqPerfSetSchedulerNop_Teardown(
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
 * @defgroup RtemsTaskReqPerfSetSchedulerOther \
 *   spec:/rtems/task/req/perf-set-scheduler-other
 *
 * @{
 */

/**
 * @brief Create and start a worker task for scheduler B.
 */
static void RtemsTaskReqPerfSetSchedulerOther_Prepare(
  RtemsTaskValPerf_Context *ctx
)
{
  ctx->worker_id = CreateTask( "WORK", PRIO_NORMAL );
  SetScheduler( ctx->worker_id, SCHEDULER_B_ID, PRIO_NORMAL );
  StartTask( ctx->worker_id, Worker, ctx );
}

/**
 * @brief Move the worker to scheduler A.
 */
static void RtemsTaskReqPerfSetSchedulerOther_Body(
  RtemsTaskValPerf_Context *ctx
)
{
  ctx->status = rtems_task_set_scheduler(
    ctx->worker_id,
    SCHEDULER_A_ID,
    PRIO_LOW
  );
}

static void RtemsTaskReqPerfSetSchedulerOther_Body_Wrap( void *arg )
{
  RtemsTaskValPerf_Context *ctx;

  ctx = arg;
  RtemsTaskReqPerfSetSchedulerOther_Body( ctx );
}

/**
 * @brief Move the worker back to scheduler B.  Discard samples interrupted by
 *   a clock tick.
 */
static bool RtemsTaskReqPerfSetSchedulerOther_Teardown(
  RtemsTaskValPerf_Context *ctx,
  T_ticks                  *delta,
  uint32_t                  tic,
  uint32_t                  toc,
  unsigned int              retry
)
{
  T_quiet_rsc_success( ctx->status );

  SetScheduler( ctx->worker_id, SCHEDULER_B_ID, PRIO_NORMAL );

  return tic == toc;
}

static bool RtemsTaskReqPerfSetSchedulerOther_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  RtemsTaskValPerf_Context *ctx;

  ctx = arg;
  return RtemsTaskReqPerfSetSchedulerOther_Teardown(
    ctx,
    delta,
    tic,
    toc,
    retry
  );
}

/**
 * @brief Delete the worker task.
 */
static void RtemsTaskReqPerfSetSchedulerOther_Cleanup(
  RtemsTaskValPerf_Context *ctx
)
{
  DeleteTask( ctx->worker_id );
}

/** @} */
#endif

#if defined(RTEMS_SMP)
/**
 * @defgroup RtemsTaskReqPerfSetSchedulerPreempt \
 *   spec:/rtems/task/req/perf-set-scheduler-preempt
 *
 * @{
 */

/**
 * @brief Create and start two worker tasks for scheduler B.  Make the second
 *   worker busy.
 */
static void RtemsTaskReqPerfSetSchedulerPreempt_Prepare(
  RtemsTaskValPerf_Context *ctx
)
{
  ctx->worker_id = CreateTask( "WORK", PRIO_NORMAL );
  SetScheduler( ctx->worker_id, SCHEDULER_B_ID, PRIO_NORMAL );
  StartTask( ctx->worker_id, Worker, ctx );
  Send( ctx, EVENT_SET_END );
  WaitForNextTask( 1, ctx->worker_id );

  ctx->worker_2_id = CreateTask( "WRK2", PRIO_NORMAL );
  SetScheduler( ctx->worker_2_id, SCHEDULER_B_ID, PRIO_HIGH );
  StartTask( ctx->worker_2_id, Worker, ctx );
  SendEvents( ctx->worker_2_id, EVENT_BUSY );
  SuspendTask( ctx->worker_2_id );
}

/**
 * @brief Move the worker to scheduler B.  Make the worker ready to set the end
 *   time.
 */
static void RtemsTaskReqPerfSetSchedulerPreempt_Setup(
  RtemsTaskValPerf_Context *ctx
)
{
  ResumeTask( ctx->worker_2_id );
  SetScheduler( ctx->worker_id, SCHEDULER_B_ID, PRIO_NORMAL );
  Send( ctx, EVENT_SET_END );
}

static void RtemsTaskReqPerfSetSchedulerPreempt_Setup_Wrap( void *arg )
{
  RtemsTaskValPerf_Context *ctx;

  ctx = arg;
  RtemsTaskReqPerfSetSchedulerPreempt_Setup( ctx );
}

/**
 * @brief Move the worker to scheduler A.
 */
static void RtemsTaskReqPerfSetSchedulerPreempt_Body(
  RtemsTaskValPerf_Context *ctx
)
{
  ctx->begin = T_tick();
  ctx->status = rtems_task_set_scheduler(
    ctx->worker_id,
    SCHEDULER_A_ID,
    PRIO_HIGH
  );
}

static void RtemsTaskReqPerfSetSchedulerPreempt_Body_Wrap( void *arg )
{
  RtemsTaskValPerf_Context *ctx;

  ctx = arg;
  RtemsTaskReqPerfSetSchedulerPreempt_Body( ctx );
}

/**
 * @brief Set the measured runtime.  Discard samples interrupted by a clock
 *   tick.
 */
static bool RtemsTaskReqPerfSetSchedulerPreempt_Teardown(
  RtemsTaskValPerf_Context *ctx,
  T_ticks                  *delta,
  uint32_t                  tic,
  uint32_t                  toc,
  unsigned int              retry
)
{
  T_quiet_rsc_success( ctx->status );

  *delta = ctx->end - ctx->begin;
  SuspendTask( ctx->worker_2_id );

  return tic == toc;
}

static bool RtemsTaskReqPerfSetSchedulerPreempt_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  RtemsTaskValPerf_Context *ctx;

  ctx = arg;
  return RtemsTaskReqPerfSetSchedulerPreempt_Teardown(
    ctx,
    delta,
    tic,
    toc,
    retry
  );
}

/**
 * @brief Delete the worker tasks.
 */
static void RtemsTaskReqPerfSetSchedulerPreempt_Cleanup(
  RtemsTaskValPerf_Context *ctx
)
{
  ResumeTask( ctx->worker_2_id );
  DeleteTask( ctx->worker_2_id );
  DeleteTask( ctx->worker_id );
}

/** @} */
#endif

/**
 * @defgroup RtemsTaskReqPerfStart spec:/rtems/task/req/perf-start
 *
 * @{
 */

/**
 * @brief Create a worker task.
 */
static void RtemsTaskReqPerfStart_Setup( RtemsTaskValPerf_Context *ctx )
{
  ctx->worker_id = CreateTask( "WORK", PRIO_LOW );
}

static void RtemsTaskReqPerfStart_Setup_Wrap( void *arg )
{
  RtemsTaskValPerf_Context *ctx;

  ctx = arg;
  RtemsTaskReqPerfStart_Setup( ctx );
}

/**
 * @brief Start the worker task.
 */
static void RtemsTaskReqPerfStart_Body( RtemsTaskValPerf_Context *ctx )
{
  ctx->status = rtems_task_start(
    ctx->worker_id,
    Worker,
    (rtems_task_argument) ctx
  );
}

static void RtemsTaskReqPerfStart_Body_Wrap( void *arg )
{
  RtemsTaskValPerf_Context *ctx;

  ctx = arg;
  RtemsTaskReqPerfStart_Body( ctx );
}

/**
 * @brief Delete the worker.  Discard samples interrupted by a clock tick.
 */
static bool RtemsTaskReqPerfStart_Teardown(
  RtemsTaskValPerf_Context *ctx,
  T_ticks                  *delta,
  uint32_t                  tic,
  uint32_t                  toc,
  unsigned int              retry
)
{
  T_quiet_rsc_success( ctx->status );

  DeleteTask( ctx->worker_id );

  return tic == toc;
}

static bool RtemsTaskReqPerfStart_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  RtemsTaskValPerf_Context *ctx;

  ctx = arg;
  return RtemsTaskReqPerfStart_Teardown( ctx, delta, tic, toc, retry );
}

/** @} */

/**
 * @defgroup RtemsTaskReqPerfStartPreempt \
 *   spec:/rtems/task/req/perf-start-preempt
 *
 * @{
 */

/**
 * @brief Create a worker task.
 */
static void RtemsTaskReqPerfStartPreempt_Setup( RtemsTaskValPerf_Context *ctx )
{
  ctx->worker_id = CreateTask( "WORK", PRIO_HIGH );
}

static void RtemsTaskReqPerfStartPreempt_Setup_Wrap( void *arg )
{
  RtemsTaskValPerf_Context *ctx;

  ctx = arg;
  RtemsTaskReqPerfStartPreempt_Setup( ctx );
}

/**
 * @brief Start the worker task.
 */
static void RtemsTaskReqPerfStartPreempt_Body( RtemsTaskValPerf_Context *ctx )
{
  ctx->begin = T_tick();
  ctx->status = rtems_task_start(
    ctx->worker_id,
    Worker,
    (rtems_task_argument) ctx
  );
}

static void RtemsTaskReqPerfStartPreempt_Body_Wrap( void *arg )
{
  RtemsTaskValPerf_Context *ctx;

  ctx = arg;
  RtemsTaskReqPerfStartPreempt_Body( ctx );
}

/**
 * @brief Set the measured runtime.  Delete the worker.  Discard samples
 *   interrupted by a clock tick.
 */
static bool RtemsTaskReqPerfStartPreempt_Teardown(
  RtemsTaskValPerf_Context *ctx,
  T_ticks                  *delta,
  uint32_t                  tic,
  uint32_t                  toc,
  unsigned int              retry
)
{
  T_quiet_rsc_success( ctx->status );

  *delta = ctx->end - ctx->begin;
  DeleteTask( ctx->worker_id );

  return tic == toc;
}

static bool RtemsTaskReqPerfStartPreempt_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  RtemsTaskValPerf_Context *ctx;

  ctx = arg;
  return RtemsTaskReqPerfStartPreempt_Teardown( ctx, delta, tic, toc, retry );
}

/** @} */

/**
 * @fn void T_case_body_RtemsTaskValPerf( void )
 */
T_TEST_CASE_FIXTURE( RtemsTaskValPerf, &RtemsTaskValPerf_Fixture )
{
  RtemsTaskValPerf_Context *ctx;

  ctx = T_fixture_context();

  ctx->request.name = "RtemsTaskReqPerfConstruct";
  ctx->request.setup = NULL;
  ctx->request.body = RtemsTaskReqPerfConstruct_Body_Wrap;
  ctx->request.teardown = RtemsTaskReqPerfConstruct_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  RtemsTaskReqPerfRestart_Prepare( ctx );
  ctx->request.name = "RtemsTaskReqPerfRestart";
  ctx->request.setup = NULL;
  ctx->request.body = RtemsTaskReqPerfRestart_Body_Wrap;
  ctx->request.teardown = RtemsTaskReqPerfRestart_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );
  RtemsTaskReqPerfRestart_Cleanup( ctx );

  RtemsTaskReqPerfRestartPreempt_Prepare( ctx );
  ctx->request.name = "RtemsTaskReqPerfRestartPreempt";
  ctx->request.setup = NULL;
  ctx->request.body = RtemsTaskReqPerfRestartPreempt_Body_Wrap;
  ctx->request.teardown = RtemsTaskReqPerfRestartPreempt_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );
  RtemsTaskReqPerfRestartPreempt_Cleanup( ctx );

  RtemsTaskReqPerfRestartSelf_Prepare( ctx );
  ctx->request.name = "RtemsTaskReqPerfRestartSelf";
  ctx->request.setup = NULL;
  ctx->request.body = RtemsTaskReqPerfRestartSelf_Body_Wrap;
  ctx->request.teardown = RtemsTaskReqPerfRestartSelf_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );
  RtemsTaskReqPerfRestartSelf_Cleanup( ctx );

  #if defined(RTEMS_SMP)
  RtemsTaskReqPerfSetSchedulerMove_Prepare( ctx );
  ctx->request.name = "RtemsTaskReqPerfSetSchedulerMove";
  ctx->request.setup = NULL;
  ctx->request.body = RtemsTaskReqPerfSetSchedulerMove_Body_Wrap;
  ctx->request.teardown = RtemsTaskReqPerfSetSchedulerMove_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );
  RtemsTaskReqPerfSetSchedulerMove_Cleanup( ctx );
  #endif

  ctx->request.name = "RtemsTaskReqPerfSetSchedulerNop";
  ctx->request.setup = NULL;
  ctx->request.body = RtemsTaskReqPerfSetSchedulerNop_Body_Wrap;
  ctx->request.teardown = RtemsTaskReqPerfSetSchedulerNop_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  #if defined(RTEMS_SMP)
  RtemsTaskReqPerfSetSchedulerOther_Prepare( ctx );
  ctx->request.name = "RtemsTaskReqPerfSetSchedulerOther";
  ctx->request.setup = NULL;
  ctx->request.body = RtemsTaskReqPerfSetSchedulerOther_Body_Wrap;
  ctx->request.teardown = RtemsTaskReqPerfSetSchedulerOther_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );
  RtemsTaskReqPerfSetSchedulerOther_Cleanup( ctx );
  #endif

  #if defined(RTEMS_SMP)
  RtemsTaskReqPerfSetSchedulerPreempt_Prepare( ctx );
  ctx->request.name = "RtemsTaskReqPerfSetSchedulerPreempt";
  ctx->request.setup = RtemsTaskReqPerfSetSchedulerPreempt_Setup_Wrap;
  ctx->request.body = RtemsTaskReqPerfSetSchedulerPreempt_Body_Wrap;
  ctx->request.teardown = RtemsTaskReqPerfSetSchedulerPreempt_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );
  RtemsTaskReqPerfSetSchedulerPreempt_Cleanup( ctx );
  #endif

  ctx->request.name = "RtemsTaskReqPerfStart";
  ctx->request.setup = RtemsTaskReqPerfStart_Setup_Wrap;
  ctx->request.body = RtemsTaskReqPerfStart_Body_Wrap;
  ctx->request.teardown = RtemsTaskReqPerfStart_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  ctx->request.name = "RtemsTaskReqPerfStartPreempt";
  ctx->request.setup = RtemsTaskReqPerfStartPreempt_Setup_Wrap;
  ctx->request.body = RtemsTaskReqPerfStartPreempt_Body_Wrap;
  ctx->request.teardown = RtemsTaskReqPerfStartPreempt_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );
}

/** @} */
