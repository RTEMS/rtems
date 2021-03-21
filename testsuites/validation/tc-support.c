/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestSuites
 *
 * @brief This source file contains the definition of DeleteTask(),
 *   DoCreateTask(), GetMode(), GetPriority(), GetSelfPriority(),
 *   ReceiveAnyEvents(), RestoreRunnerASR(), RestoreRunnerMode(),
 *   RestoreRunnerPriority(), SendEvents(), SetMode(), SetSelfPriority(),
 *   SetPriority(), and StartTask().
 */

/*
 * Copyright (C) 2021 embedded brains GmbH (http://www.embedded-brains.de)
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

#include "tc-support.h"

#include <rtems/test.h>

rtems_id DoCreateTask( rtems_name name, rtems_task_priority priority )
{
  rtems_status_code sc;
  rtems_id          id;

  sc = rtems_task_create(
    name,
    priority,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  T_assert_rsc_success( sc );

  return id;
}

void StartTask( rtems_id id, rtems_task_entry entry, void *arg )
{
  rtems_status_code sc;

  sc = rtems_task_start( id, entry, (rtems_task_argument) arg);
  T_assert_rsc_success( sc );
}

void DeleteTask( rtems_id id )
{
  if ( id != 0 ) {
    rtems_status_code sc;

    sc = rtems_task_delete( id );
    T_rsc_success( sc );
  }
}

rtems_event_set ReceiveAnyEvents( void )
{
  rtems_status_code sc;
  rtems_event_set   events;

  events = 0;
  sc = rtems_event_receive(
    RTEMS_ALL_EVENTS,
    RTEMS_EVENT_ANY | RTEMS_WAIT,
    RTEMS_NO_TIMEOUT,
    &events
  );
  T_rsc_success( sc );

  return events;
}

void SendEvents( rtems_id id, rtems_event_set events )
{
  rtems_status_code sc;

  sc = rtems_event_send( id, events );
  T_rsc_success( sc );
}

rtems_mode GetMode( void )
{
  return SetMode( RTEMS_DEFAULT_MODES, RTEMS_CURRENT_MODE );
}

rtems_mode SetMode( rtems_mode set, rtems_mode mask )
{
  rtems_status_code sc;
  rtems_mode        previous;

  sc = rtems_task_mode( set, mask, &previous );
  T_rsc_success( sc );

  return previous;
}

rtems_task_priority GetPriority( rtems_id id )
{
  return SetPriority( id, RTEMS_CURRENT_PRIORITY );
}

rtems_task_priority SetPriority( rtems_id id, rtems_task_priority priority )
{
  rtems_status_code   sc;
  rtems_task_priority previous;

  sc = rtems_task_set_priority( id, priority, &previous );
  T_rsc_success( sc );

  return previous;
}

rtems_task_priority GetSelfPriority( void )
{
  return SetPriority( RTEMS_SELF, RTEMS_CURRENT_PRIORITY );
}

rtems_task_priority SetSelfPriority( rtems_task_priority priority )
{
  return SetPriority( RTEMS_SELF, priority );
}

void RestoreRunnerASR( void )
{
  rtems_status_code sc;

  sc = rtems_signal_catch( NULL, RTEMS_DEFAULT_MODES );
  T_rsc_success( sc );
}

void RestoreRunnerMode( void )
{
  rtems_status_code sc;
  rtems_mode        mode;

  sc = rtems_task_mode( RTEMS_DEFAULT_MODES, RTEMS_ALL_MODE_MASKS, &mode );
  T_rsc_success( sc );
}

void RestoreRunnerPriority( void )
{
  SetSelfPriority( PRIO_ULTRA_HIGH );
}
