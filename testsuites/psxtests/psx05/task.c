/*  Task_1_through_3
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

void *Task_1_through_3(
  void *argument
)
{
  int status;

  printf( "Task: pthread_mutex_trylock already locked\n" );
  status = pthread_mutex_trylock( &Mutex_id );
  if ( status != EBUSY )
    printf( "status = %d\n", status );
  assert( status == EBUSY );

  printf( "Task: pthread_mutex_lock unavailable\n" );
  status = pthread_mutex_lock( &Mutex_id );
  if ( status )
    printf( "status = %d\n", status );
  assert( !status );

  printf( "Task: mutex acquired\n" );


  printf( "Task: exit\n" );
  pthread_exit( NULL );

     /* switch to Init */

  return NULL; /* just so the compiler thinks we returned something */
}
