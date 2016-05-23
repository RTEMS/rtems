/**
 *  @file
 *
 *  @brief RTEMS Barrier Wait
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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/barrierimpl.h>
#include <rtems/rtems/statusimpl.h>

THREAD_QUEUE_OBJECT_ASSERT( Barrier_Control, Barrier.Wait_queue );

rtems_status_code rtems_barrier_wait(
  rtems_id        id,
  rtems_interval  timeout
)
{
  Barrier_Control      *the_barrier;
  Thread_queue_Context  queue_context;
  Status_Control        status;

  the_barrier = _Barrier_Get( id, &queue_context );

  if ( the_barrier == NULL ) {
    return RTEMS_INVALID_ID;
  }

  status = _CORE_barrier_Seize(
    &the_barrier->Barrier,
    _Thread_Executing,
    true,
    timeout,
    &queue_context
  );
  return _Status_Get( status );
}
