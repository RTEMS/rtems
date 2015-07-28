/**
 * @file
 *
 * @brief Private Support Information for POSIX Threads
 * @ingroup POSIX_PTHREADS Private Threads
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
#include <stdio.h>

#include <errno.h>
#include <pthread.h>
#include <limits.h>
#include <assert.h>

#include <rtems/system.h>
#include <rtems/config.h>
#include <rtems/score/apiext.h>
#include <rtems/score/stack.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/threadqimpl.h>
#include <rtems/score/userextimpl.h>
#include <rtems/score/watchdogimpl.h>
#include <rtems/score/wkspace.h>
#include <rtems/posix/cancel.h>
#include <rtems/posix/pthreadimpl.h>
#include <rtems/posix/priorityimpl.h>
#include <rtems/posix/psignalimpl.h>
#include <rtems/posix/config.h>
#include <rtems/posix/keyimpl.h>
#include <rtems/score/timespec.h>
#include <rtems/score/cpusetimpl.h>
#include <rtems/score/assert.h>

/*
 *  The default pthreads attributes structure.
 *
 *  NOTE: Be careful .. if the default attribute set changes,
 *        _POSIX_Threads_Initialize_user_threads will need to be examined.
 */
pthread_attr_t _POSIX_Threads_Default_attributes = {
  .is_initialized  = true,                       /* is_initialized */
  .stackaddr       = NULL,                       /* stackaddr */
  .stacksize       = 0,                          /* stacksize -- will be adjusted to minimum */
  .contentionscope = PTHREAD_SCOPE_PROCESS,      /* contentionscope */
  .inheritsched    = PTHREAD_INHERIT_SCHED,      /* inheritsched */
  .schedpolicy     = SCHED_FIFO,                 /* schedpolicy */
  .schedparam      =
  {                           /* schedparam */
    2,                        /* sched_priority */
    #if defined(_POSIX_SPORADIC_SERVER) || \
        defined(_POSIX_THREAD_SPORADIC_SERVER)
      0,                        /* sched_ss_low_priority */
      { 0L, 0 },                /* sched_ss_repl_period */
      { 0L, 0 },                /* sched_ss_init_budget */
      0                         /* sched_ss_max_repl */
    #endif
  },

  #if HAVE_DECL_PTHREAD_ATTR_SETGUARDSIZE
    .guardsize = 0,                            /* guardsize */
  #endif
  #if defined(_POSIX_THREAD_CPUTIME)
    .cputime_clock_allowed = 1,                        /* cputime_clock_allowed */
  #endif
  .detachstate             = PTHREAD_CREATE_JOINABLE,    /* detachstate */
  #if defined(__RTEMS_HAVE_SYS_CPUSET_H__)
    .affinitysetsize         = 0,
    .affinityset             = NULL,
    .affinitysetpreallocated = {{0x0}}
  #endif
};

static bool _POSIX_Threads_Sporadic_budget_TSR_filter(
  Thread_Control   *the_thread,
  Priority_Control *new_priority,
  void             *arg
)
{
  the_thread->real_priority = *new_priority;

  /*
   * If holding a resource, then do not change it.
   *
   * If this would make them less important, then do not change it.
   */
  return !_Thread_Owns_resources( the_thread ) &&
    _Thread_Priority_less_than( the_thread->current_priority, *new_priority );
}

/*
 *  _POSIX_Threads_Sporadic_budget_TSR
 */
void _POSIX_Threads_Sporadic_budget_TSR(
  Objects_Id      id __attribute__((unused)),
  void           *argument
)
{
  uint32_t            ticks;
  Thread_Control     *the_thread;
  POSIX_API_Control  *api;

  the_thread = argument;

  api = the_thread->API_Extensions[ THREAD_API_POSIX ];

  /* ticks is guaranteed to be at least one */
  ticks = _Timespec_To_ticks( &api->schedparam.sched_ss_init_budget );

  the_thread->cpu_time_budget = ticks;

  _Thread_Change_priority(
    the_thread,
    _POSIX_Priority_To_core( api->schedparam.sched_priority ),
    NULL,
    _POSIX_Threads_Sporadic_budget_TSR_filter,
    true
  );

  /* ticks is guaranteed to be at least one */
  ticks = _Timespec_To_ticks( &api->schedparam.sched_ss_repl_period );

  _Watchdog_Insert_ticks( &api->Sporadic_timer, ticks );
}

static bool _POSIX_Threads_Sporadic_budget_callout_filter(
  Thread_Control   *the_thread,
  Priority_Control *new_priority,
  void             *arg
)
{
  the_thread->real_priority = *new_priority;

  /*
   * If holding a resource, then do not change it.
   *
   * Make sure we are actually lowering it. If they have lowered it
   * to logically lower than sched_ss_low_priority, then we do not want to
   * change it.
   */
  return !_Thread_Owns_resources( the_thread ) &&
    _Thread_Priority_less_than( *new_priority, the_thread->current_priority );
}

/*
 *  _POSIX_Threads_Sporadic_budget_callout
 */
void _POSIX_Threads_Sporadic_budget_callout(
  Thread_Control *the_thread
)
{
  POSIX_API_Control *api;

  api = the_thread->API_Extensions[ THREAD_API_POSIX ];

  /*
   *  This will prevent the thread from consuming its entire "budget"
   *  while at low priority.
   */
  the_thread->cpu_time_budget = UINT32_MAX;

  _Thread_Change_priority(
    the_thread,
    _POSIX_Priority_To_core( api->schedparam.sched_ss_low_priority ),
    NULL,
    _POSIX_Threads_Sporadic_budget_callout_filter,
    true
  );
}

/*
 *  _POSIX_Threads_Create_extension
 *
 *  This method is invoked for each thread created.
 */

static bool _POSIX_Threads_Create_extension(
  Thread_Control *executing __attribute__((unused)),
  Thread_Control *created
)
{
  POSIX_API_Control *api;
  POSIX_API_Control *executing_api;

  api = created->API_Extensions[ THREAD_API_POSIX ];

  /* XXX check all fields are touched */
  _POSIX_Threads_Initialize_attributes( &api->Attributes );
  api->detachstate = _POSIX_Threads_Default_attributes.detachstate;
  api->schedpolicy = _POSIX_Threads_Default_attributes.schedpolicy;
  api->schedparam  = _POSIX_Threads_Default_attributes.schedparam;
  api->schedparam.sched_priority =
     _POSIX_Priority_From_core( created->current_priority );

  /*
   *  POSIX 1003.1 1996, 18.2.2.2
   */
  api->cancelation_requested = 0;
  api->cancelability_state = PTHREAD_CANCEL_ENABLE;
  api->cancelability_type = PTHREAD_CANCEL_DEFERRED;
#ifndef HAVE_STRUCT__PTHREAD_CLEANUP_CONTEXT
  _Chain_Initialize_empty (&api->Cancellation_Handlers);
#else /* HAVE_STRUCT__PTHREAD_CLEANUP_CONTEXT */
  api->last_cleanup_context = NULL;
#endif /* HAVE_STRUCT__PTHREAD_CLEANUP_CONTEXT */

  /*
   *  If the thread is not a posix thread, then all posix signals are blocked
   *  by default.
   *
   *  The check for class == 1 is debug.  Should never really happen.
   */
  api->signals_pending = SIGNAL_EMPTY_MASK;
  if ( _Objects_Get_API( created->Object.id ) == OBJECTS_POSIX_API
       #if defined(RTEMS_DEBUG)
         && _Objects_Get_class( created->Object.id ) == 1
       #endif
  ) {
    executing_api = _Thread_Get_executing()->API_Extensions[ THREAD_API_POSIX ];
    api->signals_blocked = executing_api->signals_blocked;
  } else {
    api->signals_blocked = SIGNAL_ALL_MASK;
  }

  _Thread_Action_initialize(
    &api->Signal_action,
    _POSIX_signals_Action_handler
  );

  _Thread_queue_Initialize( &api->Join_List, THREAD_QUEUE_DISCIPLINE_FIFO );

  _Watchdog_Preinitialize( &api->Sporadic_timer );
  _Watchdog_Initialize(
    &api->Sporadic_timer,
    _POSIX_Threads_Sporadic_budget_TSR,
    created->Object.id,
    created
  );

  return true;
}

static void _POSIX_Threads_Restart_extension(
  Thread_Control *executing,
  Thread_Control *restarted
)
{
  (void) executing;
  _POSIX_Threads_cancel_run( restarted );
}

static void _POSIX_Threads_Terminate_extension(
  Thread_Control *executing
)
{
  Thread_Control     *the_thread;
  POSIX_API_Control  *api;
  void              **value_ptr;

  api = executing->API_Extensions[ THREAD_API_POSIX ];

  /*
   *  Run the POSIX cancellation handlers
   */
  _POSIX_Threads_cancel_run( executing );

  _Thread_Disable_dispatch();

  /*
   *  Wakeup all the tasks which joined with this one
   */
  value_ptr = (void **) executing->Wait.return_argument;

  while ( (the_thread = _Thread_queue_Dequeue( &api->Join_List )) )
      *(void **)the_thread->Wait.return_argument = value_ptr;

  if ( api->schedpolicy == SCHED_SPORADIC )
    _Watchdog_Remove_ticks( &api->Sporadic_timer );

  _Thread_queue_Destroy( &api->Join_List );

  _Thread_Enable_dispatch();
}

/*
 *  _POSIX_Threads_Exitted_extension
 *
 *  This method is invoked each time a thread exits.
 */
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

/*
 *  _POSIX_Threads_Initialize_user_threads
 *
 *  This routine creates and starts all configured user
 *  initialization threads.
 */
static void _POSIX_Threads_Initialize_user_threads( void )
{
  if ( _POSIX_Threads_Initialize_user_threads_p )
    (*_POSIX_Threads_Initialize_user_threads_p)();
}

/*
 *  API Extension control structures
 */
API_extensions_Control _POSIX_Threads_API_extensions = {
  #if defined(FUNCTIONALITY_NOT_CURRENTLY_USED_BY_ANY_API)
    .predriver_hook = NULL,
  #endif
  .postdriver_hook = _POSIX_Threads_Initialize_user_threads
};

User_extensions_Control _POSIX_Threads_User_extensions = {
  { NULL, NULL },
  { { NULL, NULL }, NULL },
  { _POSIX_Threads_Create_extension,          /* create */
    NULL,                                     /* start */
    _POSIX_Threads_Restart_extension,         /* restart */
    NULL,                                     /* delete */
    NULL,                                     /* switch */
    NULL,                                     /* begin */
    _POSIX_Threads_Exitted_extension,         /* exitted */
    NULL,                                     /* fatal */
    _POSIX_Threads_Terminate_extension        /* terminate */
  }
};

/*
 *  _POSIX_Threads_Manager_initialization
 *
 *  This routine initializes all threads manager related data structures.
 */
void _POSIX_Threads_Manager_initialization(void)
{
  #if defined(RTEMS_SMP) && defined(__RTEMS_HAVE_SYS_CPUSET_H__)
    const CPU_set_Control *affinity;
    pthread_attr_t *attr;

    /* Initialize default attribute. */
    attr = &_POSIX_Threads_Default_attributes;

    /*  Initialize the affinity to be the default cpu set for the system */
    affinity = _CPU_set_Default();
    _Assert( affinity->setsize == sizeof( attr->affinitysetpreallocated ) );
    attr->affinityset             = &attr->affinitysetpreallocated;
    attr->affinitysetsize         = affinity->setsize;
    CPU_COPY( attr->affinityset, affinity->set );
  #endif

  _Thread_Initialize_information(
    &_POSIX_Threads_Information, /* object information table */
    OBJECTS_POSIX_API,           /* object API */
    OBJECTS_POSIX_THREADS,       /* object class */
    Configuration_POSIX_API.maximum_threads,
                                 /* maximum objects of this class */
    true,                        /* true if names for this object are strings */
    _POSIX_PATH_MAX              /* maximum length of each object's name */
#if defined(RTEMS_MULTIPROCESSING)
    ,
    false                        /* true if this is a global object class */
#endif
  );

  /*
   *  Add all the extensions for this API
   */
  _User_extensions_Add_API_set( &_POSIX_Threads_User_extensions );

  _API_extensions_Add( &_POSIX_Threads_API_extensions );

  /*
   *  If we supported MP, then here we would ...
   *       Register the MP Process Packet routine.
   */
}
