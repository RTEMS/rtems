/**
 * @file
 *
 * @brief Update Scheduler Priority 
 * @ingroup ScoreScheduler
 */

/*
 *  Copyright (C) 2010 Gedare Bloom.
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/schedulerpriorityimpl.h>
#include <rtems/score/thread.h>

void _Scheduler_priority_Update(
  Thread_Control    *the_thread
)
{
  Scheduler_priority_Per_thread *sched_info;
  Chain_Control                 *rq;

  sched_info = (Scheduler_priority_Per_thread *) the_thread->scheduler_info;
  rq         = (Chain_Control *) _Scheduler.information;

  sched_info->ready_chain = &rq[ the_thread->current_priority ];

  _Priority_bit_map_Initialize_information(
    &sched_info->Priority_map,
    the_thread->current_priority
  );
}
