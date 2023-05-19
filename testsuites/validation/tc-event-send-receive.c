/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsEventValSendReceive
 * @ingroup RtemsEventValSystemSendReceive
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

#include <rtems/rtems/eventimpl.h>
#include <rtems/rtems/tasksdata.h>
#include <rtems/score/statesimpl.h>
#include <rtems/score/threadimpl.h>

#include "tr-event-send-receive.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsEventValSendReceive spec:/rtems/event/val/send-receive
 *
 * @ingroup TestsuitesValidation0
 *
 * @brief Tests the rtems_event_send() and rtems_event_receive() directives.
 *
 * This test case performs the following actions:
 *
 * - Run the event send and receive tests for the application event set defined
 *   by spec:/rtems/event/req/send-receive.
 *
 * @{
 */

static rtems_status_code EventSend(
  rtems_id        id,
  rtems_event_set event_in
)
{
  return rtems_event_send( id, event_in );
}

static rtems_status_code EventReceive(
  rtems_id         event_in,
  rtems_option     option_set,
  rtems_interval   ticks,
  rtems_event_set *event_out
)
{
  return rtems_event_receive( event_in, option_set, ticks, event_out );
}

static rtems_event_set GetPendingEvents( Thread_Control *thread )
{
  RTEMS_API_Control *api;

  api = thread->API_Extensions[ THREAD_API_RTEMS ];
  return api->Event.pending_events;
}

/**
 * @brief Run the event send and receive tests for the application event set
 *   defined by spec:/rtems/event/req/send-receive.
 */
static void RtemsEventValSendReceive_Action_0( void )
{
  RtemsEventReqSendReceive_Run(
    EventSend,
    EventReceive,
    GetPendingEvents,
    THREAD_WAIT_CLASS_EVENT,
    STATES_WAITING_FOR_EVENT
  );
}

/**
 * @fn void T_case_body_RtemsEventValSendReceive( void )
 */
T_TEST_CASE( RtemsEventValSendReceive )
{
  RtemsEventValSendReceive_Action_0();
}

/** @} */

/**
 * @defgroup RtemsEventValSystemSendReceive \
 *   spec:/rtems/event/val/system-send-receive
 *
 * @ingroup TestsuitesValidation0
 *
 * @brief Tests the rtems_event_system_send() and rtems_event_system_receive()
 *   directives.
 *
 * This test case performs the following actions:
 *
 * - Run the event send and receive tests for the system event set defined by
 *   spec:/rtems/event/req/send-receive.
 *
 * @{
 */

static rtems_status_code EventSystemSend(
  rtems_id        id,
  rtems_event_set event_in
)
{
  return rtems_event_system_send( id, event_in );
}

static rtems_status_code EventSystemReceive(
  rtems_id         event_in,
  rtems_option     option_set,
  rtems_interval   ticks,
  rtems_event_set *event_out
)
{
  return rtems_event_system_receive(
    event_in,
    option_set,
    ticks,
    event_out
  );
}

static rtems_event_set GetPendingSystemEvents( Thread_Control *thread )
{
  RTEMS_API_Control *api;

  api = thread->API_Extensions[ THREAD_API_RTEMS ];
  return api->System_event.pending_events;
}

/**
 * @brief Run the event send and receive tests for the system event set defined
 *   by spec:/rtems/event/req/send-receive.
 */
static void RtemsEventValSystemSendReceive_Action_0( void )
{
  RtemsEventReqSendReceive_Run(
    EventSystemSend,
    EventSystemReceive,
    GetPendingSystemEvents,
    THREAD_WAIT_CLASS_SYSTEM_EVENT,
    STATES_WAITING_FOR_SYSTEM_EVENT
  );
}

/**
 * @fn void T_case_body_RtemsEventValSystemSendReceive( void )
 */
T_TEST_CASE( RtemsEventValSystemSendReceive )
{
  RtemsEventValSystemSendReceive_Action_0();
}

/** @} */
