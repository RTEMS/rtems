/**
 * @file
 * 
 * @brief Scheduler Simple Handler / Unblock
 * @ingroup RTEMSScoreScheduler
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
  unsigned int              priority;
  unsigned int              insert_priority;

  (void) node;

  context = _Scheduler_simple_Get_context( scheduler );
  priority = _Thread_Get_priority( the_thread );
  insert_priority = SCHEDULER_PRIORITY_APPEND( priority );
  _Scheduler_simple_Insert( &context->Ready, the_thread, insert_priority );

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
