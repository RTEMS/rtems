/*  mutex.c
 *
 *  $Id$
 */

#include <errno.h>
#include <pthread.h>

#include <rtems/system.h>
#include <rtems/score/coremutex.h>
#include <rtems/score/watchdog.h>
#include <rtems/score/mpci.h>
#include <rtems/posix/mutex.h>
#include <rtems/posix/priority.h>
#include <rtems/posix/time.h>

/*PAGE
 *  
 *  The default mutex attributes structure.
 */

const pthread_mutexattr_t _POSIX_Mutex_Default_attributes = {
  TRUE,                                    /* is_initialized */
  PTHREAD_PROCESS_PRIVATE,                 /* process_shared */
  POSIX_SCHEDULER_MINIMUM_PRIORITY,        /* prio_ceiling   */
  PTHREAD_PRIO_NONE,                       /* protocol       */
  FALSE                                    /* recursive      */
};

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
  if ( !attr || attr->is_initialized == FALSE )
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
  if ( !attr )
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
  if ( !attr )
    return EINVAL;

  attr->process_shared = pshared;
  return 0;
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
  POSIX_Mutex_Control        *the_mutex;
  CORE_mutex_Attributes      *the_mutex_attr;
  const pthread_mutexattr_t  *the_attr;
  CORE_mutex_Disciplines      the_discipline;

  if ( attr ) the_attr = attr;
  else        the_attr = &_POSIX_Mutex_Default_attributes;

  /*
   *  XXX: Be careful about attributes when global!!!
   */

  if ( the_attr->process_shared == PTHREAD_PROCESS_SHARED )
    return POSIX_MP_NOT_IMPLEMENTED();
 
  if ( !the_attr->is_initialized ) 
    return EINVAL;

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
    return ENOMEM;
  }

  if ( the_attr->process_shared == PTHREAD_PROCESS_SHARED && 
       !( _Objects_MP_Allocate_and_open( &_POSIX_Mutex_Information, 0,
                            the_mutex->Object.id, FALSE ) ) ) {
    _POSIX_Mutex_Free( the_mutex );
    _Thread_Enable_dispatch();
    return EAGAIN;
  }

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
    0                  /* XXX proxy_extract_callout */
  );

  _Objects_Open( &_POSIX_Mutex_Information, &the_mutex->Object, 0 );

  *mutex = the_mutex->Object.id;

  if ( the_attr->process_shared == PTHREAD_PROCESS_SHARED )
    _POSIX_Mutex_MP_Send_process_packet(
      POSIX_MUTEX_MP_ANNOUNCE_CREATE,
      the_mutex->Object.id,
      0,                         /* Name not used */
      0                          /* Not used */
    );

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
    case OBJECTS_ERROR:
      return EINVAL;
    case OBJECTS_REMOTE:
      _Thread_Dispatch();
      return POSIX_MP_NOT_IMPLEMENTED();
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
        _POSIX_Mutex_MP_Send_object_was_deleted,
        EINVAL
      );
 
      _POSIX_Mutex_Free( the_mutex );
 
      if ( the_mutex->process_shared == PTHREAD_PROCESS_SHARED ) {
 
        _Objects_MP_Close( &_POSIX_Mutex_Information, the_mutex->Object.id );
 
        _POSIX_Mutex_MP_Send_process_packet(
          POSIX_MUTEX_MP_ANNOUNCE_DELETE,
          the_mutex->Object.id,
          0,                         /* Not used */
          0                          /* Not used */
        );
      }
      _Thread_Enable_dispatch();
      return 0;
  }
  return POSIX_BOTTOM_REACHED();
}

/*
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
    case OBJECTS_ERROR:
      return EINVAL;
    case OBJECTS_REMOTE:
      return _POSIX_Mutex_MP_Send_request_packet(
          POSIX_MUTEX_MP_OBTAIN_REQUEST,
          *mutex,
          0,   /* must define the option set */
          WATCHDOG_NO_TIMEOUT
      );
    case OBJECTS_LOCAL:
      _CORE_mutex_Seize(
        &the_mutex->Mutex,
        the_mutex->Object.id,
        blocking,
        timeout
      );
      _Thread_Enable_dispatch();
      return _Thread_Executing->Wait.return_code;
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

void POSIX_Threads_mutex_MP_support(
  Thread_Control *the_thread,
  Objects_Id      id
)
{
  (void) POSIX_MP_NOT_IMPLEMENTED();   /* XXX: should never get here */
}

int pthread_mutex_unlock(
  pthread_mutex_t           *mutex
)
{
  register POSIX_Mutex_Control *the_mutex;
  Objects_Locations             location;
 
  the_mutex = _POSIX_Mutex_Get( mutex, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      return EINVAL;
    case OBJECTS_REMOTE:
      return _POSIX_Mutex_MP_Send_request_packet(
          POSIX_MUTEX_MP_RELEASE_REQUEST,
          *mutex,
          0,                    /* Not used */
          MPCI_DEFAULT_TIMEOUT
      );
    case OBJECTS_LOCAL:
      _CORE_mutex_Surrender(
        &the_mutex->Mutex,
        the_mutex->Object.id, 
        POSIX_Threads_mutex_MP_support
      );
      _Thread_Enable_dispatch();
      return _Thread_Executing->Wait.return_code; /* XXX return status */
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
    _POSIX_Time_Spec_to_interval( timeout )
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
  if ( !attr )
    return EINVAL;

  attr->protocol = protocol;
  return 0;
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
  if ( !attr )
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
  if ( !attr )
    return EINVAL;

  if ( !_POSIX_Priority_Is_valid( attr->prio_ceiling ) )
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
  if ( !attr )
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
    case OBJECTS_ERROR:
      return EINVAL;
    case OBJECTS_REMOTE:
      return POSIX_MP_NOT_IMPLEMENTED();   /* XXX feels questionable */
      return EINVAL;
    case OBJECTS_LOCAL:
      the_mutex->Mutex.Attributes.priority_ceiling = the_priority;
      _CORE_mutex_Surrender(
        &the_mutex->Mutex,
        the_mutex->Object.id, 
        POSIX_Threads_mutex_MP_support
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

  the_mutex = _POSIX_Mutex_Get( mutex, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      return EINVAL;
    case OBJECTS_REMOTE:
      return POSIX_MP_NOT_IMPLEMENTED();   /* XXX feels questionable */
    case OBJECTS_LOCAL:
      *prioceiling = _POSIX_Priority_From_core(
        the_mutex->Mutex.Attributes.priority_ceiling
      );
      _Thread_Enable_dispatch();
      return 0;
  }
  return POSIX_BOTTOM_REACHED();
}
