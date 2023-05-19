/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreSchedulerPrioritySMP
 *
 * @brief This source file contains the implementation of
 *   _Scheduler_priority_SMP_Add_processor(),
 *   _Scheduler_priority_SMP_Ask_for_help(), _Scheduler_priority_SMP_Block(),
 *   _Scheduler_priority_SMP_Initialize(),
 *   _Scheduler_priority_SMP_Node_initialize(),
 *   _Scheduler_priority_SMP_Reconsider_help_request(),
 *   _Scheduler_priority_SMP_Remove_processor(),
 *   _Scheduler_priority_SMP_Unblock(),
 *   _Scheduler_priority_SMP_Update_priority(),
 *   _Scheduler_priority_SMP_Withdraw_node(),
 *   _Scheduler_priority_SMP_Make_sticky(),
 *   _Scheduler_priority_SMP_Clean_sticky(), and
 *   _Scheduler_priority_SMP_Yield().
 */

/*
 * Copyright (C) 2013, 2014 embedded brains GmbH & Co. KG
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/schedulerprioritysmpimpl.h>

static Scheduler_priority_SMP_Context *
_Scheduler_priority_SMP_Get_context( const Scheduler_Control *scheduler )
{
  return (Scheduler_priority_SMP_Context *) _Scheduler_Get_context( scheduler );
}

void _Scheduler_priority_SMP_Initialize( const Scheduler_Control *scheduler )
{
  Scheduler_priority_SMP_Context *self =
    _Scheduler_priority_SMP_Get_context( scheduler );

  _Scheduler_SMP_Initialize( &self->Base );
  self->idle_ready_queue = &self->Ready[ scheduler->maximum_priority ];
  _Priority_bit_map_Initialize( &self->Bit_map );
  _Scheduler_priority_Ready_queue_initialize(
    &self->Ready[ 0 ],
    scheduler->maximum_priority
  );
}

void _Scheduler_priority_SMP_Node_initialize(
  const Scheduler_Control *scheduler,
  Scheduler_Node          *node,
  Thread_Control          *the_thread,
  Priority_Control         priority
)
{
  Scheduler_Context              *context;
  Scheduler_priority_SMP_Context *self;
  Scheduler_priority_SMP_Node    *the_node;

  the_node = _Scheduler_priority_SMP_Node_downcast( node );
  _Scheduler_SMP_Node_initialize(
    scheduler,
    &the_node->Base,
    the_thread,
    priority
  );

  context = _Scheduler_Get_context( scheduler );
  self = _Scheduler_priority_SMP_Get_self( context );
  _Scheduler_priority_Ready_queue_update(
    &the_node->Ready_queue,
    SCHEDULER_PRIORITY_UNMAP( priority ),
    &self->Bit_map,
    &self->Ready[ 0 ]
  );
}

static Scheduler_Node *_Scheduler_priority_SMP_Get_highest_ready(
  Scheduler_Context *context,
  Scheduler_Node    *node
)
{
  Scheduler_priority_SMP_Context *self =
    _Scheduler_priority_SMP_Get_self( context );

  (void) node;

  return (Scheduler_Node *) _Scheduler_priority_Ready_queue_first(
    &self->Bit_map,
    &self->Ready[ 0 ]
  );
}

void _Scheduler_priority_SMP_Block(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Scheduler_Node          *node
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  _Scheduler_SMP_Block(
    context,
    thread,
    node,
    _Scheduler_SMP_Extract_from_scheduled,
    _Scheduler_priority_SMP_Extract_from_ready,
    _Scheduler_priority_SMP_Get_highest_ready,
    _Scheduler_priority_SMP_Move_from_ready_to_scheduled,
    _Scheduler_SMP_Allocate_processor_lazy,
    _Scheduler_priority_SMP_Get_idle
  );
}

static bool _Scheduler_priority_SMP_Enqueue(
  Scheduler_Context *context,
  Scheduler_Node    *node,
  Priority_Control   insert_priority
)
{
  return _Scheduler_SMP_Enqueue(
    context,
    node,
    insert_priority,
    _Scheduler_SMP_Priority_less_equal,
    _Scheduler_priority_SMP_Insert_ready,
    _Scheduler_SMP_Insert_scheduled,
    _Scheduler_priority_SMP_Move_from_scheduled_to_ready,
    _Scheduler_priority_SMP_Move_from_ready_to_scheduled,
    _Scheduler_SMP_Get_lowest_scheduled,
    _Scheduler_SMP_Allocate_processor_lazy,
    _Scheduler_priority_SMP_Get_idle,
    _Scheduler_priority_SMP_Release_idle
  );
}

static void _Scheduler_priority_SMP_Enqueue_scheduled(
  Scheduler_Context *context,
  Scheduler_Node    *node,
  Priority_Control   insert_priority
)
{
  _Scheduler_SMP_Enqueue_scheduled(
    context,
    node,
    insert_priority,
    _Scheduler_SMP_Priority_less_equal,
    _Scheduler_priority_SMP_Extract_from_ready,
    _Scheduler_priority_SMP_Get_highest_ready,
    _Scheduler_priority_SMP_Insert_ready,
    _Scheduler_SMP_Insert_scheduled,
    _Scheduler_priority_SMP_Move_from_ready_to_scheduled,
    _Scheduler_SMP_Allocate_processor_lazy,
    _Scheduler_priority_SMP_Get_idle,
    _Scheduler_priority_SMP_Release_idle
  );
}

void _Scheduler_priority_SMP_Unblock(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Scheduler_Node          *node
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  _Scheduler_SMP_Unblock(
    context,
    thread,
    node,
    _Scheduler_priority_SMP_Do_update,
    _Scheduler_priority_SMP_Enqueue,
    _Scheduler_priority_SMP_Release_idle
  );
}

static bool _Scheduler_priority_SMP_Do_ask_for_help(
  Scheduler_Context *context,
  Thread_Control    *the_thread,
  Scheduler_Node    *node
)
{
  return _Scheduler_SMP_Ask_for_help(
    context,
    the_thread,
    node,
    _Scheduler_SMP_Priority_less_equal,
    _Scheduler_priority_SMP_Insert_ready,
    _Scheduler_SMP_Insert_scheduled,
    _Scheduler_priority_SMP_Move_from_scheduled_to_ready,
    _Scheduler_SMP_Get_lowest_scheduled,
    _Scheduler_SMP_Allocate_processor_lazy,
    _Scheduler_priority_SMP_Release_idle
  );
}

void _Scheduler_priority_SMP_Update_priority(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Scheduler_Node          *node
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  _Scheduler_SMP_Update_priority(
    context,
    thread,
    node,
    _Scheduler_SMP_Extract_from_scheduled,
    _Scheduler_priority_SMP_Extract_from_ready,
    _Scheduler_priority_SMP_Do_update,
    _Scheduler_priority_SMP_Enqueue,
    _Scheduler_priority_SMP_Enqueue_scheduled,
    _Scheduler_priority_SMP_Do_ask_for_help
  );
}

bool _Scheduler_priority_SMP_Ask_for_help(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  return _Scheduler_priority_SMP_Do_ask_for_help( context, the_thread, node );
}

void _Scheduler_priority_SMP_Reconsider_help_request(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  _Scheduler_SMP_Reconsider_help_request(
    context,
    the_thread,
    node,
    _Scheduler_priority_SMP_Extract_from_ready
  );
}

void _Scheduler_priority_SMP_Withdraw_node(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node,
  Thread_Scheduler_state   next_state
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  _Scheduler_SMP_Withdraw_node(
    context,
    the_thread,
    node,
    next_state,
    _Scheduler_SMP_Extract_from_scheduled,
    _Scheduler_priority_SMP_Extract_from_ready,
    _Scheduler_priority_SMP_Get_highest_ready,
    _Scheduler_priority_SMP_Move_from_ready_to_scheduled,
    _Scheduler_SMP_Allocate_processor_lazy,
    _Scheduler_priority_SMP_Get_idle
  );
}

void _Scheduler_priority_SMP_Make_sticky(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
)
{
  _Scheduler_SMP_Make_sticky(
    scheduler,
    the_thread,
    node,
    _Scheduler_priority_SMP_Do_update,
    _Scheduler_priority_SMP_Enqueue
  );
}

void _Scheduler_priority_SMP_Clean_sticky(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
)
{
  _Scheduler_SMP_Clean_sticky(
    scheduler,
    the_thread,
    node,
    _Scheduler_SMP_Extract_from_scheduled,
    _Scheduler_priority_SMP_Extract_from_ready,
    _Scheduler_priority_SMP_Get_highest_ready,
    _Scheduler_priority_SMP_Move_from_ready_to_scheduled,
    _Scheduler_SMP_Allocate_processor_lazy,
    _Scheduler_priority_SMP_Get_idle,
    _Scheduler_priority_SMP_Release_idle
  );
}

void _Scheduler_priority_SMP_Add_processor(
  const Scheduler_Control *scheduler,
  Thread_Control          *idle
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  _Scheduler_SMP_Add_processor(
    context,
    idle,
    _Scheduler_priority_SMP_Has_ready,
    _Scheduler_priority_SMP_Enqueue_scheduled,
    _Scheduler_SMP_Do_nothing_register_idle
  );
}

Thread_Control *_Scheduler_priority_SMP_Remove_processor(
  const Scheduler_Control *scheduler,
  Per_CPU_Control         *cpu
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  return _Scheduler_SMP_Remove_processor(
    context,
    cpu,
    _Scheduler_SMP_Extract_from_scheduled,
    _Scheduler_priority_SMP_Extract_from_ready,
    _Scheduler_priority_SMP_Enqueue,
    _Scheduler_priority_SMP_Get_idle,
    _Scheduler_priority_SMP_Release_idle
  );
}

void _Scheduler_priority_SMP_Yield(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Scheduler_Node          *node
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  _Scheduler_SMP_Yield(
    context,
    thread,
    node,
    _Scheduler_SMP_Extract_from_scheduled,
    _Scheduler_priority_SMP_Extract_from_ready,
    _Scheduler_priority_SMP_Enqueue,
    _Scheduler_priority_SMP_Enqueue_scheduled
  );
}
