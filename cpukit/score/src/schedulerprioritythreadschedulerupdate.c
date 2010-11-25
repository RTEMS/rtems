/*
 *  Scheduler Handler
 *
 *  Copyright (C) 2010 Gedare Bloom.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/config.h>
#include <rtems/score/chain.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/score/priority.h>
#include <rtems/score/prioritybitmap.h>
#include <rtems/score/scheduler.h>
#include <rtems/score/schedulerpriority.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>

/*
 *  _Scheduler_priority_Thread_scheduler_update
 *
 * Updates @a the_thread->scheduler
 *
 *  Input parameters:
 *    the_scheduler - pointer to scheduler control
 *    the_thread    - pointer to thread control block
 *
 *  Output parameters: NONE
 */

void _Scheduler_priority_Thread_scheduler_update (
    Scheduler_Control *the_scheduler,
    Thread_Control    *the_thread
)
{
  Chain_Control *rq = the_scheduler->Ready_queues.priority;
  the_thread->scheduler.priority->ready_chain = &rq[
    the_thread->current_priority 
  ];

  _Priority_bit_map_Initialize_information( 
      &the_thread->scheduler.priority->Priority_map, 
      the_thread->current_priority 
  );
}
