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
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/regionimpl.h>
#include <rtems/score/status.h>
#include <rtems/score/threadqimpl.h>

void _Region_Process_queue(
  Region_Control *the_region
)
{
  Per_CPU_Control *cpu_self;
  Thread_Control  *the_thread;
  void            *the_segment;

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
  cpu_self = _Thread_Dispatch_disable();
  _Region_Unlock( the_region );

  /*
   *  NOTE: The following loop is O(n) where n is the number of
   *        threads whose memory request is satisfied.
   */
  for ( ; ; ) {
    the_thread = _Thread_queue_First(
      &the_region->Wait_queue,
      the_region->wait_operations
    );

    if ( the_thread == NULL )
      break;

    the_segment = (void **) _Region_Allocate_segment(
      the_region,
      the_thread->Wait.count
    );

    if ( the_segment == NULL )
      break;

    *(void **)the_thread->Wait.return_argument = the_segment;
    _Thread_queue_Extract( the_thread );
    the_thread->Wait.return_code = STATUS_SUCCESSFUL;
  }

  _Thread_Dispatch_enable( cpu_self );
}
