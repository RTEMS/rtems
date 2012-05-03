/*  Task_1_through_3
 *
 *  This routine serves as a test task.  It verifies the basic task
 *  switching capabilities of the executive.
 *
 *  Input parameters:
 *    argument - task argument
 *
 *  Output parameters:  NONE
 *
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

#include "system.h"

void Test_init_routine( void );

void Test_init_routine( void )
{
  puts( "Test_init_routine: invoked" );
}


void *Task_1_through_3(
  void *argument
)
{
  int            status;
  pthread_once_t once = PTHREAD_ONCE_INIT;

  puts( "Task_1: sched_yield to Init" );
  status = sched_yield();
  rtems_test_assert( !status );

    /* switch to Task_1 */

  /* now do some real testing */

  empty_line();

  /* get id of this thread */

  Task_id = pthread_self();
  printf( "Task_1: ID is 0x%08" PRIxpthread_t "\n", Task_id );

  /* exercise pthread_equal */

  status = pthread_equal( Task_id, Task_id );
  if ( status )
    puts( "Task_1: pthread_equal - match case passed" );
  rtems_test_assert( status );

  status = pthread_equal( Init_id, Task_id );
  if ( !status )
    puts( "Task_1: pthread_equal - different case passed" );
  rtems_test_assert( !status );

  puts( "Task_1: pthread_equal - first id bad" );
  status = pthread_equal( (pthread_t) -1, Task_id );
  rtems_test_assert( !status );

  puts( "Task_1: pthread_equal - second id bad" );
  status = pthread_equal( Init_id, (pthread_t) -1 );
  rtems_test_assert( !status );

  /* exercise pthread_once */

  puts( "Task_1: pthread_once - EINVAL (NULL once_control)" );
  status = pthread_once( NULL, Test_init_routine );
  rtems_test_assert( status == EINVAL );

  puts( "Task_1: pthread_once - EINVAL (NULL init_routine)" );
  status = pthread_once( &once, NULL );
  rtems_test_assert( status == EINVAL );

  puts( "Task_1: pthread_once - SUCCESSFUL (init_routine executes)" );
  status = pthread_once( &once, Test_init_routine );
  rtems_test_assert( !status );

  puts( "Task_1: pthread_once - SUCCESSFUL (init_routine does not execute)" );
  status = pthread_once( &once, Test_init_routine );
  rtems_test_assert( !status );

  puts( "*** END OF POSIX TEST 1 ***" );
  rtems_test_exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}
