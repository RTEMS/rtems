/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsTimerValDeleteDuringClockSet
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
#include <rtems/score/statesimpl.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsTimerValDeleteDuringClockSet \
 *   spec:/rtems/timer/val/delete-during-clock-set
 *
 * @ingroup TestsuitesValidationSmpOnly0
 *
 * @brief Tests that a delete of a timer of the realtime clock ends after a set
 *   of that clock returns from the Timer Service Routine of the timer.
 *
 * This test case performs the following actions:
 *
 * - Let a task of the second processor delete a timer of the realtime clock
 *   while a set of that clock runs the Timer Service Routine of the timer.
 *
 *   - Check that the set reached the window of the test.
 *
 *   - Check that the wait of the window reached no bound.
 *
 *   - Check that the delete of the worker ended.
 *
 *   - Check that the directive of the worker ended after the routine returned.
 *
 * @{
 */

/**
 * @brief Test context for spec:/rtems/timer/val/delete-during-clock-set test
 *   case.
 */
typedef struct {
  /**
   * @brief This member contains the identifier of the worker task.
   */
  rtems_id worker_id;

  /**
   * @brief This member contains the identifier of the timer.
   */
  rtems_id timer_id;

  /**
   * @brief If this member is true, then the Timer Service Routine of the timer
   *   gets the treatment of the test.
   */
  Atomic_Uint arm;

  /**
   * @brief If this member is true, then the set reached the window of the
   *   test.
   */
  Atomic_Uint window_reached;

  /**
   * @brief If this member is true, then a wait of the window reached its
   *   bound.
   */
  Atomic_Uint window_timed_out;

  /**
   * @brief If this member is true, then the delete of the worker returned.
   */
  Atomic_Uint delete_returned;

  /**
   * @brief If this member is true, then the delete of the worker returned
   *   while the routine was in the window of the test.
   */
  Atomic_Uint delete_returned_in_window;

  /**
   * @brief If this member is true, then the worker ended its delete.
   */
  Atomic_Uint deleted;
} RtemsTimerValDeleteDuringClockSet_Context;

static RtemsTimerValDeleteDuringClockSet_Context
  RtemsTimerValDeleteDuringClockSet_Instance;

typedef RtemsTimerValDeleteDuringClockSet_Context Context;

static Context *set_ctx;

/* The name of the timer of the test. */
#define NAME rtems_build_name( 'T', 'I', 'M', 'E' )

/* The time of day before the set of the test. */
static const rtems_time_of_day tod_now = { 2000, 1, 1, 0, 0, 0, 0 };

/* The time of day at which the timer of the test fires. */
static const rtems_time_of_day tod_fire = { 2000, 1, 1, 1, 0, 0, 0 };

/* The time of day which the set of the test hands to the clock. */
static const rtems_time_of_day tod_late = { 2000, 1, 1, 2, 0, 0, 0 };

/*
 * The window of the defect.  The set of the clock took the timer out of the
 * collection and calls this routine outside the lock of the collection.
 * The worker of the second processor deletes the timer while the routine
 * waits here.
 */
static void Routine( rtems_id id, void *arg )
{
  Context *ctx;
  bool     blocked;

  (void) id;
  ctx = (Context *) arg;

  if ( GetFlag( &ctx->arm ) != 0 ) {
    SetFlag( &ctx->arm, 0 );
    SetFlag( &ctx->window_reached, 1 );

    SendEvents( ctx->worker_id, RTEMS_EVENT_0 );

    /* The mutex of the set blocks the delete of the worker. */
    blocked = WaitForBlockedState(
      ctx->worker_id,
      STATES_WAITING_FOR_MUTEX,
      &ctx->delete_returned
    );
    SetFlag( &ctx->window_timed_out, !blocked );
    SetFlag(
      &ctx->delete_returned_in_window,
      GetFlag( &ctx->delete_returned )
    );
  }
}

static void Worker( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;

  while ( true ) {
    rtems_status_code sc;

    /* The routine of the timer opens the window of the test. */
    (void) ReceiveAnyEvents();

    sc = rtems_timer_delete( ctx->timer_id );
    SetFlag( &ctx->delete_returned, 1 );
    T_rsc_success( sc );
    SetFlag( &ctx->deleted, 1 );
  }
}

static void RtemsTimerValDeleteDuringClockSet_Setup(
  RtemsTimerValDeleteDuringClockSet_Context *ctx
)
{
  set_ctx = ctx;
  SetFlag( &ctx->arm, 0 );

  ctx->worker_id = CreateTask( "WORK", PRIO_NORMAL );
  SetScheduler( ctx->worker_id, SCHEDULER_B_ID, PRIO_NORMAL );
  StartTask( ctx->worker_id, Worker, ctx );
}

static void RtemsTimerValDeleteDuringClockSet_Setup_Wrap( void *arg )
{
  RtemsTimerValDeleteDuringClockSet_Context *ctx;

  ctx = arg;
  RtemsTimerValDeleteDuringClockSet_Setup( ctx );
}

static void RtemsTimerValDeleteDuringClockSet_Teardown(
  RtemsTimerValDeleteDuringClockSet_Context *ctx
)
{
  DeleteTask( ctx->worker_id );
  set_ctx = NULL;
}

static void RtemsTimerValDeleteDuringClockSet_Teardown_Wrap( void *arg )
{
  RtemsTimerValDeleteDuringClockSet_Context *ctx;

  ctx = arg;
  RtemsTimerValDeleteDuringClockSet_Teardown( ctx );
}

static T_fixture RtemsTimerValDeleteDuringClockSet_Fixture = {
  .setup = RtemsTimerValDeleteDuringClockSet_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsTimerValDeleteDuringClockSet_Teardown_Wrap,
  .scope = NULL,
  .initial_context = &RtemsTimerValDeleteDuringClockSet_Instance
};

/**
 * @brief Let a task of the second processor delete a timer of the realtime
 *   clock while a set of that clock runs the Timer Service Routine of the
 *   timer.
 */
static void RtemsTimerValDeleteDuringClockSet_Action_0(
  RtemsTimerValDeleteDuringClockSet_Context *ctx
)
{
  rtems_status_code sc;

  SetFlag( &ctx->window_reached, 0 );
  SetFlag( &ctx->window_timed_out, 0 );
  SetFlag( &ctx->delete_returned, 0 );
  SetFlag( &ctx->delete_returned_in_window, 1 );
  SetFlag( &ctx->deleted, 0 );

  sc = rtems_timer_create( NAME, &ctx->timer_id );
  T_rsc_success( sc );

  sc = rtems_clock_set( &tod_now );
  T_rsc_success( sc );

  sc = rtems_timer_fire_when( ctx->timer_id, &tod_fire, Routine, ctx );
  T_rsc_success( sc );

  SetFlag( &ctx->arm, 1 );

  /* The set runs the routine of the timer in this task. */
  sc = rtems_clock_set( &tod_late );
  T_rsc_success( sc );

  (void) WaitForFlag( &ctx->deleted );

  /*
   * Check that the set reached the window of the test.
   */
  T_true( GetFlag( &ctx->window_reached ) != 0 );

  /*
   * Check that the wait of the window reached no bound.
   */
  T_false( GetFlag( &ctx->window_timed_out ) != 0 );

  /*
   * Check that the delete of the worker ended.
   */
  T_true( GetFlag( &ctx->deleted ) != 0 );

  /*
   * Check that the directive of the worker ended after the routine returned.
   */
  T_false( GetFlag( &ctx->delete_returned_in_window ) != 0 );
}

/**
 * @fn void T_case_body_RtemsTimerValDeleteDuringClockSet( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsTimerValDeleteDuringClockSet,
  &RtemsTimerValDeleteDuringClockSet_Fixture
)
{
  RtemsTimerValDeleteDuringClockSet_Context *ctx;

  ctx = T_fixture_context();

  RtemsTimerValDeleteDuringClockSet_Action_0( ctx );
}

/** @} */
