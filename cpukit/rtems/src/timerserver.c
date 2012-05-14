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
 *  Copyright (c) 2009 embedded brains GmbH.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
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

static Timer_server_Control _Timer_server_Default;

static void _Timer_server_Stop_interval_system_watchdog(
  Timer_server_Control *ts
)
{
  _Watchdog_Remove( &ts->Interval_watchdogs.System_watchdog );
}

static void _Timer_server_Reset_interval_system_watchdog(
  Timer_server_Control *ts
)
{
  ISR_Level level;

  _Timer_server_Stop_interval_system_watchdog( ts );

  _ISR_Disable( level );
  if ( !_Chain_Is_empty( &ts->Interval_watchdogs.Chain ) ) {
    Watchdog_Interval delta_interval =
      _Watchdog_First( &ts->Interval_watchdogs.Chain )->delta_interval;
    _ISR_Enable( level );

    /*
     *  The unit is TICKS here.
     */
    _Watchdog_Insert_ticks(
      &ts->Interval_watchdogs.System_watchdog,
      delta_interval
    );
  } else {
    _ISR_Enable( level );
  }
}

static void _Timer_server_Stop_tod_system_watchdog(
  Timer_server_Control *ts
)
{
  _Watchdog_Remove( &ts->TOD_watchdogs.System_watchdog );
}

static void _Timer_server_Reset_tod_system_watchdog(
  Timer_server_Control *ts
)
{
  ISR_Level level;

  _Timer_server_Stop_tod_system_watchdog( ts );

  _ISR_Disable( level );
  if ( !_Chain_Is_empty( &ts->TOD_watchdogs.Chain ) ) {
    Watchdog_Interval delta_interval =
      _Watchdog_First( &ts->TOD_watchdogs.Chain )->delta_interval;
    _ISR_Enable( level );

    /*
     *  The unit is SECONDS here.
     */
    _Watchdog_Insert_seconds(
      &ts->TOD_watchdogs.System_watchdog,
      delta_interval
    );
  } else {
    _ISR_Enable( level );
  }
}

static void _Timer_server_Insert_timer(
  Timer_server_Control *ts,
  Timer_Control *timer
)
{
  if ( timer->the_class == TIMER_INTERVAL_ON_TASK ) {
    _Watchdog_Insert( &ts->Interval_watchdogs.Chain, &timer->Ticker );
  } else if ( timer->the_class == TIMER_TIME_OF_DAY_ON_TASK ) {
    _Watchdog_Insert( &ts->TOD_watchdogs.Chain, &timer->Ticker );
  }
}

static void _Timer_server_Insert_timer_and_make_snapshot(
  Timer_server_Control *ts,
  Timer_Control *timer
)
{
  Watchdog_Control *first_watchdog;
  Watchdog_Interval delta_interval;
  Watchdog_Interval last_snapshot;
  Watchdog_Interval snapshot;
  Watchdog_Interval delta;
  ISR_Level level;

  /*
   *  We have to update the time snapshots here, because otherwise we may have
   *  problems with the integer range of the delta values.  The time delta DT
   *  from the last snapshot to now may be arbitrarily long.  The last snapshot
   *  is the reference point for the delta chain.  Thus if we do not update the
   *  reference point we have to add DT to the initial delta of the watchdog
   *  being inserted.  This could result in an integer overflow.
   */

  _Thread_Disable_dispatch();

  if ( timer->the_class == TIMER_INTERVAL_ON_TASK ) {
    /*
     *  We have to advance the last known ticks value of the server and update
     *  the watchdog chain accordingly.
     */
    _ISR_Disable( level );
    snapshot = _Watchdog_Ticks_since_boot;
    last_snapshot = ts->Interval_watchdogs.last_snapshot;
    if ( !_Chain_Is_empty( &ts->Interval_watchdogs.Chain ) ) {
      first_watchdog = _Watchdog_First( &ts->Interval_watchdogs.Chain );

      /*
       *  We assume adequate unsigned arithmetic here.
       */
      delta = snapshot - last_snapshot;

      delta_interval = first_watchdog->delta_interval;
      if (delta_interval > delta) {
        delta_interval -= delta;
      } else {
        delta_interval = 0;
      }
      first_watchdog->delta_interval = delta_interval;
    }
    ts->Interval_watchdogs.last_snapshot = snapshot;
    _ISR_Enable( level );

    _Watchdog_Insert( &ts->Interval_watchdogs.Chain, &timer->Ticker );

    if ( !ts->active ) {
      _Timer_server_Reset_interval_system_watchdog( ts );
    }
  } else if ( timer->the_class == TIMER_TIME_OF_DAY_ON_TASK ) {
    /*
     *  We have to advance the last known seconds value of the server and update
     *  the watchdog chain accordingly.
     */
    _ISR_Disable( level );
    snapshot = (Watchdog_Interval) _TOD_Seconds_since_epoch();
    last_snapshot = ts->TOD_watchdogs.last_snapshot;
    if ( !_Chain_Is_empty( &ts->TOD_watchdogs.Chain ) ) {
      first_watchdog = _Watchdog_First( &ts->TOD_watchdogs.Chain );
      delta_interval = first_watchdog->delta_interval;
      if ( snapshot > last_snapshot ) {
        /*
         *  We advanced in time.
         */
        delta = snapshot - last_snapshot;
        if (delta_interval > delta) {
          delta_interval -= delta;
        } else {
          delta_interval = 0;
        }
      } else {
        /*
         *  Someone put us in the past.
         */
        delta = last_snapshot - snapshot;
        delta_interval += delta;
      }
      first_watchdog->delta_interval = delta_interval;
    }
    ts->TOD_watchdogs.last_snapshot = snapshot;
    _ISR_Enable( level );

    _Watchdog_Insert( &ts->TOD_watchdogs.Chain, &timer->Ticker );

    if ( !ts->active ) {
      _Timer_server_Reset_tod_system_watchdog( ts );
    }
  }

  _Thread_Enable_dispatch();
}

static void _Timer_server_Schedule_operation_method(
  Timer_server_Control *ts,
  Timer_Control *timer
)
{
  if ( ts->insert_chain == NULL ) {
    _Timer_server_Insert_timer_and_make_snapshot( ts, timer );
  } else {
    /*
     *  We interrupted a critical section of the timer server.  The timer
     *  server is not preemptible, so we must be in interrupt context here.  No
     *  thread dispatch will happen until the timer server finishes its
     *  critical section.  We have to use the protected chain methods because
     *  we may be interrupted by a higher priority interrupt.
     */
    _Chain_Append( ts->insert_chain, &timer->Object.Node );
  }
}

static void _Timer_server_Process_interval_watchdogs(
  Timer_server_Watchdogs *watchdogs,
  Chain_Control *fire_chain
)
{
  Watchdog_Interval snapshot = _Watchdog_Ticks_since_boot;

  /*
   *  We assume adequate unsigned arithmetic here.
   */
  Watchdog_Interval delta = snapshot - watchdogs->last_snapshot;

  watchdogs->last_snapshot = snapshot;

  _Watchdog_Adjust_to_chain( &watchdogs->Chain, delta, fire_chain );
}

static void _Timer_server_Process_tod_watchdogs(
  Timer_server_Watchdogs *watchdogs,
  Chain_Control *fire_chain
)
{
  Watchdog_Interval snapshot = (Watchdog_Interval) _TOD_Seconds_since_epoch();
  Watchdog_Interval last_snapshot = watchdogs->last_snapshot;
  Watchdog_Interval delta;

  /*
   *  Process the seconds chain.  Start by checking that the Time
   *  of Day (TOD) has not been set backwards.  If it has then
   *  we want to adjust the watchdogs->Chain to indicate this.
   */
  if ( snapshot > last_snapshot ) {
    /*
     *  This path is for normal forward movement and cases where the
     *  TOD has been set forward.
     */
    delta = snapshot - last_snapshot;
    _Watchdog_Adjust_to_chain( &watchdogs->Chain, delta, fire_chain );

  } else if ( snapshot < last_snapshot ) {
     /*
      *  The current TOD is before the last TOD which indicates that
      *  TOD has been set backwards.
      */
     delta = last_snapshot - snapshot;
     _Watchdog_Adjust( &watchdogs->Chain, WATCHDOG_BACKWARD, delta );
  }

  watchdogs->last_snapshot = snapshot;
}

static void _Timer_server_Process_insertions( Timer_server_Control *ts )
{
  while ( true ) {
    Timer_Control *timer = (Timer_Control *) _Chain_Get( ts->insert_chain );

    if ( timer == NULL ) {
      break;
    }

    _Timer_server_Insert_timer( ts, timer );
  }
}

static void _Timer_server_Get_watchdogs_that_fire_now(
  Timer_server_Control *ts,
  Chain_Control *insert_chain,
  Chain_Control *fire_chain
)
{
  /*
   *  Afterwards all timer inserts are directed to this chain and the interval
   *  and TOD chains will be no more modified by other parties.
   */
  ts->insert_chain = insert_chain;

  while ( true ) {
    ISR_Level level;

    /*
     *  Remove all the watchdogs that need to fire so we can invoke them.
     */
    _Timer_server_Process_interval_watchdogs(
      &ts->Interval_watchdogs,
      fire_chain
    );
    _Timer_server_Process_tod_watchdogs( &ts->TOD_watchdogs, fire_chain );

    /*
     *  The insertions have to take place here, because they reference the
     *  current time.  The previous process methods take a snapshot of the
     *  current time.  In case someone inserts a watchdog with an initial value
     *  of zero it will be processed in the next iteration of the timer server
     *  body loop.
     */
    _Timer_server_Process_insertions( ts );

    _ISR_Disable( level );
    if ( _Chain_Is_empty( insert_chain ) ) {
      ts->insert_chain = NULL;
      _ISR_Enable( level );

      break;
    } else {
      _ISR_Enable( level );
    }
  }
}

/**
 *  @brief Timer server body.
 *
 *  This is the server for task based timers.  This task executes whenever a
 *  task-based timer should fire.  It services both "after" and "when" timers.
 *  It is not created automatically but must be created explicitly by the
 *  application before task-based timers may be initiated.  The parameter
 *  @a arg points to the corresponding timer server control block.
 */
static rtems_task _Timer_server_Body(
  rtems_task_argument arg
)
{
  Timer_server_Control *ts = (Timer_server_Control *) arg;
  Chain_Control insert_chain;
  Chain_Control fire_chain;

  _Chain_Initialize_empty( &insert_chain );
  _Chain_Initialize_empty( &fire_chain );

  while ( true ) {
    _Timer_server_Get_watchdogs_that_fire_now( ts, &insert_chain, &fire_chain );

    if ( !_Chain_Is_empty( &fire_chain ) ) {
      /*
       *  Fire the watchdogs.
       */
      while ( true ) {
        Watchdog_Control *watchdog;
        ISR_Level level;

        /*
         *  It is essential that interrupts are disable here since an interrupt
         *  service routine may remove a watchdog from the chain.
         */
        _ISR_Disable( level );
        watchdog = (Watchdog_Control *) _Chain_Get_unprotected( &fire_chain );
        if ( watchdog != NULL ) {
          watchdog->state = WATCHDOG_INACTIVE;
          _ISR_Enable( level );
        } else {
          _ISR_Enable( level );

          break;
        }

        /*
         *  The timer server may block here and wait for resources or time.
         *  The system watchdogs are inactive and will remain inactive since
         *  the active flag of the timer server is true.
         */
        (*watchdog->routine)( watchdog->id, watchdog->user_data );
      }
    } else {
      ts->active = false;

      /*
       *  Block until there is something to do.
       */
      _Thread_Disable_dispatch();
        _Thread_Set_state( ts->thread, STATES_DELAYING );
        _Timer_server_Reset_interval_system_watchdog( ts );
        _Timer_server_Reset_tod_system_watchdog( ts );
      _Thread_Enable_dispatch();

      ts->active = true;

      /*
       *  Maybe an interrupt did reset the system timers, so we have to stop
       *  them here.  Since we are active now, there will be no more resets
       *  until we are inactive again.
       */
      _Timer_server_Stop_interval_system_watchdog( ts );
      _Timer_server_Stop_tod_system_watchdog( ts );
    }
  }
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
  rtems_id              id;
  rtems_status_code     status;
  rtems_task_priority   _priority;
  static bool           initialized = false;
  bool                  tmpInitialized;
  Timer_server_Control *ts = &_Timer_server_Default;

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
   */
  ts->thread = (Thread_Control *)_Objects_Get_local_object(
    &_RTEMS_tasks_Information,
    _Objects_Get_index(id)
  );

  /*
   *  Initialize the timer lists that the server will manage.
   */
  _Chain_Initialize_empty( &ts->Interval_watchdogs.Chain );
  _Chain_Initialize_empty( &ts->TOD_watchdogs.Chain );

  /*
   *  Initialize the timers that will be used to control when the
   *  Timer Server wakes up and services the task-based timers.
   */
  _Watchdog_Initialize(
    &ts->Interval_watchdogs.System_watchdog,
    _Thread_Delay_ended,
    id,
    NULL
  );
  _Watchdog_Initialize(
    &ts->TOD_watchdogs.System_watchdog,
    _Thread_Delay_ended,
    id,
    NULL
  );

  /*
   *  Initialize the pointer to the timer schedule method so applications that
   *  do not use the Timer Server do not have to pull it in.
   */
  ts->schedule_operation = _Timer_server_Schedule_operation_method;

  ts->Interval_watchdogs.last_snapshot = _Watchdog_Ticks_since_boot;
  ts->TOD_watchdogs.last_snapshot = (Watchdog_Interval) _TOD_Seconds_since_epoch();

  ts->insert_chain = NULL;
  ts->active = false;

  /*
   * The default timer server is now available.
   */
  _Timer_server = ts;

  /*
   *  Start the timer server
   */
  status = rtems_task_start(
    id,
    _Timer_server_Body,
    (rtems_task_argument) ts
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
