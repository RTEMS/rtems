/*
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <assert.h>
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

/*PAGE
 *
 *  _POSIX_Mutex_Lock_support
 *
 *  A support routine which implements guts of the blocking, non-blocking, and
 *  timed wait version of mutex lock.
 */

int _POSIX_Mutex_Lock_support(
  pthread_mutex_t           *mutex,
  boolean                    blocking,
  Watchdog_Interval          timeout
)
{
  register POSIX_Mutex_Control *the_mutex;
  Objects_Locations             location;
  ISR_Level                     level;
 
  the_mutex = _POSIX_Mutex_Get_interrupt_disable( mutex, &location, &level );
  switch ( location ) {
    case OBJECTS_REMOTE:
#if defined(RTEMS_MULTIPROCESSING)
      return _POSIX_Mutex_MP_Send_request_packet(
          POSIX_MUTEX_MP_OBTAIN_REQUEST,
          *mutex,
          0,   /* must define the option set */
          WATCHDOG_NO_TIMEOUT
      );
#endif
    case OBJECTS_ERROR:
      return EINVAL;
    case OBJECTS_LOCAL:
      _CORE_mutex_Seize(
        &the_mutex->Mutex,
        the_mutex->Object.id,
        blocking,
        timeout,
        level
      );
      return _POSIX_Mutex_From_core_mutex_status(
        (CORE_mutex_Status) _Thread_Executing->Wait.return_code
      );
  }
  return POSIX_BOTTOM_REACHED();
}
