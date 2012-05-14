/*
 *  16.1.8 Dynamic Package Initialization, P1003.1c/Draft 10, p. 154
 *
 *  COPYRIGHT (c) 1989-1999.
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

#include <rtems.h>
#include <rtems/system.h>
#include <rtems/score/thread.h>

int pthread_once(
  pthread_once_t  *once_control,
  void           (*init_routine)(void)
)
{
  if ( !once_control || !init_routine )
    return EINVAL;

  if ( !once_control->init_executed ) {
    rtems_mode saveMode;
    rtems_task_mode(RTEMS_NO_PREEMPT, RTEMS_PREEMPT_MASK, &saveMode);
    if ( !once_control->init_executed ) {
      once_control->is_initialized = true;
      once_control->init_executed = true;
      (*init_routine)();
    }
    rtems_task_mode(saveMode, RTEMS_PREEMPT_MASK, &saveMode);
  }
  return 0;
}
