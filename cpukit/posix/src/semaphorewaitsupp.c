/*
 *  COPYRIGHT (c) 1989-2008.
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
 *  _POSIX_Semaphore_Wait_support
 */

int _POSIX_Semaphore_Wait_support(
  sem_t             *sem,
  bool               blocking,
  Watchdog_Interval  timeout
)
{
  POSIX_Semaphore_Control *the_semaphore;
  Objects_Locations        location;

  the_semaphore = _POSIX_Semaphore_Get( sem, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      _CORE_semaphore_Seize(
        &the_semaphore->Semaphore,
        the_semaphore->Object.id,
        blocking,
        timeout
      );
      _Thread_Enable_dispatch();

      if ( !_Thread_Executing->Wait.return_code )
        return 0;

      rtems_set_errno_and_return_minus_one(
        _POSIX_Semaphore_Translate_core_semaphore_return_code(
          _Thread_Executing->Wait.return_code
        )
      );

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
#endif
    case OBJECTS_ERROR:
      break;
  }

  rtems_set_errno_and_return_minus_one( EINVAL );
}
