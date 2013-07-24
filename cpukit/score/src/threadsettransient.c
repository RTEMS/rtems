/**
 * @file
 *
 * @brief Sets the Transient State for a Thread
 *
 * @ingroup ScoreThread
 */

/*
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

void _Thread_Set_transient(
  Thread_Control *the_thread
)
{
  ISR_Level             level;
  uint32_t              old_state;

  _ISR_Disable( level );

  old_state = the_thread->current_state;
  the_thread->current_state = _States_Set( STATES_TRANSIENT, old_state );

  if ( _States_Is_ready( old_state ) ) {
    _Scheduler_Extract( the_thread );
  }

  _ISR_Enable( level );

}
