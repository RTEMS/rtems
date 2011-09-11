/*
 *  Copyright (C) 2011 Petr Benes.
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/scheduler.h>
#include <rtems/score/scheduleredf.h>

void _Scheduler_EDF_Schedule(void)
{
  RBTree_Node *first_node =
    _RBTree_Peek(&_Scheduler_EDF_Ready_queue, RBT_LEFT);
  Scheduler_EDF_Per_thread *sched_info =
    _RBTree_Container_of(first_node, Scheduler_EDF_Per_thread, Node);

  _Thread_Heir = (Thread_Control *) sched_info->thread;
}
