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
  int               status;
  rtems_unsigned32 *key_data; 

  printf( "Task_1: Setting the key to %d\n", 1 );
  status = pthread_setspecific( Key_id, &Data_array[ 1 ] );
  if ( status )
    printf( "status = %d\n", status );
  assert( !status );
 
  key_data = pthread_getspecific( Key_id );
  printf( "Task_1: Got the key value of %d\n",
          (rtems_unsigned32 *)key_data - Data_array );
  if ( status )
    printf( "status = %d\n", status );
  assert( !status );

  printf( "Task_1: exitting\n" );

  pthread_exit( NULL );

     /* switch to task 2 */

  return NULL; /* just so the compiler thinks we returned something */
}
