/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * 
 * @brief POSIX Timers Internal Support
 *
 * This include files defines the internal support for implementation of
 * POSIX Timers.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
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

#ifndef _RTEMS_POSIX_TIMER_H
#define _RTEMS_POSIX_TIMER_H

#include <rtems/score/objectdata.h>
#include <rtems/score/watchdog.h>

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup POSIX_INTERNAL_TIMERS POSIX Timer Private Support
 *
 * @ingroup POSIXAPI
 */
/**@{*/

/*
 * Data for a timer
 */
typedef struct {
  Objects_Control   Object;
  Watchdog_Control  Timer;      /* Internal Timer                        */
  pthread_t         thread_id;  /* Thread identifier                     */
  char              state;      /* State of the timer                    */
  struct sigevent   inf;        /* Information associated to the timer   */
  struct itimerspec timer_data; /* Timing data of the timer              */
  uint32_t          ticks;      /* Number of ticks of the initialization */
  uint32_t          overrun;    /* Number of expirations of the timer    */
  struct timespec   time;       /* Time at which the timer was started   */
  clockid_t         clock_type; /* The type of timer */
} POSIX_Timer_Control;

/**
 * @brief The POSIX Timer objects information.
 */
extern Objects_Information _POSIX_Timer_Information;

/**
 * @brief Macro to define the objects information for the POSIX Timer objects.
 *
 * This macro should only be used by <rtems/confdefs.h>.
 *
 * @param max The configured object maximum (the OBJECTS_UNLIMITED_OBJECTS flag
 * may be set).
 */
#define POSIX_TIMER_INFORMATION_DEFINE( max ) \
  OBJECTS_INFORMATION_DEFINE( \
    _POSIX_Timer, \
    OBJECTS_POSIX_API, \
    OBJECTS_POSIX_TIMERS, \
    POSIX_Timer_Control, \
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
