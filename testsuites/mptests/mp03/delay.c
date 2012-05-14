/*  Delayed_send_event
 *
 *  This routine is a timer service routine which sends an event to task 1.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "system.h"

rtems_timer_service_routine Delayed_send_event(
  rtems_id  ignored_id,
  void     *ignored_address
)
{
  rtems_status_code status;

  status = rtems_event_send( Task_id[ 1 ], RTEMS_EVENT_16 );
  if ( status ) printk( "Delayed_send_event failed %d\n", status );
  // directive_failed_with_level( status, "rtems_event_send", 1 );
}
