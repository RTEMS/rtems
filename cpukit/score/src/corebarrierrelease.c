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
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/corebarrierimpl.h>
#include <rtems/score/objectimpl.h>
#include <rtems/score/threadqimpl.h>

uint32_t _CORE_barrier_Do_surrender(
  CORE_barrier_Control    *the_barrier
#if defined(RTEMS_MULTIPROCESSING)
  ,
  Thread_queue_MP_callout  mp_callout,
  Objects_Id               mp_id
#endif
)
{
  Thread_Control *the_thread;
  uint32_t        count;

  count = 0;
  while (
    (
      the_thread = _Thread_queue_Dequeue(
        &the_barrier->Wait_queue,
        CORE_BARRIER_TQ_OPERATIONS,
        mp_callout,
        mp_id
      )
    )
  ) {
    count++;
  }
  the_barrier->number_of_waiting_threads = 0;
  return count;
}
