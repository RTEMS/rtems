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
