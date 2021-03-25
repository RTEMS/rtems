/**
 * @file
 *
 * @ingroup POSIX_COND_VARS Condition Variables
 *
 * @brief Implements wake up version of the "signal" operation
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
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
  POSIX_Condition_variables_Control *the_cond;
  unsigned long                      flags;
  Thread_queue_Context               queue_context;

  the_cond = _POSIX_Condition_variables_Get( cond );
  POSIX_CONDITION_VARIABLES_VALIDATE_OBJECT( the_cond, flags );
  _Thread_queue_Context_initialize( &queue_context );

  do {
    Thread_queue_Heads *heads;

    _POSIX_Condition_variables_Acquire( the_cond, &queue_context );

    heads = the_cond->Queue.Queue.heads;

    if ( heads == NULL ) {
      the_cond->mutex = POSIX_CONDITION_VARIABLES_NO_MUTEX;
      _POSIX_Condition_variables_Release( the_cond, &queue_context );

      return 0;
    }

    _Thread_queue_Surrender_no_priority(
      &the_cond->Queue.Queue,
      heads,
      &queue_context,
      POSIX_CONDITION_VARIABLES_TQ_OPERATIONS
    );
  } while ( is_broadcast );

  return 0;
}
