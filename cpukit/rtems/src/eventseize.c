/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicEvent
 *
 * @brief This source file contains the implementation of
 *   _Event_Seize() and the Event Manager multiprocessing (MP) support system
 *   initialization.
 */

/*
 *  COPYRIGHT (c) 1989-2008.
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

#include <rtems/sysinit.h>
#include <rtems/rtems/eventimpl.h>
#include <rtems/rtems/optionsimpl.h>
#include <rtems/rtems/statusimpl.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/watchdogimpl.h>

rtems_status_code _Event_Seize(
  rtems_event_set    event_in,
  rtems_option       option_set,
  rtems_interval     ticks,
  rtems_event_set   *event_out,
  Thread_Control    *executing,
  Event_Control     *event,
  Thread_Wait_flags  wait_class,
  States_Control     block_state,
  ISR_lock_Context  *lock_context
)
{
  rtems_event_set    seized_events;
  rtems_event_set    pending_events;
  bool               success;
  Thread_Wait_flags  intend_to_block;
  Per_CPU_Control   *cpu_self;

  pending_events = event->pending_events;
  seized_events  = _Event_sets_Get( pending_events, event_in );

  if ( !_Event_sets_Is_empty( seized_events ) &&
       (seized_events == event_in || _Options_Is_any( option_set )) ) {
    event->pending_events =
      _Event_sets_Clear( pending_events, seized_events );
    _Thread_Wait_release_default( executing, lock_context );
    *event_out = seized_events;
    return RTEMS_SUCCESSFUL;
  }

  if ( _Options_Is_no_wait( option_set ) ) {
    _Thread_Wait_release_default( executing, lock_context );
    *event_out = seized_events;
    return RTEMS_UNSATISFIED;
  }

  intend_to_block = wait_class | THREAD_WAIT_STATE_INTEND_TO_BLOCK;

  /*
   *  Note what we are waiting for BEFORE we enter the critical section.
   *  The interrupt critical section management code needs this to be
   *  set properly when we are marked as in the event critical section.
   *
   *  NOTE: Since interrupts are disabled, this isn't that much of an
   *        issue but better safe than sorry.
   */
  executing->Wait.return_code     = STATUS_SUCCESSFUL;
  executing->Wait.option          = option_set;
  executing->Wait.count           = event_in;
  executing->Wait.return_argument = event_out;
  _Thread_Wait_flags_set( executing, intend_to_block );

  cpu_self = _Thread_Dispatch_disable_critical( lock_context );
  _Thread_Wait_release_default( executing, lock_context );

  if ( ticks ) {
    _Thread_Add_timeout_ticks( executing, cpu_self, ticks );
  }

  _Thread_Set_state( executing, block_state );

  success = _Thread_Wait_flags_try_change_acquire(
    executing,
    intend_to_block,
    wait_class | THREAD_WAIT_STATE_BLOCKED
  );
  if ( !success ) {
    _Thread_Timer_remove( executing );
    _Thread_Unblock( executing );
  }

  _Thread_Dispatch_direct( cpu_self );
  return _Status_Get_after_wait( executing );
}

#if defined(RTEMS_MULTIPROCESSING)
static void _Event_MP_Initialize( void )
{
  _MPCI_Register_packet_processor( MP_PACKET_EVENT, _Event_MP_Process_packet );
}

RTEMS_SYSINIT_ITEM(
  _Event_MP_Initialize,
  RTEMS_SYSINIT_CLASSIC_EVENT_MP,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
#endif
