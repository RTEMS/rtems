/*
 *  COPYRIGHT (c) 1989-2008.
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
#include "tmacros.h"

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
  int                 clock_allowed;

  puts( "\n\n*** POSIX TEST 7 ***" );

  /* set the time of day, and print our buffer in multiple ways */

  set_time( TM_FRIDAY, TM_MAY, 24, 96, 11, 5, 0 );

  /* get id of this thread */

  Init_id = pthread_self();
  printf( "Init's ID is 0x%08x\n", Init_id );

  /* exercise init and destroy */

  puts( "Init: pthread_attr_init - EINVAL (NULL attr)" );
  status = pthread_attr_init( NULL );
  fatal_directive_check_status_only( status, EINVAL, "null attribute" );

  puts( "Init: pthread_attr_init - SUCCESSFUL" );
  status = pthread_attr_init( &attr );
  posix_service_failed( status, "pthread_attr_init" );

  puts( "Init: initialize and destroy an attribute - SUCCESSFUL" );
  status = pthread_attr_init( &destroyed_attr );
  posix_service_failed( status, "pthread_attr_init");

  status = pthread_attr_destroy( &destroyed_attr );
  posix_service_failed( status, "pthread_attr_destroy");

  puts( "Init: pthread_attr_destroy - EINVAL (NULL attr)" );
  status = pthread_attr_destroy( NULL );
  fatal_directive_check_status_only( status, EINVAL, "NULL attribute" );

  puts( "Init: pthread_attr_destroy - EINVAL (not initialized)" );
  status = pthread_attr_destroy( &destroyed_attr );
  fatal_directive_check_status_only( status, EINVAL, "not initialized" );

  /* check some errors in pthread_create */

  puts( "Init: pthread_create - EINVAL (attr not initialized)" );
  status = pthread_create( &Task_id, &destroyed_attr, Task_1, NULL );
  fatal_directive_check_status_only( status, EINVAL, "attribute not initialized" );

  /* junk stack address */
  status = pthread_attr_setstackaddr( &attr, (void *)&schedparam );
  posix_service_failed( status, "setstackaddr");

  /* must go around pthread_attr_setstacksize to set a bad stack size */
  attr.stacksize = 0;

  puts( "Init: pthread_create - EINVAL (stacksize too small)" );
  status = pthread_create( &Task_id, &attr, Task_1, NULL );
  fatal_directive_check_status_only( status, EINVAL, "stacksize too small" );

  /* reset all the fields */
  status = pthread_attr_init( &attr );
  posix_service_failed( status, "pthread_attr_init");
  
  attr.stacksize = rtems_configuration_get_work_space_size() * 10;
  puts( "Init: pthread_create - EAGAIN (stacksize too large)" );
  status = pthread_create( &Task_id, &attr, Task_1, NULL );
  fatal_directive_check_status_only( status, EAGAIN, "stacksize too large" );

  status = pthread_attr_init( &attr );
  posix_service_failed( status, "pthread_attr_init");

  /* must go around pthread_attr_set routines to set a bad value */
  attr.inheritsched = -1;

  puts( "Init: pthread_create - EINVAL (invalid inherit scheduler)" );
  status = pthread_create( &Task_id, &attr, Task_1, NULL );
  fatal_directive_check_status_only( status, EINVAL, "invalid inherit scheduler" );

  /* check out the error case for system scope not supported */

  status = pthread_attr_init( &attr );
  posix_service_failed( status, " pthread_attr_init");

  /* Check out pthread_attr_settime and pthread_attr_gettime */
  puts( "Init: pthread_attr_settime - EINVAL ( null attribute )" );
  status = pthread_attr_setcputime( NULL, CLOCK_ENABLED );
  fatal_directive_check_status_only( status, EINVAL, "null attribute" );

  puts( "Init: pthread_attr_gettime - EINVAL ( null attribute )" );
  status = pthread_attr_getcputime( NULL, &clock_allowed );
  fatal_directive_check_status_only( status, EINVAL, " null attribute" );

  puts( "Init: pthread_attr_settime - EINVAL ( is initialized )" );
  status = pthread_attr_setcputime( &destroyed_attr, CLOCK_ENABLED );
  fatal_directive_check_status_only( status, EINVAL, "is initialized" );

  puts( "Init: pthread_attr_gettime - EINVAL ( is initialized )" );
  status = pthread_attr_getcputime( &destroyed_attr, &clock_allowed  );
  fatal_directive_check_status_only( status, EINVAL, "is initialized" );

  puts( "Init: pthread_attr_settime - EINVAL ( invalid clock allowed )" );
  status = pthread_attr_setcputime( &attr, ~(CLOCK_ENABLED | CLOCK_DISABLED) );
  fatal_directive_check_status_only( status, EINVAL, "invalid clock allowed" );

  puts( "Init: pthread_attr_gettime - EINVAL ( NULL clock allowed )" );
  status = pthread_attr_getcputime( &attr, NULL );
  fatal_directive_check_status_only( status, EINVAL, "NULL clock allowed" );

  puts( "Init: validate pthread_attr_setcputime - CLOCK_DISABLED" );
  status = pthread_attr_setcputime( &attr, CLOCK_DISABLED );
  posix_service_failed( status, "pthread_attr_setcputime");
  status = pthread_attr_getcputime( &attr, &clock_allowed );
  posix_service_failed( status, "pthread_attr_getcputime");
  if (attr.cputime_clock_allowed != CLOCK_DISABLED)
    perror("ERROR==> pthread_attr_setcputime to CLOCK_DISABLED failed");

  puts( "Init: validate pthread_attr_setcputime - CLOCK_ENABLED" );
  status = pthread_attr_setcputime( &attr, CLOCK_ENABLED );
  posix_service_failed( status, "pthread_attr_setcputime");
  status = pthread_attr_getcputime( &attr, &clock_allowed );
  posix_service_failed( status, "pthread_attr_getcputime");
  if (attr.cputime_clock_allowed != CLOCK_ENABLED)
    perror("ERROR==> pthread_attr_setcputime to CLOCK_ENABLED failed");

  /* must go around pthread_attr_set routines to set a bad value */
  attr.contentionscope = PTHREAD_SCOPE_SYSTEM;

  puts( "Init: pthread_create - ENOTSUP (unsupported system contention scope)" );
  status = pthread_create( &Task_id, &attr, Task_1, NULL );
  fatal_directive_check_status_only( status, ENOTSUP, 
    "unsupported system contention scope" );

  status = pthread_attr_init( &attr );
  posix_service_failed( status, "pthread_attr_init");

  /* now check out pthread_create for inherit scheduler */

  status = pthread_attr_setinheritsched( &attr, PTHREAD_INHERIT_SCHED );
  posix_service_failed( status, "pthread_attr_setinheritsched");

  puts( "Init: pthread_create - SUCCESSFUL (inherit scheduler)" );
  status = pthread_create( &Task_id, &attr, Task_1, NULL );
  posix_service_failed( status, "pthread_create");

  status = pthread_join( Task_id, NULL );
  posix_service_failed( status, " pthread_join");

    /* switch to Task_1 */

  /* exercise get and set scope */

  empty_line();

  status = pthread_attr_init( &attr );
  posix_service_failed( status, "pthread_attr_init");

  puts( "Init: pthread_attr_setscope - EINVAL (NULL attr)" );
  status = pthread_attr_setscope( NULL, PTHREAD_SCOPE_PROCESS );
  fatal_directive_check_status_only( status, EINVAL , "NULL attr" );

  puts( "Init: pthread_attr_setscope - ENOTSUP" );
  status = pthread_attr_setscope( &attr, PTHREAD_SCOPE_SYSTEM );
  fatal_directive_check_status_only( status, ENOTSUP, "PTHREAD_SCOPE_SYSTEM" );

  puts( "Init: pthread_attr_setscope - EINVAL (not initialized attr)" );
  status = pthread_attr_setscope( &destroyed_attr, PTHREAD_SCOPE_PROCESS );
  fatal_directive_check_status_only( status, EINVAL, "not initialized attr" );

  puts( "Init: pthread_attr_setscope - EINVAL (invalid scope)" );
  status = pthread_attr_setscope( &attr, -1 );
  fatal_directive_check_status_only( status, EINVAL, "invalid scope" );

  puts( "Init: pthread_attr_setscope - SUCCESSFUL" );
  status = pthread_attr_setscope( &attr, PTHREAD_SCOPE_PROCESS );
  posix_service_failed( status, "pthread_attr_setscope");

  puts( "Init: pthread_attr_getscope - EINVAL (NULL attr)" );
  status = pthread_attr_getscope( NULL, &scope );
  fatal_directive_check_status_only( status, EINVAL, "NULL attr" );

  puts( "Init: pthread_attr_getscope - EINVAL (NULL scope)" );
  status = pthread_attr_getscope( &attr, NULL );
  fatal_directive_check_status_only( status, EINVAL, "NULL scope" );

  puts( "Init: pthread_attr_getscope - EINVAL (not initialized attr)" );
  status = pthread_attr_getscope( &destroyed_attr, &scope );
  fatal_directive_check_status_only( status, EINVAL, "not initialized attr" );

  puts( "Init: pthread_attr_getscope - SUCCESSFUL" );
  status = pthread_attr_getscope( &attr, &scope );
  posix_service_failed( status, "pthread_attr_getscope");
  printf( "Init: current scope attribute = %d\n", scope );

  /* exercise get and set inherit scheduler */

  empty_line();

  puts( "Init: pthread_attr_setinheritsched - EINVAL (NULL attr)" );
  status = pthread_attr_setinheritsched( NULL, PTHREAD_INHERIT_SCHED );
  fatal_directive_check_status_only( status, EINVAL, "NULL attr" );

  puts( "Init: pthread_attr_setinheritsched - EINVAL (not initialized attr)" );
  status =
     pthread_attr_setinheritsched( &destroyed_attr, PTHREAD_INHERIT_SCHED );
  fatal_directive_check_status_only( status, EINVAL, "not initialized attr" );

  puts( "Init: pthread_attr_setinheritsched - ENOTSUP (invalid inheritsched)" );
  status = pthread_attr_setinheritsched( &attr, -1 );
  fatal_directive_check_status_only( status, ENOTSUP, "invalid inheritsched" );

  puts( "Init: pthread_attr_setinheritsched - SUCCESSFUL" );
  status = pthread_attr_setinheritsched( &attr, PTHREAD_INHERIT_SCHED );
  posix_service_failed( status, "pthread_attr_setinheritsched");

  puts( "Init: pthread_attr_getinheritsched - EINVAL (NULL attr)" );
  status = pthread_attr_getinheritsched( NULL, &inheritsched );
  fatal_directive_check_status_only( status, EINVAL, "NULL attr" );

  puts( "Init: pthread_attr_getinheritsched - EINVAL (NULL inheritsched)" );
  status = pthread_attr_getinheritsched( &attr, NULL );
  fatal_directive_check_status_only( status, EINVAL, "NULL inheritsched" );

  puts( "Init: pthread_attr_getinheritsched - EINVAL (not initialized attr)" );
  status = pthread_attr_getinheritsched( &destroyed_attr, &inheritsched );
  fatal_directive_check_status_only( status, EINVAL, "not initialized attr" );

  puts( "Init: pthread_attr_getinheritsched - SUCCESSFUL" );
  status = pthread_attr_getinheritsched( &attr, &inheritsched );
  posix_service_failed( status, "pthread_attr_getinheritsched");
  printf( "Init: current inherit scheduler attribute = %d\n", inheritsched );

  /* exercise get and set inherit scheduler */

  empty_line();

  puts( "Init: pthread_attr_setschedpolicy - EINVAL (NULL attr)" );
  status = pthread_attr_setschedpolicy( NULL, SCHED_FIFO );
  fatal_directive_check_status_only( status, EINVAL, "NULL attr" );

  puts( "Init: pthread_attr_setschedpolicy - EINVAL (not initialized attr)" );
  status =
     pthread_attr_setschedpolicy( &destroyed_attr, SCHED_OTHER );
  fatal_directive_check_status_only( status, EINVAL, "not initialized attr" );

  puts( "Init: pthread_attr_setschedpolicy - ENOTSUP (invalid schedpolicy)" );
  status = pthread_attr_setschedpolicy( &attr, -1 );
  fatal_directive_check_status_only( status, ENOTSUP, "invalid schedpolicy" );

  puts( "Init: pthread_attr_setschedpolicy - SUCCESSFUL" );
  status = pthread_attr_setschedpolicy( &attr, SCHED_RR );
  posix_service_failed( status, "pthread_attr_setschedpolicy");

  puts( "Init: pthread_attr_getschedpolicy - EINVAL (NULL attr)" );
  status = pthread_attr_getschedpolicy( NULL, &schedpolicy );
  fatal_directive_check_status_only( status, EINVAL, "NULL attr" );

  puts( "Init: pthread_attr_getschedpolicy - EINVAL (NULL schedpolicy)" );
  status = pthread_attr_getschedpolicy( &attr, NULL );
  fatal_directive_check_status_only( status, EINVAL, "NULL schedpolicy" );

  puts( "Init: pthread_attr_getschedpolicy - EINVAL (not initialized attr)" );
  status = pthread_attr_getschedpolicy( &destroyed_attr, &schedpolicy );
  fatal_directive_check_status_only( status, EINVAL, "not initialized attr" );

  puts( "Init: pthread_attr_getschedpolicy - SUCCESSFUL" );
  status = pthread_attr_getschedpolicy( &attr, &schedpolicy );
  posix_service_failed( status, "pthread_attr_getschedpolicy");
  printf( "Init: current scheduler policy attribute = %d\n", schedpolicy );

  /* exercise get and set stack size */

  empty_line();

  puts( "Init: pthread_attr_setstacksize - EINVAL (NULL attr)" );
  status = pthread_attr_setstacksize( NULL, 0 );
  fatal_directive_check_status_only( status, EINVAL, "NULL attr" );

  puts( "Init: pthread_attr_setstacksize - EINVAL (not initialized attr)" );
  status =
     pthread_attr_setstacksize( &destroyed_attr, 0 );
  fatal_directive_check_status_only( status, EINVAL, "not initialized attr" );

  puts( "Init: pthread_attr_setstacksize - SUCCESSFUL (low stacksize)" );
  status = pthread_attr_setstacksize( &attr, 0 );
  posix_service_failed( status, "pthread_attr_setstacksize");

  puts( "Init: pthread_attr_setstacksize - SUCCESSFUL (high stacksize)" );
  status = pthread_attr_setstacksize( &attr, STACK_MINIMUM_SIZE * 2 );
  posix_service_failed( status, "");

  puts( "Init: pthread_attr_getstacksize - EINVAL (NULL attr)" );
  status = pthread_attr_getstacksize( NULL, &stacksize );
  fatal_directive_check_status_only( status, EINVAL, "NULL attr" );

  puts( "Init: pthread_attr_getstacksize - EINVAL (NULL stacksize)" );
  status = pthread_attr_getstacksize( &attr, NULL );
  fatal_directive_check_status_only( status, EINVAL, "NULL stacksize" );

  puts( "Init: pthread_attr_getstacksize - EINVAL (not initialized attr)" );
  status = pthread_attr_getstacksize( &destroyed_attr, &stacksize );
  fatal_directive_check_status_only( status, EINVAL, "not initialized attr" );

  puts( "Init: pthread_attr_getstacksize - SUCCESSFUL" );
  status = pthread_attr_getstacksize( &attr, &stacksize );
  posix_service_failed( status, "pthread_attr_getstacksize");
  if ( stacksize == (STACK_MINIMUM_SIZE * 2) )
    printf( "Init: current stack size attribute is OK\n" );

  /* exercise get and set stack address */

  empty_line();

  puts( "Init: pthread_attr_setstackaddr - EINVAL (NULL attr)" );
  status = pthread_attr_setstackaddr( NULL, NULL );
  fatal_directive_check_status_only( status, EINVAL, "NULL attr" );

  puts( "Init: pthread_attr_setstackaddr - EINVAL (not initialized attr)" );
  status =
     pthread_attr_setstackaddr( &destroyed_attr, NULL );
  fatal_directive_check_status_only( status, EINVAL, "not initialized attr" );

  puts( "Init: pthread_attr_setstackaddr - SUCCESSFUL" );
  status = pthread_attr_setstackaddr( &attr, 0 );
  posix_service_failed( status, "");

  puts( "Init: pthread_attr_getstackaddr - EINVAL (NULL attr)" );
  status = pthread_attr_getstackaddr( NULL, &stackaddr );
  fatal_directive_check_status_only( status, EINVAL, "NULL attr" );

  puts( "Init: pthread_attr_getstackaddr - EINVAL (NULL stackaddr)" );
  status = pthread_attr_getstackaddr( &attr, NULL );
  fatal_directive_check_status_only( status, EINVAL, "NULL stackaddr" );

  puts( "Init: pthread_attr_getstackaddr - EINVAL (not initialized attr)" );
  status = pthread_attr_getstackaddr( &destroyed_attr, &stackaddr );
  fatal_directive_check_status_only( status, EINVAL, "not initialized attr" );

  puts( "Init: pthread_attr_getstackaddr - SUCCESSFUL" );
  status = pthread_attr_getstackaddr( &attr, &stackaddr );
  posix_service_failed( status, "pthread_attr_getstackaddr");
  printf( "Init: current stack address attribute = %p\n", stackaddr );

  /* exercise get and set detach state */

  empty_line();

  puts( "Init: pthread_attr_setdetachstate - EINVAL (NULL attr)" );
  status = pthread_attr_setdetachstate( NULL, PTHREAD_CREATE_DETACHED );
  fatal_directive_check_status_only( status, EINVAL, "NULL attr" );

  puts( "Init: pthread_attr_setdetachstate - EINVAL (not initialized attr)" );
  status =
     pthread_attr_setdetachstate( &destroyed_attr, PTHREAD_CREATE_JOINABLE );
  fatal_directive_check_status_only( status, EINVAL, "not initialized att" );

  puts( "Init: pthread_attr_setdetachstate - EINVAL (invalid detachstate)" );
  status = pthread_attr_setdetachstate( &attr, -1 );
  fatal_directive_check_status_only( status, EINVAL, "invalid detachstate" );

  puts( "Init: pthread_attr_setdetachstate - SUCCESSFUL" );
  status = pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_JOINABLE );
  posix_service_failed( status, "pthread_attr_setdetachstate");

  puts( "Init: pthread_attr_getdetachstate - EINVAL (NULL attr)" );
  status = pthread_attr_getdetachstate( NULL, &detachstate );
  fatal_directive_check_status_only( status, EINVAL, "NULL attr" );

  puts( "Init: pthread_attr_getdetachstate - EINVAL (NULL detatchstate)" );
  status = pthread_attr_getdetachstate( &attr, NULL );
  fatal_directive_check_status_only( status, EINVAL, "NULL detatchstate" );

  puts( "Init: pthread_attr_getdetachstate - EINVAL (not initialized attr)" );
  status = pthread_attr_getdetachstate( &destroyed_attr, &detachstate );
  fatal_directive_check_status_only( status, EINVAL, "not initialized attr" );

  puts( "Init: pthread_attr_getdetachstate - SUCCESSFUL" );
  status = pthread_attr_getdetachstate( &attr, &detachstate );
  posix_service_failed( status, "pthread_attr_getdetachstate");
  printf( "Init: current detach state attribute = %d\n", detachstate );

  /* exercise get and set scheduling parameters */

  empty_line();

  puts( "Init: pthread_attr_getschedparam - SUCCESSFUL" );
  status = pthread_attr_getschedparam( &attr, &schedparam );
  posix_service_failed( status, "pthread_attr_getschedparam");

  print_schedparam( "Init: ", &schedparam );

  puts( "Init: pthread_attr_setschedparam - EINVAL (NULL attr)" );
  status = pthread_attr_setschedparam( NULL, &schedparam );
  fatal_directive_check_status_only( status, EINVAL, "NULL attr" );

  puts( "Init: pthread_attr_setschedparam - EINVAL (not initialized attr)" );
  status = pthread_attr_setschedparam( &destroyed_attr, &schedparam );
  fatal_directive_check_status_only( status, EINVAL, "not initialized attr" );

  puts( "Init: pthread_attr_setschedparam - EINVAL (NULL schedparam)" );
  status = pthread_attr_setschedparam( &attr, NULL );
  fatal_directive_check_status_only( status, EINVAL, "NULL schedparam" );

  puts( "Init: pthread_attr_setschedparam - SUCCESSFUL" );
  status = pthread_attr_setschedparam( &attr, &schedparam );
  posix_service_failed( status, "pthread_attr_setschedparam");

  puts( "Init: pthread_attr_getschedparam - EINVAL (NULL attr)" );
  status = pthread_attr_getschedparam( NULL, &schedparam );
  fatal_directive_check_status_only( status, EINVAL, "pthread_attr_getschedparam" );

  puts( "Init: pthread_attr_getschedparam - EINVAL (not initialized attr)" );
  status = pthread_attr_getschedparam( &destroyed_attr, &schedparam );
  fatal_directive_check_status_only( status, EINVAL, "not initialized attr" );

  puts( "Init: pthread_attr_getschedparam - EINVAL (NULL schedparam)" );
  status = pthread_attr_getschedparam( &attr, NULL );
  fatal_directive_check_status_only( status, EINVAL, "NULL schedparam" );

  /* exercise pthread_getschedparam */

  empty_line();

  puts( "Init: pthread_getschedparam - EINVAL (NULL policy)" );
  status = pthread_getschedparam( pthread_self(), NULL, &schedparam );
  fatal_directive_check_status_only( status, EINVAL, "NULL policy" );

  puts( "Init: pthread_getschedparam - EINVAL (NULL schedparam)" );
  status = pthread_getschedparam( pthread_self(), &schedpolicy, NULL );
  fatal_directive_check_status_only( status, EINVAL, "NULL schedparam" );

  puts( "Init: pthread_getschedparam - ESRCH (bad thread)" );
  status = pthread_getschedparam( -1, &schedpolicy, &schedparam );
  fatal_directive_check_status_only( status, ESRCH, "bad thread" );

  puts( "Init: pthread_getschedparam - SUCCESSFUL" );
  status = pthread_getschedparam( pthread_self(), &schedpolicy, &schedparam );
  posix_service_failed( status, "pthread_getschedparam");

  printf( "Init: policy = %d\n", schedpolicy );

  print_schedparam( "Init: ", &schedparam );

  /* exercise pthread_setschedparam */

  empty_line();

  puts( "Init: pthread_setschedparam - EINVAL (NULL schedparam)" );
  status = pthread_setschedparam( pthread_self(), SCHED_OTHER, NULL );
  fatal_directive_check_status_only( status, EINVAL, "NULL schedparam" );

  schedparam.sched_priority = -1;

  puts( "Init: pthread_setschedparam - EINVAL (invalid priority)" );
  status = pthread_setschedparam( pthread_self(), SCHED_OTHER, NULL );
  fatal_directive_check_status_only( status, EINVAL, "invalid priority" );

  schedparam.sched_priority = sched_get_priority_max(SCHED_OTHER);

  puts( "Init: pthread_setschedparam - EINVAL (invalid policy)" );
  status = pthread_setschedparam( pthread_self(), -1, &schedparam );
  fatal_directive_check_status_only( status, EINVAL, "invalid policy" );

  puts( "Init: pthread_setschedparam - ESRCH (invalid thread)" );
  status = pthread_setschedparam( -1, SCHED_OTHER, &schedparam );
  fatal_directive_check_status_only( status, ESRCH, "invalid thread" );

  /* now get sporadic server errors */

  schedparam.ss_replenish_period.tv_sec = 1;
  schedparam.ss_replenish_period.tv_nsec = 0;
  schedparam.ss_initial_budget.tv_sec = 1;
  schedparam.ss_initial_budget.tv_nsec = 1;

  puts( "Init: pthread_setschedparam - EINVAL (replenish < budget)" );
  status = pthread_setschedparam( pthread_self(), SCHED_SPORADIC, &schedparam );
  fatal_directive_check_status_only( status, EINVAL, "replenish < budget" );

  schedparam.ss_replenish_period.tv_sec = 2;
  schedparam.ss_replenish_period.tv_nsec = 0;
  schedparam.ss_initial_budget.tv_sec = 1;
  schedparam.ss_initial_budget.tv_nsec = 0;
  schedparam.ss_low_priority = -1;

  puts( "Init: pthread_setschedparam - EINVAL (invalid priority)" );
  status = pthread_setschedparam( pthread_self(), SCHED_SPORADIC, &schedparam );
  fatal_directive_check_status_only( status, EINVAL, "invalid priority" );

  puts( "*** END OF POSIX TEST 7 ***" );
  rtems_test_exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}
