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

#include "system.h"

void Test_init_routine( void )
{
  puts( "Test_init_routine: invoked" );
}
  

void *Task_1_through_3(
  void *argument
)
{
  int status;
  pthread_once_t once = PTHREAD_ONCE_INIT;

  empty_line();

  /* get id of this thread */

  Task_id = pthread_self();
  printf( "Task_1: ID is 0x%08x\n", Task_id );

  /* exercise pthread_equal */

  status = pthread_equal( Task_id, Task_id );
  if ( status )
    puts( "Task_1: pthread_equal match case passed" );
  assert( status );

  status = pthread_equal( Init_id, Task_id );
  if ( !status )
    puts( "Task_1: pthread_equal different case passed" );
  assert( !status );

  puts( "Task_1: pthread_equal first id bad" );
  status = pthread_equal( -1, Task_id );
  assert( !status );

  puts( "Task_1: pthread_equal second id bad" );
  status = pthread_equal( Init_id, -1 );
  assert( !status );

  /* exercise pthread_once */

  puts( "Task_1: pthread_once - EINVAL (NULL once_control)" );
  status = pthread_once( NULL, Test_init_routine );
  assert( status == EINVAL );

  puts( "Task_1: pthread_once - EINVAL (NULL init_routine)" );
  status = pthread_once( &once, NULL );
  assert( status == EINVAL );

  puts( "Task_1: pthread_once - SUCCESSFUL (init_routine executes)" );
  status = pthread_once( &once, Test_init_routine );
  assert( !status );

  puts( "Task_1: pthread_once - SUCCESSFUL (init_routine does not execute)" );
  status = pthread_once( &once, Test_init_routine );
  assert( !status );

  puts( "*** END OF POSIX TEST 1 ***" );
  exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}
