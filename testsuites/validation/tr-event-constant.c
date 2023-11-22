/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsEventValEventConstant
 */

/*
 * Copyright (C) 2020 embedded brains GmbH & Co. KG
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

#include "tr-event-constant.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsEventValEventConstant spec:/rtems/event/val/event-constant
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @brief Tests an event constant and number of the Event Manager using the
 *   Classic and system event sets of the executing task.
 *
 * This test case performs the following actions:
 *
 * - Validate the event constant.
 *
 *   - Check that the event constant is equal to the event number bit in the
 *     event set.
 *
 *   - Check that the event number bit of the event constant is not set in
 *     RTEMS_PENDING_EVENTS.
 *
 * - Get all pending events of the Classic event set of the executing task.
 *
 *   - Check that the directive call was successful.
 *
 *   - Check that there were no pending events.
 *
 * - Get all pending events of the system event set of the executing task.
 *
 *   - Check that the directive call was successful.
 *
 *   - Check that there were no pending events.
 *
 * - Receive all pending events of the Classic event set of the executing task.
 *
 *   - Check that the directive call was unsatisfied.
 *
 *   - Check that there were no events received.
 *
 * - Receive all pending events of the system event set of the executing task.
 *
 *   - Check that the directive call was unsatisfied.
 *
 *   - Check that there were no events received.
 *
 * - Send the event to the Classic event set of the executing task.
 *
 *   - Check that the directive call was successful.
 *
 * - Get all pending events of the Classic event set of the executing task.
 *
 *   - Check that the directive call was successful.
 *
 *   - Check that the pending event is equal to the event sent by a previous
 *     action.
 *
 * - Get all pending events of the system event set of the executing task.
 *
 *   - Check that the directive call was successful.
 *
 *   - Check that there were no pending events.
 *
 * - Receive any event of the Classic event set of the executing task.
 *
 *   - Check that the directive call was successful.
 *
 *   - Check that the received event is equal to the event sent by a previous
 *     action.
 *
 * - Receive any event of the system event set of the executing task.
 *
 *   - Check that the directive call was unsatisfied.
 *
 *   - Check that the no events were received.
 *
 * - Send the event to the Classic event set of the executing task.
 *
 *   - Check that the directive call was successful.
 *
 * - Get all pending events of the Classic event set of the executing task.
 *
 *   - Check that the directive call was successful.
 *
 *   - Check that there were no pending events.
 *
 * - Get all pending events of the system event set of the executing task.
 *
 *   - Check that the directive call was successful.
 *
 *   - Check that the pending event is equal to the event sent by a previous
 *     action.
 *
 * - Receive any event of the Classic event set of the executing task.
 *
 *   - Check that the directive call was unsatisfied.
 *
 *   - Check that the no events were received.
 *
 * - Receive any event of the system event set of the executing task.
 *
 *   - Check that the directive call was successful.
 *
 *   - Check that the received event is equal to the event sent by a previous
 *     action.
 *
 * - Get all pending events of the Classic event set of the executing task.
 *
 *   - Check that the directive call was successful.
 *
 *   - Check that there were no pending events.
 *
 * - Get all pending events of the system event set of the executing task.
 *
 *   - Check that the directive call was successful.
 *
 *   - Check that there were no pending events.
 *
 * @{
 */

/**
 * @brief Test context for spec:/rtems/event/val/event-constant test case.
 */
typedef struct {
  /**
   * @brief This member contains a copy of the corresponding
   *   RtemsEventValEventConstant_Run() parameter.
   */
  rtems_event_set event;

  /**
   * @brief This member contains a copy of the corresponding
   *   RtemsEventValEventConstant_Run() parameter.
   */
  int number;
} RtemsEventValEventConstant_Context;

static RtemsEventValEventConstant_Context
  RtemsEventValEventConstant_Instance;

static T_fixture RtemsEventValEventConstant_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = NULL,
  .initial_context = &RtemsEventValEventConstant_Instance
};

/**
 * @brief Validate the event constant.
 */
static void RtemsEventValEventConstant_Action_0(
  RtemsEventValEventConstant_Context *ctx
)
{
  /* No action */

  /*
   * Check that the event constant is equal to the event number bit in the
   * event set.
   */
  T_step_eq_u32(
    0,
    ctx->event,
    ( (rtems_event_set) 1 ) << ctx->number
  );

  /*
   * Check that the event number bit of the event constant is not set in
   * RTEMS_PENDING_EVENTS.
   */
  T_step_eq_u32( 1, ctx->event & RTEMS_PENDING_EVENTS, 0 );
}

/**
 * @brief Get all pending events of the Classic event set of the executing
 *   task.
 */
static void RtemsEventValEventConstant_Action_1(
  RtemsEventValEventConstant_Context *ctx
)
{
  rtems_status_code sc;
  rtems_event_set   out;

  out = RTEMS_ALL_EVENTS;
  sc = rtems_event_receive(
    RTEMS_PENDING_EVENTS,
    RTEMS_DEFAULT_OPTIONS,
    0,
    &out
  );

  /*
   * Check that the directive call was successful.
   */
  T_step_rsc_success( 2, sc );

  /*
   * Check that there were no pending events.
   */
  T_step_eq_u32( 3, out, 0 );
}

/**
 * @brief Get all pending events of the system event set of the executing task.
 */
static void RtemsEventValEventConstant_Action_2(
  RtemsEventValEventConstant_Context *ctx
)
{
  rtems_status_code sc;
  rtems_event_set   out;

  out = RTEMS_ALL_EVENTS;
  sc = rtems_event_system_receive(
    RTEMS_PENDING_EVENTS,
    RTEMS_DEFAULT_OPTIONS,
    0,
    &out
  );

  /*
   * Check that the directive call was successful.
   */
  T_step_rsc_success( 4, sc );

  /*
   * Check that there were no pending events.
   */
  T_step_eq_u32( 5, out, 0 );
}

/**
 * @brief Receive all pending events of the Classic event set of the executing
 *   task.
 */
static void RtemsEventValEventConstant_Action_3(
  RtemsEventValEventConstant_Context *ctx
)
{
  rtems_status_code sc;
  rtems_event_set   out;

  out = RTEMS_ALL_EVENTS;
  sc = rtems_event_receive(
    RTEMS_ALL_EVENTS,
    RTEMS_NO_WAIT | RTEMS_EVENT_ANY,
    0,
    &out
  );

  /*
   * Check that the directive call was unsatisfied.
   */
  T_step_rsc( 6, sc, RTEMS_UNSATISFIED );

  /*
   * Check that there were no events received.
   */
  T_step_eq_u32( 7, out, 0 );
}

/**
 * @brief Receive all pending events of the system event set of the executing
 *   task.
 */
static void RtemsEventValEventConstant_Action_4(
  RtemsEventValEventConstant_Context *ctx
)
{
  rtems_status_code sc;
  rtems_event_set   out;

  out = RTEMS_ALL_EVENTS;
  sc = rtems_event_system_receive(
    RTEMS_ALL_EVENTS,
    RTEMS_NO_WAIT | RTEMS_EVENT_ANY,
    0,
    &out
  );

  /*
   * Check that the directive call was unsatisfied.
   */
  T_step_rsc( 8, sc, RTEMS_UNSATISFIED );

  /*
   * Check that there were no events received.
   */
  T_step_eq_u32( 9, out, 0 );
}

/**
 * @brief Send the event to the Classic event set of the executing task.
 */
static void RtemsEventValEventConstant_Action_5(
  RtemsEventValEventConstant_Context *ctx
)
{
  rtems_status_code sc;

  sc = rtems_event_send( RTEMS_SELF, ctx->event );

  /*
   * Check that the directive call was successful.
   */
  T_step_rsc_success( 10, sc );
}

/**
 * @brief Get all pending events of the Classic event set of the executing
 *   task.
 */
static void RtemsEventValEventConstant_Action_6(
  RtemsEventValEventConstant_Context *ctx
)
{
  rtems_status_code sc;
  rtems_event_set   out;

  out = RTEMS_ALL_EVENTS;
  sc = rtems_event_receive(
    RTEMS_PENDING_EVENTS,
    RTEMS_DEFAULT_OPTIONS,
    0,
    &out
  );

  /*
   * Check that the directive call was successful.
   */
  T_step_rsc_success( 11, sc );

  /*
   * Check that the pending event is equal to the event sent by a previous
   * action.
   */
  T_step_eq_u32( 12, out, ctx->event );
}

/**
 * @brief Get all pending events of the system event set of the executing task.
 */
static void RtemsEventValEventConstant_Action_7(
  RtemsEventValEventConstant_Context *ctx
)
{
  rtems_status_code sc;
  rtems_event_set   out;

  out = RTEMS_ALL_EVENTS;
  sc = rtems_event_system_receive(
    RTEMS_PENDING_EVENTS,
    RTEMS_DEFAULT_OPTIONS,
    0,
    &out
  );

  /*
   * Check that the directive call was successful.
   */
  T_step_rsc_success( 13, sc );

  /*
   * Check that there were no pending events.
   */
  T_step_eq_u32( 14, out, 0 );
}

/**
 * @brief Receive any event of the Classic event set of the executing task.
 */
static void RtemsEventValEventConstant_Action_8(
  RtemsEventValEventConstant_Context *ctx
)
{
  rtems_status_code sc;
  rtems_event_set   out;

  out = 0;
  sc = rtems_event_receive(
    RTEMS_ALL_EVENTS,
    RTEMS_NO_WAIT | RTEMS_EVENT_ANY,
    0,
    &out
  );

  /*
   * Check that the directive call was successful.
   */
  T_step_rsc_success( 15, sc );

  /*
   * Check that the received event is equal to the event sent by a previous
   * action.
   */
  T_step_eq_u32( 16, out, ctx->event );
}

/**
 * @brief Receive any event of the system event set of the executing task.
 */
static void RtemsEventValEventConstant_Action_9(
  RtemsEventValEventConstant_Context *ctx
)
{
  rtems_status_code sc;
  rtems_event_set   out;

  out = RTEMS_ALL_EVENTS;
  sc = rtems_event_system_receive(
    RTEMS_ALL_EVENTS,
    RTEMS_NO_WAIT | RTEMS_EVENT_ANY,
    0,
    &out
  );

  /*
   * Check that the directive call was unsatisfied.
   */
  T_step_rsc( 17, sc, RTEMS_UNSATISFIED );

  /*
   * Check that the no events were received.
   */
  T_step_eq_u32( 18, out, 0 );
}

/**
 * @brief Send the event to the Classic event set of the executing task.
 */
static void RtemsEventValEventConstant_Action_10(
  RtemsEventValEventConstant_Context *ctx
)
{
  rtems_status_code sc;

  sc = rtems_event_system_send( RTEMS_SELF, ctx->event );

  /*
   * Check that the directive call was successful.
   */
  T_step_rsc_success( 19, sc );
}

/**
 * @brief Get all pending events of the Classic event set of the executing
 *   task.
 */
static void RtemsEventValEventConstant_Action_11(
  RtemsEventValEventConstant_Context *ctx
)
{
  rtems_status_code sc;
  rtems_event_set   out;

  out = RTEMS_ALL_EVENTS;
  sc = rtems_event_receive(
    RTEMS_PENDING_EVENTS,
    RTEMS_DEFAULT_OPTIONS,
    0,
    &out
  );

  /*
   * Check that the directive call was successful.
   */
  T_step_rsc_success( 20, sc );

  /*
   * Check that there were no pending events.
   */
  T_step_eq_u32( 21, out, 0 );
}

/**
 * @brief Get all pending events of the system event set of the executing task.
 */
static void RtemsEventValEventConstant_Action_12(
  RtemsEventValEventConstant_Context *ctx
)
{
  rtems_status_code sc;
  rtems_event_set   out;

  out = RTEMS_ALL_EVENTS;
  sc = rtems_event_system_receive(
    RTEMS_PENDING_EVENTS,
    RTEMS_DEFAULT_OPTIONS,
    0,
    &out
  );

  /*
   * Check that the directive call was successful.
   */
  T_step_rsc_success( 22, sc );

  /*
   * Check that the pending event is equal to the event sent by a previous
   * action.
   */
  T_step_eq_u32( 23, out, ctx->event );
}

/**
 * @brief Receive any event of the Classic event set of the executing task.
 */
static void RtemsEventValEventConstant_Action_13(
  RtemsEventValEventConstant_Context *ctx
)
{
  rtems_status_code sc;
  rtems_event_set   out;

  out = RTEMS_ALL_EVENTS;
  sc = rtems_event_receive(
    RTEMS_ALL_EVENTS,
    RTEMS_NO_WAIT | RTEMS_EVENT_ANY,
    0,
    &out
  );

  /*
   * Check that the directive call was unsatisfied.
   */
  T_step_rsc( 24, sc, RTEMS_UNSATISFIED );

  /*
   * Check that the no events were received.
   */
  T_step_eq_u32( 25, out, 0 );
}

/**
 * @brief Receive any event of the system event set of the executing task.
 */
static void RtemsEventValEventConstant_Action_14(
  RtemsEventValEventConstant_Context *ctx
)
{
  rtems_status_code sc;
  rtems_event_set   out;

  out = 0;
  sc = rtems_event_system_receive(
    RTEMS_ALL_EVENTS,
    RTEMS_NO_WAIT | RTEMS_EVENT_ANY,
    0,
    &out
  );

  /*
   * Check that the directive call was successful.
   */
  T_step_rsc_success( 26, sc );

  /*
   * Check that the received event is equal to the event sent by a previous
   * action.
   */
  T_step_eq_u32( 27, out, ctx->event );
}

/**
 * @brief Get all pending events of the Classic event set of the executing
 *   task.
 */
static void RtemsEventValEventConstant_Action_15(
  RtemsEventValEventConstant_Context *ctx
)
{
  rtems_status_code sc;
  rtems_event_set   out;

  out = RTEMS_ALL_EVENTS;
  sc = rtems_event_receive(
    RTEMS_PENDING_EVENTS,
    RTEMS_DEFAULT_OPTIONS,
    0,
    &out
  );

  /*
   * Check that the directive call was successful.
   */
  T_step_rsc_success( 28, sc );

  /*
   * Check that there were no pending events.
   */
  T_step_eq_u32( 29, out, 0 );
}

/**
 * @brief Get all pending events of the system event set of the executing task.
 */
static void RtemsEventValEventConstant_Action_16(
  RtemsEventValEventConstant_Context *ctx
)
{
  rtems_status_code sc;
  rtems_event_set   out;

  out = RTEMS_ALL_EVENTS;
  sc = rtems_event_system_receive(
    RTEMS_PENDING_EVENTS,
    RTEMS_DEFAULT_OPTIONS,
    0,
    &out
  );

  /*
   * Check that the directive call was successful.
   */
  T_step_rsc_success( 30, sc );

  /*
   * Check that there were no pending events.
   */
  T_step_eq_u32( 31, out, 0 );
}

static T_fixture_node RtemsEventValEventConstant_Node;

static T_remark RtemsEventValEventConstant_Remark = {
  .next = NULL,
  .remark = "RtemsEventValEventConstant"
};

void RtemsEventValEventConstant_Run( rtems_event_set event, int number )
{
  RtemsEventValEventConstant_Context *ctx;

  ctx = &RtemsEventValEventConstant_Instance;
  ctx->event = event;
  ctx->number = number;

  ctx = T_push_fixture(
    &RtemsEventValEventConstant_Node,
    &RtemsEventValEventConstant_Fixture
  );

  T_plan( 32 );

  RtemsEventValEventConstant_Action_0( ctx );
  RtemsEventValEventConstant_Action_1( ctx );
  RtemsEventValEventConstant_Action_2( ctx );
  RtemsEventValEventConstant_Action_3( ctx );
  RtemsEventValEventConstant_Action_4( ctx );
  RtemsEventValEventConstant_Action_5( ctx );
  RtemsEventValEventConstant_Action_6( ctx );
  RtemsEventValEventConstant_Action_7( ctx );
  RtemsEventValEventConstant_Action_8( ctx );
  RtemsEventValEventConstant_Action_9( ctx );
  RtemsEventValEventConstant_Action_10( ctx );
  RtemsEventValEventConstant_Action_11( ctx );
  RtemsEventValEventConstant_Action_12( ctx );
  RtemsEventValEventConstant_Action_13( ctx );
  RtemsEventValEventConstant_Action_14( ctx );
  RtemsEventValEventConstant_Action_15( ctx );
  RtemsEventValEventConstant_Action_16( ctx );

  T_add_remark( &RtemsEventValEventConstant_Remark );
  T_pop_fixture();
}

/** @} */
