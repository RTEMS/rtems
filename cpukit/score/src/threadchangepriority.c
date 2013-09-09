/**
 * @file
 *
 * @brief Changes the Priority of a Thread
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
#include <rtems/score/schedulerimpl.h>
#include <rtems/score/threadqimpl.h>

void _Thread_Change_priority(
  Thread_Control   *the_thread,
  Priority_Control  new_priority,
  bool              prepend_it
)
{
  ISR_Level      level;
  States_Control state, original_state;

  /*
   * Save original state
   */
  original_state = the_thread->current_state;

  /*
   * Set a transient state for the thread so it is pulled off the Ready chains.
   * This will prevent it from being scheduled no matter what happens in an
   * ISR.
   */
  _Thread_Set_transient( the_thread );

  /*
   *  Do not bother recomputing all the priority related information if
   *  we are not REALLY changing priority.
   */
 if ( the_thread->current_priority != new_priority )
    _Thread_Set_priority( the_thread, new_priority );

  _ISR_Disable( level );

  /*
   *  If the thread has more than STATES_TRANSIENT set, then it is blocked,
   *  If it is blocked on a thread queue, then we need to requeue it.
   */
  state = the_thread->current_state;
  if ( state != STATES_TRANSIENT ) {
    /* Only clear the transient state if it wasn't set already */
    if ( ! _States_Is_transient( original_state ) )
      the_thread->current_state = _States_Clear( STATES_TRANSIENT, state );

    /*
     * The thread may have new blocking states added by interrupt service
     * routines after the change into the transient state.  This will not
     * result in a _Scheduler_Block() operation.  Make sure we select an heir
     * now.
     */
    _Scheduler_Schedule( the_thread );

    _ISR_Enable( level );
    if ( _States_Is_waiting_on_thread_queue( state ) ) {
      _Thread_queue_Requeue( the_thread->Wait.queue, the_thread );
    }
    return;
  }

  /* Only clear the transient state if it wasn't set already */
  if ( ! _States_Is_transient( original_state ) ) {
    /*
     *  Interrupts are STILL disabled.
     *  We now know the thread will be in the READY state when we remove
     *  the TRANSIENT state.  So we have to place it on the appropriate
     *  Ready Queue with interrupts off.
     */
    the_thread->current_state = _States_Clear( STATES_TRANSIENT, state );

    if ( prepend_it )
      _Scheduler_Enqueue_first( the_thread );
    else
      _Scheduler_Enqueue( the_thread );
  }

  _ISR_Flash( level );

  /*
   *  We altered the set of thread priorities.  So let's figure out
   *  who is the heir and if we need to switch to them.
   */
  _Scheduler_Schedule( the_thread );

  _ISR_Enable( level );
}
