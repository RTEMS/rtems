/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreThread
 *
 * @brief This source file contains the implementation of
 *   _Thread_Do_unpin().
 */

/*
 * Copyright (c) 2018 embedded brains GmbH & Co. KG
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

#include <rtems/score/schedulerimpl.h>

void _Thread_Do_unpin( Thread_Control *executing, Per_CPU_Control *cpu_self )
{
  ISR_lock_Context         state_lock_context;
  ISR_lock_Context         scheduler_lock_context;
  Scheduler_Node          *pinned_node;
  const Scheduler_Control *pinned_scheduler;
  Scheduler_Node          *home_node;
  const Scheduler_Control *home_scheduler;
  const Scheduler_Control *scheduler;

  _Thread_State_acquire( executing, &state_lock_context );

  executing->Scheduler.pin_level = 0;

  pinned_node = SCHEDULER_NODE_OF_THREAD_SCHEDULER_NODE(
    _Chain_First( &executing->Scheduler.Scheduler_nodes )
  );
  pinned_scheduler = _Scheduler_Node_get_scheduler( pinned_node );
  home_node = _Thread_Scheduler_get_home_node( executing );
  home_scheduler = _Thread_Scheduler_get_home( executing );
  scheduler = pinned_scheduler;

  executing->Scheduler.pinned_scheduler = NULL;

  _Scheduler_Acquire_critical( scheduler, &scheduler_lock_context );

  if ( _Thread_Is_ready( executing ) ) {
    ( *scheduler->Operations.block )( scheduler, executing, pinned_node );
  }

  ( *scheduler->Operations.unpin )(
    scheduler,
    executing,
    pinned_node,
    cpu_self
  );

  if ( home_node != pinned_node ) {
    _Scheduler_Release_critical( scheduler, &scheduler_lock_context );

    _Chain_Extract_unprotected( &home_node->Thread.Scheduler_node.Chain );
    _Chain_Prepend_unprotected(
      &executing->Scheduler.Scheduler_nodes,
      &home_node->Thread.Scheduler_node.Chain
    );
    scheduler = home_scheduler;

    _Scheduler_Acquire_critical( scheduler, &scheduler_lock_context );
  }

  if ( _Thread_Is_ready( executing ) ) {
    ( *scheduler->Operations.unblock )( scheduler, executing, home_node );
  }

  _Scheduler_Release_critical( scheduler, &scheduler_lock_context );

  _Thread_State_release( executing, &state_lock_context );
}
