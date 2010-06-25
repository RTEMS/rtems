/*
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <tmacros.h>
#include "test_support.h"
#include "termios_testdriver_polled.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void write_helper(
  int        fd,
  const char *c
)
{
  size_t len;
  int    rc;
  
  len = strlen( c );
  rc = write( fd, c, len );
  rtems_test_assert( rc == len );
}
rtems_task Init(
  rtems_task_argument argument
)
{
  int  fd;
  int  rc;

  puts( "\n\n*** TEST TERMIOS03 ***" );

  puts( "open(" TERMIOS_TEST_DRIVER_DEVICE_NAME ") - OK " );
  fd = open( TERMIOS_TEST_DRIVER_DEVICE_NAME, O_RDWR );
  rtems_test_assert( fd != -1 );

  write_helper( fd, "This is a test\n" );

  puts( "close(" TERMIOS_TEST_DRIVER_DEVICE_NAME ") - OK " );
  rc = close( fd );
  rtems_test_assert( rc == 0 );

  puts( "*** END OF TEST TERMIOS03 ***" );

  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_EXTRA_DRIVERS \
  TERMIOS_TEST_DRIVER_TABLE_ENTRY

/* include an extra slot for registering the termios one dynamically */
#define CONFIGURE_MAXIMUM_DRIVERS 3

/* one for the console and one for the test port */
#define CONFIGURE_NUMBER_OF_TERMIOS_PORTS 3

/* we need to be able to open the test device */
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 4
#define CONFIGURE_MAXIMUM_TASKS             1
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
