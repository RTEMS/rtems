/*  Task_2
 *
 *  This routine serves as a test task.  It verifies the basic task
 *  switching capabilities of the executive.
 *
 *  Input parameters:
 *    argument - task argument
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-1997.
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

void *Task_2(
  void *argument
)
{
  int   status;
  void  *return_pointer;

  puts( "Task_2: join to Task_1" );
  status = pthread_join( Task_id, &return_pointer );
  puts( "Task_2: returned from pthread_join" );
  if ( status )
    printf( "status = %d\n", status );
  assert( !status );
 
  if ( return_pointer == &Task_id )
    puts( "Task_2: pthread_join returned correct pointer" );
  else
    printf(
      "Task_2: pthread_join returned incorrect pointer (%p != %p)\n",
      return_pointer,
      &Task_id
    );

  puts( "*** END OF POSIX TEST 8 ***" );
  exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}
