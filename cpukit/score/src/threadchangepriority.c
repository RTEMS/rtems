/**
 * @file
 *
 * @brief Changes the Priority of a Thread
 *
 * @ingroup ScoreThread
 */

/*
 *  COPYRIGHT (c) 1989-2014.
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
#include <rtems/score/schedulerimpl.h>
#include <rtems/score/threadqimpl.h>

void _Thread_Change_priority(
  Thread_Control   *the_thread,
  Priority_Control  new_priority,
  bool              prepend_it
)
{
  ISR_Level level;

  _ISR_Disable( level );

  /*
   *  Do not bother recomputing all the priority related information if
   *  we are not REALLY changing priority.
   */
  if ( the_thread->current_priority != new_priority ) {
    uint32_t my_generation = the_thread->Priority.generation + 1;

    the_thread->current_priority = new_priority;
    the_thread->Priority.generation = my_generation;

    (*the_thread->Priority.change_handler)(
      the_thread,
      new_priority,
      the_thread->Priority.change_handler_context
    );

    _ISR_Flash( level );

    if ( the_thread->Priority.generation == my_generation ) {
      if ( _States_Is_ready( the_thread->current_state ) ) {
        _Scheduler_Change_priority(
          the_thread,
          new_priority,
          prepend_it
        );
      } else {
        _Scheduler_Update_priority( the_thread, new_priority );
      }
    }
  }

  _ISR_Enable( level );
}
