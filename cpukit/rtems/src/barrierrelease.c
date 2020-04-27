/**
 * @file
 *
 * @ingroup ClassicBarrier Barriers
 *
 * @brief RTEMS Barrier Release
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/barrierimpl.h>

rtems_status_code rtems_barrier_release(
  rtems_id          id,
  uint32_t         *released
)
{
  Barrier_Control      *the_barrier;
  Thread_queue_Context  queue_context;

  if ( released == NULL ) {
    return RTEMS_INVALID_ADDRESS;
  }

  the_barrier = _Barrier_Get( id, &queue_context );

  if ( the_barrier == NULL ) {
    return RTEMS_INVALID_ID;
  }

  _CORE_barrier_Acquire_critical( &the_barrier->Barrier, &queue_context );
  *released = _CORE_barrier_Surrender(
    &the_barrier->Barrier,
    &queue_context
  );
  return RTEMS_SUCCESSFUL;
}
