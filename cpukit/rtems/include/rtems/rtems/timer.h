/**
 * @file rtems/rtems/timer.h
 *
 * @defgroup ClassicTimer Timers
 *
 * @ingroup ClassicRTEMS
 * @brief Instantiate RTEMS Timer Data
 *
 * This include file contains all the constants, structures, and
 * prototypes associated with the Timer Manager. This manager provides
 * facilities to configure, initiate, cancel, and delete timers which will
 * fire at specified intervals of time.
 *
 * Directives provided are:
 *
 * - create a timer
 * - get an ID of a timer
 * - delete a timer
 * - set timer to fire in context of clock tick
 * - after a number of ticks have passed
 * - when a specified date and time has been reached
 * - initiate the timer server task
 * - set timer to fire in context of the timer server task
 * - after a number of ticks have passed
 * - when a specified date and time has been reached
 * - reset a timer
 * - cancel a time
 */

/*
 * COPYRIGHT (c) 1989-2011.
 * On-Line Applications Research Corporation (OAR).
 *
 * Copyright (c) 2009 embedded brains GmbH.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_TIMER_H
#define _RTEMS_RTEMS_TIMER_H

#include <rtems/rtems/attr.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/types.h>

#ifdef __cplusplus
extern "C" {
#endif

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

/**
 * @brief RTEMS Create Timer
 *
 * This routine implements the rtems_timer_create directive. The
 * timer will have the name name. It returns the id of the
 * created timer in ID.
 *
 * @param[in] name is the timer name
 * @param[out] id is the pointer to timer id
 *
 * @retval RTEMS_SUCCESSFUL if successful or error code if unsuccessful
 */
rtems_status_code rtems_timer_create(
  rtems_name    name,
  rtems_id     *id
);

/**
 * @brief RTEMS Timer Name to Id
 *
 * This routine implements the rtems_timer_ident directive.
 * This directive returns the timer ID associated with name.
 * If more than one timer is named name, then the timer
 * to which the ID belongs is arbitrary.
 *
 * @param[in] name is the user defined message queue name
 * @param[in] id is the pointer to timer id
 *
 * @retval RTEMS_SUCCESSFUL if successful or error code if unsuccessful and
 * id filled with the message queue id
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
 * @brief RTEMS Delete Timer
 *
 * This routine implements the rtems_timer_delete directive. The
 * timer indicated by ID is deleted.
 *
 * @param[in] id is the timer id
 *
 * @retval This method returns RTEMS_SUCCESSFUL if there was not an
 *         error. Otherwise, a status code is returned indicating the
 *         source of the error.
 */
rtems_status_code rtems_timer_delete(
  rtems_id   id
);

/**
 * @brief RTEMS Timer Fire After
 *
 * This routine implements the rtems_timer_fire_after directive. It
 * initiates the timer associated with ID to fire in ticks clock ticks.
 * When the timer fires, the routine will be invoked in the context
 * of the rtems_clock_tick directive which is normally invoked as
 * part of servicing a periodic interupt.
 *
 * @param[in] id is the timer id
 * @param[in] ticks is the interval until routine is fired
 * @param[in] routine is the routine to schedule
 * @param[in] user_data is the passed as argument to routine when it is fired
 *
 * @retval This method returns RTEMS_SUCCESSFUL if there was not an
 *         error. Otherwise, a status code is returned indicating the
 *         source of the error.
 */
rtems_status_code rtems_timer_fire_after(
  rtems_id                           id,
  rtems_interval                     ticks,
  rtems_timer_service_routine_entry  routine,
  void                              *user_data
);

/**
 * @brief RTEMS Timer Server Fire After
 *
 * This routine implements the rtems_timer_server_fire_after directive. It
 * initiates the timer associated with ID to fire in ticks clock
 * ticks. When the timer fires, the routine will be invoked by the
 * Timer Server in the context of a task NOT IN THE CONTEXT of the
 * clock tick interrupt.
 *
 * @param[in] id is the timer id
 * @param[in] ticks is the interval until routine is fired
 * @param[in] routine is the routine to schedule
 * @param[in] user_data is the passed as argument to routine when it is fired
 *
 * @retval This method returns RTEMS_SUCCESSFUL if there was not an
 *         error. Otherwise, a status code is returned indicating the
 *         source of the error.
 */
rtems_status_code rtems_timer_server_fire_after(
  rtems_id                           id,
  rtems_interval                     ticks,
  rtems_timer_service_routine_entry  routine,
  void                              *user_data
);

/**
 * @brief RTEMS Timer Fire When
 *
 * This routine implements the rtems_timer_fire_when directive. It
 * initiates the timer associated with ID to fire at wall_time
 * When the timer fires, the routine will be invoked in the context
 * of the rtems_clock_tick directive which is normally invoked as
 * part of servicing a periodic interupt.
 *
 * @param[in] id is the timer id
 * @param[in] wall_time is the time of day to fire timer
 * @param[in] routine is the routine to schedule
 * @param[in] user_data is the passed as argument to routine when it is fired
 *
 * @retval This method returns RTEMS_SUCCESSFUL if there was not an
 *         error. Otherwise, a status code is returned indicating the
 *         source of the error.
 */
rtems_status_code rtems_timer_fire_when(
  rtems_id                            id,
  rtems_time_of_day                  *wall_time,
  rtems_timer_service_routine_entry   routine,
  void                               *user_data
);

/**
 *  @brief RTEMS Timer Server Fire When Directive
 *
 *  Timer Manager - RTEMS Timer Server Fire When Directive
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
 *  @brief RTEMS Timer Reset
 *
 *  Timer Manager - RTEMS Timer Reset
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
 * @brief RTEMS Get Timer Information
 *
 * This routine implements the rtems_timer_get_information directive.
 * This directive returns information about the timer.
 *
 * @param[in] id is the timer id
 * @param[in] the_info is the pointer to timer information block
 *
 * @retval RTEMS_SUCCESSFUL if successful or error code if unsuccessful and
 * 		*the_info region information block filled in
 */
rtems_status_code rtems_timer_get_information(
  rtems_id                 id,
  rtems_timer_information *the_info
);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
