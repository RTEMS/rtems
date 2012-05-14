/*
 *  Scheduler Priority Handler / Yield
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
#include <rtems/score/isr.h>
#include <rtems/score/scheduler.h>
#include <rtems/score/schedulerpriority.h>
#include <rtems/score/thread.h>

/*
 *  INTERRUPT LATENCY:
 *    ready chain
 *    select heir
 */

void _Scheduler_priority_Yield(void)
{
  Scheduler_priority_Per_thread *sched_info;
  ISR_Level                      level;
  Thread_Control                *executing;
  Chain_Control                 *ready;

  executing  = _Thread_Executing;
  sched_info = (Scheduler_priority_Per_thread *) executing->scheduler_info;
  ready      = sched_info->ready_chain;
  _ISR_Disable( level );
    if ( !_Chain_Has_only_one_node( ready ) ) {
      _Chain_Extract_unprotected( &executing->Object.Node );
      _Chain_Append_unprotected( ready, &executing->Object.Node );

      _ISR_Flash( level );

      if ( _Thread_Is_heir( executing ) )
        _Thread_Heir = (Thread_Control *) _Chain_First( ready );
      _Thread_Dispatch_necessary = true;
    }
    else if ( !_Thread_Is_heir( executing ) )
      _Thread_Dispatch_necessary = true;

  _ISR_Enable( level );
}
