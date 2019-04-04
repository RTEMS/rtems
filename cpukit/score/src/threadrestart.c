/**
 * @file
 *
 * @brief Restart Thread 
 * @ingroup RTEMSScoreThread
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (c) 2014, 2016 embedded brains GmbH.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
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
#include <rtems/score/wkspace.h>

#define THREAD_JOIN_TQ_OPERATIONS &_Thread_queue_Operations_priority

static void _Thread_Life_action_handler(
  Thread_Control   *executing,
  Thread_Action    *action,
  ISR_lock_Context *lock_context
);

typedef struct {
  Chain_Control Chain;
  ISR_lock_Control Lock;
} Thread_Zombie_control;

static Thread_Zombie_control _Thread_Zombies = {
  .Chain = CHAIN_INITIALIZER_EMPTY( _Thread_Zombies.Chain ),
  .Lock = ISR_LOCK_INITIALIZER( "thread zombies" )
};

static void _Thread_Raise_real_priority(
  Thread_Control   *the_thread,
  Priority_Control  priority
)
{
  Thread_queue_Context queue_context;

  _Thread_queue_Context_initialize( &queue_context );
  _Thread_queue_Context_clear_priority_updates( &queue_context );
  _Thread_Wait_acquire( the_thread, &queue_context );

  if ( priority < the_thread->Real_priority.priority ) {
    _Thread_Priority_change(
      the_thread,
      &the_thread->Real_priority,
      priority,
      false,
      &queue_context
    );
  }

  _Thread_Wait_release( the_thread, &queue_context );
  _Thread_Priority_update( &queue_context );
}

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

static void _Thread_Add_to_zombie_chain( Thread_Control *the_thread )
{
  ISR_lock_Context       lock_context;
  Thread_Zombie_control *zombies;

  zombies = &_Thread_Zombies;
  _ISR_lock_ISR_disable_and_acquire( &zombies->Lock, &lock_context );
  _Chain_Append_unprotected( &zombies->Chain, &the_thread->Object.Node );
  _ISR_lock_Release_and_ISR_enable( &zombies->Lock, &lock_context );
}

static void _Thread_Make_zombie( Thread_Control *the_thread )
{
#if defined(RTEMS_SCORE_THREAD_ENABLE_RESOURCE_COUNT)
  if ( _Thread_Owns_resources( the_thread ) ) {
    _Internal_error( INTERNAL_ERROR_RESOURCE_IN_USE );
  }
#endif

  _Objects_Close(
    _Objects_Get_information_id( the_thread->Object.id ),
    &the_thread->Object
  );

  _Thread_Set_state( the_thread, STATES_ZOMBIE );
  _Thread_queue_Extract_with_proxy( the_thread );
  _Thread_Timer_remove( the_thread );

  /*
   * Add the thread to the thread zombie chain before we wake up joining
   * threads, so that they are able to clean up the thread immediately.  This
   * matters for SMP configurations.
   */
  _Thread_Add_to_zombie_chain( the_thread );

  _Thread_Wake_up_joining_threads( the_thread );
}

static void _Thread_Free( Thread_Control *the_thread )
{
  Thread_Information *information = (Thread_Information *)
    _Objects_Get_information_id( the_thread->Object.id );

  _User_extensions_Thread_delete( the_thread );
  _User_extensions_Destroy_iterators( the_thread );
  _ISR_lock_Destroy( &the_thread->Keys.Lock );
  _Scheduler_Node_destroy(
    _Thread_Scheduler_get_home( the_thread ),
    _Thread_Scheduler_get_home_node( the_thread )
  );
  _ISR_lock_Destroy( &the_thread->Timer.Lock );

  /*
   *  The thread might have been FP.  So deal with that.
   */
#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
#if ( CPU_USE_DEFERRED_FP_SWITCH == TRUE )
  if ( _Thread_Is_allocated_fp( the_thread ) )
    _Thread_Deallocate_fp();
#endif

  _Workspace_Free( the_thread->Start.fp_context );
#endif

  _Freechain_Put(
    &information->Thread_queue_heads.Free,
    the_thread->Wait.spare_heads
  );

  /*
   *  Free the rest of the memory associated with this task
   *  and set the associated pointers to NULL for safety.
   */
  _Thread_Stack_Free( the_thread );

  _Workspace_Free( the_thread->Start.tls_area );

#if defined(RTEMS_SMP)
  _ISR_lock_Destroy( &the_thread->Scheduler.Lock );
  _ISR_lock_Destroy( &the_thread->Wait.Lock.Default );
  _SMP_lock_Stats_destroy( &the_thread->Potpourri_stats );
#endif

  _Thread_queue_Destroy( &the_thread->Join_queue );
  _Context_Destroy( the_thread, &the_thread->Registers );
  _Objects_Free( &information->Objects, &the_thread->Object );
}

static void _Thread_Wait_for_execution_stop( Thread_Control *the_thread )
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

void _Thread_Kill_zombies( void )
{
  ISR_lock_Context lock_context;
  Thread_Zombie_control *zombies = &_Thread_Zombies;
  Thread_Control *the_thread;

  _ISR_lock_ISR_disable_and_acquire( &zombies->Lock, &lock_context );

  the_thread = (Thread_Control *) _Chain_Get_unprotected( &zombies->Chain );
  while ( the_thread != NULL ) {
    _ISR_lock_Release_and_ISR_enable( &zombies->Lock, &lock_context );

    _Thread_Wait_for_execution_stop( the_thread );
    _Thread_Free( the_thread );

    _ISR_lock_ISR_disable_and_acquire( &zombies->Lock, &lock_context );

    the_thread = (Thread_Control *) _Chain_Get_unprotected( &zombies->Chain );
  }

  _ISR_lock_Release_and_ISR_enable( &zombies->Lock, &lock_context );
}

static Thread_Life_state _Thread_Change_life_locked(
  Thread_Control    *the_thread,
  Thread_Life_state  clear,
  Thread_Life_state  set,
  Thread_Life_state  ignore
)
{
  Thread_Life_state previous;
  Thread_Life_state state;

  previous = the_thread->Life.state;
  state = previous;
  state &= ~clear;
  state |= set;
  the_thread->Life.state = state;

  state &= ~ignore;

  if (
    _Thread_Is_life_change_allowed( state )
      && _Thread_Is_life_changing( state )
  ) {
    the_thread->is_preemptible   = the_thread->Start.is_preemptible;
    the_thread->budget_algorithm = the_thread->Start.budget_algorithm;
    the_thread->budget_callout   = the_thread->Start.budget_callout;

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
    _Thread_Dispatch_direct( cpu_self );
    RTEMS_UNREACHABLE();
  }

  _Assert( _Thread_Is_life_restarting( previous_life_state ) );

  _Thread_State_acquire( executing, lock_context );

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
  _Assert(
    executing->current_state == STATES_READY
      || executing->current_state == STATES_SUSPENDED
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
     * _Thread_queue_Extract_with_proxy() for removal of these states.
     */
    _Thread_Clear_state_locked(
      the_thread,
      STATES_LIFE_IS_CHANGING | STATES_SUSPENDED
        | ( STATES_BLOCKED & ~STATES_LOCALLY_BLOCKED )
    );
  }

  _Thread_State_release( the_thread, &lock_context );
}

static void _Thread_Finalize_life_change(
  Thread_Control   *the_thread,
  Priority_Control  priority
)
{
  _Thread_queue_Extract_with_proxy( the_thread );
  _Thread_Timer_remove( the_thread );
  _Thread_Raise_real_priority( the_thread, priority );
  _Thread_Remove_life_change_request( the_thread );
}

void _Thread_Join(
  Thread_Control       *the_thread,
  States_Control        waiting_for_join,
  Thread_Control       *executing,
  Thread_queue_Context *queue_context
)
{
  _Assert( the_thread != executing );
  _Assert( _Thread_State_is_owner( the_thread ) );

  executing->Wait.return_argument = NULL;

  _Thread_queue_Context_set_thread_state( queue_context, waiting_for_join );
  _Thread_queue_Enqueue(
    &the_thread->Join_queue.Queue,
    THREAD_JOIN_TQ_OPERATIONS,
    executing,
    queue_context
  );
}

static void _Thread_Set_exit_value(
  Thread_Control *the_thread,
  void           *exit_value
)
{
  the_thread->Life.exit_value = exit_value;
}

void _Thread_Cancel(
  Thread_Control *the_thread,
  Thread_Control *executing,
  void           *exit_value
)
{
  ISR_lock_Context   lock_context;
  Thread_Life_state  previous;
  Per_CPU_Control   *cpu_self;
  Priority_Control   priority;

  _Assert( the_thread != executing );

  _Thread_State_acquire( the_thread, &lock_context );

  _Thread_Set_exit_value( the_thread, exit_value );
  previous = _Thread_Change_life_locked(
    the_thread,
    0,
    THREAD_LIFE_TERMINATING,
    0
  );

  cpu_self = _Thread_Dispatch_disable_critical( &lock_context );
  priority = _Thread_Get_priority( executing );

  if ( _States_Is_dormant( the_thread->current_state ) ) {
    _Thread_State_release( the_thread, &lock_context );
    _Thread_Make_zombie( the_thread );
  } else if ( _Thread_Is_life_change_allowed( previous ) ) {
    _Thread_Add_life_change_request( the_thread );
    _Thread_State_release( the_thread, &lock_context );

    _Thread_Finalize_life_change( the_thread, priority );
  } else {
    _Thread_Add_life_change_request( the_thread );
    _Thread_Clear_state_locked( the_thread, STATES_SUSPENDED );
    _Thread_State_release( the_thread, &lock_context );

    _Thread_Raise_real_priority( the_thread, priority );
    _Thread_Remove_life_change_request( the_thread );
  }

  _Thread_Dispatch_enable( cpu_self );
}

static void _Thread_Close_enqueue_callout(
  Thread_queue_Queue   *queue,
  Thread_Control       *the_thread,
  Per_CPU_Control      *cpu_self,
  Thread_queue_Context *queue_context
)
{
  Thread_Close_context *context;

  context = (Thread_Close_context *) queue_context;
  _Thread_Cancel( context->cancel, the_thread, NULL );
}

void _Thread_Close(
  Thread_Control       *the_thread,
  Thread_Control       *executing,
  Thread_Close_context *context
)
{
  context->cancel = the_thread;
  _Thread_queue_Context_set_enqueue_callout(
    &context->Base,
    _Thread_Close_enqueue_callout
  );
  _Thread_State_acquire_critical(
    the_thread,
    &context->Base.Lock_context.Lock_context
  );
  _Thread_Join(
    the_thread,
    STATES_WAITING_FOR_JOIN,
    executing,
    &context->Base
  );
}

void _Thread_Exit(
  Thread_Control    *executing,
  Thread_Life_state  set,
  void              *exit_value
)
{
  ISR_lock_Context lock_context;

  _Assert(
    _Watchdog_Get_state( &executing->Timer.Watchdog ) == WATCHDOG_INACTIVE
  );
  _Assert(
    executing->current_state == STATES_READY
      || executing->current_state == STATES_SUSPENDED
  );

  _Thread_State_acquire( executing, &lock_context );
  _Thread_Set_exit_value( executing, exit_value );
  _Thread_Change_life_locked(
    executing,
    0,
    set,
    THREAD_LIFE_PROTECTED | THREAD_LIFE_CHANGE_DEFERRED
  );
  _Thread_State_release( executing, &lock_context );
}

bool _Thread_Restart_other(
  Thread_Control                 *the_thread,
  const Thread_Entry_information *entry,
  ISR_lock_Context               *lock_context
)
{
  Thread_Life_state  previous;
  Per_CPU_Control   *cpu_self;

  _Thread_State_acquire_critical( the_thread, lock_context );

  if ( _States_Is_dormant( the_thread->current_state ) ) {
    _Thread_State_release( the_thread, lock_context );
    return false;
  }

  the_thread->Start.Entry = *entry;
  previous = _Thread_Change_life_locked(
    the_thread,
    0,
    THREAD_LIFE_RESTARTING,
    0
  );

  cpu_self = _Thread_Dispatch_disable_critical( lock_context );

  if ( _Thread_Is_life_change_allowed( previous ) ) {
    _Thread_Add_life_change_request( the_thread );
    _Thread_State_release( the_thread, lock_context );

    _Thread_Finalize_life_change(
      the_thread,
      the_thread->Start.initial_priority
    );
  } else {
    _Thread_Clear_state_locked( the_thread, STATES_SUSPENDED );
    _Thread_State_release( the_thread, lock_context );
  }

  _Thread_Dispatch_enable( cpu_self );
  return true;
}

void _Thread_Restart_self(
  Thread_Control                 *executing,
  const Thread_Entry_information *entry,
  ISR_lock_Context               *lock_context
)
{
  Per_CPU_Control      *cpu_self;
  Thread_queue_Context  queue_context;

  _Assert(
    _Watchdog_Get_state( &executing->Timer.Watchdog ) == WATCHDOG_INACTIVE
  );
  _Assert(
    executing->current_state == STATES_READY
      || executing->current_state == STATES_SUSPENDED
  );

  _Thread_queue_Context_initialize( &queue_context );
  _Thread_queue_Context_clear_priority_updates( &queue_context );
  _Thread_State_acquire_critical( executing, lock_context );

  executing->Start.Entry = *entry;
  _Thread_Change_life_locked(
    executing,
    0,
    THREAD_LIFE_RESTARTING,
    THREAD_LIFE_PROTECTED | THREAD_LIFE_CHANGE_DEFERRED
  );

  cpu_self = _Thread_Dispatch_disable_critical( lock_context );
  _Thread_State_release( executing, lock_context );

  _Thread_Wait_acquire_default( executing, lock_context );
  _Thread_Priority_change(
    executing,
    &executing->Real_priority,
    executing->Start.initial_priority,
    false,
    &queue_context
  );
  _Thread_Wait_release_default( executing, lock_context );

  _Thread_Priority_update( &queue_context );
  _Thread_Dispatch_direct( cpu_self );
  RTEMS_UNREACHABLE();
}

Thread_Life_state _Thread_Change_life(
  Thread_Life_state clear,
  Thread_Life_state set,
  Thread_Life_state ignore
)
{
  ISR_lock_Context   lock_context;
  Thread_Control    *executing;
  Per_CPU_Control   *cpu_self;
  Thread_Life_state  previous;

  executing = _Thread_State_acquire_for_executing( &lock_context );

  previous = _Thread_Change_life_locked( executing, clear, set, ignore );

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
