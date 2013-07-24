/**
 *  @file
 *
 *  @brief Initialize CORE Barrier
 *  @ingroup ScoreBarrier
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
#include <rtems/score/statesimpl.h>
#include <rtems/score/threadqimpl.h>

void _CORE_barrier_Initialize(
  CORE_barrier_Control       *the_barrier,
  CORE_barrier_Attributes    *the_barrier_attributes
)
{

  the_barrier->Attributes                = *the_barrier_attributes;
  the_barrier->number_of_waiting_threads = 0;

  _Thread_queue_Initialize(
    &the_barrier->Wait_queue,
    THREAD_QUEUE_DISCIPLINE_FIFO,
    STATES_WAITING_FOR_BARRIER,
    CORE_BARRIER_TIMEOUT
  );
}
