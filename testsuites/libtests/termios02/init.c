/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include "tmacros.h"
#include <termios.h>
#include <errno.h>

rtems_task Init(
  rtems_task_argument ignored
)
{
  int sc;

  printf( "\n\n*** TERMIOS 02 TEST ***\n" );

  printf( "tcdrain(12) - EBADF\n" );
  sc = tcdrain(12);
  rtems_test_assert( sc == -1 );
  rtems_test_assert( errno == EBADF );

  printf( "tcdrain(stdin) - OK\n" );
  sc = tcdrain(0);
  rtems_test_assert( !sc );

  printf( "tcdrain(stdout) - OK\n" );
  tcdrain(1);
  rtems_test_assert( !sc );

  printf( "tcdrain(stderr) - OK\n" );
  tcdrain(2);
  rtems_test_assert( !sc );

  printf( "*** END OF TERMIOS 02 TEST ***\n" );
  exit( 0 );
}


/* NOTICE: the clock driver is explicitly disabled */
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS            1
#define CONFIGURE_USE_DEVFS_AS_BASE_FILESYSTEM

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
