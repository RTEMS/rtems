/**
 * @file
 *
 * @brief Suspends Execution of Calling Thread until Signals in set Delivered
 */

/*
 *  3.3.8 Synchronously Accept a Signal, P1003.1b-1993, p. 76
 *
 *  NOTE: P1003.1c/D10, p. 39 adds sigwait().
 *
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

#include <stddef.h>
#include <signal.h>

int sigwaitinfo(
  const sigset_t  *__restrict set,
  siginfo_t       *__restrict info
)
{
  return sigtimedwait( set, info, NULL );
}
