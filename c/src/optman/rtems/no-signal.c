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
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/score/isr.h>
#include <rtems/score/thread.h>
#include <rtems/score/interr.h>

#include <rtems/rtems/asr.h>
#include <rtems/rtems/modes.h>
#include <rtems/rtems/signal.h>

void _Signal_Manager_initialization( void )
{
}
 
rtems_status_code rtems_signal_catch(
  rtems_asr_entry   handler,
  rtems_mode        mode_set
)
{
  _Internal_error_Occurred(
    INTERNAL_ERROR_RTEMS_API,
    FALSE,
    RTEMS_NOT_CONFIGURED
  );
  return RTEMS_NOT_CONFIGURED;
}

rtems_status_code rtems_signal_send(
  rtems_id         id,
  rtems_signal_set signal_set
)
{
  _Internal_error_Occurred(
    INTERNAL_ERROR_RTEMS_API,
    FALSE,
    RTEMS_NOT_CONFIGURED
  );
  return RTEMS_NOT_CONFIGURED;
}

/* end of file */
