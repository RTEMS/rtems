/*
 *  COPYRIGHT (c) 1989-2014.
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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <rtems/malloc.h>

const char rtems_test_name[] = "SPFIFO 2";

#define NUM_OPEN_REQ 26

static void create_fifo(void)
{
  int status;

  status = mkfifo("/fifo01", 0777);
  rtems_test_assert(status == 0);
}

static void open_fifo(int expected, int flags)
{
  int fd;

  fd = open("/fifo01", flags);
  printf( "expect status=%d errno=%d/(%s)\n", fd, errno, strerror(errno) );
  if ( expected ) {
    rtems_test_assert(fd == -1);
    rtems_test_assert(errno == expected); 
  } else {
    rtems_test_assert(fd != -1);
    close( fd );
  }
}

static rtems_task Init(
  rtems_task_argument argument
)
{
  void *p;
  int num_opens = 0;
  int index = 0;

  TEST_BEGIN();

  puts( "Creating FIFO" );
  create_fifo();

  p = rtems_heap_greedy_allocate(NULL, 0);
  puts("Opening FIFO.. expect ENOMEM since no memory is available");
  open_fifo(ENOMEM, O_RDWR);
  rtems_heap_greedy_free(p);

  puts( "Opening FIFO in RDWR mode. Expect OK" );
  open_fifo(0, O_RDWR);
  ++num_opens;

  puts( "Opening FIFO in non blocking RDONLY mode. Expect OK");
  open_fifo(0, O_RDONLY | O_NONBLOCK);
  ++num_opens;

  puts( "Opening FIFO in non blocking WRONLY mode. Expect ENXIO");
  open_fifo(ENXIO, O_WRONLY | O_NONBLOCK);
  ++num_opens;

  puts("\nMultiple opens\n");
  index = 0;
  do {

    printf("%d... ", index+1);
    open_fifo(0, O_RDONLY | O_NONBLOCK);
    ++index;
  } while ( index < NUM_OPEN_REQ - num_opens );

  TEST_END();

  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS             1
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 5

#define CONFIGURE_IMFS_ENABLE_MKFIFO

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
