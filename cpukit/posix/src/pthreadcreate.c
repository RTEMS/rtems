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
#include <rtems/posix/pthreadattrimpl.h>
#include <rtems/score/assert.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/apimutex.h>
#include <rtems/score/stackimpl.h>
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
  Thread_Entry_information entry = {
    .adaptor = _Thread_Entry_adaptor_pointer,
    .Kinds = {
      .Pointer = {
        .entry = start_routine,
        .argument = arg
      }
    }
  };
  const pthread_attr_t               *the_attr;
  int                                 normal_prio;
  bool                                valid;
  Priority_Control                    core_normal_prio;
  Thread_CPU_budget_algorithms        budget_algorithm;
  Thread_CPU_budget_algorithm_callout budget_callout;
  bool                                is_fp;
  bool                                status;
  Thread_Control                     *the_thread;
  Thread_Control                     *executing;
  const Scheduler_Control            *scheduler;
  int                                 schedpolicy = SCHED_RR;
  struct sched_param                  schedparam;
  size_t                              stacksize;
  Objects_Name                        name;
  int                                 error;
  ISR_lock_Context                    lock_context;
#if defined(RTEMS_POSIX_API)
  int                                 low_prio;
  Priority_Control                    core_low_prio;
  POSIX_API_Control                  *api;
  const POSIX_API_Control            *executing_api;
#endif

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
  if ( the_attr->stackaddr != NULL ) {
    if ( !_Stack_Is_enough(the_attr->stacksize) ) {
      return EINVAL;
    }

    stacksize = the_attr->stacksize;
  } else {
    stacksize = _POSIX_Threads_Ensure_minimum_stack( the_attr->stacksize );
  }

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
      error = pthread_getschedparam(
        pthread_self(),
        &schedpolicy,
        &schedparam
      );
      _Assert( error == 0 );
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

  error = _POSIX_Thread_Translate_sched_param(
    schedpolicy,
    &schedparam,
    &budget_algorithm,
    &budget_callout
  );
  if ( error != 0 ) {
    return error;
  }

  normal_prio = schedparam.sched_priority;

  scheduler = _Thread_Scheduler_get_home( executing );

  core_normal_prio = _POSIX_Priority_To_core( scheduler, normal_prio, &valid );
  if ( !valid ) {
    return EINVAL;
  }

#if defined(RTEMS_POSIX_API)
  if ( schedpolicy == SCHED_SPORADIC ) {
    low_prio = schedparam.sched_ss_low_priority;
  } else {
    low_prio = normal_prio;
  }

  core_low_prio = _POSIX_Priority_To_core( scheduler, low_prio, &valid );
  if ( !valid ) {
    return EINVAL;
  }
#endif

  if ( the_attr->affinityset == NULL ) {
    return EINVAL;
  }

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
    scheduler,
    the_attr->stackaddr,
    stacksize,
    is_fp,
    core_normal_prio,
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

  if ( the_attr->detachstate == PTHREAD_CREATE_DETACHED ) {
    the_thread->Life.state |= THREAD_LIFE_DETACHED;
  }

  the_thread->Life.state |= THREAD_LIFE_CHANGE_DEFERRED;

  _ISR_lock_ISR_disable( &lock_context );
   status = _Scheduler_Set_affinity(
     the_thread,
     the_attr->affinitysetsize,
     the_attr->affinityset
   );
  _ISR_lock_ISR_enable( &lock_context );
   if ( !status ) {
     _POSIX_Threads_Free( the_thread );
     _RTEMS_Unlock_allocator();
     return EINVAL;
   }

  the_thread->was_created_with_inherited_scheduler =
    ( the_attr->inheritsched == PTHREAD_INHERIT_SCHED );

#if defined(RTEMS_POSIX_API)
  /*
   *  finish initializing the per API structure
   */
  api = the_thread->API_Extensions[ THREAD_API_POSIX ];
  executing_api = executing->API_Extensions[ THREAD_API_POSIX ];

  api->signals_unblocked = executing_api->signals_unblocked;

  _Priority_Node_set_priority( &api->Sporadic.Low_priority, core_low_prio );
  api->Sporadic.sched_ss_repl_period =
    the_attr->schedparam.sched_ss_repl_period;
  api->Sporadic.sched_ss_init_budget =
    the_attr->schedparam.sched_ss_init_budget;
  api->Sporadic.sched_ss_max_repl =
    the_attr->schedparam.sched_ss_max_repl;

  if ( schedpolicy == SCHED_SPORADIC ) {
    _POSIX_Threads_Sporadic_timer( &api->Sporadic.Timer );
  }
#endif

  /*
   *  POSIX threads are allocated and started in one operation.
   */
  _ISR_lock_ISR_disable( &lock_context );
  status = _Thread_Start( the_thread, &entry, &lock_context );

  #if defined(RTEMS_DEBUG)
    /*
     *  _Thread_Start only fails if the thread was in the incorrect state
     *
     *  NOTE: This can only happen if someone slips in and touches the
     *        thread while we are creating it.
     */
    if ( !status ) {
      _POSIX_Threads_Free( the_thread );
      _Objects_Allocator_unlock();
      return EINVAL;
    }
  #endif

  /*
   *  Return the id and indicate we successfully created the thread
   */
  *thread = the_thread->Object.id;

  _Objects_Allocator_unlock();
  return 0;
}
