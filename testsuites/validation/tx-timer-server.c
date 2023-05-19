/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestSuitesValidation
 *
 * @brief This source file contains the definition of DeleteTimerServer().
 */

/*
 * Copyright (C) 2021 embedded brains GmbH & Co. KG
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

#include "tx-support.h"

#include <rtems/test.h>
#include <rtems/rtems/timerimpl.h>
#include <rtems/score/todimpl.h>

rtems_id GetTimerServerTaskId( void )
{
  if ( _Timer_server == NULL ) {
    return RTEMS_INVALID_ID;
  }
  return _Timer_server->server_id;
}

bool DeleteTimerServer( void )
{
  Timer_server_Control *server;

  server = _Timer_server;

  if ( server == NULL ) {
    return false;
  }

  DeleteTask( server->server_id );
  _ISR_lock_Destroy( &server->Lock );
  T_true( _Chain_Is_empty( &server->Pending ) );
  _Timer_server = NULL;

  return true;
}

Timer_Classes GetTimerClass( rtems_id id )
{
  /* This code is derived from rtems_timer_get_information() */
  Timer_Classes result = TIMER_DORMANT;
  Timer_Control *the_timer;
  ISR_lock_Context lock_context;
  Per_CPU_Control *cpu;

  the_timer = _Timer_Get( id, &lock_context );
  if ( the_timer != NULL ) {
    cpu = _Timer_Acquire_critical( the_timer, &lock_context );
    result = the_timer->the_class;
    _Timer_Release( cpu, &lock_context );
  }

  return result;
}

void GetTimerSchedulingData(
  rtems_id id,
  Timer_Scheduling_Data *data
)
{
  /* This code is derived from rtems_timer_get_information() */
  Timer_Control *the_timer;
  ISR_lock_Context lock_context;
  Per_CPU_Control *cpu;

  if ( data == NULL ) {
    return;
  }

  the_timer = _Timer_Get( id, &lock_context );
  if ( the_timer != NULL ) {
    cpu = _Timer_Acquire_critical( the_timer, &lock_context );
    data->routine   = the_timer->routine;
    data->user_data = the_timer->user_data;
    data->interval  = the_timer->initial;
    _Timer_Release( cpu, &lock_context );
  }
}

Timer_States GetTimerState( rtems_id id )
{
  /* This code is derived from rtems_timer_cancel() and _timer_cancel() */
  Timer_States          result = TIMER_INVALID;
  Timer_Control        *the_timer;
  ISR_lock_Context      lock_context;
  Per_CPU_Control      *cpu;
  Timer_Classes         the_class;
  Timer_server_Control *timer_server = _Timer_server;
  ISR_lock_Context      lock_context_server;

  the_timer = _Timer_Get( id, &lock_context );
  if ( the_timer != NULL ) {
    result = TIMER_INACTIVE;
    cpu = _Timer_Acquire_critical( the_timer, &lock_context );
    the_class = the_timer->the_class;

    if ( _Watchdog_Is_scheduled( &the_timer->Ticker ) ) {
      result = TIMER_SCHEDULED;
    } else if ( _Timer_Is_on_task_class( the_class ) ) {
      _Assert( timer_server != NULL );
      _Timer_server_Acquire_critical( timer_server, &lock_context_server );
      if ( _Watchdog_Get_state( &the_timer->Ticker ) == WATCHDOG_PENDING ) {
        result = TIMER_PENDING;
      }
    _Timer_server_Release_critical( timer_server, &lock_context_server );
    }
    _Timer_Release( cpu, &lock_context );
  }

  return result;
}

void UnsetClock( void )
{
  _TOD.is_set = false;
}
