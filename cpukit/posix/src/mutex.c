/*
 *  $Id$
 */

#include <assert.h>
#include <errno.h>
#include <pthread.h>

#include <rtems/system.h>
#include <rtems/score/coremutex.h>
#include <rtems/score/watchdog.h>
#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/score/mpci.h>
#endif
#include <rtems/posix/mutex.h>
#include <rtems/posix/priority.h>
#include <rtems/posix/time.h>

/*
 *  TEMPORARY
 */

#if defined(RTEMS_MULTIPROCESSING)
void _POSIX_Mutex_MP_Send_process_packet (
  POSIX_Mutex_MP_Remote_operations  operation,
  Objects_Id                        mutex_id,
  Objects_Name                      name,
  Objects_Id                        proxy_id
)
{
  (void) POSIX_MP_NOT_IMPLEMENTED();
}

void _POSIX_Mutex_MP_Send_object_was_deleted (
  Thread_Control *the_proxy
)
{
  (void) POSIX_MP_NOT_IMPLEMENTED();
}

int _POSIX_Mutex_MP_Send_request_packet (
  POSIX_Mutex_MP_Remote_operations  operation,
  Objects_Id                        mutex_id,
  boolean                           wait,  /* XXX options */
  Watchdog_Interval                 timeout
)
{
  return POSIX_MP_NOT_IMPLEMENTED();
}

void POSIX_Threads_mutex_MP_support(
  Thread_Control *the_thread,
  Objects_Id      id
)
{
  (void) POSIX_MP_NOT_IMPLEMENTED();   /* XXX: should never get here */
}
#endif

/*
 *  END OF TEMPORARY
 */

/*PAGE
 *  
 *  The default mutex attributes structure.
 */

const pthread_mutexattr_t _POSIX_Mutex_Default_attributes = {
  TRUE,                                    /* is_initialized */
  PTHREAD_PROCESS_PRIVATE,                 /* process_shared */
  POSIX_SCHEDULER_MAXIMUM_PRIORITY,        /* prio_ceiling   */
  PTHREAD_PRIO_NONE,                       /* protocol       */
  FALSE                                    /* recursive      */
};

/*PAGE
 *
 *  _POSIX_Mutex_From_core_mutex_status
 */

int _POSIX_Mutex_From_core_mutex_status(
  CORE_mutex_Status  status
)
{
  switch ( status ) {
    case CORE_MUTEX_STATUS_SUCCESSFUL:
      return 0;
    case CORE_MUTEX_STATUS_UNSATISFIED_NOWAIT:
      return EBUSY;
    case CORE_MUTEX_STATUS_NESTING_NOT_ALLOWED:
      return EDEADLK;
    case CORE_MUTEX_STATUS_NOT_OWNER_OF_RESOURCE:
      return EPERM;
    case CORE_MUTEX_WAS_DELETED:
      return EINVAL;
    case CORE_MUTEX_TIMEOUT:
      return EAGAIN;
    case CORE_MUTEX_STATUS_CEILING_VIOLATED:
      return EINVAL;
    default:
      break;
  }
  assert( 0 );
  return 0;
}

/*PAGE
 *
 *  _POSIX_Mutex_Manager_initialization
 *
 *  This routine initializes all mutex manager related data structures.
 *
 *  Input parameters:
 *    maximum_mutexes - maximum configured mutexes
 *
 *  Output parameters:  NONE
 */
 
void _POSIX_Mutex_Manager_initialization(
  unsigned32 maximum_mutexes
)
{
  _Objects_Initialize_information(
    &_POSIX_Mutex_Information,
    OBJECTS_POSIX_MUTEXES,
    TRUE,
    maximum_mutexes,
    sizeof( POSIX_Mutex_Control ),
    FALSE,
    0,
    FALSE
  );
}

/*PAGE
 *
 *  11.3.1 Mutex Initialization Attributes, P1003.1c/Draft 10, p. 81
 */

int pthread_mutexattr_init(
  pthread_mutexattr_t *attr
)
{
  if ( !attr )
    return EINVAL;

  *attr = _POSIX_Mutex_Default_attributes;
  return 0;
}

/*PAGE
 *
 *  11.3.1 Mutex Initialization Attributes, P1003.1c/Draft 10, p. 81
 */

int pthread_mutexattr_destroy(
  pthread_mutexattr_t *attr
)
{
  if ( !attr || !attr->is_initialized )
    return EINVAL;

  attr->is_initialized = FALSE;
  return 0;
}

/*PAGE
 *
 *  11.3.1 Mutex Initialization Attributes, P1003.1c/Draft 10, p. 81
 */

int pthread_mutexattr_getpshared(
  const pthread_mutexattr_t *attr,
  int                       *pshared
)
{
  if ( !attr || !attr->is_initialized || !pshared )
    return EINVAL;

  *pshared = attr->process_shared;
  return 0;
}

/*PAGE
 *
 *  11.3.1 Mutex Initialization Attributes, P1003.1c/Draft 10, p. 81
 */

int pthread_mutexattr_setpshared(
  pthread_mutexattr_t *attr,
  int                  pshared
)
{
  if ( !attr || !attr->is_initialized )
    return EINVAL;

  switch ( pshared ) {
    case PTHREAD_PROCESS_SHARED:
    case PTHREAD_PROCESS_PRIVATE:
      attr->process_shared = pshared;
      return 0;

    default:
      return EINVAL;
  }
}

/*PAGE
 *
 *  11.3.2 Initializing and Destroying a Mutex, P1003.1c/Draft 10, p. 87
 *
 *  NOTE:  XXX Could be optimized so all the attribute error checking
 *             is not performed when attr is NULL.
 */

int pthread_mutex_init(
  pthread_mutex_t           *mutex,
  const pthread_mutexattr_t *attr
)
{
  POSIX_Mutex_Control          *the_mutex;
  CORE_mutex_Attributes        *the_mutex_attr;
  const pthread_mutexattr_t    *the_attr;
  CORE_mutex_Disciplines        the_discipline;
  register POSIX_Mutex_Control *mutex_in_use;
  Objects_Locations             location;

  if ( attr ) the_attr = attr;
  else        the_attr = &_POSIX_Mutex_Default_attributes;

  /* Check for NULL mutex */

  if ( !mutex )
    return EINVAL;

  /*
   *  This code should eventually be removed.  
   *
   *  Although the POSIX specification says:
   *
   *  "Attempting to initialize an already initialized mutex results
   *  in undefined behavior."
   *
   *  Trying to keep the caller from doing the create when *mutex
   *  is actually a valid ID causes grief.  All it takes is the wrong
   *  value in an uninitialized variable to make this fail.  As best
   *  I can tell, RTEMS was the only pthread implementation to choose
   *  this option for "undefined behavior" and doing so has created
   *  portability problems.  In particular, Rosimildo DaSilva
   *  <rdasilva@connecttel.com> saw seemingly random failures in the
   *  RTEMS port of omniORB2 when this code was enabled.
   *
   *  Joel Sherrill <joel@OARcorp.com>     14 May 1999
   */

  
#if 0
  /* avoid infinite recursion on call to this routine in _POSIX_Mutex_Get */

  if ( *mutex != PTHREAD_MUTEX_INITIALIZER ) {

    /* EBUSY if *mutex is a valid id */

    mutex_in_use = _POSIX_Mutex_Get( mutex, &location );
    switch ( location ) {
      case OBJECTS_REMOTE:
      case OBJECTS_ERROR:
        break;
      case OBJECTS_LOCAL:
        _Thread_Enable_dispatch();
        return EBUSY;
    }
  }
#endif
 
  if ( !the_attr->is_initialized ) 
    return EINVAL;

  /*
   *  XXX: Be careful about attributes when global!!!
   */

  assert( the_attr->process_shared == PTHREAD_PROCESS_PRIVATE );

#if defined(RTEMS_MULTIPROCESSING)
  if ( the_attr->process_shared == PTHREAD_PROCESS_SHARED )
    return POSIX_MP_NOT_IMPLEMENTED();
#endif
 
  /*
   *  Determine the discipline of the mutex
   */
  
  switch ( the_attr->protocol ) {
    case PTHREAD_PRIO_NONE:
      the_discipline = CORE_MUTEX_DISCIPLINES_FIFO;
      break;
    case PTHREAD_PRIO_INHERIT:
      the_discipline = CORE_MUTEX_DISCIPLINES_PRIORITY_INHERIT;
      break;
    case PTHREAD_PRIO_PROTECT:
      the_discipline = CORE_MUTEX_DISCIPLINES_PRIORITY_CEILING;
      break;
    default:
      return EINVAL;
  }

  if ( !_POSIX_Priority_Is_valid( the_attr->prio_ceiling ) )
    return EINVAL;

  _Thread_Disable_dispatch();

  the_mutex = _POSIX_Mutex_Allocate();
 
  if ( !the_mutex ) {
    _Thread_Enable_dispatch();
    return EAGAIN;
  }

#if defined(RTEMS_MULTIPROCESSING)
  if ( the_attr->process_shared == PTHREAD_PROCESS_SHARED && 
       !( _Objects_MP_Allocate_and_open( &_POSIX_Mutex_Information, 0,
                            the_mutex->Object.id, FALSE ) ) ) {
    _POSIX_Mutex_Free( the_mutex );
    _Thread_Enable_dispatch();
    return EAGAIN;
  }
#endif

  the_mutex->process_shared = the_attr->process_shared;

  the_mutex_attr = &the_mutex->Mutex.Attributes;

  the_mutex_attr->allow_nesting = the_attr->recursive;
  the_mutex_attr->priority_ceiling =
    _POSIX_Priority_To_core( the_attr->prio_ceiling );
  the_mutex_attr->discipline = the_discipline;

  /*
   *  Must be initialized to unlocked.
   */

  _CORE_mutex_Initialize(
    &the_mutex->Mutex,
    OBJECTS_POSIX_MUTEXES,
    the_mutex_attr,
    CORE_MUTEX_UNLOCKED,
    NULL                      /* proxy_extract_callout */
  );

  _Objects_Open( &_POSIX_Mutex_Information, &the_mutex->Object, 0 );

  *mutex = the_mutex->Object.id;

#if defined(RTEMS_MULTIPROCESSING)
  if ( the_attr->process_shared == PTHREAD_PROCESS_SHARED )
    _POSIX_Mutex_MP_Send_process_packet(
      POSIX_MUTEX_MP_ANNOUNCE_CREATE,
      the_mutex->Object.id,
      0,                         /* Name not used */
      0                          /* Not used */
    );
#endif

  _Thread_Enable_dispatch();
  return 0;
}

/*PAGE
 *
 *  11.3.2 Initializing and Destroying a Mutex, P1003.1c/Draft 10, p. 87
 */

int pthread_mutex_destroy(
  pthread_mutex_t           *mutex
)
{
  register POSIX_Mutex_Control *the_mutex;
  Objects_Locations             location;
 
  the_mutex = _POSIX_Mutex_Get( mutex, &location );
  switch ( location ) {
    case OBJECTS_REMOTE:
#if defined(RTEMS_MULTIPROCESSING)
      _Thread_Dispatch();
      return POSIX_MP_NOT_IMPLEMENTED();
      return EINVAL;
#endif
    case OBJECTS_ERROR:
      return EINVAL;
    case OBJECTS_LOCAL:
       /*
        * XXX: There is an error for the mutex being locked
        *  or being in use by a condition variable.
        */

      if ( _CORE_mutex_Is_locked( &the_mutex->Mutex ) ) {
        _Thread_Enable_dispatch();
        return EBUSY;
      }
 
      _Objects_Close( &_POSIX_Mutex_Information, &the_mutex->Object );
 
      _CORE_mutex_Flush(
        &the_mutex->Mutex,
#if defined(RTEMS_MULTIPROCESSING)
        _POSIX_Mutex_MP_Send_object_was_deleted,
#else
        NULL,
#endif
        EINVAL
      );
 
      _POSIX_Mutex_Free( the_mutex );
 
#if defined(RTEMS_MULTIPROCESSING)
      if ( the_mutex->process_shared == PTHREAD_PROCESS_SHARED ) {
 
        _Objects_MP_Close( &_POSIX_Mutex_Information, the_mutex->Object.id );
 
        _POSIX_Mutex_MP_Send_process_packet(
          POSIX_MUTEX_MP_ANNOUNCE_DELETE,
          the_mutex->Object.id,
          0,                         /* Not used */
          0                          /* Not used */
        );
      }
#endif
      _Thread_Enable_dispatch();
      return 0;
  }
  return POSIX_BOTTOM_REACHED();
}

/*PAGE
 *
 *  _POSIX_Mutex_Lock_support
 *
 *  A support routine which implements guts of the blocking, non-blocking, and
 *  timed wait version of mutex lock.
 */

int _POSIX_Mutex_Lock_support(
  pthread_mutex_t           *mutex,
  boolean                    blocking,
  Watchdog_Interval          timeout
)
{
  register POSIX_Mutex_Control *the_mutex;
  Objects_Locations             location;
 
  the_mutex = _POSIX_Mutex_Get( mutex, &location );
  switch ( location ) {
    case OBJECTS_REMOTE:
#if defined(RTEMS_MULTIPROCESSING)
      return _POSIX_Mutex_MP_Send_request_packet(
          POSIX_MUTEX_MP_OBTAIN_REQUEST,
          *mutex,
          0,   /* must define the option set */
          WATCHDOG_NO_TIMEOUT
      );
#endif
    case OBJECTS_ERROR:
      return EINVAL;
    case OBJECTS_LOCAL:
      _CORE_mutex_Seize(
        &the_mutex->Mutex,
        the_mutex->Object.id,
        blocking,
        timeout
      );
      _Thread_Enable_dispatch();
      return _POSIX_Mutex_From_core_mutex_status(
        (CORE_mutex_Status) _Thread_Executing->Wait.return_code
      );
  }
  return POSIX_BOTTOM_REACHED();
}

/*PAGE
 *
 *  11.3.3 Locking and Unlocking a Mutex, P1003.1c/Draft 10, p. 93
 *        
 *  NOTE: P1003.4b/D8 adds pthread_mutex_timedlock(), p. 29
 */

int pthread_mutex_lock(
  pthread_mutex_t           *mutex
)
{
  return _POSIX_Mutex_Lock_support( mutex, TRUE, THREAD_QUEUE_WAIT_FOREVER );
}

/*PAGE
 *
 *  11.3.3 Locking and Unlocking a Mutex, P1003.1c/Draft 10, p. 93
 *        
 *  NOTE: P1003.4b/D8 adds pthread_mutex_timedlock(), p. 29
 */

int pthread_mutex_trylock(
  pthread_mutex_t           *mutex
)
{
  return _POSIX_Mutex_Lock_support( mutex, FALSE, THREAD_QUEUE_WAIT_FOREVER );
}

/*PAGE
 *
 *  11.3.3 Locking and Unlocking a Mutex, P1003.1c/Draft 10, p. 93
 *        
 *  NOTE: P1003.4b/D8 adds pthread_mutex_timedlock(), p. 29
 */

int pthread_mutex_unlock(
  pthread_mutex_t           *mutex
)
{
  register POSIX_Mutex_Control *the_mutex;
  Objects_Locations             location;
  CORE_mutex_Status             status;
 
  the_mutex = _POSIX_Mutex_Get( mutex, &location );
  switch ( location ) {
    case OBJECTS_REMOTE:
#if defined(RTEMS_MULTIPROCESSING)
      return _POSIX_Mutex_MP_Send_request_packet(
          POSIX_MUTEX_MP_RELEASE_REQUEST,
          *mutex,
          0,                    /* Not used */
          MPCI_DEFAULT_TIMEOUT
      );
#endif
    case OBJECTS_ERROR:
      return EINVAL;
    case OBJECTS_LOCAL:
      status = _CORE_mutex_Surrender(
        &the_mutex->Mutex,
        the_mutex->Object.id, 
#if defined(RTEMS_MULTIPROCESSING)
        POSIX_Threads_mutex_MP_support
#else
        NULL
#endif
      );
      _Thread_Enable_dispatch();
      return _POSIX_Mutex_From_core_mutex_status( status );
      break;
  }
  return POSIX_BOTTOM_REACHED();
}

/*PAGE
 *
 *  11.3.3 Locking and Unlocking a Mutex, P1003.1c/Draft 10, p. 93
 *        
 *  NOTE: P1003.4b/D8 adds pthread_mutex_timedlock(), p. 29
 */

int pthread_mutex_timedlock(
  pthread_mutex_t       *mutex,
  const struct timespec *timeout
)
{
  return _POSIX_Mutex_Lock_support( 
    mutex,
    TRUE,
    _POSIX_Timespec_to_interval( timeout )
  );
}

/*PAGE
 *
 *  13.6.1 Mutex Initialization Scheduling Attributes, P1003.1c/Draft 10, p. 128
 */
 
int pthread_mutexattr_setprotocol(
  pthread_mutexattr_t   *attr,
  int                    protocol
)
{
  if ( !attr || !attr->is_initialized )
    return EINVAL;

  switch ( protocol ) {
    case PTHREAD_PRIO_NONE:
    case PTHREAD_PRIO_INHERIT:
    case PTHREAD_PRIO_PROTECT:
      attr->protocol = protocol;
      return 0;
 
    default:
      return EINVAL;
  }
}

/*PAGE
 *
 *  13.6.1 Mutex Initialization Scheduling Attributes, P1003.1c/Draft 10, p. 128
 */
 
int pthread_mutexattr_getprotocol(
  const pthread_mutexattr_t   *attr,
  int                         *protocol
)
{
  if ( !attr || !attr->is_initialized || !protocol )
    return EINVAL;

  *protocol = attr->protocol;
  return 0;
}

/*PAGE
 *
 *  13.6.1 Mutex Initialization Scheduling Attributes, P1003.1c/Draft 10, p. 128
 */
 
int pthread_mutexattr_setprioceiling(
  pthread_mutexattr_t   *attr,
  int                    prioceiling
)
{
  if ( !attr || !attr->is_initialized )
    return EINVAL;

  if ( !_POSIX_Priority_Is_valid( prioceiling ) )
    return EINVAL;

  attr->prio_ceiling = prioceiling;
  return 0;
}

/*PAGE
 *
 *  13.6.1 Mutex Initialization Scheduling Attributes, P1003.1c/Draft 10, p. 128
 */
 
int pthread_mutexattr_getprioceiling(
  const pthread_mutexattr_t   *attr,
  int                         *prioceiling
)
{
  if ( !attr || !attr->is_initialized || !prioceiling )
    return EINVAL;

  *prioceiling = attr->prio_ceiling;
  return 0;
}

/*PAGE
 *
 *  13.6.2 Change the Priority Ceiling of a Mutex, P1003.1c/Draft 10, p. 131
 */

int pthread_mutex_setprioceiling(
  pthread_mutex_t   *mutex,
  int                prioceiling,
  int               *old_ceiling
)
{
  register POSIX_Mutex_Control *the_mutex;
  Objects_Locations             location;
  Priority_Control              the_priority;
  int                           status;

  if ( !old_ceiling )
    return EINVAL;

  if ( !_POSIX_Priority_Is_valid( prioceiling ) )
    return EINVAL;

  the_priority = _POSIX_Priority_To_core( prioceiling );

  /*
   *  Must acquire the mutex before we can change it's ceiling
   */

  status = pthread_mutex_lock( mutex );
  if ( status )
    return status;

  the_mutex = _POSIX_Mutex_Get( mutex, &location );
  switch ( location ) {
    case OBJECTS_REMOTE:
#if defined(RTEMS_MULTIPROCESSING)
      /*  XXX It feels questionable to set the ceiling on a remote mutex. */
      return EINVAL;
#endif
    case OBJECTS_ERROR:
      return EINVAL;        /* impossible to get here */
    case OBJECTS_LOCAL:
      *old_ceiling = _POSIX_Priority_From_core(
        the_mutex->Mutex.Attributes.priority_ceiling
      );
      the_mutex->Mutex.Attributes.priority_ceiling = the_priority;
      _CORE_mutex_Surrender(
        &the_mutex->Mutex,
        the_mutex->Object.id, 
#if defined(RTEMS_MULTIPROCESSING)
        POSIX_Threads_mutex_MP_support
#else
        NULL
#endif
      );
      _Thread_Enable_dispatch();
      return 0;
  }
  return POSIX_BOTTOM_REACHED();
}
 
/*PAGE
 *
 *  13.6.2 Change the Priority Ceiling of a Mutex, P1003.1c/Draft 10, p. 131
 */

int pthread_mutex_getprioceiling(
  pthread_mutex_t   *mutex,
  int               *prioceiling
)
{
  register POSIX_Mutex_Control *the_mutex;
  Objects_Locations             location;

  if ( !prioceiling )
    return EINVAL;

  the_mutex = _POSIX_Mutex_Get( mutex, &location );
  switch ( location ) {
    case OBJECTS_REMOTE:
#if defined(RTEMS_MULTIPROCESSING)
      return POSIX_MP_NOT_IMPLEMENTED();   /* XXX feels questionable */
#endif
    case OBJECTS_ERROR:
      return EINVAL;
    case OBJECTS_LOCAL:
      *prioceiling = _POSIX_Priority_From_core(
        the_mutex->Mutex.Attributes.priority_ceiling
      );
      _Thread_Enable_dispatch();
      return 0;
  }
  return POSIX_BOTTOM_REACHED();
}
