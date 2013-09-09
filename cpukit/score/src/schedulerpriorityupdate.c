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
  Chain_Control *ready_queues = _Scheduler_priority_Get_ready_queues();

  _Scheduler_priority_Update_body( the_thread, ready_queues );
}
