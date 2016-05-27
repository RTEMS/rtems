/**
 * @file
 *
 * @brief Changes the Priority Ceiling of a Mutex and Releases it
 * @ingroup POSIXAPI
 */

/*
 *  COPYRIGHT (c) 1989-2009.
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

int pthread_mutex_setprioceiling(
  pthread_mutex_t   *mutex,
  int                prioceiling,
  int               *old_ceiling
)
{
  register POSIX_Mutex_Control *the_mutex;
  Priority_Control              the_priority;
  int                           error;

  if ( !old_ceiling )
    return EINVAL;

  if ( !_POSIX_Priority_Is_valid( prioceiling ) )
    return EINVAL;

  the_priority = _POSIX_Priority_To_core( prioceiling );

  /*
   *  Must acquire the mutex before we can change it's ceiling.
   *  POSIX says block until we acquire it.
   */
  error = pthread_mutex_lock( mutex );
  if ( error != 0 ) {
    _Assert( error == EINVAL );
    return EINVAL;
  }

  the_mutex = _POSIX_Mutex_Get_no_protection( mutex );
  _Assert( the_mutex != NULL );

  *old_ceiling = _POSIX_Priority_From_core(
    the_mutex->Mutex.Attributes.priority_ceiling
  );
  the_mutex->Mutex.Attributes.priority_ceiling = the_priority;

  error = pthread_mutex_unlock( mutex );
  _Assert( error == 0 );
  (void) error;
  return 0;
}
