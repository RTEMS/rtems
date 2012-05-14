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
#include <pthread.h>

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);
void *TestThread(void *argument);

int Index;

void *TestThread(
  void *argument
)
{
  int *index = (int *)argument;

  *index = 7; 

  puts( "TestThread exiting" );
  return argument;
}

void *POSIX_Init(void *argument)
{
  int             status;
  pthread_t       id;
  pthread_attr_t  attr;
  void           *join_return;

  puts( "\n\n*** POSIX TEST PSX16 ***" );

  Index = 5;

  /* Initialize and set thread detached attribute */
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  puts( "Creating TestThread" );
  status = pthread_create( &id, &attr, TestThread, (void *)&Index );
  rtems_test_assert( status == 0 );

  /* let test thread run and exit */
  puts( "Let TestThread run and exit before we attempt to join" );
  sleep( 2 );

  join_return = NULL;
  status = pthread_join( id, &join_return );
  rtems_test_assert( status == 0 );
  rtems_test_assert( join_return == &Index );
  rtems_test_assert( *(int *)join_return == 7 );
  puts( "Successfully joined with TestThread" );

  puts( "*** END OF POSIX TEST PSX16 ***" );

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
