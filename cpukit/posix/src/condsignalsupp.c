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
  POSIX_Condition_variables_Control *the_cond;
  unsigned long                      flags;
  const Thread_queue_Operations     *operations;
  Thread_queue_Heads                *heads;

  the_cond = _POSIX_Condition_variables_Get( cond );
  POSIX_CONDITION_VARIABLES_VALIDATE_OBJECT( the_cond, flags );
  operations = POSIX_CONDITION_VARIABLES_TQ_OPERATIONS;

  do {
    Thread_queue_Context queue_context;

    _Thread_queue_Context_initialize( &queue_context );
    _POSIX_Condition_variables_Acquire( the_cond, &queue_context );

    heads = the_cond->Queue.Queue.heads;

    if ( heads != NULL ) {
      Thread_Control *the_thread;

      the_thread = ( *operations->first )( heads );
      _Thread_queue_Extract_critical(
        &the_cond->Queue.Queue,
        operations,
        the_thread,
        &queue_context
      );
    } else {
      the_cond->mutex = POSIX_CONDITION_VARIABLES_NO_MUTEX;
      _POSIX_Condition_variables_Release( the_cond, &queue_context );
    }
  } while ( is_broadcast && heads != NULL );

  return 0;
}
