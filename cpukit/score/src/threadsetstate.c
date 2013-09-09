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
 *  http://www.rtems.com/license/LICENSE.
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

  _ISR_Disable( level );
  if ( !_States_Is_ready( the_thread->current_state ) ) {
    the_thread->current_state =
       _States_Set( state, the_thread->current_state );
    _ISR_Enable( level );
    return;
  }

  the_thread->current_state = state;

  _Scheduler_Block( the_thread );

  _ISR_Enable( level );
}
