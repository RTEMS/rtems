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
 *  _POSIX_Mutex_From_core_mutex_status
 */

int _POSIX_Mutex_From_core_mutex_status(
  CORE_mutex_Status  status
)
{
  switch ( status ) {
    case CORE_MUTEX_STATUS_SUCCESSFUL:
      return 0;
    case CORE_MUTEX_STATUS_UNSATISFIED_NOWAIT:
      return EBUSY;
    case CORE_MUTEX_STATUS_NESTING_NOT_ALLOWED:
      return EDEADLK;
    case CORE_MUTEX_STATUS_NOT_OWNER_OF_RESOURCE:
      return EPERM;
    case CORE_MUTEX_WAS_DELETED:
      return EINVAL;
    case CORE_MUTEX_TIMEOUT:
      return EAGAIN;
    case CORE_MUTEX_STATUS_CEILING_VIOLATED:
      return EINVAL;
    default:
      break;
  }
  assert( 0 );
  return 0;
}
