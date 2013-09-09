/**
 *  @file
 *
 *  @brief Process Region Queue
 *  @ingroup ClassicRegion
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/regionimpl.h>
#include <rtems/score/apimutex.h>
#include <rtems/score/threadqimpl.h>

void _Region_Process_queue(
  Region_Control *the_region
)
{
  Thread_Control *the_thread;
  void           *the_segment;
  /*
   *  Switch from using the memory allocation mutex to using a
   *  dispatching disabled critical section.  We have to do this
   *  because this thread may unblock one or more threads that were
   *  waiting on memory.
   *
   *  NOTE: Be sure to disable dispatching before unlocking the mutex
   *        since we do not want to open a window where a context
   *        switch could occur.
   */
  _Thread_Disable_dispatch();
  _RTEMS_Unlock_allocator();

  /*
   *  NOTE: The following loop is O(n) where n is the number of
   *        threads whose memory request is satisfied.
   */
  for ( ; ; ) {
    the_thread = _Thread_queue_First( &the_region->Wait_queue );

    if ( the_thread == NULL )
      break;

    the_segment = (void **) _Region_Allocate_segment(
      the_region,
      the_thread->Wait.count
    );

    if ( the_segment == NULL )
      break;

    *(void **)the_thread->Wait.return_argument = the_segment;
    the_region->number_of_used_blocks += 1;
    _Thread_queue_Extract( &the_region->Wait_queue, the_thread );
    the_thread->Wait.return_code = RTEMS_SUCCESSFUL;
  }
  _Thread_Enable_dispatch();
}
