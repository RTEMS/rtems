/**
 *  @file
 *
 *  @brief Scheduler Priority Yield
 *  @ingroup ScoreScheduler
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

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/scheduler.h>
#include <rtems/score/schedulerpriority.h>
#include <rtems/score/thread.h>

void _Scheduler_priority_Yield( Thread_Control *thread )
{
  Scheduler_priority_Per_thread *sched_info;
  ISR_Level                      level;
  Chain_Control                 *ready;

  sched_info = (Scheduler_priority_Per_thread *) thread->scheduler_info;
  ready      = sched_info->ready_chain;
  _ISR_Disable( level );
    if ( !_Chain_Has_only_one_node( ready ) ) {
      _Chain_Extract_unprotected( &thread->Object.Node );
      _Chain_Append_unprotected( ready, &thread->Object.Node );

      _ISR_Flash( level );

      if ( _Thread_Is_heir( thread ) )
        _Thread_Heir = (Thread_Control *) _Chain_First( ready );
      _Thread_Dispatch_necessary = true;
    }
    else if ( !_Thread_Is_heir( thread ) )
      _Thread_Dispatch_necessary = true;

  _ISR_Enable( level );
}
