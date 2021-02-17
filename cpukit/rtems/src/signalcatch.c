/**
 * @file
 *
 * @ingroup RTEMSImplClassicSignal
 *
 * @brief This source file contains the implementation of
 *   rtems_signal_catch() and the Signal Manager multiprocessing (MP) support
 *   system initialization.
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

#include <rtems/rtems/signalimpl.h>
#include <rtems/rtems/asrimpl.h>
#include <rtems/rtems/tasksdata.h>
#include <rtems/score/threadimpl.h>

RTEMS_STATIC_ASSERT( RTEMS_DEFAULT_MODES == 0, _ASR_Create_mode_set );

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

  if ( asr_handler != NULL ) {
    asr->mode_set = mode_set;
    asr->handler = asr_handler;
  } else {
    _ASR_Initialize( asr );
  }

  _Thread_State_release( executing, &lock_context );
  return RTEMS_SUCCESSFUL;
}
