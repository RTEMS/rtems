/*
 *  Event Manager
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
#include <rtems/event.h>
#include <rtems/isr.h>
#include <rtems/object.h>
#include <rtems/options.h>
#include <rtems/states.h>
#include <rtems/thread.h>

rtems_status_code rtems_event_send(
  Objects_Id         id,
  rtems_event_set event_in
)
{
  return( RTEMS_NOT_CONFIGURED );
}

rtems_status_code rtems_event_receive(
  rtems_event_set  event_in,
  rtems_option     option_set,
  rtems_interval   ticks,
  rtems_event_set *event_out
)
{
  return( RTEMS_NOT_CONFIGURED );
}

