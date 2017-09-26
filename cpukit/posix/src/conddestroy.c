/**
 *  @file
 *
 *  @brief Destroy a Condition Variable
 *  @ingroup POSIXAPI
 */

/*
 *  COPYRIGHT (c) 1989-2007.
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

/**
 *  11.4.2 Initializing and Destroying a Condition Variable,
 *         P1003.1c/Draft 10, p. 87
 */
int pthread_cond_destroy( pthread_cond_t *cond )
{
  POSIX_Condition_variables_Control *the_cond;
  unsigned long                      flags;
  Thread_queue_Context               queue_context;

  the_cond = _POSIX_Condition_variables_Get( cond );
  POSIX_CONDITION_VARIABLES_VALIDATE_OBJECT( the_cond, flags );

  _Thread_queue_Context_initialize( &queue_context );
  _POSIX_Condition_variables_Acquire( the_cond, &queue_context );

  if ( !_Thread_queue_Is_empty( &the_cond->Queue.Queue ) ) {
    _POSIX_Condition_variables_Release( the_cond, &queue_context );
    return EBUSY;
  }

  _POSIX_Condition_variables_Release( the_cond, &queue_context );
  _POSIX_Condition_variables_Destroy( the_cond );
  return 0;
}
