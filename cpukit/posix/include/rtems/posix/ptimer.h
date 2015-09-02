/**
 * @file
 * 
 * @brief POSIX Timers Private Support
 *
 * This include file contains all the private support information for
 * POSIX timers.
 */

/*
 *  Initial Implementation:
 *    COPYRIGHT (c) 1998.  Alfonso Escalera Piña
 *    Largely rewritten by Joel Sherrill (1999).
 *
 *  COPYRIGHT (c) 1999-2013.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_POSIX_PTIMER_H
#define _RTEMS_POSIX_PTIMER_H

/**
 * @defgroup POSIX_PRIV_TIMERS POSIX Timers
 *
 * @ingroup POSIXAPI
 */
/**@{**/
#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/posix/config.h>

/**
 *  @brief POSIX Timer Manager Initialization
 *
 *  This routine performs the initialization necessary for this manager.
 */
void _POSIX_Timer_Manager_initialization(void);

/**
 *  @brief Create a Per-Process Timer
 */
int timer_create(
  clockid_t        clock_id,
  struct sigevent *evp,
  timer_t         *timerid
);

/**
 *  @brief Delete a Per-Process Timer
 */
int timer_delete(
  timer_t timerid
);

/**
 *  @brief Set a Per-Process Timer
 */
int timer_settime(
  timer_t                  timerid,
  int                      flags,
  const struct itimerspec *value,
  struct itimerspec       *ovalue
);

/**
 *  @brief Set a Per-Process Timer
 */
int timer_gettime(
  timer_t            timerid,
  struct itimerspec *value
);

/**
 * @brief Get overrun count for a Per-Process Timer
 * 
 * The expiration of a timer must increase by one a counter.
 * After the signal handler associated to the timer finishes
 * its execution, _POSIX_Timer_TSR will have to set this counter to 0.
 */
int timer_getoverrun(
  timer_t   timerid
);

#ifdef __cplusplus
}
#endif
/** @} */

#endif /* _RTEMS_POSIX_PTIMER_H */
