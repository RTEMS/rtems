/*
 *  Timer Manager - rtems_timer_initiate_server directive along with
 *      the Timer Server Body and support routines
 *
 *  COPYRIGHT (c) 1989-2002.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/score/object.h>
#include <rtems/score/thread.h>
#include <rtems/rtems/timer.h>
#include <rtems/score/tod.h>
#include <rtems/score/watchdog.h>

#include <rtems/rtems/tasks.h>
#include <rtems/rtems/support.h>
#include <rtems/score/thread.h>

/*
 *  The following chains contain the list of interval timers that are
 *  executed in the context of the Timer Server.
 *
 *  NOTE: These are prototyped in rtems/timer/timer.h but since we
 *        do not actually use them until after the Timer Server is
 *        initiated, we can actually declare them here and avoid forcing
 *        them into the minimum footprint.
 */

Chain_Control _Timer_Ticks_chain;
Chain_Control _Timer_Seconds_chain;

/*
 *  The timer used to control when the Timer Server wakes up to service
 *  "when" timers.
 *
 *  NOTE: This should NOT be used outside this file.
 */

Watchdog_Control _Timer_Seconds_timer;

/*PAGE
 *
 *  _Timer_Server_body
 *
 *  This is the server for task based timers.  This task executes whenever
 *  a task-based timer should fire.  It services both "after" and "when"
 *  timers.  It is not created automatically but must be created explicitly
 *  by the application before task-based timers may be initiated.
 *
 *  Input parameters:
 *    Ignored - the task argument is ignored
 *
 *  Output parameters:  NONE
 */

Thread _Timer_Server_body(
  unsigned32 ignored
)
{
  Watchdog_Interval snapshot;
  Watchdog_Interval ticks_last_time;
  Watchdog_Interval seconds_last_time;
  Watchdog_Interval ticks;

  /*
   *  Initialize the "last time" markers to indicate the timer that
   *  the server was initiated.
   */

  ticks_last_time   = _Watchdog_Ticks_since_boot;
  seconds_last_time = _TOD_Seconds_since_epoch;

  _Thread_Disable_dispatch();
  while(1) {

    /*
     *  Block until there is something to do.
     */

      _Thread_Set_state( _Timer_Server, STATES_DELAYING );
      _Timer_Server_reset( TIMER_SERVER_RESET_TICKS );
      _Timer_Server_reset( TIMER_SERVER_RESET_SECONDS );
    _Thread_Enable_dispatch();


    /*
     *  Disable dispatching while processing the timers since we want 
     *  to mimic the environment that non-task-based TSRs execute in.
     *  This ensures that the primary difference is that _ISR_Nest_level
     *  is 0 for task-based timers and non-zero for the others.
     */

    _Thread_Disable_dispatch();

    /*
     *  Process the ticks chain
     */

    snapshot = _Watchdog_Ticks_since_boot;
    ticks = snapshot - ticks_last_time;
    ticks_last_time = snapshot;
    _Watchdog_Adjust( &_Timer_Ticks_chain, WATCHDOG_FORWARD, ticks );

    /*
     *  Process the seconds chain.  Start by checking that the Time 
     *  of Day (TOD) has not been set backwards.  If it has then
     *  we want to adjust the _Timer_Seconds_chain to indicate this.
     */

    snapshot =  _TOD_Seconds_since_epoch;
    if ( snapshot > seconds_last_time ) {
      /*
       *  This path is for normal forward movement and cases where the
       *  TOD has been set forward.
       */

      ticks = snapshot - seconds_last_time;
      _Watchdog_Adjust( &_Timer_Seconds_chain, WATCHDOG_FORWARD, ticks );

    } else if ( snapshot < seconds_last_time ) { 
       /*
        *  The current TOD is before the last TOD which indicates that
        *  TOD has been set backwards.
        */

       ticks = seconds_last_time - snapshot;
       _Watchdog_Adjust( &_Timer_Seconds_chain, WATCHDOG_BACKWARD, ticks );
    }
    seconds_last_time = snapshot;
  }
}

/*PAGE
 *
 *  rtems_timer_initiate_server
 *
 *  This directive creates and starts the server for task-based timers.
 *  It must be invoked before any task-based timers can be initiated.
 *
 *  Input parameters:
 *    stack_size       - stack size in bytes
 *    attribute_set    - thread attributes
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code       - if unsuccessful
 */


rtems_status_code rtems_timer_initiate_server(
  unsigned32           stack_size,
  rtems_attribute      attribute_set
)
{
  rtems_id          id;
  rtems_status_code status;

  /*
   *  Just to make sure the test versus create/start operation are atomic.
   */

  _Thread_Disable_dispatch();

  if ( _Timer_Server ) {
    _Thread_Enable_dispatch();
    return RTEMS_INCORRECT_STATE;
  }

  /*
   *  Create the Timer Server with the name the name of "TIME".  The priority
   *  is always "0" which makes it higher than any other task in the system.
   *  It is also always NO_PREEMPT so it looks like an interrupt to other tasks.
   */

  status = rtems_task_create(
    rtems_build_name( 'T', 'I', 'M', 'E' ),
    1,                    /* create with priority 1 since 0 is illegal */
    stack_size,           /* let user specify stack size */
    RTEMS_NO_PREEMPT,     /* no preempt is like an interrupt */
    attribute_set,        /* user may want floating point */
    &id                   /* get the id back */
  );
  if (status) {
    _Thread_Enable_dispatch();
    return status;
  }

  status = rtems_task_start(
    id,                                    /* the id from create */
    (rtems_task_entry) _Timer_Server_body, /* the timer server entry point */
    0                                      /* there is no argument */
  );
  if (status) {
    /*
     *  One would expect a call to rtems_task_delete() here to clean up
     *  but there is actually no way (in normal circumstances) that the
     *  start can fail.  The id and starting address are known to be 
     *  be good.  If this service fails, something is weirdly wrong on the
     *  target such as a stray write in an ISR or incorrect memory layout.
     */
    _Thread_Enable_dispatch();
    return status;
  }

  /*
   *  We work with the TCB pointer, not the ID, so we need to convert
   *  to a TCB pointer from here out.
   *
   *  NOTE: Setting the pointer to the Timer Server TCB to a value other than
   *        NULL indicates that task-based timer support is initialized.
   */

  _Timer_Server = (Thread_Control *)_Objects_Get_local_object(
    &_RTEMS_tasks_Information,
    _Objects_Get_index(id)
  );

  /*
   *  A priority of 0, higher than any user task, combined with no preemption
   *  makes this task the highest priority in any application.  It can be
   *  viewed as a low priority interrupt.  The FALSE argument indicates
   *  that the task is to be appended (not prepended) to its priority
   *  chain at the end of this operation.
   */

  _Thread_Change_priority( _Timer_Server, 0, FALSE );

  /*
   *  Initialize the timer lists that the server will manage.
   */

  _Chain_Initialize_empty( &_Timer_Ticks_chain );
  _Chain_Initialize_empty( &_Timer_Seconds_chain );

  /* 
   *  Initialize the timers that will be used to control when the 
   *  Timer Server wakes up and services the task-based timers.
   */

  _Watchdog_Initialize( &_Timer_Server->Timer, _Thread_Delay_ended, id, NULL );
  _Watchdog_Initialize( &_Timer_Seconds_timer, _Thread_Delay_ended, id, NULL );

  _Thread_Enable_dispatch();
  return RTEMS_SUCCESSFUL;
}

/*PAGE
 *
 *  _Timer_Server_reset
 *
 *  This routine resets the timers which determine when the Timer Server
 *  will wake up next to service task-based timers.
 *
 *  Input parameters:
 *    do_ticks - TRUE indicates to process the ticks list
 *               FALSE indicates to process the seconds list
 *
 *  Output parameters:  NONE
 */

void _Timer_Server_reset(
  Timer_Server_reset_mode reset_mode
)
{
  Watchdog_Interval  units;

  switch ( reset_mode ) {
    case TIMER_SERVER_RESET_TICKS:
      _Watchdog_Remove(  &_Timer_Server->Timer );
      units = ((Watchdog_Control *)_Timer_Ticks_chain.first)->delta_interval;
      _Watchdog_Insert_ticks( &_Timer_Server->Timer, units );
      break;
    case TIMER_SERVER_RESET_SECONDS:
      _Watchdog_Remove(  &_Timer_Seconds_timer );
      units = ((Watchdog_Control *)_Timer_Seconds_chain.first)->delta_interval;
      _Watchdog_Insert_seconds( &_Timer_Seconds_timer, units );
      break;
  }
}

