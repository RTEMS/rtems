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
 *  11.3.1 Mutex Initialization Attributes, P1003.1c/Draft 10, p. 81
 */

int pthread_mutexattr_setpshared(
  pthread_mutexattr_t *attr,
  int                  pshared
)
{
  if ( !attr || !attr->is_initialized )
    return EINVAL;

  switch ( pshared ) {
    case PTHREAD_PROCESS_SHARED:
    case PTHREAD_PROCESS_PRIVATE:
      attr->process_shared = pshared;
      return 0;

    default:
      return EINVAL;
  }
}
