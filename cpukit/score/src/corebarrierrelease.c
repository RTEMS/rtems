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

uint32_t _CORE_barrier_Do_surrender(
  CORE_barrier_Control      *the_barrier,
  Thread_queue_Flush_filter  filter,
#if defined(RTEMS_MULTIPROCESSING)
  Thread_queue_MP_callout    mp_callout,
#endif
  ISR_lock_Context          *lock_context
)
{
  the_barrier->number_of_waiting_threads = 0;
  return _Thread_queue_Flush_critical(
    &the_barrier->Wait_queue.Queue,
    CORE_BARRIER_TQ_OPERATIONS,
    filter,
    mp_callout,
    lock_context
  );
}
