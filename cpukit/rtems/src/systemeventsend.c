/**
 * @file
 *
 * @ingroup ClassicEventSystem
 *
 * @brief rtems_event_system_send() implementation.
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
#include <rtems/score/threadimpl.h>

rtems_status_code rtems_event_system_send(
  rtems_id        id,
  rtems_event_set event_in
)
{
  rtems_status_code  sc;
  Thread_Control    *thread;
  Objects_Locations  location;
  RTEMS_API_Control *api;

  thread = _Thread_Get( id, &location );
  switch ( location ) {
    case OBJECTS_LOCAL:
      api = thread->API_Extensions[ THREAD_API_RTEMS ];
      _Event_Surrender(
        thread,
        event_in,
        &api->System_event,
        &_System_event_Sync_state,
        STATES_WAITING_FOR_SYSTEM_EVENT
      );
      _Objects_Put( &thread->Object );
      sc = RTEMS_SUCCESSFUL;
      break;
#ifdef RTEMS_MULTIPROCESSING
    case OBJECTS_REMOTE:
      _Thread_Dispatch();
      sc = RTEMS_ILLEGAL_ON_REMOTE_OBJECT;
      break;
#endif
    default:
      sc = RTEMS_INVALID_ID;
      break;
  }

  return sc;
}
