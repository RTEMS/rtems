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
 *  Copyright (c) 2009-2015 embedded brains GmbH.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/rtems/timerimpl.h>
#include <rtems/rtems/tasksimpl.h>
#include <rtems/score/apimutex.h>
#include <rtems/score/todimpl.h>

static Timer_server_Control _Timer_server_Default;

static void _Timer_server_Cancel_method(
  Timer_server_Control *ts,
  Timer_Control *timer
)
{
  if ( timer->the_class == TIMER_INTERVAL_ON_TASK ) {
    _Watchdog_Remove( &ts->Interval_watchdogs.Header, &timer->Ticker );
  } else if ( timer->the_class == TIMER_TIME_OF_DAY_ON_TASK ) {
    _Watchdog_Remove( &ts->TOD_watchdogs.Header, &timer->Ticker );
  }
}

static Watchdog_Interval _Timer_server_Get_ticks( void )
{
  return _Watchdog_Ticks_since_boot;
}

static Watchdog_Interval _Timer_server_Get_seconds( void )
{
  return _TOD_Seconds_since_epoch();
}

static void _Timer_server_Update_system_watchdog(
  Timer_server_Watchdogs *watchdogs,
  Watchdog_Header *system_header
)
{
  ISR_lock_Context lock_context;

  _Watchdog_Acquire( &watchdogs->Header, &lock_context );

  if ( watchdogs->system_watchdog_helper == NULL ) {
    Thread_Control *executing;
    uint32_t my_generation;

    executing = _Thread_Executing;
    watchdogs->system_watchdog_helper = executing;

    do {
      my_generation = watchdogs->generation;

      if ( !_Watchdog_Is_empty( &watchdogs->Header ) ) {
        Watchdog_Control *first;
        Watchdog_Interval delta;

        first = _Watchdog_First( &watchdogs->Header );
        delta = first->delta_interval;

        if (
          watchdogs->System_watchdog.state == WATCHDOG_INACTIVE
            || delta != watchdogs->system_watchdog_delta
        ) {
          watchdogs->system_watchdog_delta = delta;
          _Watchdog_Release( &watchdogs->Header, &lock_context );

          _Watchdog_Remove( system_header, &watchdogs->System_watchdog );
          watchdogs->System_watchdog.initial = delta;
          _Watchdog_Insert( system_header, &watchdogs->System_watchdog );

          _Watchdog_Acquire( &watchdogs->Header, &lock_context );
        }
      }
    } while ( watchdogs->generation != my_generation );

    watchdogs->system_watchdog_helper = NULL;
  }

  _Watchdog_Release( &watchdogs->Header, &lock_context );
}

static void _Timer_server_Insert_timer(
  Timer_server_Watchdogs *watchdogs,
  Timer_Control *timer,
  Watchdog_Header *system_header,
  Watchdog_Interval (*get_ticks)( void )
)
{
  ISR_lock_Context lock_context;
  Watchdog_Interval now;
  Watchdog_Interval delta;

  _Watchdog_Acquire( &watchdogs->Header, &lock_context );

  now = (*get_ticks)();
  delta = now - watchdogs->last_snapshot;
  watchdogs->last_snapshot = now;
  watchdogs->current_snapshot = now;

  if ( watchdogs->system_watchdog_delta > delta ) {
    watchdogs->system_watchdog_delta -= delta;
  } else {
    watchdogs->system_watchdog_delta = 0;
  }

  if ( !_Watchdog_Is_empty( &watchdogs->Header ) ) {
    Watchdog_Control *first = _Watchdog_First( &watchdogs->Header );

    if ( first->delta_interval > delta ) {
      first->delta_interval -= delta;
    } else {
      first->delta_interval = 0;
    }
  }

  _Watchdog_Insert_locked(
    &watchdogs->Header,
    &timer->Ticker,
    &lock_context
  );

  ++watchdogs->generation;

  _Watchdog_Release( &watchdogs->Header, &lock_context );

  _Timer_server_Update_system_watchdog( watchdogs, system_header );
}

static void _Timer_server_Schedule_operation_method(
  Timer_server_Control *ts,
  Timer_Control *timer
)
{
  if ( timer->the_class == TIMER_INTERVAL_ON_TASK ) {
    _Timer_server_Insert_timer(
      &ts->Interval_watchdogs,
      timer,
      &_Watchdog_Ticks_header,
      _Timer_server_Get_ticks
    );
  } else if ( timer->the_class == TIMER_TIME_OF_DAY_ON_TASK ) {
    _Timer_server_Insert_timer(
      &ts->TOD_watchdogs,
      timer,
      &_Watchdog_Seconds_header,
      _Timer_server_Get_seconds
    );
  }
}

static void _Timer_server_Update_current_snapshot(
  Timer_server_Watchdogs *watchdogs,
  Watchdog_Interval (*get_ticks)( void )
)
{
  ISR_lock_Context lock_context;

  _Watchdog_Acquire( &watchdogs->Header, &lock_context );
  watchdogs->current_snapshot = (*get_ticks)();
  watchdogs->system_watchdog_delta = 0;
  _Watchdog_Release( &watchdogs->Header, &lock_context );
}

static void _Timer_server_Tickle(
  Timer_server_Watchdogs *watchdogs,
  Watchdog_Header *system_header,
  Watchdog_Interval (*get_ticks)( void ),
  bool ticks
)
{
  ISR_lock_Context lock_context;
  Watchdog_Interval now;
  Watchdog_Interval last;

  _Watchdog_Acquire( &watchdogs->Header, &lock_context );

  now = watchdogs->current_snapshot;
  last = watchdogs->last_snapshot;
  watchdogs->last_snapshot = now;

  if ( ticks || now >= last ) {
    _Watchdog_Adjust_forward_locked(
      &watchdogs->Header,
      now - last,
      &lock_context
    );
  } else {
    _Watchdog_Adjust_backward_locked(
      &watchdogs->Header,
      last - now
    );
  }

  ++watchdogs->generation;

  _Watchdog_Release( &watchdogs->Header, &lock_context );

  _Timer_server_Update_system_watchdog( watchdogs, system_header );
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

  while ( true ) {
    rtems_event_set events;

    _Timer_server_Tickle(
      &ts->Interval_watchdogs,
      &_Watchdog_Ticks_header,
      _Timer_server_Get_ticks,
      true
    );

    _Timer_server_Tickle(
      &ts->TOD_watchdogs,
      &_Watchdog_Seconds_header,
      _Timer_server_Get_seconds,
      false
    );

    (void) rtems_event_system_receive(
      RTEMS_EVENT_SYSTEM_TIMER_SERVER,
      RTEMS_EVENT_ALL | RTEMS_WAIT,
      RTEMS_NO_TIMEOUT,
      &events
    );
  }
}

static void _Timer_server_Wakeup(
  Objects_Id  id,
  void       *arg
)
{
  Timer_server_Control *ts = arg;

  _Timer_server_Update_current_snapshot(
    &ts->Interval_watchdogs,
    _Timer_server_Get_ticks
  );

  _Timer_server_Update_current_snapshot(
    &ts->TOD_watchdogs,
    _Timer_server_Get_seconds
  );

  (void) rtems_event_system_send( id, RTEMS_EVENT_SYSTEM_TIMER_SERVER );
}

static void _Timer_server_Initialize_watchdogs(
  Timer_server_Control *ts,
  rtems_id id,
  Timer_server_Watchdogs *watchdogs,
  Watchdog_Interval (*get_ticks)( void )
)
{
  Watchdog_Interval now;

  now = (*get_ticks)();
  watchdogs->last_snapshot = now;
  watchdogs->current_snapshot = now;

  _Watchdog_Header_initialize( &watchdogs->Header );
  _Watchdog_Preinitialize( &watchdogs->System_watchdog );
  _Watchdog_Initialize(
    &watchdogs->System_watchdog,
    _Timer_server_Wakeup,
    id,
    ts
  );
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
    _priority = PRIORITY_PSEUDO_ISR;
  }

  /*
   *  Just to make sure this is only called once.
   */
  _Once_Lock();
    tmpInitialized  = initialized;
    initialized = true;
  _Once_Unlock();

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
    rtems_configuration_is_smp_enabled() ?
      RTEMS_DEFAULT_MODES : /* no preempt is not supported for SMP */
      RTEMS_NO_PREEMPT,   /* no preempt is like an interrupt */
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

  _Timer_server_Initialize_watchdogs(
    ts,
    id,
    &ts->Interval_watchdogs,
    _Timer_server_Get_ticks
  );

  _Timer_server_Initialize_watchdogs(
    ts,
    id,
    &ts->TOD_watchdogs,
    _Timer_server_Get_seconds
  );

  /*
   *  Initialize the pointer to the timer server methods so applications that
   *  do not use the Timer Server do not have to pull it in.
   */
  ts->cancel = _Timer_server_Cancel_method;
  ts->schedule_operation = _Timer_server_Schedule_operation_method;

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
