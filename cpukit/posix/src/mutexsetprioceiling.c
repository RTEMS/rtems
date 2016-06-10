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
  POSIX_Mutex_Control *the_mutex;
  int                  error;
  int                  unlock_error;

  if ( old_ceiling == NULL ) {
    return EINVAL;
  }

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

  if ( the_mutex->protocol == POSIX_MUTEX_PRIORITY_CEILING ) {
    const Scheduler_Control *scheduler;
    bool                     valid;
    Priority_Control         new_priority;
    Priority_Control         old_priority;

    scheduler = _CORE_ceiling_mutex_Get_scheduler( &the_mutex->Mutex );
    old_priority = _CORE_ceiling_mutex_Get_priority( &the_mutex->Mutex );
    *old_ceiling = _POSIX_Priority_From_core( scheduler, old_priority );

    new_priority = _POSIX_Priority_To_core( scheduler, prioceiling, &valid );
    if ( valid ) {
      _CORE_ceiling_mutex_Set_priority( &the_mutex->Mutex, new_priority );
      error = 0;
    } else {
      error = EINVAL;
    }
  } else {
    *old_ceiling = 0;
    error = 0;
  }

  unlock_error = pthread_mutex_unlock( mutex );
  _Assert( unlock_error == 0 );
  (void) unlock_error;
  return error;
}
