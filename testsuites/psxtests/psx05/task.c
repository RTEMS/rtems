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
  int status;

  printf( "Task: pthread_mutex_trylock already locked\n" );
  status = pthread_mutex_trylock( &Mutex_id );
  if ( status != EBUSY )
    printf( "status = %d\n", status );
  rtems_test_assert( status == EBUSY );

  printf( "Task: pthread_mutex_lock unavailable\n" );
  status = pthread_mutex_lock( &Mutex_id );
  if ( status )
    printf( "status = %d\n", status );
  rtems_test_assert( !status );

     /* switch to init */

  printf( "Task: mutex acquired\n" );

  printf( "Task: sleep for 2 seconds\n" );
  sleep( 2 );

     /* switch to init */

  printf( "Task: exit\n" );
  pthread_exit( NULL );

     /* switch to Init */

  return NULL; /* just so the compiler thinks we returned something */
}
