/*  tasks.inl
 *
 *  This file contains the static inline implementation of all inlined
 *  routines in the with RTEMS Tasks Manager.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#ifndef __RTEMS_TASKS_inl
#define __RTEMS_TASKS_inl

#include <rtems/msgmp.h>
#include <rtems/partmp.h>
#include <rtems/regionmp.h>
#include <rtems/semmp.h>

/*PAGE
 *
 *  _RTEMS_tasks_Allocate
 *
 */

STATIC INLINE Thread_Control *_RTEMS_tasks_Allocate( void )
{
  return (Thread_Control *) _Objects_Allocate( &_Thread_Information );
}

/*PAGE
 *
 *  _RTEMS_tasks_Free
 *
 */

STATIC INLINE void _RTEMS_tasks_Free (
  Thread_Control *the_task
)
{
  _Objects_Free( &_Thread_Information, &the_task->Object );
}

/*PAGE
 *
 *  _RTEMS_tasks_Cancel_wait
 *
 */

STATIC INLINE void _RTEMS_tasks_Cancel_wait(
  Thread_Control *the_thread
)
{
  States_Control state;
  States_Control remote_state;

  state = the_thread->current_state;

  if ( _States_Is_waiting_on_thread_queue( state ) ) {
    if ( _States_Is_waiting_for_rpc_reply( state ) &&
          _States_Is_locally_blocked( state ) ) {
      remote_state = _States_Clear(
                       STATES_WAITING_FOR_RPC_REPLY | STATES_TRANSIENT,
                       state
                     );

      switch ( remote_state ) {

        case STATES_WAITING_FOR_BUFFER:
          _Partition_MP_Send_extract_proxy( the_thread );
          break;
        case STATES_WAITING_FOR_SEGMENT:
          _Region_MP_Send_extract_proxy( the_thread );
          break;
        case STATES_WAITING_FOR_SEMAPHORE:
          _Semaphore_MP_Send_extract_proxy( the_thread );
          break;
        case STATES_WAITING_FOR_MESSAGE:
          _Message_queue_MP_Send_extract_proxy( the_thread );
          break;
      }
    }
    _Thread_queue_Extract( the_thread->Wait.queue, the_thread );
  }
  else if ( _Watchdog_Is_active( &the_thread->Timer ) )
    (void) _Watchdog_Remove( &the_thread->Timer );
}

#endif
/* end of include file */
