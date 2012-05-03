/*
 *  SuperCore Barrier Handler
 *
 *  DESCRIPTION:
 *
 *  This package is part of the implementation of the SuperCore Barrier Handler.
 *
 *  COPYRIGHT (c) 1989-2006.
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
#include <rtems/score/isr.h>
#include <rtems/score/corebarrier.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>

/*
 *  _CORE_barrier_Release
 *
 *  Input parameters:
 *    the_barrier            - the barrier to be flushed
 *    id                     - id of the object for a remote unblock
 *    api_barrier_mp_support - api dependent MP support actions
 *
 *  Output parameters:
 *    CORE_BARRIER_STATUS_SUCCESSFUL - if successful
 *    core error code                - if unsuccessful
 *
 *  Output parameters:
 *    returns number of threads unblocked
 */

uint32_t _CORE_barrier_Release(
  CORE_barrier_Control                *the_barrier,
#if defined(RTEMS_MULTIPROCESSING)
  Objects_Id                           id,
  CORE_barrier_API_mp_support_callout  api_barrier_mp_support
#else
  Objects_Id                           id __attribute__((unused)),
  CORE_barrier_API_mp_support_callout  api_barrier_mp_support __attribute__((unused))
#endif
)
{
  Thread_Control *the_thread;
  uint32_t        count;

  count = 0;
  while ( (the_thread = _Thread_queue_Dequeue(&the_barrier->Wait_queue)) ) {
#if defined(RTEMS_MULTIPROCESSING)
    if ( !_Objects_Is_local_id( the_thread->Object.id ) )
      (*api_barrier_mp_support) ( the_thread, id );
#endif
    count++;
  }
  the_barrier->number_of_waiting_threads = 0;
  return count;
}
