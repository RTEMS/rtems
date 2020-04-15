/**
 *  @file
 *
 *  @brief RTEMS Send Signal
 *  @ingroup ClassicSignal
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/signalimpl.h>
#include <rtems/rtems/asrimpl.h>
#include <rtems/rtems/tasksdata.h>
#include <rtems/score/threaddispatch.h>
#include <rtems/score/threadimpl.h>

rtems_status_code rtems_signal_send(
  rtems_id          id,
  rtems_signal_set  signal_set
)
{
  Thread_Control    *the_thread;
  ISR_lock_Context   lock_context;
  RTEMS_API_Control *api;
  ASR_Information   *asr;

  if ( signal_set == 0 ) {
    return RTEMS_INVALID_NUMBER;
  }

  the_thread = _Thread_Get( id, &lock_context );

  if ( the_thread == NULL ) {
#if defined(RTEMS_MULTIPROCESSING)
    return _Signal_MP_Send( id, signal_set );
#else
    return RTEMS_INVALID_ID;
#endif
  }

  api = the_thread->API_Extensions[ THREAD_API_RTEMS ];
  asr = &api->Signal;

  _Thread_State_acquire_critical( the_thread, &lock_context );

  if ( _ASR_Is_null_handler( asr->handler ) ) {
    _Thread_State_release( the_thread, &lock_context );
    return RTEMS_NOT_DEFINED;
  }

  if ( asr->is_enabled ) {
    Per_CPU_Control *cpu_self;

    _ASR_Post_signals( signal_set, &asr->signals_posted );
    _Thread_Add_post_switch_action(
      the_thread,
      &api->Signal_action,
      _Signal_Action_handler
    );
    cpu_self = _Thread_Dispatch_disable_critical( &lock_context );
    _Thread_State_release( the_thread, &lock_context );
    _Thread_Dispatch_enable( cpu_self );
  } else {
    _ASR_Post_signals( signal_set, &asr->signals_pending );
    _Thread_State_release( the_thread, &lock_context );
  }

  return RTEMS_SUCCESSFUL;
}
