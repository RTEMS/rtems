/*  Task_1
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
#include <errno.h>

void *Task_1(
  void *argument
)
{
  int   status;

  puts( "Task_1: sleep 1 second" );

  sleep( 1 );

     /* switch to task 2 */

  puts( "Task_1: join to detached task (Init) -- EINVAL" );
  status = pthread_join( Init_id, NULL );
  if ( status != EINVAL )
    printf( "status = %d\n", status );
  assert( status == EINVAL );
  
  puts( "Task_1: join to self task (Init) -- EDEADLK" );
  status = pthread_join( pthread_self(), NULL );
  if ( status != EDEADLK )
    printf( "status = %d\n", status );
  assert( status == EDEADLK );

  puts( "Task_1: exitting" );

  pthread_exit( &Task_id );

     /* switch to init task */

  return NULL; /* just so the compiler thinks we returned something */
}
