/**
 * @file
 *
 * @brief Returns the Current Priority Ceiling of the Mutex
 * @ingroup POSIXAPI
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

#include <rtems/posix/muteximpl.h>
#include <rtems/posix/priorityimpl.h>

/*
 *  13.6.2 Change the Priority Ceiling of a Mutex, P1003.1c/Draft 10, p. 131
 */

int pthread_mutex_getprioceiling(
  pthread_mutex_t   *mutex,
  int               *prioceiling
)
{
  POSIX_Mutex_Control  *the_mutex;
  Thread_queue_Context  queue_context;

  if ( prioceiling == NULL ) {
    return EINVAL;
  }

  the_mutex = _POSIX_Mutex_Get( mutex, &queue_context );

  if ( the_mutex == NULL ) {
    return EINVAL;
  }

  _POSIX_Mutex_Acquire_critical( the_mutex, &queue_context );

  if ( the_mutex->protocol == POSIX_MUTEX_PRIORITY_CEILING ) {
    *prioceiling = _POSIX_Priority_From_core(
      _CORE_ceiling_mutex_Get_scheduler( &the_mutex->Mutex ),
      _CORE_ceiling_mutex_Get_priority( &the_mutex->Mutex )
    );
  } else {
    *prioceiling = 0;
  }

  _POSIX_Mutex_Release( the_mutex, &queue_context );

  return 0;
}
