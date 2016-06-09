/**
 * @file
 *
 * @brief Scheduler EDF Initialize and Support
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

int _Scheduler_EDF_Priority_compare (
  Priority_Control p1,
  Priority_Control p2
)
{
  Watchdog_Interval time = _Watchdog_Ticks_since_boot;

  /*
   * Reorder priorities to separate deadline driven and background tasks.
   *
   * The background tasks have p1 or p2 > SCHEDULER_EDF_PRIO_MSB.
   * The deadline driven tasks need to have subtracted current time in order
   * to see which deadline is closer wrt. current time.
   */
  if (!(p1 & SCHEDULER_EDF_PRIO_MSB))
    p1 = (p1 - time) & ~SCHEDULER_EDF_PRIO_MSB;
  if (!(p2 & SCHEDULER_EDF_PRIO_MSB))
    p2 = (p2 - time) & ~SCHEDULER_EDF_PRIO_MSB;

  return ((p1<p2) - (p1>p2));
}

RBTree_Compare_result _Scheduler_EDF_Compare(
  const RBTree_Node* n1,
  const RBTree_Node* n2
)
{
  Scheduler_EDF_Node *edf1 =
    RTEMS_CONTAINER_OF( n1, Scheduler_EDF_Node, Node );
  Scheduler_EDF_Node *edf2 =
    RTEMS_CONTAINER_OF( n2, Scheduler_EDF_Node, Node );
  Priority_Control value1 = edf1->thread->current_priority;
  Priority_Control value2 = edf2->thread->current_priority;

  /*
   * This function compares only numbers for the red-black tree,
   * but priorities have an opposite sense.
   */
  return (-1)*_Scheduler_EDF_Priority_compare(value1, value2);
}

void _Scheduler_EDF_Initialize( const Scheduler_Control *scheduler )
{
  Scheduler_EDF_Context *context =
    _Scheduler_EDF_Get_context( scheduler );

  _RBTree_Initialize_empty( &context->Ready );
}
