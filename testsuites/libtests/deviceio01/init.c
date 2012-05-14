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
#include <rtems/devfs.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "test_driver.h"
#include <rtems/devnull.h>

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);

rtems_task Init(
  rtems_task_argument argument
)
{
  int status;
  int fdr = 0, fdw = 0;
  char buf[10];

  puts( "\n\n*** TEST DEVICEIO - 01 ***" );

  puts( "Init - attempt to open the /dev/test WR mode -- OK" );
  fdw = open( "/dev/test", O_WRONLY );
  rtems_test_assert( fdw != -1 );
  
  puts( "Init - attempt to write to /dev/test - expect ENOSYS" );
  status = write( fdw, "data", 10 );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == ENOSYS );

  puts( "Init - attempt to open the /dev/test RD mode -- OK" );
  fdr = open( "/dev/test", O_RDONLY );
  rtems_test_assert( fdr != -1 );

  puts( "Init - attempt to read from /dev/test - expect ENOSYS" );
  status = read( fdr, buf, 10 );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == ENOSYS );

  puts( "Init - attempt ioctl on the device - expect ENOSYS" );
  status = ioctl( fdr, -1 );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == ENOSYS );

  puts( "*** END OF TEST DEVICEIO - 01 ***" );
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_EXTRA_DRIVERS TEST_DRIVER_TABLE_ENTRY

/* include an extra slot for registering the termios one dynamically */
#define CONFIGURE_MAXIMUM_DRIVERS 3

#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 5

#define CONFIGURE_MAXIMUM_TASKS             1
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
/* end of file */
