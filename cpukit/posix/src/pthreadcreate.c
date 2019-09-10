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
#if defined(RTEMS_POSIX_API)
#include <rtems/posix/psignalimpl.h>
#endif
#include <rtems/posix/pthreadimpl.h>
#include <rtems/posix/pthreadattrimpl.h>
#include <rtems/score/assert.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/apimutex.h>
#include <rtems/score/stackimpl.h>
#include <rtems/score/schedulerimpl.h>
#include <rtems/score/userextimpl.h>
#include <rtems/sysinit.h>

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
      (void) error; /* error only used when debug enabled */
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

#if defined(RTEMS_POSIX_API)
void _POSIX_Threads_Sporadic_timer( Watchdog_Control *watchdog )
{
  POSIX_API_Control    *api;
  Thread_Control       *the_thread;
  Thread_queue_Context  queue_context;

  api = RTEMS_CONTAINER_OF( watchdog, POSIX_API_Control, Sporadic.Timer );
  the_thread = api->Sporadic.thread;

  _Thread_queue_Context_initialize( &queue_context );
  _Thread_queue_Context_clear_priority_updates( &queue_context );
  _Thread_Wait_acquire( the_thread, &queue_context );

  if ( _Priority_Node_is_active( &api->Sporadic.Low_priority ) ) {
    _Thread_Priority_add(
      the_thread,
      &the_thread->Real_priority,
      &queue_context
    );
    _Thread_Priority_remove(
      the_thread,
      &api->Sporadic.Low_priority,
      &queue_context
    );
    _Priority_Node_set_inactive( &api->Sporadic.Low_priority );
  }

  _Watchdog_Per_CPU_remove_ticks( &api->Sporadic.Timer );
  _POSIX_Threads_Sporadic_timer_insert( the_thread, api );

  _Thread_Wait_release( the_thread, &queue_context );
  _Thread_Priority_update( &queue_context );
}

void _POSIX_Threads_Sporadic_budget_callout( Thread_Control *the_thread )
{
  POSIX_API_Control    *api;
  Thread_queue_Context  queue_context;

  api = the_thread->API_Extensions[ THREAD_API_POSIX ];

  _Thread_queue_Context_initialize( &queue_context );
  _Thread_queue_Context_clear_priority_updates( &queue_context );
  _Thread_Wait_acquire( the_thread, &queue_context );

  /*
   *  This will prevent the thread from consuming its entire "budget"
   *  while at low priority.
   */
  the_thread->cpu_time_budget = UINT32_MAX;

  if ( !_Priority_Node_is_active( &api->Sporadic.Low_priority ) ) {
    _Thread_Priority_add(
      the_thread,
      &api->Sporadic.Low_priority,
      &queue_context
    );
    _Thread_Priority_remove(
      the_thread,
      &the_thread->Real_priority,
      &queue_context
    );
  }

  _Thread_Wait_release( the_thread, &queue_context );
  _Thread_Priority_update( &queue_context );
}

static bool _POSIX_Threads_Create_extension(
  Thread_Control *executing,
  Thread_Control *created
)
{
  POSIX_API_Control *api;

  api = created->API_Extensions[ THREAD_API_POSIX ];

  api->Sporadic.thread = created;
  _Watchdog_Preinitialize( &api->Sporadic.Timer, _Per_CPU_Get_by_index( 0 ) );
  _Watchdog_Initialize( &api->Sporadic.Timer, _POSIX_Threads_Sporadic_timer );
  _Priority_Node_set_inactive( &api->Sporadic.Low_priority );

#if defined(RTEMS_POSIX_API)
  /*
   * There are some subtle rules which need to be followed for
   * the value of the created thread's signal mask. Because signals
   * are part of C99 and enhanced by POSIX, both Classic API tasks
   * and POSIX threads have to have them enabled.
   *
   * + Internal system threads should have no signals enabled. They
   *   have no business executing user signal handlers -- especially IDLE.
   * + The initial signal mask for other threads needs to follow the
   *   implication of a pure C99 environment which only has the methods
   *   raise() and signal(). This implies that all signals are unmasked
   *   until the thread explicitly uses a POSIX methods to block some.
   *   This applies to both Classic tasks and POSIX threads created
   *   as initalization tasks/threads (e.g. before the system is up).
   * + After the initial threads are created, the signal mask should
   *   be inherited from the creator.
   *
   * NOTE: The default signal mask does not matter for any application
   *       that does not use POSIX signals.
   */
  if ( _Objects_Get_API(created->Object.id) == OBJECTS_INTERNAL_API ) {
      /*
       * Ensure internal (especially IDLE) is handled first.
       *
       * Block signals for all internal threads -- especially IDLE.
       */
      api->signals_unblocked = 0;
  } else if ( _Objects_Get_API(executing->Object.id) == OBJECTS_INTERNAL_API ) {
      /*
       * Threads being created while an internal thread is executing
       * should only happen for the initialization threads/tasks.
       *
       * Default state (signals unblocked) for all Initialization tasks
       * and POSIX threads. We should not inherit from IDLE which is
       * what appears to be executing during initialization.
       */
      api->signals_unblocked = SIGNAL_ALL_MASK;
  } else {
    const POSIX_API_Control            *executing_api;
    /*
     * RTEMS is running so follow the POSIX rules to inherit the signal mask.
     */ 
    executing_api = executing->API_Extensions[ THREAD_API_POSIX ];
    api->signals_unblocked = executing_api->signals_unblocked;
  }
#endif
  return true;
}

static void _POSIX_Threads_Terminate_extension( Thread_Control *executing )
{
  POSIX_API_Control *api;
  ISR_lock_Context   lock_context;

  api = executing->API_Extensions[ THREAD_API_POSIX ];

  _Thread_State_acquire( executing, &lock_context );
  _Watchdog_Per_CPU_remove_ticks( &api->Sporadic.Timer );
  _Thread_State_release( executing, &lock_context );
}
#endif

static void _POSIX_Threads_Exitted_extension(
  Thread_Control *executing
)
{
  /*
   *  If the executing thread was not created with the POSIX API, then this
   *  API do not get to define its exit behavior.
   */
  if ( _Objects_Get_API( executing->Object.id ) == OBJECTS_POSIX_API )
    pthread_exit( executing->Wait.return_argument );
}

static User_extensions_Control _POSIX_Threads_User_extensions = {
  .Callouts = {
#if defined(RTEMS_POSIX_API)
    .thread_create    = _POSIX_Threads_Create_extension,
    .thread_terminate = _POSIX_Threads_Terminate_extension,
#endif
    .thread_exitted   = _POSIX_Threads_Exitted_extension
  }
};

static void _POSIX_Threads_Manager_initialization( void )
{
  _Thread_Initialize_information( &_POSIX_Threads_Information );
  _User_extensions_Add_API_set( &_POSIX_Threads_User_extensions );
}

RTEMS_SYSINIT_ITEM(
  _POSIX_Threads_Manager_initialization,
  RTEMS_SYSINIT_POSIX_THREADS,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
