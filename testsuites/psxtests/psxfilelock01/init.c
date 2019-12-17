/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>
#include "test_support.h"
#include <errno.h>

const char rtems_test_name[] = "PSXFILELOCK 1";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);

rtems_task Init(
  rtems_task_argument argument
)
{
  FILE *fd;
  int   sc;

  TEST_BEGIN();

  puts( "Open /testfile" );
  fd = fopen( "/testfile", "w+" );
  rtems_test_assert( fd );

  puts( "flockfile /testfile" );
  flockfile( fd );
  
  puts( "ftrylockfile /testfile" );
  sc = ftrylockfile( fd );
  rtems_test_assert( sc == -1 );
  rtems_test_assert( errno == ENOTSUP );
  
  puts( "flockfile /testfile" );
  flockfile( fd );
  
  puts( "funlockfile /testfile" );
  funlockfile( fd );
  
  TEST_END();

  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 4
#define CONFIGURE_MAXIMUM_TASKS             1
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
