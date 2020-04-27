/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Examine and Change Blocked Signals
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <signal.h>

/**
 * 3.3.5 Examine and Change Blocked Signals, P1003.1b-1993, p. 73
 *
 * NOTE: P1003.1c/D10, p. 37 adds pthread_sigmask().
 */
int sigprocmask(
  int               how,
  const sigset_t   *__restrict set,
  sigset_t         *__restrict oset
)
{
  /*
   *  P1003.1c/Draft 10, p. 38 maps sigprocmask to pthread_sigmask.
   */

#if defined(RTEMS_POSIX_API)
  return pthread_sigmask( how, set, oset );
#else
  return -1;
#endif
}
