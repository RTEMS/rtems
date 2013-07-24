/**
 * @file
 *
 * @brief Manually releases the Barrier
 *
 * @ingroup ScoreBarrier
 */

/*
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

#include <rtems/score/corebarrierimpl.h>
#include <rtems/score/objectimpl.h>
#include <rtems/score/threadqimpl.h>

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
