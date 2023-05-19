/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsMessageValPerf
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
 * @defgroup RtemsMessageValPerf spec:/rtems/message/val/perf
 *
 * @ingroup TestsuitesPerformanceNoClock0
 *
 * @brief This test case provides a context to run @ref RTEMSAPIClassicMessage
 *   performance tests.
 *
 * @{
 */

/**
 * @brief Test context for spec:/rtems/message/val/perf test case.
 */
typedef struct {
  /**
   * @brief This member provides a message queue identifier.
   */
  rtems_id queue_id;

  /**
   * @brief This member provides a message to send.
   */
  long message;

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
} RtemsMessageValPerf_Context;

static RtemsMessageValPerf_Context
  RtemsMessageValPerf_Instance;

#define MAXIMUM_PENDING_MESSAGES 1

#define MAXIMUM_MESSAGE_SIZE 8

#define EVENT_END RTEMS_EVENT_0

#define EVENT_SEND RTEMS_EVENT_1

#define EVENT_SEND_END RTEMS_EVENT_2

#define EVENT_RECEIVE RTEMS_EVENT_3

#define EVENT_RECEIVE_END RTEMS_EVENT_4

typedef RtemsMessageValPerf_Context Context;

static RTEMS_MESSAGE_QUEUE_BUFFER( MAXIMUM_MESSAGE_SIZE )
  storage_area[ MAXIMUM_PENDING_MESSAGES ];

rtems_message_queue_config config = {
  .name = OBJECT_NAME,
  .maximum_pending_messages = MAXIMUM_PENDING_MESSAGES,
  .maximum_message_size = MAXIMUM_MESSAGE_SIZE,
  .storage_area = storage_area,
  .storage_size = sizeof( storage_area )
};

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

    if ( ( events & EVENT_SEND ) != 0 ) {
      sc = rtems_message_queue_send(
        ctx->queue_id,
        &ctx->message,
        sizeof( ctx->message )
      );
      ticks = T_tick();
      T_quiet_rsc_success( sc );

      if ( ( events & EVENT_SEND_END ) != 0 ) {
        ctx->end = ticks;
      }
    }

    if ( ( events & EVENT_RECEIVE ) != 0 ) {
      long   message;
      size_t size;

      sc = rtems_message_queue_receive(
        ctx->queue_id,
        &message,
        &size,
        RTEMS_WAIT,
        RTEMS_NO_TIMEOUT
      );
      ticks = T_tick();
      T_quiet_rsc_success( sc );

      if ( ( events & EVENT_RECEIVE_END ) != 0 ) {
        ctx->end = ticks;
      }
    }
  }
}

static void RtemsMessageValPerf_Setup_Context(
  RtemsMessageValPerf_Context *ctx
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

/**
 * @brief Create a message queue and a worker task.
 */
static void RtemsMessageValPerf_Setup( RtemsMessageValPerf_Context *ctx )
{
  rtems_status_code sc;

  SetSelfPriority( PRIO_NORMAL );

  sc = rtems_message_queue_construct( &config, &ctx->queue_id );
  T_rsc_success( sc );

  ctx->worker_id = CreateTask( "WORK", PRIO_HIGH );
  StartTask( ctx->worker_id, Worker, ctx );
}

static void RtemsMessageValPerf_Setup_Wrap( void *arg )
{
  RtemsMessageValPerf_Context *ctx;

  ctx = arg;
  RtemsMessageValPerf_Setup_Context( ctx );
  RtemsMessageValPerf_Setup( ctx );
}

/**
 * @brief Delete the worker task and the message queue.
 */
static void RtemsMessageValPerf_Teardown( RtemsMessageValPerf_Context *ctx )
{
  rtems_status_code sc;

  DeleteTask( ctx->worker_id );

  sc = rtems_message_queue_delete( ctx->queue_id );
  T_rsc_success( sc );

  RestoreRunnerPriority();
}

static void RtemsMessageValPerf_Teardown_Wrap( void *arg )
{
  RtemsMessageValPerf_Context *ctx;

  ctx = arg;
  RtemsMessageValPerf_Teardown( ctx );
}

static T_fixture RtemsMessageValPerf_Fixture = {
  .setup = RtemsMessageValPerf_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsMessageValPerf_Teardown_Wrap,
  .scope = NULL,
  .initial_context = &RtemsMessageValPerf_Instance
};

/**
 * @defgroup RtemsMessageReqPerfReceiveTry \
 *   spec:/rtems/message/req/perf-receive-try
 *
 * @{
 */

/**
 * @brief Try to receive a message.
 */
static void RtemsMessageReqPerfReceiveTry_Body(
  RtemsMessageValPerf_Context *ctx
)
{
  uint64_t message;
  size_t   size;

  ctx->status = rtems_message_queue_receive(
    ctx->queue_id,
    &message,
    &size,
    RTEMS_NO_WAIT,
    0
  );
}

static void RtemsMessageReqPerfReceiveTry_Body_Wrap( void *arg )
{
  RtemsMessageValPerf_Context *ctx;

  ctx = arg;
  RtemsMessageReqPerfReceiveTry_Body( ctx );
}

/**
 * @brief Discard samples interrupted by a clock tick.
 */
static bool RtemsMessageReqPerfReceiveTry_Teardown(
  RtemsMessageValPerf_Context *ctx,
  T_ticks                     *delta,
  uint32_t                     tic,
  uint32_t                     toc,
  unsigned int                 retry
)
{
  T_quiet_rsc( ctx->status, RTEMS_UNSATISFIED );

  return tic == toc;
}

static bool RtemsMessageReqPerfReceiveTry_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  RtemsMessageValPerf_Context *ctx;

  ctx = arg;
  return RtemsMessageReqPerfReceiveTry_Teardown( ctx, delta, tic, toc, retry );
}

/** @} */

/**
 * @defgroup RtemsMessageReqPerfReceiveWaitForever \
 *   spec:/rtems/message/req/perf-receive-wait-forever
 *
 * @{
 */

/**
 * @brief Schedule a message send.
 */
static void RtemsMessageReqPerfReceiveWaitForever_Setup(
  RtemsMessageValPerf_Context *ctx
)
{
  SetPriority( ctx->worker_id, PRIO_LOW );
  Send( ctx, EVENT_END | EVENT_SEND );
}

static void RtemsMessageReqPerfReceiveWaitForever_Setup_Wrap( void *arg )
{
  RtemsMessageValPerf_Context *ctx;

  ctx = arg;
  RtemsMessageReqPerfReceiveWaitForever_Setup( ctx );
}

/**
 * @brief Receive a message.  Wait forever.
 */
static void RtemsMessageReqPerfReceiveWaitForever_Body(
  RtemsMessageValPerf_Context *ctx
)
{
  uint64_t message;
  size_t   size;

  ctx->begin = T_tick();
  ctx->status = rtems_message_queue_receive(
    ctx->queue_id,
    &message,
    &size,
    RTEMS_WAIT,
    RTEMS_NO_TIMEOUT
  );
}

static void RtemsMessageReqPerfReceiveWaitForever_Body_Wrap( void *arg )
{
  RtemsMessageValPerf_Context *ctx;

  ctx = arg;
  RtemsMessageReqPerfReceiveWaitForever_Body( ctx );
}

/**
 * @brief Set the measured runtime.  Restore the worker priority.  Discard
 *   samples interrupted by a clock tick.
 */
static bool RtemsMessageReqPerfReceiveWaitForever_Teardown(
  RtemsMessageValPerf_Context *ctx,
  T_ticks                     *delta,
  uint32_t                     tic,
  uint32_t                     toc,
  unsigned int                 retry
)
{
  T_quiet_rsc_success( ctx->status );

  *delta = ctx->end - ctx->begin;
  SetPriority( ctx->worker_id, PRIO_HIGH );

  return tic == toc;
}

static bool RtemsMessageReqPerfReceiveWaitForever_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  RtemsMessageValPerf_Context *ctx;

  ctx = arg;
  return RtemsMessageReqPerfReceiveWaitForever_Teardown(
    ctx,
    delta,
    tic,
    toc,
    retry
  );
}

/** @} */

/**
 * @defgroup RtemsMessageReqPerfReceiveWaitTimed \
 *   spec:/rtems/message/req/perf-receive-wait-timed
 *
 * @{
 */

/**
 * @brief Schedule a message send.
 */
static void RtemsMessageReqPerfReceiveWaitTimed_Setup(
  RtemsMessageValPerf_Context *ctx
)
{
  SetPriority( ctx->worker_id, PRIO_LOW );
  Send( ctx, EVENT_END | EVENT_SEND );
}

static void RtemsMessageReqPerfReceiveWaitTimed_Setup_Wrap( void *arg )
{
  RtemsMessageValPerf_Context *ctx;

  ctx = arg;
  RtemsMessageReqPerfReceiveWaitTimed_Setup( ctx );
}

/**
 * @brief Receive a message.  Wait with a timeout.
 */
static void RtemsMessageReqPerfReceiveWaitTimed_Body(
  RtemsMessageValPerf_Context *ctx
)
{
  uint64_t message;
  size_t   size;

  ctx->begin = T_tick();
  ctx->status = rtems_message_queue_receive(
    ctx->queue_id,
    &message,
    &size,
    RTEMS_WAIT,
    UINT32_MAX
  );
}

static void RtemsMessageReqPerfReceiveWaitTimed_Body_Wrap( void *arg )
{
  RtemsMessageValPerf_Context *ctx;

  ctx = arg;
  RtemsMessageReqPerfReceiveWaitTimed_Body( ctx );
}

/**
 * @brief Set the measured runtime.  Restore the worker priority.  Discard
 *   samples interrupted by a clock tick.
 */
static bool RtemsMessageReqPerfReceiveWaitTimed_Teardown(
  RtemsMessageValPerf_Context *ctx,
  T_ticks                     *delta,
  uint32_t                     tic,
  uint32_t                     toc,
  unsigned int                 retry
)
{
  T_quiet_rsc_success( ctx->status );

  *delta = ctx->end - ctx->begin;
  SetPriority( ctx->worker_id, PRIO_HIGH );

  return tic == toc;
}

static bool RtemsMessageReqPerfReceiveWaitTimed_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  RtemsMessageValPerf_Context *ctx;

  ctx = arg;
  return RtemsMessageReqPerfReceiveWaitTimed_Teardown(
    ctx,
    delta,
    tic,
    toc,
    retry
  );
}

/** @} */

/**
 * @defgroup RtemsMessageReqPerfSend spec:/rtems/message/req/perf-send
 *
 * @{
 */

/**
 * @brief Send a message.
 */
static void RtemsMessageReqPerfSend_Body( RtemsMessageValPerf_Context *ctx )
{
  ctx->status = rtems_message_queue_send(
    ctx->queue_id,
    &ctx->message,
    sizeof( ctx->message )
  );
}

static void RtemsMessageReqPerfSend_Body_Wrap( void *arg )
{
  RtemsMessageValPerf_Context *ctx;

  ctx = arg;
  RtemsMessageReqPerfSend_Body( ctx );
}

/**
 * @brief Flush the message queue.  Discard samples interrupted by a clock
 *   tick.
 */
static bool RtemsMessageReqPerfSend_Teardown(
  RtemsMessageValPerf_Context *ctx,
  T_ticks                     *delta,
  uint32_t                     tic,
  uint32_t                     toc,
  unsigned int                 retry
)
{
  rtems_status_code sc;
  uint32_t          count;

  T_quiet_rsc_success( ctx->status );

  sc = rtems_message_queue_flush( ctx->queue_id, &count );
  T_quiet_rsc_success( sc );
  T_quiet_eq_u32( count, 1 );

  return tic == toc;
}

static bool RtemsMessageReqPerfSend_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  RtemsMessageValPerf_Context *ctx;

  ctx = arg;
  return RtemsMessageReqPerfSend_Teardown( ctx, delta, tic, toc, retry );
}

/** @} */

/**
 * @defgroup RtemsMessageReqPerfSendOther \
 *   spec:/rtems/message/req/perf-send-other
 *
 * @{
 */

/**
 * @brief Let the worker wait on the message queue.
 */
static void RtemsMessageReqPerfSendOther_Setup(
  RtemsMessageValPerf_Context *ctx
)
{
  Send( ctx, EVENT_RECEIVE );
  SetPriority( ctx->worker_id, PRIO_LOW );
}

static void RtemsMessageReqPerfSendOther_Setup_Wrap( void *arg )
{
  RtemsMessageValPerf_Context *ctx;

  ctx = arg;
  RtemsMessageReqPerfSendOther_Setup( ctx );
}

/**
 * @brief Send a message.
 */
static void RtemsMessageReqPerfSendOther_Body(
  RtemsMessageValPerf_Context *ctx
)
{
  ctx->status = rtems_message_queue_send(
    ctx->queue_id,
    &ctx->message,
    sizeof( ctx->message )
  );
}

static void RtemsMessageReqPerfSendOther_Body_Wrap( void *arg )
{
  RtemsMessageValPerf_Context *ctx;

  ctx = arg;
  RtemsMessageReqPerfSendOther_Body( ctx );
}

/**
 * @brief Restore the worker priority.  Discard samples interrupted by a clock
 *   tick.
 */
static bool RtemsMessageReqPerfSendOther_Teardown(
  RtemsMessageValPerf_Context *ctx,
  T_ticks                     *delta,
  uint32_t                     tic,
  uint32_t                     toc,
  unsigned int                 retry
)
{
  T_quiet_rsc_success( ctx->status );

  SetPriority( ctx->worker_id, PRIO_HIGH );

  return tic == toc;
}

static bool RtemsMessageReqPerfSendOther_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  RtemsMessageValPerf_Context *ctx;

  ctx = arg;
  return RtemsMessageReqPerfSendOther_Teardown( ctx, delta, tic, toc, retry );
}

/** @} */

#if defined(RTEMS_SMP)
/**
 * @defgroup RtemsMessageReqPerfSendOtherCpu \
 *   spec:/rtems/message/req/perf-send-other-cpu
 *
 * @{
 */

/**
 * @brief Move worker to scheduler B.
 */
static void RtemsMessageReqPerfSendOtherCpu_Prepare(
  RtemsMessageValPerf_Context *ctx
)
{
  SetScheduler( ctx->worker_id, SCHEDULER_B_ID, PRIO_NORMAL );
}

/**
 * @brief Let the worker wait on the message queue.
 */
static void RtemsMessageReqPerfSendOtherCpu_Setup(
  RtemsMessageValPerf_Context *ctx
)
{
  Send( ctx, EVENT_RECEIVE | EVENT_RECEIVE_END );
  WaitForNextTask( 1, ctx->worker_id );
}

static void RtemsMessageReqPerfSendOtherCpu_Setup_Wrap( void *arg )
{
  RtemsMessageValPerf_Context *ctx;

  ctx = arg;
  RtemsMessageReqPerfSendOtherCpu_Setup( ctx );
}

/**
 * @brief Send a message.
 */
static void RtemsMessageReqPerfSendOtherCpu_Body(
  RtemsMessageValPerf_Context *ctx
)
{
  ctx->begin = T_tick();
  ctx->status = rtems_message_queue_send(
    ctx->queue_id,
    &ctx->message,
    sizeof( ctx->message )
  );
}

static void RtemsMessageReqPerfSendOtherCpu_Body_Wrap( void *arg )
{
  RtemsMessageValPerf_Context *ctx;

  ctx = arg;
  RtemsMessageReqPerfSendOtherCpu_Body( ctx );
}

/**
 * @brief Make sure the worker waits for the next event.  Set the measured
 *   runtime. Discard samples interrupted by a clock tick.
 */
static bool RtemsMessageReqPerfSendOtherCpu_Teardown(
  RtemsMessageValPerf_Context *ctx,
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

static bool RtemsMessageReqPerfSendOtherCpu_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  RtemsMessageValPerf_Context *ctx;

  ctx = arg;
  return RtemsMessageReqPerfSendOtherCpu_Teardown(
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
static void RtemsMessageReqPerfSendOtherCpu_Cleanup(
  RtemsMessageValPerf_Context *ctx
)
{
  SetScheduler( ctx->worker_id, SCHEDULER_A_ID, PRIO_HIGH );
}

/** @} */
#endif

/**
 * @defgroup RtemsMessageReqPerfSendPreempt \
 *   spec:/rtems/message/req/perf-send-preempt
 *
 * @{
 */

/**
 * @brief Let the worker wait on the message queue.
 */
static void RtemsMessageReqPerfSendPreempt_Setup(
  RtemsMessageValPerf_Context *ctx
)
{
  Send( ctx, EVENT_RECEIVE | EVENT_RECEIVE_END );
}

static void RtemsMessageReqPerfSendPreempt_Setup_Wrap( void *arg )
{
  RtemsMessageValPerf_Context *ctx;

  ctx = arg;
  RtemsMessageReqPerfSendPreempt_Setup( ctx );
}

/**
 * @brief Send a message.
 */
static void RtemsMessageReqPerfSendPreempt_Body(
  RtemsMessageValPerf_Context *ctx
)
{
  ctx->begin = T_tick();
  ctx->status = rtems_message_queue_send(
    ctx->queue_id,
    &ctx->message,
    sizeof( ctx->message )
  );
}

static void RtemsMessageReqPerfSendPreempt_Body_Wrap( void *arg )
{
  RtemsMessageValPerf_Context *ctx;

  ctx = arg;
  RtemsMessageReqPerfSendPreempt_Body( ctx );
}

/**
 * @brief Set the measured runtime.  Discard samples interrupted by a clock
 *   tick.
 */
static bool RtemsMessageReqPerfSendPreempt_Teardown(
  RtemsMessageValPerf_Context *ctx,
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

static bool RtemsMessageReqPerfSendPreempt_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  RtemsMessageValPerf_Context *ctx;

  ctx = arg;
  return RtemsMessageReqPerfSendPreempt_Teardown(
    ctx,
    delta,
    tic,
    toc,
    retry
  );
}

/** @} */

/**
 * @fn void T_case_body_RtemsMessageValPerf( void )
 */
T_TEST_CASE_FIXTURE( RtemsMessageValPerf, &RtemsMessageValPerf_Fixture )
{
  RtemsMessageValPerf_Context *ctx;

  ctx = T_fixture_context();

  ctx->request.name = "RtemsMessageReqPerfReceiveTry";
  ctx->request.setup = NULL;
  ctx->request.body = RtemsMessageReqPerfReceiveTry_Body_Wrap;
  ctx->request.teardown = RtemsMessageReqPerfReceiveTry_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  ctx->request.name = "RtemsMessageReqPerfReceiveWaitForever";
  ctx->request.setup = RtemsMessageReqPerfReceiveWaitForever_Setup_Wrap;
  ctx->request.body = RtemsMessageReqPerfReceiveWaitForever_Body_Wrap;
  ctx->request.teardown = RtemsMessageReqPerfReceiveWaitForever_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  ctx->request.name = "RtemsMessageReqPerfReceiveWaitTimed";
  ctx->request.setup = RtemsMessageReqPerfReceiveWaitTimed_Setup_Wrap;
  ctx->request.body = RtemsMessageReqPerfReceiveWaitTimed_Body_Wrap;
  ctx->request.teardown = RtemsMessageReqPerfReceiveWaitTimed_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  ctx->request.name = "RtemsMessageReqPerfSend";
  ctx->request.setup = NULL;
  ctx->request.body = RtemsMessageReqPerfSend_Body_Wrap;
  ctx->request.teardown = RtemsMessageReqPerfSend_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  ctx->request.name = "RtemsMessageReqPerfSendOther";
  ctx->request.setup = RtemsMessageReqPerfSendOther_Setup_Wrap;
  ctx->request.body = RtemsMessageReqPerfSendOther_Body_Wrap;
  ctx->request.teardown = RtemsMessageReqPerfSendOther_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  #if defined(RTEMS_SMP)
  RtemsMessageReqPerfSendOtherCpu_Prepare( ctx );
  ctx->request.name = "RtemsMessageReqPerfSendOtherCpu";
  ctx->request.setup = RtemsMessageReqPerfSendOtherCpu_Setup_Wrap;
  ctx->request.body = RtemsMessageReqPerfSendOtherCpu_Body_Wrap;
  ctx->request.teardown = RtemsMessageReqPerfSendOtherCpu_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );
  RtemsMessageReqPerfSendOtherCpu_Cleanup( ctx );
  #endif

  ctx->request.name = "RtemsMessageReqPerfSendPreempt";
  ctx->request.setup = RtemsMessageReqPerfSendPreempt_Setup_Wrap;
  ctx->request.body = RtemsMessageReqPerfSendPreempt_Body_Wrap;
  ctx->request.teardown = RtemsMessageReqPerfSendPreempt_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );
}

/** @} */
