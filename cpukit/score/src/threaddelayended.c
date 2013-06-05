/**
 * @file
 * 
 * @brief End the Delay of a Thread
 * @ingroup ScoreThread
 */

/*
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
#include <rtems/score/apiext.h>
#include <rtems/score/context.h>
#include <rtems/score/interr.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/score/priority.h>
#include <rtems/score/states.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/thread.h>
#include <rtems/score/threaddispatch.h>
#include <rtems/score/threadq.h>
#include <rtems/score/wkspace.h>

void _Thread_Delay_ended(
  Objects_Id  id,
  void       *ignored __attribute__((unused))
)
{
  Thread_Control    *the_thread;
  Objects_Locations  location;

  the_thread = _Thread_Get( id, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:  /* impossible */
#endif
      break;
    case OBJECTS_LOCAL:
      _Thread_Clear_state(
        the_thread,
        STATES_DELAYING
          | STATES_WAITING_FOR_TIME
          | STATES_INTERRUPTIBLE_BY_SIGNAL
      );
      _Objects_Put_without_thread_dispatch( &the_thread->Object );
      break;
  }
}
