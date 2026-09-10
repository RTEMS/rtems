/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsTimerValDeleteDuringTickle
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
#include <rtems/rtems/timerimpl.h>
#include <rtems/score/watchdogimpl.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsTimerValDeleteDuringTickle \
 *   spec:/rtems/timer/val/delete-during-tickle
 *
 * @ingroup TestsuitesValidationSmpOnly0
 *
 * @brief Tests that a delete of a timer in interrupt context ends after the
 *   Timer Service Routine of the timer returns.
 *
 * This test case performs the following actions:
 *
 * - Let a task of the second processor delete a timer in interrupt context
 *   while the tickle of the first processor runs the adaptor of the timer.
 *
 *   - Check that the tickle reached the window of the test.
 *
 *   - Check that the delete waited for the end of the adaptor.
 *
 *   - Check that the directive of the worker ended after the adaptor returned.
 *
 * @{
 */

/**
 * @brief Test context for spec:/rtems/timer/val/delete-during-tickle test
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
   * @brief If this member is true, then the adaptor of the timer gets the
   *   treatment of the test.
   */
  Atomic_Uint arm;

  /**
   * @brief This member references the watchdog of the timer of the test while
   *   the tickle runs the adaptor of that timer.
   */
  const Watchdog_Control *watchdog;

  /**
   * @brief If this member is true, then the tickle reached the window of the
   *   test.
   */
  Atomic_Uint window_reached;

  /**
   * @brief If this member is true, then the delete of the worker waits for the
   *   end of the adaptor.
   */
  Atomic_Uint delete_waits;

  /**
   * @brief If this member is true, then the delete of the worker returned.
   */
  Atomic_Uint delete_returned;

  /**
   * @brief If this member is true, then the delete of the worker returned
   *   while the adaptor was in the window of the test.
   */
  Atomic_Uint delete_returned_in_window;

  /**
   * @brief If this member is true, then the worker ended its delete.
   */
  Atomic_Uint deleted;
} RtemsTimerValDeleteDuringTickle_Context;

static RtemsTimerValDeleteDuringTickle_Context
  RtemsTimerValDeleteDuringTickle_Instance;

typedef RtemsTimerValDeleteDuringTickle_Context Context;

static Context *delete_ctx;

/* The name of the timer of the test. */
#define NAME rtems_build_name( 'T', 'I', 'M', 'E' )

/*
 * The length in clock ticks of the interval of the timer.  The clock driver
 * expires it, so the arm and the store of the member below have to fit in
 * it.
 */
#define FIRE_LENGTH 5

static void Routine( rtems_id id, void *arg )
{
  (void) id;
  (void) arg;
}

void __real__Timer_Routine_adaptor(
  Watchdog_Control *the_watchdog,
  unsigned int      token
);

void __wrap__Timer_Routine_adaptor(
  Watchdog_Control *the_watchdog,
  unsigned int      token
);

/*
 * The window of the defect.  The tickle took the ticker of the timer out of
 * the collection and calls this routine outside the lock of the collection.
 * The worker of the second processor deletes the timer while the routine
 * waits here, so the read of the timer below meets the delete.
 */
void __wrap__Timer_Routine_adaptor(
  Watchdog_Control *the_watchdog,
  unsigned int      token
)
{
  Context             *ctx;
  const Timer_Control *the_timer;

  ctx = delete_ctx;

  /* The link wrap covers every timer of the test suite. */
  the_timer = RTEMS_CONTAINER_OF( the_watchdog, Timer_Control, Ticker );

  if (
    ctx != NULL && GetFlag( &ctx->arm ) != 0 &&
    the_timer->Object.id == ctx->timer_id
  ) {
    SetFlag( &ctx->arm, 0 );
    ctx->watchdog = the_watchdog;
    SetFlag( &ctx->window_reached, 1 );

    SendEvents( ctx->worker_id, RTEMS_EVENT_0 );
    (void) WaitForFlag( &ctx->delete_waits );
    SetFlag(
      &ctx->delete_returned_in_window,
      GetFlag( &ctx->delete_returned )
    );
  }

  __real__Timer_Routine_adaptor( the_watchdog, token );
}

void __real__Watchdog_Wait_for_service_stop(
  const Watchdog_Control *the_watchdog
);

void __wrap__Watchdog_Wait_for_service_stop(
  const Watchdog_Control *the_watchdog
);

/*
 * The delete of the worker tells the window that it reached the wait.  The
 * link wrap covers every watchdog of the test suite, so the member which the
 * adaptor kept names the one of the test.
 */
void __wrap__Watchdog_Wait_for_service_stop(
  const Watchdog_Control *the_watchdog
)
{
  Context *ctx;

  ctx = delete_ctx;

  if (
    ctx != NULL && GetFlag( &ctx->window_reached ) != 0 &&
    the_watchdog == ctx->watchdog
  ) {
    SetFlag( &ctx->delete_waits, 1 );
  }

  __real__Watchdog_Wait_for_service_stop( the_watchdog );
}

static void Worker( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;

  while ( true ) {
    rtems_status_code sc;

    /* The adaptor of the timer opens the window of the test. */
    (void) ReceiveAnyEvents();

    sc = rtems_timer_delete( ctx->timer_id );
    SetFlag( &ctx->delete_returned, 1 );
    T_rsc_success( sc );
    SetFlag( &ctx->deleted, 1 );
  }
}

static void RtemsTimerValDeleteDuringTickle_Setup(
  RtemsTimerValDeleteDuringTickle_Context *ctx
)
{
  delete_ctx = ctx;
  SetFlag( &ctx->arm, 0 );

  ctx->worker_id = CreateTask( "WORK", PRIO_NORMAL );
  SetScheduler( ctx->worker_id, SCHEDULER_B_ID, PRIO_NORMAL );
  StartTask( ctx->worker_id, Worker, ctx );
}

static void RtemsTimerValDeleteDuringTickle_Setup_Wrap( void *arg )
{
  RtemsTimerValDeleteDuringTickle_Context *ctx;

  ctx = arg;
  RtemsTimerValDeleteDuringTickle_Setup( ctx );
}

static void RtemsTimerValDeleteDuringTickle_Teardown(
  RtemsTimerValDeleteDuringTickle_Context *ctx
)
{
  DeleteTask( ctx->worker_id );
  delete_ctx = NULL;
}

static void RtemsTimerValDeleteDuringTickle_Teardown_Wrap( void *arg )
{
  RtemsTimerValDeleteDuringTickle_Context *ctx;

  ctx = arg;
  RtemsTimerValDeleteDuringTickle_Teardown( ctx );
}

static T_fixture RtemsTimerValDeleteDuringTickle_Fixture = {
  .setup = RtemsTimerValDeleteDuringTickle_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsTimerValDeleteDuringTickle_Teardown_Wrap,
  .scope = NULL,
  .initial_context = &RtemsTimerValDeleteDuringTickle_Instance
};

/**
 * @brief Let a task of the second processor delete a timer in interrupt
 *   context while the tickle of the first processor runs the adaptor of the
 *   timer.
 */
static void RtemsTimerValDeleteDuringTickle_Action_0(
  RtemsTimerValDeleteDuringTickle_Context *ctx
)
{
  rtems_status_code sc;

  ctx->watchdog = NULL;
  SetFlag( &ctx->window_reached, 0 );
  SetFlag( &ctx->delete_waits, 0 );
  SetFlag( &ctx->delete_returned, 0 );
  SetFlag( &ctx->delete_returned_in_window, 1 );
  SetFlag( &ctx->deleted, 0 );

  sc = rtems_timer_create( NAME, &ctx->timer_id );
  T_rsc_success( sc );

  sc = rtems_timer_fire_after( ctx->timer_id, FIRE_LENGTH, Routine, ctx );
  T_rsc_success( sc );

  SetFlag( &ctx->arm, 1 );
  (void) WaitForFlag( &ctx->deleted );

  /*
   * Check that the tickle reached the window of the test.
   */
  T_true( GetFlag( &ctx->window_reached ) != 0 );

  /*
   * Check that the delete waited for the end of the adaptor.
   */
  T_true( GetFlag( &ctx->delete_waits ) != 0 );

  /*
   * Check that the directive of the worker ended after the adaptor returned.
   */
  T_false( GetFlag( &ctx->delete_returned_in_window ) != 0 );
}

/**
 * @fn void T_case_body_RtemsTimerValDeleteDuringTickle( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsTimerValDeleteDuringTickle,
  &RtemsTimerValDeleteDuringTickle_Fixture
)
{
  RtemsTimerValDeleteDuringTickle_Context *ctx;

  ctx = T_fixture_context();

  RtemsTimerValDeleteDuringTickle_Action_0( ctx );
}

/** @} */
