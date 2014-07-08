/**
 * @file
 *
 * @ingroup ScoreSchedulerPrioritySMP
 *
 * @brief Deterministic Priority SMP Scheduler API
 */

/*
 * Copyright (c) 2013-2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_SCHEDULERPRIORITYSMPIMPL_H
#define _RTEMS_SCORE_SCHEDULERPRIORITYSMPIMPL_H

#include <rtems/score/schedulerprioritysmp.h>
#include <rtems/score/schedulerpriorityimpl.h>
#include <rtems/score/schedulersimpleimpl.h>
#include <rtems/score/schedulersmpimpl.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @ingroup ScoreSchedulerPrioritySMP
 * @{
 */

static inline Scheduler_priority_SMP_Context *_Scheduler_priority_SMP_Get_self(
  Scheduler_Context *context
)
{
  return (Scheduler_priority_SMP_Context *) context;
}

static inline Scheduler_priority_SMP_Node *_Scheduler_priority_SMP_Thread_get_node(
  Thread_Control *thread
)
{
  return (Scheduler_priority_SMP_Node *) _Scheduler_Thread_get_node( thread );
}

static inline Scheduler_priority_SMP_Node *
_Scheduler_priority_SMP_Node_downcast( Scheduler_Node *node )
{
  return (Scheduler_priority_SMP_Node *) node;
}

static inline void _Scheduler_priority_SMP_Move_from_scheduled_to_ready(
  Scheduler_Context *context,
  Scheduler_Node    *scheduled_to_ready
)
{
  Scheduler_priority_SMP_Context *self =
    _Scheduler_priority_SMP_Get_self( context );
  Scheduler_priority_SMP_Node *node =
    _Scheduler_priority_SMP_Node_downcast( scheduled_to_ready );

  _Chain_Extract_unprotected( &node->Base.Base.Node );
  _Scheduler_priority_Ready_queue_enqueue_first(
    &node->Base.Base.Node,
    &node->Ready_queue,
    &self->Bit_map
  );
}

static inline void _Scheduler_priority_SMP_Move_from_ready_to_scheduled(
  Scheduler_Context *context,
  Scheduler_Node    *ready_to_scheduled
)
{
  Scheduler_priority_SMP_Context *self =
    _Scheduler_priority_SMP_Get_self( context );
  Scheduler_priority_SMP_Node *node =
    _Scheduler_priority_SMP_Node_downcast( ready_to_scheduled );

  _Scheduler_priority_Ready_queue_extract(
    &node->Base.Base.Node,
    &node->Ready_queue,
    &self->Bit_map
  );
  _Chain_Insert_ordered_unprotected(
    &self->Base.Scheduled,
    &node->Base.Base.Node,
    _Scheduler_SMP_Insert_priority_fifo_order
  );
}

static inline void _Scheduler_priority_SMP_Insert_ready_lifo(
  Scheduler_Context *context,
  Scheduler_Node    *thread
)
{
  Scheduler_priority_SMP_Context *self =
    _Scheduler_priority_SMP_Get_self( context );
  Scheduler_priority_SMP_Node *node =
    _Scheduler_priority_SMP_Node_downcast( thread );

  _Scheduler_priority_Ready_queue_enqueue(
    &node->Base.Base.Node,
    &node->Ready_queue,
    &self->Bit_map
  );
}

static inline void _Scheduler_priority_SMP_Insert_ready_fifo(
  Scheduler_Context *context,
  Scheduler_Node    *thread
)
{
  Scheduler_priority_SMP_Context *self =
    _Scheduler_priority_SMP_Get_self( context );
  Scheduler_priority_SMP_Node *node =
    _Scheduler_priority_SMP_Node_downcast( thread );

  _Scheduler_priority_Ready_queue_enqueue_first(
    &node->Base.Base.Node,
    &node->Ready_queue,
    &self->Bit_map
  );
}

static inline void _Scheduler_priority_SMP_Extract_from_ready(
  Scheduler_Context *context,
  Scheduler_Node    *thread
)
{
  Scheduler_priority_SMP_Context *self =
    _Scheduler_priority_SMP_Get_self( context );
  Scheduler_priority_SMP_Node *node =
    _Scheduler_priority_SMP_Node_downcast( thread );

  _Scheduler_priority_Ready_queue_extract(
    &node->Base.Base.Node,
    &node->Ready_queue,
    &self->Bit_map
  );
}

static inline void _Scheduler_priority_SMP_Do_update(
  Scheduler_Context *context,
  Scheduler_Node *node_to_update,
  Priority_Control new_priority
)
{
  Scheduler_priority_SMP_Context *self =
    _Scheduler_priority_SMP_Get_self( context );
  Scheduler_priority_SMP_Node *node =
    _Scheduler_priority_SMP_Node_downcast( node_to_update );

  _Scheduler_SMP_Node_update_priority( &node->Base, new_priority );
  _Scheduler_priority_Ready_queue_update(
    &node->Ready_queue,
    new_priority,
    &self->Bit_map,
    &self->Ready[ 0 ]
  );
}

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_SCHEDULERPRIORITYSMPIMPL_H */
