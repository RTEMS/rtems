/*
 *  COPYRIGHT (c) 1989-2008.
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

#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <limits.h>

#include <rtems/system.h>
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

/*PAGE
 *
 *  The default pthreads attributes structure.
 *
 *  NOTE: Be careful .. if the default attribute set changes,
 *        _POSIX_Threads_Initialize_user_threads will need to be examined.
 *
 */

const pthread_attr_t _POSIX_Threads_Default_attributes = {
  TRUE,                       /* is_initialized */
  NULL,                       /* stackaddr */
  0,                          /* stacksize -- will be adjusted to minimum */
  PTHREAD_SCOPE_PROCESS,      /* contentionscope */
  PTHREAD_INHERIT_SCHED,      /* inheritsched */
  SCHED_FIFO,                 /* schedpolicy */
  {                           /* schedparam */
    2,                        /* sched_priority */
    0,                        /* ss_low_priority */
    { 0L, 0 },                /* ss_replenish_period */
    { 0L, 0 }                 /* ss_initial_budget */
  },
  PTHREAD_CREATE_JOINABLE,    /* detachstate */
  1                           /* cputime_clock_allowed */
};

/*PAGE
 *
 *  _POSIX_Threads_Sporadic_budget_TSR
 */

void _POSIX_Threads_Sporadic_budget_TSR(
  Objects_Id      id,
  void           *argument
)
{
  uint32_t            ticks;
  uint32_t            new_priority;
  Thread_Control     *the_thread;
  POSIX_API_Control  *api;

  the_thread = argument;

  api = the_thread->API_Extensions[ THREAD_API_POSIX ];

  ticks = _Timespec_To_ticks( &api->schedparam.ss_initial_budget );

  if ( !ticks )
    ticks = 1;

  the_thread->cpu_time_budget = ticks;

  new_priority = _POSIX_Priority_To_core( api->ss_high_priority );
  the_thread->real_priority = new_priority;

  if ( the_thread->resource_count == 0 ||
       the_thread->current_priority > new_priority )
    _Thread_Change_priority( the_thread, new_priority, TRUE );

  ticks = _Timespec_To_ticks( &api->schedparam.ss_replenish_period );

  if ( !ticks )
    ticks = 1;

  _Watchdog_Insert_ticks( &api->Sporadic_timer, ticks );
}

/*PAGE
 *
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

  new_priority = _POSIX_Priority_To_core( api->schedparam.ss_low_priority );
  the_thread->real_priority = new_priority;

 if ( the_thread->resource_count == 0 ||
      the_thread->current_priority > new_priority )
    _Thread_Change_priority( the_thread, new_priority, TRUE );
}

/*PAGE
 *
 *  _POSIX_Threads_Create_extension
 *
 *  XXX
 */

bool _POSIX_Threads_Create_extension(
  Thread_Control *executing,
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
   */

  /* XXX use signal constants */
  api->signals_pending = 0;
  if ( _Objects_Get_API( created->Object.id ) == OBJECTS_POSIX_API &&
       _Objects_Get_class( created->Object.id ) == 1 ) {
    executing_api = _Thread_Executing->API_Extensions[ THREAD_API_POSIX ];
    api->signals_blocked = executing_api->signals_blocked;
  } else {
    api->signals_blocked = 0xffffffff;
  }

  _Thread_queue_Initialize(
    &api->Join_List,
    THREAD_QUEUE_DISCIPLINE_FIFO,
    STATES_WAITING_FOR_JOIN_AT_EXIT,
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

/*PAGE
 *
 *  _POSIX_Threads_Delete_extension
 */

User_extensions_routine _POSIX_Threads_Delete_extension(
  Thread_Control *executing,
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

  (void) _Workspace_Free( api );
}

/*
 *
 *  _POSIX_Threads_Exitted_extension
 */

User_extensions_routine _POSIX_Threads_Exitted_extension(
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

/*PAGE
 *
 *  _POSIX_Threads_Initialize_user_threads
 *
 *  This routine creates and starts all configured user
 *  initialzation threads.
 *
 *  Input parameters: NONE
 *
 *  Output parameters:  NONE
 */

void _POSIX_Threads_Initialize_user_threads( void )
{
  if ( _POSIX_Threads_Initialize_user_threads_p )
    (*_POSIX_Threads_Initialize_user_threads_p)();
}

/*PAGE
 *
 *  API Extension control structures
 */

API_extensions_Control _POSIX_Threads_API_extensions = {
  { NULL, NULL },
  NULL,                                     /* predriver */
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

/*PAGE
 *
 *  _POSIX_Threads_Manager_initialization
 *
 *  This routine initializes all threads manager related data structures.
 *
 *  Input parameters:
 *    maximum_pthreads - maximum configured pthreads
 *
 *  Output parameters:  NONE
 */

void _POSIX_Threads_Manager_initialization(
  uint32_t                            maximum_pthreads,
  uint32_t                            number_of_initialization_threads,
  posix_initialization_threads_table *user_threads

)
{
  _POSIX_Threads_Number_of_initialization_threads =
                                           number_of_initialization_threads;
  _POSIX_Threads_User_initialization_threads = user_threads;

  /*
   *  There may not be any POSIX initialization threads configured.
   */

#if 0
  if ( user_threads == NULL || number_of_initialization_threads == 0 )
    _Internal_error_Occurred( INTERNAL_ERROR_POSIX_API, TRUE, EINVAL );
#endif

  _Objects_Initialize_information(
    &_POSIX_Threads_Information, /* object information table */
    OBJECTS_POSIX_API,           /* object API */
    OBJECTS_POSIX_THREADS,       /* object class */
    maximum_pthreads,            /* maximum objects of this class */
    sizeof( Thread_Control ),
                                 /* size of this object's control block */
    TRUE,                        /* TRUE if names for this object are strings */
    _POSIX_PATH_MAX             /* maximum length of each object's name */
#if defined(RTEMS_MULTIPROCESSING)
    ,
    FALSE,                       /* TRUE if this is a global object class */
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
