/**
 *  @file
 *
 *  @brief Scheduler Priority Enqueue
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

void _Scheduler_priority_Enqueue(
  Thread_Control      *the_thread
)
{
  Scheduler_priority_Control *scheduler = _Scheduler_priority_Instance();

  _Scheduler_priority_Ready_queue_enqueue( the_thread, &scheduler->Bit_map );
}
