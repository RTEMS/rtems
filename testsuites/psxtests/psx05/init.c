/* 
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#define CONFIGURE_INIT
#include "system.h"
#include <errno.h>

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
  assert( !status );

  printf( "%smutex protocol is (%d) -- ", msg, protocol );
  switch ( protocol ) {
    case PTHREAD_PRIO_NONE:
      printf( "PTHREAD_PRIO_NONE\n" );
      break;
    case PTHREAD_PRIO_INHERIT:
      printf( "PTHREAD_PRIO_INHERIT\n" );
      break;
    case PTHREAD_PRIO_PROTECT:
      printf( "PTHREAD_PRIO_PROTECT\n" );
      break;
    default:
      printf( "UNKNOWN\n" );
      assert( 0 );
      break;
  }

  /* priority ceiling */

  status = pthread_mutexattr_getprioceiling( attr, &prioceiling );
  assert( !status );
  printf( "%smutex priority ceiling is %d\n", msg, prioceiling );

  /* process shared */

  status = pthread_mutexattr_getpshared( attr, &pshared );
  assert( !status );
  printf( "%smutex process shared is (%d) -- ", msg, pshared );
  switch ( pshared ) {
    case PTHREAD_PROCESS_PRIVATE:
      printf( "PTHREAD_PROCESS_PRIVATE\n" );
      break;
    case PTHREAD_PROCESS_SHARED:
      printf( "PTHREAD_PROCESS_SHARED\n" );
      break;
    default:
      printf( "UNKNOWN\n" );
      assert( 0 );
      break;
  }
}

void *POSIX_Init(
  void *argument
)
{
  int                  status;
  pthread_mutexattr_t  attr;

  puts( "\n\n*** POSIX TEST 5 ***" );

  /* set the time of day, and print our buffer in multiple ways */

  set_time( TM_FRIDAY, TM_MAY, 24, 96, 11, 5, 0 );

  /* get id of this thread */

  Init_id = pthread_self();
  printf( "Init's ID is 0x%08x\n", Init_id );
  
  /* basic checkout of mutex attributes */

  printf( "Init: Initializing mutex attributes\n" );
  status = pthread_mutexattr_init( &attr );
  assert( !status );

  Print_mutexattr( "Init: ", &attr );

  printf( "Init: Changing mutex attributes\n" );
  status = pthread_mutexattr_setprotocol( &attr, PTHREAD_PRIO_INHERIT );
  assert( !status );
 
  status = pthread_mutexattr_setprioceiling( &attr, 128 );
  assert( !status );
 
  status = pthread_mutexattr_setpshared( &attr, PTHREAD_PROCESS_SHARED );
  assert( !status );
 
  Print_mutexattr( "Init: ", &attr );

  printf( "Init: Resetting mutex attributes\n" );
  status = pthread_mutexattr_init( &attr );
  assert( !status );

  /* create a thread */

  status = pthread_create( &Task_id, NULL, Task_1_through_3, NULL );
  assert( !status );

  /* now try some basic mutex operations */

  empty_line();

  printf( "Init: Creating a mutex\n" );
  status = pthread_mutex_init( &Mutex_id, &attr );
  if ( status )
    printf( "status = %d\n", status );
  assert( !status );

  printf( "Init: pthread_mutex_trylock successfully\n" );
  status = pthread_mutex_trylock( &Mutex_id );
  if ( status )
    printf( "status = %d\n", status );
  assert( !status );

  printf( "Init: pthread_mutex_trylock already locked\n" );
  status = pthread_mutex_trylock( &Mutex_id );
  if ( status != EDEADLK )
    printf( "status = %d\n", status );
  assert( status == EDEADLK );

  printf( "Init: pthread_mutex_lock already locked\n" );
  status = pthread_mutex_lock( &Mutex_id );
  if ( status != EDEADLK )
    printf( "status = %d\n", status );
  assert( status == EDEADLK );

  printf( "Init: Sleep 1 second\n" );

  sleep( 1 );
  
     /* switch to task 1 */

  printf( "Init: pthread_mutex_unlock successfully\n" );
  status = pthread_mutex_unlock( &Mutex_id );
  if ( status )
    printf( "status = %d\n", status );
  assert( !status );

  printf( "Init: pthread_mutex_unlock not owner\n" );
  status = pthread_mutex_unlock( &Mutex_id );
  if ( status != EPERM )
    printf( "status = %d\n", status );
  assert( status == EPERM );

  puts( "*** END OF POSIX TEST 5 ***" );
  exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}
