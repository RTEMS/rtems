/*
 *  Signal Manager
 *
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/asr.h>
#include <rtems/core/isr.h>
#include <rtems/rtems/modes.h>
#include <rtems/rtems/signal.h>
#include <rtems/core/thread.h>
#include <rtems/rtems/tasks.h>

/*PAGE
 *
 *  _Signal_Manager_initialization
 *
 *  This routine initializes all signal manager related data structures.
 *
 *  Input parameters:   NONE
 *
 *  Output parameters:  NONE
 */
 
void _Signal_Manager_initialization( void )
{
  /*
   *  Register the MP Process Packet routine.
   */
 
  _MPCI_Register_packet_processor(
    MP_PACKET_SIGNAL,
    _Signal_MP_Process_packet
  );
}
 
/*PAGE
 *
 *  rtems_signal_catch
 *
 *  This directive allows a thread to specify what action to take when
 *  catching signals.
 *
 *  Input parameters:
 *    handler  - address of asynchronous signal routine (asr)
 *              ( NULL indicates asr is invalid )
 *    mode_set - mode value for asr
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - always succeeds
 */

rtems_status_code rtems_signal_catch(
  rtems_asr_entry   asr_handler,
  rtems_mode        mode_set
)
{
  Thread_Control     *executing;
  RTEMS_API_Control  *api;
  ASR_Information    *asr;

/* XXX normalize mode */
  executing = _Thread_Executing;
  api = executing->API_Extensions[ THREAD_API_RTEMS ];
  asr = &api->Signal;

  _Thread_Disable_dispatch(); /* cannot reschedule while */
                              /*   the thread is inconsistent */

  if ( !_ASR_Is_null_handler( asr_handler ) ) {
    asr->mode_set = mode_set;
    asr->handler = asr_handler;
  }
  else
    _ASR_Initialize( asr );
  _Thread_Enable_dispatch();
  return RTEMS_SUCCESSFUL;
}

/*PAGE
 *
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
 *    error code - if unsuccessful
 */

rtems_status_code rtems_signal_send(
  Objects_Id             id,
  rtems_signal_set signal_set
)
{
  register Thread_Control *the_thread;
  Objects_Locations        location;
  RTEMS_API_Control       *api;
  ASR_Information         *asr;

  the_thread = _Thread_Get( id, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      return RTEMS_INVALID_ID;
    case OBJECTS_REMOTE:
      return _Signal_MP_Send_request_packet(
        SIGNAL_MP_SEND_REQUEST,
        id,
        signal_set
      );
    case OBJECTS_LOCAL:
      api = the_thread->API_Extensions[ THREAD_API_RTEMS ];
      asr = &api->Signal;

      if ( ! _ASR_Is_null_handler( asr->handler ) ) {
        if ( asr->is_enabled ) {
          _ASR_Post_signals( signal_set, &asr->signals_posted );
          if ( _ISR_Is_in_progress() && _Thread_Is_executing( the_thread ) )
            _ISR_Signals_to_thread_executing = TRUE;
        } else {
          _ASR_Post_signals( signal_set, &asr->signals_pending );
        }
        _Thread_Enable_dispatch();
        return RTEMS_SUCCESSFUL;
      }
      _Thread_Enable_dispatch();
      return RTEMS_NOT_DEFINED;
  }

  return RTEMS_INTERNAL_ERROR;   /* unreached - only to remove warnings */
}
