/*  Task_1
 *
 *  This routine serves as a test task.
 *
 *  Input parameters:
 *    argument - task argument
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
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
