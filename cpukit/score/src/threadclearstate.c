/**
 *  @file
 *
 *  @brief Clear Thread state
 *  @ingroup RTEMSScoreThread
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/threadimpl.h>
#include <rtems/score/assert.h>
#include <rtems/score/schedulerimpl.h>

States_Control _Thread_Clear_state_locked(
  Thread_Control *the_thread,
  States_Control  state
)
{
  States_Control previous_state;

  _Assert( state != 0 );
  _Assert( _Thread_State_is_owner( the_thread ) );

  previous_state = the_thread->current_state;

  if ( ( previous_state & state ) != 0 ) {
    States_Control next_state;

    next_state = _States_Clear( state, previous_state );
    the_thread->current_state = next_state;

    if ( _States_Is_ready( next_state ) ) {
      _Scheduler_Unblock( the_thread );
    }
  }

  return previous_state;
}

States_Control _Thread_Clear_state(
  Thread_Control *the_thread,
  States_Control  state
)
{
  ISR_lock_Context lock_context;
  States_Control   previous_state;

  _Thread_State_acquire( the_thread, &lock_context );
  previous_state = _Thread_Clear_state_locked( the_thread, state );
  _Thread_State_release( the_thread, &lock_context );

  return previous_state;
}
