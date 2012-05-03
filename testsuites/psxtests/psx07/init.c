/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <sched.h>

#define CONFIGURE_INIT
#include "system.h"
#include <errno.h>
#include "tmacros.h"
#include "pritime.h"

void print_schedparam(
  char               *prefix,
  struct sched_param *schedparam
);

void print_schedparam(
  char               *prefix,
  struct sched_param *schedparam
)
{
  printf( "%ssched priority      = %d\n", prefix, schedparam->sched_priority );
#if defined(_POSIX_SPORADIC_SERVER)
  printf( "%ssched_ss_low_priority     = %d\n",
     prefix, schedparam->sched_ss_low_priority );
  printf( "%ssched_ss_replenish_period = (%" PRIdtime_t ", %ld)\n", prefix,
     schedparam->sched_ss_repl_period.tv_sec,
     schedparam->sched_ss_repl_period.tv_nsec );
  printf( "%ssched_sched_ss_initial_budget = (%" PRIdtime_t ", %ld)\n", prefix,
     schedparam->sched_ss_init_budget.tv_sec,
     schedparam->sched_ss_init_budget.tv_nsec );
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
#if HAVE_DECL_PTHREAD_ATTR_SETGUARDSIZE
  size_t              guardsize;
#endif
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
  printf( "Init's ID is 0x%08" PRIxpthread_t "\n", Init_id );

  /* exercise init and destroy */

  puts( "Init - pthread_attr_init - EINVAL (NULL attr)" );
  status = pthread_attr_init( NULL );
  fatal_directive_check_status_only( status, EINVAL, "null attribute" );

  puts( "Init - pthread_attr_init - SUCCESSFUL" );
  status = pthread_attr_init( &attr );
  posix_service_failed( status, "pthread_attr_init" );

  puts( "Init - initialize and destroy an attribute - SUCCESSFUL" );
  status = pthread_attr_init( &destroyed_attr );
  posix_service_failed( status, "pthread_attr_init");

  status = pthread_attr_destroy( &destroyed_attr );
  posix_service_failed( status, "pthread_attr_destroy");

  puts( "Init - pthread_attr_destroy - EINVAL (NULL attr)" );
  status = pthread_attr_destroy( NULL );
  fatal_directive_check_status_only( status, EINVAL, "NULL attribute" );

  puts( "Init - pthread_attr_destroy - EINVAL (not initialized)" );
  status = pthread_attr_destroy( &destroyed_attr );
  fatal_directive_check_status_only( status, EINVAL, "not initialized" );

  /* check some errors in pthread_create */

  puts( "Init - pthread_create - EINVAL (attr not initialized)" );
  status = pthread_create( &Task_id, &destroyed_attr, Task_1, NULL );
  fatal_directive_check_status_only( status, EINVAL, "attribute not initialized" );

  /* junk stack address */
  status = pthread_attr_setstackaddr( &attr, (void *)&schedparam );
  posix_service_failed( status, "setstackaddr");

  /* must go around pthread_attr_setstacksize to set a bad stack size */
  attr.stacksize = 0;

  puts( "Init - pthread_create - EINVAL (stacksize too small)" );
  status = pthread_create( &Task_id, &attr, Task_1, NULL );
  fatal_directive_check_status_only( status, EINVAL, "stacksize too small" );

  /* reset all the fields */
  status = pthread_attr_init( &attr );
  posix_service_failed( status, "pthread_attr_init");

#if HAVE_DECL_PTHREAD_ATTR_SETSTACKADDR
  attr.stacksize = rtems_configuration_get_work_space_size() * 10;
  puts( "Init - pthread_create - EAGAIN (stacksize too large)" );
  status = pthread_create( &Task_id, &attr, Task_1, NULL );
  fatal_directive_check_status_only( status, EAGAIN, "stacksize too large" );
#endif

  status = pthread_attr_init( &attr );
  posix_service_failed( status, "pthread_attr_init");

  /* must go around pthread_attr_set routines to set a bad value */
  attr.inheritsched = -1;

  puts( "Init - pthread_create - EINVAL (invalid inherit scheduler)" );
  status = pthread_create( &Task_id, &attr, Task_1, NULL );
  fatal_directive_check_status_only( status, EINVAL, "invalid inherit scheduler" );

  /* check out the error case for system scope not supported */

  status = pthread_attr_init( &attr );
  posix_service_failed( status, " pthread_attr_init");

  /* must go around pthread_attr_set routines to set a bad value */
  attr.contentionscope = PTHREAD_SCOPE_SYSTEM;

  puts( "Init - pthread_create - ENOTSUP (unsupported system contention scope)" );
  status = pthread_create( &Task_id, &attr, Task_1, NULL );
  fatal_directive_check_status_only( status, ENOTSUP,
    "unsupported system contention scope" );

  status = pthread_attr_init( &attr );
  posix_service_failed( status, "pthread_attr_init");

  /* now check out pthread_create for inherit scheduler */

  status = pthread_attr_setinheritsched( &attr, PTHREAD_INHERIT_SCHED );
  posix_service_failed( status, "pthread_attr_setinheritsched");

  puts( "Init - pthread_create - SUCCESSFUL (inherit scheduler)" );
  status = pthread_create( &Task_id, &attr, Task_1, NULL );
  posix_service_failed( status, "pthread_create");

  status = pthread_join( Task_id, NULL );
  posix_service_failed( status, " pthread_join");

    /* switch to Task_1 */

  /* exercise get and set scope */

  empty_line();

  status = pthread_attr_init( &attr );
  posix_service_failed( status, "pthread_attr_init");

  puts( "Init - pthread_attr_setscope - EINVAL (NULL attr)" );
  status = pthread_attr_setscope( NULL, PTHREAD_SCOPE_PROCESS );
  fatal_directive_check_status_only( status, EINVAL , "NULL attr" );

  puts( "Init - pthread_attr_setscope - ENOTSUP" );
  status = pthread_attr_setscope( &attr, PTHREAD_SCOPE_SYSTEM );
  fatal_directive_check_status_only( status, ENOTSUP, "PTHREAD_SCOPE_SYSTEM" );

  puts( "Init - pthread_attr_setscope - EINVAL (not initialized attr)" );
  status = pthread_attr_setscope( &destroyed_attr, PTHREAD_SCOPE_PROCESS );
  fatal_directive_check_status_only( status, EINVAL, "not initialized attr" );

  puts( "Init - pthread_attr_setscope - EINVAL (invalid scope)" );
  status = pthread_attr_setscope( &attr, -1 );
  fatal_directive_check_status_only( status, EINVAL, "invalid scope" );

  puts( "Init - pthread_attr_setscope - SUCCESSFUL" );
  status = pthread_attr_setscope( &attr, PTHREAD_SCOPE_PROCESS );
  posix_service_failed( status, "pthread_attr_setscope");

  puts( "Init - pthread_attr_getscope - EINVAL (NULL attr)" );
  status = pthread_attr_getscope( NULL, &scope );
  fatal_directive_check_status_only( status, EINVAL, "NULL attr" );

  puts( "Init - pthread_attr_getscope - EINVAL (NULL scope)" );
  status = pthread_attr_getscope( &attr, NULL );
  fatal_directive_check_status_only( status, EINVAL, "NULL scope" );

  puts( "Init - pthread_attr_getscope - EINVAL (not initialized attr)" );
  status = pthread_attr_getscope( &destroyed_attr, &scope );
  fatal_directive_check_status_only( status, EINVAL, "not initialized attr" );

  puts( "Init - pthread_attr_getscope - SUCCESSFUL" );
  status = pthread_attr_getscope( &attr, &scope );
  posix_service_failed( status, "pthread_attr_getscope");
  printf( "Init - current scope attribute = %d\n", scope );

  /* exercise get and set inherit scheduler */

  empty_line();

  puts( "Init - pthread_attr_setinheritsched - EINVAL (NULL attr)" );
  status = pthread_attr_setinheritsched( NULL, PTHREAD_INHERIT_SCHED );
  fatal_directive_check_status_only( status, EINVAL, "NULL attr" );

  puts( "Init - pthread_attr_setinheritsched - EINVAL (not initialized attr)" );
  status =
     pthread_attr_setinheritsched( &destroyed_attr, PTHREAD_INHERIT_SCHED );
  fatal_directive_check_status_only( status, EINVAL, "not initialized attr" );

  puts( "Init - pthread_attr_setinheritsched - ENOTSUP (invalid inheritsched)" );
  status = pthread_attr_setinheritsched( &attr, -1 );
  fatal_directive_check_status_only( status, ENOTSUP, "invalid inheritsched" );

  puts( "Init - pthread_attr_setinheritsched - SUCCESSFUL" );
  status = pthread_attr_setinheritsched( &attr, PTHREAD_INHERIT_SCHED );
  posix_service_failed( status, "pthread_attr_setinheritsched");

  puts( "Init - pthread_attr_getinheritsched - EINVAL (NULL attr)" );
  status = pthread_attr_getinheritsched( NULL, &inheritsched );
  fatal_directive_check_status_only( status, EINVAL, "NULL attr" );

  puts( "Init - pthread_attr_getinheritsched - EINVAL (NULL inheritsched)" );
  status = pthread_attr_getinheritsched( &attr, NULL );
  fatal_directive_check_status_only( status, EINVAL, "NULL inheritsched" );

  puts( "Init - pthread_attr_getinheritsched - EINVAL (not initialized attr)" );
  status = pthread_attr_getinheritsched( &destroyed_attr, &inheritsched );
  fatal_directive_check_status_only( status, EINVAL, "not initialized attr" );

  puts( "Init - pthread_attr_getinheritsched - SUCCESSFUL" );
  status = pthread_attr_getinheritsched( &attr, &inheritsched );
  posix_service_failed( status, "pthread_attr_getinheritsched");
  printf( "Init - current inherit scheduler attribute = %d\n", inheritsched );

  /* exercise get and set inherit scheduler */

  empty_line();

  puts( "Init - pthread_attr_setschedpolicy - EINVAL (NULL attr)" );
  status = pthread_attr_setschedpolicy( NULL, SCHED_FIFO );
  fatal_directive_check_status_only( status, EINVAL, "NULL attr" );

  puts( "Init - pthread_attr_setschedpolicy - EINVAL (not initialized attr)" );
  status =
     pthread_attr_setschedpolicy( &destroyed_attr, SCHED_OTHER );
  fatal_directive_check_status_only( status, EINVAL, "not initialized attr" );

  puts( "Init - pthread_attr_setschedpolicy - ENOTSUP (invalid schedpolicy)" );
  status = pthread_attr_setschedpolicy( &attr, -1 );
  fatal_directive_check_status_only( status, ENOTSUP, "invalid schedpolicy" );

  puts( "Init - pthread_attr_setschedpolicy - SUCCESSFUL" );
  status = pthread_attr_setschedpolicy( &attr, SCHED_RR );
  posix_service_failed( status, "pthread_attr_setschedpolicy");

  puts( "Init - pthread_attr_getschedpolicy - EINVAL (NULL attr)" );
  status = pthread_attr_getschedpolicy( NULL, &schedpolicy );
  fatal_directive_check_status_only( status, EINVAL, "NULL attr" );

  puts( "Init - pthread_attr_getschedpolicy - EINVAL (NULL schedpolicy)" );
  status = pthread_attr_getschedpolicy( &attr, NULL );
  fatal_directive_check_status_only( status, EINVAL, "NULL schedpolicy" );

  puts( "Init - pthread_attr_getschedpolicy - EINVAL (not initialized attr)" );
  status = pthread_attr_getschedpolicy( &destroyed_attr, &schedpolicy );
  fatal_directive_check_status_only( status, EINVAL, "not initialized attr" );

  puts( "Init - pthread_attr_getschedpolicy - SUCCESSFUL" );
  status = pthread_attr_getschedpolicy( &attr, &schedpolicy );
  posix_service_failed( status, "pthread_attr_getschedpolicy");
  printf( "Init - current scheduler policy attribute = %d\n", schedpolicy );

  /* exercise get and set stack size */

  empty_line();

  puts( "Init - pthread_attr_setstacksize - EINVAL (NULL attr)" );
  status = pthread_attr_setstacksize( NULL, 0 );
  fatal_directive_check_status_only( status, EINVAL, "NULL attr" );

  puts( "Init - pthread_attr_setstacksize - EINVAL (not initialized attr)" );
  status =
     pthread_attr_setstacksize( &destroyed_attr, 0 );
  fatal_directive_check_status_only( status, EINVAL, "not initialized attr" );

  puts( "Init - pthread_attr_setstacksize - SUCCESSFUL (low stacksize)" );
  status = pthread_attr_setstacksize( &attr, 0 );
  posix_service_failed( status, "pthread_attr_setstacksize");

  puts( "Init - pthread_attr_setstacksize - SUCCESSFUL (high stacksize)" );
  status = pthread_attr_setstacksize( &attr, STACK_MINIMUM_SIZE * 2 );
  posix_service_failed( status, "");

  puts( "Init - pthread_attr_getstacksize - EINVAL (NULL attr)" );
  status = pthread_attr_getstacksize( NULL, &stacksize );
  fatal_directive_check_status_only( status, EINVAL, "NULL attr" );

  puts( "Init - pthread_attr_getstacksize - EINVAL (NULL stacksize)" );
  status = pthread_attr_getstacksize( &attr, NULL );
  fatal_directive_check_status_only( status, EINVAL, "NULL stacksize" );

  puts( "Init - pthread_attr_getstacksize - EINVAL (not initialized attr)" );
  status = pthread_attr_getstacksize( &destroyed_attr, &stacksize );
  fatal_directive_check_status_only( status, EINVAL, "not initialized attr" );

  puts( "Init - pthread_attr_getstacksize - SUCCESSFUL" );
  status = pthread_attr_getstacksize( &attr, &stacksize );
  posix_service_failed( status, "pthread_attr_getstacksize");
  if ( stacksize == (STACK_MINIMUM_SIZE * 2) )
    printf( "Init - current stack size attribute is OK\n" );

  /* exercise get and set stack address */
  empty_line();

  puts( "Init - pthread_attr_setstackaddr - EINVAL (NULL attr)" );
  status = pthread_attr_setstackaddr( NULL, NULL );
  fatal_directive_check_status_only( status, EINVAL, "NULL attr" );

  puts( "Init - pthread_attr_setstackaddr - EINVAL (not initialized attr)" );
  status = pthread_attr_setstackaddr( &destroyed_attr, NULL );
  fatal_directive_check_status_only( status, EINVAL, "not initialized attr" );

  puts( "Init - pthread_attr_setstackaddr - SUCCESSFUL" );
  status = pthread_attr_setstackaddr( &attr, 0 );
  posix_service_failed( status, "");

  /* get stack addr */
  puts( "Init - pthread_attr_getstackaddr - EINVAL (NULL attr)" );
  status = pthread_attr_getstackaddr( NULL, &stackaddr );
  fatal_directive_check_status_only( status, EINVAL, "NULL attr" );

  puts( "Init - pthread_attr_getstackaddr - EINVAL (NULL stackaddr)" );
  status = pthread_attr_getstackaddr( &attr, NULL );
  fatal_directive_check_status_only( status, EINVAL, "NULL stackaddr" );

  puts( "Init - pthread_attr_getstackaddr - EINVAL (not initialized attr)" );
  status = pthread_attr_getstackaddr( &destroyed_attr, &stackaddr );
  fatal_directive_check_status_only( status, EINVAL, "not initialized attr" );

  puts( "Init - pthread_attr_getstackaddr - SUCCESSFUL" );
  status = pthread_attr_getstackaddr( &attr, &stackaddr );
  posix_service_failed( status, "pthread_attr_getstackaddr");
  printf( "Init - current stack address attribute = %p\n", stackaddr );

  /* exercise get and set stack (as pair) */
  empty_line();

#if HAVE_DECL_PTHREAD_ATTR_SETSTACK
  puts( "Init - pthread_attr_setstack- EINVAL (NULL attr)" );
  status = pthread_attr_setstack( NULL, &stackaddr, 1024 );
  fatal_directive_check_status_only( status, EINVAL, "NULL attr" );

  puts( "Init - pthread_attr_setstack- EINVAL (destroyed attr)" );
  status = pthread_attr_setstack( &destroyed_attr, &stackaddr, 1024 );
  fatal_directive_check_status_only( status, EINVAL, "NULL attr" );

  puts( "Init - pthread_attr_setstack- SUCCESSFUL (< min stack)" );
  status = pthread_attr_setstack( &attr, stackaddr, 0 );
  posix_service_failed( status, "OK");

  puts( "Init - pthread_attr_setstack- SUCCESSFUL (big stack)" );
  status = pthread_attr_setstack( &attr, stackaddr, STACK_MINIMUM_SIZE * 2 );
  posix_service_failed( status, "OK");
#endif

#if HAVE_DECL_PTHREAD_ATTR_GETSTACK
  puts( "Init - pthread_attr_getstack- EINVAL (NULL attr)" );
  status = pthread_attr_getstack( NULL, &stackaddr, &stacksize );
  fatal_directive_check_status_only( status, EINVAL, "NULL attr" );

  puts( "Init - pthread_attr_getstack- EINVAL (destroyed attr)" );
  status = pthread_attr_getstack( &destroyed_attr, &stackaddr, &stacksize );
  fatal_directive_check_status_only( status, EINVAL, "&destroyed attr" );

  puts( "Init - pthread_attr_getstack- EINVAL (NULL stack)" );
  status = pthread_attr_getstack( &attr, NULL, &stacksize );
  fatal_directive_check_status_only( status, EINVAL, "&NULL stack" );

  puts( "Init - pthread_attr_getstack- EINVAL (NULL stacksize)" );
  status = pthread_attr_getstack( &attr, &stackaddr, NULL );
  fatal_directive_check_status_only( status, EINVAL, "&NULL size" );

  puts( "Init - pthread_attr_getstack- SUCCESSFUL" );
  status = pthread_attr_getstack( &attr, &stackaddr, &stacksize );
  posix_service_failed( status, "pthread_attr_getstack");
#endif

  /* exercise get and set detach state */
  empty_line();

#if HAVE_DECL_PTHREAD_ATTR_SETGUARDSIZE
  puts( "Init - pthread_attr_setguardsize - EINVAL (NULL attr)" );
  status = pthread_attr_setguardsize( NULL, 0 );
  fatal_directive_check_status_only( status, EINVAL, "NULL attr" );

  puts( "Init - pthread_attr_setguardsize - EINVAL (not initialized attr)" );
  status = pthread_attr_setguardsize( &destroyed_attr, 0 );
  fatal_directive_check_status_only( status, EINVAL, "not initialized attr" );

  puts( "Init - pthread_attr_setguardsize - SUCCESSFUL (low guardsize)" );
  status = pthread_attr_setguardsize( &attr, 0 );
  posix_service_failed( status, "pthread_attr_setguardsize");

  puts( "Init - pthread_attr_setguardsize - SUCCESSFUL (high guardsize)" );
  status = pthread_attr_setguardsize( &attr, STACK_MINIMUM_SIZE * 2 );
  posix_service_failed( status, "");
#endif

#if HAVE_DECL_PTHREAD_ATTR_GETGUARDSIZE
  puts( "Init - pthread_attr_getguardsize - EINVAL (NULL attr)" );
  status = pthread_attr_getguardsize( NULL, &guardsize );
  fatal_directive_check_status_only( status, EINVAL, "NULL attr" );

  puts( "Init - pthread_attr_getguardsize - EINVAL (NULL guardsize)" );
  status = pthread_attr_getguardsize( &attr, NULL );
  fatal_directive_check_status_only( status, EINVAL, "NULL guardsize" );

  puts( "Init - pthread_attr_getguardsize - EINVAL (not initialized attr)" );
  status = pthread_attr_getguardsize( &destroyed_attr, &guardsize );
  fatal_directive_check_status_only( status, EINVAL, "not initialized attr" );

  puts( "Init - pthread_attr_getguardsize - SUCCESSFUL" );
  status = pthread_attr_getguardsize( &attr, &guardsize );
  posix_service_failed( status, "pthread_attr_getguardsize");
#endif

  /* exercise get and set detach state */
  empty_line();

  puts( "Init - pthread_attr_setdetachstate - EINVAL (NULL attr)" );
  status = pthread_attr_setdetachstate( NULL, PTHREAD_CREATE_DETACHED );
  fatal_directive_check_status_only( status, EINVAL, "NULL attr" );

  puts( "Init - pthread_attr_setdetachstate - EINVAL (not initialized attr)" );
  status =
     pthread_attr_setdetachstate( &destroyed_attr, PTHREAD_CREATE_JOINABLE );
  fatal_directive_check_status_only( status, EINVAL, "not initialized att" );

  puts( "Init - pthread_attr_setdetachstate - EINVAL (invalid detachstate)" );
  status = pthread_attr_setdetachstate( &attr, -1 );
  fatal_directive_check_status_only( status, EINVAL, "invalid detachstate" );

  puts( "Init - pthread_attr_setdetachstate - SUCCESSFUL" );
  status = pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_JOINABLE );
  posix_service_failed( status, "pthread_attr_setdetachstate");

  puts( "Init - pthread_attr_getdetachstate - EINVAL (NULL attr)" );
  status = pthread_attr_getdetachstate( NULL, &detachstate );
  fatal_directive_check_status_only( status, EINVAL, "NULL attr" );

  puts( "Init - pthread_attr_getdetachstate - EINVAL (NULL detatchstate)" );
  status = pthread_attr_getdetachstate( &attr, NULL );
  fatal_directive_check_status_only( status, EINVAL, "NULL detatchstate" );

  puts( "Init - pthread_attr_getdetachstate - EINVAL (not initialized attr)" );
  status = pthread_attr_getdetachstate( &destroyed_attr, &detachstate );
  fatal_directive_check_status_only( status, EINVAL, "not initialized attr" );

  puts( "Init - pthread_attr_getdetachstate - SUCCESSFUL" );
  status = pthread_attr_getdetachstate( &attr, &detachstate );
  posix_service_failed( status, "pthread_attr_getdetachstate");
  printf( "Init - current detach state attribute = %d\n", detachstate );

  /* exercise get and set scheduling parameters */

  empty_line();

  puts( "Init - pthread_attr_getschedparam - SUCCESSFUL" );
  status = pthread_attr_getschedparam( &attr, &schedparam );
  posix_service_failed( status, "pthread_attr_getschedparam");

  print_schedparam( "Init - ", &schedparam );

  puts( "Init - pthread_attr_setschedparam - EINVAL (NULL attr)" );
  status = pthread_attr_setschedparam( NULL, &schedparam );
  fatal_directive_check_status_only( status, EINVAL, "NULL attr" );

  puts( "Init - pthread_attr_setschedparam - EINVAL (not initialized attr)" );
  status = pthread_attr_setschedparam( &destroyed_attr, &schedparam );
  fatal_directive_check_status_only( status, EINVAL, "not initialized attr" );

  puts( "Init - pthread_attr_setschedparam - EINVAL (NULL schedparam)" );
  status = pthread_attr_setschedparam( &attr, NULL );
  fatal_directive_check_status_only( status, EINVAL, "NULL schedparam" );

  puts( "Init - pthread_attr_setschedparam - SUCCESSFUL" );
  status = pthread_attr_setschedparam( &attr, &schedparam );
  posix_service_failed( status, "pthread_attr_setschedparam");

  puts( "Init - pthread_attr_getschedparam - EINVAL (NULL attr)" );
  status = pthread_attr_getschedparam( NULL, &schedparam );
  fatal_directive_check_status_only( status, EINVAL, "pthread_attr_getschedparam" );

  puts( "Init - pthread_attr_getschedparam - EINVAL (not initialized attr)" );
  status = pthread_attr_getschedparam( &destroyed_attr, &schedparam );
  fatal_directive_check_status_only( status, EINVAL, "not initialized attr" );

  puts( "Init - pthread_attr_getschedparam - EINVAL (NULL schedparam)" );
  status = pthread_attr_getschedparam( &attr, NULL );
  fatal_directive_check_status_only( status, EINVAL, "NULL schedparam" );

  /* exercise pthread_getschedparam */

  empty_line();

  puts( "Init - pthread_getschedparam - EINVAL (NULL policy)" );
  status = pthread_getschedparam( pthread_self(), NULL, &schedparam );
  fatal_directive_check_status_only( status, EINVAL, "NULL policy" );

  puts( "Init - pthread_getschedparam - EINVAL (NULL schedparam)" );
  status = pthread_getschedparam( pthread_self(), &schedpolicy, NULL );
  fatal_directive_check_status_only( status, EINVAL, "NULL schedparam" );

  puts( "Init - pthread_getschedparam - ESRCH (bad thread)" );
  status = pthread_getschedparam( (pthread_t) -1, &schedpolicy, &schedparam );
  fatal_directive_check_status_only( status, ESRCH, "bad thread" );

  puts( "Init - pthread_getschedparam - SUCCESSFUL" );
  status = pthread_getschedparam( pthread_self(), &schedpolicy, &schedparam );
  posix_service_failed( status, "pthread_getschedparam");

  printf( "Init - policy = %d\n", schedpolicy );

  print_schedparam( "Init - ", &schedparam );

  /* exercise pthread_setschedparam */

  empty_line();

  puts( "Init - pthread_setschedparam - EINVAL (NULL schedparam)" );
  status = pthread_setschedparam( pthread_self(), SCHED_OTHER, NULL );
  fatal_directive_check_status_only( status, EINVAL, "NULL schedparam" );

  schedparam.sched_priority = -1;

  puts( "Init - pthread_setschedparam - EINVAL (invalid priority)" );
  status = pthread_setschedparam( pthread_self(), SCHED_OTHER, NULL );
  fatal_directive_check_status_only( status, EINVAL, "invalid priority" );

  /* reset sched_param */
  status = pthread_getschedparam( pthread_self(), &schedpolicy, &schedparam );
  posix_service_failed( status, "pthread_getschedparam");

  puts( "Init - pthread_setschedparam - EINVAL (invalid policy)" );
  status = pthread_setschedparam( pthread_self(), -1, &schedparam );
  fatal_directive_check_status_only( status, EINVAL, "invalid policy" );

  puts( "Init - pthread_setschedparam - ESRCH (invalid thread)" );
  status = pthread_setschedparam( (pthread_t) -1, SCHED_OTHER, &schedparam );
  fatal_directive_check_status_only( status, ESRCH, "invalid thread" );

  /* now get sporadic server errors */

  schedparam.sched_ss_repl_period.tv_sec = 0;
  schedparam.sched_ss_repl_period.tv_nsec = 0;
  schedparam.sched_ss_init_budget.tv_sec = 1;
  schedparam.sched_ss_init_budget.tv_nsec = 1;

  puts( "Init - pthread_setschedparam - EINVAL (replenish == 0)" );
  status = pthread_setschedparam( pthread_self(), SCHED_SPORADIC, &schedparam );
  fatal_directive_check_status_only( status, EINVAL, "replenish == 0" );

  schedparam.sched_ss_repl_period.tv_sec = 1;
  schedparam.sched_ss_repl_period.tv_nsec = 1;
  schedparam.sched_ss_init_budget.tv_sec = 0;
  schedparam.sched_ss_init_budget.tv_nsec = 0;

  puts( "Init - pthread_setschedparam - EINVAL (budget == 0)" );
  status = pthread_setschedparam( pthread_self(), SCHED_SPORADIC, &schedparam );
  fatal_directive_check_status_only( status, EINVAL, "budget == 0" );

  schedparam.sched_ss_repl_period.tv_sec = 1;
  schedparam.sched_ss_repl_period.tv_nsec = 0;
  schedparam.sched_ss_init_budget.tv_sec = 1;
  schedparam.sched_ss_init_budget.tv_nsec = 1;

  puts( "Init - pthread_setschedparam - EINVAL (replenish < budget)" );
  status = pthread_setschedparam( pthread_self(), SCHED_SPORADIC, &schedparam );
  fatal_directive_check_status_only( status, EINVAL, "replenish < budget" );

  schedparam.sched_ss_repl_period.tv_sec = 2;
  schedparam.sched_ss_repl_period.tv_nsec = 0;
  schedparam.sched_ss_init_budget.tv_sec = 1;
  schedparam.sched_ss_init_budget.tv_nsec = 0;
  schedparam.sched_ss_low_priority = -1;

  puts( "Init - pthread_setschedparam - EINVAL (invalid priority)" );
  status = pthread_setschedparam( pthread_self(), SCHED_SPORADIC, &schedparam );
  fatal_directive_check_status_only( status, EINVAL, "invalid priority" );

  /*
   *  Create a sporadic thread that doesn't need it's priority
   *  boosted
   */
  empty_line();

  puts( "Init - pthread_attr_init - SUCCESSFUL" );
  status = pthread_attr_init( &attr );
  posix_service_failed( status, "pthread_attr_init" );

  puts( "Init - pthread_attr_setinheritsched - EXPLICIT - SUCCESSFUL" );
  status = pthread_attr_setinheritsched( &attr, PTHREAD_EXPLICIT_SCHED );
  rtems_test_assert( !status );

  schedparam.sched_ss_repl_period.tv_sec = 3;
  schedparam.sched_ss_repl_period.tv_nsec = 3;
  schedparam.sched_ss_init_budget.tv_sec = 1;
  schedparam.sched_ss_init_budget.tv_nsec = 1;
  schedparam.sched_priority = sched_get_priority_max( SCHED_FIFO );
  schedparam.sched_ss_low_priority = sched_get_priority_max( SCHED_FIFO ) - 6;

  puts( "Init - pthread_attr_setschedpolicy - SUCCESSFUL" );
  status = pthread_attr_setschedpolicy( &attr, SCHED_SPORADIC );
  posix_service_failed( status, "pthread_attr_setschedparam");
  puts( "Init - pthread_attr_setschedparam - SUCCESSFUL" );
  status = pthread_attr_setschedparam( &attr, &schedparam );
  posix_service_failed( status, "pthread_attr_setschedparam");

  status = pthread_create( &Task2_id, &attr, Task_2, NULL );
  rtems_test_assert( !status );

  status = pthread_join( Task2_id, NULL );
  posix_service_failed( status, " pthread_join");

  puts( "*** END OF POSIX TEST 7 ***" );
  rtems_test_exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}
