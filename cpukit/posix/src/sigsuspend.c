/**
 * @file
 *
 * @brief Replacing signal mask with *sigmask and suspending calling process
 * @ingroup POSIXAPI
 */

/*
 *  3.3.7 Wait for a Signal, P1003.1b-1993, p. 75
 *
 *  COPYRIGHT (c) 1989-2004.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stddef.h>
#include <assert.h>
#include <signal.h>
#include <errno.h>

#include <rtems/seterr.h>

int sigsuspend(
  const sigset_t  *sigmask
)
{
  sigset_t            saved_signals_blocked;
  sigset_t            current_unblocked_signals;

  /*
   *  We use SIG_BLOCK and not SIG_SETMASK because there may be
   *  signals which might be pending, which might get caught here.
   *  We want the signals to be caught inside sigtimedwait.
   *
   *  We ignore the return status codes because sigsuspend() is
   *  defined to either terminate or return -1 with errno set to 
   *  EINTR.
   */
  (void) sigprocmask( SIG_BLOCK, sigmask, &saved_signals_blocked );

  current_unblocked_signals = ~(*sigmask);
  (void) sigtimedwait( &current_unblocked_signals, NULL, NULL );

  (void) sigprocmask( SIG_SETMASK, &saved_signals_blocked, NULL );

  /*
   * sigtimedwait() returns the signal number while sigsuspend()
   * is supposed to return -1 and EINTR when a signal is caught.
   */
  #if defined(RTEMS_DEBUG)
    assert( status != -1 );
  #endif

  rtems_set_errno_and_return_minus_one( EINTR );
}
