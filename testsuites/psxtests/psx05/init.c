/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sched.h>

#define CONFIGURE_INIT
#include "system.h"
#include <errno.h>
#include <rtems/posix/priority.h>

#define MUTEX_BAD_ID 0xfffffffe

void Print_mutexattr(
  char                *msg,
  pthread_mutexattr_t *attr
);

void calculate_abstimeout(
  struct timespec *times,
  uint32_t         seconds,
  uint32_t         nanoseconds
);

void Print_mutexattr(
  char                *msg,
  pthread_mutexattr_t *attr
)
{
  int status;
  int protocol;
  int prioceiling;
  int pshared;

  /* protocol */

  status = pthread_mutexattr_getprotocol( attr, &protocol );
  rtems_test_assert( !status );

  printf( "%smutex protocol is (%d) -- ", msg, protocol );
  switch ( protocol ) {
    case PTHREAD_PRIO_NONE:
      puts( "PTHREAD_PRIO_NONE" );
      break;
    case PTHREAD_PRIO_INHERIT:
      puts( "PTHREAD_PRIO_INHERIT" );
      break;
    case PTHREAD_PRIO_PROTECT:
      puts( "PTHREAD_PRIO_PROTECT" );
      break;
    default:
      puts( "UNKNOWN" );
      rtems_test_assert( 0 );
      break;
  }

  /* priority ceiling */

  status = pthread_mutexattr_getprioceiling( attr, &prioceiling );
  rtems_test_assert( !status );
  printf( "%smutex priority ceiling is %d\n", msg, prioceiling );

  /* process shared */

  status = pthread_mutexattr_getpshared( attr, &pshared );
  rtems_test_assert( !status );
  printf( "%smutex process shared is (%d) -- ", msg, pshared );
  switch ( pshared ) {
    case PTHREAD_PROCESS_PRIVATE:
      puts( "PTHREAD_PROCESS_PRIVATE" );
      break;
    case PTHREAD_PROCESS_SHARED:
      puts( "PTHREAD_PROCESS_SHARED" );
      break;
    default:
      puts( "UNKNOWN" );
      rtems_test_assert( 0 );
      break;
  }
}

void calculate_abstimeout(
  struct timespec *times,
  uint32_t         seconds,
  uint32_t         nanoseconds
)
{
  struct timeval       tv1;
  struct timezone      tz1;

  gettimeofday( &tv1, &tz1 );

  times->tv_sec  = seconds     + tv1.tv_sec;
  times->tv_nsec = nanoseconds + (tv1.tv_usec * 1000);

  while ( times->tv_nsec >= TOD_NANOSECONDS_PER_SECOND ) {
    times->tv_sec++;
    times->tv_nsec -= TOD_NANOSECONDS_PER_SECOND;
  }

}

void *POSIX_Init(
  void *argument
)
{
  int                  status;
  pthread_mutexattr_t  attr;
  pthread_mutexattr_t  destroyed_attr;
  struct timespec      times;
  struct sched_param   param;
  int                  pshared;
  int                  policy;
  int                  protocol;
  int                  ceiling;
  int                  old_ceiling;
  int                  priority;

  rtems_test_assert( MUTEX_BAD_ID != PTHREAD_MUTEX_INITIALIZER );
  Mutex_bad_id = MUTEX_BAD_ID;

  puts( "\n\n*** POSIX TEST 5 ***" );

  /* set the time of day, and print our buffer in multiple ways */

  set_time( TM_FRIDAY, TM_MAY, 24, 96, 11, 5, 0 );

  /* get id of this thread */

  Init_id = pthread_self();
  printf( "Init's ID is 0x%08" PRIxpthread_t "\n", Init_id );

  /* tes pthread_mutex_attr_init */

  puts( "Init: pthread_mutexattr_init - EINVAL (NULL attr)" );
  status = pthread_mutexattr_init( NULL );
  rtems_test_assert( status == EINVAL );

  puts( "Init: pthread_mutexattr_init - SUCCESSFUL" );
  status = pthread_mutexattr_init( &attr );
  rtems_test_assert( !status );

  Print_mutexattr( "Init: ", &attr );

  /* create an "uninitialized" attribute structure */

  status = pthread_mutexattr_init( &destroyed_attr );
  rtems_test_assert( !status );

  puts( "Init: pthread_mutexattr_destroy - SUCCESSFUL" );
  status = pthread_mutexattr_destroy( &destroyed_attr );
  rtems_test_assert( !status );

  puts( "Init: pthread_mutexattr_destroy - EINVAL (NULL attr)" );
  status = pthread_mutexattr_destroy( NULL );
  rtems_test_assert( status == EINVAL );

  puts( "Init: pthread_mutexattr_destroy - EINVAL (not initialized)" );
  status = pthread_mutexattr_destroy( &destroyed_attr );
  rtems_test_assert( status == EINVAL );

  /* error cases for set and get pshared attribute */

  empty_line();

  puts( "Init: pthread_mutexattr_getpshared - EINVAL (NULL attr)" );
  status = pthread_mutexattr_getpshared( NULL, &pshared );
  rtems_test_assert( status == EINVAL );

  puts( "Init: pthread_mutexattr_getpshared - EINVAL (NULL pshared)" );
  status = pthread_mutexattr_getpshared( &attr, NULL );
  rtems_test_assert( status == EINVAL );

  puts( "Init: pthread_mutexattr_getpshared - EINVAL (not initialized)" );
  status = pthread_mutexattr_getpshared( &destroyed_attr, &pshared );
  rtems_test_assert( status == EINVAL );

  pshared = PTHREAD_PROCESS_PRIVATE;
  puts( "Init: pthread_mutexattr_setpshared - EINVAL (NULL attr)" );
  status = pthread_mutexattr_setpshared( NULL, pshared );
  rtems_test_assert( status == EINVAL );

  pshared = PTHREAD_PROCESS_PRIVATE;
  puts( "Init: pthread_mutexattr_setpshared - EINVAL (not initialized)" );
  status = pthread_mutexattr_setpshared( &destroyed_attr, pshared );
  rtems_test_assert( status == EINVAL );

  /* error cases for set and get protocol attribute */

  empty_line();

  puts( "Init: pthread_mutexattr_getprotocol - EINVAL (NULL attr)" );
  status = pthread_mutexattr_getprotocol( NULL, &protocol );
  rtems_test_assert( status == EINVAL );

  puts( "Init: pthread_mutexattr_getprotocol - EINVAL (NULL protocol)" );
  status = pthread_mutexattr_getprotocol( &attr, NULL );
  rtems_test_assert( status == EINVAL );

  puts( "Init: pthread_mutexattr_getprotocol - EINVAL (not initialized)" );
  status = pthread_mutexattr_getprotocol( &destroyed_attr, &protocol );
  rtems_test_assert( status == EINVAL );

  puts( "Init: pthread_mutexattr_setprotocol - EINVAL (NULL attr)" );
  status = pthread_mutexattr_setprotocol( NULL, PTHREAD_PRIO_NONE );
  rtems_test_assert( status == EINVAL );

  puts( "Init: pthread_mutexattr_setprotocol - EINVAL (invalid protocol)" );
  status = pthread_mutexattr_setprotocol( &attr, -1 );
  rtems_test_assert( status == EINVAL );

  puts( "Init: pthread_mutexattr_setprotocol - EINVAL (not initialized)" );
  status = pthread_mutexattr_setprotocol( &destroyed_attr, -1 );
  rtems_test_assert( status == EINVAL );

  /* error cases for set and get prioceiling attribute */

  empty_line();

  puts( "Init: pthread_mutexattr_getprioceiling - EINVAL (NULL attr)" );
  status = pthread_mutexattr_getprioceiling( NULL, &ceiling );
  rtems_test_assert( status == EINVAL );

  puts( "Init: pthread_mutexattr_getprioceiling - EINVAL (NULL prioceiling)" );
  status = pthread_mutexattr_getprioceiling( &attr, NULL );
  rtems_test_assert( status == EINVAL );

  puts( "Init: pthread_mutexattr_getprioceiling - EINVAL (not initialized)" );
  status = pthread_mutexattr_getprioceiling( &destroyed_attr, &ceiling );
  rtems_test_assert( status == EINVAL );

  puts( "Init: pthread_mutexattr_setprioceiling - EINVAL (NULL attr)" );
  status = pthread_mutexattr_setprioceiling( NULL, 128 );
  rtems_test_assert( status == EINVAL );

  puts( "Init: pthread_mutexattr_setprioceiling - EINVAL (invalid priority)" );
  status = pthread_mutexattr_setprioceiling( &attr, 512 );
  if ( status != EINVAL )
    printf( "status = %d\n", status );
  rtems_test_assert( status == EINVAL );

  puts( "Init: pthread_mutexattr_setprioceiling - EINVAL (not initialized)" );
  status = pthread_mutexattr_setprioceiling( &destroyed_attr, -1 );
  rtems_test_assert( status == EINVAL );

  /* create a thread */

  status = pthread_create( &Task_id, NULL, Task_1, NULL );
  rtems_test_assert( !status );

  /* now try some basic mutex operations */

  empty_line();

  puts( "Init: pthread_mutex_init - EINVAL (NULL mutex_id)" );
  status = pthread_mutex_init( NULL, &attr );
  rtems_test_assert( status == EINVAL );

  puts( "Init: pthread_mutex_init - EINVAL (not initialized attr)" );
  status = pthread_mutex_init( &Mutex_id, &destroyed_attr );
  rtems_test_assert( status == EINVAL );

  /* must get around error checks in attribute set routines */
  attr.protocol = -1;

  puts( "Init: pthread_mutex_init - EINVAL (bad protocol)" );
  status = pthread_mutex_init( &Mutex_id, &attr );
  rtems_test_assert( status == EINVAL );

  /* must get around error checks in attribute set routines */
  attr.protocol = PTHREAD_PRIO_INHERIT;
  attr.prio_ceiling = -1;

  puts( "Init: pthread_mutex_init - EINVAL (bad priority ceiling)" );
  status = pthread_mutex_init( &Mutex_id, &attr );
  rtems_test_assert( status == EINVAL );

  /* must get around various error checks before checking bad scope */
  puts( "Init: Resetting mutex attributes" );
  status = pthread_mutexattr_init( &attr );
  rtems_test_assert( !status );

  puts( "Init: pthread_mutex_init - ENOSYS (process wide scope)" );
  attr.process_shared = PTHREAD_PROCESS_SHARED;
  status = pthread_mutex_init( &Mutex_id, &attr );
  rtems_test_assert( status == ENOSYS );

  puts( "Init: pthread_mutex_init - EINVAL (invalid scope)" );
  attr.process_shared = -1;
  status = pthread_mutex_init( &Mutex_id, &attr );
  rtems_test_assert( status == EINVAL );

  /* bad kind */
  status = pthread_mutexattr_init( &attr );
  rtems_test_assert( !status );

  puts( "Init: pthread_mutex_init - EINVAL (invalid type)" );
  attr.type = -1;
  status = pthread_mutex_init( &Mutex_id, &attr );
  rtems_test_assert( status == EINVAL );

  /* now set up for a success pthread_mutex_init */

  puts( "Init: Resetting mutex attributes" );
  status = pthread_mutexattr_init( &attr );
  rtems_test_assert( !status );

  puts( "Init: Changing mutex attributes" );
  status = pthread_mutexattr_setprotocol( &attr, PTHREAD_PRIO_INHERIT );
  rtems_test_assert( !status );

  status = pthread_mutexattr_setprioceiling(
    &attr,
    (sched_get_priority_max(SCHED_FIFO) / 2) + 1
  );
  rtems_test_assert( !status );

  status = pthread_mutexattr_setpshared( &attr, PTHREAD_PROCESS_SHARED );
  rtems_test_assert( !status );

  Print_mutexattr( "Init: ", &attr );

  puts( "Init: Resetting mutex attributes" );
  status = pthread_mutexattr_init( &attr );
  rtems_test_assert( !status );

  /*
   *  Set the protocol to priority ceiling so the owner check happens
   *  and the EPERM test (later) will work.
   */

  status = pthread_mutexattr_setprotocol( &attr, PTHREAD_PRIO_INHERIT );
  rtems_test_assert( !status );

  puts( "Init: pthread_mutex_init - SUCCESSFUL" );
  status = pthread_mutex_init( &Mutex_id, &attr );
  if ( status )
    printf( "status = %d\n", status );
  rtems_test_assert( !status );

  /*
   *  This is not required to be an error and when it is, there are
   *  behavioral conflicts with other implementations.
   */
  puts( "Init: pthread_mutex_init - EBUSY (reinitialize an existing mutex) - skipped" );

#if 0
  status = pthread_mutex_init( &Mutex_id, &attr );
  if ( !status )
    printf( "status = %d\n", status );
  rtems_test_assert( status == EBUSY );
#endif

  puts( "Init: pthread_mutex_trylock - EINVAL (illegal ID)" );
  status = pthread_mutex_trylock( &Mutex_bad_id );
  if ( status != EINVAL )
    printf( "status = %d\n", status );
  rtems_test_assert( status == EINVAL );

  puts( "Init: pthread_mutex_trylock - SUCCESSFUL" );
  status = pthread_mutex_trylock( &Mutex_id );
  if ( status )
    printf( "status = %d\n", status );
  rtems_test_assert( !status );

  puts( "Init: pthread_mutex_trylock - EDEADLK (already locked)" );
  status = pthread_mutex_trylock( &Mutex_id );
  if ( status != EDEADLK )
    printf( "status = %d\n", status );
  rtems_test_assert( status == EDEADLK );

  puts( "Init: pthread_mutex_lock - EINVAL (NULL id)" );
  status = pthread_mutex_lock( NULL );
  if ( status != EINVAL )
    printf( "status = %d\n", status );
  rtems_test_assert( status == EINVAL );

  puts( "Init: pthread_mutex_unlock - EINVAL (NULL id)" );
  status = pthread_mutex_unlock( NULL );
  if ( status != EINVAL )
    printf( "status = %d\n", status );
  rtems_test_assert( status == EINVAL );

  puts( "Init: pthread_mutex_lock - EDEADLK (already locked)" );
  status = pthread_mutex_lock( &Mutex_id );
  if ( status != EDEADLK )
    printf( "status = %d\n", status );
  rtems_test_assert( status == EDEADLK );

  puts( "Init: Sleep 1 second" );

  sleep( 1 );

     /* switch to task 1 */

  puts( "Init: pthread_mutex_unlock - EINVAL (invalid id)" );
  status = pthread_mutex_unlock( &Mutex_bad_id );
  if ( status != EINVAL )
    printf( "status = %d\n", status );
  rtems_test_assert( status == EINVAL );

  puts( "Init: pthread_mutex_unlock - SUCCESSFUL" );
  status = pthread_mutex_unlock( &Mutex_id );
  if ( status )
    printf( "status = %d\n", status );
  rtems_test_assert( !status );

  puts( "Init: pthread_mutex_unlock - EPERM (not owner)" );
  status = pthread_mutex_unlock( &Mutex_id );
  if ( status != EPERM )
    printf( "status = %d\n", status );
  rtems_test_assert( status == EPERM );

  puts( "Init: pthread_mutex_timedlock - time out in 1/2 second" );
  calculate_abstimeout( &times, 0, (TOD_NANOSECONDS_PER_SECOND / 2) );

  status = pthread_mutex_timedlock( &Mutex_id, &times );
  if ( status != ETIMEDOUT )
    printf( "status = %d\n", status );
  rtems_test_assert( status == ETIMEDOUT );

  puts( "Init: pthread_mutex_timedlock - time out in the past" );
  calculate_abstimeout( &times, -1, (TOD_NANOSECONDS_PER_SECOND / 2) );

  status = pthread_mutex_timedlock( &Mutex_id, &times );
  if ( status != ETIMEDOUT )
    printf( "status = %d\n", status );
  rtems_test_assert( status == ETIMEDOUT );

     /* switch to idle */

  puts( "Init: pthread_mutex_timedlock - EAGAIN (timeout)" );

  /* destroy a mutex */

  empty_line();

  puts( "Init: pthread_mutex_init - SUCCESSFUL" );
  status = pthread_mutex_init( &Mutex2_id, &attr );
  if ( status )
    printf( "status = %d\n", status );
  rtems_test_assert( !status );

  puts( "Init: pthread_mutex_init - EAGAIN (too many)" );
  status = pthread_mutex_init( &Mutex3_id, &attr );
  rtems_test_assert( status == EAGAIN );

  puts( "Init: pthread_mutexattr_destroy - SUCCESSFUL" );
  status = pthread_mutexattr_destroy( &attr );
  rtems_test_assert( !status );

  puts( "Init: pthread_mutex_destroy - SUCCESSFUL" );
  status = pthread_mutex_destroy( &Mutex2_id );
  rtems_test_assert( !status );

  puts( "Init: pthread_mutex_destroy - EINVAL (invalid id)" );
  status = pthread_mutex_destroy( &Mutex_bad_id );
  rtems_test_assert( status == EINVAL );

  /* destroy a busy mutex */

  empty_line();

  puts( "Init: pthread_mutexattr_init - SUCCESSFUL" );
  status = pthread_mutexattr_init( &attr );
  rtems_test_assert( !status );

  puts( "Init: pthread_mutex_init - SUCCESSFUL" );
  status = pthread_mutex_init( &Mutex2_id, &attr );
  rtems_test_assert( !status );

  puts( "Init: pthread_mutex_trylock - SUCCESSFUL" );
  status = pthread_mutex_trylock( &Mutex2_id );
  if ( status )
    printf( "status = %d\n", status );
  rtems_test_assert( !status );

  puts( "Init: pthread_mutex_destroy - EBUSY (already locked)" );
  status = pthread_mutex_destroy( &Mutex2_id );
  if ( status != EBUSY )
    printf( "status = %d\n", status );
  rtems_test_assert( status == EBUSY );

  puts( "Init: pthread_mutex_unlock - SUCCESSFUL" );
  status = pthread_mutex_unlock( &Mutex2_id );
  rtems_test_assert( !status );

  puts( "Init: pthread_mutex_destroy - SUCCESSFUL" );
  status = pthread_mutex_destroy( &Mutex2_id );
  rtems_test_assert( !status );

  /* priority inherit mutex */

  empty_line();

  puts( "Init: pthread_mutexattr_init - SUCCESSFUL" );
  status = pthread_mutexattr_init( &attr );
  rtems_test_assert( !status );

  puts(
    "Init: pthread_mutexattr_setprotocol - SUCCESSFUL (PTHREAD_PRIO_INHERIT)"
  );
  status = pthread_mutexattr_setprotocol( &attr, PTHREAD_PRIO_INHERIT );
  rtems_test_assert( !status );

  puts( "Init: pthread_mutex_init - SUCCESSFUL" );
  status = pthread_mutex_init( &Mutex2_id, &attr );
  rtems_test_assert( !status );

  puts( "Init: pthread_mutex_trylock - SUCCESSFUL" );
  status = pthread_mutex_trylock( &Mutex2_id );
  rtems_test_assert( !status );

  /* create a thread at a lower priority */

  status = pthread_create( &Task2_id, NULL, Task_2, NULL );
  rtems_test_assert( !status );

  /* set priority of Task2 to highest priority */

  param.sched_priority = sched_get_priority_max( SCHED_FIFO );

  puts( "Init: pthread_setschedparam - Setting Task2 priority to highest" );
  status = pthread_setschedparam( Task2_id, SCHED_FIFO, &param );
  rtems_test_assert( !status );

  /* switching to Task2 */

  status = pthread_getschedparam( pthread_self(), &policy, &param );
  rtems_test_assert( !status );
  printf( "Init: pthread_getschedparam - priority = %d\n", param.sched_priority);

  puts( "Init: pthread_mutex_unlock - SUCCESSFUL" );
  status = pthread_mutex_unlock( &Mutex2_id );
  rtems_test_assert( !status );

  puts( "Init: pthread_mutexattr_destroy - SUCCESSFUL" );
  status = pthread_mutexattr_destroy( &attr );
  rtems_test_assert( !status );

  puts( "Init: pthread_mutex_destroy - SUCCESSFUL" );
  status = pthread_mutex_destroy( &Mutex2_id );
  rtems_test_assert( !status );

  /* priority ceiling mutex */

  empty_line();

  puts( "Init: pthread_mutexattr_init - SUCCESSFUL" );
  status = pthread_mutexattr_init( &attr );
  rtems_test_assert( !status );

  puts(
    "Init: pthread_mutexattr_setprotocol - SUCCESSFUL (PTHREAD_PRIO_PROTECT)"
  );
  status = pthread_mutexattr_setprotocol( &attr, PTHREAD_PRIO_PROTECT );
  rtems_test_assert( !status );

  puts( "Init: pthread_mutex_init - SUCCESSFUL" );
  status = pthread_mutex_init( &Mutex2_id, &attr );
  rtems_test_assert( !status );

  puts( "Init: pthread_mutex_getprioceiling - EINVAL (invalid id)" );
  status = pthread_mutex_getprioceiling( &Mutex_bad_id, &ceiling );
  rtems_test_assert( status == EINVAL );

  puts( "Init: pthread_mutex_getprioceiling - EINVAL (NULL ceiling)" );
  status = pthread_mutex_getprioceiling( &Mutex2_id, NULL );
  rtems_test_assert( status == EINVAL );

  status = pthread_mutex_getprioceiling( &Mutex2_id, &ceiling );
  rtems_test_assert( !status );
  printf( "Init: pthread_mutex_getprioceiling - %d\n", ceiling );

  puts( "Init: pthread_mutex_setprioceiling - EINVAL (invalid id)" );
  status = pthread_mutex_setprioceiling( &Mutex_bad_id, 200, &old_ceiling );
  rtems_test_assert( status == EINVAL );

  puts( "Init: pthread_mutex_setprioceiling - EINVAL (illegal priority)" );
  status = pthread_mutex_setprioceiling( &Mutex2_id, 512, &old_ceiling );
  rtems_test_assert( status == EINVAL );

  puts( "Init: pthread_mutex_setprioceiling - EINVAL (NULL ceiling)" );
  status = pthread_mutex_setprioceiling( &Mutex2_id, 128, NULL );
  rtems_test_assert( status == EINVAL );

  /* normal cases of set priority ceiling */

  priority = sched_get_priority_max( SCHED_FIFO );
  priority = (priority == 254) ? 200 : 13;

  printf( "Init: pthread_mutex_setprioceiling - new ceiling = %d\n", priority );
  status = pthread_mutex_setprioceiling( &Mutex2_id, priority, &old_ceiling );
  rtems_test_assert( !status );
  printf(
    "Init: pthread_mutex_setprioceiling - old ceiling = %d\n",old_ceiling
  );

  status = pthread_getschedparam( pthread_self(), &policy, &param );
  rtems_test_assert( !status );
  printf(
    "Init: pthread_getschedparam - priority = %d\n", param.sched_priority
  );

  puts( "Init: pthread_mutex_trylock - SUCCESSFUL" );
  status = pthread_mutex_trylock( &Mutex2_id );
  rtems_test_assert( !status );

  status = pthread_getschedparam( pthread_self(), &policy, &param );
  rtems_test_assert( !status );
  printf(
    "Init: pthread_getschedparam - priority = %d\n", param.sched_priority
  );

  /* create a thread at a higher priority */

  status = pthread_create( &Task3_id, NULL, Task_3, NULL );
  rtems_test_assert( !status );

  /* set priority of Task3 to highest priority */

  param.sched_priority = --priority;

  status = pthread_setschedparam( Task3_id, SCHED_FIFO, &param );
  rtems_test_assert( !status );
  puts( "Init: pthread_setschedparam - set Task3 priority to highest" );

  /* DOES NOT SWITCH to Task3 */

  puts( "Init: Sleep 1 second" );
  rtems_test_assert( !status );
  sleep( 1 );

  /* switch to task 3 */

  puts( "Init: pthread_mutex_unlock - SUCCESSFUL" );
  status = pthread_mutex_unlock( &Mutex2_id );
  rtems_test_assert( !status );

  status = pthread_mutex_getprioceiling( &Mutex2_id, &ceiling );
  rtems_test_assert( !status );
  printf( "Init: pthread_mutex_getprioceiling- ceiling = %d\n", ceiling );

  /* set priority of Init to highest priority */

  param.sched_priority = sched_get_priority_max(SCHED_FIFO);

  status = pthread_setschedparam( Init_id, SCHED_FIFO, &param );
  rtems_test_assert( !status );
  puts( "Init: pthread_setschedparam - set Init priority to highest" );

  puts( "Init: pthread_mutex_lock - EINVAL (priority ceiling violation)" );
  status = pthread_mutex_lock( &Mutex2_id );
  if ( status != EINVAL )
    printf( "status = %d\n", status );
  rtems_test_assert( status == EINVAL );

  /* mutexinit.c: Initialising recursive mutex */

  puts( "Init: Recursive Mutex" );

  status = pthread_mutex_destroy( &Mutex2_id );
  if( status )
    printf( "status mutex destroy:%d\n", status );

  status = pthread_mutexattr_init( &attr );
  if( status )
    printf( "status mutexattr:%d\n", status );

  attr.recursive=true;
  status = pthread_mutex_init( &Mutex2_id, &attr );
  if ( status )
    printf( "status recursive mutex :%d\n", status );
  rtems_test_assert( !status );

  puts( "*** END OF POSIX TEST 5 ***" );
  rtems_test_exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}
