/*  pthread.h
 *
 *  $Id$
 */

#ifndef __PTHREAD_h
#define __PTHREAD_h

#include <rtems/posix/features.h>

#if defined(_POSIX_THREADS)

#include <sys/types.h>
#include <time.h>
#include <sys/sched.h>

/*
 *  3.1.3 Register Fork Handlers, P1003.1c/Draft 10, P1003.1c/Draft 10, p. 27
 */

int pthread_atfork(
  void (*prepare)(void),
  void (*parent)(void),
  void (*child)(void)
);

/*
 *  11.3.1 Mutex Initialization Attributes, P1003.1c/Draft 10, p. 81
 */

int pthread_mutexattr_init(
  pthread_mutexattr_t *attr
);

int pthread_mutexattr_destroy(
  pthread_mutexattr_t *attr
);

int pthread_mutexattr_getpshared(
  const pthread_mutexattr_t *attr,
  int                       *pshared
);

int pthread_mutexattr_setpshared(
  pthread_mutexattr_t *attr,
  int                  pshared
);

/*
 *  11.3.2 Initializing and Destroying a Mutex, P1003.1c/Draft 10, p. 87
 */

int pthread_mutex_init(
  pthread_mutex_t           *mutex,
  const pthread_mutexattr_t *attr
);

int pthread_mutex_destroy(
  pthread_mutex_t           *mutex
);

/*
 *  This is used to statically initialize a pthread_mutex_t. Example:
 *
 *  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
 */

#define PTHREAD_MUTEX_INITIALIZER  ((pthread_mutex_t) 0xFFFFFFFF)

/*
 *  11.3.3 Locking and Unlocking a Mutex, P1003.1c/Draft 10, p. 93
 *        
 *  NOTE: P1003.4b/D8 adds pthread_mutex_timedlock(), p. 29
 */

int pthread_mutex_lock(
  pthread_mutex_t           *mutex
);

int pthread_mutex_trylock(
  pthread_mutex_t           *mutex
);

int pthread_mutex_unlock(
  pthread_mutex_t           *mutex
);

#if defined(_POSIX_TIMEOUTS)

int pthread_mutex_timedlock(
  pthread_mutex_t       *mutex,
  const struct timespec *timeout
);

#endif /* _POSIX_TIMEOUTS */

/*
 *  11.4.1 Condition Variable Initialization Attributes, 
 *            P1003.1c/Draft 10, p. 96
 */
 
int pthread_condattr_init(
  pthread_condattr_t *attr
);
 
int pthread_condattr_destroy(
  pthread_condattr_t *attr
);
 
int pthread_condattr_getpshared(
  const pthread_condattr_t *attr,
  int                      *pshared
);
 
int pthread_condattr_setpshared(
  pthread_condattr_t  *attr,
  int                  pshared
);
 
/*
 *  11.4.2 Initializing and Destroying a Condition Variable, 
 *         P1003.1c/Draft 10, p. 87
 */
 
int pthread_cond_init(
  pthread_cond_t           *cond,
  const pthread_condattr_t *attr
);
 
int pthread_cond_destroy(
  pthread_cond_t           *mutex
);
 
/*
 *  This is used to statically initialize a pthread_cond_t. Example:
 *
 *  pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
 */
 
#define PTHREAD_COND_INITIALIZER  ((pthread_mutex_t) 0xFFFFFFFF)
 
/*
 *  11.4.3 Broadcasting and Signaling a Condition, P1003.1c/Draft 10, p. 101
 */
 
int pthread_cond_signal(
  pthread_cond_t   *cond
);
 
int pthread_cond_broadcast(
  pthread_cond_t   *cond
);
 
/*
 *  11.4.4 Waiting on a Condition, P1003.1c/Draft 10, p. 105
 */
 
int pthread_cond_wait(
  pthread_cond_t     *cond,
  pthread_mutex_t    *mutex
);
 
int pthread_cond_timedwait(
  pthread_cond_t        *cond,
  pthread_mutex_t       *mutex,
  const struct timespec *abstime
);
 
#if defined(_POSIX_THREAD_PRIORITY_SCHEDULING)

/*
 *  13.5.1 Thread Creation Scheduling Attributes, P1003.1c/Draft 10, p. 120
 */

int pthread_attr_setscope(
  pthread_attr_t  *attr,
  int              contentionscope
);

int pthread_attr_getscope(
  const pthread_attr_t  *attr,
  int                   *contentionscope
);

#define PTHREAD_INHERIT_SCHED  0      /* scheduling policy and associated */
                                      /*   attributes are inherited from */
                                      /*   the calling thread. */
#define PTHREAD_EXPLICIT_SCHED 0      /* set from provided attribute object */

int pthread_attr_setinheritsched(
  pthread_attr_t  *attr,
  int              inheritsched
);

int pthread_attr_getinheritsched(
  const pthread_attr_t  *attr,
  int                   *inheritsched
);

int pthread_attr_setschedpolicy(
  pthread_attr_t  *attr,
  int              policy
);

int pthread_attr_getschedpolicy(
  const pthread_attr_t  *attr,
  int                   *policy
);

#endif /* defined(_POSIX_THREAD_PRIORITY_SCHEDULING) */

int pthread_attr_setschedparam(
  pthread_attr_t            *attr,
  const struct sched_param   param
);

int pthread_attr_getschedparam(
  const pthread_attr_t  *attr,
  struct sched_param    *param
);

#if defined(_POSIX_THREAD_PRIORITY_SCHEDULING)

/*
 *  13.5.2 Dynamic Thread Scheduling Parameters Access, 
 *         P1003.1c/Draft 10, p. 124
 */

int pthread_getschedparam(
  pthread_t           thread,
  int                *policy,
  struct sched_param *param
);

int pthread_setschedparam(
  pthread_t           thread,
  int                 policy,
  struct sched_param *param
);

#endif /* defined(_POSIX_THREAD_PRIORITY_SCHEDULING) */

#if defined(_POSIX_THREAD_PRIO_INHERIT) || defined(_POSIX_THREAD_PRIO_PROTECT)

/*
 *  13.6.1 Mutex Initialization Scheduling Attributes, P1003.1c/Draft 10, p. 128
 */
 
/*
 *  Values for protocol.
 */

#define PTHREAD_PRIO_NONE    0
#define PTHREAD_PRIO_INHERIT 1
#define PTHREAD_PRIO_PROTECT 2

int pthread_mutexattr_setprotocol(
  pthread_mutexattr_t   *attr,
  int                    protocol
);

int pthread_mutexattr_getprotocol(
  const pthread_mutexattr_t   *attr,
  int                         *protocol
);

int pthread_mutexattr_setprioceiling(
  pthread_mutexattr_t   *attr,
  int                    prioceiling
);

int pthread_mutexattr_getprioceiling(
  const pthread_mutexattr_t   *attr,
  int                         *prioceiling
);

#endif /* _POSIX_THREAD_PRIO_INHERIT || _POSIX_THREAD_PRIO_PROTECT */

#if defined(_POSIX_THREAD_PRIO_PROTECT)

/*
 *  13.6.2 Change the Priority Ceiling of a Mutex, P1003.1c/Draft 10, p. 131
 */

int pthread_mutex_setprioceiling(
  pthread_mutex_t   *mutex,
  int                prioceiling,
  int               *old_ceiling
);
 
int pthread_mutex_getprioceiling(
  pthread_mutex_t   *mutex,
  int               *prioceiling
);

#endif /* _POSIX_THREAD_PRIO_PROTECT */

/*
 *  16.1.1 Thread Creation Attributes, P1003.1c/Draft 10, p, 140
 */

int pthread_attr_init(
  pthread_attr_t  *attr
);

int pthread_attr_destroy(
  pthread_attr_t  *attr
);
 
int pthread_attr_getstacksize(
  const pthread_attr_t  *attr,
  size_t                *stacksize
);
 
int pthread_attr_setstacksize(
  pthread_attr_t  *attr,
  size_t           stacksize
);
 
int pthread_attr_getstackaddr(
  const pthread_attr_t   *attr,
  void                  **stackaddr
);
 
int pthread_attr_setstackaddr(
  pthread_attr_t  *attr,
  void            *stackaddr
);
 
int pthread_attr_getdetachstate(
  const pthread_attr_t  *attr,
  int                   *detachstate
);
 
int pthread_attr_setdetachstate(
  pthread_attr_t  *attr,
  int              detachstate
);

/*
 *  16.1.2 Thread Creation, P1003.1c/Draft 10, p. 144
 */

int pthread_create(
  pthread_t             *thread,
  const pthread_attr_t  *attr,
  void                 (*start_routine)( void * ),
  void                  *arg
);

/*
 *  16.1.3 Wait for Thread Termination, P1003.1c/Draft 10, p. 147
 */

int pthread_join(
  pthread_t   thread,
  void      **value_ptr
);

/*
 *  16.1.4 Detaching a Thread, P1003.1c/Draft 10, p. 149
 */

int pthread_detach(
  pthread_t   thread
);

/*
 * 16.1.6 Get Calling Thread's ID, p1003.1c/Draft 10, p. XXX
 */

pthread_t pthread_self( void );

/*
 *  16.1.7 Compare Thread IDs, p1003.1c/Draft 10, p. 153
 */

int pthread_equal( 
  pthread_t  t1,
  pthread_t  t2
);

/*
 *  16.1.8 Dynamic Package Initialization
 */

/*
 *  This is used to statically initialize a pthread_once_t. Example:
 *
 *  pthread_once_t once = PTHREAD_ONCE_INITIALIZER;
 */
 
#define PTHREAD_ONCE_INITIALIZER  { TRUE, FALSE }
 
int pthread_once(
  pthread_once_t  *once_control,
  void           (*init_routine)(void)
);

/*
 *  17.1.1 Thread-Specific Data Key Create, P1003.1c/Draft 10, p. 163
 */

int pthread_key_create(
  pthread_key_t  *key,
  void          (*destructor)( void * )
);

/*
 *  17.1.2 Thread-Specific Data Management, P1003.1c/Draft 10, p. 165
 */

int pthread_setspecific(
  pthread_key_t  key,
  const void    *value
);

void *pthread_getspecific(
  pthread_key_t  key
);

/*
 *  17.1.3 Thread-Specific Data Key Deletion, P1003.1c/Draft 10, p. 167
 */

int pthread_key_delete(
  pthread_key_t  key
);

/*
 *  18.2.1 Canceling Execution of a Thread, P1003.1c/Draft 10, p. 181
 */

#define PTHREAD_CANCEL_ENABLE  0
#define PTHREAD_CANCEL_DISABLE 1

#define PTHREAD_CANCEL_DEFERRED 0
#define PTHREAD_CANCEL_ASYNCHRONOUS 1

int pthread_cancel(
  pthread_t  thread
);

/*
 *  18.2.2 Setting Cancelability State, P1003.1c/Draft 10, p. 183
 */

int pthread_setcancelstate( 
  int  state,
  int *oldstate
);

int pthread_setcanceltype(
  int  type,
  int *oldtype
);

void pthread_testcancel( void );

/*
 *  18.2.3.1 Establishing Cancellation Handlers, P1003.1c/Draft 10, p. 184
 */

void pthread_cleanup_push(
  void   (*routine)( void * ),
  void    *arg
);

void pthread_cleanup_pop(
  int    execute
);

#if defined(_POSIX_THREAD_CPUTIME)
 
/*
 *  20.1.6 Accessing a Thread CPU-time Clock, P1003.4b/D8, p. 58
 */
 
int pthread_getcpuclockid(
  pthread_t      pid,
  clockid_t *clock_id
);
 
/*
 *  20.1.7 CPU-time Clock Thread Creation Attribute, P1003.4b/D8, p. 59
 */

int pthread_attr_setcputime(
  pthread_attr_t  *attr,
  int              clock_allowed
);

int pthread_attr_getcputime(
  pthread_attr_t  *attr,
  int             *clock_allowed
);

#endif /* defined(_POSIX_THREAD_CPUTIME) */

#endif /* defined(_POSIX_THREADS) */
#endif
/* end of include file */
