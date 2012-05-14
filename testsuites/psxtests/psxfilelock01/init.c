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
#include <errno.h>

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);

rtems_task Init(
  rtems_task_argument argument
)
{
  FILE *fd;
  int   sc;

  puts( "\n\n*** TEST FILE LOCK 01 ***" );

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
  
  puts( "*** END OF TEST FILE LOCK 01 ***" );

  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 4
#define CONFIGURE_MAXIMUM_TASKS             1
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
