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
 *  _POSIX_Mutex_Lock_support
 *
 *  A support routine which implements guts of the blocking, non-blocking, and
 *  timed wait version of mutex lock.
 */

int _POSIX_Mutex_Lock_support(
  pthread_mutex_t           *mutex,
  bool                       blocking,
  Watchdog_Interval          timeout
)
{
  register POSIX_Mutex_Control *the_mutex;
  Objects_Locations             location;
  ISR_Level                     level;

  the_mutex = _POSIX_Mutex_Get_interrupt_disable( mutex, &location, &level );
  switch ( location ) {

    case OBJECTS_LOCAL:
      _CORE_mutex_Seize(
        &the_mutex->Mutex,
        the_mutex->Object.id,
        blocking,
        timeout,
        level
      );
      return _POSIX_Mutex_Translate_core_mutex_return_code(
        (CORE_mutex_Status) _Thread_Executing->Wait.return_code
      );

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
#endif
    case OBJECTS_ERROR:
      break;
  }

  return EINVAL;
}
