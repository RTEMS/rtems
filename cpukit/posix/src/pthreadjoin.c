/**
 * @file
 *
 * @brief Suspends Execution of Calling Thread until Target Thread Terminates 
 * @ingroup POSIXAPI
 */

/*
 *  16.1.3 Wait for Thread Termination, P1003.1c/Draft 10, p. 147
 *
 *  COPYRIGHT (c) 1989-2011.
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

#include <rtems/posix/pthreadimpl.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/threadqimpl.h>

int pthread_join(
  pthread_t   thread,
  void      **value_ptr
)
{
  register Thread_Control *the_thread;
  POSIX_API_Control       *api;
  Objects_Locations        location;
  void                    *return_pointer;
  Thread_Control          *executing;

on_EINTR:
  the_thread = _Thread_Get( thread, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      api = the_thread->API_Extensions[ THREAD_API_POSIX ];

      if ( api->detachstate == PTHREAD_CREATE_DETACHED ) {
        _Objects_Put( &the_thread->Object );
        return EINVAL;
      }

      executing = _Thread_Executing;

      if ( executing == the_thread ) {
        _Objects_Put( &the_thread->Object );
        return EDEADLK;
      }

      /*
       *  Put ourself on the threads join list
       */

      if ( the_thread->current_state ==
             (STATES_WAITING_FOR_JOIN_AT_EXIT | STATES_TRANSIENT) ) {
         return_pointer = the_thread->Wait.return_argument;
         _Thread_Clear_state(
           the_thread,
           (STATES_WAITING_FOR_JOIN_AT_EXIT | STATES_TRANSIENT)
         );
      } else {
        executing->Wait.return_argument = &return_pointer;
        _Thread_queue_Enter_critical_section( &api->Join_List );
        _Thread_queue_Enqueue(
          &api->Join_List,
          executing,
          WATCHDOG_NO_TIMEOUT
        );
      }
      _Objects_Put( &the_thread->Object );

      if ( executing->Wait.return_code == EINTR )
        goto on_EINTR;

      if ( value_ptr )
        *value_ptr = return_pointer;
      return 0;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
#endif
    case OBJECTS_ERROR:
      break;
  }

  return ESRCH;
}
