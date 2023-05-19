/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2016 embedded brains GmbH & Co. KG
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

#include <rtems/posix/priorityimpl.h>
#include <rtems/posix/threadsup.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/schedulerimpl.h>

int pthread_setschedprio( pthread_t thread, int prio )
{
  Thread_Control          *the_thread;
  Per_CPU_Control         *cpu_self;
  Thread_queue_Context     queue_context;
  const Scheduler_Control *scheduler;
  Priority_Control         new_priority;
  bool                     valid;

  _Thread_queue_Context_initialize( &queue_context );
  _Thread_queue_Context_clear_priority_updates( &queue_context );
  the_thread = _Thread_Get( thread, &queue_context.Lock_context.Lock_context );

  if ( the_thread == NULL ) {
    return ESRCH;
  }

  _Thread_Wait_acquire_critical( the_thread, &queue_context );

  scheduler = _Thread_Scheduler_get_home( the_thread );

  new_priority = _POSIX_Priority_To_core( scheduler, prio, &valid );
  if ( !valid ) {
    _Thread_Wait_release( the_thread, &queue_context );
    return EINVAL;
  }

  _Thread_Priority_change(
    the_thread,
    &the_thread->Real_priority,
    new_priority,
    PRIORITY_GROUP_FIRST,
    &queue_context
  );

  cpu_self = _Thread_queue_Dispatch_disable( &queue_context );
  _Thread_Wait_release( the_thread, &queue_context );

  _Thread_Priority_update( &queue_context );

  _Thread_Dispatch_enable( cpu_self );
  return 0;
}
