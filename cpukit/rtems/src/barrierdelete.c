/**
 *  @file
 *
 *  @brief RTEMS Delete Barrier
 *  @ingroup ClassicBarrier
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
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/rtems/barrierimpl.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>

rtems_status_code rtems_barrier_delete(
  rtems_id   id
)
{
  Barrier_Control   *the_barrier;
  Objects_Locations  location;

  the_barrier = _Barrier_Get( id, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      _CORE_barrier_Flush(
        &the_barrier->Barrier,
        NULL,
        CORE_BARRIER_WAS_DELETED
      );

      _Objects_Close( &_Barrier_Information, &the_barrier->Object );

      _Barrier_Free( the_barrier );

      _Objects_Put( &the_barrier->Object );
      return RTEMS_SUCCESSFUL;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
#endif
    case OBJECTS_ERROR:
      break;
  }

  return RTEMS_INVALID_ID;
}
