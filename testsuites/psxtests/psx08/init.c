/*
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
  printf( "Init's ID is 0x%08" PRIxpthread_t "\n", Init_id );

  puts( "Init: pthread_detach - ESRCH (invalid id)" );
  status = pthread_detach( (pthread_t) -1 );
  rtems_test_assert( status == ESRCH );

  /* detach this thread */

  puts( "Init: pthread_detach self" );
  status = pthread_detach( pthread_self() );
  rtems_test_assert( !status );

  /* create thread */

  status = pthread_create( &Task1_id, NULL, Task_1, NULL );
  rtems_test_assert( !status );

  puts( "Init: pthread_join - ESRCH (invalid id)" );
  status = pthread_join( (pthread_t) -1, &return_pointer );
  rtems_test_assert( status == ESRCH );

  puts( "Init: pthread_join - SUCCESSFUL" );
  status = pthread_join( Task1_id, &return_pointer );

  puts( "Init: returned from pthread_join through return" );
  if ( status )
    printf( "status = %d\n", status );
  rtems_test_assert( !status );

  if ( return_pointer == &Task1_id )
    puts( "Init: pthread_join returned correct pointer" );
  else
    printf(
      "Init: pthread_join returned incorrect pointer (%p != %p)\n",
      return_pointer,
      &Task1_id
    );

  puts( "Init: creating two pthreads" );
  status = pthread_create( &Task2_id, NULL, Task_2, NULL );
  rtems_test_assert( !status );

  status = pthread_create( &Task3_id, NULL, Task_3, NULL );
  rtems_test_assert( !status );

  puts( "Init: pthread_join - SUCCESSFUL" );
  status = pthread_join( Task2_id, &return_pointer );
  /* assert is below comment */

  puts( "Init: returned from pthread_join through pthread_exit" );
  if ( status )
    printf( "status = %d\n", status );
  rtems_test_assert( !status );

  if ( return_pointer == &Task2_id )
    puts( "Init: pthread_join returned correct pointer" );
  else
    printf(
      "Init: pthread_join returned incorrect pointer (%p != %p)\n",
      return_pointer,
      &Task2_id
    );

  puts( "Init: exitting" );
  return NULL;
}
