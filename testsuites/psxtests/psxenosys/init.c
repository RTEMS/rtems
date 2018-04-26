/*
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <pthread.h>

#define CONFIGURE_INIT
#include "system.h"
#include "tmacros.h"

#include <aio.h>
#include <time.h>
#include <unistd.h>
#include <sched.h>

const char rtems_test_name[] = "PSXENOSYS";

void check_enosys(int status);

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

  TEST_BEGIN();

  puts( "lio_listio -- ENOSYS" );
  sc = lio_listio( 0, NULL, 0, NULL );
  check_enosys( sc );

  puts( "aio_suspend -- ENOSYS" );
  sc = aio_suspend( NULL, 0, NULL );
  check_enosys( sc );

  puts( "clock_getcpuclockid -- ENOSYS" );
  sc = clock_getcpuclockid( 0, NULL );
  check_enosys( sc );

  puts( "execl -- ENOSYS" );
  sc = execl( NULL, NULL, (char*)0 );
  check_enosys( sc );

  puts( "execle -- ENOSYS" );
  sc = execle( NULL, NULL, (char*)0, NULL );
  check_enosys( sc );

  puts( "execlp -- ENOSYS" );
  sc = execlp( NULL, NULL, (char*)0 );
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

  puts( "mprotect -- stub implementation - OK" );
  sc = mprotect( NULL, 0, 0 );
  posix_service_failed( sc, "mprotect" );

  puts( "vfork -- stub implementation - OK" );
  sc = vfork();
  if ( sc != -1 ) {
    puts( "vfork did not return -1" );
    rtems_test_exit( 0 );
  }

  TEST_END();
  rtems_test_exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}
