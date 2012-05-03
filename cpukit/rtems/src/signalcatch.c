/*
 *  Signal Manager
 *
 *
 *  COPYRIGHT (c) 1989-1999.
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
  api = (RTEMS_API_Control*)executing->API_Extensions[ THREAD_API_RTEMS ];
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
