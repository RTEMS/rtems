/**
 *  @file
 *
 *  @brief Destroy an Unnamed Semaphore
 *  @ingroup POSIX_SEMAPHORE
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <semaphore.h>

#include <rtems/posix/semaphoreimpl.h>

int sem_destroy(
  sem_t *sem
)
{
  POSIX_Semaphore_Control *the_semaphore;
  Thread_queue_Context     queue_context;

  _Objects_Allocator_lock();
  the_semaphore = _POSIX_Semaphore_Get( sem, &queue_context );

  if ( the_semaphore == NULL ) {
    _Objects_Allocator_unlock();
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  _CORE_semaphore_Acquire_critical(
    &the_semaphore->Semaphore,
    &queue_context
  );

  if ( the_semaphore->named ) {
    /* Undefined operation on a named semaphore */
    _CORE_semaphore_Release( &the_semaphore->Semaphore, &queue_context );
    _Objects_Allocator_unlock();
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  _POSIX_Semaphore_Delete( the_semaphore, &queue_context );

  _Objects_Allocator_unlock();
  return 0;
}
