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
 *  13.6.2 Change the Priority Ceiling of a Mutex, P1003.1c/Draft 10, p. 131
 */

int pthread_mutex_setprioceiling(
  pthread_mutex_t   *mutex,
  int                prioceiling,
  int               *old_ceiling
)
{
  register POSIX_Mutex_Control *the_mutex;
  Objects_Locations             location;
  Priority_Control              the_priority;
  int                           status;

  if ( !old_ceiling )
    return EINVAL;

  if ( !_POSIX_Priority_Is_valid( prioceiling ) )
    return EINVAL;

  the_priority = _POSIX_Priority_To_core( prioceiling );

  /*
   *  Must acquire the mutex before we can change it's ceiling
   */

  status = pthread_mutex_lock( mutex );
  if ( status )
    return status;

  the_mutex = _POSIX_Mutex_Get( mutex, &location );
  switch ( location ) {
    case OBJECTS_REMOTE:
#if defined(RTEMS_MULTIPROCESSING)
      /*  XXX It feels questionable to set the ceiling on a remote mutex. */
      return EINVAL;
#endif
    case OBJECTS_ERROR:
      return EINVAL;        /* impossible to get here */
    case OBJECTS_LOCAL:
      *old_ceiling = _POSIX_Priority_From_core(
        the_mutex->Mutex.Attributes.priority_ceiling
      );
      the_mutex->Mutex.Attributes.priority_ceiling = the_priority;
      _CORE_mutex_Surrender(
        &the_mutex->Mutex,
        the_mutex->Object.id, 
#if defined(RTEMS_MULTIPROCESSING)
        _POSIX_Threads_mutex_MP_support
#else
        NULL
#endif
      );
      _Thread_Enable_dispatch();
      return 0;
  }
  return POSIX_BOTTOM_REACHED();
}
