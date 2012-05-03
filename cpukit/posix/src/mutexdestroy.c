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
#include <rtems/posix/mutex.h>
#include <rtems/posix/priority.h>
#include <rtems/posix/time.h>

/*
 *  11.3.2 Initializing and Destroying a Mutex, P1003.1c/Draft 10, p. 87
 */

int pthread_mutex_destroy(
  pthread_mutex_t           *mutex
)
{
  register POSIX_Mutex_Control *the_mutex;
  Objects_Locations             location;

  the_mutex = _POSIX_Mutex_Get( mutex, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
       /*
        * XXX: There is an error for the mutex being locked
        *  or being in use by a condition variable.
        */

      if ( _CORE_mutex_Is_locked( &the_mutex->Mutex ) ) {
        _Thread_Enable_dispatch();
        return EBUSY;
      }

      _Objects_Close( &_POSIX_Mutex_Information, &the_mutex->Object );

      _CORE_mutex_Flush( &the_mutex->Mutex, NULL, EINVAL );

      _POSIX_Mutex_Free( the_mutex );
      _Thread_Enable_dispatch();
      return 0;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
#endif
    case OBJECTS_ERROR:
      break;
  }

  return EINVAL;
}
