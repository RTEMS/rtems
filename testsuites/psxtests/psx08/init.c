/* 
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

#define CONFIGURE_INIT
#include "system.h"
#include <errno.h>

void *POSIX_Init(
  void *argument
)
{
  int    status;
  void  *return_pointer;

  puts( "\n\n*** POSIX TEST 8 ***" );

  /* set the time of day, and print our buffer in multiple ways */

  set_time( TM_FRIDAY, TM_MAY, 24, 96, 11, 5, 0 );

  /* get id of this thread */

  Init_id = pthread_self();
  printf( "Init's ID is 0x%08x\n", Init_id );

  puts( "Init: pthread_detach - ESRCH (invalid id)" );
  status = pthread_detach( -1 );
  assert( status == ESRCH );

  /* detach this thread */

  puts( "Init: pthread_detach self" );
  status = pthread_detach( pthread_self() );
  assert( !status );

  /* create thread */
 
  puts( "Init: creating two tasks" );
  status = pthread_create( &Task_id, NULL, Task_1, NULL );
  assert( !status );
  
  status = pthread_create( &Task2_id, NULL, Task_2, NULL );
  assert( !status );
  
  puts( "Init: pthread_join - ESRCH (invalid id)" );
  status = pthread_join( -1, &return_pointer );
  assert( status == ESRCH );

  puts( "Init: pthread_join - SUCCESSFUL" );
  status = pthread_join( Task_id, &return_pointer );
  /* assert is below comment */

     /* switch to Task 1 */

  puts( "Init: returned from pthread_join" );
  if ( status )
    printf( "status = %d\n", status );
  assert( !status );

  if ( return_pointer == &Task_id )
    puts( "Init: pthread_join returned correct pointer" );
  else
    printf(
      "Init: pthread_join returned incorrect pointer (%p != %p)\n",
      return_pointer,
      &Task_id
    );
  
  puts( "Init: exitting" );
  pthread_exit( NULL );

  return NULL; /* just so the compiler thinks we returned something */
}
