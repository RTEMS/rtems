/*
 *  COPYRIGHT (c) 1989-2009.
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

#if defined(_UNIX98_THREAD_MUTEX_ATTRIBUTES)
int pthread_mutexattr_settype(
  pthread_mutexattr_t *attr,
  int                  type
)
{
  if ( !attr || !attr->is_initialized )
    return EINVAL;

  switch ( type ) {
    case PTHREAD_MUTEX_NORMAL:
    case PTHREAD_MUTEX_RECURSIVE:
    case PTHREAD_MUTEX_ERRORCHECK:
    case PTHREAD_MUTEX_DEFAULT:
      attr->type = type;
      return 0;

    default:
      return EINVAL;
  }
}
#endif
