/*  cond.c
 *
 *  $Id$
 */

#include <pthread.h>
#include <errno.h>

#include <rtems/score/object.h>
#include <rtems/score/states.h>
#include <rtems/score/watchdog.h>
#include <rtems/posix/cond.h>
#include <rtems/posix/time.h>

/*PAGE
 *
 *  The default condition variable attributes structure.
 */
 
const pthread_condattr_t _POSIX_Condition_variables_Default_attributes = {
  TRUE,                      /* is_initialized */
  PTHREAD_PROCESS_PRIVATE    /* process_shared */
};
 
/*PAGE
 *
 *  _POSIX_Condition_variables_Manager_initialization
 *
 *  This routine initializes all condition variable manager related data 
 *  structures.
 *
 *  Input parameters:
 *    maximum_condition_variables - maximum configured condition_variables
 *
 *  Output parameters:  NONE
 */
 
void _POSIX_Condition_variables_Manager_initialization(
  unsigned32 maximum_condition_variables
)
{
  _Objects_Initialize_information(
    &_POSIX_Condition_variables_Information,
    OBJECTS_POSIX_CONDITION_VARIABLES,
    TRUE,
    maximum_condition_variables,
    sizeof( POSIX_Condition_variables_Control ),
    FALSE,
    0,
    FALSE
  );
}

/*PAGE
 *
 *  11.4.1 Condition Variable Initialization Attributes, 
 *            P1003.1c/Draft 10, p. 96
 */
 
int pthread_condattr_init(
  pthread_condattr_t *attr
)
{
  if ( !attr )
    return EINVAL;

  *attr = _POSIX_Condition_variables_Default_attributes;
  return 0;
}
 
/*PAGE
 *
 *  11.4.1 Condition Variable Initialization Attributes, 
 *            P1003.1c/Draft 10, p. 96
 */
 
int pthread_condattr_destroy(
  pthread_condattr_t *attr
)
{
  if ( !attr || attr->is_initialized == FALSE )
    return EINVAL;

  attr->is_initialized = FALSE;
  return 0;
}
 
/*PAGE
 *
 *  11.4.1 Condition Variable Initialization Attributes, 
 *            P1003.1c/Draft 10, p. 96
 */
 
int pthread_condattr_getpshared(
  const pthread_condattr_t *attr,
  int                      *pshared
)
{
  if ( !attr )
    return EINVAL;

  *pshared = attr->process_shared;
  return 0;
}
 
/*PAGE
 *
 *  11.4.1 Condition Variable Initialization Attributes, 
 *            P1003.1c/Draft 10, p. 96
 */
 
int pthread_condattr_setpshared(
  pthread_condattr_t *attr,
  int                 pshared
)
{
  if ( !attr )
    return EINVAL;

  attr->process_shared = pshared;
  return 0;
}
 
/*PAGE
 *
 *  11.4.2 Initializing and Destroying a Condition Variable, 
 *         P1003.1c/Draft 10, p. 87
 */
 
int pthread_cond_init(
  pthread_cond_t           *cond,
  const pthread_condattr_t *attr
)
{
  POSIX_Condition_variables_Control   *the_cond;
  const pthread_condattr_t            *the_attr;
 
  if ( attr ) the_attr = attr;
  else        the_attr = &_POSIX_Condition_variables_Default_attributes;
 
  /*
   *  XXX: Be careful about attributes when global!!!
   */
 
  if ( the_attr->process_shared == PTHREAD_PROCESS_SHARED )
    return POSIX_MP_NOT_IMPLEMENTED();
 
  if ( !the_attr->is_initialized )
    return EINVAL;
 
  _Thread_Disable_dispatch();
 
  the_cond = _POSIX_Condition_variables_Allocate();
 
  if ( !the_cond ) {
    _Thread_Enable_dispatch();
    return ENOMEM;
  }
 
  if ( the_attr->process_shared == PTHREAD_PROCESS_SHARED &&
     !( _Objects_MP_Allocate_and_open( &_POSIX_Condition_variables_Information,
                0, the_cond->Object.id, FALSE ) ) ) {
    _POSIX_Condition_variables_Free( the_cond );
    _Thread_Enable_dispatch();
    return EAGAIN;
  }
 
  the_cond->process_shared  = the_attr->process_shared;

  the_cond->Mutex = 0;

/* XXX some more initialization might need to go here */
  _Thread_queue_Initialize(
    &the_cond->Wait_queue,
    OBJECTS_POSIX_CONDITION_VARIABLES,
    THREAD_QUEUE_DISCIPLINE_FIFO,
    STATES_WAITING_FOR_CONDITION_VARIABLE,
    _POSIX_Condition_variables_MP_Send_extract_proxy,
    ETIMEDOUT
  );

  _Objects_Open(
    &_POSIX_Condition_variables_Information,
    &the_cond->Object,
    0
  );
 
  *cond = the_cond->Object.id;
 
  if ( the_attr->process_shared == PTHREAD_PROCESS_SHARED )
    _POSIX_Condition_variables_MP_Send_process_packet(
      POSIX_CONDITION_VARIABLES_MP_ANNOUNCE_CREATE,
      the_cond->Object.id,
      0,                         /* Name not used */
      0                          /* Not used */
    );
 
  _Thread_Enable_dispatch();

  return 0;
}
 
/*PAGE
 *
 *  11.4.2 Initializing and Destroying a Condition Variable, 
 *         P1003.1c/Draft 10, p. 87
 */
 
int pthread_cond_destroy(
  pthread_cond_t           *cond
)
{
  register POSIX_Condition_variables_Control *the_cond;
  Objects_Locations                           location;
 
  the_cond = _POSIX_Condition_variables_Get( cond, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      return EINVAL;
    case OBJECTS_REMOTE:
      _Thread_Dispatch();
      return POSIX_MP_NOT_IMPLEMENTED();
      return EINVAL;
    case OBJECTS_LOCAL:
 
      _Objects_Close(
        &_POSIX_Condition_variables_Information,
        &the_cond->Object
      );
 
      if ( _Thread_queue_Get_number_waiting( &the_cond->Wait_queue ) ) 
        return EBUSY;
 
      _POSIX_Condition_variables_Free( the_cond );
 
      if ( the_cond->process_shared == PTHREAD_PROCESS_SHARED ) {
 
        _Objects_MP_Close(
          &_POSIX_Condition_variables_Information,
          the_cond->Object.id
        );
 
        _POSIX_Condition_variables_MP_Send_process_packet(
          POSIX_CONDITION_VARIABLES_MP_ANNOUNCE_DELETE,
          the_cond->Object.id,
          0,                         /* Not used */
          0                          /* Not used */
        );
      }
      _Thread_Enable_dispatch();
      return 0;
  }
  return POSIX_BOTTOM_REACHED();
}
 
/*PAGE
 *
 *  _POSIX_Condition_variables_Signal_support
 *
 *  A support routine which implements guts of the broadcast and single task
 *  wake up version of the "signal" operation.
 */
 
int _POSIX_Condition_variables_Signal_support(
  pthread_cond_t            *cond,
  boolean                    is_broadcast
)
{
  register POSIX_Condition_variables_Control *the_cond;
  Objects_Locations                           location;
  Thread_Control                             *the_thread;
 
  the_cond = _POSIX_Condition_variables_Get( cond, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      return EINVAL;
    case OBJECTS_REMOTE:
      _Thread_Dispatch();
      return POSIX_MP_NOT_IMPLEMENTED();
      return EINVAL;
    case OBJECTS_LOCAL:
 
      do { 
        the_thread = _Thread_queue_Dequeue( &the_cond->Wait_queue );
      } while ( is_broadcast && the_thread );
      return 0;
  }
  return POSIX_BOTTOM_REACHED();
}

/*PAGE
 *
 *  11.4.3 Broadcasting and Signaling a Condition, P1003.1c/Draft 10, p. 101
 */
 
int pthread_cond_signal(
  pthread_cond_t   *cond
)
{
  return _POSIX_Condition_variables_Signal_support( cond, FALSE );
}
 
/*PAGE
 *
 *  11.4.3 Broadcasting and Signaling a Condition, P1003.1c/Draft 10, p. 101
 */
 
int pthread_cond_broadcast(
  pthread_cond_t   *cond
)
{
  return _POSIX_Condition_variables_Signal_support( cond, TRUE );
}
 
/*PAGE
 *
 *  _POSIX_Condition_variables_Wait_support
 *
 *  A support routine which implements guts of the blocking, non-blocking, and
 *  timed wait version of condition variable wait routines.
 */
 
int _POSIX_Condition_variables_Wait_support(
  pthread_cond_t            *cond,
  pthread_mutex_t           *mutex,
  Watchdog_Interval          timeout
)
{
  register POSIX_Condition_variables_Control *the_cond;
  Objects_Locations                           location;
  int                                         status;
 
  the_cond = _POSIX_Condition_variables_Get( cond, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      return EINVAL;
    case OBJECTS_REMOTE:
      _Thread_Dispatch();
      return POSIX_MP_NOT_IMPLEMENTED();
      return EINVAL;
    case OBJECTS_LOCAL:
 
      /*
       *  XXX: should be an error if cond->Mutex != mutex
       */
 
      status = pthread_mutex_unlock( mutex );
      if ( !status )
        return status;
 
      the_cond->Mutex = *mutex;
 
/* XXX .. enter critical section .. */
      _Thread_queue_Enqueue( &the_cond->Wait_queue, 0 );

      _Thread_Enable_dispatch();

      status = pthread_mutex_lock( mutex );
      if ( !status )
        return status;
    
      return _Thread_Executing->Wait.return_code;
  }
  return POSIX_BOTTOM_REACHED();
}

/*PAGE
 *
 *  11.4.4 Waiting on a Condition, P1003.1c/Draft 10, p. 105
 */
 
int pthread_cond_wait(
  pthread_cond_t     *cond,
  pthread_mutex_t    *mutex
)
{
  return _POSIX_Condition_variables_Wait_support(
    cond,
    mutex,
    THREAD_QUEUE_WAIT_FOREVER
  );
}
 
/*PAGE
 *
 *  11.4.4 Waiting on a Condition, P1003.1c/Draft 10, p. 105
 */
 
int pthread_cond_timedwait(
  pthread_cond_t        *cond,
  pthread_mutex_t       *mutex,
  const struct timespec *abstime
)
{
  return _POSIX_Condition_variables_Wait_support(
    cond,
    mutex,
    _POSIX_Time_Spec_to_interval( abstime )
  );
}
