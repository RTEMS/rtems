/*
 *  Barrier Manager -- Release Tasks Waitng at a Barrier
 *
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/rtems/barrier.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>

/*
 *  rtems_barrier_release
 *
 *  This directive releases all threads waiting at a barrier.
 *
 *  Input parameters:
 *    id         - barrier id
 *    released   - pointer to number of threads unblocked
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code       - if unsuccessful
 *    *released        - number of threads unblocked
 */

rtems_status_code rtems_barrier_release(
  rtems_id          id,
  rtems_unsigned32 *released
)
{
  Barrier_Control   *the_barrier;
  Objects_Locations  location;

  if ( !released )
    return RTEMS_INVALID_ADDRESS;

  the_barrier = _Barrier_Get( id, &location );
  switch ( location ) {
    case OBJECTS_REMOTE:
    case OBJECTS_ERROR:
      return RTEMS_INVALID_ID;

    case OBJECTS_LOCAL:
      *released = _CORE_barrier_Release( &the_barrier->Barrier, id, NULL );
      _Thread_Enable_dispatch();
      return _Barrier_Translate_core_barrier_return_code(
                _Thread_Executing->Wait.return_code );

  }

  return RTEMS_INTERNAL_ERROR;   /* unreached - only to remove warnings */
}
