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
