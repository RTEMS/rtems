/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicTimer
 *
 * @brief This header file provides data structures used by the implementation
 *   and the @ref RTEMSImplApplConfig to define ::_Timer_Information.
 */

/*
 * COPYRIGHT (c) 1989-2011.
 * On-Line Applications Research Corporation (OAR).
 *
 * Copyright (C) 2009, 2016 embedded brains GmbH & Co. KG
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

#ifndef _RTEMS_RTEMS_TIMERDATA_H
#define _RTEMS_RTEMS_TIMERDATA_H

#include <rtems/rtems/timer.h>
#include <rtems/score/objectdata.h>
#include <rtems/score/watchdog.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup RTEMSImplClassicTimer
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
