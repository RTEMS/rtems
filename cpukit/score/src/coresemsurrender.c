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
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/coresemimpl.h>
#include <rtems/score/objectimpl.h>

CORE_semaphore_Status _CORE_semaphore_Surrender(
  CORE_semaphore_Control                *the_semaphore,
  Objects_Id                             id,
  CORE_semaphore_API_mp_support_callout  api_semaphore_mp_support
)
{
  Thread_Control *the_thread;
  ISR_Level       level;
  CORE_semaphore_Status status;

  status = CORE_SEMAPHORE_STATUS_SUCCESSFUL;

  if ( (the_thread = _Thread_queue_Dequeue(&the_semaphore->Wait_queue)) ) {

#if defined(RTEMS_MULTIPROCESSING)
    if ( !_Objects_Is_local_id( the_thread->Object.id ) )
      (*api_semaphore_mp_support) ( the_thread, id );
#endif

  } else {
    _ISR_Disable( level );
      if ( the_semaphore->count < the_semaphore->Attributes.maximum_count )
        the_semaphore->count += 1;
      else
        status = CORE_SEMAPHORE_MAXIMUM_COUNT_EXCEEDED;
    _ISR_Enable( level );
  }

  return status;
}
