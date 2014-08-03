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
