/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <pmacros.h>
#include <sys/time.h>
#include <errno.h>

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);

void *POSIX_Init(
  void *argument
)
{
  int              status;
  struct itimerval itimer;
  struct itimerval otimer;

  puts( "\n\n*** POSIX TEST ITIMER ***" );

  /* test getitimer stub */
  puts( "getitimer -- bad which - EINVAL " );
  status = getitimer( 1234, &itimer );
  rtems_test_assert( status == -1 && errno == EINVAL );

  puts( "getitimer -- NULL pointer - EFAULT " );
  status = getitimer( ITIMER_REAL, NULL );
  rtems_test_assert( status == -1 && errno == EFAULT );

  puts( "getitimer -- ITIMER_REAL - ENOSYS " );
  status = getitimer( ITIMER_REAL, &itimer );
  rtems_test_assert( status == -1 && errno == ENOSYS );

  puts( "getitimer -- ITIMER_VIRTUAL - ENOSYS " );
  status = getitimer( ITIMER_VIRTUAL, &itimer );
  rtems_test_assert( status == -1 && errno == ENOSYS );

  puts( "getitimer -- ITIMER_PROF - ENOSYS " );
  status = getitimer( ITIMER_PROF, &itimer );
  rtems_test_assert( status == -1 && errno == ENOSYS );

  /* test setitimer stub */
  puts( "setitimer -- bad which - EINVAL " );
  status = setitimer( 1234, &itimer, &otimer );
  rtems_test_assert( status == -1 && errno == EINVAL );

  puts( "setitimer -- NULL value pointer - EFAULT " );
  status = setitimer( ITIMER_REAL, NULL, &otimer );
  rtems_test_assert( status == -1 && errno == EFAULT );

  puts( "setitimer -- NULL value pointer - EFAULT " );
  status = setitimer( ITIMER_REAL, &itimer, NULL );
  rtems_test_assert( status == -1 && errno == EFAULT );

  puts( "setitimer -- ITIMER_REAL - ENOSYS " );
  status = setitimer( ITIMER_REAL, &itimer, &otimer );
  rtems_test_assert( status == -1 && errno == ENOSYS );

  puts( "setitimer -- ITIMER_VIRTUAL - ENOSYS " );
  status = setitimer( ITIMER_VIRTUAL, &itimer, &otimer );
  rtems_test_assert( status == -1 && errno == ENOSYS );

  puts( "setitimer -- ITIMER_PROF - ENOSYS " );
  status = setitimer( ITIMER_PROF, &itimer, &otimer );
  rtems_test_assert( status == -1 && errno == ENOSYS );

  puts( "*** END OF POSIX TEST ITIMER ***" );
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_POSIX_THREADS     1

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
