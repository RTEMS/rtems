/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicTimer
 *
 * @brief This header file provides the Timer Manager API.
 */

/*
 * Copyright (C) 2020 embedded brains GmbH (http://www.embedded-brains.de)
 * Copyright (C) 1988, 2008 On-Line Applications Research Corporation (OAR)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * This file is part of the RTEMS quality process and was automatically
 * generated.  If you find something that needs to be fixed or
 * worded better please post a report or patch to an RTEMS mailing list
 * or raise a bug report:
 *
 * https://www.rtems.org/bugs.html
 *
 * For information on updating and regenerating please refer to the How-To
 * section in the Software Requirements Engineering chapter of the
 * RTEMS Software Engineering manual.  The manual is provided as a part of
 * a release.  For development sources please refer to the online
 * documentation at:
 *
 * https://docs.rtems.org
 */

/* Generated from spec:/rtems/timer/if/header */

#ifndef _RTEMS_RTEMS_TIMER_H
#define _RTEMS_RTEMS_TIMER_H

#include <stddef.h>
#include <rtems/rtems/attr.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/tasks.h>
#include <rtems/rtems/types.h>
#include <rtems/score/watchdogticks.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/rtems/timer/if/group */

/**
 * @defgroup RTEMSAPIClassicTimer Timer Manager
 *
 * @ingroup RTEMSAPIClassic
 *
 * @brief The Timer Manager provides support for timer facilities.
 */

/* Generated from spec:/rtems/timer/if/class-bit-not-dormant */

/**
 * @ingroup RTEMSAPIClassicTimer
 *
 * @brief This timer class bit indicates that the timer is not dormant.
 */
#define TIMER_CLASS_BIT_NOT_DORMANT 0x4

/* Generated from spec:/rtems/timer/if/class-bit-on-task */

/**
 * @ingroup RTEMSAPIClassicTimer
 *
 * @brief This timer class bit indicates that the timer routine executes in a
 *   task context.
 */
#define TIMER_CLASS_BIT_ON_TASK 0x2

/* Generated from spec:/rtems/timer/if/class-bit-time-of-day */

/**
 * @ingroup RTEMSAPIClassicTimer
 *
 * @brief This timer class bit indicates that the timer uses a time of day.
 */
#define TIMER_CLASS_BIT_TIME_OF_DAY 0x1

/* Generated from spec:/rtems/timer/if/classes */

/**
 * @ingroup RTEMSAPIClassicTimer
 *
 * @brief The timer class indicates how the timer was most recently fired.
 */
typedef enum {
  /**
   * @brief This timer class indicates that the timer was never in use.
   */
  TIMER_DORMANT,

  /**
   * @brief This timer class indicates that the timer is currently in use as an
   *   interval timer which will fire in the context of the clock tick ISR.
   */
  TIMER_INTERVAL = TIMER_CLASS_BIT_NOT_DORMANT,

  /**
   * @brief This timer class indicates that the timer is currently in use as an
   *   interval timer which will fire in the context of the Timer Server task.
   */
  TIMER_INTERVAL_ON_TASK = TIMER_CLASS_BIT_NOT_DORMANT |
    TIMER_CLASS_BIT_ON_TASK,

  /**
   * @brief This timer class indicates that the timer is currently in use as an
   *   time of day timer which will fire in the context of the clock tick ISR.
   */
  TIMER_TIME_OF_DAY = TIMER_CLASS_BIT_NOT_DORMANT |
    TIMER_CLASS_BIT_TIME_OF_DAY,

  /**
   * @brief This timer class indicates that the timer is currently in use as an
   *   time of day timer which will fire in the context of the Timer Server task.
   */
  TIMER_TIME_OF_DAY_ON_TASK = TIMER_CLASS_BIT_NOT_DORMANT |
    TIMER_CLASS_BIT_TIME_OF_DAY |
    TIMER_CLASS_BIT_ON_TASK
} Timer_Classes;

/* Generated from spec:/rtems/timer/if/information */

/**
 * @ingroup RTEMSAPIClassicTimer
 *
 * @brief The structure contains information about a timer.
 */
typedef struct {
  /**
   * @brief The timer class member indicates how the timer was most recently
   *   fired.
   */
  Timer_Classes the_class;

  /**
   * @brief This member indicates the initial requested interval.
   */
  Watchdog_Interval initial;

  /**
   * @brief This member indicates the time the timer was initially scheduled.
   *
   * The time is in clock ticks since the clock driver initialization or the last
   * clock tick counter overflow.
   */
  Watchdog_Interval start_time;

  /**
   * @brief This member indicates the time the timer was scheduled to fire.
   *
   * The time is in clock ticks since the clock driver initialization or the last
   * clock tick counter overflow.
   */
  Watchdog_Interval stop_time;
} rtems_timer_information;

/* Generated from spec:/rtems/timer/if/get-information */

/**
 * @ingroup RTEMSAPIClassicTimer
 *
 * @brief Gets information about the timer.
 *
 * @param id is the timer identifier.
 *
 * @param[out] the_info is the pointer to a timer information variable.  The
 *   information about the timer will be stored in this variable, in case of a
 *   successful operation.
 *
 * This directive returns information about the timer.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``the_info`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_ID There was no timer associated with the identifier
 *   specified by ``id``.
 *
 * @par Notes
 * This directive will not cause the running task to be preempted.
 */
rtems_status_code rtems_timer_get_information(
  rtems_id                 id,
  rtems_timer_information *the_info
);

/* Generated from spec:/rtems/timer/if/server-default-priority */

/**
 * @ingroup RTEMSAPIClassicTimer
 *
 * @brief This constant represents the default value for the task priority of
 *   the Timer Server.
 *
 * When given this priority, a special high priority not accessible via the
 * Classic API is used.
 */
#define RTEMS_TIMER_SERVER_DEFAULT_PRIORITY ( (rtems_task_priority) -1 )

/* Generated from spec:/rtems/timer/if/service-routine */

/**
 * @ingroup RTEMSAPIClassicTimer
 *
 * @brief This type defines the return type of routines which can be fired by
 *   directives of the Timer Manager.
 *
 * This type can be used to document timer service routines in the source code.
 */
typedef void rtems_timer_service_routine;

/* Generated from spec:/rtems/timer/if/service-routine-entry */

/**
 * @ingroup RTEMSAPIClassicTimer
 *
 * @brief This type defines the prototype of routines which can be fired by
 *   directives of the Timer Manager.
 */
typedef rtems_timer_service_routine ( *rtems_timer_service_routine_entry )( rtems_id, void * );

/* Generated from spec:/rtems/timer/if/create */

/**
 * @ingroup RTEMSAPIClassicTimer
 *
 * @brief Creates a timer.
 *
 * @param name is the name of the timer.
 *
 * @param[out] id is the pointer to an object identifier variable.  The
 *   identifier of the created timer object will be stored in this variable, in
 *   case of a successful operation.
 *
 * This directive creates a timer.  The assigned object identifier is returned
 * in ``id``.  This identifier is used to access the timer with other timer
 * related directives.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_NAME The timer name was invalid.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``id`` parameter was NULL.
 *
 * @retval ::RTEMS_TOO_MANY There was no inactive object available to create a
 *   new timer.  The number of timers available to the application is
 *   configured through the #CONFIGURE_MAXIMUM_TIMERS configuration option.
 *
 * @par Notes
 * @parblock
 * This directive may cause the calling task to be preempted due to an obtain
 * and release of the object allocator mutex.
 *
 * For control and maintenance of the timer, RTEMS allocates a TMCB from the
 * local TMCB free pool and initializes it.
 *
 * In SMP configurations, the processor of the currently executing thread
 * determines the processor used for the created timer.  During the life-time
 * of the timer this processor is used to manage the timer internally.
 * @endparblock
 */
rtems_status_code rtems_timer_create( rtems_name name, rtems_id *id );

/* Generated from spec:/rtems/timer/if/ident */

/**
 * @ingroup RTEMSAPIClassicTimer
 *
 * @brief Identifies a timer by the object name.
 *
 * @param name is the object name to look up.
 *
 * @param[out] id is the pointer to an object identifier variable.  The object
 *   identifier of an object with the specified name will be stored in this
 *   variable, in case of a successful operation.
 *
 * This directive obtains the timer identifier associated with the timer name
 * specified in ``name``.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``id`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_NAME The ``name`` parameter was 0.
 *
 * @retval ::RTEMS_INVALID_NAME There was no object with the specified name on
 *   the local node.
 *
 * @par Notes
 * @parblock
 * If the timer name is not unique, then the timer identifier will match the
 * first timer with that name in the search order.  However, this timer
 * identifier is not guaranteed to correspond to the desired timer.  The timer
 * identifier is used with other timer related directives to access the timer.
 *
 * The objects are searched from lowest to the highest index.  Only the local
 * node is searched.
 * @endparblock
 */
rtems_status_code rtems_timer_ident( rtems_name name, rtems_id *id );

/* Generated from spec:/rtems/timer/if/cancel */

/**
 * @ingroup RTEMSAPIClassicTimer
 *
 * @brief Cancels the timer.
 *
 * @param id is the timer identifier.
 *
 * This directive cancels the timer specified in the ``id`` parameter.  This
 * timer will be reinitiated by the next invocation of rtems_timer_reset(),
 * rtems_timer_fire_after(), or rtems_timer_fire_when() with the same timer
 * identifier.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ID There was no timer associated with the identifier
 *   specified by ``id``.
 *
 * @par Notes
 * This directive will not cause the running task to be preempted.
 */
rtems_status_code rtems_timer_cancel( rtems_id id );

/* Generated from spec:/rtems/timer/if/delete */

/**
 * @ingroup RTEMSAPIClassicTimer
 *
 * @brief Deletes the timer.
 *
 * @param id is the timer identifier.
 *
 * This directive deletes the timer specified by the ``id`` parameter.  If the
 * timer is running, it is automatically canceled.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ID There was no timer associated with the identifier
 *   specified by ``id``.
 *
 * @par Notes
 * @parblock
 * This directive may cause the calling task to be preempted due to an obtain
 * and release of the object allocator mutex.
 *
 * The TMCB for the deleted timer is reclaimed by RTEMS.
 *
 * A timer can be deleted by a task other than the task which created the
 * timer.
 * @endparblock
 */
rtems_status_code rtems_timer_delete( rtems_id id );

/* Generated from spec:/rtems/timer/if/fire-after */

/**
 * @ingroup RTEMSAPIClassicTimer
 *
 * @brief Fires the timer after the interval.
 *
 * @param id is the timer identifier.
 *
 * @param ticks is the interval until the routine is fired in clock ticks.
 *
 * @param routine is the routine to schedule.
 *
 * @param user_data is the argument passed to the routine when it is fired.
 *
 * This directive initiates the timer specified by ``id``.  If the timer is
 * running, it is automatically canceled before being initiated.  The timer is
 * scheduled to fire after an interval of clock ticks has passed specified by
 * ``ticks``.  When the timer fires, the timer service routine ``routine`` will
 * be invoked with the argument ``user_data`` in the context of the clock tick
 * ISR.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_NUMBER The ``ticks`` parameter was 0.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``routine`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_ID There was no timer associated with the identifier
 *   specified by ``id``.
 *
 * @par Notes
 * This directive will not cause the running task to be preempted.
 */
rtems_status_code rtems_timer_fire_after(
  rtems_id                          id,
  rtems_interval                    ticks,
  rtems_timer_service_routine_entry routine,
  void                             *user_data
);

/* Generated from spec:/rtems/timer/if/fire-when */

/**
 * @ingroup RTEMSAPIClassicTimer
 *
 * @brief Fires the timer at the time of day.
 *
 * @param id is the timer identifier.
 *
 * @param wall_time is the time of day when the routine is fired.
 *
 * @param routine is the routine to schedule.
 *
 * @param user_data is the argument passed to the routine when it is fired.
 *
 * This directive initiates the timer specified by ``id``.  If the timer is
 * running, it is automatically canceled before being initiated.  The timer is
 * scheduled to fire at the time of day specified by ``wall_time``.  When the
 * timer fires, the timer service routine ``routine`` will be invoked with the
 * argument ``user_data`` in the context of the clock tick ISR.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_NOT_DEFINED The system date and time was not set.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``routine`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``wall_time`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_CLOCK The time of day was invalid.
 *
 * @retval ::RTEMS_INVALID_ID There was no timer associated with the identifier
 *   specified by ``id``.
 *
 * @par Notes
 * This directive will not cause the running task to be preempted.
 */
rtems_status_code rtems_timer_fire_when(
  rtems_id                          id,
  rtems_time_of_day                *wall_time,
  rtems_timer_service_routine_entry routine,
  void                             *user_data
);

/* Generated from spec:/rtems/timer/if/initiate-server */

/**
 * @ingroup RTEMSAPIClassicTimer
 *
 * @brief Initiates the Timer Server.
 *
 * @param priority is the task priority.
 *
 * @param stack_size is the task stack size in bytes.
 *
 * @param attribute_set is the task attribute set.
 *
 * This directive initiates the Timer Server task.  This task is responsible
 * for executing all timers initiated via the rtems_timer_server_fire_after()
 * or rtems_timer_server_fire_when() directives.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INCORRECT_STATE The Timer Server was already initiated.
 *
 * @retval ::RTEMS_INVALID_PRIORITY The task priority was invalid.
 *
 * @retval ::RTEMS_TOO_MANY There was no inactive task object available to
 *   create the Timer Server task.
 *
 * @retval ::RTEMS_UNSATISFIED There was not enough memory to allocate the task
 *   storage area.  The task storage area contains the task stack, the
 *   thread-local storage, and the floating point context.
 *
 * @retval ::RTEMS_UNSATISFIED One of the task create extensions failed to
 *   create the Timer Server task.
 *
 * @par Notes
 * @parblock
 * This directive may cause the calling task to be preempted due to an obtain
 * and release of the object allocator mutex.
 *
 * The Timer Server task is created using the rtems_task_create() directive and
 * must be accounted for when configuring the system.
 * @endparblock
 */
rtems_status_code rtems_timer_initiate_server(
  rtems_task_priority priority,
  size_t              stack_size,
  rtems_attribute     attribute_set
);

/* Generated from spec:/rtems/timer/if/server-fire-after */

/**
 * @ingroup RTEMSAPIClassicTimer
 *
 * @brief Fires the timer after the interval using the Timer Server.
 *
 * @param id is the timer identifier.
 *
 * @param ticks is the interval until the routine is fired in clock ticks.
 *
 * @param routine is the routine to schedule.
 *
 * @param user_data is the argument passed to the routine when it is fired.
 *
 * This directive initiates the timer specified by ``id``.  If the timer is
 * running, it is automatically canceled before being initiated.  The timer is
 * scheduled to fire after an interval of clock ticks has passed specified by
 * ``ticks``.  When the timer fires, the timer service routine ``routine`` will
 * be invoked with the argument ``user_data`` in the context of the Timer
 * Server task.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INCORRECT_STATE The Timer Server was not initiated.
 *
 * @retval ::RTEMS_INVALID_NUMBER The ``ticks`` parameter was 0.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``routine`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_ID There was no timer associated with the identifier
 *   specified by ``id``.
 *
 * @par Notes
 * This directive will not cause the running task to be preempted.
 */
rtems_status_code rtems_timer_server_fire_after(
  rtems_id                          id,
  rtems_interval                    ticks,
  rtems_timer_service_routine_entry routine,
  void                             *user_data
);

/* Generated from spec:/rtems/timer/if/server-fire-when */

/**
 * @ingroup RTEMSAPIClassicTimer
 *
 * @brief Fires the timer at the time of day using the Timer Server.
 *
 * @param id is the timer identifier.
 *
 * @param wall_time is the time of day when the routine is fired.
 *
 * @param routine is the routine to schedule.
 *
 * @param user_data is the argument passed to the routine when it is fired.
 *
 * This directive initiates the timer specified by ``id``.  If the timer is
 * running, it is automatically canceled before being initiated.  The timer is
 * scheduled to fire at the time of day specified by ``wall_time``.  When the
 * timer fires, the timer service routine ``routine`` will be invoked with the
 * argument ``user_data`` in the context of the Timer Server task.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INCORRECT_STATE The Timer Server was not initiated.
 *
 * @retval ::RTEMS_NOT_DEFINED The system date and time was not set.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``routine`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``wall_time`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_CLOCK The time of day was invalid.
 *
 * @retval ::RTEMS_INVALID_ID There was no timer associated with the identifier
 *   specified by ``id``.
 *
 * @par Notes
 * This directive will not cause the running task to be preempted.
 */
rtems_status_code rtems_timer_server_fire_when(
  rtems_id                          id,
  rtems_time_of_day                *wall_time,
  rtems_timer_service_routine_entry routine,
  void                             *user_data
);

/* Generated from spec:/rtems/timer/if/reset */

/**
 * @ingroup RTEMSAPIClassicTimer
 *
 * @brief Resets the timer.
 *
 * @param id is the timer identifier.
 *
 * This directive resets the timer specified by ``id``.  This timer must have
 * been previously initiated with either the rtems_timer_fire_after() or
 * rtems_timer_server_fire_after() directive.  If active the timer is canceled,
 * after which the timer is reinitiated using the same interval and timer
 * service routine which the original rtems_timer_fire_after() or
 * rtems_timer_server_fire_after() directive used.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ID There was no timer associated with the identifier
 *   specified by ``id``.
 *
 * @retval ::RTEMS_NOT_DEFINED The timer was not of the interval class.
 *
 * @par Notes
 * @parblock
 * This directive will not cause the running task to be preempted.
 *
 * If the timer has not been used or the last usage of this timer was by a
 * rtems_timer_fire_when() or rtems_timer_server_fire_when() directive, then
 * the ::RTEMS_NOT_DEFINED error is returned.
 *
 * Restarting a cancelled after timer results in the timer being reinitiated
 * with its previous timer service routine and interval.
 * @endparblock
 */
rtems_status_code rtems_timer_reset( rtems_id id );

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_RTEMS_TIMER_H */
