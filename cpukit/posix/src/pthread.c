/*
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
  PTHREAD_MINIMUM_STACK_SIZE, /* stacksize */
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
  unsigned32          ticks;
  unsigned32          new_priority;
  Thread_Control     *the_thread;
  POSIX_API_Control  *api;

  the_thread = argument;

  api = the_thread->API_Extensions[ THREAD_API_POSIX ];

  ticks = _POSIX_Timespec_to_interval( &api->schedparam.ss_initial_budget );

  if ( !ticks )
    ticks = 1;

  the_thread->cpu_time_budget = ticks;
  
  new_priority = _POSIX_Priority_To_core( api->ss_high_priority );
  the_thread->real_priority = new_priority;

  if ( the_thread->resource_count == 0 ||
       the_thread->current_priority > new_priority )
    _Thread_Change_priority( the_thread, new_priority, TRUE );

  ticks = _POSIX_Timespec_to_interval( &api->schedparam.ss_replenish_period );

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
  unsigned32         new_priority;

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
 
boolean _POSIX_Threads_Create_extension(
  Thread_Control *executing,
  Thread_Control *created
)
{
  POSIX_API_Control *api;
  POSIX_API_Control *executing_api;
 
  api = _Workspace_Allocate( sizeof( POSIX_API_Control ) );
 
  if ( !api )
    return FALSE;
 
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
  if ( _Objects_Get_class( created->Object.id ) == OBJECTS_POSIX_THREADS ) {
    executing_api = _Thread_Executing->API_Extensions[ THREAD_API_POSIX ];
    api->signals_blocked = api->signals_blocked;
  } else
    api->signals_blocked = 0xffffffff;

/* XXX set signal parameters -- block all signals for non-posix threads */

  _Thread_queue_Initialize(
    &api->Join_List,
    OBJECTS_NO_CLASS,                 /* only used for proxy operations */
    THREAD_QUEUE_DISCIPLINE_FIFO,
    STATES_WAITING_FOR_JOIN_AT_EXIT,
    NULL,                             /* no extract proxy handler */
    0
  );
 
  _Watchdog_Initialize(
    &api->Sporadic_timer,
    _POSIX_Threads_Sporadic_budget_TSR,
    created->Object.id,
    created
  );

  return TRUE;
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
 
  /* XXX run cancellation handlers */

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
  int                                 status;
  unsigned32                          index;
  unsigned32                          maximum;
  posix_initialization_threads_table *user_threads;
  pthread_t                           thread_id;
  pthread_attr_t                      attr;
 
  user_threads = _POSIX_Threads_User_initialization_threads;
  maximum      = _POSIX_Threads_Number_of_initialization_threads;

  if ( !user_threads || maximum == 0 )
    return;
 
  /*
   *  Be careful .. if the default attribute set changes, this may need to.
   *
   *  Setting the attributes explicitly is critical, since we don't want
   *  to inherit the idle tasks attributes. 
   */

  for ( index=0 ; index < maximum ; index++ ) {
    status = pthread_attr_init( &attr );
    assert( !status );
      
    status = pthread_attr_setinheritsched( &attr, PTHREAD_EXPLICIT_SCHED );
    assert( !status );

    status = pthread_attr_setstacksize( &attr, user_threads[ index ].stack_size);
    assert( !status );

    status = pthread_create(
      &thread_id,
      &attr,
      user_threads[ index ].thread_entry,
      NULL
    );
    assert( !status );
  }
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
  unsigned32                          maximum_pthreads,
  unsigned32                          number_of_initialization_threads,
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
    &_POSIX_Threads_Information,
    OBJECTS_POSIX_THREADS,
    FALSE,                               /* does not support global */
    maximum_pthreads,
    sizeof( Thread_Control ),
    FALSE,
    0,                                   /* length is arbitrary for now */
    TRUE                                 /* this class is threads */
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
