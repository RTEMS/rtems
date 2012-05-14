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

#include <tmacros.h>
#include "test_support.h"
#include <sys/times.h>
#include <errno.h>

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
clock_t _times_r(struct _reent *ptr, struct tms  *ptms);
clock_t _times(struct tms  *ptms);

rtems_task Init(
  rtems_task_argument argument
)
{
  clock_t    start;
  clock_t    end;
  clock_t    now;
  clock_t    sc;
  clock_t    difference;
  struct tms start_tm;
  struct tms end_tm;

  puts( "\n\n*** TEST TIMES 01 ***" );

  puts( "times( NULL ) -- EFAULT" );
  sc = times( NULL );
  rtems_test_assert( sc == -1 );
  rtems_test_assert( errno == EFAULT );

  puts( "_times_r( NULL, NULL ) -- EFAULT" );
  start = _times_r( NULL, NULL );
  rtems_test_assert( sc == -1 );
  rtems_test_assert( errno == EFAULT );

  while ( rtems_clock_get_ticks_since_boot() == 0 )
    ;

  puts( "_times( &start_tm ) -- OK" );
  now = _times( &start_tm );
  rtems_test_assert( start != 0 );
  rtems_test_assert( now != 0 );
  
  rtems_test_spin_for_ticks(5);

  puts( "_times( &end_tm ) -- OK" );
  end = _times( &end_tm );
  rtems_test_assert( end != 0 );
  
  puts( "Check various values" );
  difference = end - start;
  rtems_test_assert( difference >= 5 );

  rtems_test_assert( end_tm.tms_utime >= start_tm.tms_utime );
  rtems_test_assert( end_tm.tms_stime >= start_tm.tms_stime );
  rtems_test_assert( end_tm.tms_cutime == 0 );
  rtems_test_assert( end_tm.tms_cstime == 0 );
  
  puts( "*** END OF TEST TIMES 01 ***" );

  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS             1
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
