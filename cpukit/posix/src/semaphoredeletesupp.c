/**
 *  @file
 *
 *  @brief POSIX Delete Semaphore
 *  @ingroup POSIX_SEMAPHORE
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

#include <rtems/posix/semaphoreimpl.h>

void _POSIX_Semaphore_Delete(
  POSIX_Semaphore_Control *the_semaphore,
  Thread_queue_Context    *queue_context
)
{
  if ( !the_semaphore->linked && !the_semaphore->open_count ) {
    _Objects_Close( &_POSIX_Semaphore_Information, &the_semaphore->Object );
    _CORE_semaphore_Destroy(
      &the_semaphore->Semaphore,
      POSIX_SEMAPHORE_TQ_OPERATIONS,
      queue_context
    );
    _POSIX_Semaphore_Free( the_semaphore );
  } else {
    _CORE_semaphore_Release( &the_semaphore->Semaphore, queue_context );
  }
}
