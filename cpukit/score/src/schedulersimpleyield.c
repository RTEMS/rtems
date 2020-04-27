/**
 * @file
 *
 * @ingroup RTEMSScoreScheduler
 *
 * @brief Simple Schedule Yield CPU
 */

/*
 *  COPYRIGHT (c) 2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/schedulersimpleimpl.h>

void _Scheduler_simple_Yield(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
)
{
  Scheduler_simple_Context *context;
  unsigned int              insert_priority;

  context = _Scheduler_simple_Get_context( scheduler );

  (void) node;

  _Chain_Extract_unprotected( &the_thread->Object.Node );
  insert_priority = (unsigned int) _Thread_Get_priority( the_thread );
  insert_priority = SCHEDULER_PRIORITY_APPEND( insert_priority );
  _Scheduler_simple_Insert( &context->Ready, the_thread, insert_priority );
  _Scheduler_simple_Schedule_body( scheduler, the_thread, false );
}
