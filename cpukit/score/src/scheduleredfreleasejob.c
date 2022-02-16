/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreSchedulerEDF
 *
 * @brief This source file contains the implementation of
 *   _Scheduler_EDF_Map_priority(), _Scheduler_EDF_Unmap_priority(),
 *   _Scheduler_EDF_Release_job(), and _Scheduler_EDF_Cancel_job().
 */

/*
 *  Copyright (C) 2011 Petr Benes.
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
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

#include <rtems/score/scheduleredfimpl.h>

Priority_Control _Scheduler_EDF_Map_priority(
  const Scheduler_Control *scheduler,
  Priority_Control         priority
)
{
  return SCHEDULER_EDF_PRIO_MSB | SCHEDULER_PRIORITY_MAP( priority );
}

Priority_Control _Scheduler_EDF_Unmap_priority(
  const Scheduler_Control *scheduler,
  Priority_Control         priority
)
{
  return SCHEDULER_PRIORITY_UNMAP( priority & ~SCHEDULER_EDF_PRIO_MSB );
}

void _Scheduler_EDF_Release_job(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Priority_Node           *priority_node,
  uint64_t                 deadline,
  Thread_queue_Context    *queue_context
)
{
  (void) scheduler;

  _Thread_Wait_acquire_critical( the_thread, queue_context );

  /*
   * There is no integer overflow problem here due to the
   * SCHEDULER_PRIORITY_MAP().  The deadline is in clock ticks.  With the
   * minimum clock tick interval of 1us, the uptime is limited to about 146235
   * years.
   */
  _Priority_Node_set_priority(
    priority_node,
    SCHEDULER_PRIORITY_MAP( deadline )
  );

  if ( _Priority_Node_is_active( priority_node ) ) {
    _Thread_Priority_changed(
      the_thread,
      priority_node,
      PRIORITY_GROUP_LAST,
      queue_context
    );
  } else {
    _Thread_Priority_add( the_thread, priority_node, queue_context );
  }

  _Thread_Wait_release_critical( the_thread, queue_context );
}

void _Scheduler_EDF_Cancel_job(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Priority_Node           *priority_node,
  Thread_queue_Context    *queue_context
)
{
  (void) scheduler;

  _Thread_Wait_acquire_critical( the_thread, queue_context );

  if ( _Priority_Node_is_active( priority_node ) ) {
    _Thread_Priority_remove( the_thread, priority_node, queue_context );
    _Priority_Node_set_inactive( priority_node );
  }

  _Thread_Wait_release_critical( the_thread, queue_context );
}
