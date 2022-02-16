/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Sends a signal Asynchronously directed to a thread
 */

/*
 *  3.3.10 Send a Signal to a Thread, P1003.1c/D10, p. 43
 *
 *  COPYRIGHT (c) 1989-2007.
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

#include <rtems/posix/threadsup.h>
#include <rtems/posix/psignalimpl.h>
#include <rtems/score/threadimpl.h>

int pthread_kill( pthread_t thread, int sig )
{
  Thread_Control    *the_thread;
  ISR_lock_Context   lock_context;
  POSIX_API_Control *api;
  Per_CPU_Control   *cpu_self;

  if ( !is_valid_signo( sig ) ) {
    return EINVAL;
  }

  the_thread = _Thread_Get( thread, &lock_context );

  if ( the_thread == NULL ) {
    return ESRCH;
  }

  api = the_thread->API_Extensions[ THREAD_API_POSIX ];

  if ( _POSIX_signals_Vectors[ sig ].sa_handler == SIG_IGN ) {
    _ISR_lock_ISR_enable( &lock_context );
    return 0;
  }

  /* XXX critical section */

  api->signals_pending |= signo_to_mask( sig );

  cpu_self = _Thread_Dispatch_disable_critical( &lock_context );
  _ISR_lock_ISR_enable( &lock_context );

  (void) _POSIX_signals_Unblock_thread( the_thread, sig, NULL );
  _Thread_Dispatch_enable( cpu_self );
  return 0;
}
