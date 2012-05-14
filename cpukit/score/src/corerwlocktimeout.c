/*
 *  Thread Queue Handler
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
#include <rtems/score/corerwlock.h>
#include <rtems/score/corerwlock.h>

/*
 *  _CORE_RWLock_Timeout
 *
 *  This routine processes a thread which timeouts while waiting on
 *  a thread queue. It is called by the watchdog handler.
 *
 *  Input parameters:
 *    id - thread id
 *
 *  Output parameters: NONE
 */

void _CORE_RWLock_Timeout(
  Objects_Id  id,
  void       *ignored
)
{
  Thread_Control       *the_thread;
  Objects_Locations     location;

  the_thread = _Thread_Get( id, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:  /* impossible */
#endif
      break;
    case OBJECTS_LOCAL:
      _Thread_queue_Process_timeout( the_thread );
      _Thread_Unnest_dispatch();
      break;
  }
}
