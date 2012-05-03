/*  Task_3
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
#include <errno.h>

void *Task_3(
  void *argument
)
{
  int   status;
  void  *return_pointer;

  puts( "Task_3: join to Task_2" );
  status = pthread_join( Task2_id, &return_pointer );
  puts( "Task_3: returned from pthread_join" );
  if ( status )
    printf( "status = %d\n", status );
  rtems_test_assert( !status );

  if ( return_pointer == &Task2_id )
    puts( "Task_3: pthread_join returned correct pointer" );
  else
    printf(
      "Task_3: pthread_join returned incorrect pointer (%p != %p)\n",
      return_pointer,
      &Task2_id
    );

  puts( "*** END OF POSIX TEST 8 ***" );
  rtems_test_exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}
