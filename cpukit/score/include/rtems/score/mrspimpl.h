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

#define MRSP_RIVAL_STATE_WAITING 0x0U

#define MRSP_RIVAL_STATE_NEW_OWNER 0x1U

#define MRSP_RIVAL_STATE_TIMEOUT 0x2U

RTEMS_INLINE_ROUTINE void _MRSP_Claim_ownership(
  MRSP_Control     *mrsp,
  Thread_Control   *new_owner,
  Priority_Control  ceiling_priority
)
{
  ++new_owner->resource_count;
  mrsp->owner = new_owner;
  _Thread_Change_priority( new_owner, ceiling_priority, false );
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

  mrsp->owner = NULL;
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

RTEMS_INLINE_ROUTINE void _MRSP_Restore_priority( Thread_Control *thread )
{
  _Thread_Change_priority( thread, thread->real_priority, true );
}

RTEMS_INLINE_ROUTINE void _MRSP_Add_state(
  MRSP_Rival   *rival,
  unsigned int  state
)
{
  _Atomic_Fetch_or_uint( &rival->state, state, ATOMIC_ORDER_RELEASE );
}

RTEMS_INLINE_ROUTINE void _MRSP_Timeout(
  Objects_Id  id,
  void       *arg
)
{
  MRSP_Rival *rival = arg;

  (void) id;

  _MRSP_Add_state( rival, MRSP_RIVAL_STATE_TIMEOUT );
}

RTEMS_INLINE_ROUTINE MRSP_Status _MRSP_Wait_for_ownership(
  MRSP_Control      *mrsp,
  Thread_Control    *executing,
  Priority_Control   ceiling_priority,
  Watchdog_Interval  timeout
)
{
  MRSP_Status status;
  MRSP_Rival rival;
  bool previous_life_protection;
  unsigned int state;

  _Thread_Change_priority( executing, ceiling_priority, false );

  rival.thread = executing;
  _Atomic_Init_uint( &rival.state, MRSP_RIVAL_STATE_WAITING );
  _Chain_Append_unprotected( &mrsp->Rivals, &rival.Node );

  if ( timeout > 0 ) {
    _Watchdog_Initialize(
      &executing->Timer,
      _MRSP_Timeout,
      0,
      &rival
    );
    _Watchdog_Insert_ticks( &executing->Timer, timeout );
  }

  previous_life_protection = _Thread_Set_life_protection( true );
  _Thread_Enable_dispatch();

  _Assert( _Debug_Is_thread_dispatching_allowed() );

  while (
    _Atomic_Load_uint( &rival.state, ATOMIC_ORDER_ACQUIRE )
      == MRSP_RIVAL_STATE_WAITING
  ) {
    /* Wait for state change */
  }

  _Thread_Disable_dispatch();
  _Thread_Set_life_protection( previous_life_protection );

  if ( timeout > 0 ) {
    _Watchdog_Remove( &executing->Timer );
  }

  _Chain_Extract_unprotected( &rival.Node );
  state = _Atomic_Load_uint( &rival.state, ATOMIC_ORDER_RELAXED );

  if ( ( state & MRSP_RIVAL_STATE_NEW_OWNER ) != 0 ) {
    ++executing->resource_count;

    status = MRSP_SUCCESSFUL;
  } else {
    if ( executing->resource_count == 0 ) {
      _MRSP_Restore_priority( executing );
    }

    status = MRSP_TIMEOUT;
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
  Priority_Control ceiling_priority =
    _MRSP_Get_ceiling_priority( mrsp, scheduler_index );
  bool priority_ok = !_Scheduler_Is_priority_higher_than(
    scheduler,
    executing->current_priority,
    ceiling_priority
  );

  if ( !priority_ok) {
    return MRSP_INVALID_PRIORITY;
  }

  if ( mrsp->owner == NULL ) {
    _MRSP_Claim_ownership( mrsp, executing, ceiling_priority );
    status = MRSP_SUCCESSFUL;
  } else if ( mrsp->owner == executing ) {
    status = MRSP_UNSATISFIED;
  } else if ( wait ) {
    status = _MRSP_Wait_for_ownership(
      mrsp,
      executing,
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
  uint32_t resource_count = executing->resource_count;

  if ( mrsp->owner != executing ) {
    return MRSP_NOT_OWNER_OF_RESOURCE;
  }

  if ( resource_count == 1 ) {
    executing->resource_count = 0;
    _MRSP_Restore_priority( executing );
  } else {
    executing->resource_count = resource_count - 1;
  }

  if ( _Chain_Is_empty( &mrsp->Rivals ) ) {
    mrsp->owner = NULL;
  } else {
    MRSP_Rival *rival = (MRSP_Rival *) _Chain_First( &mrsp->Rivals );

    mrsp->owner = rival->thread;
    _MRSP_Add_state( rival, MRSP_RIVAL_STATE_NEW_OWNER );
  }

  return MRSP_SUCCESSFUL;
}

RTEMS_INLINE_ROUTINE MRSP_Status _MRSP_Destroy( MRSP_Control *mrsp )
{
  if ( mrsp->owner != NULL ) {
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
