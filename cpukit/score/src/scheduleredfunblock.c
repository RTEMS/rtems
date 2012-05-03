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
#include <rtems/score/scheduler.h>
#include <rtems/score/scheduleredf.h>

void _Scheduler_EDF_Unblock(
  Thread_Control    *the_thread
)
{
  _Scheduler_EDF_Enqueue(the_thread);
  /* TODO: flash critical section? */

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
  if ( _Scheduler_Is_priority_lower_than(
         _Thread_Heir->current_priority,
         the_thread->current_priority )) {
    _Thread_Heir = the_thread;
    if ( _Thread_Executing->is_preemptible ||
         the_thread->current_priority == 0 )
      _Thread_Dispatch_necessary = true;
  }
}
