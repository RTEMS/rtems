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
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
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
