/**
 * @file
 *
 * @ingroup RTEMSImplClassicSignal
 *
 * @brief This source file contains the implementation of
 *   rtems_signal_send().
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
#include <rtems/rtems/tasksdata.h>
#include <rtems/score/threaddispatch.h>
#include <rtems/score/threadimpl.h>

#include <rtems/sysinit.h>

static void _Signal_Action_handler(
  Thread_Control   *executing,
  Thread_Action    *action,
  ISR_lock_Context *lock_context
)
{
  RTEMS_API_Control *api;
  ASR_Information   *asr;
  rtems_signal_set   signal_set;
  rtems_mode      prev_mode;

  (void) action;

  api = executing->API_Extensions[ THREAD_API_RTEMS ];
  asr = &api->Signal;

  /* Get and clear the pending signals */
  signal_set = asr->signals_pending;
  _Assert( signal_set != 0 );
  asr->signals_pending = 0;

  _Thread_State_release( executing, lock_context );

  rtems_task_mode( asr->mode_set, RTEMS_ALL_MODE_MASKS, &prev_mode );

  /* Call the ASR handler in the ASR processing mode */
  ( *asr->handler )( signal_set );

  rtems_task_mode( prev_mode, RTEMS_ALL_MODE_MASKS, &prev_mode );

  _Thread_State_acquire( executing, lock_context );
}

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

  if ( asr->handler == NULL ) {
    _Thread_State_release( the_thread, &lock_context );
    return RTEMS_NOT_DEFINED;
  }

  /* Make the signals of the set pending */
  asr->signals_pending |= signal_set;

  if ( asr->is_enabled ) {
    Per_CPU_Control *cpu_self;

    _Thread_Add_post_switch_action(
      the_thread,
      &api->Signal_action,
      _Signal_Action_handler
    );
    cpu_self = _Thread_Dispatch_disable_critical( &lock_context );
    _Thread_State_release( the_thread, &lock_context );
    _Thread_Dispatch_enable( cpu_self );
  } else {
    _Thread_State_release( the_thread, &lock_context );
  }

  return RTEMS_SUCCESSFUL;
}

#if defined(RTEMS_MULTIPROCESSING)
static void _Signal_MP_Initialize( void )
{
  _MPCI_Register_packet_processor(
    MP_PACKET_SIGNAL,
    _Signal_MP_Process_packet
  );
}

RTEMS_SYSINIT_ITEM(
  _Signal_MP_Initialize,
  RTEMS_SYSINIT_CLASSIC_SIGNAL_MP,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
#endif
