/**
 * @file
 *
 * @brief Manually releases the Barrier
 *
 * @ingroup RTEMSScoreBarrier
 */

/*
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/corebarrierimpl.h>

uint32_t _CORE_barrier_Do_flush(
  CORE_barrier_Control      *the_barrier,
  Thread_queue_Flush_filter  filter,
  Thread_queue_Context      *queue_context
)
{
  the_barrier->number_of_waiting_threads = 0;
  return _Thread_queue_Flush_critical(
    &the_barrier->Wait_queue.Queue,
    CORE_BARRIER_TQ_OPERATIONS,
    filter,
    queue_context
  );
}
