/*
 *  COPYRIGHT (c) 2010
 *  Bharath Suri<bharath.s.jois@gmail.com>.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

/* Includes */
#include <bsp.h>
#include <tmacros.h>

/* Includes */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <rtems.h>
#include <rtems/libio.h>


void test_main(void)
{

  int status = -1;
  int fd = 0;

  puts("\n\n*** FIFO / PIPE OPEN TEST - 2 ***");
  puts(
"\n\nConfiguration: Pipes configured, \
but number of barriers configured = 0"
       );

  puts("\n\nCreating directory /tmp");
  status = mkdir("/tmp", 0777);
  rtems_test_assert(status == 0);

  puts("\n\nCreating fifo /tmp/fifo");
  status = mkfifo("/tmp/fifo01", 0777);
  rtems_test_assert(status == 0);

  puts("\n\nAttempt to open the fifo file\n");
  puts(
"Must result in failure since \n\
number of barriers = 0 => not all resources\n\
were acquired"
       );

  fd = open("/tmp/fifo01", O_RDONLY);
  rtems_test_assert(fd == -1);
  rtems_test_assert(errno == EINTR); // Should this
                                     // be ENOMEM?
  puts("\n\nRemove the entry /tmp/fifo01");
  status = unlink("/tmp/fifo01");
  rtems_test_assert(status == 0);

  puts("\n\nRemove directory /tmp");
  status = rmdir("/tmp");
  rtems_test_assert(status == 0);

  puts("\n\n*** END OF FIFO / PIPE OPEN TEST - 2 ***");
}
  
rtems_task Init(
  rtems_task_argument not_used
)
{
  test_main();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 6

#define CONFIGURE_PIPES_ENABLED
#define CONFIGURE_MAXIMUM_PIPES 1
#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_INIT

#include <rtems/confdefs.h>

/* end of file */
