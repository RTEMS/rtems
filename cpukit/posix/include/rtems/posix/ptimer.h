/**
 * @file rtems/posix/ptimer.h
 *
 * This include file contains all the private support information for
 * POSIX timers.
 */

/*
 *  Initial Implementation:
 *    COPYRIGHT (c) 1998.  Alfonso Escalera Piña
 *  Largely rewritten by Joel Sherrill.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  ptimer.h,v 1.0 1998/03/31 16:21:16
 */

#ifndef _RTEMS_POSIX_PTIMER_H
#define _RTEMS_POSIX_PTIMER_H

/**
 *  @defgroup POSIX_PRIV_TIMERS Timers
 *
 *  @ingroup POSIX
 */
/**@{*/
#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/posix/config.h>

/*
 *  _POSIX_Timers_Manager_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine performs the initialization necessary for this manager.
 */

void _POSIX_Timer_Manager_initialization(void);

/*
 *  @brief Create a Per-Process Timer
 *
 *  14.2.2 Create a Per-Process Timer, P1003.1b-1993, p. 264
 *
 *  timer_create
 */

int timer_create(
  clockid_t        clock_id,
  struct sigevent *evp,
  timer_t         *timerid
);

/*
 *  14.2.3 Delete a Per_process Timer, P1003.1b-1993, p. 266
 */

int timer_delete(
  timer_t timerid
);

/*
 *  14.2.4 Per-Process Timers, P1003.1b-1993, p. 267
 *
 *  timer_settime
 */

int timer_settime(
  timer_t                  timerid,
  int                      flags,
  const struct itimerspec *value,
  struct itimerspec       *ovalue
);

/*
 *  14.2.4 Per-Process Timers, P1003.1b-1993, p. 267
 *
 *  timer_gettime
 */

int timer_gettime(
  timer_t            timerid,
  struct itimerspec *value
);

/**
 *  @brief Get Overrun Count for a POSIX Per-Process Timer
 * 
 *  The expiration of a timer must increase by one a counter.
 *  After the signal handler associated to the timer finishes
 *  its execution, _POSIX_Timer_TSR will have to set this counter to 0.
 * 
 *  14.2.4 Per-Process Timers, P1003.1b-1993, p. 267
 */
int timer_getoverrun(
  timer_t   timerid
);
/**@}*/
#endif
