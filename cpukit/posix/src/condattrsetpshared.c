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

#include <pthread.h>
#include <errno.h>

#include <rtems/system.h>
#include <rtems/score/object.h>
#include <rtems/score/states.h>
#include <rtems/score/watchdog.h>
#include <rtems/posix/cond.h>
#include <rtems/posix/time.h>
#include <rtems/posix/mutex.h>

/*
 *  11.4.1 Condition Variable Initialization Attributes,
 *            P1003.1c/Draft 10, p. 96
 */

int pthread_condattr_setpshared(
  pthread_condattr_t *attr,
  int                 pshared
)
{
  if ( !attr )
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
