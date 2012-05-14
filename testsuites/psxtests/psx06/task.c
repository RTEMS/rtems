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

void *Task_1(
  void *argument
)
{
  int               status;
  uint32_t   *key_data;

  printf( "Task_1: Setting the key to %d\n", 1 );
  status = pthread_setspecific( Key_id, &Data_array[ 1 ] );
  if ( status )
    printf( "status = %d\n", status );
  rtems_test_assert( !status );

  key_data = pthread_getspecific( Key_id );
  printf( "Task_1: Got the key value of %ld\n",
          (unsigned long) ((uint32_t   *)key_data - Data_array) );
  if ( status )
    printf( "status = %d\n", status );
  rtems_test_assert( !status );

  puts( "Task_1: exitting" );
  pthread_exit( NULL );

     /* switch to task 2 */

  return NULL; /* just so the compiler thinks we returned something */
}
