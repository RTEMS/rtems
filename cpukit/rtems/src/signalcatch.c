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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/sysinit.h>
#include <rtems/rtems/signalimpl.h>
#include <rtems/rtems/asrimpl.h>
#include <rtems/rtems/tasks.h>
#include <rtems/score/isrlevel.h>
#include <rtems/score/threaddispatch.h>
#include <rtems/score/threadimpl.h>

void _Signal_Action_handler(
  Thread_Control  *executing,
  Thread_Action   *action,
  Per_CPU_Control *cpu,
  ISR_Level        level
)
{
  RTEMS_API_Control *api;
  ASR_Information   *asr;
  rtems_signal_set   signal_set;
  Modes_Control      prev_mode;

  (void) action;
  _Thread_Action_release_and_ISR_enable( cpu, level );

  api = executing->API_Extensions[ THREAD_API_RTEMS ];
  if ( !api )
    return;

  /*
   *  Signal Processing
   */

  asr = &api->Signal;
  signal_set = _ASR_Get_posted_signals( asr );

  if ( !signal_set ) /* similar to _ASR_Are_signals_pending( asr ) */
    return;

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

  executing = _Thread_Get_executing();
  api = (RTEMS_API_Control*)executing->API_Extensions[ THREAD_API_RTEMS ];
  asr = &api->Signal;

  _ASR_Acquire( asr, &lock_context );

  if ( !_ASR_Is_null_handler( asr_handler ) ) {
    asr->mode_set = mode_set;
    asr->handler = asr_handler;
  } else {
    _ASR_Initialize( asr );
  }

  _ASR_Release( asr, &lock_context );
  return RTEMS_SUCCESSFUL;
}

#if defined(RTEMS_MULTIPROCESSING)
static void _Signal_Manager_initialization( void )
{
  _MPCI_Register_packet_processor(
    MP_PACKET_SIGNAL,
    _Signal_MP_Process_packet
  );
}

RTEMS_SYSINIT_ITEM(
  _Signal_Manager_initialization,
  RTEMS_SYSINIT_CLASSIC_SIGNAL,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
#endif
