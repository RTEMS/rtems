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

#include <rtems/system.h>
#include <rtems/score/thread.h>
#include <rtems/posix/pthread.h>

int pthread_join(
  pthread_t   thread,
  void      **value_ptr
)
{
  register Thread_Control *the_thread;
  POSIX_API_Control       *api;
  Objects_Locations        location;
  void                    *return_pointer;

on_EINTR:
  the_thread = _Thread_Get( thread, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      api = the_thread->API_Extensions[ THREAD_API_POSIX ];

      if ( api->detachstate == PTHREAD_CREATE_DETACHED ) {
        _Thread_Enable_dispatch();
        return EINVAL;
      }

      if ( _Thread_Is_executing( the_thread ) ) {
        _Thread_Enable_dispatch();
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
	_Thread_Executing->Wait.return_argument = &return_pointer;
        _Thread_queue_Enter_critical_section( &api->Join_List );
        _Thread_queue_Enqueue( &api->Join_List, WATCHDOG_NO_TIMEOUT );
      }
      _Thread_Enable_dispatch();

      if ( _Thread_Executing->Wait.return_code == EINTR )
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
