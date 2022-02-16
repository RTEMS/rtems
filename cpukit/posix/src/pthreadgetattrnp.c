/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Pthread Get Attribute
 */

/*
 *  COPYRIGHT (c) 2014.
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

#define  _GNU_SOURCE

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <errno.h>
#include <string.h>

#include <rtems/posix/pthreadimpl.h>
#include <rtems/posix/posixapi.h>
#include <rtems/posix/pthreadattrimpl.h>
#include <rtems/posix/priorityimpl.h>
#include <rtems/score/schedulerimpl.h>
#include <rtems/score/threadimpl.h>

int pthread_getattr_np(
  pthread_t       thread,
  pthread_attr_t *attr
)
{
  Thread_Control                     *the_thread;
  ISR_lock_Context                    lock_context;
  const Thread_CPU_budget_operations *cpu_budget_operations;
  const Scheduler_Control            *scheduler;
  Priority_Control                    priority;
  Status_Control                      status;

  if ( attr == NULL ) {
    return EINVAL;
  }

  attr = memset( attr, 0, sizeof( *attr ) );

  the_thread = _Thread_Get( thread, &lock_context );

  if ( the_thread == NULL ) {
    return ESRCH;
  }

  _Thread_State_acquire_critical( the_thread, &lock_context );

  attr->stackaddr = the_thread->Start.Initial_stack.area;
  attr->stacksize = the_thread->Start.Initial_stack.size;

  if ( the_thread->was_created_with_inherited_scheduler ) {
    attr->inheritsched = PTHREAD_INHERIT_SCHED;
  } else {
    attr->inheritsched = PTHREAD_EXPLICIT_SCHED;
  }

  scheduler = _Thread_Scheduler_get_home( the_thread );
  _POSIX_Threads_Get_sched_param_sporadic(
    the_thread,
    scheduler,
    &attr->schedparam
  );
  priority = the_thread->Real_priority.priority;

  if ( _Thread_Is_joinable( the_thread ) ) {
    attr->detachstate = PTHREAD_CREATE_JOINABLE;
  } else {
    attr->detachstate = PTHREAD_CREATE_DETACHED;
  }

  attr->affinityset = &attr->affinitysetpreallocated;
  attr->affinitysetsize = sizeof( attr->affinitysetpreallocated );
  status = _Scheduler_Get_affinity(
    the_thread,
    attr->affinitysetsize,
    attr->affinityset
  );

  cpu_budget_operations = the_thread->CPU_budget.operations;

  _Thread_State_release( the_thread, &lock_context );

  attr->is_initialized = true;
  attr->contentionscope = PTHREAD_SCOPE_PROCESS;
  attr->cputime_clock_allowed = 1;
  attr->schedparam.sched_priority = _POSIX_Priority_From_core(
    scheduler,
    priority
  );
  attr->schedpolicy =
    _POSIX_Thread_Translate_to_sched_policy( cpu_budget_operations );

  return _POSIX_Get_error( status );
}
