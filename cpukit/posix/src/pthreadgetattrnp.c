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
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#define  _GNU_SOURCE

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <errno.h>
#include <string.h>

#include <rtems/posix/pthreadimpl.h>
#include <rtems/posix/pthreadattrimpl.h>
#include <rtems/posix/priorityimpl.h>
#include <rtems/score/schedulerimpl.h>
#include <rtems/score/threadimpl.h>

int pthread_getattr_np(
  pthread_t       thread,
  pthread_attr_t *attr
)
{
  Thread_Control               *the_thread;
  ISR_lock_Context              lock_context;
  Thread_CPU_budget_algorithms  budget_algorithm;
  const Scheduler_Control      *scheduler;
  Priority_Control              priority;
  bool                          ok;

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
  ok = _Scheduler_Get_affinity(
    the_thread,
    attr->affinitysetsize,
    attr->affinityset
  );

  budget_algorithm = the_thread->budget_algorithm;

  _Thread_State_release( the_thread, &lock_context );

  attr->is_initialized = true;
  attr->contentionscope = PTHREAD_SCOPE_PROCESS;
  attr->cputime_clock_allowed = 1;
  attr->schedparam.sched_priority = _POSIX_Priority_From_core(
    scheduler,
    priority
  );
  attr->schedpolicy =
    _POSIX_Thread_Translate_to_sched_policy( budget_algorithm );

  return ok ? 0 : EINVAL;
}
