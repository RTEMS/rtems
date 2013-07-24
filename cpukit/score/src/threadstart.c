/**
 * @file
 *
 * @brief Initializes Thread and Executes it
 *
 * @ingroup ScoreThread
 */

/*
 *  COPYRIGHT (c) 1989-1999.
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
#include <rtems/score/userextimpl.h>

bool _Thread_Start(
  Thread_Control            *the_thread,
  Thread_Start_types         the_prototype,
  void                      *entry_point,
  void                      *pointer_argument,
  Thread_Entry_numeric_type  numeric_argument,
  Per_CPU_Control           *processor
)
{
  if ( _States_Is_dormant( the_thread->current_state ) ) {

    the_thread->Start.entry_point      = (Thread_Entry) entry_point;

    the_thread->Start.prototype        = the_prototype;
    the_thread->Start.pointer_argument = pointer_argument;
    the_thread->Start.numeric_argument = numeric_argument;

    _Thread_Load_environment( the_thread );

    if ( processor == NULL ) {
      _Thread_Ready( the_thread );
    } else {
      the_thread->current_state = STATES_READY;
      _Scheduler_Start_idle( the_thread, processor );
    }

    _User_extensions_Thread_start( the_thread );

    return true;
  }

  return false;
}
