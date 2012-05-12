/*
 *  COPYRIGHT (c) 2010
 *  Bharath Suri<bharath.s.jois@gmail.com>.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include "tmacros.h"

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);

#define FIFO_PATH "/fifo01"

static void test_main(void)
{
  int status = -1;
  int fd;
  char buf [1];
  ssize_t n;

  puts("\n\n*** FIFO / PIPE OPEN TEST - 1 ***");

  puts(
    "\nConfiguration: Pipes disabled.\n"
    "Creating named fifo '" FIFO_PATH "'.\n"
    "Must result in failure since pipes are disabled in the configuration."
  );
  status = mkfifo(FIFO_PATH, 0777);
  rtems_test_assert(status == 0);

  fd = open(FIFO_PATH, O_RDWR);
  rtems_test_assert(fd >= 0);

  n = read(fd, buf, sizeof(buf));
  rtems_test_assert(n == -1);
  rtems_test_assert(errno == ENOTSUP);

  n = write(fd, buf, sizeof(buf));
  rtems_test_assert(n == -1);
  rtems_test_assert(errno == ENOTSUP);

  status = close(fd);
  rtems_test_assert(status == 0);

  puts("*** END OF FIFO / PIPE OPEN TEST - 1 ***");
}

rtems_task Init(rtems_task_argument not_used)
{
  test_main();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM

#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 4

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
