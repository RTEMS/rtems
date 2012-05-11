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

#include <bsp.h>
#include <pmacros.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument ignored);

rtems_task Init(
  rtems_task_argument ignored
)
{
  int status = 0;
  int fd = 0;

  puts( "*** TEST Posix file op tests - 01 ***" );

  /****************************************************
   *                   fchdir tests
   ***************************************************/

  puts( "Init - fchdir tests" );

  puts( "Init - Attempt fchdir with bad file descriptor - expect EBADF" );
  status = fchdir( 5 );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EBADF );

  puts( "Init - Attempt fchdir with bad file descriptor - expect EBADF" );
  status = fchdir( 3 );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EBADF );
  
  puts( "Init - opening /newfile in write-mode -- OK" );
  fd = open( "/newfile", O_WRONLY | O_CREAT, S_IWUSR | S_IXUSR );
  rtems_test_assert( fd != -1 );

  puts( "Init - fchdir on the file descriptor - expect ENOTDIR" );
  status = fchdir( fd );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == ENOTDIR );

  puts( "Init - closing /newfile -- OK" );
  status = close( fd );
  rtems_test_assert( status == 0 );

  puts( "Init - removing /newfile -- OK" );
  status = unlink( "/newfile" );
  rtems_test_assert( status == 0 );

  puts( "Init - opening /newfile in read-mode -- OK" );
  fd = open( "/newfile", O_RDONLY | O_CREAT, S_IRUSR | S_IXUSR);
  rtems_test_assert( fd != -1 );
  
  puts( "Init - fchdir on the file descriptor - expect ENOTDIR" );
  status = fchdir( fd );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == ENOTDIR );

  puts( "Init - closing and removing /newfile -- OK" );
  status = close( fd );
  status |= unlink( "/newfile" );
  rtems_test_assert( status == 0 );

  puts( "Init - create directory  /tmp - RWX permissions -- OK" );
  status = mkdir( "/tmp", S_IRWXU );
  rtems_test_assert( status == 0 );

  puts( "Init - open the /tmp, get the file descriptor -- OK" );
  fd = open( "/tmp", O_RDONLY );
  rtems_test_assert( fd != -1 );

  puts( "Init - fchdir on the file descriptor -- OK" );
  status = fchdir( fd );
  rtems_test_assert( status == 0 );

  puts( "Init - close the file descriptor -- OK" );
  status = close( fd );
  rtems_test_assert( status == 0 );

  puts( "Init - remove directory /tmp -- OK" );
  status = rmdir( "/tmp" );
  rtems_test_assert( status == 0 );

  puts( "Init - creating directory /tmp - read permission -- OK" );
  status = mkdir( "/tmp", S_IRUSR );
  rtems_test_assert( status == 0 );

  puts( "Init - open the /tmp, get the file descriptor -- OK" );
  fd = open( "/tmp", O_RDONLY );
  rtems_test_assert( fd != -1 );

  puts( "Init - attempt fchdir on the file descriptor -- expect EACCES" );
  status = fchdir( fd );
  rtems_test_assert( status == -1);
  rtems_test_assert( errno == EACCES );

  puts( "Init - close the file descriptor -- OK" );
  status = close( fd );
  rtems_test_assert( status == 0 );

  puts( "Init - remove directory /tmp -- OK" );
  status = rmdir( "/tmp" );
  rtems_test_assert( status == 0 );

  puts( "End of fchdir tests" );

  /****************************************************
   *                   fchmod tests
   ***************************************************/

  puts( "\nInit - fchmod tests" );

  puts( "Init - fchmod, with a bad file descriptor - expect EBADF" );
  status = fchmod( 4, 0 );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EBADF );

  puts( "Init - fchmod, with an unopened file descriptor - expect EBADF" );
  status = fchmod( 3, 0 );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EBADF );

  puts( "Init - open new file: /newfile in read-only mode -- OK" );
  fd = open( "/newfile", O_RDONLY | O_CREAT, S_IRWXU );
  rtems_test_assert( fd != -1 );
  
  puts( "Init - fchmod, with the opened file descriptor -- OK" );
  status = fchmod( fd, 0 );
  rtems_test_assert( status == 0 );

  puts( "Init - close and remove /newfile" );
  status = close( fd );
  status |= unlink( "/newfile" );
  rtems_test_assert( status == 0 );

  puts( "Init - open new file: /newfile in read-write mode -- OK" );
  fd = open( "/newfile", O_RDWR | O_CREAT, S_IRWXU );
  rtems_test_assert( fd != -1 );
  
  puts( "Init - fchmod, with the opened file descriptor -- OK" );
  status = fchmod( fd, S_IRUSR );
  rtems_test_assert( status == 0 );

  puts( "Init - close and remove /newfile -- OK" );
  status = close( fd );
  status |= unlink( "/newfile" );
  rtems_test_assert( status == 0 );

  puts( "End of fchmod tests" );

  /****************************************************
   *                   fchown tests
   ***************************************************/

  puts( "\nInit - fchown tests" );

  puts( "Init - fchown, with a bad file descriptor - expect EBADF" );
  status = fchown( 4, 0, 0 );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EBADF );

  puts( "Init - fchown, with an unopened file descriptor - expect EBADF" );
  status = fchown( 3, 0, 0 );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EBADF );

  puts( "Init - open new file: /newfile in read-only mode -- OK" );
  fd = open( "/newfile", O_RDONLY | O_CREAT, S_IRWXU );
  rtems_test_assert( fd != -1 );
  
  puts( "Init - fchown, with the opened file descriptor - OK" );
  status = fchown( fd, 0, 0 );
  rtems_test_assert( status == 0 );

  puts( "Init - close and remove /newfile" );
  status = close( fd );
  status |= unlink( "/newfile" );
  rtems_test_assert( status == 0 );

  puts( "Init - open new file: /newfile in read-write mode -- OK" );
  fd = open( "/newfile", O_RDWR | O_CREAT, S_IRWXU );
  rtems_test_assert( fd != -1 );
  
  puts( "Init - fchown, with the opened file descriptor -- OK" );
  status = fchown( fd, 1, 0 );
  rtems_test_assert( status == 0 );

  puts( "Init - close and remove /newfile -- OK" );
  status = close( fd );
  status |= unlink( "/newfile" );
  rtems_test_assert( status == 0 );

  puts( "End of fchown tests" );

  puts( "*** END OF TEST Posix file op tests - 01 ***" );
  rtems_test_exit( 0 );
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
