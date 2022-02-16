/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Compare Thread IDs
 */

/*
 *  NOTE:  POSIX does not define the behavior when either thread id is invalid.
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
#include <errno.h>

#include <rtems/posix/pthreadimpl.h>
#include <rtems/score/threadimpl.h>

int pthread_equal(
  pthread_t  t1,
  pthread_t  t2
)
{
  /*
   *  If the system is configured for debug, then we will do everything we
   *  can to insure that both ids are valid.  Otherwise, we will do the
   *  cheapest possible thing to determine if they are equal.
   */

#ifndef RTEMS_DEBUG
  return _Objects_Are_ids_equal( t1, t2 );
#else
  ISR_lock_Context  lock_context_1;
  ISR_lock_Context  lock_context_2;
  Thread_Control   *thread_1;
  Thread_Control   *thread_2;

  thread_1 = _Thread_Get( t1, &lock_context_1 );
  thread_2 = _Thread_Get( t2, &lock_context_2 );

  if ( thread_2 != NULL ) {
    _ISR_lock_ISR_enable( &lock_context_2 );
  }

  if ( thread_1 != NULL ) {
    _ISR_lock_ISR_enable( &lock_context_1 );
  }

  return thread_1 != NULL && thread_1 == thread_2;
#endif
}
