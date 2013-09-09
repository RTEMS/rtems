/**
 * @file
 *
 * @ingroup ClassicEventSystem
 *
 * @brief rtems_event_system_receive() implementation.
 */

/*
 * Copyright (c) 2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/rtems/eventimpl.h>
#include <rtems/rtems/tasks.h>
#include <rtems/score/statesimpl.h>
#include <rtems/score/threaddispatch.h>

rtems_status_code rtems_event_system_receive(
  rtems_event_set  event_in,
  rtems_option     option_set,
  rtems_interval   ticks,
  rtems_event_set *event_out
)
{
  rtems_status_code sc;

  if ( event_out != NULL ) {
    Thread_Control    *executing = _Thread_Get_executing();
    RTEMS_API_Control *api = executing->API_Extensions[ THREAD_API_RTEMS ];
    Event_Control     *event = &api->System_event;

    if ( !_Event_sets_Is_empty( event_in ) ) {
      _Thread_Disable_dispatch();
      _Event_Seize(
        event_in,
        option_set,
        ticks,
        event_out,
        executing,
        event,
        &_System_event_Sync_state,
        STATES_WAITING_FOR_SYSTEM_EVENT
      );
      _Thread_Enable_dispatch();

      sc = executing->Wait.return_code;
    } else {
      *event_out = event->pending_events;
      sc = RTEMS_SUCCESSFUL;
    }
  } else {
    sc = RTEMS_INVALID_ADDRESS;
  }

  return sc;
}
