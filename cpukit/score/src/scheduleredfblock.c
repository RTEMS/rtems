/*
 *  Copyright (C) 2011 Petr Benes.
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
#include <rtems/score/priority.h>
#include <rtems/score/scheduler.h>
#include <rtems/score/scheduleredf.h>
#include <rtems/score/thread.h>

void _Scheduler_EDF_Block(
  Thread_Control    *the_thread
)
{
  _Scheduler_EDF_Extract( the_thread );

  /* TODO: flash critical section? */

  if ( _Thread_Is_heir( the_thread ) )
    _Scheduler_EDF_Schedule();

  if ( _Thread_Is_executing( the_thread ) )
    _Thread_Dispatch_necessary = true;
}
