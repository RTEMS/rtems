/*
 * Copyright (c) 2018 embedded brains GmbH
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
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
