/*
 *  COPYRIGHT (c) 1989-2007.
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

#include <aio.h>
#include <sys/types.h>
#include <time.h>
#include <devctl.h>
#include <unistd.h>
#include <sched.h>

void check_enosys(int status)
{
  if ( (status == -1) && (errno == ENOSYS) ) 
    return;
  puts( "ERROR -- did not return ENOSYS as expected" );
  rtems_test_exit(0);
}

void *POSIX_Init(
  void *argument
)
{
  int             sc;

  puts( "\n\n*** POSIX TEST -- ENOSYS ***" );

  puts( "aio_read -- ENOSYS" );
  sc = aio_read( NULL );
  check_enosys( sc );

  puts( "aio_write -- ENOSYS" );
  sc = aio_write( NULL );
  check_enosys( sc );

  puts( "lio_listio -- ENOSYS" );
  sc = lio_listio( 0, NULL, 0, NULL );
  check_enosys( sc );

  puts( "aio_error -- ENOSYS" );
  sc = aio_error( NULL );
  check_enosys( sc );

  puts( "aio_return -- ENOSYS" );
  sc = aio_return( NULL );
  check_enosys( sc );

  puts( "aio_cancel -- ENOSYS" );
  sc = aio_cancel( 0, NULL );
  check_enosys( sc );

  puts( "aio_suspend -- ENOSYS" );
  sc = aio_suspend( NULL, 0, NULL );
  check_enosys( sc );

  puts( "aio_fsync -- ENOSYS" );
  sc = aio_fsync( 0, NULL );
  check_enosys( sc );

  puts( "clock_getcpuclockid -- ENOSYS" );
  sc = clock_getcpuclockid( 0, NULL );
  check_enosys( sc );

  puts( "clock_getenable_attr -- ENOSYS" );
  sc = clock_getenable_attr( 0, NULL );
  check_enosys( sc );

  puts( "clock_setenable_attr -- ENOSYS" );
  sc = clock_setenable_attr( 0, 0 );
  check_enosys( sc );

  puts( "clock_gettime - CLOCK_THREAD_CPUTIME -- ENOSYS" );
  #if defined(_POSIX_THREAD_CPUTIME)
    {
      struct timespec tp;
      sc = clock_gettime( CLOCK_THREAD_CPUTIME, &tp );
      check_enosys( sc );
    }
  #endif

  puts( "clock_settime - CLOCK_PROCESS_CPUTIME -- ENOSYS" );
  #if defined(_POSIX_CPUTIME)
    {
      struct timespec tp;
      sc = clock_settime( CLOCK_PROCESS_CPUTIME, &tp );
      check_enosys( sc );
    }
  #endif

  puts( "clock_settime - CLOCK_THREAD_CPUTIME -- ENOSYS" );
  #if defined(_POSIX_THREAD_CPUTIME)
    {
      struct timespec tp;
      sc = clock_settime( CLOCK_THREAD_CPUTIME, &tp );
      check_enosys( sc );
    }
  #endif

  puts( "devctl -- ENOSYS" );
  sc = devctl( 0, NULL, 0, NULL );
  check_enosys( sc );

  puts( "execl -- ENOSYS" );
  sc = execl( NULL, NULL );
  check_enosys( sc );

  puts( "execle -- ENOSYS" );
  sc = execle( NULL, NULL );
  check_enosys( sc );

  puts( "execlp -- ENOSYS" );
  sc = execlp( NULL, NULL );
  check_enosys( sc );

  puts( "execv -- ENOSYS" );
  sc = execv( NULL, NULL );
  check_enosys( sc );

  puts( "execve -- ENOSYS" );
  sc = execve( NULL, NULL, NULL );
  check_enosys( sc );

  puts( "execvp -- ENOSYS" );
  sc = execvp( NULL, NULL );
  check_enosys( sc );

  puts( "fork -- ENOSYS" );
  sc = fork();
  check_enosys( sc );

  puts( "pthread_atfork -- ENOSYS" );
  sc = pthread_atfork( NULL, NULL, NULL );
  check_enosys( sc );

  puts( "pthread_getcpuclockid -- ENOSYS" );
  sc = pthread_getcpuclockid( 0, NULL );
  check_enosys( sc );

  puts( "sched_setparam -- ENOSYS" );
  sc = sched_setparam( 0, NULL );
  check_enosys( sc );

  puts( "sched_getparam -- ENOSYS" );
  sc = sched_getparam( 0, NULL );
  check_enosys( sc );

  puts( "sched_setscheduler -- ENOSYS" );
  sc = sched_setscheduler( 0, 0, NULL );
  check_enosys( sc );

  puts( "sched_getscheduler -- ENOSYS" );
  sc = sched_getscheduler( 0 );
  check_enosys( sc );

  puts( "wait -- ENOSYS" );
  sc = wait( NULL );
  check_enosys( sc );

  puts( "waitpid -- ENOSYS" );
  sc = waitpid( 0, NULL, 0 );
  check_enosys( sc );

  puts( "*** END OF POSIX TEST ENOSYS ***" );
  rtems_test_exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}
