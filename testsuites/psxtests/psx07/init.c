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
#include <errno.h>

void *POSIX_Init(
  void *argument
)
{
  int             status;
  int             scope;
  int             inheritsched;
  int             schedpolicy;
  size_t          stacksize;
  void           *stackaddr;
  int             detachstate;
  pthread_attr_t  attr;
  pthread_attr_t  destroyed_attr;

  puts( "\n\n*** POSIX TEST 7 ***" );

  /* set the time of day, and print our buffer in multiple ways */

  set_time( TM_FRIDAY, TM_MAY, 24, 96, 11, 5, 0 );

  /* get id of this thread */

  Init_id = pthread_self();
  printf( "Init's ID is 0x%08x\n", Init_id );
  
  /* exercise init and destroy */

  puts( "Init: pthread_attr_init - EINVAL (NULL attr)" );
  status = pthread_attr_init( NULL );
  assert( status == EINVAL );

  puts( "Init: pthread_attr_init - SUCCESSFUL" );
  status = pthread_attr_init( &attr );
  assert( !status );

  puts( "Init: initialize and destroy an attribute - SUCCESSFUL" );
  status = pthread_attr_init( &destroyed_attr );
  assert( !status );

  status = pthread_attr_destroy( &destroyed_attr );
  assert( !status );

  puts( "Init: pthread_attr_destroy - EINVAL (NULL attr)" );
  status = pthread_attr_destroy( NULL );
  assert( status == EINVAL );

  puts( "Init: pthread_attr_destroy - EINVAL (not initialized)" );
  status = pthread_attr_destroy( &destroyed_attr );
  assert( status == EINVAL );

  /* exercise get and set scope */

  empty_line();

  puts( "Init: pthread_attr_setscope - EINVAL (NULL attr)" );
  status = pthread_attr_setscope( NULL, PTHREAD_SCOPE_PROCESS );
  assert( status == EINVAL );

  puts( "Init: pthread_attr_setscope - ENOSYS" );
  status = pthread_attr_setscope( &attr, PTHREAD_SCOPE_SYSTEM );
  assert( status == ENOSYS );

  puts( "Init: pthread_attr_setscope - EINVAL (not initialized attr)" );
  status = pthread_attr_setscope( &destroyed_attr, PTHREAD_SCOPE_PROCESS );
  assert( status == EINVAL );

  puts( "Init: pthread_attr_setscope - EINVAL (invalid scope)" );
  status = pthread_attr_setscope( &attr, -1 );
  assert( status == EINVAL );

  puts( "Init: pthread_attr_setscope - SUCCESSFUL" );
  status = pthread_attr_setscope( &attr, PTHREAD_SCOPE_PROCESS );
  assert( !status );
 
  puts( "Init: pthread_attr_getscope - EINVAL (NULL attr)" );
  status = pthread_attr_getscope( NULL, &scope );
  assert( status == EINVAL );
 
  puts( "Init: pthread_attr_getscope - EINVAL (not initialized attr)" );
  status = pthread_attr_getscope( &destroyed_attr, &scope );
  assert( status == EINVAL );
 
  puts( "Init: pthread_attr_getscope - SUCCESSFUL" );
  status = pthread_attr_getscope( &attr, &scope );
  assert( !status );
  printf( "Init: current scope attribute = %d\n", scope );

  /* exercise get and set inherit scheduler */

  empty_line();

  puts( "Init: pthread_attr_setinheritsched - EINVAL (NULL attr)" );
  status = pthread_attr_setinheritsched( NULL, PTHREAD_INHERIT_SCHED );
  assert( status == EINVAL );
 
  puts( "Init: pthread_attr_setinheritsched - EINVAL (not initialized attr)" );
  status =
     pthread_attr_setinheritsched( &destroyed_attr, PTHREAD_INHERIT_SCHED );
  assert( status == EINVAL );
 
  puts( "Init: pthread_attr_setinheritsched - EINVAL (invalid inheritsched)" );
  status = pthread_attr_setinheritsched( &attr, -1 );
  assert( status == EINVAL );
 
  puts( "Init: pthread_attr_setinheritsched - SUCCESSFUL" );
  status = pthread_attr_setinheritsched( &attr, PTHREAD_INHERIT_SCHED );
  assert( !status );
 
  puts( "Init: pthread_attr_getinheritsched - EINVAL (NULL attr)" );
  status = pthread_attr_getinheritsched( NULL, &inheritsched );
  assert( status == EINVAL );
 
  puts( "Init: pthread_attr_getinheritsched - EINVAL (not initialized attr)" );
  status = pthread_attr_getinheritsched( &destroyed_attr, &inheritsched );
  assert( status == EINVAL );
 
  puts( "Init: pthread_attr_getinheritsched - SUCCESSFUL" );
  status = pthread_attr_getinheritsched( &attr, &inheritsched );
  assert( !status );
  printf( "Init: current inherit scheduler attribute = %d\n", inheritsched );

  /* exercise get and set inherit scheduler */
 
  empty_line();
 
  puts( "Init: pthread_attr_setschedpolicy - EINVAL (NULL attr)" );
  status = pthread_attr_setschedpolicy( NULL, SCHED_FIFO );
  assert( status == EINVAL );
 
  puts( "Init: pthread_attr_setschedpolicy - EINVAL (not initialized attr)" );
  status =
     pthread_attr_setschedpolicy( &destroyed_attr, SCHED_OTHER );
  assert( status == EINVAL );
 
  puts( "Init: pthread_attr_setschedpolicy - EINVAL (invalid schedpolicy)" );
  status = pthread_attr_setschedpolicy( &attr, -1 );
  assert( status == EINVAL );
 
  puts( "Init: pthread_attr_setschedpolicy - SUCCESSFUL" );
  status = pthread_attr_setschedpolicy( &attr, SCHED_RR );
  assert( !status );
 
  puts( "Init: pthread_attr_getschedpolicy - EINVAL (NULL attr)" );
  status = pthread_attr_getschedpolicy( NULL, &schedpolicy );
  assert( status == EINVAL );
 
  puts( "Init: pthread_attr_getschedpolicy - EINVAL (not initialized attr)" );
  status = pthread_attr_getschedpolicy( &destroyed_attr, &schedpolicy );
  assert( status == EINVAL );
 
  puts( "Init: pthread_attr_getschedpolicy - SUCCESSFUL" );
  status = pthread_attr_getschedpolicy( &attr, &schedpolicy );
  assert( !status );
  printf( "Init: current scheduler policy attribute = %d\n", schedpolicy );
 
  /* exercise get and set stack size */
 
  empty_line();
 
  puts( "Init: pthread_attr_setstacksize - EINVAL (NULL attr)" );
  status = pthread_attr_setstacksize( NULL, 0 );
  assert( status == EINVAL );
 
  puts( "Init: pthread_attr_setstacksize - EINVAL (not initialized attr)" );
  status =
     pthread_attr_setstacksize( &destroyed_attr, 0 );
  assert( status == EINVAL );
 
  puts( "Init: pthread_attr_setstacksize - SUCCESSFUL (low stacksize)" );
  status = pthread_attr_setstacksize( &attr, 0 );
  assert( !status );

  puts( "Init: pthread_attr_setstacksize - SUCCESSFUL (high stacksize)" );
  status = pthread_attr_setstacksize( &attr, STACK_MINIMUM_SIZE * 2 );
  assert( !status );
 
  puts( "Init: pthread_attr_getstacksize - EINVAL (NULL attr)" );
  status = pthread_attr_getstacksize( NULL, &stacksize );
  assert( status == EINVAL );
 
  puts( "Init: pthread_attr_getstacksize - EINVAL (not initialized attr)" );
  status = pthread_attr_getstacksize( &destroyed_attr, &stacksize );
  assert( status == EINVAL );
 
  puts( "Init: pthread_attr_getstacksize - SUCCESSFUL" );
  status = pthread_attr_getstacksize( &attr, &stacksize );
  assert( !status );
  printf( "Init: current stack size attribute = %ld\n", (long) stacksize );

  /* exercise get and set stack address */
 
  empty_line();
 
  puts( "Init: pthread_attr_setstackaddr - EINVAL (NULL attr)" );
  status = pthread_attr_setstackaddr( NULL, NULL );
  assert( status == EINVAL );
 
  puts( "Init: pthread_attr_setstackaddr - EINVAL (not initialized attr)" );
  status =
     pthread_attr_setstackaddr( &destroyed_attr, NULL );
  assert( status == EINVAL );
 
  puts( "Init: pthread_attr_setstackaddr - SUCCESSFUL" );
  status = pthread_attr_setstackaddr( &attr, 0 );
  assert( !status );
 
  puts( "Init: pthread_attr_getstackaddr - EINVAL (NULL attr)" );
  status = pthread_attr_getstackaddr( NULL, &stackaddr );
  assert( status == EINVAL );
 
  puts( "Init: pthread_attr_getstackaddr - EINVAL (not initialized attr)" );
  status = pthread_attr_getstackaddr( &destroyed_attr, &stackaddr );
  assert( status == EINVAL );
 
  puts( "Init: pthread_attr_getstackaddr - SUCCESSFUL" );
  status = pthread_attr_getstackaddr( &attr, &stackaddr );
  assert( !status );
  printf( "Init: current stack address attribute = %p\n", stackaddr );
 
  /* exercise get and set detach state */
 
  empty_line();
 
  puts( "Init: pthread_attr_setdetachstate - EINVAL (NULL attr)" );
  status = pthread_attr_setdetachstate( NULL, PTHREAD_CREATE_DETACHED );
  assert( status == EINVAL );
 
  puts( "Init: pthread_attr_setdetachstate - EINVAL (not initialized attr)" );
  status =
     pthread_attr_setdetachstate( &destroyed_attr, PTHREAD_CREATE_JOINABLE );
  assert( status == EINVAL );
 
  puts( "Init: pthread_attr_setdetachstate - EINVAL (invalid detachstate)" );
  status = pthread_attr_setdetachstate( &attr, -1 );
  assert( status == EINVAL );
 
  puts( "Init: pthread_attr_setdetachstate - SUCCESSFUL" );
  status = pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_JOINABLE );
  assert( !status );
 
  puts( "Init: pthread_attr_getdetachstate - EINVAL (NULL attr)" );
  status = pthread_attr_getdetachstate( NULL, &detachstate );
  assert( status == EINVAL );
 
  puts( "Init: pthread_attr_getdetachstate - EINVAL (not initialized attr)" );
  status = pthread_attr_getdetachstate( &destroyed_attr, &detachstate );
  assert( status == EINVAL );
 
  puts( "Init: pthread_attr_getdetachstate - SUCCESSFUL" );
  status = pthread_attr_getdetachstate( &attr, &detachstate );
  assert( !status );
  printf( "Init: current detach state attribute = %d\n", detachstate );

  puts( "*** END OF POSIX TEST 7 ***" );
  exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}
