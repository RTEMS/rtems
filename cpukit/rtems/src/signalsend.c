/*
 *  Signal Manager
 *
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/asr.h>
#include <rtems/score/isr.h>
#include <rtems/rtems/modes.h>
#include <rtems/rtems/signal.h>
#include <rtems/score/thread.h>
#include <rtems/rtems/tasks.h>

/*
 *  rtems_signal_send
 *
 *  This directive allows a thread to send signals to a thread.
 *
 *  Input parameters:
 *    id         - thread id
 *    signal_set - signal set
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code       - if unsuccessful
 */

rtems_status_code rtems_signal_send(
  rtems_id          id,
  rtems_signal_set  signal_set
)
{
  register Thread_Control *the_thread;
  Objects_Locations        location;
  RTEMS_API_Control       *api;
  ASR_Information         *asr;

  if ( !signal_set )
    return RTEMS_INVALID_NUMBER;

  the_thread = _Thread_Get( id, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      api = the_thread->API_Extensions[ THREAD_API_RTEMS ];
      asr = &api->Signal;

      if ( ! _ASR_Is_null_handler( asr->handler ) ) {
        if ( asr->is_enabled ) {
          _ASR_Post_signals( signal_set, &asr->signals_posted );

          if ( _ISR_Is_in_progress() && _Thread_Is_executing( the_thread ) )
            _Thread_Dispatch_necessary = true;
        } else {
          _ASR_Post_signals( signal_set, &asr->signals_pending );
        }
        _Thread_Enable_dispatch();
        return RTEMS_SUCCESSFUL;
      }
      _Thread_Enable_dispatch();
      return RTEMS_NOT_DEFINED;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
      return _Signal_MP_Send_request_packet(
        SIGNAL_MP_SEND_REQUEST,
        id,
        signal_set
      );
#endif

    case OBJECTS_ERROR:
      break;
  }

  return RTEMS_INVALID_ID;
}
