/**
 * @file
 *
 * @brief Sets States for a Thread
 *
 * @ingroup ScoreThread
 */

/*
 *  Thread Handler / Thread Set State
 *
 *  COPYRIGHT (c) 1989-2011.
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
#include <rtems/score/isrlevel.h>
#include <rtems/score/schedulerimpl.h>

void _Thread_Set_state(
  Thread_Control *the_thread,
  States_Control  state
)
{
  ISR_Level      level;
  States_Control current_state;

  _ISR_Disable( level );

  current_state = the_thread->current_state;
  if ( _States_Is_ready( current_state ) ) {
    the_thread->current_state = state;

    _Scheduler_Block( the_thread );
  } else {
    the_thread->current_state = _States_Set( state, current_state);
  }

  _ISR_Enable( level );
}
