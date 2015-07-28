/**
 * @file
 *
 * @brief POSIX Condition Variables Wait Support
 * @ingroup POSIX_COND_VARS
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

#include <pthread.h>
#include <errno.h>

#include <rtems/system.h>
#include <rtems/score/watchdog.h>
#include <rtems/score/statesimpl.h>
#include <rtems/posix/condimpl.h>
#include <rtems/posix/muteximpl.h>

int _POSIX_Condition_variables_Wait_support(
  pthread_cond_t            *cond,
  pthread_mutex_t           *mutex,
  Watchdog_Interval          timeout,
  bool                       already_timedout
)
{
  POSIX_Condition_variables_Control          *the_cond;
  POSIX_Mutex_Control                        *the_mutex;
  Objects_Locations                           location;
  int                                         status;
  int                                         mutex_status;
  Thread_Control                             *executing;

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


      mutex_status = pthread_mutex_unlock( mutex );
      /*
       *  Historically, we ignored the return code since the behavior
       *  is undefined by POSIX. But GNU/Linux returns EPERM in this
       *  case, so we follow their lead.
       */
      if ( mutex_status ) {
        _Objects_Put( &the_cond->Object );
        return EPERM;
      }

      if ( !already_timedout ) {
        the_cond->Mutex = *mutex;

        executing = _Thread_Executing;
        executing->Wait.return_code = 0;
        executing->Wait.id          = *cond;

        _Thread_queue_Enqueue(
          &the_cond->Wait_queue,
          executing,
          STATES_WAITING_FOR_CONDITION_VARIABLE
            | STATES_INTERRUPTIBLE_BY_SIGNAL,
          timeout,
          ETIMEDOUT
        );

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
        status = executing->Wait.return_code;
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
