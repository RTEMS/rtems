/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define _GNU_SOURCE
#include <tmacros.h>
#include <errno.h>
#include <sched.h>
#include <pthread.h>
#include <rtems/posix/pthreadimpl.h>

const char rtems_test_name[] = "PSXGETATTRNP 1";

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);

#if HAVE_DECL_PTHREAD_GETATTR_NP


void *Thread_1(void *argument);

pthread_t           Init_id;
pthread_t           Thread_id;
pthread_attr_t      Thread_attr;
int                 max_priority;

void *Thread_1(
  void *argument
)
{
  pthread_attr_t      attr;
  struct sched_param  param;
  int                 sc;
  int                 value;

  puts("Thread - pthread_getattr_np - Verify value");
  sc = pthread_getattr_np( Thread_id, &attr );
  rtems_test_assert( sc == 0 );
  rtems_test_assert( ! rtems_pthread_attribute_compare(&attr, &Thread_attr) );

  param.sched_priority = max_priority;

  puts( "Thread - pthread_setschedparam: Setting highest priority SCHED_FIFO" );
  sc = pthread_setschedparam( Thread_id, SCHED_FIFO, &param );
  rtems_test_assert( !sc );

  puts("Thread - Detach");
  sc = pthread_detach( Thread_id );
  rtems_test_assert( !sc );

  puts("Thread - pthread_getattr_np");
  sc = pthread_getattr_np( Thread_id, &attr );
  rtems_test_assert( !sc );

  puts("Thread - Verify SCHED_FIFO policy");
  sc = pthread_attr_getschedpolicy( &attr, &value );
  rtems_test_assert( !sc );
  rtems_test_assert( value == SCHED_FIFO );

  puts("Thread - Verify max priority");
  sc = pthread_attr_getschedparam( &attr, &param );
  rtems_test_assert( !sc );
  rtems_test_assert( param.sched_priority == max_priority );

  puts("Thread - Verify detached");
  sc = pthread_attr_getdetachstate( &attr, &value );
  rtems_test_assert( value == PTHREAD_CREATE_DETACHED );

  return NULL; /* just so the compiler thinks we returned something */
}

void *POSIX_Init(
  void *ignored
)
{
  int                 sc;
  pthread_attr_t      attribute;
  void               *stackaddr;
  size_t              stacksize;
  size_t              guardsize;
  struct sched_param  param;

  TEST_BEGIN();

  /* Initialize thread id */
  Init_id = pthread_self();
  max_priority = sched_get_priority_max( SCHED_FIFO );

  puts( "Init - pthread_getattr_np - attr NULL - EINVAL" );
  sc = pthread_getattr_np( Init_id, NULL );
  rtems_test_assert( sc == EINVAL );

  puts( "Init - pthread_getattr_np - invalid id - ESRCH" );
  sc = pthread_getattr_np( 0xffff, &attribute );
  rtems_test_assert( sc == ESRCH );


  /* init task attributes */
  puts("Init - pthread_attr_init");
  sc = pthread_attr_init(&Thread_attr);
  rtems_test_assert(!sc);

  puts("Init - pthread_attr_setinheritsched - PTHREAD_EXPLICIT_SCHED");
  sc = pthread_attr_setinheritsched( &Thread_attr, PTHREAD_EXPLICIT_SCHED );
  rtems_test_assert(!sc);
  rtems_test_assert( Thread_attr.inheritsched == PTHREAD_EXPLICIT_SCHED );

  puts("Init - pthread_attr_setschedpolicy to SCHED_RR");
  sc = pthread_attr_setschedpolicy(&Thread_attr, SCHED_RR);
  rtems_test_assert(!sc);

  puts("Init - pthread_attr_setschedparam to minimum priority + 2");
  param.sched_priority = sched_get_priority_min( SCHED_RR ) + 2;
  sc = pthread_attr_setschedparam( &Thread_attr, &param );
  rtems_test_assert(!sc);

  puts("Init - pthread_attr_getstack");
  sc = pthread_attr_getstack( &Thread_attr, &stackaddr, &stacksize );
  rtems_test_assert(!sc);

  stacksize *= 2;
  puts("Init - pthread_attr_setstack double the stacksize");
  sc = pthread_attr_setstacksize( &Thread_attr, stacksize );
  rtems_test_assert(!sc);

  puts("Init - pthread_attr_getguardsize");
  sc = pthread_attr_getguardsize( &Thread_attr, &guardsize );
  rtems_test_assert(!sc);

  guardsize *= 2;
  puts("Init - pthread_attr_setguardsize double the guardsize");
  sc = pthread_attr_setguardsize( &Thread_attr, guardsize );
  rtems_test_assert(!sc);

  puts("Init - raise priority to max");
  param.sched_priority = max_priority;
  sc = pthread_setschedparam( Init_id, SCHED_RR, &param );
  rtems_test_assert( !sc );

 puts("Init - pthread_create");
  sc = pthread_create( &Thread_id, &Thread_attr, Thread_1, NULL );
  rtems_test_assert( !sc );

  puts("Init - Lower priority");
  fflush(stdout);
  param.sched_priority = sched_get_priority_min( SCHED_RR );
  sc = pthread_setschedparam( Init_id, SCHED_RR, &param );
  rtems_test_assert(!sc);

#if 0
  sc = pthread_join( Thread_id, NULL );
  rtems_test_assert( !sc );
#endif

  TEST_END();
  rtems_test_exit(0);
  return NULL; /* just so the compiler thinks we returned something */
}
#else
void *POSIX_Init(
  void *ignored
)
{
  TEST_BEGIN();
  puts( "  pthread_getattr_np NOT supported" );
  TEST_END();
  rtems_test_exit(0);
  return NULL; /* just so the compiler thinks we returned something */
}

#endif
/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_POSIX_THREADS  2

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
