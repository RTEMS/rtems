/**
 * @file
 *
 * @brief POSIX Condition Variables Wait Support
 * @ingroup POSIX_COND_VARS
 */

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

int _POSIX_Condition_variables_Wait_support(
  pthread_cond_t            *cond,
  pthread_mutex_t           *mutex,
  Watchdog_Interval          timeout,
  bool                       already_timedout
)
{
  register POSIX_Condition_variables_Control *the_cond;
  POSIX_Mutex_Control                        *the_mutex;
  Objects_Locations                           location;
  int                                         status;
  int                                         mutex_status;

  the_mutex = _POSIX_Mutex_Get( mutex, &location );
  if ( !the_mutex ) {
     return EINVAL;
  }

  _Objects_Put_without_thread_dispatch( &the_mutex->Object );

  the_cond = _POSIX_Condition_variables_Get( cond, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:

      if ( the_cond->Mutex && ( the_cond->Mutex != *mutex ) ) {
        _Objects_Put( &the_cond->Object );
        return EINVAL;
      }

      (void) pthread_mutex_unlock( mutex );
/* XXX ignore this for now  since behavior is undefined
      if ( mutex_status ) {
        _Objects_Put( &the_cond->Object );
        return EINVAL;
      }
*/

      if ( !already_timedout ) {
        the_cond->Mutex = *mutex;

        _Thread_queue_Enter_critical_section( &the_cond->Wait_queue );
        _Thread_Executing->Wait.return_code = 0;
        _Thread_Executing->Wait.queue       = &the_cond->Wait_queue;
        _Thread_Executing->Wait.id          = *cond;

        _Thread_queue_Enqueue( &the_cond->Wait_queue, timeout );

        _Objects_Put( &the_cond->Object );

        /*
         *  Switch ourself out because we blocked as a result of the
         *  _Thread_queue_Enqueue.
         */

        /*
         *  If the thread is interrupted, while in the thread queue, by
         *  a POSIX signal, then pthread_cond_wait returns spuriously,
         *  according to the POSIX standard. It means that pthread_cond_wait
         *  returns a success status, except for the fact that it was not
         *  woken up a pthread_cond_signal or a pthread_cond_broadcast.
         */
        status = _Thread_Executing->Wait.return_code;
        if ( status == EINTR )
          status = 0;

      } else {
        _Objects_Put( &the_cond->Object );
        status = ETIMEDOUT;
      }

      /*
       *  When we get here the dispatch disable level is 0.
       */

      mutex_status = pthread_mutex_lock( mutex );
      if ( mutex_status )
        return EINVAL;

      return status;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
#endif
    case OBJECTS_ERROR:
      break;
  }

  return EINVAL;
}
