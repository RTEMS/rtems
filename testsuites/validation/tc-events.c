/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsEventValEvents
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
 * @defgroup RtemsEventValEvents spec:/rtems/event/val/events
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @brief Tests the Event Manager API.
 *
 * This test case performs the following actions:
 *
 * - Run the event constant and number test for all 32 event constants.
 *
 * - Check that RTEMS_PENDING_EVENTS is a constant expression which evaluates
 *   to a value of zero.
 *
 * - Calculate the value of a bitwise or of all 32 event constants.
 *
 *   - Check that the value is equal to RTEMS_ALL_EVENTS.
 *
 * - Validate the Event Manager directive options.
 *
 *   - Check that RTEMS_EVENT_ALL is equal to zero.
 *
 *   - Check that RTEMS_EVENT_ANY is a power of two.
 *
 * @{
 */

static const rtems_event_set events[] = {
  RTEMS_EVENT_0,
  RTEMS_EVENT_1,
  RTEMS_EVENT_2,
  RTEMS_EVENT_3,
  RTEMS_EVENT_4,
  RTEMS_EVENT_5,
  RTEMS_EVENT_6,
  RTEMS_EVENT_7,
  RTEMS_EVENT_8,
  RTEMS_EVENT_9,
  RTEMS_EVENT_10,
  RTEMS_EVENT_11,
  RTEMS_EVENT_12,
  RTEMS_EVENT_13,
  RTEMS_EVENT_14,
  RTEMS_EVENT_15,
  RTEMS_EVENT_16,
  RTEMS_EVENT_17,
  RTEMS_EVENT_18,
  RTEMS_EVENT_19,
  RTEMS_EVENT_20,
  RTEMS_EVENT_21,
  RTEMS_EVENT_22,
  RTEMS_EVENT_23,
  RTEMS_EVENT_24,
  RTEMS_EVENT_25,
  RTEMS_EVENT_26,
  RTEMS_EVENT_27,
  RTEMS_EVENT_28,
  RTEMS_EVENT_29,
  RTEMS_EVENT_30,
  RTEMS_EVENT_31
};

/**
 * @brief Run the event constant and number test for all 32 event constants.
 */
static void RtemsEventValEvents_Action_0( void )
{
  unsigned int i;

  for ( i = 0; i < 32; ++i ) {
    RtemsEventValEventConstant_Run( events[ i ], i );
    T_step( i ); /* Accounts for 32 test plan steps */
  }
}

/**
 * @brief Check that RTEMS_PENDING_EVENTS is a constant expression which
 *   evaluates to a value of zero.
 */
static void RtemsEventValEvents_Action_1( void )
{
  RTEMS_STATIC_ASSERT( RTEMS_PENDING_EVENTS == 0, PENDING_EVENTS );
}

/**
 * @brief Calculate the value of a bitwise or of all 32 event constants.
 */
static void RtemsEventValEvents_Action_2( void )
{
  rtems_event_set all;
  int             i;

  all = 0;

  for ( i = 0; i < 32; ++i ) {
    all |= events[ i ];
  }

  /*
   * Check that the value is equal to RTEMS_ALL_EVENTS.
   */
  T_step_eq_u32( 32, all, RTEMS_ALL_EVENTS );
}

/**
 * @brief Validate the Event Manager directive options.
 */
static void RtemsEventValEvents_Action_3( void )
{
  /* No action */

  /*
   * Check that RTEMS_EVENT_ALL is equal to zero.
   */
  T_step_eq_u32( 33, RTEMS_EVENT_ALL, 0 );

  /*
   * Check that RTEMS_EVENT_ANY is a power of two.
   */
  T_step_ne_u32( 34, RTEMS_EVENT_ANY, 0 );
  T_step_eq_u32( 35, RTEMS_EVENT_ANY & ( RTEMS_EVENT_ANY - 1 ), 0 );
}

/**
 * @fn void T_case_body_RtemsEventValEvents( void )
 */
T_TEST_CASE( RtemsEventValEvents )
{
  T_plan( 36 );

  RtemsEventValEvents_Action_0();
  RtemsEventValEvents_Action_1();
  RtemsEventValEvents_Action_2();
  RtemsEventValEvents_Action_3();
}

/** @} */
