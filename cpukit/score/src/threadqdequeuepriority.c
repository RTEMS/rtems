/**
 * @file
 *
 * @brief  Thread Queue Dequeue Priority
 * @ingroup ScoreThreadQ
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

#include <rtems/score/threadqimpl.h>
#include <rtems/score/rbtreeimpl.h>
#include <rtems/score/isrlevel.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/watchdogimpl.h>

Thread_Control *_Thread_queue_Dequeue_priority(
  Thread_queue_Control *the_thread_queue
)
{
  ISR_Level       level;
  Thread_Control *the_thread = NULL;  /* just to remove warnings */
  RBTree_Node    *first;

  _ISR_Disable( level );

  first = _RBTree_Get( &the_thread_queue->Queues.Priority, RBT_LEFT );
  if ( !first ) {
    /*
     * We did not find a thread to unblock.
     */
    _ISR_Enable( level );
    return NULL;
  }

  /*
   * We found a thread to unblock.
   */

  the_thread = _RBTree_Container_of( first, Thread_Control, RBNode );
  the_thread->Wait.queue = NULL;
  if ( !_Watchdog_Is_active( &the_thread->Timer ) ) {
    _ISR_Enable( level );
  } else {
    _Watchdog_Deactivate( &the_thread->Timer );
    _ISR_Enable( level );
    (void) _Watchdog_Remove( &the_thread->Timer );
  }

  _Thread_Unblock( the_thread );

#if defined(RTEMS_MULTIPROCESSING)
  if ( !_Objects_Is_local_id( the_thread->Object.id ) )
    _Thread_MP_Free_proxy( the_thread );
#endif
  return( the_thread );
}
