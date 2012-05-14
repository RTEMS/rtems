/*
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <pthread.h>
#include <limits.h>

#include <rtems/system.h>
#include <rtems/config.h>
#include <rtems/score/apiext.h>
#include <rtems/score/stack.h>
#include <rtems/score/thread.h>
#include <rtems/score/userext.h>
#include <rtems/score/wkspace.h>
#include <rtems/posix/cancel.h>
#include <rtems/posix/pthread.h>
#include <rtems/posix/priority.h>
#include <rtems/posix/psignal.h>
#include <rtems/posix/config.h>
#include <rtems/posix/key.h>
#include <rtems/posix/time.h>
#include <rtems/score/timespec.h>

/*
 *  The default pthreads attributes structure.
 *
 *  NOTE: Be careful .. if the default attribute set changes,
 *        _POSIX_Threads_Initialize_user_threads will need to be examined.
 */
const pthread_attr_t _POSIX_Threads_Default_attributes = {
  true,                       /* is_initialized */
  NULL,                       /* stackaddr */
  0,                          /* stacksize -- will be adjusted to minimum */
  PTHREAD_SCOPE_PROCESS,      /* contentionscope */
  PTHREAD_INHERIT_SCHED,      /* inheritsched */
  SCHED_FIFO,                 /* schedpolicy */
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
    0,                        /* guardsize */
  #endif
  #if defined(_POSIX_THREAD_CPUTIME)
    1,                        /* cputime_clock_allowed */
  #endif
  PTHREAD_CREATE_JOINABLE,    /* detachstate */
};

/*
 *  _POSIX_Threads_Sporadic_budget_TSR
 */
void _POSIX_Threads_Sporadic_budget_TSR(
  Objects_Id      id __attribute__((unused)),
  void           *argument
)
{
  uint32_t            ticks;
  uint32_t            new_priority;
  Thread_Control     *the_thread;
  POSIX_API_Control  *api;

  the_thread = argument;

  api = the_thread->API_Extensions[ THREAD_API_POSIX ];

  /* ticks is guaranteed to be at least one */
  ticks = _Timespec_To_ticks( &api->schedparam.sched_ss_init_budget );

  the_thread->cpu_time_budget = ticks;

  new_priority = _POSIX_Priority_To_core( api->schedparam.sched_priority );
  the_thread->real_priority = new_priority;

  /*
   *  If holding a resource, then do not change it.
   */
  #if 0
    printk( "TSR %d %d %d\n", the_thread->resource_count,
        the_thread->current_priority, new_priority );
  #endif
  if ( the_thread->resource_count == 0 ) {
    /*
     *  If this would make them less important, then do not change it.
     */
    if ( the_thread->current_priority > new_priority ) {
      _Thread_Change_priority( the_thread, new_priority, true );
      #if 0
        printk( "raise priority\n" );
      #endif
    }
  }

  /* ticks is guaranteed to be at least one */
  ticks = _Timespec_To_ticks( &api->schedparam.sched_ss_repl_period );

  _Watchdog_Insert_ticks( &api->Sporadic_timer, ticks );
}

/*
 *  _POSIX_Threads_Sporadic_budget_callout
 */
void _POSIX_Threads_Sporadic_budget_callout(
  Thread_Control *the_thread
)
{
  POSIX_API_Control *api;
  uint32_t           new_priority;

  api = the_thread->API_Extensions[ THREAD_API_POSIX ];

  /*
   *  This will prevent the thread from consuming its entire "budget"
   *  while at low priority.
   */
  the_thread->cpu_time_budget = 0xFFFFFFFF; /* XXX should be based on MAX_U32 */

  new_priority = _POSIX_Priority_To_core(api->schedparam.sched_ss_low_priority);
  the_thread->real_priority = new_priority;

  /*
   *  If holding a resource, then do not change it.
   */
  #if 0
    printk( "callout %d %d %d\n", the_thread->resource_count,
	the_thread->current_priority, new_priority );
  #endif
  if ( the_thread->resource_count == 0 ) {
    /*
     *  Make sure we are actually lowering it. If they have lowered it
     *  to logically lower than sched_ss_low_priority, then we do not want to
     *  change it.
     */
    if ( the_thread->current_priority < new_priority ) {
      _Thread_Change_priority( the_thread, new_priority, true );
      #if 0
        printk( "lower priority\n" );
      #endif
    }
  }
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

  api = _Workspace_Allocate( sizeof( POSIX_API_Control ) );

  if ( !api )
    return false;

  created->API_Extensions[ THREAD_API_POSIX ] = api;

  /* XXX check all fields are touched */
  api->Attributes  = _POSIX_Threads_Default_attributes;
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
  _Chain_Initialize_empty (&api->Cancellation_Handlers);

  /*
   *  If the thread is not a posix thread, then all posix signals are blocked
   *  by default.
   *
   *  The check for class == 1 is debug.  Should never really happen.
   */

  /* XXX use signal constants */
  api->signals_pending = 0;
  if ( _Objects_Get_API( created->Object.id ) == OBJECTS_POSIX_API
       #if defined(RTEMS_DEBUG)
         && _Objects_Get_class( created->Object.id ) == 1
       #endif
  ) {
    executing_api = _Thread_Executing->API_Extensions[ THREAD_API_POSIX ];
    api->signals_blocked = executing_api->signals_blocked;
  } else {
    api->signals_blocked = 0xffffffff;
  }

  _Thread_queue_Initialize(
    &api->Join_List,
    THREAD_QUEUE_DISCIPLINE_FIFO,
    STATES_WAITING_FOR_JOIN_AT_EXIT | STATES_INTERRUPTIBLE_BY_SIGNAL,
    0
  );

  _Watchdog_Initialize(
    &api->Sporadic_timer,
    _POSIX_Threads_Sporadic_budget_TSR,
    created->Object.id,
    created
  );

  return true;
}

/*
 *  _POSIX_Threads_Delete_extension
 *
 *  This method is invoked for each thread deleted.
 */
static void _POSIX_Threads_Delete_extension(
  Thread_Control *executing __attribute__((unused)),
  Thread_Control *deleted
)
{
  Thread_Control     *the_thread;
  POSIX_API_Control  *api;
  void              **value_ptr;

  api = deleted->API_Extensions[ THREAD_API_POSIX ];

  /*
   *  Run the POSIX cancellation handlers
   */
  _POSIX_Threads_cancel_run( deleted );

  /*
   *  Run all the key destructors
   */
  _POSIX_Keys_Run_destructors( deleted );

  /*
   *  Wakeup all the tasks which joined with this one
   */
  value_ptr = (void **) deleted->Wait.return_argument;

  while ( (the_thread = _Thread_queue_Dequeue( &api->Join_List )) )
      *(void **)the_thread->Wait.return_argument = value_ptr;

  if ( api->schedpolicy == SCHED_SPORADIC )
    (void) _Watchdog_Remove( &api->Sporadic_timer );

  deleted->API_Extensions[ THREAD_API_POSIX ] = NULL;

  _Workspace_Free( api );
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
  { NULL, NULL },
  #if defined(FUNCTIONALITY_NOT_CURRENTLY_USED_BY_ANY_API)
    NULL,                                   /* predriver */
  #endif
  _POSIX_Threads_Initialize_user_threads,   /* postdriver */
  _POSIX_signals_Post_switch_extension,     /* post switch */
};

User_extensions_Control _POSIX_Threads_User_extensions = {
  { NULL, NULL },
  { { NULL, NULL }, NULL },
  { _POSIX_Threads_Create_extension,          /* create */
    NULL,                                     /* start */
    NULL,                                     /* restart */
    _POSIX_Threads_Delete_extension,          /* delete */
    NULL,                                     /* switch */
    NULL,                                     /* begin */
    _POSIX_Threads_Exitted_extension,         /* exitted */
    NULL                                      /* fatal */
  }
};

/*
 *  _POSIX_Threads_Manager_initialization
 *
 *  This routine initializes all threads manager related data structures.
 */
void _POSIX_Threads_Manager_initialization(void)
{
  _Objects_Initialize_information(
    &_POSIX_Threads_Information, /* object information table */
    OBJECTS_POSIX_API,           /* object API */
    OBJECTS_POSIX_THREADS,       /* object class */
    Configuration_POSIX_API.maximum_threads,
                                 /* maximum objects of this class */
    sizeof( Thread_Control ),
                                 /* size of this object's control block */
    true,                        /* true if names for this object are strings */
    _POSIX_PATH_MAX              /* maximum length of each object's name */
#if defined(RTEMS_MULTIPROCESSING)
    ,
    false,                       /* true if this is a global object class */
    NULL                         /* Proxy extraction support callout */
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
