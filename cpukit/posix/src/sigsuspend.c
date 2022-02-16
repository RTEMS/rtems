/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Replacing signal mask with *sigmask and suspending calling process
 */

/*
 *  3.3.7 Wait for a Signal, P1003.1b-1993, p. 75
 *
 *  COPYRIGHT (c) 1989-2004.
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

#ifdef HAVE_CONFIG_H
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
#if defined(RTEMS_DEBUG)
    int                 status;
#endif

  /*
   *  We use SIG_BLOCK and not SIG_SETMASK because there may be
   *  signals which might be pending, which might get caught here.
   *  We want the signals to be caught inside sigtimedwait.
   *
   *  We ignore the return status codes because sigsuspend() is
   *  defined to either terminate or return -1 with errno set to
   *  EINTR.
   */
#if defined(RTEMS_DEBUG)
    status =
#else
  (void)
#endif
      sigprocmask( SIG_BLOCK, sigmask, &saved_signals_blocked );

  current_unblocked_signals = ~(*sigmask);

  #if defined(RTEMS_DEBUG)
    status =
#else
  (void)
#endif
      sigtimedwait( &current_unblocked_signals, NULL, NULL );

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
