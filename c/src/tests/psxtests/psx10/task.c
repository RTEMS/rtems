/*  Task_1
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

void *Task_1(
  void *argument
)
{
  int  status;

/* wait for a condition variable signal from Init */

  Task_id = pthread_self();
  printf( "Task_1: ID is 0x%08x\n", Task_id );

  status = pthread_mutex_init( &Mutex_id, NULL );
  assert( !status );

  status = pthread_mutex_lock( &Mutex_id );
  assert( !status );

  puts( "Task_1: pthread_cond_wait" );
  status = pthread_cond_wait( &Cond1_id, &Mutex_id );
  assert( !status );

  puts( "Task_1: back from pthread_cond_wait release mutex" );
  status = pthread_mutex_unlock( &Mutex_id );
  assert( !status );

/* wait for a condition variable broadcast from Init */

  status = pthread_mutex_lock( &Mutex_id );
  assert( !status );

  puts( "Task_1: pthread_cond_wait" );
  status = pthread_cond_wait( &Cond1_id, &Mutex_id );
  assert( !status );

  puts( "Task_1: back from pthread_cond_wait release mutex" );
  status = pthread_mutex_unlock( &Mutex_id );
  assert( !status );

  puts( "Task_1: task exit" );
  pthread_exit( NULL );

  return NULL; /* just so the compiler thinks we returned something */
}
