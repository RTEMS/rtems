/**
 *  @file
 *
 *  @brief Thread Ready
 *  @ingroup ScoreThread
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

void _Thread_Ready(
  Thread_Control *the_thread
)
{
  ISR_Level              level;

  _ISR_Disable( level );

  the_thread->current_state = STATES_READY;

  _Scheduler_Unblock( the_thread );

  _ISR_Enable( level );
}
