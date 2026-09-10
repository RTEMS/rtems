/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicTimer
 *
 * @brief This source file contains the implementation of
 *   rtems_timer_delete().
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
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

#include <rtems/rtems/timerimpl.h>
#include <rtems/score/threadimpl.h>

/*
 * The timer server holds the ticker of a task based timer between the tickle
 * phase which took it out of its collection and the return of the service
 * routine.  WATCHDOG_INACTIVE names that situation and two others, so the
 * member of the server tells them apart.  The wait gives the processor away,
 * so the function releases the object allocator mutex and obtains it again.
 */
static void _Timer_Wait_for_server( void )
{
  Timer_server_Control *ts;
  Thread_queue_Context  queue_context;

  ts = _Timer_server;

  if ( ts == NULL ) {
    return;
  }

  /*
   * A service routine of the server which deletes a timer would wait for the
   * phase which it runs in itself.
   */
  if ( _Thread_Get_executing()->Object.id == ts->server_id ) {
    return;
  }

  _Objects_Allocator_unlock();

  _Thread_queue_Context_initialize( &queue_context );
  _Condition_Acquire( &ts->Condition, &queue_context );

  if ( ts->tickling ) {
    _Condition_Enqueue( &ts->Condition, &queue_context );
  } else {
    _Condition_Release( &ts->Condition, &queue_context );
  }

  _Objects_Allocator_lock();
}

rtems_status_code rtems_timer_delete( rtems_id id )
{
  Timer_Control    *the_timer;
  ISR_lock_Context  lock_context;
  Thread_Life_state previous_thread_life_state;

  /*
   * The directive releases the object allocator mutex for the wait of the
   * timer server, and that release ends the thread life protection which the
   * mutex provides.  A delete of the calling task in the wait would leave the
   * timer closed and unallocated for the life of the system.
   */
  previous_thread_life_state = _Thread_Set_life_protection(
    THREAD_LIFE_PROTECTED
  );
  _Objects_Allocator_lock();

  the_timer = _Timer_Get( id, &lock_context );
  if ( the_timer != NULL ) {
    Per_CPU_Control *cpu;

    _Objects_Close( &_Timer_Information, &the_timer->Object );
    cpu = _Timer_Acquire_critical( the_timer, &lock_context );
    _Timer_Cancel( cpu, the_timer );
    _Timer_Release( cpu, &lock_context );

    _Watchdog_Wait_for_service_stop( &the_timer->Ticker );

    /*
     * The adaptor of the timer server puts the ticker on the chain of the
     * server during the wait above, and the cancel before that wait could not
     * take it off.
     */
    _ISR_lock_ISR_disable( &lock_context );
    cpu = _Timer_Acquire_critical( the_timer, &lock_context );
    _Timer_Cancel( cpu, the_timer );
    _Timer_Release( cpu, &lock_context );

    _Timer_Wait_for_server();

    _Timer_Free( the_timer );
    _Objects_Allocator_unlock();
    _Thread_Set_life_protection( previous_thread_life_state );
    return RTEMS_SUCCESSFUL;
  }

  _Objects_Allocator_unlock();
  _Thread_Set_life_protection( previous_thread_life_state );
  return RTEMS_INVALID_ID;
}
