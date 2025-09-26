/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicTimer
 *
 * @brief This source file contains the implementation of
 *   rtems_timer_initiate_server().
 */

/*  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (C) 2009, 2017 embedded brains GmbH & Co. KG
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

#include <rtems.h>
#include <rtems/rtems/timerimpl.h>
#include <rtems/rtems/tasksimpl.h>
#include <rtems/score/todimpl.h>

static Timer_server_Control _Timer_server_Default;

static void _Timer_server_Acquire(
  Timer_server_Control *ts,
  ISR_lock_Context     *lock_context
)
{
  (void) ts;

  _ISR_lock_ISR_disable_and_acquire( &ts->Lock, lock_context );
}

static void _Timer_server_Release(
  Timer_server_Control *ts,
  ISR_lock_Context     *lock_context
)
{
  (void) ts;

  _ISR_lock_Release_and_ISR_enable( &ts->Lock, lock_context );
}

void _Timer_server_Routine_adaptor( Watchdog_Control *the_watchdog )
{
  Timer_Control        *the_timer;
  ISR_lock_Context      lock_context;
  Per_CPU_Control      *cpu;
  Timer_server_Control *ts;
  bool                  wakeup;

  ts = _Timer_server;
  _Assert( ts != NULL );
  the_timer = RTEMS_CONTAINER_OF( the_watchdog, Timer_Control, Ticker );

  _Timer_server_Acquire( ts, &lock_context );

  _Assert( _Watchdog_Get_state( &the_timer->Ticker ) == WATCHDOG_INACTIVE );
  _Watchdog_Set_state( &the_timer->Ticker, WATCHDOG_PENDING );
  cpu = _Watchdog_Get_CPU( &the_timer->Ticker );
  the_timer->stop_time = _Timer_Get_CPU_ticks( cpu );
  wakeup = _Chain_Is_empty( &ts->Pending );
  _Chain_Append_unprotected( &ts->Pending, &the_timer->Ticker.Node.Chain );

  _Timer_server_Release( ts, &lock_context );

  if ( wakeup ) {
    (void) rtems_event_system_send( ts->server_id, RTEMS_EVENT_SYSTEM_SERVER );
  }
}

/**
 *  @brief Timer server body.
 *
 *  This is the server for task based timers.  This task executes whenever a
 *  task-based timer should fire.  It services both "after" and "when" timers.
 *  It is not created automatically but must be created explicitly by the
 *  application before task-based timers may be initiated.  The parameter
 *  @a arg points to the corresponding timer server control block.
 */
static rtems_task _Timer_server_Body(
  rtems_task_argument arg
)
{
  Timer_server_Control *ts = (Timer_server_Control *) arg;
#if defined(RTEMS_SCORE_THREAD_ENABLE_RESOURCE_COUNT)
  Thread_Control *executing = _Thread_Get_executing();
#endif

  while ( true ) {
    ISR_lock_Context  lock_context;
    rtems_event_set   events;

    _Timer_server_Acquire( ts, &lock_context );

    while ( true ) {
      Watchdog_Control                  *the_watchdog;
      Timer_Control                     *the_timer;
      rtems_timer_service_routine_entry  routine;
      Objects_Id                         id;
      void                              *user_data;

      the_watchdog = (Watchdog_Control *) _Chain_Get_unprotected( &ts->Pending );
      if ( the_watchdog == NULL ) {
        break;
      }

      _Assert( _Watchdog_Get_state( the_watchdog ) == WATCHDOG_PENDING );
      _Watchdog_Set_state( the_watchdog, WATCHDOG_INACTIVE );
      the_timer = RTEMS_CONTAINER_OF( the_watchdog, Timer_Control, Ticker );
      routine = the_timer->routine;
      id = the_timer->Object.id;
      user_data = the_timer->user_data;

      _Timer_server_Release( ts, &lock_context );

      ( *routine )( id, user_data );
#if defined(RTEMS_SCORE_THREAD_ENABLE_RESOURCE_COUNT)
      _Assert( !_Thread_Owns_resources( executing ) );
#endif

      _Timer_server_Acquire( ts, &lock_context );
    }

    _Timer_server_Release( ts, &lock_context );

    (void) rtems_event_system_receive(
      RTEMS_EVENT_SYSTEM_SERVER,
      RTEMS_EVENT_ALL | RTEMS_WAIT,
      RTEMS_NO_TIMEOUT,
      &events
    );
  }
}

static rtems_status_code _Timer_server_Initiate(
  rtems_task_priority priority,
  size_t              stack_size,
  rtems_attribute     attribute_set
)
{
  rtems_status_code     status;
  rtems_id              id;
  Timer_server_Control *ts;

  /*
   *  Just to make sure this is only called once.
   */
  if ( _Timer_server != NULL ) {
    return RTEMS_INCORRECT_STATE;
  }

  if ( priority == RTEMS_TIMER_SERVER_DEFAULT_PRIORITY ) {
    priority = PRIORITY_MINIMUM;
  }

  /*
   *  Create the Timer Server with the name the name of "TIME".  The attribute
   *  RTEMS_SYSTEM_TASK allows us to set a priority to 0 which will makes it
   *  higher than any other task in the system.  It can be viewed as a low
   *  priority interrupt.  It is also always NO_PREEMPT so it looks like
   *  an interrupt to other tasks.
   *
   *  We allow the user to override the default priority because the Timer
   *  Server can invoke TSRs which must adhere to language run-time or
   *  other library rules.  For example, if using a TSR written in Ada the
   *  Server should run at the same priority as the priority Ada task.
   *  Otherwise, the priority ceiling for the mutex used to protect the
   *  GNAT run-time is violated.
   */
  status = rtems_task_create(
    rtems_build_name('T','I','M','E'),
    priority,
    stack_size,
#ifdef RTEMS_SMP
    RTEMS_DEFAULT_MODES, /* no preempt is not recommended for SMP */
#else
    RTEMS_NO_PREEMPT,    /* no preempt is like an interrupt */
#endif
    /* user may want floating point but we need */
    /*   system task specified for 0 priority */
    attribute_set | RTEMS_SYSTEM_TASK,
    &id
  );
  if (status != RTEMS_SUCCESSFUL) {
    return status;
  }

  /*
   *  Do all the data structure initialization before starting the
   *  Timer Server so we do not have to have a critical section.
   */

  ts = &_Timer_server_Default;
  _ISR_lock_Initialize( &ts->Lock, "Timer Server" );
  _Chain_Initialize_empty( &ts->Pending );
  ts->server_id = id;

  /*
   * The default timer server is now available.
   */
  _Timer_server = ts;

  /*
   *  Start the timer server
   */
  status = rtems_task_start(
    id,
    _Timer_server_Body,
    (rtems_task_argument) ts
  );
  _Assert( status == RTEMS_SUCCESSFUL );

  return status;
}

rtems_status_code rtems_timer_initiate_server(
  rtems_task_priority priority,
  size_t              stack_size,
  rtems_attribute     attribute_set
)
{
  rtems_status_code status;

  _Objects_Allocator_lock();
  status = _Timer_server_Initiate( priority, stack_size, attribute_set );
  _Objects_Allocator_unlock();

  return status;
}
