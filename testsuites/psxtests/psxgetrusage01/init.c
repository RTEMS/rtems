/*
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/time.h>
#include <sys/resource.h>
#include <errno.h>

#if !HAVE_DECL_GETRUSAGE
extern int getrusage(int who, struct rusage *usage);
#endif

#include <tmacros.h>
#include "test_support.h"

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS             1
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>

rtems_task Init(
  rtems_task_argument argument
)
{
  int           sc;
  struct rusage usage;

  puts( "\n\n*** TEST GETRUSAGE 01 ***" );

  puts( "getrusage( RUSAGE_SELF, NULL ) -- EFAULT" );
  sc = getrusage( RUSAGE_SELF, NULL );
  rtems_test_assert( sc == -1 );
  rtems_test_assert( errno == EFAULT );

  puts( "getrusage( RUSAGE_CHILDREN, &usage ) -- ENOSYS" );
  sc = getrusage( RUSAGE_CHILDREN, &usage );
  rtems_test_assert( sc == -1 );
  rtems_test_assert( errno == ENOSYS );

  puts( "getrusage( 77, &usage ) -- EINVAL" );
  sc = getrusage( 77, &usage );
  rtems_test_assert( sc == -1 );
  rtems_test_assert( errno == EINVAL );

  puts( "Consume CPU long enough to have non-zero usage" );
  rtems_test_spin_for_ticks( 5 );
  
  puts( "getrusage( RUSAGE_SELF, &usage ) -- EINVAL" );
  sc = getrusage( RUSAGE_SELF, &usage );
  rtems_test_assert( sc == 0 );

  /* CPU usage is non-zero */
  rtems_test_assert( usage.ru_utime.tv_sec == 0 );
  rtems_test_assert( usage.ru_utime.tv_usec != 0 );

  /* System and user time is the same */
  rtems_test_assert( usage.ru_utime.tv_sec == usage.ru_stime.tv_sec );
  rtems_test_assert( usage.ru_utime.tv_usec == usage.ru_stime.tv_usec );
  
  puts( "*** END OF TEST GETRUSAGE 01 ***" );

  rtems_test_exit(0);
}
