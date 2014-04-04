/**
 * @file
 *
 * @brief Scheduler EDF Yield
 *
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

void _Scheduler_EDF_Yield(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread
)
{
  Scheduler_EDF_Context *context =
    _Scheduler_EDF_Get_context( scheduler );
  ISR_Level              level;

  Scheduler_EDF_Per_thread *thread_info =
    (Scheduler_EDF_Per_thread *) the_thread->scheduler_info;
  RBTree_Node *thread_node = &(thread_info->Node);

  _ISR_Disable( level );

  /*
   * The RBTree has more than one node, enqueue behind the tasks
   * with the same priority in case there are such ones.
   */
  _RBTree_Extract( &context->Ready, thread_node );
  _RBTree_Insert( &context->Ready, thread_node );

  _ISR_Flash( level );

  _Scheduler_EDF_Schedule_body( scheduler, the_thread, false );

  _ISR_Enable( level );
}
