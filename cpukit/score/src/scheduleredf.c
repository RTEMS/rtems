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

#include <rtems/system.h>
#include <rtems/config.h>
#include <rtems/score/scheduler.h>
#include <rtems/score/scheduleredf.h>

static int _Scheduler_EDF_RBTree_compare_function
(
  const RBTree_Node* n1,
  const RBTree_Node* n2
)
{
  Priority_Control value1 = _RBTree_Container_of
    (n1,Scheduler_EDF_Per_thread,Node)->thread->current_priority;
  Priority_Control value2 = _RBTree_Container_of
    (n2,Scheduler_EDF_Per_thread,Node)->thread->current_priority;

  /*
   * This function compares only numbers for the red-black tree,
   * but priorities have an opposite sense.
   */
  return (-1)*_Scheduler_Priority_compare(value1, value2);
}

void _Scheduler_EDF_Initialize(void)
{
  _RBTree_Initialize_empty(
      &_Scheduler_EDF_Ready_queue,
      &_Scheduler_EDF_RBTree_compare_function,
      0
  );
}

/* Instantiate any global variables needed by the EDF scheduler */
RBTree_Control _Scheduler_EDF_Ready_queue;
