/* 
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

#define CONFIGURE_INIT
#include "system.h"
#include <sched.h>


void *POSIX_Init(
  void *argument
)
{
  int                 status;
  pthread_t           thread_id;
  pthread_condattr_t  attr;
  int                 pshared;
  pthread_cond_t      cond = PTHREAD_COND_INITIALIZER;

  puts( "\n\n*** POSIX TEST 10 ***" );

  puts( "Init: pthread_condattr_init" );
  status = pthread_condattr_init( &attr );
  assert( !status );

  puts( "Init: pthread_condattr_init - EINVAL" );
  status = pthread_condattr_init( NULL );
  if ( status != EINVAL )
    printf( "status = %d\n", status );
  assert( status == EINVAL );

  puts( "Init: pthread_condattr_destroy" );
  status = pthread_condattr_destroy( &attr );
  assert( !status );

  puts( "Init: pthread_condattr_destroy - EINVAL" );
  status = pthread_condattr_destroy( NULL );
  if ( status != EINVAL )
    printf( "status = %d\n", status );
  assert( status == EINVAL );

  puts( "Init: pthread_condattr_setpshared - PTHREAD_PROCESS_SHARED" );
  status = pthread_condattr_setpshared( &attr, PTHREAD_PROCESS_SHARED );
  assert( !status );

  puts( "Init: pthread_condattr_setpshared - PTHREAD_PROCESS_PRIVATE" );
  status = pthread_condattr_setpshared( &attr, PTHREAD_PROCESS_PRIVATE );
  assert( !status );

  status = pthread_condattr_setpshared( NULL, PTHREAD_PROCESS_PRIVATE );
  if ( status != EINVAL )
    printf( "status = %d\n", status );
  assert( status == EINVAL );
  puts( "Init: pthread_condattr_setpshared - EINVAL - attr" );

  status = pthread_condattr_setpshared( &attr, 0xFFFFFF );
  if ( status != EINVAL )
    printf( "status = %d\n", status );
  assert( status == EINVAL );
  puts( "Init: pthread_condattr_setpshared - EINVAL - pshared" );

  status = pthread_condattr_getpshared( &attr, &pshared );
  assert( !status );
  printf( "Init: pthread_condattr_getpshared - %d\n", pshared );

  status = pthread_condattr_getpshared( NULL, &pshared );
  if ( status != EINVAL )
    printf( "status = %d\n", status );
  assert( status == EINVAL );
  puts( "Init: pthread_condattr_getpshared - EINVAL" );

  Init_id = pthread_self();
  printf( "Init: ID is 0x%08x\n", Init_id );

  puts( "Init: pthread_cond_init - NULL attr" );
  status = pthread_cond_init( &cond, NULL );
  assert( !status );

  puts( "Init: pthread_cond_destroy" );
  status = pthread_cond_destroy( &cond );
  assert( !status );

  /* create a thread */

/*
  status = pthread_create( &thread_id, NULL, Task_1_through_3, NULL );
  assert( !status );
*/

  /* exit this thread */

  puts( "*** END OF POSIX TEST 5 ***" );
  exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}
