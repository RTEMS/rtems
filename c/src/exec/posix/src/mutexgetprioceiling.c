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

int pthread_mutex_getprioceiling(
  pthread_mutex_t   *mutex,
  int               *prioceiling
)
{
  register POSIX_Mutex_Control *the_mutex;
  Objects_Locations             location;

  if ( !prioceiling )
    return EINVAL;

  the_mutex = _POSIX_Mutex_Get( mutex, &location );
  switch ( location ) {
    case OBJECTS_REMOTE:
#if defined(RTEMS_MULTIPROCESSING)
      return POSIX_MP_NOT_IMPLEMENTED();   /* XXX feels questionable */
#endif
    case OBJECTS_ERROR:
      return EINVAL;
    case OBJECTS_LOCAL:
      *prioceiling = _POSIX_Priority_From_core(
        the_mutex->Mutex.Attributes.priority_ceiling
      );
      _Thread_Enable_dispatch();
      return 0;
  }
  return POSIX_BOTTOM_REACHED();
}
