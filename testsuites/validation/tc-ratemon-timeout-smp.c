/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsRatemonValTimeoutSmp
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
 * @defgroup RtemsRatemonValTimeoutSmp spec:/rtems/ratemon/val/timeout-smp
 *
 * @ingroup TestsuitesValidationSmpOnly0
 *
 * @brief Tests that a timeout of a period which the owner task ended changes
 *   neither the state of the period nor a wait which began after it.
 *
 * This test case performs the following actions:
 *
 * - Let the timeout of a period expire on the first processor while the owner
 *   task cancels the period on the second processor.
 *
 *   - Check that the timeout reached the window of the test.
 *
 *   - Check that no wait of the test reached its bound.
 *
 *   - Check that the timeout of the interval which ended leaves the state of
 *     the period alone.
 *
 * - Let the timeout of a period expire on the first processor while the owner
 *   task begins a new interval on the second processor.
 *
 *   - Check that the timeout reached the window of the test.
 *
 *   - Check that no wait of the test reached its bound.
 *
 *   - Check that the timeout of the interval which ended does not end the wait
 *     of the owner task which began after it.
 *
 * @{
 */

/**
 * @brief Test context for spec:/rtems/ratemon/val/timeout-smp test case.
 */
typedef struct {
  /**
   * @brief This member contains the identifier of the owner task.
   */
  rtems_id owner_id;

  /**
   * @brief This member references the thread control block of the owner task.
   */
  Thread_Control *owner_tcb;

  /**
   * @brief This member contains the identifier of the period.
   */
  rtems_id period_id;

  /**
   * @brief This member contains the work of the owner task in the window of
   *   the test.
   */
  Atomic_Uint mode;

  /**
   * @brief If this member is true, then the owner task armed the period on the
   *   first processor.
   */
  Atomic_Uint armed;

  /**
   * @brief If this member is true, then the owner task cancelled the period in
   *   the window of the test.
   */
  Atomic_Uint cancelled;

  /**
   * @brief If this member is true, then the timeout of the period gets the
   *   treatment of the test.
   */
  Atomic_Uint arm;

  /**
   * @brief If this member is true, then the timeout reached the window of the
   *   test.
   */
  Atomic_Uint window_reached;

  /**
   * @brief If this member is true, then a wait of the test reached its bound.
   */
  Atomic_Uint window_timed_out;
} RtemsRatemonValTimeoutSmp_Context;

static RtemsRatemonValTimeoutSmp_Context RtemsRatemonValTimeoutSmp_Instance;

typedef RtemsRatemonValTimeoutSmp_Context Context;

static Context *timeout_ctx;

/*
 * The length in clock ticks of the interval which the timeout ends.  The
 * clock driver expires it, so the arm of the period and the move of the
 * owner to the second processor have to fit in it.
 */
#define SHORT_LENGTH 5

/* The length in clock ticks of the interval which shall stay. */
#define LONG_LENGTH 1000

/* The owner cancels the period in the window and waits for nothing. */
#define MODE_CANCEL 0

/* The owner begins a new interval in the window and waits for it. */
#define MODE_WAIT 1

static bool WaitsForPeriod( const Context *ctx )
{
  Thread_Wait_flags flags;

  flags = _Thread_Wait_flags_get( ctx->owner_tcb );

  return ( flags & THREAD_WAIT_CLASS_PERIOD ) != 0 &&
         ( flags & THREAD_WAIT_STATE_BLOCKED ) != 0;
}

void __real__Rate_monotonic_Timeout(
  Watchdog_Control *the_watchdog,
  unsigned int      token
);

void __wrap__Rate_monotonic_Timeout(
  Watchdog_Control *the_watchdog,
  unsigned int      token
);

/*
 * The window of the defect.  The tickle took the timer of the period out of
 * the collection and calls this routine outside the lock of the collection.
 * The owner runs on the second processor, so it ends the interval of the
 * timer while the routine waits here.
 */
void __wrap__Rate_monotonic_Timeout(
  Watchdog_Control *the_watchdog,
  unsigned int      token
)
{
  Context *ctx;

  ctx = timeout_ctx;

  if ( ctx != NULL && GetFlag( &ctx->arm ) != 0 ) {
    int64_t begin;

    SetFlag( &ctx->arm, 0 );
    SetFlag( &ctx->window_reached, 1 );

    SendEvents( ctx->owner_id, RTEMS_EVENT_0 );
    begin = rtems_clock_get_monotonic_sbintime();

    if ( GetFlag( &ctx->mode ) == MODE_WAIT ) {
      while ( !WaitsForPeriod( ctx ) && !WaitTimedOut( begin ) ) {
        /* Wait until the owner waits for a new interval of the period. */
      }
    } else {
      while ( GetFlag( &ctx->cancelled ) == 0 && !WaitTimedOut( begin ) ) {
        /* Wait until the owner cancelled the period. */
      }
    }

    if ( WaitTimedOut( begin ) ) {
      SetFlag( &ctx->window_timed_out, 1 );
    }
  }

  __real__Rate_monotonic_Timeout( the_watchdog, token );
}

static void Owner( rtems_task_argument arg )
{
  Context          *ctx;
  rtems_status_code sc;

  ctx = (Context *) arg;

  sc = rtems_rate_monotonic_create( OBJECT_NAME, &ctx->period_id );
  T_rsc_success( sc );

  while ( true ) {
    /* The runner asks for a timer on its own processor. */
    (void) ReceiveAnyEvents();

    /* The call arms the timer of the period and returns at once. */
    (void) rtems_rate_monotonic_period( ctx->period_id, SHORT_LENGTH );
    SetFlag( &ctx->armed, 1 );

    /* The routine of the timeout opens the window of the test. */
    (void) ReceiveAnyEvents();

    /* The cancel ends the interval whose timeout is in flight. */
    (void) rtems_rate_monotonic_cancel( ctx->period_id );

    if ( GetFlag( &ctx->mode ) == MODE_WAIT ) {
      (void) rtems_rate_monotonic_period( ctx->period_id, LONG_LENGTH );
      (void) rtems_rate_monotonic_period( ctx->period_id, LONG_LENGTH );
    } else {
      SetFlag( &ctx->cancelled, 1 );
    }
  }
}

/*
 * The owner arms the timer where it runs, so it gets the processor of the
 * runner for the arm and the other processor for the window.
 */
static void ArmOnFirstProcessor( Context *ctx )
{
  int64_t begin;

  SetFlag( &ctx->armed, 0 );
  SetAffinityOne( ctx->owner_id, 0 );
  SendEvents( ctx->owner_id, RTEMS_EVENT_0 );
  begin = rtems_clock_get_monotonic_sbintime();

  while ( GetFlag( &ctx->armed ) == 0 && !WaitTimedOut( begin ) ) {
    Yield();
  }

  if ( WaitTimedOut( begin ) ) {
    SetFlag( &ctx->window_timed_out, 1 );
  }

  SetAffinityOne( ctx->owner_id, 1 );
}

/*
 * The clock driver expires the period on the processor of the runner, so
 * the interrupt which runs the window takes the processor away from this
 * loop.  A read of the member therefore happens after the window ended.
 */
static void WaitForWindow( Context *ctx )
{
  int64_t begin;

  begin = rtems_clock_get_monotonic_sbintime();

  while ( GetFlag( &ctx->window_reached ) == 0 && !WaitTimedOut( begin ) ) {
    /* Wait until the timeout of the period reached the window. */
  }

  if ( WaitTimedOut( begin ) ) {
    SetFlag( &ctx->window_timed_out, 1 );
  }
}

static void RtemsRatemonValTimeoutSmp_Setup(
  RtemsRatemonValTimeoutSmp_Context *ctx
)
{
  timeout_ctx = ctx;
  SetFlag( &ctx->arm, 0 );
  SetFlag( &ctx->armed, 0 );

  /*
   * The second scheduler gives its processor up, so the owner arms the
   * period on the processor of the runner.  The timer of the period stays
   * there while the owner runs on the second processor later on.
   */
  RemoveProcessor( SCHEDULER_B_ID, 1 );

  ctx->owner_id = CreateTask( "OWNR", GetSelfPriority() );
  ctx->owner_tcb = GetThread( ctx->owner_id );
  StartTask( ctx->owner_id, Owner, ctx );
  Yield();

  AddProcessor( SCHEDULER_A_ID, 1 );

  /*
   * The affinity of a task holds the processors of its scheduler at the
   * time of its create, so both tasks get a set of their own here.
   */
  SetSelfAffinityOne( 0 );
}

static void RtemsRatemonValTimeoutSmp_Setup_Wrap( void *arg )
{
  RtemsRatemonValTimeoutSmp_Context *ctx;

  ctx = arg;
  RtemsRatemonValTimeoutSmp_Setup( ctx );
}

static void RtemsRatemonValTimeoutSmp_Teardown(
  RtemsRatemonValTimeoutSmp_Context *ctx
)
{
  rtems_status_code sc;

  sc = rtems_rate_monotonic_delete( ctx->period_id );
  T_rsc_success( sc );

  DeleteTask( ctx->owner_id );

  SetSelfAffinityAll();
  RemoveProcessor( SCHEDULER_A_ID, 1 );
  AddProcessor( SCHEDULER_B_ID, 1 );

  timeout_ctx = NULL;
}

static void RtemsRatemonValTimeoutSmp_Teardown_Wrap( void *arg )
{
  RtemsRatemonValTimeoutSmp_Context *ctx;

  ctx = arg;
  RtemsRatemonValTimeoutSmp_Teardown( ctx );
}

static T_fixture RtemsRatemonValTimeoutSmp_Fixture = {
  .setup = RtemsRatemonValTimeoutSmp_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsRatemonValTimeoutSmp_Teardown_Wrap,
  .scope = NULL,
  .initial_context = &RtemsRatemonValTimeoutSmp_Instance
};

/**
 * @brief Let the timeout of a period expire on the first processor while the
 *   owner task cancels the period on the second processor.
 */
static void RtemsRatemonValTimeoutSmp_Action_0(
  RtemsRatemonValTimeoutSmp_Context *ctx
)
{
  ArmOnFirstProcessor( ctx );
  SetFlag( &ctx->mode, MODE_CANCEL );
  SetFlag( &ctx->cancelled, 0 );
  SetFlag( &ctx->window_reached, 0 );
  SetFlag( &ctx->window_timed_out, 0 );
  SetFlag( &ctx->arm, 1 );

  WaitForWindow( ctx );

  /*
   * Check that the timeout reached the window of the test.
   */
  T_true( GetFlag( &ctx->window_reached ) != 0 );

  /*
   * Check that no wait of the test reached its bound.
   */
  T_false( GetFlag( &ctx->window_timed_out ) != 0 );

  /*
   * Check that the timeout of the interval which ended leaves the state of the
   * period alone.
   */
  rtems_status_code                  sc;
  rtems_rate_monotonic_period_status status;

  sc = rtems_rate_monotonic_get_status( ctx->period_id, &status );
  T_rsc_success( sc );
  T_eq_int( status.state, RATE_MONOTONIC_INACTIVE );
  T_eq_u32( status.postponed_jobs_count, 0 );
}

/**
 * @brief Let the timeout of a period expire on the first processor while the
 *   owner task begins a new interval on the second processor.
 */
static void RtemsRatemonValTimeoutSmp_Action_1(
  RtemsRatemonValTimeoutSmp_Context *ctx
)
{
  ArmOnFirstProcessor( ctx );
  SetFlag( &ctx->mode, MODE_WAIT );
  SetFlag( &ctx->window_reached, 0 );
  SetFlag( &ctx->window_timed_out, 0 );
  SetFlag( &ctx->arm, 1 );

  WaitForWindow( ctx );

  /*
   * Check that the timeout reached the window of the test.
   */
  T_true( GetFlag( &ctx->window_reached ) != 0 );

  /*
   * Check that no wait of the test reached its bound.
   */
  T_false( GetFlag( &ctx->window_timed_out ) != 0 );

  /*
   * Check that the timeout of the interval which ended does not end the wait
   * of the owner task which began after it.
   */
  T_true( WaitsForPeriod( ctx ) );
}

/**
 * @fn void T_case_body_RtemsRatemonValTimeoutSmp( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsRatemonValTimeoutSmp,
  &RtemsRatemonValTimeoutSmp_Fixture
)
{
  RtemsRatemonValTimeoutSmp_Context *ctx;

  ctx = T_fixture_context();

  RtemsRatemonValTimeoutSmp_Action_0( ctx );
  RtemsRatemonValTimeoutSmp_Action_1( ctx );
}

/** @} */
