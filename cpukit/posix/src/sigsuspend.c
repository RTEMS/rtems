/*
 *  3.3.7 Wait for a Signal, P1003.1b-1993, p. 75
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <signal.h>
#include <errno.h>

#include <rtems/system.h>
#include <rtems/posix/pthread.h>
#include <rtems/posix/psignal.h>

int sigsuspend(
  const sigset_t  *sigmask
)
{
  sigset_t            saved_signals_blocked;
  sigset_t            all_signals;
  int                 status;
  POSIX_API_Control  *api;

  api = _Thread_Executing->API_Extensions[ THREAD_API_POSIX ];

  status = sigprocmask( SIG_BLOCK, sigmask, &saved_signals_blocked );

  (void) sigfillset( &all_signals );

  status = sigtimedwait( &all_signals, NULL, NULL );

  (void) sigprocmask( SIG_SETMASK, &saved_signals_blocked, NULL );

  return status;
}
