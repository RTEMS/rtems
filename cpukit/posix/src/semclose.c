/**
 * @file
 *
 * @brief Close a Named Semaphore
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

#include <stdarg.h>

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <limits.h>

#include <rtems/system.h>
#include <rtems/posix/semaphoreimpl.h>
#include <rtems/seterr.h>

int sem_close(
  sem_t *sem
)
{
  POSIX_Semaphore_Control          *the_semaphore;
  Objects_Locations                 location;

  _Objects_Allocator_lock();
  the_semaphore = _POSIX_Semaphore_Get( sem, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      the_semaphore->open_count -= 1;
      _POSIX_Semaphore_Delete( the_semaphore );
      _Objects_Put( &the_semaphore->Object );
      _Objects_Allocator_unlock();
      return 0;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
#endif
    case OBJECTS_ERROR:
      break;
  }

  _Objects_Allocator_unlock();

  rtems_set_errno_and_return_minus_one( EINVAL );
}
