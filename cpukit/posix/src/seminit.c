/**
 * @file
 *
 * @brief Initializing of an Unnamed Semaphore
 * @ingroup POSIXAPI
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

#include <limits.h>

/*
 *  11.2.1 Initialize an Unnamed Semaphore, P1003.1b-1993, p.219
 */

int sem_init(
  sem_t         *sem,
  int            pshared,
  unsigned int   value
)
{
  if ( sem == NULL ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  if ( value > SEM_VALUE_MAX ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  _POSIX_Semaphore_Initialize( sem, NULL, value );
  return 0;
}
