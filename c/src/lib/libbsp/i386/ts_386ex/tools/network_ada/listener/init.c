/* 
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be found in
 *  the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <bsp.h>

#include <assert.h>
#include <pthread.h>

#include <stdio.h>
#include <rtems/rtems_bsdnet.h>
#include "../networkconfig.h"

#ifdef GNAT_PID
#include <unistd.h>
pid_t getpid()
{
  return GNAT_PID;
}
#endif

/*
 *  By having the POSIX_Init thread create a second thread just
 *  to invoke gnat_main, we can override all default attributes
 *  of the "Ada environment task".  Otherwise, we would be
 *  stuck with the defaults set by RTEMS.
 */
 
void *start_gnat_main( void * argument )
{
  extern int gnat_main ( int argc, char **argv, char **envp );

  (void) gnat_main ( 0, 0, 0 );

  exit( 0 );

  return 0;
}

void *POSIX_Init( void *argument )
{
  pthread_t       thread_id;
  pthread_attr_t  attr;
  int             status;

  rtems_bsdnet_initialize_network ();

  status = pthread_attr_init( &attr );
  assert( !status );

#ifdef GNAT_MAIN_STACKSPACE
  status = pthread_attr_setstacksize( &attr, GNAT_MAIN_STACKSPACE );
  assert( !status );
#endif

  status = pthread_create( &thread_id, &attr, start_gnat_main, NULL );
  assert( !status );

  pthread_exit( 0 );

  return 0;
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

/*
#define CONFIGURE_MICROSECONDS_PER_TICK RTEMS_MILLISECONDS_TO_MICROSECONDS(1)
*/

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_EXECUTIVE_RAM_SIZE	(512*1024)
#define CONFIGURE_INIT_TASK_STACK_SIZE	(10*1024)
#define CONFIGURE_INIT_TASK_PRIORITY	120
#define CONFIGURE_INIT_TASK_INITIAL_MODES (RTEMS_PREEMPT | \
                                           RTEMS_NO_TIMESLICE | \
                                           RTEMS_NO_ASR | \
                                           RTEMS_INTERRUPT_LEVEL(0))

#define CONFIGURE_MAXIMUM_POSIX_THREADS              20
#define CONFIGURE_MAXIMUM_POSIX_KEYS                 20
#define CONFIGURE_MAXIMUM_POSIX_MUTEXES              30
#define CONFIGURE_MAXIMUM_POSIX_CONDITION_VARIABLES  20

#define CONFIGURE_INIT

#include <confdefs.h>

