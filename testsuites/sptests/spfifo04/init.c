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
#include <tmacros.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <rtems.h>
#include <rtems/libio.h>

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);

#define SEND_RCV_BUFSIZ 12

rtems_task Init(
  rtems_task_argument not_used
)
{
  int fd = -1;
  int status = -1;
  off_t offset = 4;
  int pipe_length = -1;
  int flag = 1;

  puts( "\n\n*** TEST PIPE/FIFO - 04 ***" );

  puts( "Init - Creating /fifo" );
  status = mkfifo( "/fifo", 0777 );
  rtems_test_assert( status == 0 );
  
  puts( "Init - Opening /fifo in readonly, non-blocking mode" );
  fd = open( "/fifo", O_RDONLY | O_NONBLOCK );
  rtems_test_assert( fd != -1 );
  
  puts( "Init - Attempt to lseek on fifo -- Expected ESPIPE" );
  offset = lseek( fd, offset, SEEK_CUR );
  rtems_test_assert( offset == -1 );
  rtems_test_assert( errno == ESPIPE );

  puts( "Init - ioctl: FIONBIO -- Expected EFAULT" );
  status = ioctl( fd, FIONBIO, NULL );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EFAULT );

  puts( "Init - ioctl: FIONBIO -- OK" );
  status = ioctl( fd, FIONBIO, &flag );
  rtems_test_assert( status == 0 );
 
  flag = 0;
  puts( "Init - ioctl: FIONBIO -- OK" );
  status = ioctl( fd, FIONBIO, &flag );
  rtems_test_assert( status == 0 );

  puts( "Init - ioctl: Dummy Command -- Expected EINVAL" );
  status = ioctl( fd, -1, NULL );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EINVAL );

  puts( "Init - ioctl: FIONREAD -- Expected EFAULT" );
  status = ioctl( fd, FIONREAD, NULL );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EFAULT );

  puts( "Init - ioctl: FIONREAD -- OK" );
  status = ioctl( fd, FIONREAD, &pipe_length );
  rtems_test_assert( status == 0 );
  rtems_test_assert( pipe_length == 0 );
  
  puts( "Init - closing /fifo" );
  status = close( fd );
  rtems_test_assert( status == 0 );
  
  puts( "Init - removing /fifo" );
  status = unlink( "/fifo" );
  rtems_test_assert( status == 0 );

  puts( "*** END OF TEST PIPE/FIFO - 04 ***" );
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 6

#define CONFIGURE_MAXIMUM_TASKS 3
#define CONFIGURE_MAXIMUM_BARRIERS 1
#define CONFIGURE_MAXIMUM_FIFOS 1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_FIFOS_ENABLED

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
/* end of file */
