/*  Delayed_send_event
 *
 *  This routine is a timer service routine which sends an event to a task.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
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

#include "system.h"

rtems_timer_service_routine Delayed_send_event(
  rtems_id  timer_id,
  void     *ignored
)
{
  rtems_status_code status;

  status = rtems_event_send(
    Task_id[ rtems_get_index( timer_id ) ],
    RTEMS_EVENT_16
  );
  directive_failed( status, "rtems_event_send" );
}
