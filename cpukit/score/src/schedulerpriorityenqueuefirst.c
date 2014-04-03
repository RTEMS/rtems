/**
 *  @file
 *
 *  @brief Scheduler Priority Enqueue First
 *  @ingroup ScoreScheduler
 */

/*
 *  COPYRIGHT (c) 2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/schedulerpriorityimpl.h>

void _Scheduler_priority_Enqueue_first(
  Scheduler_Control *base,
  Thread_Control    *the_thread
)
{
  Scheduler_priority_Control *self =
    _Scheduler_priority_Self_from_base( base );

  _Scheduler_priority_Ready_queue_enqueue_first(
    the_thread,
    &self->Bit_map
  );
}

