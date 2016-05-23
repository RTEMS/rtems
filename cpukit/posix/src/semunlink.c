/**
 * @file
 *
 * @brief Remove a Named Semaphore
 * @ingroup POSIX_SEMAPHORE
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

int sem_unlink(
  const char *name
)
{
  POSIX_Semaphore_Control   *the_semaphore;
  Objects_Get_by_name_error  error;
  Thread_queue_Context       queue_context;

  _Objects_Allocator_lock();

  the_semaphore = _POSIX_Semaphore_Get_by_name( name, NULL, &error );
  if ( the_semaphore == NULL ) {
    _Objects_Allocator_unlock();
    rtems_set_errno_and_return_minus_one( _POSIX_Get_by_name_error( error ) );
  }

  _POSIX_Semaphore_Namespace_remove( the_semaphore );

  _ISR_lock_ISR_disable( &queue_context.Lock_context );
  _CORE_semaphore_Acquire_critical( &the_semaphore->Semaphore, &queue_context );
  the_semaphore->linked = false;
  _POSIX_Semaphore_Delete( the_semaphore, &queue_context );

  _Objects_Allocator_unlock();
  return 0;
}
