/*  Task_3
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

void *Task_3(
  void *argument
)
{
  int  status;

  Task_id = pthread_self();
  printf( "Task_3: ID is 0x%08x\n", Task_id );

  status = pthread_mutex_lock( &Mutex_id );
  assert( !status );

  puts( "Task_3: pthread_cond_wait" );
  status = pthread_cond_wait( &Cond1_id, &Mutex_id );
  if ( status != EINVAL )
    printf( "status = %d\n", status );
  assert( status == EINVAL );
  puts( "Task_3: pthread_cond_wait - EINVAL (mutex not locked after signal)");

  puts( "Task_3: task exit" );
  pthread_exit( NULL );

  return NULL; /* just so the compiler thinks we returned something */
}
