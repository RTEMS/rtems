/*
 *  This test exercises the POSIX Spinlock manager.
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

#include <pthread.h>

#include <rtems.h>  /* for task creation */

pthread_spinlock_t Spinlock;

rtems_task SpinlockThread(rtems_task_argument arg)
{
  int  status;

  puts( "pthread_spin_trylock( &Spinlock ) -- EBUSY" );
  status = pthread_spin_trylock( &Spinlock );
  assert( status == EBUSY );

  puts( "pthread_spin_unlock( &Spinlock ) -- EPERM" );
  status = pthread_spin_unlock( &Spinlock );
  assert( status == EPERM );

  rtems_task_delete( RTEMS_SELF );
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
  int                   status;
  rtems_status_code     rstatus;
  rtems_id              taskid;

  puts( "\n\n*** POSIX SPINLOCK TEST 02 ***" );

  /* This successfully creates one */
  puts( "pthread_spin_init( &Spinlock, PTHREAD_PROCESS_PRIVATE ) -- OK" );
  status = pthread_spin_init( &Spinlock, PTHREAD_PROCESS_PRIVATE );
  assert( status == 0 );

  /* Lock it */
  puts( "pthread_spin_lock( &Spinlock ) -- OK" );
  status = pthread_spin_lock( &Spinlock );
  assert( status == 0 );

  /*  Create a helper task */
  rstatus = rtems_task_create(
     rtems_build_name( 'S', 'P', 'I', 'N' ),
     1,
     RTEMS_MINIMUM_STACK_SIZE,
     RTEMS_DEFAULT_MODES,
     RTEMS_DEFAULT_ATTRIBUTES,
     &taskid
  );
  assert( rstatus == RTEMS_SUCCESSFUL );

  rstatus = rtems_task_start( taskid, SpinlockThread, 0 );
  assert( rstatus == RTEMS_SUCCESSFUL );

  sleep(1);

  puts( "pthread_spin_unlock( &Spinlock ) -- OK" );
  status = pthread_spin_unlock( &Spinlock );
  assert( status == 0 );

  puts( "pthread_spin_destroy( &Spinlock ) -- OK" );
  status = pthread_spin_destroy( &Spinlock );
  assert( status == 0 );

  /*************** END OF TEST *****************/
  puts( "*** END OF POSIX SPINLOCK TEST 02 ***" );
  exit(0);
}
