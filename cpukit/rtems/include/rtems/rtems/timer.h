/**
 * @file rtems/rtems/timer.h
 *
 *  This include file contains all the constants, structures, and
 *  prototypes associated with the Timer Manager.  This manager provides
 *  facilities to configure, initiate, cancel, and delete timers which will
 *  fire at specified intervals of time.
 *
 *  Directives provided are:
 *
 *     - create a timer
 *     - get an ID of a timer
 *     - delete a timer
 *     - set timer to fire in context of clock tick
 *        - after a number of ticks have passed
 *        - when a specified date and time has been reached
 *     - initiate the timer server task
 *     - set timer to fire in context of the timer server task
 *        - after a number of ticks have passed
 *        - when a specified date and time has been reached
 *     - reset a timer
 *     - cancel a time
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (c) 2009 embedded brains GmbH.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_TIMER_H
#define _RTEMS_RTEMS_TIMER_H

/**
 *  This constant is defined to extern most of the time when using
 *  this header file.  However by defining it to nothing, the data
 *  declared in this header file can be instantiated.  This is done
 *  in a single per manager file.
 */
#ifndef RTEMS_TIMER_EXTERN
#define RTEMS_TIMER_EXTERN extern
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/object.h>
#include <rtems/score/watchdog.h>
#include <rtems/score/thread.h>
#include <rtems/score/chain.h>
#include <rtems/rtems/clock.h>
#include <rtems/rtems/attr.h>

/**
 *  @defgroup ClassicTimer Timers
 *
 *  @ingroup ClassicRTEMS
 *
 *  This encapsulates functionality related to the Classic API Timer
 *  Manager.  This manager provides functionality which allows the
 *  application to schedule the execution of methods at a specified
 *  time in the future.  These methods may be scheduled based upon
 *  interval or wall time and may be executed in either the clock tick
 *  ISR or in a special dedicated timer server task.
 */
/**@{*/

/**
 *  The following enumerated type details the classes to which a timer
 *  may belong.
 */
typedef enum {
  /**
   * This value indicates the timer is currently in use as an interval
   * timer which will fire in the clock tick ISR.
   */
  TIMER_INTERVAL,

  /**
   * This value indicates the timer is currently in use as an interval
   * timer which will fire in the timer server task.
   */
  TIMER_INTERVAL_ON_TASK,

  /**
   * This value indicates the timer is currently in use as an time of day
   * timer which will fire in the clock tick ISR.
   */
  TIMER_TIME_OF_DAY,

  /**
   * This value indicates the timer is currently in use as an time of day
   * timer which will fire in the timer server task.
   */
  TIMER_TIME_OF_DAY_ON_TASK,

  /**
   * This value indicates the timer is currently not in use.
   */
  TIMER_DORMANT
} Timer_Classes;

/**
 *  The following types define a pointer to a timer service routine.
 */
typedef void rtems_timer_service_routine;

/**
 *  This type defines the type used to manage and indirectly invoke
 *  Timer Service Routines (TSRs).  This defines the prototype and interface
 *  for a function which is to be used as a TSR.
 */
typedef rtems_timer_service_routine ( *rtems_timer_service_routine_entry )(
                 rtems_id,
                 void *
             );

/**
 *  The following records define the control block used to manage
 *  each timer.
 */
typedef struct {
  /** This field is the object management portion of a Timer instance. */
  Objects_Control  Object;
  /** This field is the Watchdog instance which will be the scheduled. */
  Watchdog_Control Ticker;
  /** This field indicates what type of timer this currently is. */
  Timer_Classes    the_class;
}   Timer_Control;

typedef struct Timer_server_Control Timer_server_Control;

/**
 * @brief Method used to schedule the insertion of task based timers.
 */
typedef void (*Timer_server_Schedule_operation)(
  Timer_server_Control *timer_server,
  Timer_Control        *timer
);

typedef struct {
  /**
   * @brief This watchdog that will be registered in the system tick mechanic
   * for timer server wake-up.
   */
  Watchdog_Control System_watchdog;

  /**
   * @brief Chain for watchdogs which will be triggered by the timer server.
   */
  Chain_Control Chain;

  /**
   * @brief Last known time snapshot of the timer server.
   *
   * The units may be ticks or seconds.
   */
  Watchdog_Interval volatile last_snapshot;
} Timer_server_Watchdogs;

struct Timer_server_Control {
  /**
   * @brief Timer server thread.
   */
  Thread_Control *thread;

  /**
   * @brief The schedule operation method of the timer server.
   */
  Timer_server_Schedule_operation schedule_operation;

  /**
   * @brief Interval watchdogs triggered by the timer server.
   */
  Timer_server_Watchdogs Interval_watchdogs;

  /**
   * @brief TOD watchdogs triggered by the timer server.
   */
  Timer_server_Watchdogs TOD_watchdogs;

  /**
   * @brief Chain of timers scheduled for insert.
   *
   * This pointer is not @c NULL whenever the interval and TOD chains are
   * processed.  After the processing this list will be checked and if
   * necessary the processing will be restarted.  Processing of these chains
   * can be only interrupted through interrupts.
   */
  Chain_Control *volatile insert_chain;

  /**
   * @brief Indicates that the timer server is active or not.
   *
   * The server is active after the delay on a system watchdog.  The activity
   * period of the server ends when no more watchdogs managed by the server
   * fire.  The system watchdogs must not be manipulated when the server is
   * active.
   */
  bool volatile active;
};

/**
 * @brief Pointer to default timer server control block.
 *
 * This value is @c NULL when the default timer server is not initialized.
 */
RTEMS_TIMER_EXTERN Timer_server_Control *volatile _Timer_server;

/**
 *  The following defines the information control block used to manage
 *  this class of objects.
 */
RTEMS_TIMER_EXTERN Objects_Information  _Timer_Information;

/**
 *  @brief _Timer_Manager_initialization
 *
 *  This routine performs the initialization necessary for this manager.
 */
void _Timer_Manager_initialization(void);

/**
 *  @brief rtems_timer_create
 *
 *  This routine implements the rtems_timer_create directive.  The
 *  timer will have the name name.  It returns the id of the
 *  created timer in ID.
 */
rtems_status_code rtems_timer_create(
  rtems_name    name,
  rtems_id     *id
);

/**
 *  @brief rtems_timer_ident
 *
 *  This routine implements the rtems_timer_ident directive.
 *  This directive returns the timer ID associated with name.
 *  If more than one timer is named name, then the timer
 *  to which the ID belongs is arbitrary.
 */
rtems_status_code rtems_timer_ident(
  rtems_name    name,
  rtems_id     *id
);

/**
 *  @brief rtems_timer_cancel
 *
 *  This routine implements the rtems_timer_cancel directive.  It is used
 *  to stop the timer associated with ID from firing.
 */
rtems_status_code rtems_timer_cancel(
  rtems_id   id
);

/**
 *  @brief rtems_timer_delete
 *
 *  This routine implements the rtems_timer_delete directive.  The
 *  timer indicated by ID is deleted.
 */
rtems_status_code rtems_timer_delete(
  rtems_id   id
);

/**
 *  @brief rtems_timer_fire_after
 *
 *  This routine implements the rtems_timer_fire_after directive.  It
 *  initiates the timer associated with ID to fire in ticks clock ticks.
 *  When the timer fires, the routine will be invoked in the context
 *  of the rtems_clock_tick directive which is normally invoked as
 *  part of servicing a periodic interupt.
 */
rtems_status_code rtems_timer_fire_after(
  rtems_id                           id,
  rtems_interval                     ticks,
  rtems_timer_service_routine_entry  routine,
  void                              *user_data
);

/**
 *  @brief rtems_timer_server_fire_after
 *
 *  This routine implements the rtems_timer_server_fire_after directive.  It
 *  initiates the timer associated with ID to fire in ticks clock
 *  ticks.  When the timer fires, the routine will be invoked by the
 *  Timer Server in the context of a task NOT IN THE CONTEXT of the
 *  clock tick interrupt.
 */
rtems_status_code rtems_timer_server_fire_after(
  rtems_id                           id,
  rtems_interval                     ticks,
  rtems_timer_service_routine_entry  routine,
  void                              *user_data
);

/**
 *  @brief rtems_timer_fire_when
 *
 *  This routine implements the rtems_timer_fire_when directive.  It
 *  initiates the timer associated with ID to fire at wall_time
 *  When the timer fires, the routine will be invoked in the context
 *  of the rtems_clock_tick directive which is normally invoked as
 *  part of servicing a periodic interupt.
 */
rtems_status_code rtems_timer_fire_when(
  rtems_id                            id,
  rtems_time_of_day                  *wall_time,
  rtems_timer_service_routine_entry   routine,
  void                               *user_data
);

/**
 *  @brief rtems_timer_server_fire_when
 *
 *  This routine implements the rtems_timer_server_fire_when directive.  It
 *  initiates the timer associated with ID to fire at wall_time
 *  When the timer fires, the routine will be invoked by the
 *  Timer Server in the context of a task NOT IN THE CONTEXT of the
 *  clock tick interrupt.
 */
rtems_status_code rtems_timer_server_fire_when(
  rtems_id                            id,
  rtems_time_of_day                  *wall_time,
  rtems_timer_service_routine_entry   routine,
  void                               *user_data
);

/**
 *  @brief rtems_timer_reset
 *
 *  This routine implements the rtems_timer_reset directive.  It is used
 *  to reinitialize the interval timer associated with ID just as if
 *  rtems_timer_fire_after were re-invoked with the same arguments that
 *  were used to initiate this timer.
 */
rtems_status_code rtems_timer_reset(
  rtems_id   id
);

/**
 *  @brief rtems_timer_initiate_server
 *
 *  This routine implements the rtems_timer_initiate_server directive.
 *  It creates and starts the server that executes task-based timers.
 *  It must be invoked before any task-based timers can be initiated.
 */
rtems_status_code rtems_timer_initiate_server(
  uint32_t             priority,
  uint32_t             stack_size,
  rtems_attribute      attribute_set
);

/**
 *  This is the default value for the priority of the Timer Server.
 *  When given this priority, a special high priority not accessible
 *  via the Classic API is used.
 */
#define RTEMS_TIMER_SERVER_DEFAULT_PRIORITY (uint32_t) -1

/**
 *  This is the structure filled in by the timer get information
 *  service.
 */
typedef struct {
  /** This indicates the current type of the timer. */
  Timer_Classes      the_class;
  /** This indicates the initial requested interval. */
  Watchdog_Interval  initial;
  /** This indicates the time the timer was initially scheduled. */
  Watchdog_Interval  start_time;
  /** This indicates the time the timer is scheduled to fire. */
  Watchdog_Interval  stop_time;
} rtems_timer_information;

/**
 *  @brief rtems_timer_get_information
 *
 *  This routine implements the rtems_timer_get_information directive.
 *  This directive returns information about the timer.
 */
rtems_status_code rtems_timer_get_information(
  rtems_id                 id,
  rtems_timer_information *the_info
);

#ifndef __RTEMS_APPLICATION__
#include <rtems/rtems/timer.inl>
#endif

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
