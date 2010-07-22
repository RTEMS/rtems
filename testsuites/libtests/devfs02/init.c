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
#include <rtems/devfs.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

rtems_task Init(
  rtems_task_argument argument
)
{
  int status;
  rtems_filesystem_location_info_t *temp_loc;
  rtems_device_name_t *device_name_table;
  int temp_size = 0;
  struct stat statbuf;

  puts( "\n\n*** TEST DEVFS02 ***" );

  puts( "Init - attempt to create a fifo - expect EINVAL" );
  status = mkfifo( "/fifo01", 0 );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EINVAL );
  
  /* Manipulate the root */
  puts( "Init - set the device name table to NULL" );
  temp_loc = &rtems_filesystem_root;
  device_name_table = (rtems_device_name_t *)temp_loc->node_access;
  temp_loc->node_access = NULL;

  puts(" Init - attempt to create a node - expect EFAULT" );
  status = mknod( "/node", S_IFBLK, 0LL );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EFAULT );

  /* This case actually stops at evaluation of path */
  puts( "Init - attempt to stat a node - expect EFAULT" );
  status = stat( "/", &statbuf );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EFAULT );

  puts( "Init - attempt to open a node" );
  status = open( "/node", O_RDWR );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EFAULT );

  /* Now restore */
  puts( "Init - restore the device name table" );
  temp_loc->node_access = device_name_table;

  /* Manipulate the device table size */
  puts( "Init - set device table size to zero" );
  temp_size = rtems_device_table_size;
  rtems_device_table_size = 0;

  puts( "Init - attempt to create a node - expect ENOMEM" );
  status = mknod( "/node", S_IFBLK, 0LL );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == ENOMEM );

  /* Now restore */
  puts( "Init - restore device table size" );
  rtems_device_table_size = temp_size;

  puts( "Init - attempt to create /node -- OK" );
  status = mknod( "/node", S_IFBLK, 0LL );
  rtems_test_assert( status == 0 );

  puts( "Init - attempt to create /node - expect EEXIST" );
  status = mknod( "/node", S_IFBLK, 0LL );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EEXIST );

  puts( "*** END OF TEST DEVFS02 ***" );

  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS             1
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 5

#define CONFIGURE_USE_DEVFS_AS_BASE_FILESYSTEM

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
/* end of file */
