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
  Scheduler_Control *scheduler_base,
  Thread_Control    *the_thread
)
{
  Scheduler_EDF_Control *scheduler =
    _Scheduler_EDF_Self_from_base( scheduler_base );
  ISR_Level              level;

  Scheduler_EDF_Per_thread *thread_info =
    (Scheduler_EDF_Per_thread *) the_thread->scheduler_info;
  RBTree_Node *thread_node = &(thread_info->Node);

  _ISR_Disable( level );

  /*
   * The RBTree has more than one node, enqueue behind the tasks
   * with the same priority in case there are such ones.
   */
  _RBTree_Extract( &scheduler->Ready, thread_node );
  _RBTree_Insert( &scheduler->Ready, thread_node );

  _ISR_Flash( level );

  _Scheduler_EDF_Schedule_body( scheduler_base, the_thread, false );

  _ISR_Enable( level );
}
