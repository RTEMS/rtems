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
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/scheduleredfimpl.h>

void _Scheduler_EDF_Yield( Thread_Control *thread )
{
  ISR_Level                 level;

  Scheduler_EDF_Per_thread *thread_info =
    (Scheduler_EDF_Per_thread *) thread->scheduler_info;
  RBTree_Node *thread_node = &(thread_info->Node);

  _ISR_Disable( level );

  /*
   * The RBTree has more than one node, enqueue behind the tasks
   * with the same priority in case there are such ones.
   */
  _RBTree_Extract( &_Scheduler_EDF_Ready_queue, thread_node );
  _RBTree_Insert( &_Scheduler_EDF_Ready_queue, thread_node );

  _ISR_Flash( level );

  _Scheduler_EDF_Schedule_body( thread, false );

  _ISR_Enable( level );
}
