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
#include <rtems/malloc.h>

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);

rtems_task Init(
  rtems_task_argument argument
)
{
  int                        sc;
  rtems_malloc_statistics_t  stats;

  puts( "\n\n*** TEST MALLOC05 ***" );

  puts( "malloc_get_statistics( NULL ) - returns -1" );
  sc = malloc_get_statistics( NULL );
  rtems_test_assert( sc == -1 );

  puts( "malloc_get_statistics( &stats ) - returns -0" );
  sc = malloc_get_statistics( &stats );
  rtems_test_assert( sc == 0 );

  puts( "*** END OF TEST MALLOC05 ***" );

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
