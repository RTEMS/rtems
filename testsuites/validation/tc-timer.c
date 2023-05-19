/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsTimerValTimer
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
#include <rtems/score/atomic.h>

#include "ts-config.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsTimerValTimer spec:/rtems/timer/val/timer
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @brief Tests general timer behaviour.
 *
 * This test case performs the following actions:
 *
 * - Create a couple of timers.
 *
 *   - Schedule some timers at the same time point.
 *
 *   - Fire the timers and check that they fired in the expected order.
 *
 *   - Clean up all used resources.
 *
 * @{
 */

/**
 * @brief Test context for spec:/rtems/timer/val/timer test case.
 */
typedef struct {
  /**
   * @brief This member contains the timer identifiers.
   */
  rtems_id timer_ids[ TEST_MAXIMUM_TIMERS ];

  /**
   * @brief This member contains the counter.
   */
  Atomic_Uint counter;

  /**
   * @brief This member contains the timer counter snapshots.
   */
  unsigned int counter_snapshots[ TEST_MAXIMUM_TIMERS ];
} RtemsTimerValTimer_Context;

static RtemsTimerValTimer_Context
  RtemsTimerValTimer_Instance;

typedef RtemsTimerValTimer_Context Context;

static void Timer( rtems_id timer, void *arg )
{
  Context      *ctx;
  unsigned int *counter;

  ctx = T_fixture_context();
  counter = arg;
  *counter = _Atomic_Fetch_add_uint(
    &ctx->counter,
    1,
    ATOMIC_ORDER_RELAXED
  ) + 1;
}

static void Fire( Context *ctx, size_t i, rtems_interval ticks )
{
  rtems_status_code sc;

  ctx->counter_snapshots[ i ] = 0;
  sc = rtems_timer_fire_after(
    ctx->timer_ids[ i ],
    ticks,
    Timer,
    &ctx->counter_snapshots[ i ]
  );
  T_rsc_success( sc );
}

static T_fixture RtemsTimerValTimer_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = NULL,
  .initial_context = &RtemsTimerValTimer_Instance
};

/**
 * @brief Create a couple of timers.
 */
static void RtemsTimerValTimer_Action_0( RtemsTimerValTimer_Context *ctx )
{
  rtems_status_code sc;
  size_t            i;

  T_assert_eq_sz( TEST_MAXIMUM_TIMERS, 10 );

  _Atomic_Init_uint( &ctx->counter, 0 );

  for ( i = 0; i < TEST_MAXIMUM_TIMERS ; ++i ) {
    sc = rtems_timer_create(
      rtems_build_name( 'T', 'E', 'S', 'T' ),
      &ctx->timer_ids[ i ]
    );
    T_rsc_success( sc );
  }

  /*
   * Schedule some timers at the same time point.
   */
  Fire( ctx, 3, 2 );
  Fire( ctx, 0, 1 );
  Fire( ctx, 7, 3 );
  Fire( ctx, 4, 2 );
  Fire( ctx, 5, 2 );
  Fire( ctx, 8, 3 );
  Fire( ctx, 9, 3 );
  Fire( ctx, 1, 1 );
  Fire( ctx, 2, 1 );
  Fire( ctx, 6, 2 );

  /*
   * Fire the timers and check that they fired in the expected order.
   */
  FinalClockTick();

  for ( i = 0; i < TEST_MAXIMUM_TIMERS ; ++i ) {
    T_eq_sz( ctx->counter_snapshots[ i ], i + 1 );
  }

  /*
   * Clean up all used resources.
   */
  for ( i = 0; i < TEST_MAXIMUM_TIMERS ; ++i ) {
    sc = rtems_timer_delete( ctx->timer_ids[ i ] );
    T_rsc_success( sc );
  }
}

/**
 * @fn void T_case_body_RtemsTimerValTimer( void )
 */
T_TEST_CASE_FIXTURE( RtemsTimerValTimer, &RtemsTimerValTimer_Fixture )
{
  RtemsTimerValTimer_Context *ctx;

  ctx = T_fixture_context();

  RtemsTimerValTimer_Action_0( ctx );
}

/** @} */
