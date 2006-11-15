/*
 *  This test exercises the POSIX RWLock manager.
 *
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <assert.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

/* #define __USE_XOPEN2K XXX already defined on GNU/Linux */
#include <pthread.h>

#define NUMBER_THREADS 2
pthread_t ThreadIds[NUMBER_THREADS];
pthread_rwlock_t RWLock;

void *RWLockThread(void *arg)
{
  return NULL;
}

/*
 *  main entry point to the test
 */

#if defined(__rtems__)
int test_main(void)
#else
int main(
  int    argc,
  char **argv
)
#endif
{
  pthread_rwlock_t     rwlock;
  pthread_rwlockattr_t attr;
  int                  status;
  int                  p;
  pthread_t            thread_id;
  int                  i;
  struct timespec      abstime;

  puts( "\n\n*** POSIX RWLOCK TEST 01 ***" );

  /*************** NULL POINTER CHECKS *****************/
  puts( "pthread_rwlockattr_init( NULL ) -- EINVAL" );
  status = pthread_rwlockattr_init( NULL );
  assert( status == EINVAL );

  puts( "pthread_rwlockattr_setpshared( NULL, private ) -- EINVAL" );
  status = pthread_rwlockattr_setpshared( NULL, PTHREAD_PROCESS_PRIVATE );
  assert( status == EINVAL );

  puts( "pthread_rwlockattr_setpshared( NULL, shared ) -- EINVAL" );
  status = pthread_rwlockattr_setpshared( NULL, PTHREAD_PROCESS_SHARED );
  assert( status == EINVAL );

  puts( "pthread_rwlockattr_getpshared( NULL, &p ) -- EINVAL" );
  status = pthread_rwlockattr_getpshared( NULL, &p );
  assert( status == EINVAL );

  puts( "pthread_rwlockattr_destroy( NULL ) -- EINVAL" );
  status = pthread_rwlockattr_destroy( NULL );
  assert( status == EINVAL );

  /*************** NOT INITIALIZED CHECKS *****************/
  /* cheat visibility */
  attr.is_initialized = 0; 
  puts( "pthread_rwlockattr_setpshared( &attr, shared ) -- EINVAL" );
  status = pthread_rwlockattr_setpshared( &attr, PTHREAD_PROCESS_SHARED );
  assert( status == EINVAL );

  puts( "pthread_rwlockattr_getpshared( &attr, NULL ) -- EINVAL" );
  status = pthread_rwlockattr_getpshared( &attr, NULL );
  assert( status == EINVAL );

  puts( "pthread_rwlockattr_destroy( &attr ) -- EINVAL" );
  status = pthread_rwlockattr_destroy( &attr );
  assert( status == EINVAL );


  /*************** ACTUALLY WORK THIS TIME *****************/

  puts( "pthread_rwlockattr_init( &attr ) -- OK" );
  status = pthread_rwlockattr_init( &attr );
  assert( status == 0 );

  puts( "pthread_rwlockattr_setpshared( &attr, private ) -- OK" );
  status = pthread_rwlockattr_setpshared( &attr, PTHREAD_PROCESS_PRIVATE );
  assert( status == 0 );

  puts( "pthread_rwlockattr_getpshared( &attr, &p ) -- OK" );
  status = pthread_rwlockattr_getpshared( &attr, &p );
  assert( status == 0 );
  assert( p == PTHREAD_PROCESS_PRIVATE );

  puts( "pthread_rwlockattr_setpshared( &attr, shared ) -- OK" );
  status = pthread_rwlockattr_setpshared( &attr, PTHREAD_PROCESS_SHARED );
  assert( status == 0 );

  puts( "pthread_rwlockattr_getpshared( &attr, &p ) -- OK" );
  status = pthread_rwlockattr_getpshared( &attr, &p );
  assert( status == 0 );
  assert( p == PTHREAD_PROCESS_SHARED );

  /*************** BAD PSHARED CHECK *****************/
  puts( "pthread_rwlockattr_setpshared( &attr, private ) -- EINVAL" );
  status = pthread_rwlockattr_setpshared( &attr, ~PTHREAD_PROCESS_PRIVATE );
  assert( status == EINVAL );

  /*************** DESTROY/REUSE CHECK *****************/
  puts( "pthread_rwlockattr_destroy( &attr ) -- OK" );
  status = pthread_rwlockattr_destroy( &attr );
  assert( status == 0 );

  puts( "pthread_rwlockattr_getpshared( &attr, &p ) destroyed -- EINVAL" );
  status = pthread_rwlockattr_getpshared( &attr, &p );
  assert( status == EINVAL );


  /* XXX _init error checks */

  /*************** NULL ID ARGUMENT CHECKS *****************/
  puts( "pthread_rwlock_init(NULL, &attr) -- EINVAL" );
  status = pthread_rwlock_init(NULL, &attr);
  assert( status == EINVAL );

  puts( "pthread_rwlock_init(&rwlock, NULL) -- EINVAL" );
  status = pthread_rwlock_init(&rwlock, NULL);
  assert( status == EINVAL );

  puts( "pthread_rwlock_destroy(NULL) -- EINVAL" );
  status = pthread_rwlock_destroy(NULL);
  assert( status == EINVAL );

  puts( "pthread_rwlock_rdlock(NULL) -- EINVAL" );
  status = pthread_rwlock_rdlock(NULL);
  assert( status == EINVAL );

  puts( "pthread_rwlock_tryrdlock(NULL) -- EINVAL" );
  status = pthread_rwlock_tryrdlock(NULL);
  assert( status == EINVAL );

  puts( "pthread_rwlock_timedrdlock( NULL, &abstime) -- EINVAL" );
  status = pthread_rwlock_timedrdlock( NULL, &abstime);
  assert( status == EINVAL );

  puts( "pthread_rwlock_timedrdlock( &rwlock, NULL) -- EINVAL" );
  status = pthread_rwlock_timedrdlock( &rwlock, NULL);
  assert( status == EINVAL );

  puts( "pthread_rwlock_timedrdlock( &rwlock, NULL) -- EINVAL" );
  status = pthread_rwlock_timedrdlock( &rwlock, NULL);
  assert( status == EINVAL );

  puts( "pthread_rwlock_wrlock(NULL) -- EINVAL" );
  status = pthread_rwlock_wrlock(NULL);
  assert( status == EINVAL );

  puts( "pthread_rwlock_trywrlock(NULL) -- EINVAL" );
  status = pthread_rwlock_trywrlock(NULL);
  assert( status == EINVAL );

  puts( "pthread_rwlock_timedwrlock( NULL, &abstime) -- EINVAL" );
  status = pthread_rwlock_timedwrlock( NULL, &abstime );
  assert( status == EINVAL );

  puts( "pthread_rwlock_timedwrlock( &rwlock, NULL) -- EINVAL" );
  status = pthread_rwlock_timedwrlock( &rwlock, NULL);
  assert( status == EINVAL );


  /*************** BAD ID CHECK *****************/
  rwlock = 1;

  /* XXX make a valid abstime */

  puts( "pthread_rwlock_destroy(BadId) -- EINVAL" );
  status = pthread_rwlock_destroy(&rwlock);
  assert( status == EINVAL );

  puts( "pthread_rwlock_rdlock(BadId) -- EINVAL" );
  status = pthread_rwlock_rdlock(NULL);
  assert( status == EINVAL );

  puts( "pthread_rwlock_tryrdlock(BadId) -- EINVAL" );
  status = pthread_rwlock_tryrdlock(NULL);
  assert( status == EINVAL );

  puts( "pthread_rwlock_timedrdlock( BadId, &abstime) -- EINVAL" );
  status = pthread_rwlock_timedrdlock( NULL, &abstime);
  assert( status == EINVAL );

  puts( "pthread_rwlock_timedrdlock( &rwlock, BadId) -- EINVAL" );
  status = pthread_rwlock_timedrdlock( &rwlock, NULL);
  assert( status == EINVAL );

  puts( "pthread_rwlock_timedrdlock( &rwlock, BadId) -- EINVAL" );
  status = pthread_rwlock_timedrdlock( &rwlock, NULL);
  assert( status == EINVAL );

  puts( "pthread_rwlock_wrlock(BadId) -- EINVAL" );
  status = pthread_rwlock_wrlock(NULL);
  assert( status == EINVAL );

  puts( "pthread_rwlock_trywrlock(BadId) -- EINVAL" );
  status = pthread_rwlock_trywrlock(NULL);
  assert( status == EINVAL );

  puts( "pthread_rwlock_timedwrlock( BadId, &abstime) -- EINVAL" );
  status = pthread_rwlock_timedwrlock( &rwlock, &abstime );
  assert( status == EINVAL );

  /*************** BAD ABSTIME CHECK *****************/

  /* in the past */
  abstime.tv_sec = 0;
  abstime.tv_nsec = 0;

  /* invalid tv_nsec */
  abstime.tv_sec = 0;
  abstime.tv_nsec = 0x7fffffffL;



  /*************** ACTUALLY CREATE ONE CHECK *****************/
  puts( "pthread_rwlockattr_init( &attr ) -- OK" );
  status = pthread_rwlockattr_init( &attr );
  assert( status == 0 );

  puts( "pthread_rwlock_init( &rwlock, &attr ) -- OK" );
  status = pthread_rwlock_init( &rwlock, &attr );
  assert( status == 0 );
  assert( rwlock != 0 );

  puts( "pthread_rwlock_destroy( &rwlock ) -- OK" );
  status = pthread_rwlock_destroy( &rwlock );
  assert( status == 0 );

#if 0
  /*************** CREATE TESTS AND LET THEM RELEASE *****************/
  puts( "pthread_rwlock_init( &RWLock, &attr, 2 ) -- OK" );
  status = pthread_rwlock_init( &RWLock, &attr, 2 );
  assert( status == 0 );
  assert( rwlock != 0 );

  for (i=0 ; i<NUMBER_THREADS ; i++ ) {
    printf( "Init: pthread_create - thread %d OK\n", i+1 );
    status = pthread_create(&ThreadIds[i], NULL, RWLockThread, &ThreadIds[i]);
    assert( !status );

    sleep(1);
  }
#endif

  /*************** END OF TEST *****************/
  puts( "*** END OF POSIX RWLOCK TEST 01 ***" );
  exit(0);
}
