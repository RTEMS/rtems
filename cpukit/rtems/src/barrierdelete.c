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
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/barrierimpl.h>

rtems_status_code rtems_barrier_delete(
  rtems_id   id
)
{
  Barrier_Control      *the_barrier;
  Thread_queue_Context  queue_context;

  _Objects_Allocator_lock();
  the_barrier = _Barrier_Get( id, &queue_context );

  if ( the_barrier == NULL ) {
    _Objects_Allocator_unlock();
    return RTEMS_INVALID_ID;
  }

  _CORE_barrier_Acquire_critical( &the_barrier->Barrier, &queue_context );
  _Objects_Close( &_Barrier_Information, &the_barrier->Object );
  _CORE_barrier_Flush( &the_barrier->Barrier, &queue_context );
  _Barrier_Free( the_barrier );
  _Objects_Allocator_unlock();
  return RTEMS_SUCCESSFUL;
}
