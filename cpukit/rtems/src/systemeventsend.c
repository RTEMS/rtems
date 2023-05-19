/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicEvent
 *
 * @brief This source file contains the implementation of
 *   rtems_event_system_send().
 */

/*
 * Copyright (C) 2012, 2016 embedded brains GmbH & Co. KG
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
