/*
 *  Thread Handler
 *
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/apiext.h>
#include <rtems/score/context.h>
#include <rtems/score/interr.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/score/priority.h>
#include <rtems/score/states.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/score/userext.h>
#include <rtems/score/wkspace.h>

/*
 *  _Thread_Reset
 *
 *  DESCRIPTION:
 *
 *  This routine resets a thread to its initial stat but does
 *  not actually restart it.  Some APIs do this in separate
 *  operations and this division helps support this.
 */

void _Thread_Reset(
  Thread_Control            *the_thread,
  void                      *pointer_argument,
  Thread_Entry_numeric_type  numeric_argument
)
{
  the_thread->resource_count   = 0;
  the_thread->is_preemptible   = the_thread->Start.is_preemptible;
  the_thread->budget_algorithm = the_thread->Start.budget_algorithm;
  the_thread->budget_callout   = the_thread->Start.budget_callout;

  the_thread->Start.pointer_argument = pointer_argument;
  the_thread->Start.numeric_argument = numeric_argument;

  if ( !_Thread_queue_Extract_with_proxy( the_thread ) ) {

    if ( _Watchdog_Is_active( &the_thread->Timer ) )
      (void) _Watchdog_Remove( &the_thread->Timer );
  }

  if ( the_thread->current_priority != the_thread->Start.initial_priority ) {
    the_thread->real_priority = the_thread->Start.initial_priority;
    _Thread_Set_priority( the_thread, the_thread->Start.initial_priority );
  }
}
