/* 
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
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
  int                  status;
  struct sched_param   param;
  pthread_attr_t       attr;

  puts( "\n\n*** POSIX TEST 11 ***" );

  /* set the time of day, and print our buffer in multiple ways */

  set_time( TM_FRIDAY, TM_MAY, 24, 96, 11, 5, 0 );

  /* get id of this thread */

  Init_id = pthread_self();
  printf( "Init's ID is 0x%08x\n", Init_id );
  
  /* exercise pthread_setschedparam */

  param.sched_priority = 127;

  puts( "Init: Setting scheduling parameters to FIFO with priority 127" );
  status = pthread_setschedparam( Init_id, SCHED_FIFO, &param );
  assert( !status );

  param.sched_priority = 125;

  puts( "Init: Setting scheduling parameters to RR with priority 125" );
  status = pthread_setschedparam( Init_id, SCHED_RR, &param );
  assert( !status );

  param.sched_priority = 121;

  puts( "Init: Setting scheduling parameters to OTHER with priority 121" );
  status = pthread_setschedparam( Init_id, SCHED_OTHER, &param );
  assert( !status );

  /* create a thread as SCHED_FIFO */
 
  puts( "Init: create a thread of SCHED_FIFO with priority 120" );
  status = pthread_attr_init( &attr );
  assert( !status );
 
  attr.schedpolicy = SCHED_FIFO;
  attr.schedparam.sched_priority = 120;
 
  status = pthread_create( &Task_id, &attr, Task_1, NULL );
  assert( !status );
 
  puts( "Init: join with the other thread" );
  status = pthread_join( Task_id, NULL );
  assert( !status );

  /* create a thread as SCHED_RR */
 
  puts( "Init: create a thread of SCHED_RR with priority 120" );
  status = pthread_attr_init( &attr );
  assert( !status );
 
  attr.schedpolicy = SCHED_RR;
  attr.schedparam.sched_priority = 120;
 
  status = pthread_create( &Task_id, &attr, Task_1, NULL );
  assert( !status );
 
  puts( "Init: join with the other thread" );
  status = pthread_join( Task_id, NULL );
  assert( !status );

  /* create a thread as SCHED_OTHER */

  puts( "Init: create a thread of SCHED_OTHER with priority 120" );
  status = pthread_attr_init( &attr );
  assert( !status );

  attr.schedpolicy = SCHED_OTHER;
  attr.schedparam.sched_priority = 120;

  status = pthread_create( &Task_id, &attr, Task_1, NULL );
  assert( !status );

  puts( "Init: join with the other thread" );
  status = pthread_join( Task_id, NULL );
  assert( !status );

  puts( "*** END OF POSIX TEST 11 ***" );
  rtems_test_exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}
