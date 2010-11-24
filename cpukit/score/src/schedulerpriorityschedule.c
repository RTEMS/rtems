/*
 *  Scheduler Handler
 *
 *  Copyright (C) 2010 Gedare Bloom.
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/context.h>
#include <rtems/score/interr.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/score/priority.h>
#include <rtems/score/percpu.h>
#include <rtems/score/scheduler.h>
#include <rtems/score/schedulerpriority.h>
#include <rtems/score/thread.h>

/*
 * _Scheduler_priority_Schedule
 *
 * This kernel routine implements scheduling decision logic for priority-based
 * scheduling.  
 *
 * Input parameters:
 *   the_scheduler  - pointer to scheduler control
 *   the_thread     - pointer to thread control block
 *
 * Output parameters:  NONE
 *
 *  INTERRUPT LATENCY:
 */

void _Scheduler_priority_Schedule(
  Scheduler_Control     *the_scheduler
)
{
  _Scheduler_priority_Schedule_body( the_scheduler );
}
