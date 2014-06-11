/**
 * @file
 *
 * @brief Simple Schedule Yield CPU
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

Scheduler_Void_or_thread _Scheduler_simple_Yield(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread
)
{
  Scheduler_simple_Context *context =
    _Scheduler_simple_Get_context( scheduler );

  _Chain_Extract_unprotected( &the_thread->Object.Node );
  _Scheduler_simple_Insert_priority_fifo( &context->Ready, the_thread );
  _Scheduler_simple_Schedule_body( scheduler, the_thread, false );

  SCHEDULER_RETURN_VOID_OR_NULL;
}
