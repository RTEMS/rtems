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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/posix/semaphoreimpl.h>

int sem_destroy( sem_t *sem )
{
  POSIX_SEMAPHORE_VALIDATE_OBJECT( sem );

  if ( _POSIX_Semaphore_Is_named( sem ) ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  if ( _POSIX_Semaphore_Is_busy( sem ) ) {
    rtems_set_errno_and_return_minus_one( EBUSY );
  }

  _POSIX_Semaphore_Destroy( sem );
  return 0;
}
