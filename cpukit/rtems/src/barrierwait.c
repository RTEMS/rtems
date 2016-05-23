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

THREAD_QUEUE_OBJECT_ASSERT( Barrier_Control, Barrier.Wait_queue );

rtems_status_code rtems_barrier_wait(
  rtems_id        id,
  rtems_interval  timeout
)
{
  Barrier_Control  *the_barrier;
  ISR_lock_Context  lock_context;
  Thread_Control   *executing;

  the_barrier = _Barrier_Get( id, &lock_context );

  if ( the_barrier == NULL ) {
    return RTEMS_INVALID_ID;
  }

  executing = _Thread_Executing;
  _CORE_barrier_Seize(
    &the_barrier->Barrier,
    executing,
    true,
    timeout,
    NULL,
    &lock_context
  );
  return _Barrier_Translate_core_barrier_return_code(
    executing->Wait.return_code
  );
}
