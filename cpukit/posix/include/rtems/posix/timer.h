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
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_POSIX_TIMER_H
#define _RTEMS_POSIX_TIMER_H

#include <rtems/score/object.h>
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
} POSIX_Timer_Control;

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
