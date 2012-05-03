/*
 *  Scheduler Handler
 *
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

#include <rtems/system.h>
#include <rtems/score/context.h>
#include <rtems/score/interr.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/score/priority.h>
#include <rtems/score/scheduler.h>
#include <rtems/score/schedulerpriority.h>
#include <rtems/score/thread.h>

void _Scheduler_priority_Block(
  Thread_Control   *the_thread
)
{
  _Scheduler_priority_Ready_queue_extract( the_thread );

  /* TODO: flash critical section? */

  if ( _Thread_Is_heir( the_thread ) )
     _Scheduler_priority_Schedule_body();

  if ( _Thread_Is_executing( the_thread ) )
    _Thread_Dispatch_necessary = true;

}
