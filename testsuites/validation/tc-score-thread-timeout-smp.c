/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreThreadValTimeoutSmp
 */

/*
 * Copyright (C) 2026 embedded brains GmbH & Co. KG
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
#include <sys/time.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/watchdogimpl.h>

#include "ts-config.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreThreadValTimeoutSmp spec:/score/thread/val/timeout-smp
 *
 * @ingroup TestsuitesValidationSmpOnly0
 *
 * @brief Tests that a timeout of a wait which ended ends no other wait of the
 *   thread.
 *
 * This test case performs the following actions:
 *
 * - End the wait of a worker on the second processor while the timeout of that
 *   wait is in flight. Let the worker begin a second wait before the timeout
 *   carries on.
 *
 *   - Check that the timeout reached the window of the test.
 *
 *   - Check that no wait of the window reached its bound.
 *
 *   - Check that the event and not the timeout ended the first wait of the
 *     worker.
 *
 *   - Check that the timeout of the wait which ended does not end the second
 *     wait of the worker.
 *
 * @{
 */

/**
 * @brief Test context for spec:/score/thread/val/timeout-smp test case.
 */
typedef struct {
  /**
   * @brief This member contains the identifier of the worker task.
   */
  rtems_id worker_id;

  /**
   * @brief This member references the thread control block of the worker task.
   */
  Thread_Control *worker_tcb;

  /**
   * @brief This member contains the status of the first wait of the worker.
   */
  rtems_status_code first_wait_status;

  /**
   * @brief If this member is true, then the timeout of the worker gets the
   *   treatment of the test.
   */
  Atomic_Uint arm;

  /**
   * @brief If this member is true, then the timeout of the worker reached the
   *   window of the test.
   */
  Atomic_Uint window_reached;

  /**
   * @brief If this member is true, then a wait of the window reached its
   *   bound.
   */
  Atomic_Uint window_timed_out;

  /**
   * @brief If this member is true, then the first wait of the worker ended.
   */
  Atomic_Uint first_wait_ended;
} ScoreThreadValTimeoutSmp_Context;

static ScoreThreadValTimeoutSmp_Context ScoreThreadValTimeoutSmp_Instance;

typedef ScoreThreadValTimeoutSmp_Context Context;

static Context *timeout_ctx;

/*
 * The timeout in clock ticks of the wait which the tickle ends.  No clock
 * tick of the driver reaches this time point within the case, so the
 * FinalClockTick() of the runner is the only party which expires the timer.
 */
#define WAIT_TIMEOUT 1000

static bool WaitsForEvent( const Context *ctx )
{
  Thread_Wait_flags flags;

  flags = _Thread_Wait_flags_get( ctx->worker_tcb );

  return ( flags & THREAD_WAIT_CLASS_EVENT ) != 0 &&
         ( flags & THREAD_WAIT_STATE_BLOCKED ) != 0;
}

void __real__Thread_Timeout(
  Watchdog_Control *the_watchdog,
  unsigned int      token
);

void __wrap__Thread_Timeout(
  Watchdog_Control *the_watchdog,
  unsigned int      token
);

/*
 * The window of the defect.  The tickle took the timer of the worker out of
 * the collection and calls this routine outside the lock of the collection.
 * The worker runs on the second processor, so it ends its first wait and
 * begins a second one while the routine waits here.
 */
void __wrap__Thread_Timeout(
  Watchdog_Control *the_watchdog,
  unsigned int      token
)
{
  Context *ctx;

  ctx = timeout_ctx;

  if (
    ctx != NULL && GetFlag( &ctx->arm ) != 0 &&
    the_watchdog == &ctx->worker_tcb->Timer.Watchdog
  ) {
    int64_t begin;

    SetFlag( &ctx->arm, 0 );
    SetFlag( &ctx->window_reached, 1 );

    SendEvents( ctx->worker_id, RTEMS_EVENT_0 );
    begin = rtems_clock_get_monotonic_sbintime();

    while (
      GetFlag( &ctx->first_wait_ended ) == 0 && !WaitTimedOut( begin )
    ) {
      /* Wait until the event ended the first wait of the worker. */
    }

    while ( !WaitsForEvent( ctx ) && !WaitTimedOut( begin ) ) {
      /* Wait until the worker began its second wait. */
    }

    SetFlag( &ctx->window_timed_out, WaitTimedOut( begin ) );
  }

  __real__Thread_Timeout( the_watchdog, token );
}

static void Worker( rtems_task_argument arg )
{
  Context        *ctx;
  rtems_event_set events;

  ctx = (Context *) arg;

  /* The runner asks for a timer on its own processor. */
  (void) ReceiveAnyEvents();

  /*
   * The receive arms the timeout of this wait.  The routine of the timeout
   * sends the event which ends the wait.
   */
  ctx->first_wait_status = rtems_event_receive(
    RTEMS_EVENT_0,
    RTEMS_EVENT_ANY | RTEMS_WAIT,
    WAIT_TIMEOUT,
    &events
  );
  SetFlag( &ctx->first_wait_ended, 1 );

  /* The timeout of the wait which ended shall not end this wait. */
  (void) ReceiveAnyEvents();

  (void) rtems_task_suspend( RTEMS_SELF );
}

/*
 * The worker arms the timer where it runs, so it gets the processor of the
 * runner for the arm and the other processor for the window.  The runner
 * and the worker carry the same priority, so the yield hands the processor
 * over until the timer of the worker is on the ticks collection.
 */
static void ArmOnFirstProcessor( Context *ctx )
{
  TaskTimerInfo info;

  SetFlag( &ctx->first_wait_ended, 0 );
  ctx->first_wait_status = RTEMS_INTERNAL_ERROR;
  SetAffinityOne( ctx->worker_id, 0 );
  SendEvents( ctx->worker_id, RTEMS_EVENT_1 );

  do {
    Yield();
    GetTaskTimerInfo( ctx->worker_id, &info );
  } while ( info.state != TASK_TIMER_TICKS );

  SetAffinityOne( ctx->worker_id, 1 );
}

static void ScoreThreadValTimeoutSmp_Setup(
  ScoreThreadValTimeoutSmp_Context *ctx
)
{
  timeout_ctx = ctx;
  SetFlag( &ctx->arm, 0 );

  /*
   * The second scheduler gives its processor up, so the worker arms the
   * timeout on the processor of the runner.  The timer stays there while
   * the worker runs on the second processor later on.
   */
  RemoveProcessor( SCHEDULER_B_ID, 1 );

  ctx->worker_id = CreateTask( "WORK", GetSelfPriority() );
  ctx->worker_tcb = GetThread( ctx->worker_id );
  StartTask( ctx->worker_id, Worker, ctx );
  Yield();

  AddProcessor( SCHEDULER_A_ID, 1 );

  /*
   * The affinity of a task holds the processors of its scheduler at the
   * time of its create, so both tasks get a set of their own here.
   */
  SetSelfAffinityOne( 0 );
}

static void ScoreThreadValTimeoutSmp_Setup_Wrap( void *arg )
{
  ScoreThreadValTimeoutSmp_Context *ctx;

  ctx = arg;
  ScoreThreadValTimeoutSmp_Setup( ctx );
}

static void ScoreThreadValTimeoutSmp_Teardown(
  ScoreThreadValTimeoutSmp_Context *ctx
)
{
  DeleteTask( ctx->worker_id );

  SetSelfAffinityAll();
  RemoveProcessor( SCHEDULER_A_ID, 1 );
  AddProcessor( SCHEDULER_B_ID, 1 );

  timeout_ctx = NULL;
}

static void ScoreThreadValTimeoutSmp_Teardown_Wrap( void *arg )
{
  ScoreThreadValTimeoutSmp_Context *ctx;

  ctx = arg;
  ScoreThreadValTimeoutSmp_Teardown( ctx );
}

static T_fixture ScoreThreadValTimeoutSmp_Fixture = {
  .setup = ScoreThreadValTimeoutSmp_Setup_Wrap,
  .stop = NULL,
  .teardown = ScoreThreadValTimeoutSmp_Teardown_Wrap,
  .scope = NULL,
  .initial_context = &ScoreThreadValTimeoutSmp_Instance
};

/**
 * @brief End the wait of a worker on the second processor while the timeout of
 *   that wait is in flight. Let the worker begin a second wait before the
 *   timeout carries on.
 */
static void ScoreThreadValTimeoutSmp_Action_0(
  ScoreThreadValTimeoutSmp_Context *ctx
)
{
  ArmOnFirstProcessor( ctx );
  SetFlag( &ctx->window_reached, 0 );
  SetFlag( &ctx->window_timed_out, 0 );
  SetFlag( &ctx->arm, 1 );

  FinalClockTick();

  /*
   * Check that the timeout reached the window of the test.
   */
  T_true( GetFlag( &ctx->window_reached ) != 0 );

  /*
   * Check that no wait of the window reached its bound.
   */
  T_false( GetFlag( &ctx->window_timed_out ) != 0 );

  /*
   * Check that the event and not the timeout ended the first wait of the
   * worker.
   */
  T_rsc_success( ctx->first_wait_status );

  /*
   * Check that the timeout of the wait which ended does not end the second
   * wait of the worker.
   */
  T_true( WaitsForEvent( ctx ) );
}

/**
 * @fn void T_case_body_ScoreThreadValTimeoutSmp( void )
 */
T_TEST_CASE_FIXTURE(
  ScoreThreadValTimeoutSmp,
  &ScoreThreadValTimeoutSmp_Fixture
)
{
  ScoreThreadValTimeoutSmp_Context *ctx;

  ctx = T_fixture_context();

  ScoreThreadValTimeoutSmp_Action_0( ctx );
}

/** @} */
