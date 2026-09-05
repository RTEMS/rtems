/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsClockValSetNoDispatch
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
#include <rtems/score/threaddispatch.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsClockValSetNoDispatch spec:/rtems/clock/val/set-no-dispatch
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @brief Tests the thread dispatch state of the realtime tickle of
 *   rtems_clock_set().
 *
 * This test case performs the following actions:
 *
 * - Arm a realtime timer and move the time of day past the time point of the
 *   timer. Let the service routine of the timer report the thread dispatch
 *   state of the tickle.
 *
 *   - Check that the service routine of the timer ran.
 *
 *   - Check that no thread dispatch may take place while rtems_clock_set()
 *     expires the realtime watchdogs.
 *
 * @{
 */

/**
 * @brief Test context for spec:/rtems/clock/val/set-no-dispatch test case.
 */
typedef struct {
  /**
   * @brief This member contains the identifier of the timer.
   */
  rtems_id timer_id;

  /**
   * @brief If this member is true, then the service routine of the timer ran.
   */
  bool routine_ran;

  /**
   * @brief If this member is true, then thread dispatching was enabled while
   *   the service routine of the timer ran.
   */
  bool dispatch_enabled;
} RtemsClockValSetNoDispatch_Context;

static RtemsClockValSetNoDispatch_Context RtemsClockValSetNoDispatch_Instance;

typedef RtemsClockValSetNoDispatch_Context Context;

static const rtems_time_of_day tod_begin = { 2026, 1, 1, 0, 0, 0, 0 };

static const rtems_time_of_day tod_fire = { 2026, 1, 1, 5, 0, 0, 0 };

static const rtems_time_of_day tod_end = { 2026, 1, 1, 6, 0, 0, 0 };

static void Routine( rtems_id id, void *arg )
{
  Context *ctx;

  (void) id;

  ctx = arg;
  ctx->dispatch_enabled = _Thread_Dispatch_is_enabled();
  ctx->routine_ran = true;
}

static void RtemsClockValSetNoDispatch_Teardown(
  RtemsClockValSetNoDispatch_Context *ctx
)
{
  rtems_status_code sc;

  sc = rtems_timer_delete( ctx->timer_id );
  T_rsc_success( sc );
}

static void RtemsClockValSetNoDispatch_Teardown_Wrap( void *arg )
{
  RtemsClockValSetNoDispatch_Context *ctx;

  ctx = arg;
  RtemsClockValSetNoDispatch_Teardown( ctx );
}

static T_fixture RtemsClockValSetNoDispatch_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = RtemsClockValSetNoDispatch_Teardown_Wrap,
  .scope = NULL,
  .initial_context = &RtemsClockValSetNoDispatch_Instance
};

/**
 * @brief Arm a realtime timer and move the time of day past the time point of
 *   the timer. Let the service routine of the timer report the thread dispatch
 *   state of the tickle.
 */
static void RtemsClockValSetNoDispatch_Action_0(
  RtemsClockValSetNoDispatch_Context *ctx
)
{
  rtems_status_code sc;

  ctx->routine_ran = false;
  ctx->dispatch_enabled = true;

  sc = rtems_clock_set( &tod_begin );
  T_rsc_success( sc );

  sc = rtems_timer_create( OBJECT_NAME, &ctx->timer_id );
  T_rsc_success( sc );

  sc = rtems_timer_fire_when( ctx->timer_id, &tod_fire, Routine, ctx );
  T_rsc_success( sc );

  sc = rtems_clock_set( &tod_end );
  T_rsc_success( sc );

  /*
   * Check that the service routine of the timer ran.
   */
  T_true( ctx->routine_ran );

  /*
   * Check that no thread dispatch may take place while rtems_clock_set()
   * expires the realtime watchdogs.
   */
  T_false( ctx->dispatch_enabled );
}

/**
 * @fn void T_case_body_RtemsClockValSetNoDispatch( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsClockValSetNoDispatch,
  &RtemsClockValSetNoDispatch_Fixture
)
{
  RtemsClockValSetNoDispatch_Context *ctx;

  ctx = T_fixture_context();

  RtemsClockValSetNoDispatch_Action_0( ctx );
}

/** @} */
