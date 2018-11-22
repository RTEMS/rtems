/**
 * @file
 *
 * @ingroup ClassicTimerImpl
 *
 * @brief Classic Partition Manager Data Structures
 */

/*
 * COPYRIGHT (c) 1989-2011.
 * On-Line Applications Research Corporation (OAR).
 *
 * Copyright (c) 2009, 2016 embedded brains GmbH.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_TIMERDATA_H
#define _RTEMS_RTEMS_TIMERDATA_H

#include <rtems/rtems/timer.h>
#include <rtems/score/objectdata.h>
#include <rtems/score/watchdog.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ClassicTimerImpl
 *
 * @{
 */

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
  /** This field is the timer service routine. */
  rtems_timer_service_routine_entry routine;
  /** This field is the timer service routine user data. */
  void *user_data;
  /** This field is the timer interval in ticks or seconds. */
  Watchdog_Interval initial;
  /** This field is the timer start time point in ticks. */
  Watchdog_Interval start_time;
  /** This field is the timer stop time point in ticks. */
  Watchdog_Interval stop_time;
}   Timer_Control;

/**
 * @brief The Classic Timer objects information.
 */
extern Objects_Information  _Timer_Information;

/**
 * @brief Macro to define the objects information for the Classic Timer
 * objects.
 *
 * This macro should only be used by <rtems/confdefs.h>.
 *
 * @param max The configured object maximum (the OBJECTS_UNLIMITED_OBJECTS flag
 * may be set).
 */
#define TIMER_INFORMATION_DEFINE( max ) \
  OBJECTS_INFORMATION_DEFINE( \
    _Timer, \
    OBJECTS_CLASSIC_API, \
    OBJECTS_RTEMS_TIMERS, \
    Timer_Control, \
    max, \
    OBJECTS_NO_STRING_NAME, \
    NULL \
  )

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
