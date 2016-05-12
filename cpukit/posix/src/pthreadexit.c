/**
 * @file
 *
 * @brief POSIX Thread Exit Shared Helper
 * @ingroup POSIX_THREAD Thread API Extension
 */

/*
 *  COPYRIGHT (c) 1989-2011.
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

#include <rtems/posix/pthreadimpl.h>
#include <rtems/score/assert.h>
#include <rtems/score/apimutex.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/threadqimpl.h>

void _POSIX_Thread_Exit(
  Thread_Control *the_thread,
  void           *value_ptr
)
{
  Thread_Control    *executing;
  Thread_Control    *unblocked;
  POSIX_API_Control *api;

  api = the_thread->API_Extensions[ THREAD_API_POSIX ];

  _Assert( _Debug_Is_thread_dispatching_allowed() );

  _Thread_Disable_dispatch();

  the_thread->Wait.return_argument = value_ptr;

  /*
   * Process join
   */
  if ( api->detachstate == PTHREAD_CREATE_JOINABLE ) {
    unblocked = _POSIX_Threads_Join_dequeue( api );
    if ( unblocked ) {
      do {
        *(void **)unblocked->Wait.return_argument = value_ptr;
      } while ( ( unblocked = _POSIX_Threads_Join_dequeue( api ) ) );
    } else {
      _Thread_Set_state( the_thread, STATES_WAITING_FOR_JOIN_AT_EXIT );
      _Thread_Enable_dispatch();
      /* now waiting for thread to arrive */
      _Thread_Disable_dispatch();
    }
  }

  executing = _Thread_Executing;

  /*
   *  Now shut down the thread
   */
  if ( the_thread == executing ) {
    _Thread_Exit( executing );
  } else {
    _Thread_Close( the_thread, executing );
  }

  _Thread_Enable_dispatch();
}

void pthread_exit(
  void  *value_ptr
)
{
  _POSIX_Thread_Exit( _Thread_Get_executing(), value_ptr );
  RTEMS_UNREACHABLE();
}
