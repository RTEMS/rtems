/**
 *  @file
 *
 *  This test exercises the POSIX RWLock manager.
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "tmacros.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

/* #define __USE_XOPEN2K XXX already defined on GNU/Linux */
#include <pthread.h>

/* forward declarations to avoid warnings */
void *ReadLockThread(void *arg);
void *WriteLockThread(void *arg);
int test_main(void);

#if !HAVE_DECL_PTHREAD_RWLOCK_UNLOCK
/* FIXME: Newlib should provide the decl. */
extern int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);
#endif

#define NUMBER_THREADS 2
pthread_t ThreadIds[NUMBER_THREADS];
pthread_rwlock_t RWLock;

/*
 * Test thread to block for read lock and unlock it
 */
void *ReadLockThread(void *arg)
{
  int status;

  /*
   * Detach ourselves so we don't wait for a join that won't happen.
   */
  pthread_detach( pthread_self() );

  puts( "ReadThread - pthread_rwlock_rdlock(RWLock) blocking -- OK" );
  status = pthread_rwlock_rdlock(&RWLock);
  rtems_test_assert( !status );
  puts( "ReadThread - pthread_rwlock_rdlock(RWLock) unblocked -- OK" );

  status = pthread_rwlock_unlock(&RWLock);
  rtems_test_assert( !status );
  return NULL;
}

/*
 * Test thread to block for write lock and unlock it
 */
void *WriteLockThread(void *arg)
{
  int status;

  /*
   * Detach ourselves so we don't wait for a join that won't happen.
   */
  pthread_detach( pthread_self() );

  puts( "WriteThread - pthread_rwlock_wrlock(RWLock) blocking -- OK" );
  status = pthread_rwlock_wrlock(&RWLock);
  rtems_test_assert( !status );
  puts( "WriteThread - pthread_rwlock_wrlock(RWLock) unblocked -- OK" );

  sleep( 2 );

  puts( "WriteThread - pthread_rwlock_unlock(RWLock) -- OK" );
  status = pthread_rwlock_unlock(&RWLock);
  if ( status )
   printf( "status=%s\n", strerror(status) );
  rtems_test_assert( !status );
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
  int                  i;
  struct timespec      abstime;

  puts( "\n\n*** POSIX RWLOCK TEST 01 ***" );

  /*************** NULL POINTER CHECKS *****************/
  puts( "pthread_rwlockattr_init( NULL ) -- EINVAL" );
  status = pthread_rwlockattr_init( NULL );
  rtems_test_assert( status == EINVAL );

  puts( "pthread_rwlockattr_setpshared( NULL, private ) -- EINVAL" );
  status = pthread_rwlockattr_setpshared( NULL, PTHREAD_PROCESS_PRIVATE );
  rtems_test_assert( status == EINVAL );

  puts( "pthread_rwlockattr_setpshared( NULL, shared ) -- EINVAL" );
  status = pthread_rwlockattr_setpshared( NULL, PTHREAD_PROCESS_SHARED );
  rtems_test_assert( status == EINVAL );

  puts( "pthread_rwlockattr_getpshared( NULL, &p ) -- EINVAL" );
  status = pthread_rwlockattr_getpshared( NULL, &p );
  rtems_test_assert( status == EINVAL );

  puts( "pthread_rwlockattr_destroy( NULL ) -- EINVAL" );
  status = pthread_rwlockattr_destroy( NULL );
  rtems_test_assert( status == EINVAL );

  /*************** NOT INITIALIZED CHECKS *****************/
  /* cheat visibility */
  attr.is_initialized = 0;
  puts( "pthread_rwlockattr_setpshared( &attr, shared ) -- EINVAL" );
  status = pthread_rwlockattr_setpshared( &attr, PTHREAD_PROCESS_SHARED );
  rtems_test_assert( status == EINVAL );

  puts( "pthread_rwlockattr_getpshared( &attr, NULL ) -- EINVAL" );
  status = pthread_rwlockattr_getpshared( &attr, NULL );
  rtems_test_assert( status == EINVAL );

  puts( "pthread_rwlockattr_destroy( &attr ) -- EINVAL" );
  status = pthread_rwlockattr_destroy( &attr );
  rtems_test_assert( status == EINVAL );

  /*************** BAD PSHARED CHECK *****************/
  puts( "pthread_rwlockattr_setpshared( &attr, private ) -- EINVAL" );
  status = pthread_rwlockattr_setpshared( &attr, ~PTHREAD_PROCESS_PRIVATE );
  rtems_test_assert( status == EINVAL );

  /*************** ACTUALLY WORK THIS TIME *****************/
  puts( "pthread_rwlockattr_init( &attr ) -- OK" );
  status = pthread_rwlockattr_init( &attr );
  rtems_test_assert( status == 0 );

  puts( "pthread_rwlockattr_setpshared( &attr, private ) -- OK" );
  status = pthread_rwlockattr_setpshared( &attr, PTHREAD_PROCESS_PRIVATE );
  rtems_test_assert( status == 0 );

  puts( "pthread_rwlockattr_getpshared( &attr, &p ) -- OK" );
  status = pthread_rwlockattr_getpshared( &attr, &p );
  rtems_test_assert( status == 0 );
  rtems_test_assert( p == PTHREAD_PROCESS_PRIVATE );

  puts( "pthread_rwlockattr_setpshared( &attr, shared ) -- OK" );
  status = pthread_rwlockattr_setpshared( &attr, PTHREAD_PROCESS_SHARED );
  rtems_test_assert( status == 0 );

  puts( "pthread_rwlockattr_getpshared( &attr, &p ) -- OK" );
  status = pthread_rwlockattr_getpshared( &attr, &p );
  rtems_test_assert( status == 0 );
  rtems_test_assert( p == PTHREAD_PROCESS_SHARED );

  /*************** DESTROY/REUSE CHECK *****************/
  puts( "pthread_rwlockattr_destroy( &attr ) -- OK" );
  status = pthread_rwlockattr_destroy( &attr );
  rtems_test_assert( status == 0 );

  puts( "pthread_rwlockattr_getpshared( &attr, &p ) destroyed -- EINVAL" );
  status = pthread_rwlockattr_getpshared( &attr, &p );
  rtems_test_assert( status == EINVAL );

  /*************** NULL ARGUMENT CHECKS *****************/
  abstime.tv_sec = 0;
  abstime.tv_nsec = 0;

  puts( "pthread_rwlock_init(NULL, &attr) -- EINVAL" );
  status = pthread_rwlock_init(NULL, &attr);
  rtems_test_assert( status == EINVAL );

  puts( "pthread_rwlock_destroy(NULL) -- EINVAL" );
  status = pthread_rwlock_destroy(NULL);
  rtems_test_assert( status == EINVAL );

  puts( "pthread_rwlock_rdlock(NULL) -- EINVAL" );
  status = pthread_rwlock_rdlock(NULL);
  rtems_test_assert( status == EINVAL );

  puts( "pthread_rwlock_timedrdlock( NULL, &abstime) -- EINVAL" );
  status = pthread_rwlock_timedrdlock( NULL, &abstime);
  rtems_test_assert( status == EINVAL );

  puts( "pthread_rwlock_timedrdlock( &rwlock, NULL) -- EINVAL" );
  status = pthread_rwlock_timedrdlock( &rwlock, NULL);
  rtems_test_assert( status == EINVAL );

  puts( "pthread_rwlock_tryrdlock(NULL) -- EINVAL" );
  status = pthread_rwlock_tryrdlock(NULL);
  rtems_test_assert( status == EINVAL );

  puts( "pthread_rwlock_wrlock(NULL) -- EINVAL" );
  status = pthread_rwlock_wrlock(NULL);
  rtems_test_assert( status == EINVAL );

  puts( "pthread_rwlock_timedwrlock( NULL, &abstime) -- EINVAL" );
  status = pthread_rwlock_timedwrlock( NULL, &abstime );
  rtems_test_assert( status == EINVAL );

  puts( "pthread_rwlock_timedwrlock( &rwlock, NULL) -- EINVAL" );
  status = pthread_rwlock_timedwrlock( &rwlock, NULL);
  rtems_test_assert( status == EINVAL );

  puts( "pthread_rwlock_trywrlock(NULL) -- EINVAL" );
  status = pthread_rwlock_trywrlock(NULL);
  rtems_test_assert( status == EINVAL );

  puts( "pthread_rwlock_unlock(NULL) -- EINVAL" );
  status = pthread_rwlock_unlock(NULL);
  rtems_test_assert( status == EINVAL );

  /*************** BAD ID CHECK *****************/
  rwlock = 1;
  /* make a valid abstime */
  puts( "clock_gettime(CLOCK_REALTIME, &abstime) -- OK" );
  status = clock_gettime( CLOCK_REALTIME, &abstime );
  rtems_test_assert( !status );
  abstime.tv_sec += 5;

  puts( "pthread_rwlock_destroy(BadId) -- EINVAL" );
  status = pthread_rwlock_destroy(&rwlock);
  rtems_test_assert( status == EINVAL );

  puts( "pthread_rwlock_rdlock(BadId) -- EINVAL" );
  status = pthread_rwlock_rdlock(&rwlock);
  rtems_test_assert( status == EINVAL );

  puts( "pthread_rwlock_timedrdlock(BadId, &abstime) -- EINVAL" );
  status = pthread_rwlock_timedrdlock( &rwlock, &abstime);
  rtems_test_assert( status == EINVAL );

  puts( "pthread_rwlock_tryrdlock(BadId) -- EINVAL" );
  status = pthread_rwlock_tryrdlock(&rwlock);
  rtems_test_assert( status == EINVAL );

  puts( "pthread_rwlock_wrlock(BadId) -- EINVAL" );
  status = pthread_rwlock_wrlock(&rwlock);
  rtems_test_assert( status == EINVAL );

  puts( "pthread_rwlock_timedwrlock(BadId, &abstime) -- EINVAL" );
  status = pthread_rwlock_timedwrlock( &rwlock, &abstime );
  rtems_test_assert( status == EINVAL );

  puts( "pthread_rwlock_trywrlock(BadId) -- EINVAL" );
  status = pthread_rwlock_trywrlock(&rwlock);
  rtems_test_assert( status == EINVAL );

  puts( "pthread_rwlock_unlock(BadId) -- EINVAL" );
  status = pthread_rwlock_unlock(&rwlock);
  rtems_test_assert( status == EINVAL );

  /*************** BAD ABSTIME CHECK *****************/

  /* in the past */
  abstime.tv_sec = 0;
  abstime.tv_nsec = 0;

  /* invalid tv_nsec */
  abstime.tv_sec = 0;
  abstime.tv_nsec = 0x7fffffffL;

  /* XXX do we need bad time check? */

  /*************** ACTUALLY CREATE ONE CHECK *****************/
  puts( "pthread_rwlockattr_init( &attr ) -- OK" );
  status = pthread_rwlockattr_init( &attr );
  rtems_test_assert( status == 0 );

  puts( "pthread_rwlock_init( &rwlock, &attr ) -- OK" );
  status = pthread_rwlock_init( &rwlock, &attr );
  rtems_test_assert( status == 0 );
  rtems_test_assert( rwlock != 0 );

  puts( "pthread_rwlock_init( &rwlock, &attr ) -- EAGAIN" );
  status = pthread_rwlock_init( &rwlock, &attr );
  rtems_test_assert( status == EAGAIN );

  puts( "pthread_rwlock_destroy( &rwlock ) -- OK" );
  status = pthread_rwlock_destroy( &rwlock );
  rtems_test_assert( status == 0 );

  /********* CREATE RWLOCK WITH DEFAULT ATTRIBUTES AND DESTROY IT *********/
  puts( "pthread_rwlock_init( &rwlock, NULL ) -- OK" );
  status = pthread_rwlock_init( &rwlock, NULL );
  rtems_test_assert( status == 0 );

  puts( "pthread_rwlock_destroy( &rwlock ) -- OK" );
  status = pthread_rwlock_destroy( &rwlock );
  rtems_test_assert( status == 0 );

  /*************** CREATE THREADS AND LET THEM OBTAIN READLOCK ***************/
  puts( "pthread_rwlock_init( &RWLock, &attr ) -- OK" );
  status = pthread_rwlock_init( &RWLock, &attr );
  rtems_test_assert( status == 0 );

  puts( "pthread_rwlock_tryrdlock(RWLock) -- OK" );
  status = pthread_rwlock_tryrdlock(&RWLock);
  rtems_test_assert( !status );

  for (i=0 ; i<NUMBER_THREADS ; i++ ) {
    printf( "Init: pthread_create - thread %d OK\n", i+1 );
    status = pthread_create(&ThreadIds[i], NULL, ReadLockThread, &ThreadIds[i]);
    rtems_test_assert( !status );

    sleep(1);
  }

  puts( "pthread_rwlock_unlock(RWLock) -- OK" );
  status = pthread_rwlock_unlock(&RWLock);
  rtems_test_assert( !status );

  sleep(1);

  /*************** CREATE THREADS AND LET THEM OBTAIN READLOCK ***************/
  puts( "pthread_rwlock_trywrlock(RWLock) -- OK" );
  status = pthread_rwlock_trywrlock(&RWLock);
  rtems_test_assert( !status );

  puts( "pthread_rwlock_tryrdlock(&RWLock) -- EBUSY" );
  status = pthread_rwlock_tryrdlock(&RWLock);
  rtems_test_assert( status == EBUSY );

  for (i=0 ; i<NUMBER_THREADS ; i++ ) {
    printf( "Init: pthread_create - thread %d OK\n", i+1 );
    status = pthread_create(&ThreadIds[i], NULL, ReadLockThread, &ThreadIds[i]);
    rtems_test_assert( !status );

    sleep(1);
  }

  /* Attempt delete while threads are blocked */
  puts( "pthread_rwlock_destroy( &RWLock ) -- EBUSY" );
  status = pthread_rwlock_destroy( &RWLock );
  rtems_test_assert( status == EBUSY );

  /* now unlock it so the threads can continue */
  puts( "pthread_rwlock_unlock(RWLock) -- OK" );
  status = pthread_rwlock_unlock(&RWLock);
  rtems_test_assert( !status );

  sleep(2);

  /*************** CREATE THREADS AND LET THEM OBTAIN WRITE LOCK *************/
  puts( "\npthread_rwlock_trywrlock(RWLock) -- OK" );
  status = pthread_rwlock_trywrlock(&RWLock);
  rtems_test_assert( !status );

  puts( "pthread_rwlock_trywrlock(&RWLock) -- EBUSY" );
  status = pthread_rwlock_trywrlock(&RWLock);
  rtems_test_assert( status == EBUSY );

  for (i=0 ; i<NUMBER_THREADS ; i++ ) {
    printf( "Init: pthread_create - thread %d OK\n", i+1 );
    status =
      pthread_create(&ThreadIds[i], NULL, WriteLockThread, &ThreadIds[i]);
    rtems_test_assert( !status );

    sleep(2);
  }

  puts( "pthread_rwlock_unlock(RWLock) -- OK" );
  status = pthread_rwlock_unlock(&RWLock);
  rtems_test_assert( !status );

  sleep(6);

  /*************** CREATE THREADS AND LET THEM OBTAIN WRITE LOCK *************/
  /***************    THEN ATTEMPT TO OBTAIN A READLOCK          *************/
 
  puts( "\npthread_rwlock_tryrdlock(&RWLock) -- OK" );
  status = pthread_rwlock_tryrdlock(&RWLock);
  rtems_test_assert( !status );

  printf( "Init: pthread_create - thread reader & writer OK\n" );
  status = pthread_create(&ThreadIds[0], NULL, WriteLockThread, &ThreadIds[0]);
  rtems_test_assert( !status );

  sleep(1);
  status = pthread_create(&ThreadIds[1], NULL, ReadLockThread, &ThreadIds[1]);
  rtems_test_assert( !status );

  sleep(1);

  puts( "pthread_rwlock_tryrdlock(&RWLock) -- EBUSY" );
  status = pthread_rwlock_tryrdlock(&RWLock);
  rtems_test_assert( status == EBUSY );

  puts( "pthread_rwlock_trywrlock(&RWLock) -- EBUSY" );
  status = pthread_rwlock_trywrlock(&RWLock);
  rtems_test_assert( status == EBUSY );

  sleep( 5 );

  puts( "pthread_rwlock_unlock(&RWLock) -- OK" );
  status = pthread_rwlock_unlock(&RWLock);
  rtems_test_assert( !status );

  sleep( 5 );

  /*************** TIMEOUT ON RWLOCK ***************/
  puts( "clock_gettime(CLOCK_REALTIME, &abstime) -- OK" );
  status = clock_gettime( CLOCK_REALTIME, &abstime );
  rtems_test_assert( !status );

  abstime.tv_sec += 1;
  puts( "pthread_rwlock_timedwrlock( &RWLock, &abstime) -- OK" );
  status = pthread_rwlock_timedwrlock( &RWLock, &abstime );
  rtems_test_assert( status == 0 );

  abstime.tv_sec += 1;
  puts( "pthread_rwlock_timedrdlock( &RWLock, &abstime) -- ETIMEDOUT" );
  status = pthread_rwlock_timedrdlock( &RWLock, &abstime );
  rtems_test_assert( status == ETIMEDOUT );

  abstime.tv_sec -= 1;
  puts( "pthread_rwlock_timedrdlock( &RWLock, &abstime) -- ETIMEDOUT" );
  status = pthread_rwlock_timedrdlock( &RWLock, &abstime );
  rtems_test_assert( status == ETIMEDOUT );

  abstime.tv_sec -= 1;
  puts( "pthread_rwlock_timedwrlock( &RWLock, &abstime) -- ETIMEDOUT" );
  status = pthread_rwlock_timedwrlock( &RWLock, &abstime );
  rtems_test_assert( status == ETIMEDOUT );

  /*************** OBTAIN RWLOCK WITH ABSTIME IN PAST ***************/
  status = pthread_rwlock_unlock(&RWLock);
  rtems_test_assert( !status );

  abstime.tv_sec -= 1;
  puts( "pthread_rwlock_timedrdlock( &RWLock, &abstime) -- in past -- OK" );
  status = pthread_rwlock_timedrdlock( &RWLock, &abstime );
  rtems_test_assert( status == 0 );

  /*************** OBTAIN RWLOCK FOR WRITE WITH ABSTIME IN PAST ***************/
  status = pthread_rwlock_unlock(&RWLock);
  rtems_test_assert( !status );

  abstime.tv_sec -= 1;
  puts( "pthread_rwlock_timedwrlock( &RWLock, &abstime) -- in past -- OK" );
  status = pthread_rwlock_timedwrlock( &RWLock, &abstime );
  rtems_test_assert( status == 0 );

  /*************** DESTROY RWLOCK ***************/
  puts( "pthread_rwlock_destroy( &RWLock ) -- OK" );
  status = pthread_rwlock_destroy( &RWLock );
  rtems_test_assert( status == 0 );

  /*************** OBTAIN A LOCK AND THEN RELEASE IT TWICE ***************/

  puts( "pthread_rwlock_init( &rwlock, NULL ) -- OK" );
  status = pthread_rwlock_init( &rwlock, NULL );
  rtems_test_assert( status == 0 );
  rtems_test_assert( rwlock != 0 );

  puts( "pthread_rwlock_unlock ( &rwlock ) -- OK" );
  status = pthread_rwlock_unlock( &rwlock );
  rtems_test_assert( status == 0 );

  puts( "pthread_rwlock_unlock ( &rwlock ) -- OK" );
  status = pthread_rwlock_unlock( &rwlock );
  rtems_test_assert( status == 0 );

  /*************** END OF TEST *****************/
  puts( "*** END OF POSIX RWLOCK TEST 01 ***" );
  exit(0);
}
