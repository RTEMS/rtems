/**
 * @file
 *
 * @brief Function Starts a New Thread in The Calling Process
 * @ingroup POSIXAPI
 */

/*
 *  16.1.2 Thread Creation, P1003.1c/Draft 10, p. 144
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <errno.h>

#include <rtems/posix/priorityimpl.h>
#include <rtems/posix/pthreadimpl.h>
#include <rtems/score/cpusetimpl.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/apimutex.h>
#include <rtems/score/stackimpl.h>
#include <rtems/score/watchdogimpl.h>
#include <rtems/score/schedulerimpl.h>


static inline size_t _POSIX_Threads_Ensure_minimum_stack (
  size_t size
)
{
  if ( size >= PTHREAD_MINIMUM_STACK_SIZE )
    return size;
  return PTHREAD_MINIMUM_STACK_SIZE;
}


int pthread_create(
  pthread_t              *thread,
  const pthread_attr_t   *attr,
  void                 *(*start_routine)( void * ),
  void                   *arg
)
{
  const pthread_attr_t               *the_attr;
  Priority_Control                    core_priority;
  Thread_CPU_budget_algorithms        budget_algorithm;
  Thread_CPU_budget_algorithm_callout budget_callout;
  bool                                is_fp;
  bool                                status;
  Thread_Control                     *the_thread;
  Thread_Control                     *executing;
  POSIX_API_Control                  *api;
  int                                 schedpolicy = SCHED_RR;
  struct sched_param                  schedparam;
  Objects_Name                        name;
  int                                 rc;

  if ( !start_routine )
    return EFAULT;

  the_attr = (attr) ? attr : &_POSIX_Threads_Default_attributes;

  if ( !the_attr->is_initialized )
    return EINVAL;

  /*
   *  Core Thread Initialize ensures we get the minimum amount of
   *  stack space if it is allowed to allocate it itself.
   *
   *  NOTE: If the user provides the stack we will let it drop below
   *        twice the minimum.
   */
  if ( the_attr->stackaddr && !_Stack_Is_enough(the_attr->stacksize) )
    return EINVAL;

  #if 0
    int  cputime_clock_allowed;  /* see time.h */
    rtems_set_errno_and_return_minus_one( ENOSYS );
  #endif

  executing = _Thread_Get_executing();

  /*
   *  P1003.1c/Draft 10, p. 121.
   *
   *  If inheritsched is set to PTHREAD_INHERIT_SCHED, then this thread
   *  inherits scheduling attributes from the creating thread.   If it is
   *  PTHREAD_EXPLICIT_SCHED, then scheduling parameters come from the
   *  attributes structure.
   */
  switch ( the_attr->inheritsched ) {
    case PTHREAD_INHERIT_SCHED:
      api = executing->API_Extensions[ THREAD_API_POSIX ];
      schedpolicy = api->schedpolicy;
      schedparam  = api->schedparam;
      break;

    case PTHREAD_EXPLICIT_SCHED:
      schedpolicy = the_attr->schedpolicy;
      schedparam  = the_attr->schedparam;
      break;

    default:
      return EINVAL;
  }

  /*
   *  Check the contentionscope since rtems only supports PROCESS wide
   *  contention (i.e. no system wide contention).
   */
  if ( the_attr->contentionscope != PTHREAD_SCOPE_PROCESS )
    return ENOTSUP;

  /*
   *  Interpret the scheduling parameters.
   */
  if ( !_POSIX_Priority_Is_valid( schedparam.sched_priority ) )
    return EINVAL;

  core_priority = _POSIX_Priority_To_core( schedparam.sched_priority );

  /*
   *  Set the core scheduling policy information.
   */
  rc = _POSIX_Thread_Translate_sched_param(
    schedpolicy,
    &schedparam,
    &budget_algorithm,
    &budget_callout
  );
  if ( rc )
    return rc;

#if defined(RTEMS_SMP)
#if __RTEMS_HAVE_SYS_CPUSET_H__
  status = _CPU_set_Is_valid( the_attr->affinityset, the_attr->affinitysetsize );
  if ( !status )
    return EINVAL;
#endif
#endif

  /*
   *  Currently all POSIX threads are floating point if the hardware
   *  supports it.
   */
  is_fp = true;

  /*
   *  Allocate the thread control block.
   *
   *  NOTE:  Global threads are not currently supported.
   */
  the_thread = _POSIX_Threads_Allocate();
  if ( !the_thread ) {
    _Objects_Allocator_unlock();
    return EAGAIN;
  }

  /*
   *  Initialize the core thread for this task.
   */
  name.name_p = NULL;   /* posix threads don't have a name by default */
  status = _Thread_Initialize(
    &_POSIX_Threads_Information,
    the_thread,
    _Scheduler_Get( executing ),
    the_attr->stackaddr,
    _POSIX_Threads_Ensure_minimum_stack(the_attr->stacksize),
    is_fp,
    core_priority,
    true,                 /* preemptible */
    budget_algorithm,
    budget_callout,
    0,                    /* isr level */
    name                  /* posix threads don't have a name */
  );
  if ( !status ) {
    _POSIX_Threads_Free( the_thread );
    _Objects_Allocator_unlock();
    return EAGAIN;
  }

#if defined(RTEMS_SMP) && __RTEMS_HAVE_SYS_CPUSET_H__
   status = _Scheduler_Set_affinity(
     the_thread,
     the_attr->affinitysetsize,
     the_attr->affinityset
   );
   if ( !status ) {
     _POSIX_Threads_Free( the_thread );
     _RTEMS_Unlock_allocator();
     return EINVAL;
   }
#endif

  /*
   *  finish initializing the per API structure
   */
  api = the_thread->API_Extensions[ THREAD_API_POSIX ];

  _POSIX_Threads_Copy_attributes( &api->Attributes, the_attr );
  api->detachstate = the_attr->detachstate;
  api->schedpolicy = schedpolicy;
  api->schedparam  = schedparam;

  _Thread_Disable_dispatch();

  /*
   *  POSIX threads are allocated and started in one operation.
   */
  status = _Thread_Start(
    the_thread,
    THREAD_START_POINTER,
    start_routine,
    arg,
    0,                    /* unused */
    NULL
  );

  #if defined(RTEMS_DEBUG)
    /*
     *  _Thread_Start only fails if the thread was in the incorrect state
     *
     *  NOTE: This can only happen if someone slips in and touches the
     *        thread while we are creating it.
     */
    if ( !status ) {
      _Thread_Enable_dispatch();
      _POSIX_Threads_Free( the_thread );
      _Objects_Allocator_unlock();
      return EINVAL;
    }
  #endif

  if ( schedpolicy == SCHED_SPORADIC ) {
    _Watchdog_Insert_ticks(
      &api->Sporadic_timer,
      _Timespec_To_ticks( &api->schedparam.sched_ss_repl_period )
    );
  }

  _Thread_Enable_dispatch();

  /*
   *  Return the id and indicate we successfully created the thread
   */
  *thread = the_thread->Object.id;

  _Objects_Allocator_unlock();
  return 0;
}
