/*  Task_2
 *
 *  This routine serves as a test task.
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
  int status;

  printf( "Task 2: pthread_mutex_lock unavailable (inherit case)\n" );
  status = pthread_mutex_lock( &Mutex2_id );
  if ( status )
    printf( "status =%d\n", status );
  assert( !status );
  printf( "Task 2: mutex acquired\n" );

     /* switch to init */

  printf( "Task 2: unlock Mutex 2\n" );
  status = pthread_mutex_unlock( &Mutex2_id );
  assert( !status );

  printf( "Task 2: exit\n" );
  pthread_exit( NULL );

     /* switch to Init */

  return NULL; /* just so the compiler thinks we returned something */
}
