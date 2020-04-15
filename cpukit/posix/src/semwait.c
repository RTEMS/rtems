/**
 *  @file
 *
 *  @brief Lock a Semaphore
 *  @ingroup POSIX_SEMAPHORE
 */

/*
 *  COPYRIGHT (c) 1989-2008.
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

int sem_wait( sem_t *sem )
{
  POSIX_SEMAPHORE_VALIDATE_OBJECT( sem );
  _Semaphore_Wait( &sem->_Semaphore );
  return 0;
}
