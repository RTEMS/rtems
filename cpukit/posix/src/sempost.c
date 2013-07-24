/**
 *  @file
 *
 *  @brief Unlock a Semaphore
 *  @ingroup POSIX_SEMAPHORE
 */

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
#include <rtems/posix/semaphoreimpl.h>
#include <rtems/posix/time.h>
#include <rtems/seterr.h>

int sem_post(
  sem_t  *sem
)
{
  register POSIX_Semaphore_Control *the_semaphore;
  Objects_Locations                 location;

  the_semaphore = _POSIX_Semaphore_Get( sem, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      _CORE_semaphore_Surrender(
        &the_semaphore->Semaphore,
        the_semaphore->Object.id,
#if defined(RTEMS_MULTIPROCESSING)
        NULL         /* XXX need to define a routine to handle this case */
#else
        NULL
#endif
      );
      _Objects_Put( &the_semaphore->Object );
      return 0;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
#endif
    case OBJECTS_ERROR:
      break;
  }

  rtems_set_errno_and_return_minus_one( EINVAL );
}
