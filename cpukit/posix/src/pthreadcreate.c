/*
 *  16.1.2 Thread Creation, P1003.1c/Draft 10, p. 144
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <errno.h>

#include <rtems/system.h>
#include <rtems/score/thread.h>
#include <rtems/posix/pthread.h>
#include <rtems/posix/priority.h>
#include <rtems/posix/time.h>

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
  boolean                             is_fp;
  boolean                             status;
  Thread_Control                     *the_thread;
  POSIX_API_Control                  *api;
  int                                 schedpolicy = SCHED_RR;
  struct sched_param                  schedparam;

  the_attr = (attr) ? attr : &_POSIX_Threads_Default_attributes;

  if ( !the_attr->is_initialized )
    return EINVAL;

  /*
   *  Core Thread Initialize insures we get the minimum amount of
   *  stack space if it is allowed to allocate it itself.
   */

  if ( the_attr->stackaddr && !_Stack_Is_enough( the_attr->stacksize ) )
    return EINVAL;

#if 0
  int  cputime_clock_allowed;  /* see time.h */
  POSIX_NOT_IMPLEMENTED();
#endif

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
      api = _Thread_Executing->API_Extensions[ THREAD_API_POSIX ];
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

  budget_callout = NULL;
  budget_algorithm = THREAD_CPU_BUDGET_ALGORITHM_NONE;

  switch ( schedpolicy ) {
    case SCHED_OTHER:
      budget_algorithm = THREAD_CPU_BUDGET_ALGORITHM_RESET_TIMESLICE;
      break;
     
    case SCHED_FIFO:
      budget_algorithm = THREAD_CPU_BUDGET_ALGORITHM_NONE;
      break;

    case SCHED_RR:
      budget_algorithm = THREAD_CPU_BUDGET_ALGORITHM_EXHAUST_TIMESLICE;
      break;

    case SCHED_SPORADIC:
      budget_algorithm  = THREAD_CPU_BUDGET_ALGORITHM_CALLOUT;
      budget_callout = _POSIX_Threads_Sporadic_budget_callout;
  
      if ( _POSIX_Timespec_to_interval( &schedparam.ss_replenish_period ) <
           _POSIX_Timespec_to_interval( &schedparam.ss_initial_budget ) )
        return EINVAL;

      if ( !_POSIX_Priority_Is_valid( schedparam.ss_low_priority ) )
        return EINVAL;

      break;

    default:
      return EINVAL;
  }

  /*
   *  Currently all POSIX threads are floating point if the hardware 
   *  supports it.
   */


#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
  is_fp = TRUE;
#else
  is_fp = FALSE;
#endif

  /*
   *  Disable dispatch for protection
   */
 
  _Thread_Disable_dispatch();
 
  /*
   *  Allocate the thread control block.
   *
   *  NOTE:  Global threads are not currently supported.
   */

  the_thread = _POSIX_Threads_Allocate();

  if ( !the_thread ) {
    _Thread_Enable_dispatch();
    return EAGAIN;
  }

  /*
   *  Initialize the core thread for this task.
   */
 
  status = _Thread_Initialize(
    &_POSIX_Threads_Information,
    the_thread,
    the_attr->stackaddr,
    the_attr->stacksize,
    is_fp,
    core_priority,
    TRUE,                 /* preemptible */
    budget_algorithm,
    budget_callout,
    0,                    /* isr level */
    NULL                  /* posix threads don't have a name */
  );
 
  if ( !status ) {
    _POSIX_Threads_Free( the_thread );
    _Thread_Enable_dispatch();
    return EAGAIN;
  }

  /*
   *  finish initializing the per API structure
   */

  
  api = the_thread->API_Extensions[ THREAD_API_POSIX ];

  api->Attributes  = *the_attr;
  api->detachstate = the_attr->detachstate;
  api->schedpolicy = schedpolicy;
  api->schedparam  = schedparam;

  /*
   *  This insures we evaluate the process-wide signals pending when we
   *  first run.
   *
   *  NOTE:  Since the thread starts with all unblocked, this is necessary.
   */

  the_thread->do_post_task_switch_extension = TRUE;

  /*
   *  POSIX threads are allocated and started in one operation.
   */

  status = _Thread_Start(
    the_thread,
    THREAD_START_POINTER,
    start_routine,
    arg,
    0                     /* unused */
  );

  if ( schedpolicy == SCHED_SPORADIC ) {
    _Watchdog_Insert_ticks(
      &api->Sporadic_timer,
      _POSIX_Timespec_to_interval( &api->schedparam.ss_replenish_period )
    );
  }

  /*
   *  _Thread_Start only fails if the thread was in the incorrect state
   */

  if ( !status ) {
    _POSIX_Threads_Free( the_thread );
    _Thread_Enable_dispatch();
    return EINVAL;
  }

  /*
   *  Return the id and indicate we successfully created the thread
   */

  *thread = the_thread->Object.id;

 _Thread_Enable_dispatch();

 return 0;
}

