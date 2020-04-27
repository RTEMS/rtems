/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Returns the Current Priority Ceiling of the Mutex
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/posix/muteximpl.h>
#include <rtems/posix/priorityimpl.h>

/*
 *  13.6.2 Change the Priority Ceiling of a Mutex, P1003.1c/Draft 10, p. 131
 */

int pthread_mutex_getprioceiling(
#ifdef HAVE_PTHREAD_MUTEX_GETCEILING_CONST
  const pthread_mutex_t *mutex,
#else
  pthread_mutex_t       *mutex,
#endif
  int                   *prioceiling
)
{
  POSIX_Mutex_Control  *the_mutex;
  unsigned long         flags;
  Thread_queue_Context  queue_context;

  if ( prioceiling == NULL ) {
    return EINVAL;
  }

  the_mutex = _POSIX_Mutex_Get( RTEMS_DECONST( pthread_mutex_t *, mutex ) );
  POSIX_MUTEX_VALIDATE_OBJECT( the_mutex, flags );

  _POSIX_Mutex_Acquire( the_mutex, &queue_context );

  if ( _POSIX_Mutex_Get_protocol( flags ) == POSIX_MUTEX_PRIORITY_CEILING ) {
    *prioceiling = _POSIX_Priority_From_core(
      _POSIX_Mutex_Get_scheduler( the_mutex ),
      _POSIX_Mutex_Get_priority( the_mutex )
    );
  } else {
    *prioceiling = 0;
  }

  _POSIX_Mutex_Release( the_mutex, &queue_context );
  return 0;
}
