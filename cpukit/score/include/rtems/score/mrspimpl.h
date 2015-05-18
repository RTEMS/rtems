/*
 * Copyright (c) 2014-2015 embedded brains GmbH.  All rights reserved.
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

#ifndef _RTEMS_SCORE_MRSPIMPL_H
#define _RTEMS_SCORE_MRSPIMPL_H

#include <rtems/score/mrsp.h>

#if defined(RTEMS_SMP)

#include <rtems/score/assert.h>
#include <rtems/score/chainimpl.h>
#include <rtems/score/resourceimpl.h>
#include <rtems/score/schedulerimpl.h>
#include <rtems/score/watchdogimpl.h>
#include <rtems/score/wkspace.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @addtogroup ScoreMRSP
 *
 * @{
 */

/*
 * FIXME: Operations with the resource dependency tree are protected by the
 * global scheduler lock.  Since the scheduler lock should be scheduler
 * instance specific in the future this will only work temporarily.  A more
 * sophisticated locking strategy is necessary.
 */

RTEMS_INLINE_ROUTINE void _MRSP_Giant_acquire( ISR_lock_Context *lock_context )
{
  _ISR_lock_Acquire( &_Scheduler_Lock, lock_context );
}

RTEMS_INLINE_ROUTINE void _MRSP_Giant_release( ISR_lock_Context *lock_context )
{
  _ISR_lock_Release( &_Scheduler_Lock, lock_context );
}

RTEMS_INLINE_ROUTINE bool _MRSP_Restore_priority_filter(
  Thread_Control   *thread,
  Priority_Control *new_priority,
  void             *arg
)
{
  *new_priority = _Thread_Priority_highest(
    thread->real_priority,
    *new_priority
  );

  return *new_priority != thread->current_priority;
}

RTEMS_INLINE_ROUTINE void _MRSP_Restore_priority(
  Thread_Control   *thread,
  Priority_Control  initial_priority
)
{
  /*
   * The Thread_Control::resource_count is used by the normal priority ceiling
   * or priority inheritance semaphores.
   */
  if ( thread->resource_count == 0 ) {
    _Thread_Change_priority(
      thread,
      initial_priority,
      NULL,
      _MRSP_Restore_priority_filter,
      true
    );
  }
}

RTEMS_INLINE_ROUTINE void _MRSP_Claim_ownership(
  MRSP_Control     *mrsp,
  Thread_Control   *new_owner,
  Priority_Control  initial_priority,
  Priority_Control  ceiling_priority,
  ISR_lock_Context *lock_context
)
{
  Per_CPU_Control *cpu_self;

  _Resource_Node_add_resource( &new_owner->Resource_node, &mrsp->Resource );
  _Resource_Set_owner( &mrsp->Resource, &new_owner->Resource_node );
  mrsp->initial_priority_of_owner = initial_priority;
  _Scheduler_Thread_change_help_state( new_owner, SCHEDULER_HELP_ACTIVE_OWNER );

  cpu_self = _Thread_Dispatch_disable_critical( lock_context );
  _ISR_lock_Release_and_ISR_enable( &mrsp->Lock, lock_context );

  _Thread_Raise_priority( new_owner, ceiling_priority );

  _Thread_Dispatch_enable( cpu_self );
}

RTEMS_INLINE_ROUTINE MRSP_Status _MRSP_Initialize(
  MRSP_Control     *mrsp,
  Priority_Control  ceiling_priority,
  Thread_Control   *executing,
  bool              initially_locked
)
{
  uint32_t scheduler_count = _Scheduler_Count;
  uint32_t i;

  if ( initially_locked ) {
    return MRSP_INVALID_NUMBER;
  }

  mrsp->ceiling_priorities = _Workspace_Allocate(
    sizeof( *mrsp->ceiling_priorities ) * scheduler_count
  );
  if ( mrsp->ceiling_priorities == NULL ) {
    return MRSP_NO_MEMORY;
  }

  for ( i = 0 ; i < scheduler_count ; ++i ) {
    mrsp->ceiling_priorities[ i ] = ceiling_priority;
  }

  _Resource_Initialize( &mrsp->Resource );
  _Chain_Initialize_empty( &mrsp->Rivals );
  _ISR_lock_Initialize( &mrsp->Lock, "MrsP" );

  return MRSP_SUCCESSFUL;
}

RTEMS_INLINE_ROUTINE Priority_Control _MRSP_Get_ceiling_priority(
  MRSP_Control *mrsp,
  uint32_t      scheduler_index
)
{
  return mrsp->ceiling_priorities[ scheduler_index ];
}

RTEMS_INLINE_ROUTINE void _MRSP_Set_ceiling_priority(
  MRSP_Control      *mrsp,
  uint32_t           scheduler_index,
  Priority_Control   ceiling_priority
)
{
  mrsp->ceiling_priorities[ scheduler_index ] = ceiling_priority;
}

RTEMS_INLINE_ROUTINE void _MRSP_Timeout(
  Objects_Id  id,
  void       *arg
)
{
  MRSP_Rival *rival = arg;
  MRSP_Control *mrsp = rival->resource;
  Thread_Control *thread = rival->thread;
  ISR_lock_Context lock_context;

  (void) id;

  _ISR_lock_ISR_disable_and_acquire( &mrsp->Lock, &lock_context );

  if ( rival->status == MRSP_WAIT_FOR_OWNERSHIP ) {
    ISR_lock_Context giant_lock_context;

    _MRSP_Giant_acquire( &giant_lock_context );

    _Chain_Extract_unprotected( &rival->Node );
    _Resource_Node_extract( &thread->Resource_node );
    _Resource_Node_set_dependency( &thread->Resource_node, NULL );
    _Scheduler_Thread_change_help_state( thread, rival->initial_help_state );
    _Scheduler_Thread_change_resource_root( thread, thread );

    _MRSP_Giant_release( &giant_lock_context );

    rival->status = MRSP_TIMEOUT;

    _ISR_lock_Release_and_ISR_enable( &mrsp->Lock, &lock_context );
  } else {
    _ISR_lock_Release_and_ISR_enable( &mrsp->Lock, &lock_context );
  }
}

RTEMS_INLINE_ROUTINE MRSP_Status _MRSP_Wait_for_ownership(
  MRSP_Control      *mrsp,
  Resource_Node     *owner,
  Thread_Control    *executing,
  Priority_Control   initial_priority,
  Priority_Control   ceiling_priority,
  Watchdog_Interval  timeout,
  ISR_lock_Context  *lock_context
)
{
  MRSP_Status status;
  MRSP_Rival rival;
  bool initial_life_protection;
  Per_CPU_Control *cpu_self;
  ISR_lock_Context giant_lock_context;

  rival.thread = executing;
  rival.resource = mrsp;
  rival.initial_priority = initial_priority;

  _MRSP_Giant_acquire( &giant_lock_context );

  rival.initial_help_state =
    _Scheduler_Thread_change_help_state( executing, SCHEDULER_HELP_ACTIVE_RIVAL );
  rival.status = MRSP_WAIT_FOR_OWNERSHIP;

  _Chain_Append_unprotected( &mrsp->Rivals, &rival.Node );
  _Resource_Add_rival( &mrsp->Resource, &executing->Resource_node );
  _Resource_Node_set_dependency( &executing->Resource_node, &mrsp->Resource );
  _Scheduler_Thread_change_resource_root(
    executing,
    THREAD_RESOURCE_NODE_TO_THREAD( _Resource_Node_get_root( owner ) )
  );

  _MRSP_Giant_release( &giant_lock_context );

  cpu_self = _Thread_Dispatch_disable_critical( lock_context );
  _ISR_lock_Release_and_ISR_enable( &mrsp->Lock, lock_context );

  _Thread_Raise_priority( executing, ceiling_priority );

  if ( timeout > 0 ) {
    _Watchdog_Initialize(
      &executing->Timer,
      _MRSP_Timeout,
      0,
      &rival
    );
    _Watchdog_Insert_ticks( &executing->Timer, timeout );
  }

  initial_life_protection = _Thread_Set_life_protection( true );
  _Thread_Dispatch_enable( cpu_self );

  _Assert( _Debug_Is_thread_dispatching_allowed() );

  /* Wait for state change */
  do {
    status = rival.status;
  } while ( status == MRSP_WAIT_FOR_OWNERSHIP );

  _Thread_Set_life_protection( initial_life_protection );

  if ( timeout > 0 ) {
    _Watchdog_Remove_ticks( &executing->Timer );

    if ( status == MRSP_TIMEOUT ) {
      _MRSP_Restore_priority( executing, initial_priority );
    }
  }

  return status;
}

RTEMS_INLINE_ROUTINE MRSP_Status _MRSP_Obtain(
  MRSP_Control      *mrsp,
  Thread_Control    *executing,
  bool               wait,
  Watchdog_Interval  timeout,
  ISR_lock_Context  *lock_context
)
{
  MRSP_Status status;
  const Scheduler_Control *scheduler = _Scheduler_Get_own( executing );
  uint32_t scheduler_index = _Scheduler_Get_index( scheduler );
  Priority_Control initial_priority = executing->current_priority;
  Priority_Control ceiling_priority =
    _MRSP_Get_ceiling_priority( mrsp, scheduler_index );
  bool priority_ok = !_Thread_Priority_less_than(
    ceiling_priority,
    initial_priority
  );
  Resource_Node *owner;

  if ( !priority_ok) {
    _ISR_lock_ISR_enable( lock_context );
    return MRSP_INVALID_PRIORITY;
  }

  _ISR_lock_Acquire( &mrsp->Lock, lock_context );
  owner = _Resource_Get_owner( &mrsp->Resource );
  if ( owner == NULL ) {
    _MRSP_Claim_ownership(
      mrsp,
      executing,
      initial_priority,
      ceiling_priority,
      lock_context
    );
    status = MRSP_SUCCESSFUL;
  } else if (
    wait
      && _Resource_Node_get_root( owner ) != &executing->Resource_node
  ) {
    status = _MRSP_Wait_for_ownership(
      mrsp,
      owner,
      executing,
      initial_priority,
      ceiling_priority,
      timeout,
      lock_context
    );
  } else {
    _ISR_lock_Release_and_ISR_enable( &mrsp->Lock, lock_context );
    /* Not available, nested access or deadlock */
    status = MRSP_UNSATISFIED;
  }

  return status;
}

RTEMS_INLINE_ROUTINE MRSP_Status _MRSP_Release(
  MRSP_Control     *mrsp,
  Thread_Control   *executing,
  ISR_lock_Context *lock_context
)
{
  Priority_Control initial_priority;
  Per_CPU_Control *cpu_self;
  ISR_lock_Context giant_lock_context;

  if ( _Resource_Get_owner( &mrsp->Resource ) != &executing->Resource_node ) {
    _ISR_lock_ISR_enable( lock_context );
    return MRSP_NOT_OWNER_OF_RESOURCE;
  }

  if (
    !_Resource_Is_most_recently_obtained(
      &mrsp->Resource,
      &executing->Resource_node
    )
  ) {
    _ISR_lock_ISR_enable( lock_context );
    return MRSP_INCORRECT_STATE;
  }

  initial_priority = mrsp->initial_priority_of_owner;

  _ISR_lock_Acquire( &mrsp->Lock, lock_context );

  _MRSP_Giant_acquire( &giant_lock_context );

  _Resource_Extract( &mrsp->Resource );

  if ( _Chain_Is_empty( &mrsp->Rivals ) ) {
    _Resource_Set_owner( &mrsp->Resource, NULL );
  } else {
    MRSP_Rival *rival = (MRSP_Rival *)
      _Chain_Get_first_unprotected( &mrsp->Rivals );
    Thread_Control *new_owner;

    /*
     * This must be inside the critical section since the status prevents a
     * potential double extraction in _MRSP_Timeout().
     */
    rival->status = MRSP_SUCCESSFUL;

    new_owner = rival->thread;
    mrsp->initial_priority_of_owner = rival->initial_priority;
    _Resource_Node_extract( &new_owner->Resource_node );
    _Resource_Node_set_dependency( &new_owner->Resource_node, NULL );
    _Resource_Node_add_resource( &new_owner->Resource_node, &mrsp->Resource );
    _Resource_Set_owner( &mrsp->Resource, &new_owner->Resource_node );
    _Scheduler_Thread_change_help_state( new_owner, SCHEDULER_HELP_ACTIVE_OWNER );
    _Scheduler_Thread_change_resource_root( new_owner, new_owner );
  }

  if ( !_Resource_Node_owns_resources( &executing->Resource_node ) ) {
    _Scheduler_Thread_change_help_state( executing, SCHEDULER_HELP_YOURSELF );
  }

  _MRSP_Giant_release( &giant_lock_context );

  cpu_self = _Thread_Dispatch_disable_critical( lock_context );
  _ISR_lock_Release_and_ISR_enable( &mrsp->Lock, lock_context );

  _MRSP_Restore_priority( executing, initial_priority );

  _Thread_Dispatch_enable( cpu_self );

  return MRSP_SUCCESSFUL;
}

RTEMS_INLINE_ROUTINE MRSP_Status _MRSP_Destroy( MRSP_Control *mrsp )
{
  if ( _Resource_Get_owner( &mrsp->Resource ) != NULL ) {
    return MRSP_RESOUCE_IN_USE;
  }

  _ISR_lock_Destroy( &mrsp->Lock );
  _Workspace_Free( mrsp->ceiling_priorities );

  return MRSP_SUCCESSFUL;
}

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RTEMS_SMP */

#endif /* _RTEMS_SCORE_MRSPIMPL_H */
