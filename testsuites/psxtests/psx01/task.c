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

void *Task_1_through_3(
  void *argument
)
{
  int status;

  /* XXX temporary */

  /* get id of this thread */

  Task_id = pthread_self();
  printf( "Task's ID is 0x%08x\n", Task_id );

  status = pthread_equal( Task_id, Task_id );
  if ( status )
    puts( "pthread_equal match case passed" );
  assert( status );

  status = pthread_equal( Init_id, Task_id );
  if ( !status )
    puts( "pthread_equal different case passed" );
  assert( !status );

  puts( "pthread_equal first id bad" );
  status = pthread_equal( -1, Task_id );
  assert( status == 0);

  puts( "pthread_equal second id bad" );
  status = pthread_equal( Init_id, -1 );
  assert( status == 0);

  puts( "*** END OF POSIX TEST 1 ***" );
  exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}
