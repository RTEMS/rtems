/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreSchedulerPriority
 *
 * @brief This source file contains the implementation of
 *   _Scheduler_priority_Update_priority().
 */

/*
 *  COPYRIGHT (c) 2011.
 *  On-Line Applications Research Corporation (OAR).
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

#include <rtems/score/schedulerpriorityimpl.h>

void _Scheduler_priority_Update_priority(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
)
{
  Scheduler_priority_Context *context;
  Scheduler_priority_Node    *the_node;
  unsigned int                new_priority;
  unsigned int                unmapped_priority;

  if ( !_Thread_Is_ready( the_thread ) ) {
    /* Nothing to do */
    return;
  }

  the_node = _Scheduler_priority_Node_downcast( node );
  new_priority = (unsigned int) _Scheduler_Node_get_priority( &the_node->Base );
  unmapped_priority = SCHEDULER_PRIORITY_UNMAP( new_priority );

  if ( unmapped_priority == the_node->Ready_queue.current_priority ) {
    /* Nothing to do */
    return;
  }

  context = _Scheduler_priority_Get_context( scheduler );

  _Scheduler_priority_Ready_queue_extract(
    &the_thread->Object.Node,
    &the_node->Ready_queue,
    &context->Bit_map
  );

  _Scheduler_priority_Ready_queue_update(
    &the_node->Ready_queue,
    unmapped_priority,
    &context->Bit_map,
    &context->Ready[ 0 ]
  );

  if ( SCHEDULER_PRIORITY_IS_APPEND( new_priority ) ) {
    _Scheduler_priority_Ready_queue_enqueue(
      &the_thread->Object.Node,
      &the_node->Ready_queue,
      &context->Bit_map
    );
  } else {
    _Scheduler_priority_Ready_queue_enqueue_first(
      &the_thread->Object.Node,
      &the_node->Ready_queue,
      &context->Bit_map
    );
  }

  _Scheduler_uniprocessor_Schedule(
    scheduler,
    _Scheduler_priority_Get_highest_ready
  );
}
