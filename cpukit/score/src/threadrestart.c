/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreThread
 *
 * @brief This source file contains the implementation of _Thread_Cancel(),
 *   _Thread_Change_life(), _Thread_Close(), _Thread_Exit(), _Thread_Join(),
 *   _Thread_Kill_zombies(), _Thread_Restart(), and _Thread_Set_life_protection().
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Copyright (C) 2014, 2022 embedded brains GmbH & Co. KG
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

#include <rtems/score/threadimpl.h>
#include <rtems/score/apimutex.h>
#include <rtems/score/assert.h>
#include <rtems/score/chainimpl.h>
#include <rtems/score/isrlock.h>
#include <rtems/score/schedulerimpl.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/threadqimpl.h>
#include <rtems/score/userextimpl.h>
#include <rtems/score/watchdogimpl.h>

#include <pthread.h>

#define THREAD_JOIN_TQ_OPERATIONS &_Thread_queue_Operations_priority_inherit

static void _Thread_Life_action_handler(
  Thread_Control   *executing,
  Thread_Action    *action,
  ISR_lock_Context *lock_context
);

Thread_Zombie_registry _Thread_Zombies = {
#if defined(RTEMS_SMP)
  .Lock = ISR_LOCK_INITIALIZER( "Thread Zombies" ),
#endif
  .Chain = CHAIN_INITIALIZER_EMPTY( _Thread_Zombies.Chain )
};

typedef struct {
  Thread_queue_Context  Base;
  void                 *exit_value;
} Thread_Join_context;

static Thread_Control *_Thread_Join_flush_filter(
  Thread_Control       *the_thread,
  Thread_queue_Queue   *queue,
  Thread_queue_Context *queue_context
)
{
  Thread_Join_context *join_context;

  join_context = (Thread_Join_context *) queue_context;

  the_thread->Wait.return_argument = join_context->exit_value;

  return the_thread;
}

static void _Thread_Wake_up_joining_threads( Thread_Control *the_thread )
{
  Thread_Join_context join_context;

  join_context.exit_value = the_thread->Life.exit_value;

  _Thread_queue_Context_initialize( &join_context.Base );
  _Thread_queue_Acquire( &the_thread->Join_queue, &join_context.Base );
  _Thread_queue_Flush_critical(
    &the_thread->Join_queue.Queue,
    THREAD_JOIN_TQ_OPERATIONS,
    _Thread_Join_flush_filter,
    &join_context.Base
  );
}

static void _Thread_Add_to_zombie_registry( Thread_Control *the_thread )
{
  ISR_lock_Context        lock_context;
  Thread_Zombie_registry *zombies;

  zombies = &_Thread_Zombies;
  _ISR_lock_ISR_disable_and_acquire( &zombies->Lock, &lock_context );
  _Chain_Append_unprotected( &zombies->Chain, &the_thread->Object.Node );
  _ISR_lock_Release_and_ISR_enable( &zombies->Lock, &lock_context );
}

static void _Thread_Make_zombie( Thread_Control *the_thread )
{
  Thread_Information *information;

#if defined(RTEMS_SCORE_THREAD_ENABLE_RESOURCE_COUNT)
  if ( _Thread_Owns_resources( the_thread ) ) {
    _Internal_error( INTERNAL_ERROR_RESOURCE_IN_USE );
  }
#endif

  information = _Thread_Get_objects_information( the_thread );
  _Objects_Close( &information->Objects, &the_thread->Object );

  _Thread_Set_state( the_thread, STATES_ZOMBIE );
  _Thread_Timer_remove_and_continue( the_thread, STATUS_INTERNAL_ERROR );

  /*
   * Add the thread to the thread zombie chain before we wake up joining
   * threads, so that they are able to clean up the thread immediately.  This
   * matters for SMP configurations.
   */
  _Thread_Add_to_zombie_registry( the_thread );

  _Thread_Wake_up_joining_threads( the_thread );
}

static void _Thread_Wait_for_execution_stop( const Thread_Control *the_thread )
{
#if defined(RTEMS_SMP)
  /*
   * It is very unlikely that we see an executing thread here.  It can happen
   * in case the thread termination sequence is interrupted by a slow interrupt
   * service on a remote processor.
   */
  while ( _Thread_Is_executing_on_a_processor( the_thread ) ) {
    /* Wait */
  }
#else
  (void) the_thread;
#endif
}

static Thread_Control *_Thread_Get_zombie( Thread_Zombie_registry *zombies )
{
  return (Thread_Control *) _Chain_Get_unprotected( &zombies->Chain );
}

void _Thread_Kill_zombies( void )
{
  ISR_lock_Context        lock_context;
  Thread_Zombie_registry *zombies;
  Thread_Control         *the_thread;

  zombies = &_Thread_Zombies;
  _ISR_lock_ISR_disable_and_acquire( &zombies->Lock, &lock_context );

  while ( ( the_thread = _Thread_Get_zombie( zombies ) ) != NULL ) {
    Thread_Information *information;

    _ISR_lock_Release_and_ISR_enable( &zombies->Lock, &lock_context );

    _Thread_Wait_for_execution_stop( the_thread );
    information = _Thread_Get_objects_information( the_thread );
    _Thread_Free( information, the_thread );

    _ISR_lock_ISR_disable_and_acquire( &zombies->Lock, &lock_context );
  }

  _ISR_lock_Release_and_ISR_enable( &zombies->Lock, &lock_context );
}

static Thread_Life_state _Thread_Change_life_locked(
  Thread_Control    *the_thread,
  Thread_Life_state  life_states_to_clear,
  Thread_Life_state  life_states_to_set,
  Thread_Life_state  ignored_life_states
)
{
  Thread_Life_state previous;
  Thread_Life_state state;

  previous = the_thread->Life.state;
  state = previous;
  state &= ~life_states_to_clear;
  state |= life_states_to_set;
  the_thread->Life.state = state;

  state &= ~ignored_life_states;

  if (
    _Thread_Is_life_change_allowed( state )
      && _Thread_Is_life_changing( state )
  ) {
    _Thread_Add_post_switch_action(
      the_thread,
      &the_thread->Life.Action,
      _Thread_Life_action_handler
    );
  }

  return previous;
}

static Per_CPU_Control *_Thread_Wait_for_join(
  Thread_Control  *executing,
  Per_CPU_Control *cpu_self
)
{
  ISR_lock_Context lock_context;

  _Thread_State_acquire( executing, &lock_context );

  if (
    _Thread_Is_joinable( executing )
      && _Thread_queue_Is_empty( &executing->Join_queue.Queue )
  ) {
    _Thread_Set_state_locked( executing, STATES_WAITING_FOR_JOIN_AT_EXIT );
    _Thread_State_release( executing, &lock_context );
    _Thread_Dispatch_direct( cpu_self );

    /* Let other threads run */

    cpu_self = _Thread_Dispatch_disable();
  } else {
    _Thread_State_release( executing, &lock_context );
  }

  return cpu_self;
}

void _Thread_Life_action_handler(
  Thread_Control   *executing,
  Thread_Action    *action,
  ISR_lock_Context *lock_context
)
{
  Thread_Life_state  previous_life_state;
  Per_CPU_Control   *cpu_self;

  (void) action;

  previous_life_state = executing->Life.state;
  executing->Life.state = previous_life_state | THREAD_LIFE_PROTECTED;

  _Thread_State_release( executing, lock_context );

  if ( _Thread_Is_life_terminating( previous_life_state ) ) {
    _User_extensions_Thread_terminate( executing );
  } else {
    _Assert( _Thread_Is_life_restarting( previous_life_state ) );

    _User_extensions_Thread_restart( executing );
  }

  cpu_self = _Thread_Dispatch_disable();

  if ( _Thread_Is_life_terminating( previous_life_state ) ) {
    cpu_self = _Thread_Wait_for_join( executing, cpu_self );
    _Thread_Make_zombie( executing );
    _Thread_Dispatch_direct_no_return( cpu_self );
    RTEMS_UNREACHABLE();
  }

  _Assert( _Thread_Is_life_restarting( previous_life_state ) );

  _Thread_State_acquire( executing, lock_context );

  /*
   * The executing thread runs with thread dispatching disabled right now.
   * Other threads may have suspended the executing thread.  The thread life
   * handler may run in parallel with _Thread_Add_life_change_request() which
   * may have set STATES_LIFE_IS_CHANGING.
   */
  _Assert(
    executing->current_state == STATES_READY
      || executing->current_state == STATES_SUSPENDED
      || executing->current_state == STATES_LIFE_IS_CHANGING
  );

  _Thread_Change_life_locked(
    executing,
    THREAD_LIFE_PROTECTED | THREAD_LIFE_RESTARTING,
    0,
    0
  );

  _Thread_State_release( executing, lock_context );

  _Assert(
    _Watchdog_Get_state( &executing->Timer.Watchdog ) == WATCHDOG_INACTIVE
  );

  _User_extensions_Destroy_iterators( executing );
  _Thread_Load_environment( executing );

#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
  if ( executing->fp_context != NULL ) {
    _Context_Restore_fp( &executing->fp_context );
  }
#endif

  _Context_Restart_self( &executing->Registers );
  RTEMS_UNREACHABLE();
}

static void _Thread_Add_life_change_request( Thread_Control *the_thread )
{
  uint32_t pending_requests;

  _Assert( _Thread_State_is_owner( the_thread ) );

  pending_requests = the_thread->Life.pending_life_change_requests;
  the_thread->Life.pending_life_change_requests = pending_requests + 1;

  if ( pending_requests == 0 ) {
    _Thread_Set_state_locked( the_thread, STATES_LIFE_IS_CHANGING );
  }
}

static void _Thread_Remove_life_change_request( Thread_Control *the_thread )
{
  ISR_lock_Context lock_context;
  uint32_t         pending_requests;

  _Thread_State_acquire( the_thread, &lock_context );

  pending_requests = the_thread->Life.pending_life_change_requests;
  the_thread->Life.pending_life_change_requests = pending_requests - 1;

  if ( pending_requests == 1 ) {
    /*
     * Do not remove states used for thread queues to avoid race conditions on
     * SMP configurations.  We could interrupt an extract operation on another
     * processor disregarding the thread wait flags.  Rely on
     * _Thread_Continue() for removal of these states.
     */
    _Thread_Clear_state_locked(
      the_thread,
      STATES_LIFE_IS_CHANGING | STATES_SUSPENDED
        | ( STATES_BLOCKED & ~STATES_LOCALLY_BLOCKED )
    );
  }

  _Thread_State_release( the_thread, &lock_context );
}

static void _Thread_Clear_waiting_for_join_at_exit(
  Thread_queue_Queue   *queue,
  Thread_Control       *the_thread,
  Per_CPU_Control      *cpu_self,
  Thread_queue_Context *queue_context
)
{
  (void) the_thread;
  (void) cpu_self;
  (void) queue_context;
  _Thread_Clear_state( queue->owner, STATES_WAITING_FOR_JOIN_AT_EXIT );
}

Status_Control _Thread_Join(
  Thread_Control       *the_thread,
  States_Control        waiting_for_join,
  Thread_Control       *executing,
  Thread_queue_Context *queue_context
)
{
  _Assert( _Thread_State_is_owner( the_thread ) );

  executing->Wait.return_argument = NULL;
  _Thread_queue_Context_set_enqueue_callout(
    queue_context,
    _Thread_Clear_waiting_for_join_at_exit
  );
  _Thread_queue_Context_set_deadlock_callout(
    queue_context,
    _Thread_queue_Deadlock_status
  );
  _Thread_queue_Context_set_thread_state( queue_context, waiting_for_join );
  _Thread_queue_Enqueue(
    &the_thread->Join_queue.Queue,
    THREAD_JOIN_TQ_OPERATIONS,
    executing,
    queue_context
  );
  return _Thread_Wait_get_status( executing );
}

static void _Thread_Set_exit_value(
  Thread_Control *the_thread,
  void           *exit_value
)
{
  the_thread->Life.exit_value = exit_value;
}

static void _Thread_Try_life_change_request(
  Thread_Control    *the_thread,
  Thread_Life_state  previous,
  ISR_lock_Context  *lock_context
)
{
  if ( _Thread_Is_life_change_allowed( previous ) ) {
    _Thread_Add_life_change_request( the_thread );
    _Thread_State_release( the_thread, lock_context );

    _Thread_Timer_remove_and_continue( the_thread, STATUS_INTERNAL_ERROR );
    _Thread_Remove_life_change_request( the_thread );
  } else {
    _Thread_Clear_state_locked( the_thread, STATES_SUSPENDED );
    _Thread_State_release( the_thread, lock_context );
  }
}

Thread_Cancel_state _Thread_Cancel(
  Thread_Control   *the_thread,
  Thread_Control   *executing,
  Thread_Life_state life_states_to_clear
)
{
  ISR_lock_Context  lock_context;
  Thread_Life_state previous;

  _Assert( the_thread != executing );

  _Thread_State_acquire( the_thread, &lock_context );

  _Thread_Set_exit_value( the_thread, PTHREAD_CANCELED );
  previous = _Thread_Change_life_locked(
    the_thread,
    life_states_to_clear,
    THREAD_LIFE_TERMINATING,
    0
  );

  if ( _States_Is_dormant( the_thread->current_state ) ) {
    _Thread_State_release( the_thread, &lock_context );
    _Thread_Make_zombie( the_thread );
    return THREAD_CANCEL_DONE;
  }

  _Thread_Try_life_change_request( the_thread, previous, &lock_context );
  return THREAD_CANCEL_IN_PROGRESS;
}

Status_Control _Thread_Close(
  Thread_Control       *the_thread,
  Thread_Control       *executing,
  Thread_queue_Context *queue_context
)
{
  Per_CPU_Control    *cpu_self;
  Thread_Cancel_state cancel_state;

  cpu_self = _Thread_Dispatch_disable_critical(
    &queue_context->Lock_context.Lock_context
  );
  _ISR_lock_ISR_enable( &queue_context->Lock_context.Lock_context );

  cancel_state = _Thread_Cancel( the_thread, executing, THREAD_LIFE_DETACHED );

  if ( cancel_state == THREAD_CANCEL_DONE ) {
    _Thread_Dispatch_enable( cpu_self );
    return STATUS_SUCCESSFUL;
  }

  _ISR_lock_ISR_disable( &queue_context->Lock_context.Lock_context );
  _Thread_Dispatch_unnest( cpu_self );
  _Thread_State_acquire_critical(
    the_thread,
    &queue_context->Lock_context.Lock_context
  );

  return _Thread_Join(
    the_thread,
    STATES_WAITING_FOR_JOIN,
    executing,
    queue_context
  );
}

RTEMS_NO_RETURN void _Thread_Exit(
  void              *exit_value,
  Thread_Life_state  life_states_to_set
)
{
  Per_CPU_Control *cpu_self;
  Thread_Control  *executing;
  ISR_lock_Context lock_context;

  _ISR_lock_ISR_disable( &lock_context );
  cpu_self = _Thread_Dispatch_disable_critical( &lock_context );
  executing = _Per_CPU_Get_executing( cpu_self );

  _Assert(
    _Watchdog_Get_state( &executing->Timer.Watchdog ) == WATCHDOG_INACTIVE
  );
  _Assert(
    executing->current_state == STATES_READY
      || executing->current_state == STATES_SUSPENDED
  );

  _Thread_State_acquire_critical( executing, &lock_context );
  _Thread_Set_exit_value( executing, exit_value );
  _Thread_Change_life_locked(
    executing,
    0,
    life_states_to_set,
    THREAD_LIFE_PROTECTED | THREAD_LIFE_CHANGE_DEFERRED
  );
  _Thread_State_release( executing, &lock_context );

  _Thread_Dispatch_direct_no_return( cpu_self );
  RTEMS_UNREACHABLE();
}

Status_Control _Thread_Restart(
  Thread_Control                 *the_thread,
  const Thread_Entry_information *entry,
  ISR_lock_Context               *lock_context
)
{
  Thread_Life_state    previous;
  Per_CPU_Control     *cpu_self;
  bool                 is_self_restart;
  Thread_Life_state    ignored_life_states;
  Thread_queue_Context queue_context;

  _Thread_State_acquire_critical( the_thread, lock_context );

  if ( _States_Is_dormant( the_thread->current_state ) ) {
    _Thread_State_release( the_thread, lock_context );
    return STATUS_INCORRECT_STATE;
  }

  cpu_self = _Thread_Dispatch_disable_critical( lock_context );
  is_self_restart = ( the_thread == _Per_CPU_Get_executing( cpu_self ) &&
    !_Per_CPU_Is_ISR_in_progress( cpu_self ) );

  if ( is_self_restart ) {
    ignored_life_states = THREAD_LIFE_PROTECTED | THREAD_LIFE_CHANGE_DEFERRED;
  } else {
    ignored_life_states = 0;
  }

  the_thread->Start.Entry = *entry;
  previous = _Thread_Change_life_locked(
    the_thread,
    0,
    THREAD_LIFE_RESTARTING,
    ignored_life_states
  );
  _Thread_Try_life_change_request( the_thread, previous, lock_context );

  _Thread_queue_Context_initialize( &queue_context );
  _Thread_queue_Context_clear_priority_updates( &queue_context );
  _Thread_Wait_acquire( the_thread, &queue_context );
  _Thread_Priority_change(
    the_thread,
    &the_thread->Real_priority,
    the_thread->Start.initial_priority,
    PRIORITY_GROUP_LAST,
    &queue_context
  );
  _Thread_Wait_release( the_thread, &queue_context );

  _Thread_Priority_update( &queue_context );

  if ( is_self_restart ) {
    _Thread_Dispatch_direct_no_return( cpu_self );
  } else {
    _Thread_Dispatch_enable( cpu_self );
  }

  return STATUS_SUCCESSFUL;
}

Thread_Life_state _Thread_Change_life(
  Thread_Life_state life_states_to_clear,
  Thread_Life_state life_states_to_set,
  Thread_Life_state ignored_life_states
)
{
  ISR_lock_Context   lock_context;
  Thread_Control    *executing;
  Per_CPU_Control   *cpu_self;
  Thread_Life_state  previous;

  executing = _Thread_State_acquire_for_executing( &lock_context );

  previous = _Thread_Change_life_locked(
    executing,
    life_states_to_clear,
    life_states_to_set,
    ignored_life_states
  );

  cpu_self = _Thread_Dispatch_disable_critical( &lock_context );
  _Thread_State_release( executing, &lock_context );
  _Thread_Dispatch_enable( cpu_self );

  return previous;
}

Thread_Life_state _Thread_Set_life_protection( Thread_Life_state state )
{
  return _Thread_Change_life(
    THREAD_LIFE_PROTECTED,
    state & THREAD_LIFE_PROTECTED,
    0
  );
}
