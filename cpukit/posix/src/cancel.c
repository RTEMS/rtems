/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Canceling Execution of a Thread
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (c) 2016 embedded brains GmbH & Co. KG
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

#include <rtems/score/isr.h>
#include <rtems/score/threadimpl.h>

/*
 *  18.2.1 Canceling Execution of a Thread, P1003.1c/Draft 10, p. 181
 */

int pthread_cancel( pthread_t thread )
{
  Thread_Control   *the_thread;
  ISR_lock_Context  lock_context;
  Thread_Control   *executing;
  Per_CPU_Control  *cpu_self;

  the_thread = _Thread_Get( thread, &lock_context );

  if ( the_thread == NULL ) {
    return ESRCH;
  }

  cpu_self = _Per_CPU_Get();
  executing = _Per_CPU_Get_executing( cpu_self );

  if (
    the_thread == executing &&
    !_Per_CPU_Is_ISR_in_progress( cpu_self )
  ) {
    _ISR_lock_ISR_enable( &lock_context );
    _Thread_Exit( PTHREAD_CANCELED, THREAD_LIFE_TERMINATING );
  } else {
    _Thread_Dispatch_disable_with_CPU( cpu_self, &lock_context );
    _ISR_lock_ISR_enable( &lock_context );
    (void) _Thread_Cancel( the_thread, executing, 0 );
    _Thread_Dispatch_enable( cpu_self );
  }
  return 0;
}
