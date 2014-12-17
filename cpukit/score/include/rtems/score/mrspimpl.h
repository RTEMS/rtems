/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
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

RTEMS_INLINE_ROUTINE void _MRSP_Elevate_priority(
  MRSP_Control     *mrsp,
  Thread_Control   *new_owner,
  Priority_Control  ceiling_priority
)
{
  _Thread_Change_priority( new_owner, ceiling_priority, false );
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
    Priority_Control new_priority = _Scheduler_Highest_priority_of_two(
      _Scheduler_Get( thread ),
      initial_priority,
      thread->real_priority
    );

    _Thread_Change_priority( thread, new_priority, true );
  }
}

RTEMS_INLINE_ROUTINE void _MRSP_Claim_ownership(
  MRSP_Control     *mrsp,
  Thread_Control   *new_owner,
  Priority_Control  initial_priority,
  Priority_Control  ceiling_priority
)
{
  _Resource_Node_add_resource( &new_owner->Resource_node, &mrsp->Resource );
  _Resource_Set_owner( &mrsp->Resource, &new_owner->Resource_node );
  mrsp->initial_priority_of_owner = initial_priority;
  _MRSP_Elevate_priority( mrsp, new_owner, ceiling_priority );
  _Scheduler_Thread_change_help_state( new_owner, SCHEDULER_HELP_ACTIVE_OWNER );
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
  Thread_Control *thread = rival->thread;
  ISR_Level level;

  (void) id;

  _ISR_Disable( level );

  if ( rival->status == MRSP_WAIT_FOR_OWNERSHIP ) {
    _Chain_Extract_unprotected( &rival->Node );

    _ISR_Enable( level );

    rival->status = MRSP_TIMEOUT;

    _Resource_Node_extract( &thread->Resource_node );
    _Resource_Node_set_dependency( &thread->Resource_node, NULL );
    _Scheduler_Thread_change_help_state( thread, rival->initial_help_state );
    _Scheduler_Thread_change_resource_root( thread, thread );
    _MRSP_Restore_priority( thread, rival->initial_priority );
  } else {
    _ISR_Enable( level );
  }
}

RTEMS_INLINE_ROUTINE MRSP_Status _MRSP_Wait_for_ownership(
  MRSP_Control      *mrsp,
  Resource_Node     *owner,
  Thread_Control    *executing,
  Priority_Control   initial_priority,
  Priority_Control   ceiling_priority,
  Watchdog_Interval  timeout
)
{
  MRSP_Status status;
  MRSP_Rival rival;
  bool initial_life_protection;
  ISR_Level level;

  rival.thread = executing;
  rival.initial_priority = initial_priority;
  rival.initial_help_state =
    _Scheduler_Thread_change_help_state( executing, SCHEDULER_HELP_ACTIVE_RIVAL );
  rival.status = MRSP_WAIT_FOR_OWNERSHIP;

  _MRSP_Elevate_priority( mrsp, executing, ceiling_priority );

  _ISR_Disable( level );
  _Chain_Append_unprotected( &mrsp->Rivals, &rival.Node );
  _ISR_Enable( level );

  _Resource_Add_rival( &mrsp->Resource, &executing->Resource_node );
  _Resource_Node_set_dependency( &executing->Resource_node, &mrsp->Resource );

  _Scheduler_Thread_change_resource_root(
    executing,
    THREAD_RESOURCE_NODE_TO_THREAD( _Resource_Node_get_root( owner ) )
  );

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
  _Thread_Enable_dispatch();

  _Assert( _Debug_Is_thread_dispatching_allowed() );

  /* Wait for state change */
  do {
    status = rival.status;
  } while ( status == MRSP_WAIT_FOR_OWNERSHIP );

  _Thread_Disable_dispatch();
  _Thread_Set_life_protection( initial_life_protection );

  if ( timeout > 0 ) {
    _Watchdog_Remove( &executing->Timer );
  }

  return status;
}

RTEMS_INLINE_ROUTINE MRSP_Status _MRSP_Obtain(
  MRSP_Control      *mrsp,
  Thread_Control    *executing,
  bool               wait,
  Watchdog_Interval  timeout
)
{
  MRSP_Status status;
  const Scheduler_Control *scheduler = _Scheduler_Get( executing );
  uint32_t scheduler_index = _Scheduler_Get_index( scheduler );
  Priority_Control initial_priority = executing->current_priority;
  Priority_Control ceiling_priority =
    _MRSP_Get_ceiling_priority( mrsp, scheduler_index );
  bool priority_ok = !_Scheduler_Is_priority_higher_than(
    scheduler,
    initial_priority,
    ceiling_priority
  );
  Resource_Node *owner;

  if ( !priority_ok) {
    return MRSP_INVALID_PRIORITY;
  }

  owner = _Resource_Get_owner( &mrsp->Resource );
  if ( owner == NULL ) {
    _MRSP_Claim_ownership(
      mrsp,
      executing,
      initial_priority,
      ceiling_priority
    );
    status = MRSP_SUCCESSFUL;
  } else if ( _Resource_Node_get_root( owner ) == &executing->Resource_node ) {
    /* Nested access or deadlock */
    status = MRSP_UNSATISFIED;
  } else if ( wait ) {
    status = _MRSP_Wait_for_ownership(
      mrsp,
      owner,
      executing,
      initial_priority,
      ceiling_priority,
      timeout
    );
  } else {
    status = MRSP_UNSATISFIED;
  }

  return status;
}

RTEMS_INLINE_ROUTINE MRSP_Status _MRSP_Release(
  MRSP_Control   *mrsp,
  Thread_Control *executing
)
{
  ISR_Level level;

  if ( _Resource_Get_owner( &mrsp->Resource ) != &executing->Resource_node ) {
    return MRSP_NOT_OWNER_OF_RESOURCE;
  }

  if (
    !_Resource_Is_most_recently_obtained(
      &mrsp->Resource,
      &executing->Resource_node
    )
  ) {
    return MRSP_INCORRECT_STATE;
  }

  _Resource_Extract( &mrsp->Resource );
  _MRSP_Restore_priority( executing, mrsp->initial_priority_of_owner );

  _ISR_Disable( level );

  if ( _Chain_Is_empty( &mrsp->Rivals ) ) {
    _ISR_Enable( level );

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

    _ISR_Enable( level );

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

  return MRSP_SUCCESSFUL;
}

RTEMS_INLINE_ROUTINE MRSP_Status _MRSP_Destroy( MRSP_Control *mrsp )
{
  if ( _Resource_Get_owner( &mrsp->Resource ) != NULL ) {
    return MRSP_RESOUCE_IN_USE;
  }

  _Workspace_Free( mrsp->ceiling_priorities );

  return MRSP_SUCCESSFUL;
}

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RTEMS_SMP */

#endif /* _RTEMS_SCORE_MRSPIMPL_H */
