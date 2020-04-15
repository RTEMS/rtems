/**
 *  @file
 *
 *  @brief RTEMS Catch Signal
 *  @ingroup ClassicSignal
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/sysinit.h>
#include <rtems/rtems/signalimpl.h>
#include <rtems/rtems/asrimpl.h>
#include <rtems/rtems/tasksdata.h>
#include <rtems/score/assert.h>
#include <rtems/score/threadimpl.h>

RTEMS_STATIC_ASSERT( RTEMS_DEFAULT_MODES == 0, _ASR_Create_mode_set );

void _Signal_Action_handler(
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

  /*
   *  Signal Processing
   */

  api = executing->API_Extensions[ THREAD_API_RTEMS ];
  asr = &api->Signal;
  signal_set = _ASR_Get_posted_signals( asr );

  _Thread_State_release( executing, lock_context );

  if ( signal_set == 0 ) {
    return;
  }

  asr->nest_level += 1;
  rtems_task_mode( asr->mode_set, RTEMS_ALL_MODE_MASKS, &prev_mode );

  (*asr->handler)( signal_set );

  asr->nest_level -= 1;
  rtems_task_mode( prev_mode, RTEMS_ALL_MODE_MASKS, &prev_mode );
}

rtems_status_code rtems_signal_catch(
  rtems_asr_entry   asr_handler,
  rtems_mode        mode_set
)
{
  Thread_Control     *executing;
  RTEMS_API_Control  *api;
  ASR_Information    *asr;
  ISR_lock_Context    lock_context;

  executing = _Thread_State_acquire_for_executing( &lock_context );
  api = executing->API_Extensions[ THREAD_API_RTEMS ];
  asr = &api->Signal;

  if ( !_ASR_Is_null_handler( asr_handler ) ) {
    asr->mode_set = mode_set;
    asr->handler = asr_handler;
  } else {
    _ASR_Initialize( asr );
  }

  _Thread_State_release( executing, &lock_context );
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
