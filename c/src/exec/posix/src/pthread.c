/*
 *  $Id$
 */

#include <errno.h>
#include <pthread.h>
#include <limits.h>

#include <rtems/system.h>
#include <rtems/score/stack.h>
#include <rtems/score/thread.h>
#include <rtems/posix/pthread.h>

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
  unsigned32 maximum_pthreads
)
{
  _Objects_Initialize_information(
    &_POSIX_Threads_Information,
    OBJECTS_POSIX_THREADS,
    TRUE,
    maximum_pthreads,
    sizeof( POSIX_Threads_Control ),
    TRUE,
    _POSIX_PATH_MAX,
    TRUE
  );
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

  attr->contentionscope = contentionscope;
  return 0;
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

  attr->inheritsched = inheritsched;
  return 0;
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

  attr->schedpolicy = policy;
  return 0;
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
  pthread_attr_t *attr;   /* XXX: really need to get this from the thread */

  if ( !policy || !param  )
    return EINVAL;

  *policy = attr->schedpolicy;
  *param  = attr->schedparam;
  return 0;
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
  /* XXX need to reschedule after doing this to the thread */
  pthread_attr_t *attr;   /* XXX: really need to get this from the thread */

  if ( !param )
    return EINVAL;

  attr->schedpolicy = policy;
  attr->schedparam  = *param;
  return 0;
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

  attr->detachstate = detachstate;
  return 0;
}

#ifdef NOT_IMPLEMENTED_YET

/*PAGE
 *
 *  16.1.2 Thread Creation, P1003.1c/Draft 10, p. 144
 */

int pthread_create(
  pthread_t             *thread,
  const pthread_attr_t  *attr,
  void                 (*start_routine)( void * ),
  void                  *arg
)
{
  return POSIX_NOT_IMPLEMENTED();
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
  return POSIX_NOT_IMPLEMENTED();
}

/*PAGE
 *
 *  16.1.4 Detaching a Thread, P1003.1c/Draft 10, p. 149
 */

int pthread_detach(
  pthread_t   thread
)
{
  return POSIX_NOT_IMPLEMENTED();
}

#endif

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
#ifdef RTEMS_DEBUG
 /* XXX may want to do a "get" to make sure both are valid. */
 /* XXX behavior is undefined if not valid pthread_t's */
#endif
  return _Objects_Are_ids_equal( t1, t1 ); 
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

#ifdef NOT_IMPLEMENTED_YET

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

#endif
 
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

  attr->cputime_clock_allowed = clock_allowed;
  return 0;
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
