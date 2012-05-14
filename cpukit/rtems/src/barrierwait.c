/*
 *  Barrier Manager -- Wait at a Barrier
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
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/rtems/barrier.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>

/*
 *  rtems_barrier_wait
 *
 *  This directive allows a thread to wait at a barrier.
 *
 *  Input parameters:
 *    id         - barrier id
 *    timeout    - number of ticks to wait (0 means wait forever)
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code        - if unsuccessful
 */

rtems_status_code rtems_barrier_wait(
  rtems_id        id,
  rtems_interval  timeout
)
{
  Barrier_Control   *the_barrier;
  Objects_Locations  location;

  the_barrier = _Barrier_Get( id, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      _CORE_barrier_Wait(
        &the_barrier->Barrier,
        id,
        true,
        timeout,
        NULL
      );
      _Thread_Enable_dispatch();
      return _Barrier_Translate_core_barrier_return_code(
                _Thread_Executing->Wait.return_code );

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
#endif
    case OBJECTS_ERROR:
      break;
  }

  return RTEMS_INVALID_ID;
}
