/*
 *  $Id$
 */

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
#include <rtems/posix/pthread.h>
#include <rtems/posix/priority.h>
#include <rtems/posix/config.h>

/*PAGE
 *
 *  The default pthreads attributes structure.
 */
 
const pthread_attr_t _POSIX_Threads_Default_attributes = {
  TRUE,                    /* is_initialized */
  0,                       /* stackaddr */
  STACK_MINIMUM_SIZE,      /* stacksize */
  PTHREAD_SCOPE_PROCESS,   /* contentionscope */
  PTHREAD_INHERIT_SCHED,   /* inheritsched */
  SCHED_FIFO,              /* schedpolicy */
  {                        /* schedparam */
    128,                   /* sched_priority */
    0,                     /* ss_low_priority */
    { 0L, 0 },             /* ss_replenish_period */
    { 0L, 0 }              /* ss_initial_budget */
  },
  PTHREAD_CREATE_DETACHED, /* detachstate */
  1                        /* cputime_clock_allowed */
};

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
 
  api = _Workspace_Allocate( sizeof( POSIX_API_Control ) );
 
  if ( !api )
    return FALSE;
 
  created->API_Extensions[ THREAD_API_POSIX ] = api;
 
  /* XXX something should go here */

  return TRUE;
}
 
/*PAGE
 *
 *  _POSIX_Threads_Delete_extension
 *
 *  XXX
 */
 
User_extensions_routine _POSIX_Threads_Delete_extension(
  Thread_Control *executing,
  Thread_Control *deleted
)
{
  (void) _Workspace_Free( deleted->API_Extensions[ THREAD_API_POSIX ] );
 
  deleted->API_Extensions[ THREAD_API_POSIX ] = NULL;

  /* XXX run _POSIX_Keys_Run_destructors here? */
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
 
  user_threads = _POSIX_Threads_User_initialization_threads;
  maximum      = _POSIX_Threads_Number_of_initialization_threads;

  if ( !user_threads || maximum == 0 )
    return;
 
  for ( index=0 ; index < maximum ; index++ ) {
    status = pthread_create(
      &thread_id,
      NULL,
      user_threads[ index ].entry,
      NULL
    );
    assert( !status );
  }
}

API_extensions_Control _POSIX_Threads_API_extensions = {
  { NULL, NULL },
  NULL,                                     /* predriver */
  _POSIX_Threads_Initialize_user_threads,   /* postdriver */
  NULL,                                     /* post switch */
};
 
User_extensions_Control _POSIX_Threads_User_extensions = {
  { NULL, NULL },
  { _POSIX_Threads_Create_extension,          /* create */
    NULL,                                     /* start */
    NULL,                                     /* restart */
    _POSIX_Threads_Delete_extension,          /* delete */
    NULL,                                     /* switch */
    NULL,                                     /* begin */
    NULL,                                     /* exitted */
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
    TRUE,
    5,                                   /* length is arbitrary for now */
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

/*PAGE
 *
 *  3.1.3 Register Fork Handlers, P1003.1c/Draft 10, P1003.1c/Draft 10, p. 27
 *
 *  RTEMS does not support processes, so we fall under this and do not
 *  provide this routine:
 *
 *  "Either the implementation shall support the pthread_atfork() function
 *   as described above or the pthread_atfork() funciton shall not be
 *   provided."
 */

/*PAGE
 *
 *  13.5.1 Thread Creation Scheduling Attributes, P1003.1c/Draft 10, p. 120
 */

int pthread_attr_setscope(
  pthread_attr_t  *attr,
  int              contentionscope
)
{
  if ( !attr || !attr->is_initialized )
    return EINVAL;

  switch ( contentionscope ) {
    case PTHREAD_SCOPE_PROCESS:
    case PTHREAD_SCOPE_SYSTEM:
      attr->contentionscope = contentionscope;
      return 0;

    default:
      return EINVAL;
  }
}

/*PAGE
 *
 *  13.5.1 Thread Creation Scheduling Attributes, P1003.1c/Draft 10, p. 120
 */

int pthread_attr_getscope(
  const pthread_attr_t  *attr,
  int                   *contentionscope
)
{
  if ( !attr || !attr->is_initialized )
    return EINVAL;

  *contentionscope = attr->contentionscope;
  return 0;
}

/*PAGE
 *
 *  13.5.1 Thread Creation Scheduling Attributes, P1003.1c/Draft 10, p. 120
 */

int pthread_attr_setinheritsched(
  pthread_attr_t  *attr,
  int              inheritsched
)
{
  if ( !attr || !attr->is_initialized )
    return EINVAL;

  switch ( inheritsched ) {
    case PTHREAD_INHERIT_SCHED:
    case PTHREAD_EXPLICIT_SCHED:
      attr->inheritsched = inheritsched;
      return 0;

    default:
      return EINVAL;
  }
}

/*PAGE
 *
 *  13.5.1 Thread Creation Scheduling Attributes, P1003.1c/Draft 10, p. 120
 */

int pthread_attr_getinheritsched(
  const pthread_attr_t  *attr,
  int                   *inheritsched
)
{
  if ( !attr || !attr->is_initialized )
    return EINVAL;

  *inheritsched = attr->inheritsched;
  return 0;
}

/*PAGE
 *
 *  13.5.1 Thread Creation Scheduling Attributes, P1003.1c/Draft 10, p. 120
 */

int pthread_attr_setschedpolicy(
  pthread_attr_t  *attr,
  int              policy
)
{
  if ( !attr || !attr->is_initialized )
    return EINVAL;

  switch ( policy ) {
    case SCHED_OTHER:
    case SCHED_FIFO:
    case SCHED_RR:
    case SCHED_SPORADIC:
      attr->schedpolicy = policy;
      return 0;
 
    default:
      return EINVAL;
  }
}

/*PAGE
 *
 *  13.5.1 Thread Creation Scheduling Attributes, P1003.1c/Draft 10, p. 120
 */

int pthread_attr_getschedpolicy(
  const pthread_attr_t  *attr,
  int                   *policy
)
{
  if ( !attr || !attr->is_initialized )
    return EINVAL;

  *policy = attr->schedpolicy;
  return 0;
}

/*PAGE
 *
 *  13.5.1 Thread Creation Scheduling Attributes, P1003.1c/Draft 10, p. 120
 */

int pthread_attr_setschedparam(
  pthread_attr_t            *attr,
  const struct sched_param  *param
)
{
  if ( !attr || !attr->is_initialized )
    return EINVAL;

  attr->schedparam = *param;
  return 0;
}

/*PAGE
 *
 *  13.5.1 Thread Creation Scheduling Attributes, P1003.1c/Draft 10, p. 120
 */

int pthread_attr_getschedparam(
  const pthread_attr_t   *attr,
  struct sched_param     *param
)
{
  if ( !attr || !attr->is_initialized )
    return EINVAL;

  *param = attr->schedparam;
  return 0;
}

/*PAGE
 *
 *  13.5.2 Dynamic Thread Scheduling Parameters Access, 
 *         P1003.1c/Draft 10, p. 124
 */

int pthread_getschedparam(
  pthread_t           thread,
  int                *policy,
  struct sched_param *param
)
{
  Objects_Locations        location;
  POSIX_API_Control       *api;
  register Thread_Control *the_thread;

  if ( !policy || !param  )
    return EINVAL;

  the_thread = _POSIX_Threads_Get( thread, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
    case OBJECTS_REMOTE:
      _Thread_Enable_dispatch();
      return ESRCH;
    case OBJECTS_LOCAL:
      api = the_thread->API_Extensions[ THREAD_API_POSIX ];
      *policy = api->schedpolicy;
      *param  = api->Schedule;
      _Thread_Enable_dispatch();
      return 0;
  }
 
  return POSIX_BOTTOM_REACHED();

}

/*PAGE
 *
 *  13.5.2 Dynamic Thread Scheduling Parameters Access, 
 *         P1003.1c/Draft 10, p. 124
 */

int pthread_setschedparam(
  pthread_t           thread,
  int                 policy,
  struct sched_param *param
)
{
  register Thread_Control *the_thread;
  POSIX_API_Control       *api;
  Objects_Locations        location;
 
  if ( !param )
    return EINVAL;

  /* XXX need to reschedule after doing this to the thread */
  /* XXX need to have one routine called by create and here */
#warning "pthread_setschedparam needs work"

  the_thread = _POSIX_Threads_Get( thread, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
    case OBJECTS_REMOTE:
      _Thread_Enable_dispatch();
      return ESRCH;
    case OBJECTS_LOCAL:
      switch ( policy ) {
        case SCHED_OTHER:
        case SCHED_FIFO:
        case SCHED_RR:
        case SCHED_SPORADIC:
          /* XXX this is where the interpretation work should go */
          break;
 
        default:
          _Thread_Enable_dispatch();
          return EINVAL;
      }

      api = the_thread->API_Extensions[ THREAD_API_POSIX ];

      api->schedpolicy = policy;
      api->Schedule    = *param;
      _Thread_Enable_dispatch();
      return 0;
  }
  return POSIX_BOTTOM_REACHED();
}

/*PAGE
 *
 *  16.1.1 Thread Creation Attributes, P1003.1c/Draft 10, p, 140
 */

int pthread_attr_init(
  pthread_attr_t  *attr
)
{
  if ( !attr )
    return EINVAL;
 
  *attr = _POSIX_Threads_Default_attributes;
  return 0;
}

/*PAGE
 *
 *  16.1.1 Thread Creation Attributes, P1003.1c/Draft 10, p, 140
 */

int pthread_attr_destroy(
  pthread_attr_t  *attr
)
{
  if ( !attr || !attr->is_initialized )
    return EINVAL;
 
  attr->is_initialized = FALSE;
  return 0;
}
 
/*PAGE
 *
 *  16.1.1 Thread Creation Attributes, P1003.1c/Draft 10, p, 140
 */

int pthread_attr_getstacksize(
  const pthread_attr_t  *attr,
  size_t                *stacksize
)
{
  if ( !attr || !attr->is_initialized )
    return EINVAL;

  *stacksize = attr->stacksize;
  return 0;
}
 
/*PAGE
 *
 *  16.1.1 Thread Creation Attributes, P1003.1c/Draft 10, p, 140
 */

int pthread_attr_setstacksize(
  pthread_attr_t  *attr,
  size_t           stacksize
)
{
  if ( !attr || !attr->is_initialized )
    return EINVAL;

  if (stacksize < STACK_MINIMUM_SIZE)
    attr->stacksize = STACK_MINIMUM_SIZE;
  else
    attr->stacksize = stacksize;
  return 0;
}
 
/*PAGE
 *
 *  16.1.1 Thread Creation Attributes, P1003.1c/Draft 10, p, 140
 */

int pthread_attr_getstackaddr(
  const pthread_attr_t   *attr,
  void                  **stackaddr
)
{
  if ( !attr || !attr->is_initialized )
    return EINVAL;

  *stackaddr = attr->stackaddr;
  return 0;
}
 
/*PAGE
 *
 *  16.1.1 Thread Creation Attributes, P1003.1c/Draft 10, p, 140
 */

int pthread_attr_setstackaddr(
  pthread_attr_t  *attr,
  void            *stackaddr
)
{
  if ( !attr || !attr->is_initialized )
    return EINVAL;

  attr->stackaddr = stackaddr;
  return 0;
}
 
/*PAGE
 *
 *  16.1.1 Thread Creation Attributes, P1003.1c/Draft 10, p, 140
 */

int pthread_attr_getdetachstate(
  const pthread_attr_t  *attr,
  int                   *detachstate
)
{
  if ( !attr || !attr->is_initialized )
    return EINVAL;

  *detachstate = attr->detachstate;
  return 0;
}
 
/*PAGE
 *
 *  16.1.1 Thread Creation Attributes, P1003.1c/Draft 10, p, 140
 */

int pthread_attr_setdetachstate(
  pthread_attr_t  *attr,
  int              detachstate
)
{
  if ( !attr || !attr->is_initialized )
    return EINVAL;

  switch ( detachstate ) {
    case PTHREAD_CREATE_DETACHED:
    case PTHREAD_CREATE_JOINABLE:
      attr->detachstate = detachstate;
      return 0;
 
    default:
      return EINVAL;
  }
}

/*PAGE
 *
 *  16.1.2 Thread Creation, P1003.1c/Draft 10, p. 144
 */

int pthread_create(
  pthread_t              *thread,
  const pthread_attr_t   *attr,
  void                 *(*start_routine)( void * ),
  void                   *arg
)
{
  const pthread_attr_t  *the_attr;
  Priority_Control       core_priority;
  boolean                is_timesliced;
  boolean                is_fp;
  boolean                status;
  Thread_Control        *the_thread;
  char                  *default_name = "psx";
  POSIX_API_Control     *api;
  int                    schedpolicy = SCHED_RR;
  struct sched_param     schedparams;

  the_attr = (attr) ? attr : &_POSIX_Threads_Default_attributes;

  if ( !the_attr->is_initialized )
    return EINVAL;

  /*
   *  Core Thread Initialize insures we get the minimum amount of
   *  stack space.
   */

#if 0
  int contentionscope;
  int schedpolicy;
  struct sched_param schedparam;

#if defined(_POSIX_THREAD_CPUTIME)
  int  cputime_clock_allowed;  /* see time.h */
#endif
#endif

  /*
   *  P1003.1c/D10, p. 121.
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
      schedparams = api->Schedule;
      break; 
    case PTHREAD_EXPLICIT_SCHED:
      schedpolicy = the_attr->schedpolicy;
      schedparams = the_attr->schedparam;
      break; 
  }

  /*
   *  Interpret the scheduling parameters.
   */

  is_timesliced = FALSE;

  if ( !_POSIX_Priority_Is_valid( the_attr->schedparam.sched_priority ) )
    return EINVAL;
 
  core_priority = _POSIX_Priority_To_core(the_attr->schedparam.sched_priority);
 
  switch ( schedpolicy ) {
    case SCHED_OTHER:
    case SCHED_FIFO:
      break;
    case SCHED_RR:
      is_timesliced = TRUE;
      break;
    case SCHED_SPORADIC:
      /*  XXX interpret the following parameters */
#if 0
  ss_low_priority;     /* Low scheduling priority for sporadic */
  ss_replenish_period; /* Replenishment period for sporadic server */
  ss_initial_budget;   /* Initial budget for sporadic server */
#endif
      break;
  }

  /*
   *  Currently all POSIX threads are floating point if the hardware 
   *  supports it.
   */

  is_fp = CPU_HARDWARE_FP;

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
    return EINVAL;
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
    is_timesliced,        /* timesliced */
    0,                    /* isr level */
    &default_name         /* posix threads don't have a name */
  );
 
  if ( !status ) {
    _POSIX_Threads_Free( the_thread );
    _Thread_Enable_dispatch();
    return EINVAL;
  }

  /*
   *  finish initializing the per API structure
   */

  
  api = the_thread->API_Extensions[ THREAD_API_POSIX ];

  api->Attributes = *the_attr;
  api->detachstate = attr->detachstate;

  _Thread_queue_Initialize(
    &api->Join_List,
    OBJECTS_NO_CLASS,                 /* only used for proxy operations */
    THREAD_QUEUE_DISCIPLINE_FIFO,
    0,                                /* XXX join blocking state */
    NULL,                             /* no extract proxy handler */
    0
  );

  status = _Thread_Start(
    the_thread,
    THREAD_START_POINTER,
    start_routine,
    arg,
    0                     /* unused */
  );

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

/*PAGE
 *
 *  16.1.3 Wait for Thread Termination, P1003.1c/Draft 10, p. 147
 */

int pthread_join(
  pthread_t   thread,
  void      **value_ptr
)
{
  register Thread_Control *the_thread;
  POSIX_API_Control       *api;
  Objects_Locations        location;

  the_thread = _POSIX_Threads_Get( thread, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
    case OBJECTS_REMOTE:
      return ESRCH;
    case OBJECTS_LOCAL:
      api = the_thread->API_Extensions[ THREAD_API_POSIX ];

      if ( api->detachstate == PTHREAD_CREATE_DETACHED ) {
        _Thread_Enable_dispatch();
        return EINVAL;
      }

      /*
       *  Put ourself on the threads join list
       */

      /* XXX is this right? */

      _Thread_Executing->Wait.return_argument = (unsigned32 *) value_ptr;

      _Thread_queue_Enqueue( &api->Join_List, WATCHDOG_NO_TIMEOUT );

      _Thread_Enable_dispatch();

      return 0;
  }

  return POSIX_BOTTOM_REACHED();
}

/*PAGE
 *
 *  16.1.4 Detaching a Thread, P1003.1c/Draft 10, p. 149
 */

int pthread_detach(
  pthread_t   thread
)
{
  register Thread_Control *the_thread;
  POSIX_API_Control       *api;
  Objects_Locations        location;
 
  the_thread = _POSIX_Threads_Get( thread, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
    case OBJECTS_REMOTE:
      _Thread_Enable_dispatch();
      return ESRCH;
    case OBJECTS_LOCAL:

      api = the_thread->API_Extensions[ THREAD_API_POSIX ];
      api->detachstate = PTHREAD_CREATE_DETACHED;
      _Thread_Enable_dispatch();
      return 0;
  }
 
  return POSIX_BOTTOM_REACHED();
}

/*PAGE
 *
 * 16.1.5.1 Thread Termination, p1003.1c/Draft 10, p. 150
 */
 
void pthread_exit(
  void  *value_ptr
)
{
  register Thread_Control *executing;
  register Thread_Control *the_thread;
  POSIX_API_Control       *api;

  executing = _Thread_Executing;

  _Thread_Disable_dispatch();

  _Thread_Close( &_POSIX_Threads_Information, executing );

  /*
   *  Wakeup all the tasks which joined with this one
   */

  api = executing->API_Extensions[ THREAD_API_POSIX ];

  while ( (the_thread = _Thread_queue_Dequeue( &api->Join_List )) )
      *(void **)the_thread->Wait.return_argument = value_ptr;

  /* XXX run _POSIX_Keys_Run_destructors here? */

  _POSIX_Threads_Free( executing );

  _Thread_Enable_dispatch();
}

/*PAGE
 *
 * 16.1.6 Get Calling Thread's ID, p1003.1c/Draft 10, p. XXX
 */

pthread_t pthread_self( void )
{
  return _Thread_Executing->Object.id;
}

/*PAGE
 *
 *  16.1.7 Compare Thread IDs, p1003.1c/Draft 10, p. 153
 */

int pthread_equal( 
  pthread_t  t1,
  pthread_t  t2
)
{
  int               status;
  Objects_Locations location;

 /* XXX may want to do a "get" to make sure both are valid. */
 /* XXX behavior is undefined if not valid pthread_t's */
 
  /*
   *  By default this is not a match.
   */

  status = 0;

  /*
   *  Validate the first id and return 0 if it is not valid
   */

  (void) _POSIX_Threads_Get( t1, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
    case OBJECTS_REMOTE:
      break;

    case OBJECTS_LOCAL:

      /*
       *  Validate the second id and return 0 if it is not valid
       */

      (void) _POSIX_Threads_Get( t2, &location );
      switch ( location ) {
        case OBJECTS_ERROR:
        case OBJECTS_REMOTE:
          break; 
        case OBJECTS_LOCAL:
          status = _Objects_Are_ids_equal( t1, t2 ); 
          break;
      }
      _Thread_Unnest_dispatch();
      break;
  }

  _Thread_Enable_dispatch();
  return status;

}

/*PAGE
 *
 *  16.1.8 Dynamic Package Initialization
 */

int pthread_once(
  pthread_once_t  *once_control,
  void           (*init_routine)(void)
)
{
  /* XXX: Should we implement this routine this way or make it a full */
  /* XXX: fledged object? */

  if ( !once_control || !init_routine )
    return EINVAL;

  _Thread_Disable_dispatch();

  if ( !once_control->is_initialized ) {

    once_control->is_initialized = TRUE;
    once_control->init_executed = TRUE;
    (*init_routine)();

  } if ( !once_control->init_executed ) {

    once_control->init_executed = TRUE;
    (*init_routine)();

  }
  
  _Thread_Enable_dispatch();

  return 0;
}

/*PAGE
 *
 *  20.1.6 Accessing a Thread CPU-time Clock, P1003.4b/D8, p. 58
 */
 
int pthread_getcpuclockid(
  pthread_t    pid,
  clockid_t   *clock_id
)
{
  return POSIX_NOT_IMPLEMENTED();
}

/*PAGE
 *
 *  20.1.7 CPU-time Clock Thread Creation Attribute, P1003.4b/D8, p. 59
 */

int pthread_attr_setcputime(
  pthread_attr_t  *attr,
  int              clock_allowed
)
{
  if ( !attr || !attr->is_initialized )
    return EINVAL;

  switch ( clock_allowed ) {
    case CLOCK_ENABLED:
    case CLOCK_DISABLED:
      attr->cputime_clock_allowed = clock_allowed;
      return 0;
 
    default:
      return EINVAL;
  }
}

/*PAGE
 *
 *  20.1.7 CPU-time Clock Thread Creation Attribute, P1003.4b/D8, p. 59
 */

int pthread_attr_getcputime(
  pthread_attr_t  *attr,
  int             *clock_allowed
)
{
  if ( !attr || !attr->is_initialized )
    return EINVAL;

  *clock_allowed = attr->cputime_clock_allowed;
  return 0;
}
