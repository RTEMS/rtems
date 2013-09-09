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

#include <tmacros.h>
#include "test_support.h"

#include <errno.h>
#include <pthread.h>
#include <sched.h>

#include <rtems/posix/pthreadimpl.h>
#include <rtems/score/stackimpl.h>

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);
void *Test_Thread(void *arg);

void *Stack_Low;
void *Stack_High;

void *Test_Thread(void *arg)
{
  #if defined(__GNUC__)
    void *sp = __builtin_frame_address(0);

    #if 0
      printf( "Stack(%p - %p) and sp=%p\n", Stack_Low, Stack_High, sp );
    #endif

    if ( sp >= Stack_Low && sp <= Stack_High )
      puts( "Test_Thread - running on user provided stack - OK" );
    else {
      puts( "Test_Thread - ERROR running on other stack" );
      rtems_test_exit(0);
    }
  #else
      puts( "Test_Thread - no way to get stack pointer and verify" );
  #endif
  puts( "Test_Thread - delete self" );
  return NULL;
}

void *POSIX_Init(void *argument)
{
#if HAVE_DECL_PTHREAD_ATTR_SETSTACK
  int                 sc;
  pthread_t           id;
  pthread_attr_t      attr;
  struct timespec     delay_request;

  puts( "\n\n*** POSIX STACK ATTRIBUTE TEST 02  ***" );

  puts( "Init - Allocate stack from heap" );
  Stack_Low = malloc(PTHREAD_MINIMUM_STACK_SIZE);
  rtems_test_assert( Stack_Low );
  Stack_High = Stack_Low + PTHREAD_MINIMUM_STACK_SIZE;

  puts( "Init - Initialize thread attribute for user provided stack" );
  sc = pthread_attr_init( &attr );
  rtems_test_assert( !sc );

  sc = pthread_attr_setstack( &attr, Stack_Low, PTHREAD_MINIMUM_STACK_SIZE );
  rtems_test_assert( !sc );

  /* create threads */
  sc = pthread_create( &id, &attr, Test_Thread, NULL );
  rtems_test_assert( !sc );

  puts( "Init - let other thread run" );
  delay_request.tv_sec = 0;
  delay_request.tv_nsec = 5 * 100000000;
  sc = nanosleep( &delay_request, NULL );
  rtems_test_assert( !sc );
#else
  puts( "pthread_set_stack not supported - SKIPPING TEST CASE" );
#endif
  
  puts( "*** END OF POSIX STACK ATTRIBUTE TEST 02  ***" );

  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_POSIX_THREADS        2

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
/* end of file */
