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

void print_schedparam(
  char               *prefix,
  struct sched_param *schedparam
)
{
  printf( "%ssched priority      = %d\n", prefix, schedparam->sched_priority ); 
#if defined(_POSIX_SPORADIC_SERVER)
  printf( "%sss_low_priority     = %d\n", prefix, schedparam->ss_low_priority );
  printf( "%sss_replenish_period = (%ld, %ld)\n", prefix,
     schedparam->ss_replenish_period.tv_sec,
     schedparam->ss_replenish_period.tv_nsec );
  printf( "%sss_initial_budget = (%ld, %ld)\n", prefix,
     schedparam->ss_initial_budget.tv_sec,
     schedparam->ss_initial_budget.tv_nsec );
#else
  printf( "%s_POSIX_SPORADIC_SERVER is not defined\n" );
#endif
}
 
void *POSIX_Init(
  void *argument
)
{
  int                 status;
  int                 scope;
  int                 inheritsched;
  int                 schedpolicy;
  size_t              stacksize;
  void               *stackaddr;
  int                 detachstate;
  struct sched_param  schedparam;
  pthread_attr_t      attr;
  pthread_attr_t      destroyed_attr;

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

  /* check some errors in pthread_create */

  puts( "Init: pthread_create - EINVAL (attr not initialized)" );
  status = pthread_create( &Task_id, &destroyed_attr, Task_1, NULL );
  assert( status == EINVAL );

  /* junk stack address */
  status = pthread_attr_setstackaddr( &attr, (void *)&schedparam );
  assert( !status );
 
  /* must go around pthread_attr_setstacksize to set a bad stack size */
  attr.stacksize = 0;
 
  puts( "Init: pthread_create - EINVAL (stacksize too small)" );
  status = pthread_create( &Task_id, &attr, Task_1, NULL );
  assert( status == EINVAL );

  /* reset all the fields */
  status = pthread_attr_init( &attr );
  assert( !status );

  attr.stacksize = rtems_configuration_get_work_space_size() * 10;
  puts( "Init: pthread_create - EAGAIN (stacksize too large)" );
  status = pthread_create( &Task_id, &attr, Task_1, NULL );
  assert( status == EAGAIN );

  status = pthread_attr_init( &attr );
  assert( !status );

  /* must go around pthread_attr_set routines to set a bad value */
  attr.inheritsched = -1;
  
  puts( "Init: pthread_create - EINVAL (invalid inherit scheduler)" );
  status = pthread_create( &Task_id, &attr, Task_1, NULL );
  assert( status == EINVAL );

  /* check out the error case for system scope not supported */

  status = pthread_attr_init( &attr );
  assert( !status );

  /* must go around pthread_attr_set routines to set a bad value */
  attr.contentionscope = PTHREAD_SCOPE_SYSTEM;

  puts( "Init: pthread_create - ENOTSUP (unsupported system contention scope)" );
  status = pthread_create( &Task_id, &attr, Task_1, NULL );
  assert( status == ENOTSUP );

  status = pthread_attr_init( &attr );
  assert( !status );

  /* now check out pthread_create for inherit scheduler */

  status = pthread_attr_setinheritsched( &attr, PTHREAD_INHERIT_SCHED );
  assert( !status );
  
  puts( "Init: pthread_create - SUCCESSFUL (inherit scheduler)" );
  status = pthread_create( &Task_id, &attr, Task_1, NULL );
  assert( !status );

  status = pthread_join( Task_id, NULL );
  assert( !status );

    /* switch to Task_1 */

  /* exercise get and set scope */

  empty_line();

  status = pthread_attr_init( &attr );
  assert( !status );

  puts( "Init: pthread_attr_setscope - EINVAL (NULL attr)" );
  status = pthread_attr_setscope( NULL, PTHREAD_SCOPE_PROCESS );
  assert( status == EINVAL );

  puts( "Init: pthread_attr_setscope - ENOTSUP" );
  status = pthread_attr_setscope( &attr, PTHREAD_SCOPE_SYSTEM );
  assert( status == ENOTSUP );

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
 
  puts( "Init: pthread_attr_getscope - EINVAL (NULL scope)" );
  status = pthread_attr_getscope( &attr, NULL );
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
 
  puts( "Init: pthread_attr_setinheritsched - ENOTSUP (invalid inheritsched)" );
  status = pthread_attr_setinheritsched( &attr, -1 );
  assert( status == ENOTSUP );
 
  puts( "Init: pthread_attr_setinheritsched - SUCCESSFUL" );
  status = pthread_attr_setinheritsched( &attr, PTHREAD_INHERIT_SCHED );
  assert( !status );
 
  puts( "Init: pthread_attr_getinheritsched - EINVAL (NULL attr)" );
  status = pthread_attr_getinheritsched( NULL, &inheritsched );
  assert( status == EINVAL );
 
  puts( "Init: pthread_attr_getinheritsched - EINVAL (NULL inheritsched)" );
  status = pthread_attr_getinheritsched( &attr, NULL );
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
 
  puts( "Init: pthread_attr_setschedpolicy - ENOTSUP (invalid schedpolicy)" );
  status = pthread_attr_setschedpolicy( &attr, -1 );
  assert( status == ENOTSUP );
 
  puts( "Init: pthread_attr_setschedpolicy - SUCCESSFUL" );
  status = pthread_attr_setschedpolicy( &attr, SCHED_RR );
  assert( !status );
 
  puts( "Init: pthread_attr_getschedpolicy - EINVAL (NULL attr)" );
  status = pthread_attr_getschedpolicy( NULL, &schedpolicy );
  assert( status == EINVAL );
 
  puts( "Init: pthread_attr_getschedpolicy - EINVAL (NULL schedpolicy)" );
  status = pthread_attr_getschedpolicy( &attr, NULL );
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
 
  puts( "Init: pthread_attr_getstacksize - EINVAL (NULL stacksize)" );
  status = pthread_attr_getstacksize( &attr, NULL );
  assert( status == EINVAL );
 
  puts( "Init: pthread_attr_getstacksize - EINVAL (not initialized attr)" );
  status = pthread_attr_getstacksize( &destroyed_attr, &stacksize );
  assert( status == EINVAL );
 
  puts( "Init: pthread_attr_getstacksize - SUCCESSFUL" );
  status = pthread_attr_getstacksize( &attr, &stacksize );
  assert( !status );
  if ( stacksize == (STACK_MINIMUM_SIZE * 2) )
  printf( "Init: current stack size attribute is OK\n" );

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
 
  puts( "Init: pthread_attr_getstackaddr - EINVAL (NULL stackaddr)" );
  status = pthread_attr_getstackaddr( &attr, NULL );
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
 
  puts( "Init: pthread_attr_getdetachstate - EINVAL (NULL detatchstate)" );
  status = pthread_attr_getdetachstate( &attr, NULL );
  assert( status == EINVAL );
 
  puts( "Init: pthread_attr_getdetachstate - EINVAL (not initialized attr)" );
  status = pthread_attr_getdetachstate( &destroyed_attr, &detachstate );
  assert( status == EINVAL );
 
  puts( "Init: pthread_attr_getdetachstate - SUCCESSFUL" );
  status = pthread_attr_getdetachstate( &attr, &detachstate );
  assert( !status );
  printf( "Init: current detach state attribute = %d\n", detachstate );

  /* exercise get and set scheduling parameters */

  empty_line();

  puts( "Init: pthread_attr_getschedparam - SUCCESSFUL" );
  status = pthread_attr_getschedparam( &attr, &schedparam );
  assert( !status );

  print_schedparam( "Init: ", &schedparam );

  puts( "Init: pthread_attr_setschedparam - EINVAL (NULL attr)" );
  status = pthread_attr_setschedparam( NULL, &schedparam );
  assert( status == EINVAL );
 
  puts( "Init: pthread_attr_setschedparam - EINVAL (not initialized attr)" );
  status = pthread_attr_setschedparam( &destroyed_attr, &schedparam );
  assert( status == EINVAL );
 
  puts( "Init: pthread_attr_setschedparam - EINVAL (NULL schedparam)" );
  status = pthread_attr_setschedparam( &attr, NULL );
  assert( status == EINVAL );
 
  puts( "Init: pthread_attr_setschedparam - SUCCESSFUL" );
  status = pthread_attr_setschedparam( &attr, &schedparam );
  assert( !status );
 
  puts( "Init: pthread_attr_getschedparam - EINVAL (NULL attr)" );
  status = pthread_attr_getschedparam( NULL, &schedparam );
  assert( status == EINVAL );
 
  puts( "Init: pthread_attr_getschedparam - EINVAL (not initialized attr)" );
  status = pthread_attr_getschedparam( &destroyed_attr, &schedparam );
  assert( status == EINVAL );
 
  puts( "Init: pthread_attr_getschedparam - EINVAL (NULL schedparam)" );
  status = pthread_attr_getschedparam( &attr, NULL );
  assert( status == EINVAL );
 
  /* exercise pthread_getschedparam */

  empty_line();

  puts( "Init: pthread_getschedparam - EINVAL (NULL policy)" );
  status = pthread_getschedparam( pthread_self(), NULL, &schedparam );
  assert( status == EINVAL );
 
  puts( "Init: pthread_getschedparam - EINVAL (NULL schedparam)" );
  status = pthread_getschedparam( pthread_self(), &schedpolicy, NULL );
  assert( status == EINVAL );
 
  puts( "Init: pthread_getschedparam - ESRCH (bad thread)" );
  status = pthread_getschedparam( -1, &schedpolicy, &schedparam );
  assert( status == ESRCH );
 
  puts( "Init: pthread_getschedparam - SUCCESSFUL" );
  status = pthread_getschedparam( pthread_self(), &schedpolicy, &schedparam );
  assert( !status );
 
  printf( "Init: policy = %d\n", schedpolicy );

  print_schedparam( "Init: ", &schedparam );

  /* exercise pthread_setschedparam */

  empty_line();

  puts( "Init: pthread_setschedparam - EINVAL (NULL schedparam)" );
  status = pthread_setschedparam( pthread_self(), SCHED_OTHER, NULL );
  assert( status == EINVAL );

  schedparam.sched_priority = -1;

  puts( "Init: pthread_setschedparam - EINVAL (invalid priority)" );
  status = pthread_setschedparam( pthread_self(), SCHED_OTHER, NULL );
  assert( status == EINVAL );

  schedparam.sched_priority = 128;

  puts( "Init: pthread_setschedparam - EINVAL (invalid policy)" );
  status = pthread_setschedparam( pthread_self(), -1, &schedparam );
  assert( status == EINVAL );

  puts( "Init: pthread_setschedparam - ESRCH (invalid thread)" );
  status = pthread_setschedparam( -1, SCHED_OTHER, &schedparam );
  assert( status == ESRCH );

  /* now get sporadic server errors */ 

  schedparam.ss_replenish_period.tv_sec = 1;
  schedparam.ss_replenish_period.tv_nsec = 0;
  schedparam.ss_initial_budget.tv_sec = 1;
  schedparam.ss_initial_budget.tv_nsec = 1;

  puts( "Init: pthread_setschedparam - EINVAL (replenish < budget)" );
  status = pthread_setschedparam( pthread_self(), SCHED_SPORADIC, &schedparam );
  assert( status == EINVAL );

  schedparam.ss_replenish_period.tv_sec = 2;
  schedparam.ss_replenish_period.tv_nsec = 0;
  schedparam.ss_initial_budget.tv_sec = 1;
  schedparam.ss_initial_budget.tv_nsec = 0;
  schedparam.ss_low_priority = -1;

  puts( "Init: pthread_setschedparam - EINVAL (invalid priority)" );
  status = pthread_setschedparam( pthread_self(), SCHED_SPORADIC, &schedparam );
  assert( status == EINVAL );

  puts( "*** END OF POSIX TEST 7 ***" );
  rtems_test_exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}
