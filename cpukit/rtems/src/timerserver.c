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
 *  These variables keep track of the last time the Timer Server actually
 *  processed the chain.
 */

Watchdog_Interval _Timer_Server_seconds_last_time;
Watchdog_Interval _Timer_Server_ticks_last_time;

/*
 *  The timer used to control when the Timer Server wakes up to service
 *  "when" timers.
 *
 *  NOTE: This should NOT be used outside this file.
 */

Watchdog_Control _Timer_Seconds_timer;

/*
 *  prototypes for support routines to process the chains
 */

void _Timer_Process_ticks_chain(void);
void _Timer_Process_seconds_chain(void);

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
  /*
   *  Initialize the "last time" markers to indicate the timer that
   *  the server was initiated.
   */

  _Timer_Server_ticks_last_time   = _Watchdog_Ticks_since_boot;
  _Timer_Server_seconds_last_time = _TOD_Seconds_since_epoch;

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
      _Timer_Process_ticks_chain();
      _Timer_Process_seconds_chain();
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
 *    priority         - timer server priority 
 *    stack_size       - stack size in bytes
 *    attribute_set    - timer server attributes
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code       - if unsuccessful
 */


rtems_status_code rtems_timer_initiate_server(
  unsigned32           priority,
  unsigned32           stack_size,
  rtems_attribute      attribute_set
)
{
  rtems_id            id;
  rtems_status_code   status;
  rtems_task_priority _priority;

  /*
   *  Make sure the requested priority is valid.
   */

  _priority = priority;
  if ( priority == RTEMS_TIMER_SERVER_DEFAULT_PRIORITY ) 
    _priority = 0;
  else if ( !_RTEMS_tasks_Priority_is_valid( priority ) )
    return RTEMS_INVALID_PRIORITY;

  /*
   *  Just to make sure the test versus create/start operation are atomic.
   */

  _Thread_Disable_dispatch();

  if ( _Timer_Server ) {
    _Thread_Enable_dispatch();
    return RTEMS_INCORRECT_STATE;
  }

  /*
   *  Create the Timer Server with the name the name of "TIME".  The attribute
   *  RTEMS_SYSTEM_TASK allows us to set a priority to 0 which will makes it
   *  higher than any other task in the system.  It can be viewed as a low
   *  priority interrupt.  It is also always NO_PREEMPT so it looks like
   *  an interrupt to other tasks.
   *
   *  We allow the user to override the default priority because the Timer
   *  Server can invoke TSRs which must adhere to language run-time or 
   *  other library rules.  For example, if using a TSR written in Ada the
   *  Server should run at the same priority as the priority Ada task.
   *  Otherwise, the priority ceiling for the mutex used to protect the
   *  GNAT run-time is violated.
   */

  status = rtems_task_create(
    0x4954454d,           /* "TIME" */
    _priority,            /* create with priority 1 since 0 is illegal */
    stack_size,           /* let user specify stack size */
    RTEMS_NO_PREEMPT,     /* no preempt is like an interrupt */
                          /* user may want floating point but we need */
                          /*   system task specified for 0 priority */
    attribute_set | RTEMS_SYSTEM_TASK,
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
      _Watchdog_Remove( &_Timer_Server->Timer );
      _Timer_Process_ticks_chain();
      if ( !_Chain_Is_empty( &_Timer_Ticks_chain ) ) {
        units = ((Watchdog_Control *)_Timer_Ticks_chain.first)->delta_interval;
        _Watchdog_Insert_ticks( &_Timer_Server->Timer, units );
      }
      break;
    case TIMER_SERVER_RESET_SECONDS:
      _Watchdog_Remove(  &_Timer_Seconds_timer );
      _Timer_Process_seconds_chain();
      if ( !_Chain_Is_empty( &_Timer_Seconds_chain ) ) {
        units = ((Watchdog_Control *)_Timer_Seconds_chain.first)->delta_interval;
        _Watchdog_Insert_seconds( &_Timer_Seconds_timer, units );
      }
      break;
  }
}

/*PAGE
 *
 *  _Timer_Server_Process_ticks_chain
 *
 *  This routine is responsible for adjusting the list of task-based 
 *  interval timers to reflect the passage of time.
 *
 *  Input parameters:   NONE
 *
 *  Output parameters:  NONE
 */

void _Timer_Process_ticks_chain(void)
{
  Watchdog_Interval snapshot;
  Watchdog_Interval ticks;

  snapshot = _Watchdog_Ticks_since_boot;
  if ( snapshot >= _Timer_Server_ticks_last_time )
     ticks = snapshot - _Timer_Server_ticks_last_time;
  else
     ticks = (0xFFFFFFFF - _Timer_Server_ticks_last_time) + snapshot;
 
  _Timer_Server_ticks_last_time = snapshot;
  _Watchdog_Adjust( &_Timer_Ticks_chain, WATCHDOG_FORWARD, ticks );
}

/*PAGE
 *
 *  _Timer_Server_Process_seconds_chain
 *
 *  This routine is responsible for adjusting the list of task-based 
 *  time of day timers to reflect the passage of time.
 *
 *  Input parameters:   NONE
 *
 *  Output parameters:  NONE
 */

void _Timer_Process_seconds_chain(void)
{
  Watchdog_Interval snapshot;
  Watchdog_Interval ticks;

  /*
   *  Process the seconds chain.  Start by checking that the Time 
   *  of Day (TOD) has not been set backwards.  If it has then
   *  we want to adjust the _Timer_Seconds_chain to indicate this.
   */

  snapshot =  _TOD_Seconds_since_epoch;
  if ( snapshot > _Timer_Server_seconds_last_time ) {
    /*
     *  This path is for normal forward movement and cases where the
     *  TOD has been set forward.
     */

    ticks = snapshot - _Timer_Server_seconds_last_time;
    _Watchdog_Adjust( &_Timer_Seconds_chain, WATCHDOG_FORWARD, ticks );

  } else if ( snapshot < _Timer_Server_seconds_last_time ) { 
     /*
      *  The current TOD is before the last TOD which indicates that
      *  TOD has been set backwards.
      */

     ticks = _Timer_Server_seconds_last_time - snapshot;
     _Watchdog_Adjust( &_Timer_Seconds_chain, WATCHDOG_BACKWARD, ticks );
  }
  _Timer_Server_seconds_last_time = snapshot;
}

