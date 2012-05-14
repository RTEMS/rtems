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
 *  11.4.2 Initializing and Destroying a Condition Variable,
 *         P1003.1c/Draft 10, p. 87
 */

int pthread_cond_init(
  pthread_cond_t           *cond,
  const pthread_condattr_t *attr
)
{
  POSIX_Condition_variables_Control   *the_cond;
  const pthread_condattr_t            *the_attr;

  if ( attr ) the_attr = attr;
  else        the_attr = &_POSIX_Condition_variables_Default_attributes;

  /*
   *  Be careful about attributes when global!!!
   */
  if ( the_attr->process_shared == PTHREAD_PROCESS_SHARED )
    return EINVAL;

  if ( !the_attr->is_initialized )
    return EINVAL;

  _Thread_Disable_dispatch();

  the_cond = _POSIX_Condition_variables_Allocate();

  if ( !the_cond ) {
    _Thread_Enable_dispatch();
    return ENOMEM;
  }

  the_cond->process_shared  = the_attr->process_shared;

  the_cond->Mutex = POSIX_CONDITION_VARIABLES_NO_MUTEX;

  _Thread_queue_Initialize(
    &the_cond->Wait_queue,
    THREAD_QUEUE_DISCIPLINE_FIFO,
    STATES_WAITING_FOR_CONDITION_VARIABLE | STATES_INTERRUPTIBLE_BY_SIGNAL,
    ETIMEDOUT
  );

  _Objects_Open_u32(
    &_POSIX_Condition_variables_Information,
    &the_cond->Object,
    0
  );

  *cond = the_cond->Object.id;

  _Thread_Enable_dispatch();

  return 0;
}
