/*
 *  Thread Handler
 *
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/score/apiext.h>
#include <rtems/score/context.h>
#include <rtems/score/interr.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/score/priority.h>
#include <rtems/score/states.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/score/userext.h>
#include <rtems/score/wkspace.h>

/*PAGE
 *
 *  _Thread_Delay_ended
 *
 *  This routine processes a thread whose delay period has ended.
 *  It is called by the watchdog handler.
 *
 *  Input parameters:
 *    id - thread id
 *
 *  Output parameters: NONE
 */

void _Thread_Delay_ended(
  Objects_Id  id,
  void       *ignored
)
{
  Thread_Control    *the_thread;
  Objects_Locations  location;

  the_thread = _Thread_Get( id, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
    case OBJECTS_REMOTE:  /* impossible */
      break;
    case OBJECTS_LOCAL:
      _Thread_Unblock( the_thread );
      _Thread_Unnest_dispatch();
      break;
  }
}
