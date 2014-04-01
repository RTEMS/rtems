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
 *  http://www.rtems.org/license/LICENSE.
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
  Scheduler_priority_Control *scheduler = _Scheduler_priority_Instance();

  _Scheduler_priority_Update_body(
    the_thread,
    &scheduler->Bit_map,
    &scheduler->Ready[ 0 ]
  );
}
