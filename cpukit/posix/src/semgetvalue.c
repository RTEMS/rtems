/**
 * @file
 *
 * @brief Get the Value of a Semaphore
 * @ingroup POSIX_SEMAPHORE
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

int sem_getvalue(
  sem_t  *__restrict sem,
  int    *__restrict sval
)
{
  register POSIX_Semaphore_Control *the_semaphore;
  Objects_Locations                 location;

  the_semaphore = _POSIX_Semaphore_Get( sem, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      *sval = _CORE_semaphore_Get_count( &the_semaphore->Semaphore );
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
