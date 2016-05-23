/**
 * @file
 *
 * @brief Implements wake up version of the "signal" operation
 * @ingroup POSIX_COND_VARS Condition Variables
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/posix/condimpl.h>

/*
 *  _POSIX_Condition_variables_Signal_support
 *
 *  A support routine which implements guts of the broadcast and single task
 *  wake up version of the "signal" operation.
 */

int _POSIX_Condition_variables_Signal_support(
  pthread_cond_t            *cond,
  bool                       is_broadcast
)
{
  Thread_Control *the_thread;

  do {
    POSIX_Condition_variables_Control *the_cond;
    Thread_queue_Context               queue_context;

    the_cond = _POSIX_Condition_variables_Get( cond, &queue_context );

    if ( the_cond == NULL ) {
      return EINVAL;
    }

    _POSIX_Condition_variables_Acquire_critical( the_cond, &queue_context );

    the_thread = _Thread_queue_First_locked(
      &the_cond->Wait_queue,
      POSIX_CONDITION_VARIABLES_TQ_OPERATIONS
    );

    if ( the_thread != NULL ) {
      _Thread_queue_Extract_critical(
        &the_cond->Wait_queue.Queue,
        POSIX_CONDITION_VARIABLES_TQ_OPERATIONS,
        the_thread,
        &queue_context
      );
    } else {
      the_cond->mutex = POSIX_CONDITION_VARIABLES_NO_MUTEX;
      _POSIX_Condition_variables_Release( the_cond, &queue_context );
    }
  } while ( is_broadcast && the_thread != NULL );

  return 0;
}
