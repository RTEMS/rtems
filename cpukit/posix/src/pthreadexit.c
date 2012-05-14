/*
 *  16.1.5.1 Thread Termination, p1003.1c/Draft 10, p. 150
 *
 *  NOTE: Key destructors are executed in the POSIX api delete extension.
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
#include <rtems/score/apimutex.h>
#include <rtems/score/thread.h>
#include <rtems/posix/pthread.h>


void _POSIX_Thread_Exit(
  Thread_Control *the_thread,
  void           *value_ptr
)
{
  Objects_Information  *the_information;
  Thread_Control       *unblocked;
  POSIX_API_Control    *api;

  the_information = _Objects_Get_information_id( the_thread->Object.id );

  api = the_thread->API_Extensions[ THREAD_API_POSIX ];


  /*
   * The_information has to be non-NULL.  Otherwise, we couldn't be
   * running in a thread of this API and class.
   *
   * NOTE: Lock and unlock in different order so we do not throw a
   *       fatal error when locking the allocator mutex.  And after
   *       we unlock, we want to defer the context switch until we
   *       are ready to be switched out.  Otherwise, an ISR could
   *       occur and preempt us out while we still hold the
   *       allocator mutex.
   */

  _RTEMS_Lock_allocator();
    _Thread_Disable_dispatch();

      the_thread->Wait.return_argument = value_ptr;

      /*
       * Process join
       */
      if ( api->detachstate == PTHREAD_CREATE_JOINABLE ) {
        unblocked = _Thread_queue_Dequeue( &api->Join_List );
        if ( unblocked ) {
          do {
            *(void **)unblocked->Wait.return_argument = value_ptr;
          } while ( (unblocked = _Thread_queue_Dequeue( &api->Join_List )) );
        } else {
          _Thread_Set_state(
            the_thread,
            STATES_WAITING_FOR_JOIN_AT_EXIT | STATES_TRANSIENT
          );
           _RTEMS_Unlock_allocator();
          _Thread_Enable_dispatch();
          /* now waiting for thread to arrive */
          _RTEMS_Lock_allocator();
          _Thread_Disable_dispatch();
        }
      }

      /*
       *  Now shut down the thread
       */
      _Thread_Close( the_information, the_thread );

      _POSIX_Threads_Free( the_thread );

    _RTEMS_Unlock_allocator();
  _Thread_Enable_dispatch();
}

void pthread_exit(
  void  *value_ptr
)
{
  _POSIX_Thread_Exit( _Thread_Executing, value_ptr );
}
