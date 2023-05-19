/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreSchedulerPrioritySMP
 *
 * @brief This header file provides interfaces of the
 *   @ref RTEMSScoreSchedulerPrioritySMP which are only used by the
 *   implementation.
 */

/*
 * Copyright (C) 2013, 2017 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
 * @ingroup RTEMSScoreSchedulerPrioritySMP
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
  return (Scheduler_priority_SMP_Node *) _Thread_Scheduler_get_home_node( thread );
}

static inline Scheduler_priority_SMP_Node *
_Scheduler_priority_SMP_Node_downcast( Scheduler_Node *node )
{
  return (Scheduler_priority_SMP_Node *) node;
}

static inline bool _Scheduler_priority_SMP_Has_ready( Scheduler_Context *context )
{
  Scheduler_priority_SMP_Context *self =
    _Scheduler_priority_SMP_Get_self( context );

  return !_Priority_bit_map_Is_empty( &self->Bit_map );
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

  _Chain_Extract_unprotected( &node->Base.Base.Node.Chain );
  _Scheduler_priority_Ready_queue_enqueue_first(
    &node->Base.Base.Node.Chain,
    &node->Ready_queue,
    &self->Bit_map
  );
}

static inline void _Scheduler_priority_SMP_Move_from_ready_to_scheduled(
  Scheduler_Context *context,
  Scheduler_Node    *ready_to_scheduled
)
{
  Scheduler_priority_SMP_Context *self;
  Scheduler_priority_SMP_Node    *node;
  Priority_Control                insert_priority;

  self = _Scheduler_priority_SMP_Get_self( context );
  node = _Scheduler_priority_SMP_Node_downcast( ready_to_scheduled );

  _Scheduler_priority_Ready_queue_extract(
    &node->Base.Base.Node.Chain,
    &node->Ready_queue,
    &self->Bit_map
  );
  insert_priority = _Scheduler_SMP_Node_priority( &node->Base.Base );
  insert_priority = SCHEDULER_PRIORITY_APPEND( insert_priority );
  _Chain_Insert_ordered_unprotected(
    &self->Base.Scheduled,
    &node->Base.Base.Node.Chain,
    &insert_priority,
    _Scheduler_SMP_Priority_less_equal
  );
}

static inline void _Scheduler_priority_SMP_Insert_ready(
  Scheduler_Context *context,
  Scheduler_Node    *node_base,
  Priority_Control   insert_priority
)
{
  Scheduler_priority_SMP_Context *self;
  Scheduler_priority_SMP_Node    *node;

  self = _Scheduler_priority_SMP_Get_self( context );
  node = _Scheduler_priority_SMP_Node_downcast( node_base );

  if ( SCHEDULER_PRIORITY_IS_APPEND( insert_priority ) ) {
    _Scheduler_priority_Ready_queue_enqueue(
      &node->Base.Base.Node.Chain,
      &node->Ready_queue,
      &self->Bit_map
    );
  } else {
    _Scheduler_priority_Ready_queue_enqueue_first(
      &node->Base.Base.Node.Chain,
      &node->Ready_queue,
      &self->Bit_map
    );
  }
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
    &node->Base.Base.Node.Chain,
    &node->Ready_queue,
    &self->Bit_map
  );
}

static inline Scheduler_Node *_Scheduler_priority_SMP_Get_idle( void *arg )
{
  Scheduler_priority_SMP_Context *self;
  Scheduler_priority_SMP_Node    *lowest_ready;

  self = _Scheduler_priority_SMP_Get_self( arg );
  lowest_ready = (Scheduler_priority_SMP_Node *)
    _Chain_Last( self->idle_ready_queue );
  _Scheduler_priority_Ready_queue_extract(
    &lowest_ready->Base.Base.Node.Chain,
    &lowest_ready->Ready_queue,
    &self->Bit_map
  );

  return &lowest_ready->Base.Base;
}

static inline void _Scheduler_priority_SMP_Release_idle(
  Scheduler_Node *node_base,
  void           *arg
)
{
  Scheduler_priority_SMP_Context *self;
  Scheduler_priority_SMP_Node    *node;

  self = _Scheduler_priority_SMP_Get_self( arg );
  node = _Scheduler_priority_SMP_Node_downcast( node_base );

  _Scheduler_priority_Ready_queue_enqueue(
    &node->Base.Base.Node.Chain,
    &node->Ready_queue,
    &self->Bit_map
  );
}

static inline void _Scheduler_priority_SMP_Do_update(
  Scheduler_Context *context,
  Scheduler_Node    *node_to_update,
  Priority_Control   new_priority
)
{
  Scheduler_priority_SMP_Context *self;
  Scheduler_priority_SMP_Node    *node;

  self = _Scheduler_priority_SMP_Get_self( context );
  node = _Scheduler_priority_SMP_Node_downcast( node_to_update );

  _Scheduler_SMP_Node_update_priority( &node->Base, new_priority );
  _Scheduler_priority_Ready_queue_update(
    &node->Ready_queue,
    SCHEDULER_PRIORITY_UNMAP( new_priority ),
    &self->Bit_map,
    &self->Ready[ 0 ]
  );
}

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_SCHEDULERPRIORITYSMPIMPL_H */
