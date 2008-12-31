/**
 *  @file timerserver.c
 *
 *  Timer Manager - rtems_timer_initiate_server directive along with
 *  the Timer Server Body and support routines
 *
 *  @note Data specific to the Timer Server is declared in this
 *        file as the Timer Server so it does not have to be in the
 *        minimum footprint.  It is only really required when
 *        task-based timers are used.  Since task-based timers can
 *        not be started until the server is initiated, this structure
 *        does not have to be initialized until then.
 */

/*  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

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

/**
 *  This chain contains the list of interval timers that are
 *  executed in the context of the Timer Server.
 */
Chain_Control _Timer_Ticks_chain;

/**
 *  This chain contains the list of time of day timers that are
 *  executed in the context of the Timer Server.
 */
Chain_Control _Timer_Seconds_chain;

/**
 *  This chain holds the set of timers to be inserted when the
 *  server runs again.
 */
Chain_Control _Timer_To_be_inserted;

/**
 *  This variables keeps track of the last time the Timer Server actually
 *  processed the ticks chain.
 */
Watchdog_Interval _Timer_Server_ticks_last_time;

/**
 *  This variable keeps track of the last time the Timer Server actually
 *  processed the seconds chain.
 */
Watchdog_Interval _Timer_Server_seconds_last_time;

/**
 *  This is the timer used to control when the Timer Server wakes up to
 *  service "when" timers.
 *
 *  @note The timer in the Timer Server TCB is used for ticks timer.
 */
Watchdog_Control _Timer_Seconds_timer;

/**
 *  This method is used to temporarily disable updates to the 
 *  Ticks Timer Chain managed by the Timer Server.
 */
#define _Timer_Server_stop_ticks_timer() \
      _Watchdog_Remove( &_Timer_Server->Timer )

/**
 *  This method is used to temporarily disable updates to the 
 *  Seconds Timer Chain managed by the Timer Server.
 */
#define _Timer_Server_stop_seconds_timer() \
      _Watchdog_Remove( &_Timer_Seconds_timer );

/**
 *  This method resets a timer and places it on the Ticks chain.  It
 *  is assumed that the timer has already been canceled.
 */
#define _Timer_Server_reset_ticks_timer() \
   do { \
      if ( !_Chain_Is_empty( &_Timer_Ticks_chain ) ) { \
        _Watchdog_Insert_ticks( &_Timer_Server->Timer, \
           ((Watchdog_Control *)_Timer_Ticks_chain.first)->delta_interval ); \
      } \
   } while (0)

/**
 *  This method resets a timer and places it on the Seconds chain.  It
 *  is assumed that the timer has already been canceled.
 */
#define _Timer_Server_reset_seconds_timer() \
   do { \
      if ( !_Chain_Is_empty( &_Timer_Seconds_chain ) ) { \
        _Watchdog_Insert_seconds( &_Timer_Seconds_timer, \
          ((Watchdog_Control *)_Timer_Seconds_chain.first)->delta_interval ); \
      } \
   } while (0)

/**
 *  @brief _Timer_Server_process_insertions
 *
 *  This method processes the set of timers scheduled for insertion
 *  onto one of the Timer Server chains.
 *
 *  @note It is only to be called from the Timer Server task.
 */
static void _Timer_Server_process_insertions(void)
{
  Timer_Control *the_timer;

  while ( 1 ) {
    the_timer = (Timer_Control *) _Chain_Get( &_Timer_To_be_inserted );
    if ( the_timer == NULL )
      break;

    if ( the_timer->the_class == TIMER_INTERVAL_ON_TASK ) {
      _Watchdog_Insert( &_Timer_Ticks_chain, &the_timer->Ticker );
    } else if ( the_timer->the_class == TIMER_TIME_OF_DAY_ON_TASK ) {
      _Watchdog_Insert( &_Timer_Seconds_chain, &the_timer->Ticker );
    }
  }
}

/**
 *  @brief _Timer_Server_process_ticks_chain
 *
 *  This routine is responsible for adjusting the list of task-based
 *  interval timers to reflect the passage of time.
 *
 *  @param[in] to_fire will contain the set of timers that are to be fired.
 *
 *  @note It is only to be called from the Timer Server task.
 */
static void _Timer_Server_process_ticks_chain(
  Chain_Control *to_fire
)
{
  Watchdog_Interval snapshot;
  Watchdog_Interval ticks;

  snapshot = _Watchdog_Ticks_since_boot;
  if ( snapshot >= _Timer_Server_ticks_last_time )
     ticks = snapshot - _Timer_Server_ticks_last_time;
  else
     ticks = (0xFFFFFFFF - _Timer_Server_ticks_last_time) + snapshot;

  _Timer_Server_ticks_last_time = snapshot;
  _Watchdog_Adjust_to_chain( &_Timer_Ticks_chain, ticks, to_fire );
}

/**
 *  @brief _Timer_Server_process_seconds_chain
 *
 *  This routine is responsible for adjusting the list of task-based
 *  time of day timers to reflect the passage of time.
 *
 *  @param[in] to_fire will contain the set of timers that are to be fired.
 *
 *  @note It is only to be called from the Timer Server task.
 */
static void _Timer_Server_process_seconds_chain(
  Chain_Control *to_fire
)
{
  Watchdog_Interval snapshot;
  Watchdog_Interval ticks;

  /*
   *  Process the seconds chain.  Start by checking that the Time
   *  of Day (TOD) has not been set backwards.  If it has then
   *  we want to adjust the _Timer_Seconds_chain to indicate this.
   */
  snapshot =  _TOD_Seconds_since_epoch();
  if ( snapshot > _Timer_Server_seconds_last_time ) {
    /*
     *  This path is for normal forward movement and cases where the
     *  TOD has been set forward.
     */
    ticks = snapshot - _Timer_Server_seconds_last_time;
    _Watchdog_Adjust_to_chain( &_Timer_Seconds_chain, ticks, to_fire );

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

/**
 *  @brief _Timer_Server_body
 *
 *  This is the server for task based timers.  This task executes whenever
 *  a task-based timer should fire.  It services both "after" and "when"
 *  timers.  It is not created automatically but must be created explicitly
 *  by the application before task-based timers may be initiated.
 *
 *  @param[in] ignored is the the task argument that is ignored
 */
rtems_task _Timer_Server_body(
  rtems_task_argument argument __attribute__((unused))
)
{
  Chain_Control to_fire;

  _Chain_Initialize_empty( &to_fire );

  /*
   *  Initialize the "last time" markers to indicate the timer that
   *  the server was initiated.
   */
  _Timer_Server_ticks_last_time   = _Watchdog_Ticks_since_boot;
  _Timer_Server_seconds_last_time = _TOD_Seconds_since_epoch();

  /*
   *  Insert the timers that were inserted before we got to run.
   *  This should be done with dispatching disabled.
   */
  _Thread_Disable_dispatch();
    _Timer_Server_process_insertions();
  _Thread_Enable_dispatch();

  while(1) {

    /*
     *  Block until there is something to do.
     */
    _Thread_Disable_dispatch();
      _Thread_Set_state( _Timer_Server, STATES_DELAYING );
      _Timer_Server_reset_ticks_timer();
      _Timer_Server_reset_seconds_timer();
    _Thread_Enable_dispatch();

    /********************************************************************
     ********************************************************************
     ****                TIMER SERVER BLOCKS HERE                    ****
     ********************************************************************
     ********************************************************************/

    /*
     *  Disable dispatching while processing the timers since we want
     *  the removal of the timers from the chain to be atomic.
     *
     *  NOTE: Dispatching is disabled for interrupt based TSRs.
     *        Dispatching is enabled for task based TSRs so they
     *          can temporarily malloc memory or block.
     *        _ISR_Nest_level is 0 for task-based TSRs and non-zero
     *          for the others.
     */
    _Thread_Disable_dispatch();

    /*
     *  At this point, at least one of the timers this task relies
     *  upon has fired.  Stop them both while we process any outstanding
     *  timers.  Before we block, we will restart them.
     */
    _Timer_Server_stop_ticks_timer();
    _Timer_Server_stop_seconds_timer();

    /*
     *  Remove all the timers that need to fire so we can invoke them
     *  outside the critical section.
     */
    _Timer_Server_process_ticks_chain( &to_fire );
    _Timer_Server_process_seconds_chain( &to_fire );

    /*
     *  Insert the timers that have been requested to be inserted.
     */
    _Timer_Server_process_insertions();

    /*
     * Enable dispatching to process the set that are ready "to fire."
     */
    _Thread_Enable_dispatch();

    /*
     *  Now we actually invoke the TSR for all the timers that fired.
     *  This is done with dispatching
     */
    while (1) {
      Watchdog_Control *watch;
      ISR_Level         level;

      _ISR_Disable( level );
      watch = (Watchdog_Control *) _Chain_Get_unprotected( &to_fire );
      if ( watch == NULL ) {
        _ISR_Enable( level );
        break;
      }

      watch->state = WATCHDOG_INACTIVE;
      _ISR_Enable( level );

      (*watch->routine)( watch->id, watch->user_data );
    }
  }
}

/**
 *  This method schedules the insertion of timers on the proper list.  It
 *  wakes up the Timer Server task to process the insertion.
 *
 *  @param[in] the_timer is the timer to insert
 *
 *  @note It is highly likely the executing task will be preempted after
 *        the directive invoking this is executed.
 */
static void _Timer_Server_schedule_operation_method(
  Timer_Control     *the_timer
)
{
  _Chain_Append( &_Timer_To_be_inserted, &the_timer->Object.Node );
  _Watchdog_Remove( &_Timer_Server->Timer );
  _Thread_Delay_ended( _Timer_Server->Object.id, NULL );
}

/**
 *  @brief rtems_timer_initiate_server
 *
 *  This directive creates and starts the server for task-based timers.
 *  It must be invoked before any task-based timers can be initiated.
 *
 *  @param[in] priority is the timer server priority
 *  @param[in] stack_size is the stack size in bytes
 *  @param[in] attribute_set is the timer server attributes
 *
 *  @return This method returns RTEMS_SUCCESSFUL if successful and an 
 *          error code otherwise.
 */
rtems_status_code rtems_timer_initiate_server(
  uint32_t             priority,
  uint32_t             stack_size,
  rtems_attribute      attribute_set
)
{
  rtems_id            id;
  rtems_status_code   status;
  rtems_task_priority _priority;
  static bool         initialized = false;
  bool                tmpInitialized;

  /*
   *  Make sure the requested priority is valid.  The if is
   *  structured so we check it is invalid before looking for
   *  a specific invalid value as the default.
   */
  _priority = priority;
  if ( !_RTEMS_tasks_Priority_is_valid( priority ) ) {
    if ( priority != RTEMS_TIMER_SERVER_DEFAULT_PRIORITY )
      return RTEMS_INVALID_PRIORITY;
    _priority = 0;
  }

  /*
   *  Just to make sure this is only called once.
   */
  _Thread_Disable_dispatch();
    tmpInitialized  = initialized;
    initialized = true;
  _Thread_Enable_dispatch();

  if ( tmpInitialized )
    return RTEMS_INCORRECT_STATE;

  /*
   *  Initialize the set of timers to be inserted by the server.
   */
  _Chain_Initialize_empty( &_Timer_To_be_inserted );

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
    _Objects_Build_name('T','I','M','E'),           /* "TIME" */
    _priority,            /* create with priority 1 since 0 is illegal */
    stack_size,           /* let user specify stack size */
    RTEMS_NO_PREEMPT,     /* no preempt is like an interrupt */
                          /* user may want floating point but we need */
                          /*   system task specified for 0 priority */
    attribute_set | RTEMS_SYSTEM_TASK,
    &id                   /* get the id back */
  );
  if (status) {
    initialized = false;
    return status;
  }

  /*
   *  Do all the data structure initialization before starting the
   *  Timer Server so we do not have to have a critical section.
   */

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

  /*
   *  Initialize the pointer to the timer reset method so applications
   *  that do not use the Timer Server do not have to pull it in.
   */
  _Timer_Server_schedule_operation = _Timer_Server_schedule_operation_method;

  /*
   *  Start the timer server
   */
  status = rtems_task_start(
    id,                                    /* the id from create */
    _Timer_Server_body,                    /* the timer server entry point */
    0                                      /* there is no argument */
  );
  
  #if defined(RTEMS_DEBUG)
    /*
     *  One would expect a call to rtems_task_delete() here to clean up
     *  but there is actually no way (in normal circumstances) that the
     *  start can fail.  The id and starting address are known to be
     *  be good.  If this service fails, something is weirdly wrong on the
     *  target such as a stray write in an ISR or incorrect memory layout.
     */
    if (status) {
      initialized = false;
    }
  #endif

  return status;
}
