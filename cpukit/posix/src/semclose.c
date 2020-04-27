/**
 * @file
 *
 * @ingroup POSIX_SEMAPHORE
 *
 * @brief Close a Named Semaphore
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/posix/semaphoreimpl.h>

int sem_close( sem_t *sem )
{
  POSIX_Semaphore_Control *the_semaphore;
  uint32_t                 open_count;

  POSIX_SEMAPHORE_VALIDATE_OBJECT( sem );

  if ( !_POSIX_Semaphore_Is_named( sem ) ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  the_semaphore = _POSIX_Semaphore_Get( sem );

  _Objects_Allocator_lock();

  open_count = the_semaphore->open_count;

  if ( open_count == 0 ) {
    _Objects_Allocator_unlock();
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  if ( open_count == 1 && _POSIX_Semaphore_Is_busy( sem ) ) {
    _Objects_Allocator_unlock();
    rtems_set_errno_and_return_minus_one( EBUSY );
  }

  the_semaphore->open_count = open_count - 1;
  _POSIX_Semaphore_Delete( the_semaphore );
  _Objects_Allocator_unlock();
  return 0;
}
