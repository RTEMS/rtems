/**
 * @file
 *
 * @brief RTEMS Barrier Release
 * @ingroup ClassicBarrier Barriers
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/barrierimpl.h>

rtems_status_code rtems_barrier_release(
  rtems_id          id,
  uint32_t         *released
)
{
  Barrier_Control  *the_barrier;
  ISR_lock_Context  lock_context;

  if ( released == NULL ) {
    return RTEMS_INVALID_ADDRESS;
  }

  the_barrier = _Barrier_Get( id, &lock_context );

  if ( the_barrier == NULL ) {
    return RTEMS_INVALID_ID;
  }

  _CORE_barrier_Acquire_critical( &the_barrier->Barrier, &lock_context );
  *released = _CORE_barrier_Surrender(
    &the_barrier->Barrier,
    NULL,
    &lock_context
  );
  return RTEMS_SUCCESSFUL;
}
