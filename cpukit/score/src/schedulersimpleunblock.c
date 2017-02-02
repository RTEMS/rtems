/**
 * @file 
 * 
 * @brief Scheduler Simple Handler / Unblock
 * @ingroup ScoreScheduler
 */

/*
 *  COPYRIGHT (c) 2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/schedulersimpleimpl.h>
#include <rtems/score/thread.h>

void _Scheduler_simple_Unblock(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
)
{
  Scheduler_simple_Context *context;
  Priority_Control          priority;

  (void) node;

  context = _Scheduler_simple_Get_context( scheduler );
  _Scheduler_simple_Insert_priority_fifo( &context->Ready, the_thread );
  priority = _Thread_Get_priority( the_thread );

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
  if ( priority < _Thread_Get_priority( _Thread_Heir ) ) {
    _Scheduler_Update_heir(
      the_thread,
      priority == PRIORITY_PSEUDO_ISR
    );
  }
}
