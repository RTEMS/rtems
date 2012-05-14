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

void *Task_2(
  void *argument
)
{
  int   status;

  puts( "Task_2: sleep 1 second" );

  sleep( 1 );

  /* switch to task 3 */

  puts( "Task_2: join to detached task (Init) -- EINVAL" );
  status = pthread_join( Init_id, NULL );
  if ( status != EINVAL )
    printf( "status = %d\n", status );
  rtems_test_assert( status == EINVAL );

  puts( "Task_2: join to self task (Init) -- EDEADLK" );
  status = pthread_join( pthread_self(), NULL );
  if ( status != EDEADLK )
    printf( "status = %d\n", status );
  rtems_test_assert( status == EDEADLK );

  puts( "Task_2: exitting" );

  pthread_exit( &Task2_id );

     /* switch to init task */

  return NULL; /* just so the compiler thinks we returned something */
}
