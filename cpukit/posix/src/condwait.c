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
 *  11.4.4 Waiting on a Condition, P1003.1c/Draft 10, p. 105
 */

int pthread_cond_wait(
  pthread_cond_t     *cond,
  pthread_mutex_t    *mutex
)
{
  return _POSIX_Condition_variables_Wait_support(
    cond,
    mutex,
    THREAD_QUEUE_WAIT_FOREVER,
    false
  );
}
