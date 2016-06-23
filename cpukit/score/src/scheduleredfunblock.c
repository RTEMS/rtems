/**
 * @file
 *
 * @brief Scheduler EDF Unblock
 * @ingroup ScoreScheduler
 */

/*
 *  Copyright (C) 2011 Petr Benes.
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/scheduleredfimpl.h>
#include <rtems/score/schedulerimpl.h>
#include <rtems/score/thread.h>

Scheduler_Void_or_thread _Scheduler_EDF_Unblock(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread
)
{
  Scheduler_EDF_Context *context;
  Scheduler_EDF_Node    *node;
  Priority_Control       priority;
  bool                   prepend_it;

  context = _Scheduler_EDF_Get_context( scheduler );
  node = _Scheduler_EDF_Thread_get_node( the_thread );
  priority = _Scheduler_Node_get_priority( &node->Base, &prepend_it );
  (void) prepend_it;

  node->current_priority = priority;
  _Scheduler_EDF_Enqueue( context, node, priority );

  /*
   *  If the thread that was unblocked is more important than the heir,
   *  then we have a new heir.  This may or may not result in a
   *  context switch.
   *
   *  Normal case:
   *    If the current thread is preemptible, then we need to do
   *    a context switch.
   *  Pseudo-ISR case:
   *    Even if the thread isn't preemptible, if the new heir is
   *    a pseudo-ISR system task, we need to do a context switch.
   */
  if ( priority < _Thread_Heir->current_priority ) {
    _Scheduler_Update_heir(
      the_thread,
      priority == ( SCHEDULER_EDF_PRIO_MSB | PRIORITY_PSEUDO_ISR )
    );
  }

  SCHEDULER_RETURN_VOID_OR_NULL;
}
