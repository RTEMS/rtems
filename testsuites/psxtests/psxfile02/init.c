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

#include <stdio.h>
#include <sys/uio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <rtems/imfs.h>
#include <reent.h>

#include <rtems.h>
#include <rtems/libio.h>

#include <tmacros.h>
#include "test_support.h"

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
void do_with_fd(int fd, const char *description);

void do_with_fd(
  int         fd,
  const char *description
)
{
  struct stat       stat_buff;
  struct iovec      vec[4];
  off_t             res;
  int               status;

  printf("ftruncate %s\n", description);
  status = ftruncate(fd, 40);
  rtems_test_assert( status == -1 );
  printf( "%d: %s\n", errno, strerror( errno ) );
  rtems_test_assert( errno == EBADF );

  printf("_fcntl_r %s\n", description);
  status = _fcntl_r( NULL, fd, F_SETFD, 1 );
  rtems_test_assert( status == -1 );
  printf( "%d: %s\n", errno, strerror( errno ) );
  rtems_test_assert( errno == EBADF );

  printf("fdatasync %s\n", description);
  status = fdatasync( fd );
  rtems_test_assert( status == -1 );
  printf( "%d: %s\n", errno, strerror( errno ) );
  rtems_test_assert( errno == EBADF );

  printf("fstat %s\n", description);
  status = fstat( fd, &stat_buff );
  rtems_test_assert( status == -1 );
  printf( "%d: %s\n", errno, strerror( errno ) );
  rtems_test_assert( errno == EBADF );

  printf("fsync %s\n", description);
  status = fsync( fd );
  rtems_test_assert( status == -1 );
  printf( "%d: %s\n", errno, strerror( errno ) );
  rtems_test_assert( errno == EBADF );

  printf("ioctl %s\n", description);
  status = ioctl( fd, 0 );
  rtems_test_assert( status == -1 );
  printf( "%d: %s\n", errno, strerror( errno ) );
  rtems_test_assert( errno == EBADF );

  printf("_lseek_r %s\n", description);
  res = _lseek_r (NULL, fd, 0, SEEK_SET);
  rtems_test_assert( res == -1 );
  printf( "%d: %s\n", errno, strerror( errno ) );
  rtems_test_assert( errno == EBADF );

  printf("readv %s\n", description);
  status = readv(fd, vec, 4);
  rtems_test_assert( status == -1 );
  printf( "%d: %s\n", errno, strerror( errno ) );
  rtems_test_assert( errno == EBADF );

  printf("writev %s\n", description);
  status = writev(fd, vec, 4);
  rtems_test_assert( status == -1 );
  printf( "%d: %s\n", errno, strerror( errno ) );
  rtems_test_assert( errno == EBADF );

  printf("write %s\n", description);
  status = write(fd, "1234", 4);
  rtems_test_assert( status == -1 );
  printf( "%d: %s\n", errno, strerror( errno ) );
  rtems_test_assert( errno == EBADF );
}

rtems_task Init(
  rtems_task_argument argument
)
{
  int   status;
  int   fd;

  puts( "\n\n*** PSXFILE02 TEST  ***" );

  /*
   *  Simple open case where the file is created.
   */
  puts( "mkdir /tmp" );
  status = mkdir( "/tmp", S_IRWXU );
  rtems_test_assert( !status );

  puts( "open /tmp/j" );
  fd = open( "/tmp/j", O_RDWR|O_CREAT, S_IRWXU|S_IRWXG|S_IRWXO );
  rtems_test_assert( fd != -1 );
  printf( "open returned file descriptor %d\n", fd );

  puts( "close /tmp/j" );
  status = close( fd );
  rtems_test_assert( !status );

  do_with_fd( fd, "an unopened file" );
  puts("");
  do_with_fd( 1000, "a too large file descriptor" );

  puts( "*** END OF PSXFILE02 TEST  ***" );

  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 4
#define CONFIGURE_MAXIMUM_TASKS 1
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
