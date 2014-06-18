/**
 * @file
 * 
 * @brief End the Delay of a Thread
 * @ingroup ScoreThread
 */

/*
 *  COPYRIGHT (c) 1989-2007.
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

void _Thread_Delay_ended(
  Objects_Id  id,
  void       *arg
)
{
  Thread_Control *the_thread = arg;

  (void) id;

  _Thread_Clear_state(
    the_thread,
    STATES_DELAYING
      | STATES_WAITING_FOR_TIME
      | STATES_INTERRUPTIBLE_BY_SIGNAL
  );
}
