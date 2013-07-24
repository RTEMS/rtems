/**
 *  @file
 *
 *  @brief Set Thread Priority
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
#include <rtems/score/schedulerimpl.h>

void _Thread_Set_priority(
  Thread_Control   *the_thread,
  Priority_Control  new_priority
)
{
  the_thread->current_priority = new_priority;

  _Scheduler_Update( the_thread );
}
