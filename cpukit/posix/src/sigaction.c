/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Allows calling process to examine action of a Specific Signal
 */

/*
 *  3.3.4 Examine and Change Signal Action, P1003.1b-1993, p. 70
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

#include <rtems/posix/psignalimpl.h>
#include <rtems/seterr.h>

int sigaction(
  int                     sig,
  const struct sigaction *__restrict act,
  struct sigaction       *__restrict oact
)
{
  Thread_queue_Context queue_context;

  if ( !sig )
    rtems_set_errno_and_return_minus_one( EINVAL );

  if ( !is_valid_signo(sig) )
    rtems_set_errno_and_return_minus_one( EINVAL );

  /*
   *  Some signals cannot be ignored (P1003.1b-1993, pp. 70-72 and references.
   *
   *  NOTE: Solaris documentation claims to "silently enforce" this which
   *        contradicts the POSIX specification.
   */

  if ( sig == SIGKILL )
    rtems_set_errno_and_return_minus_one( EINVAL );

  _Thread_queue_Context_initialize( &queue_context );
  _POSIX_signals_Acquire( &queue_context );

  if ( oact )
    *oact = _POSIX_signals_Vectors[ sig ];

  /*
   *  Evaluate the new action structure and set the global signal vector
   *  appropriately.
   */

  if ( act ) {

    /*
     *  Unless the user is installing the default signal actions, then
     *  we can just copy the provided sigaction structure into the vectors.
     */

    if ( act->sa_handler == SIG_DFL ) {
      _POSIX_signals_Vectors[ sig ] = _POSIX_signals_Default_vectors[ sig ];
    } else {
       _POSIX_signals_Clear_process_signals( sig );
       _POSIX_signals_Vectors[ sig ] = *act;
    }
  }

  _POSIX_signals_Release( &queue_context );

  return 0;
}
