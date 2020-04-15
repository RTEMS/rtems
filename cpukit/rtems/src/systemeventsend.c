/**
 * @file
 *
 * @ingroup ClassicEventSystem
 *
 * @brief rtems_event_system_send() implementation.
 */

/*
 * Copyright (c) 2012, 2016 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/eventimpl.h>
#include <rtems/rtems/tasksdata.h>
#include <rtems/score/threadimpl.h>

rtems_status_code rtems_event_system_send(
  rtems_id        id,
  rtems_event_set event_in
)
{
  Thread_Control    *the_thread;
  RTEMS_API_Control *api;
  ISR_lock_Context   lock_context;

  the_thread = _Thread_Get( id, &lock_context );

  if ( the_thread == NULL ) {
#if defined(RTEMS_MULTIPROCESSING)
    if ( _Thread_MP_Is_remote( id ) ) {
      return RTEMS_ILLEGAL_ON_REMOTE_OBJECT;
    }
#endif

    return RTEMS_INVALID_ID;
  }

  api = the_thread->API_Extensions[ THREAD_API_RTEMS ];
  return _Event_Surrender(
    the_thread,
    event_in,
    &api->System_event,
    THREAD_WAIT_CLASS_SYSTEM_EVENT,
    &lock_context
  );
}
