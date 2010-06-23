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

#include <rtems.h>
#include <rtems/libio.h>

#include <tmacros.h>
#include "test_support.h"

int _fcntl_r(
  struct _reent *ptr __attribute__((unused)),
  int fd,
  int cmd,
  int arg
);

off_t _lseek_r(
  struct _reent *ptr __attribute__((unused)),
  int            fd,
  off_t          offset,
  int            whence
);



rtems_task Init(
  rtems_task_argument argument
)
{
  int               fd;
  struct stat       stat_buff;
  struct iovec      vec[4];
  off_t             res;
  int               status;

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

  puts("ftruncate an unopened file");
  status = ftruncate(fd, 40);
  rtems_test_assert( status == -1 );
  printf( "%d: %s\n", errno, strerror( errno ) );
  rtems_test_assert( errno == EBADF );

  puts("_fcntl_r unopened file");
  status = _fcntl_r( NULL, fd, F_SETFD, 1 );
  rtems_test_assert( status == -1 );
  printf( "%d: %s\n", errno, strerror( errno ) );
  rtems_test_assert( errno == EBADF );

  puts("fdatasync unopened file");
  status = fdatasync( fd );
  rtems_test_assert( status == -1 );
  printf( "%d: %s\n", errno, strerror( errno ) );
  rtems_test_assert( errno == EBADF );

  puts("fstat unopened file");
  status = fstat( fd, &stat_buff );
  rtems_test_assert( status == -1 );
  printf( "%d: %s\n", errno, strerror( errno ) );
  rtems_test_assert( errno == EBADF );

  puts("fsync unopened file");
  status = fsync( fd );
  rtems_test_assert( status == -1 );
  printf( "%d: %s\n", errno, strerror( errno ) );
  rtems_test_assert( errno == EBADF );

  puts("ioctl unopened file");
  status = ioctl( fd, 0 );
  rtems_test_assert( status == -1 );
  printf( "%d: %s\n", errno, strerror( errno ) );
  rtems_test_assert( errno == EBADF );

  puts("_lseek_r unopened file");
  res = _lseek_r (NULL, fd, 0, SEEK_SET);
  rtems_test_assert( res == -1 );
  printf( "%d: %s\n", errno, strerror( errno ) );
  rtems_test_assert( errno == EBADF );

  puts("readv unopened file");
  status = readv(fd, vec, 4);
  rtems_test_assert( status == -1 );
  printf( "%d: %s\n", errno, strerror( errno ) );
  rtems_test_assert( errno == EBADF );

  puts("writev unopened file");
  status = writev(fd, vec, 4);
  rtems_test_assert( status == -1 );
  printf( "%d: %s\n", errno, strerror( errno ) );
  rtems_test_assert( errno == EBADF );

  puts("write unopened file");
  status = write(fd, "1234", 4);
  rtems_test_assert( status == -1 );
  printf( "%d: %s\n", errno, strerror( errno ) );
  rtems_test_assert( errno == EBADF );




  puts( "*** END OF PSXFILE02 TEST  ***" );

  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 6
#define CONFIGURE_MAXIMUM_TASKS 1
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
