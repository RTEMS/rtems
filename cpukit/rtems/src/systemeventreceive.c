/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicEvent
 *
 * @brief This source file contains the implementation of
 *   rtems_event_system_receive().
 */

/*
 * Copyright (c) 2012 embedded brains GmbH & Co. KG
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

#include <rtems/rtems/eventimpl.h>
#include <rtems/rtems/tasksdata.h>
#include <rtems/score/statesimpl.h>
#include <rtems/score/threadimpl.h>

rtems_status_code rtems_event_system_receive(
  rtems_event_set  event_in,
  rtems_option     option_set,
  rtems_interval   ticks,
  rtems_event_set *event_out
)
{
  rtems_status_code sc;

  if ( event_out != NULL ) {
    ISR_lock_Context   lock_context;
    Thread_Control    *executing;
    RTEMS_API_Control *api;
    Event_Control     *event;

    executing = _Thread_Wait_acquire_default_for_executing( &lock_context );
    api = executing->API_Extensions[ THREAD_API_RTEMS ];
    event = &api->System_event;

    if ( !_Event_sets_Is_empty( event_in ) ) {
      sc = _Event_Seize(
        event_in,
        option_set,
        ticks,
        event_out,
        executing,
        event,
        THREAD_WAIT_CLASS_SYSTEM_EVENT,
        STATES_WAITING_FOR_SYSTEM_EVENT,
        &lock_context
      );
    } else {
      *event_out = event->pending_events;
      _Thread_Wait_release_default( executing, &lock_context );
      sc = RTEMS_SUCCESSFUL;
    }
  } else {
    sc = RTEMS_INVALID_ADDRESS;
  }

  return sc;
}
