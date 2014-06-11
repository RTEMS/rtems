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

Scheduler_Void_or_thread _Scheduler_simple_Unblock(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread
)
{
  Scheduler_simple_Context *context =
    _Scheduler_simple_Get_context( scheduler );

  _Scheduler_simple_Insert_priority_fifo( &context->Ready, the_thread );

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
  if ( the_thread->current_priority < _Thread_Heir->current_priority ) {
    _Thread_Heir = the_thread;
    if ( _Thread_Executing->is_preemptible ||
        the_thread->current_priority == 0 )
      _Thread_Dispatch_necessary = true;
  }

  SCHEDULER_RETURN_VOID_OR_NULL;
}
