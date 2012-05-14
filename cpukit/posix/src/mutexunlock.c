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

#include <errno.h>
#include <pthread.h>

#include <rtems/system.h>
#include <rtems/score/coremutex.h>
#include <rtems/score/watchdog.h>
#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/score/mpci.h>
#endif
#include <rtems/posix/mutex.h>
#include <rtems/posix/priority.h>
#include <rtems/posix/time.h>

/*
 *  11.3.3 Locking and Unlocking a Mutex, P1003.1c/Draft 10, p. 93
 *
 *  NOTE: P1003.4b/D8 adds pthread_mutex_timedlock(), p. 29
 */

int pthread_mutex_unlock(
  pthread_mutex_t           *mutex
)
{
  register POSIX_Mutex_Control *the_mutex;
  Objects_Locations             location;
  CORE_mutex_Status             status;

  the_mutex = _POSIX_Mutex_Get( mutex, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      status = _CORE_mutex_Surrender(
        &the_mutex->Mutex,
        the_mutex->Object.id,
        NULL
      );
      _Thread_Enable_dispatch();
      return _POSIX_Mutex_Translate_core_mutex_return_code( status );

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
#endif
    case OBJECTS_ERROR:
      break;
  }

  return EINVAL;
}
