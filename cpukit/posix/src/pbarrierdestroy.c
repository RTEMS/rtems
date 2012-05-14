/*
 *  POSIX Barrier Manager -- Destroy a Barrier
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

#include <pthread.h>
#include <errno.h>

#include <rtems/system.h>
#include <rtems/posix/barrier.h>

/*
 *  pthread_barrier_destroy
 *
 *  This directive allows a thread to delete a barrier specified by
 *  the barrier id.  The barrier is freed back to the inactive
 *  barrier chain.
 *
 *  Input parameters:
 *    barrier - barrier id
 *
 *  Output parameters:
 *    0           - if successful
 *    error code  - if unsuccessful
 */

int pthread_barrier_destroy(
  pthread_barrier_t *barrier
)
{
  POSIX_Barrier_Control *the_barrier = NULL;
  Objects_Locations      location;

  if ( !barrier )
    return EINVAL;

  the_barrier = _POSIX_Barrier_Get( barrier, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      if ( the_barrier->Barrier.number_of_waiting_threads != 0 ) {
        _Thread_Enable_dispatch();
        return EBUSY;
      }

      _Objects_Close( &_POSIX_Barrier_Information, &the_barrier->Object );

      _POSIX_Barrier_Free( the_barrier );

      _Thread_Enable_dispatch();
      return 0;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
#endif
    case OBJECTS_ERROR:
      break;
  }

  return EINVAL;
}
