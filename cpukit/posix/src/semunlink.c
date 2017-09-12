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

#include <rtems/posix/semaphoreimpl.h>

int sem_unlink( const char *name )
{
  POSIX_Semaphore_Control   *the_semaphore;
  Objects_Get_by_name_error  error;

  _Objects_Allocator_lock();

  the_semaphore = _POSIX_Semaphore_Get_by_name( name, NULL, &error );
  if ( the_semaphore == NULL ) {
    _Objects_Allocator_unlock();
    rtems_set_errno_and_return_minus_one( _POSIX_Get_by_name_error( error ) );
  }

  _POSIX_Semaphore_Namespace_remove( the_semaphore );
  the_semaphore->linked = false;
  _POSIX_Semaphore_Delete( the_semaphore );
  _Objects_Allocator_unlock();
  return 0;
}
