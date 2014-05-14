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
  Thread_Control    *unblocked;
  POSIX_API_Control *api;
  bool               previous_life_protection;

  api = the_thread->API_Extensions[ THREAD_API_POSIX ];

  _Assert( _Debug_Is_thread_dispatching_allowed() );

  previous_life_protection = _Thread_Set_life_protection( true );
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
      _Thread_Set_state( the_thread, STATES_WAITING_FOR_JOIN_AT_EXIT );
      _Thread_Enable_dispatch();
      /* now waiting for thread to arrive */
      _Thread_Disable_dispatch();
    }
  }

  /*
   *  Now shut down the thread
   */
  _Thread_Close( the_thread, _Thread_Executing );

  _Thread_Enable_dispatch();
  _Thread_Set_life_protection( previous_life_protection );
}

void pthread_exit(
  void  *value_ptr
)
{
  _POSIX_Thread_Exit( _Thread_Get_executing(), value_ptr );
}
