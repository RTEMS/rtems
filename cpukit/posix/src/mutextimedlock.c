/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Mutex Timed Lock
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/posix/muteximpl.h>

/**
 * 11.3.3 Locking and Unlocking a Mutex, P1003.1c/Draft 10, p. 93
 *
 * NOTE: P1003.4b/D8 adds pthread_mutex_timedlock(), p. 29
 */
int pthread_mutex_timedlock(
  pthread_mutex_t       *mutex,
  const struct timespec *abstime
)
{
  return _POSIX_Mutex_Lock_support(
    mutex,
    abstime,
    _Thread_queue_Add_timeout_realtime_timespec
  );
}
