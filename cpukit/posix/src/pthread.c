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
#include <rtems/sysinit.h>
#include <rtems/score/stack.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/threadqimpl.h>
#include <rtems/score/userextimpl.h>
#include <rtems/score/wkspace.h>
#include <rtems/posix/pthreadimpl.h>
#include <rtems/posix/priorityimpl.h>
#include <rtems/posix/psignalimpl.h>
#include <rtems/posix/config.h>
#include <rtems/posix/keyimpl.h>
#include <rtems/score/assert.h>
#include <rtems/score/cpusetimpl.h>
#include <rtems/score/schedulerimpl.h>

Thread_Information _POSIX_Threads_Information;

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
  .affinitysetsize         = 0,
  .affinityset             = NULL,
  .affinitysetpreallocated = {{0x0}}
};

void _POSIX_Threads_Sporadic_timer( Watchdog_Control *watchdog )
{
  POSIX_API_Control    *api;
  Thread_Control       *the_thread;
  Thread_queue_Context  queue_context;

  api = RTEMS_CONTAINER_OF( watchdog, POSIX_API_Control, Sporadic.Timer );
  the_thread = api->thread;

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

  _Watchdog_Per_CPU_remove_relative( &api->Sporadic.Timer );
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

/*
 *  _POSIX_Threads_Create_extension
 *
 *  This method is invoked for each thread created.
 */

static bool _POSIX_Threads_Create_extension(
  Thread_Control *executing RTEMS_UNUSED,
  Thread_Control *created
)
{
  POSIX_API_Control *api;
  POSIX_API_Control *executing_api;

  api = created->API_Extensions[ THREAD_API_POSIX ];

  /* XXX check all fields are touched */
  api->thread = created;
  _POSIX_Threads_Initialize_attributes( &api->Attributes );
  api->Attributes.schedparam.sched_priority = _POSIX_Priority_From_core(
    _Thread_Scheduler_get_home( created ),
    _Thread_Get_priority( created )
  );

  /*
   *  If the thread is not a posix thread, then all posix signals are blocked
   *  by default.
   *
   *  The check for class == 1 is debug.  Should never really happen.
   */
  RTEMS_STATIC_ASSERT( SIGNAL_EMPTY_MASK == 0, signals_pending );
  if ( _Objects_Get_API( created->Object.id ) == OBJECTS_POSIX_API
       #if defined(RTEMS_DEBUG)
         && _Objects_Get_class( created->Object.id ) == 1
       #endif
  ) {
    executing_api = _Thread_Get_executing()->API_Extensions[ THREAD_API_POSIX ];
    api->signals_unblocked = executing_api->signals_unblocked;
  }

  _Watchdog_Preinitialize( &api->Sporadic.Timer, _Per_CPU_Get_by_index( 0 ) );
  _Watchdog_Initialize( &api->Sporadic.Timer, _POSIX_Threads_Sporadic_timer );

  return true;
}

static void _POSIX_Threads_Terminate_extension( Thread_Control *executing )
{
  POSIX_API_Control *api;
  ISR_lock_Context   lock_context;

  api = executing->API_Extensions[ THREAD_API_POSIX ];

  _Thread_State_acquire( executing, &lock_context );

  if ( api->Attributes.schedpolicy == SCHED_SPORADIC ) {
    _Watchdog_Per_CPU_remove_relative( &api->Sporadic.Timer );
  }

  _Thread_State_release( executing, &lock_context );
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

User_extensions_Control _POSIX_Threads_User_extensions = {
  .Callouts = {
    .thread_create    = _POSIX_Threads_Create_extension,
    .thread_exitted   = _POSIX_Threads_Exitted_extension,
    .thread_terminate = _POSIX_Threads_Terminate_extension
  }
};

/*
 *  _POSIX_Threads_Manager_initialization
 *
 *  This routine initializes all threads manager related data structures.
 */
static void _POSIX_Threads_Manager_initialization(void)
{
  #if defined(RTEMS_SMP)
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
  );

  /*
   *  Add all the extensions for this API
   */
  _User_extensions_Add_API_set( &_POSIX_Threads_User_extensions );

  /*
   *  If we supported MP, then here we would ...
   *       Register the MP Process Packet routine.
   */
}

RTEMS_SYSINIT_ITEM(
  _POSIX_Threads_Manager_initialization,
  RTEMS_SYSINIT_POSIX_THREADS,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
