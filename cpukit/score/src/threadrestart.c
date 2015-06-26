/**
 * @file
 *
 * @brief Restart Thread 
 * @ingroup ScoreThread
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (c) 2014 embedded brains GmbH.
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

typedef struct {
  Chain_Control Chain;
  ISR_lock_Control Lock;
} Thread_Zombie_control;

static Thread_Zombie_control _Thread_Zombies = {
  .Chain = CHAIN_INITIALIZER_EMPTY( _Thread_Zombies.Chain ),
  .Lock = ISR_LOCK_INITIALIZER( "thread zombies" )
};

static bool _Thread_Raise_real_priority_filter(
  Thread_Control   *the_thread,
  Priority_Control *new_priority_ptr,
  void             *arg
)
{
  Priority_Control real_priority;
  Priority_Control new_priority;
  Priority_Control current_priority;

  real_priority = the_thread->real_priority;
  new_priority = *new_priority_ptr;
  current_priority = the_thread->current_priority;

  new_priority = _Thread_Priority_highest( real_priority, new_priority );
  *new_priority_ptr = new_priority;

  the_thread->real_priority = new_priority;

  return _Thread_Priority_less_than( current_priority, new_priority );
}

static void _Thread_Make_zombie( Thread_Control *the_thread )
{
  ISR_lock_Context lock_context;
  Thread_Zombie_control *zombies = &_Thread_Zombies;

  if ( _Thread_Owns_resources( the_thread ) ) {
    _Terminate(
      INTERNAL_ERROR_CORE,
      false,
      INTERNAL_ERROR_RESOURCE_IN_USE
    );
  }

  _Objects_Close(
    _Objects_Get_information_id( the_thread->Object.id ),
    &the_thread->Object
  );

  _Thread_Set_state( the_thread, STATES_ZOMBIE );
  _Thread_queue_Extract_with_proxy( the_thread );
  _Watchdog_Remove_ticks( &the_thread->Timer );

  _ISR_lock_ISR_disable_and_acquire( &zombies->Lock, &lock_context );
  _Chain_Append_unprotected( &zombies->Chain, &the_thread->Object.Node );
  _ISR_lock_Release_and_ISR_enable( &zombies->Lock, &lock_context );
}

static void _Thread_Free( Thread_Control *the_thread )
{
  Thread_Information *information = (Thread_Information *)
    _Objects_Get_information_id( the_thread->Object.id );

  _User_extensions_Thread_delete( the_thread );

  /*
   * Free the per-thread scheduling information.
   */
  _Scheduler_Node_destroy( _Scheduler_Get( the_thread ), the_thread );

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
    &information->Free_thread_queue_heads,
    the_thread->Wait.spare_heads
  );

  /*
   *  Free the rest of the memory associated with this task
   *  and set the associated pointers to NULL for safety.
   */
  _Thread_Stack_Free( the_thread );

  _Workspace_Free( the_thread->Start.tls_area );

#if defined(RTEMS_SMP)
  _SMP_ticket_lock_Destroy( &the_thread->Lock.Default );
  _SMP_lock_Stats_destroy( &the_thread->Lock.Stats );
  _SMP_lock_Stats_destroy( &the_thread->Potpourri_stats );
#endif

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

static void _Thread_Start_life_change_for_executing(
  Thread_Control *executing
)
{
  _Assert( executing->Timer.state == WATCHDOG_INACTIVE );
  _Assert(
    executing->current_state == STATES_READY
      || executing->current_state == STATES_SUSPENDED
  );

  _Thread_Add_post_switch_action( executing, &executing->Life.Action );
}

void _Thread_Life_action_handler(
  Thread_Control  *executing,
  Thread_Action   *action,
  Per_CPU_Control *cpu,
  ISR_Level        level
)
{
  Thread_Life_state previous_life_state;

  (void) action;

  previous_life_state = executing->Life.state;
  executing->Life.state = THREAD_LIFE_PROTECTED;

  _Thread_Action_release_and_ISR_enable( cpu, level );

  if ( _Thread_Is_life_terminating( previous_life_state ) ) {
    _User_extensions_Thread_terminate( executing );
  } else {
    _Assert( _Thread_Is_life_restarting( previous_life_state ) );

    _User_extensions_Thread_restart( executing );
  }

  _Thread_Disable_dispatch();

  if ( _Thread_Is_life_terminating( previous_life_state ) ) {
    _Thread_Make_zombie( executing );

    if ( executing->Life.terminator != NULL ) {
      _Thread_Clear_state(
        executing->Life.terminator,
        STATES_WAITING_FOR_TERMINATION
      );
    }

    _Thread_Enable_dispatch();

    _Assert_Not_reached();
  } else {
    _Assert( _Thread_Is_life_restarting( previous_life_state ) );

    if ( _Thread_Is_life_terminating( executing->Life.state ) ) {
      /* Someone deleted us in the mean-time */
      _Thread_Start_life_change_for_executing( executing );
    } else {
      _Assert( executing->Timer.state == WATCHDOG_INACTIVE );
      _Assert(
        executing->current_state == STATES_READY
          || executing->current_state == STATES_SUSPENDED
      );

      executing->Life.state = THREAD_LIFE_NORMAL;

      _Thread_Load_environment( executing );
      _Thread_Restart_self( executing );

      _Assert_Not_reached();
    }
  }
}

static void _Thread_Start_life_change(
  Thread_Control          *the_thread,
  const Scheduler_Control *scheduler,
  Priority_Control         priority
)
{
  the_thread->is_preemptible   = the_thread->Start.is_preemptible;
  the_thread->budget_algorithm = the_thread->Start.budget_algorithm;
  the_thread->budget_callout   = the_thread->Start.budget_callout;

  _Thread_Set_state( the_thread, STATES_RESTARTING );
  _Thread_queue_Extract_with_proxy( the_thread );
  _Watchdog_Remove_ticks( &the_thread->Timer );
  _Thread_Change_priority(
    the_thread,
    priority,
    NULL,
    _Thread_Raise_real_priority_filter,
    false
  );
  _Thread_Add_post_switch_action( the_thread, &the_thread->Life.Action );
  _Thread_Ready( the_thread );
}

static void _Thread_Request_life_change(
  Thread_Control    *the_thread,
  Thread_Control    *executing,
  Priority_Control   priority,
  Thread_Life_state  additional_life_state
)
{
  Thread_Life_state previous_life_state;
  Per_CPU_Control *cpu;
  ISR_Level level;
  const Scheduler_Control *scheduler;

  cpu = _Thread_Action_ISR_disable_and_acquire( the_thread, &level );
  previous_life_state = the_thread->Life.state;
  the_thread->Life.state = previous_life_state | additional_life_state;
  _Thread_Action_release_and_ISR_enable( cpu, level );

  scheduler = _Scheduler_Get( the_thread );
  if ( the_thread == executing ) {
    Priority_Control unused;

    _Thread_Set_priority( the_thread, priority, &unused, true );
    _Thread_Start_life_change_for_executing( executing );
  } else if ( previous_life_state == THREAD_LIFE_NORMAL ) {
    _Thread_Start_life_change( the_thread, scheduler, priority );
  } else {
    _Thread_Clear_state( the_thread, STATES_SUSPENDED );

    if ( _Thread_Is_life_terminating( additional_life_state ) ) {
      _Thread_Change_priority(
        the_thread,
        priority,
        NULL,
        _Thread_Raise_real_priority_filter,
        false
      );
    }
  }
}

void _Thread_Close( Thread_Control *the_thread, Thread_Control *executing )
{
  _Assert( _Thread_Is_life_protected( executing->Life.state ) );

  if ( _States_Is_dormant( the_thread->current_state ) ) {
    _Thread_Make_zombie( the_thread );
  } else {
    if (
      the_thread != executing
        && !_Thread_Is_life_terminating( executing->Life.state )
    ) {
      /*
       * Wait for termination of victim thread.  If the executing thread is
       * also terminated, then do not wait.  This avoids potential cyclic
       * dependencies and thus dead lock.
       */
       the_thread->Life.terminator = executing;
       _Thread_Set_state( executing, STATES_WAITING_FOR_TERMINATION );
    }

    _Thread_Request_life_change(
      the_thread,
      executing,
      executing->current_priority,
      THREAD_LIFE_TERMINATING
    );
  }
}

bool _Thread_Restart(
  Thread_Control            *the_thread,
  Thread_Control            *executing,
  void                      *pointer_argument,
  Thread_Entry_numeric_type  numeric_argument
)
{
  if ( !_States_Is_dormant( the_thread->current_state ) ) {
    the_thread->Start.pointer_argument = pointer_argument;
    the_thread->Start.numeric_argument = numeric_argument;

    _Thread_Request_life_change(
      the_thread,
      executing,
      the_thread->Start.initial_priority,
      THREAD_LIFE_RESTARTING
    );

    return true;
  }

  return false;
}

bool _Thread_Set_life_protection( bool protect )
{
  bool previous_life_protection;
  ISR_Level level;
  Per_CPU_Control *cpu;
  Thread_Control *executing;
  Thread_Life_state previous_life_state;

  cpu = _Thread_Action_ISR_disable_and_acquire_for_executing( &level );
  executing = cpu->executing;

  previous_life_state = executing->Life.state;
  previous_life_protection = _Thread_Is_life_protected( previous_life_state );

  if ( protect ) {
    executing->Life.state = previous_life_state | THREAD_LIFE_PROTECTED;
  } else {
    executing->Life.state = previous_life_state & ~THREAD_LIFE_PROTECTED;
  }

  _Thread_Action_release_and_ISR_enable( cpu, level );

#if defined(RTEMS_SMP)
  /*
   * On SMP configurations it is possible that a life change of an executing
   * thread is requested, but this thread didn't notice it yet.  The life
   * change is first marked in the life state field and then all scheduling and
   * other thread state updates are performed.  The last step is to issues an
   * inter-processor interrupt if necessary.  Since this takes some time we
   * have to synchronize here.
   */
  if (
    !_Thread_Is_life_protected( previous_life_state )
      && _Thread_Is_life_changing( previous_life_state )
  ) {
    _Thread_Disable_dispatch();
    _Thread_Enable_dispatch();
  }
#endif

  if (
    !protect
      && _Thread_Is_life_changing( previous_life_state )
  ) {
    _Thread_Disable_dispatch();
    _Thread_Start_life_change_for_executing( executing );
    _Thread_Enable_dispatch();
  }

  return previous_life_protection;
}
