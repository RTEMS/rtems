/**
 * @file
 *
 * @ingroup ScoreProtHeap
 *
 * @brief Protected Heap Handler implementation.
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/protectedheap.h>

bool _Protected_heap_Walk(
  Heap_Control *the_heap,
  int           source,
  bool          do_dump
)
{
  bool    status;

  /*
   * If we are called from within a dispatching critical section,
   * then it is forbidden to lock a mutex.  But since we are inside
   * a critical section, it should be safe to walk it unlocked.
   *
   * NOTE: Dispatching is also disabled during initialization.
   */
  if ( _Thread_Dispatch_in_critical_section() == false ) {
    _RTEMS_Lock_allocator();
      status = _Heap_Walk( the_heap, source, do_dump );
    _RTEMS_Unlock_allocator();
  } else {
    status = _Heap_Walk( the_heap, source, do_dump );
  }
  return status;
}
