/**
 * @file
 *
 * @brief Surrenders a Unit to a Semaphore
 *
 * @ingroup ScoreSemaphore
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/coresemimpl.h>

CORE_semaphore_Status _CORE_semaphore_Surrender(
  CORE_semaphore_Control                *the_semaphore,
  Objects_Id                             id,
  CORE_semaphore_API_mp_support_callout  api_semaphore_mp_support,
  ISR_lock_Context                      *lock_context
)
{
  Thread_Control *the_thread;
  CORE_semaphore_Status status;

  status = CORE_SEMAPHORE_STATUS_SUCCESSFUL;

  _Thread_queue_Acquire_critical( &the_semaphore->Wait_queue, lock_context );

  the_thread = _Thread_queue_First_locked( &the_semaphore->Wait_queue );
  if ( the_thread != NULL ) {
#if defined(RTEMS_MULTIPROCESSING)
    _Thread_Dispatch_disable();
#endif

    _Thread_queue_Extract_critical(
      &the_semaphore->Wait_queue,
      the_thread,
      lock_context
    );

#if defined(RTEMS_MULTIPROCESSING)
    if ( !_Objects_Is_local_id( the_thread->Object.id ) )
      (*api_semaphore_mp_support) ( the_thread, id );

    _Thread_Dispatch_enable( _Per_CPU_Get() );
#endif
  } else {
    if ( the_semaphore->count < the_semaphore->Attributes.maximum_count )
      the_semaphore->count += 1;
    else
      status = CORE_SEMAPHORE_MAXIMUM_COUNT_EXCEEDED;

    _Thread_queue_Release( &the_semaphore->Wait_queue, lock_context );
  }

  return status;
}
