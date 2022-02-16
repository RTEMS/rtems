/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Examine and/or change the calling thread's signal mask
 */

/*
 *  3.3.5 Examine and Change Blocked Signals, P1003.1b-1993, p. 73
 *
 *  NOTE: P1003.1c/D10, p. 37 adds pthread_sigmask().
 *
 *  COPYRIGHT (c) 1989-1999.
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

#include <pthread.h>
#include <signal.h>
#include <errno.h>

#include <rtems/posix/pthreadimpl.h>
#include <rtems/posix/psignalimpl.h>
#include <rtems/seterr.h>

int pthread_sigmask(
  int               how,
  const sigset_t   *__restrict set,
  sigset_t         *__restrict oset
)
{
  POSIX_API_Control  *api;

  if ( !set && !oset )
    rtems_set_errno_and_return_minus_one( EINVAL );

  api = _Thread_Get_executing()->API_Extensions[ THREAD_API_POSIX ];

  if ( oset )
    *oset = ~api->signals_unblocked;

  if ( !set )
    return 0;

  switch ( how ) {
    case SIG_BLOCK:
      api->signals_unblocked &= ~*set;
      break;
    case SIG_UNBLOCK:
      api->signals_unblocked |= *set;
      break;
    case SIG_SETMASK:
      api->signals_unblocked = ~*set;
      break;
    default:
      rtems_set_errno_and_return_minus_one( EINVAL );
  }

  /* XXX are there critical section problems here? */

  /* XXX evaluate the new set */

  if ( api->signals_unblocked &
       (api->signals_pending | _POSIX_signals_Pending) ) {
    _Thread_Dispatch();
  }

  return 0;
}
