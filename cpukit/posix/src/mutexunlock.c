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
    case OBJECTS_REMOTE:
#if defined(RTEMS_MULTIPROCESSING)
      return _POSIX_Mutex_MP_Send_request_packet(
          POSIX_MUTEX_MP_RELEASE_REQUEST,
          *mutex,
          0,                    /* Not used */
          MPCI_DEFAULT_TIMEOUT
      );
#endif
    case OBJECTS_ERROR:
      return EINVAL;
    case OBJECTS_LOCAL:
      status = _CORE_mutex_Surrender(
        &the_mutex->Mutex,
        the_mutex->Object.id, 
#if defined(RTEMS_MULTIPROCESSING)
        _POSIX_Threads_mutex_MP_support
#else
        NULL
#endif
      );
      _Thread_Enable_dispatch();
      return _POSIX_Mutex_From_core_mutex_status( status );
      break;
  }
  return POSIX_BOTTOM_REACHED();
}
