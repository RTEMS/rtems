/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup TestsuitesModel0
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
 * Do not manually edit this file.  It is part of the RTEMS quality process
 * and was automatically generated.
 *
 * If you find something that needs to be fixed or worded better please
 * post a report to an RTEMS mailing list or raise a bug report:
 *
 * https://docs.rtems.org/branches/master/user/support/bugs.html
 *
 * For information on updating and regenerating please refer to:
 *
 * https://docs.rtems.org/branches/master/eng/req/howto.html
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/eventimpl.h>
#include <rtems/rtems/tasksdata.h>
#include <rtems/score/statesimpl.h>
#include <rtems/score/threadimpl.h>

#include "tr-event-mgr-model.h"

#include <rtems/test.h>

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
 * @fn void T_case_body_RtemsEventValSendReceive( void )
 */
T_TEST_CASE( RtemsModelEventsMgr0 )
{
  RtemsModelEventsMgr_Run0(
    EventSend,
    EventReceive,
    GetPendingEvents,
    THREAD_WAIT_CLASS_EVENT,
    STATES_WAITING_FOR_EVENT
  );
}

T_TEST_CASE( RtemsModelEventsMgr1 )
{
  RtemsModelEventsMgr_Run1(
    EventSend,
    EventReceive,
    GetPendingEvents,
    THREAD_WAIT_CLASS_EVENT,
    STATES_WAITING_FOR_EVENT
  );
}

T_TEST_CASE( RtemsModelEventsMgr2 )
{
  RtemsModelEventsMgr_Run2(
    EventSend,
    EventReceive,
    GetPendingEvents,
    THREAD_WAIT_CLASS_EVENT,
    STATES_WAITING_FOR_EVENT
  );
}

T_TEST_CASE( RtemsModelEventsMgr3 )
{
  RtemsModelEventsMgr_Run3(
    EventSend,
    EventReceive,
    GetPendingEvents,
    THREAD_WAIT_CLASS_EVENT,
    STATES_WAITING_FOR_EVENT
  );
}

T_TEST_CASE( RtemsModelEventsMgr4 )
{
  RtemsModelEventsMgr_Run4(
    EventSend,
    EventReceive,
    GetPendingEvents,
    THREAD_WAIT_CLASS_EVENT,
    STATES_WAITING_FOR_EVENT
  );
}

T_TEST_CASE( RtemsModelEventsMgr5 )
{
  RtemsModelEventsMgr_Run5(
    EventSend,
    EventReceive,
    GetPendingEvents,
    THREAD_WAIT_CLASS_EVENT,
    STATES_WAITING_FOR_EVENT
  );
}

T_TEST_CASE( RtemsModelEventsMgr6 )
{
  RtemsModelEventsMgr_Run6(
    EventSend,
    EventReceive,
    GetPendingEvents,
    THREAD_WAIT_CLASS_EVENT,
    STATES_WAITING_FOR_EVENT
  );
}

T_TEST_CASE( RtemsModelEventsMgr7 )
{
  RtemsModelEventsMgr_Run7(
    EventSend,
    EventReceive,
    GetPendingEvents,
    THREAD_WAIT_CLASS_EVENT,
    STATES_WAITING_FOR_EVENT
  );
}

T_TEST_CASE( RtemsModelEventsMgr8 )
{
  RtemsModelEventsMgr_Run8(
    EventSend,
    EventReceive,
    GetPendingEvents,
    THREAD_WAIT_CLASS_EVENT,
    STATES_WAITING_FOR_EVENT
  );
}

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
 * @fn void T_case_body_RtemsEventValSystemSendReceive( void )
 */
T_TEST_CASE( RtemsModelSystemEventsMgr0 )
{
  RtemsModelEventsMgr_Run0(
    EventSystemSend,
    EventSystemReceive,
    GetPendingSystemEvents,
    THREAD_WAIT_CLASS_SYSTEM_EVENT,
    STATES_WAITING_FOR_SYSTEM_EVENT
  );
}

T_TEST_CASE( RtemsModelSystemEventsMgr1 )
{
  RtemsModelEventsMgr_Run1(
    EventSystemSend,
    EventSystemReceive,
    GetPendingSystemEvents,
    THREAD_WAIT_CLASS_SYSTEM_EVENT,
    STATES_WAITING_FOR_SYSTEM_EVENT
  );
}

T_TEST_CASE( RtemsModelSystemEventsMgr2 )
{
  RtemsModelEventsMgr_Run2(
    EventSystemSend,
    EventSystemReceive,
    GetPendingSystemEvents,
    THREAD_WAIT_CLASS_SYSTEM_EVENT,
    STATES_WAITING_FOR_SYSTEM_EVENT
  );
}

T_TEST_CASE( RtemsModelSystemEventsMgr3 )
{
  RtemsModelEventsMgr_Run3(
    EventSystemSend,
    EventSystemReceive,
    GetPendingSystemEvents,
    THREAD_WAIT_CLASS_SYSTEM_EVENT,
    STATES_WAITING_FOR_SYSTEM_EVENT
  );
}

T_TEST_CASE( RtemsModelSystemEventsMgr4 )
{
  RtemsModelEventsMgr_Run4(
    EventSystemSend,
    EventSystemReceive,
    GetPendingSystemEvents,
    THREAD_WAIT_CLASS_SYSTEM_EVENT,
    STATES_WAITING_FOR_SYSTEM_EVENT
  );
}

T_TEST_CASE( RtemsModelSystemEventsMgr5 )
{
  RtemsModelEventsMgr_Run5(
    EventSystemSend,
    EventSystemReceive,
    GetPendingSystemEvents,
    THREAD_WAIT_CLASS_SYSTEM_EVENT,
    STATES_WAITING_FOR_SYSTEM_EVENT
  );
}

T_TEST_CASE( RtemsModelSystemEventsMgr6 )
{
  RtemsModelEventsMgr_Run6(
    EventSystemSend,
    EventSystemReceive,
    GetPendingSystemEvents,
    THREAD_WAIT_CLASS_SYSTEM_EVENT,
    STATES_WAITING_FOR_SYSTEM_EVENT
  );
}

T_TEST_CASE( RtemsModelSystemEventsMgr7 )
{
  RtemsModelEventsMgr_Run7(
    EventSystemSend,
    EventSystemReceive,
    GetPendingSystemEvents,
    THREAD_WAIT_CLASS_SYSTEM_EVENT,
    STATES_WAITING_FOR_SYSTEM_EVENT
  );
}

T_TEST_CASE( RtemsModelSystemEventsMgr8 )
{
  RtemsModelEventsMgr_Run8(
    EventSystemSend,
    EventSystemReceive,
    GetPendingSystemEvents,
    THREAD_WAIT_CLASS_SYSTEM_EVENT,
    STATES_WAITING_FOR_SYSTEM_EVENT
  );
}
