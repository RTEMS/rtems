/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdarg.h>

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <limits.h>

#include <rtems/system.h>
#include <rtems/score/object.h>
#include <rtems/posix/semaphore.h>
#include <rtems/posix/time.h>
#include <rtems/seterr.h>

/*
 *  11.2.1 Initialize an Unnamed Semaphore, P1003.1b-1993, p.219
 */

int sem_init(
  sem_t         *sem,
  int            pshared,
  unsigned int   value
)
{
  int                        status;
  POSIX_Semaphore_Control   *the_semaphore;

  if ( !sem )
    rtems_set_errno_and_return_minus_one( EINVAL );

  status = _POSIX_Semaphore_Create_support(
    NULL,
    0,
    pshared,
    value,
    &the_semaphore
  );

  if ( status != -1 )
    *sem = the_semaphore->Object.id;

  return status;
}
