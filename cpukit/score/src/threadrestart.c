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
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/threadimpl.h>
#include <rtems/score/threadqimpl.h>
#include <rtems/score/userextimpl.h>
#include <rtems/score/watchdogimpl.h>

void _Thread_Life_action_handler(
  Thread_Control  *executing,
  Thread_Action   *action,
  Per_CPU_Control *cpu,
  ISR_Level        level
)
{
  (void) action;
  _Thread_Action_release_and_ISR_enable( cpu, level );

  _User_extensions_Thread_restart( the_thread );

  _Thread_Disable_dispatch();

  _Thread_Load_environment( executing );
  _Thread_Restart_self( executing );
}

static void _Thread_Start_life_change(
  Thread_Control   *the_thread,
  Priority_Control  priority
)
{
  the_thread->resource_count   = 0;
  the_thread->is_preemptible   = the_thread->Start.is_preemptible;
  the_thread->budget_algorithm = the_thread->Start.budget_algorithm;
  the_thread->budget_callout   = the_thread->Start.budget_callout;
  the_thread->real_priority    = priority;

  _Thread_Set_transient( the_thread );
  _Thread_queue_Extract_with_proxy( the_thread );
  _Watchdog_Remove( &the_thread->Timer );
  _Thread_Set_priority( the_thread, priority );
  _Thread_Add_post_switch_action( the_thread, &the_thread->Life.Action );
  _Thread_Ready( the_thread );
  _Thread_Request_dispatch_if_executing( the_thread );
}

static void _Thread_Request_life_change(
  Thread_Control    *the_thread,
  Thread_Control    *executing,
  Priority_Control   priority
)
{
  _Thread_Start_life_change( the_thread, priority );
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
      the_thread->Start.initial_priority
    );

    return true;
  }

  return false;
}
