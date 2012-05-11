/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modifications to support reference counting in the file system are
 *  Copyright (c) 2012 embedded brains GmbH.
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

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);

rtems_task Init(
  rtems_task_argument argument
)
{
  int status;
  devFS_node nodes [1];
  devFS_data data = {
    .nodes = nodes,
    .count = 1
  };

  puts( "\n\n*** TEST DEVFS03 ***" );

  puts( "Init - attempt to create /dir01 -- OK" );
  status = mkdir( "/dir01", S_IRWXU );
  rtems_test_assert( status == 0 );

  puts( "Init - mount a new fs at /dir01 - expect EINVAL" );
  status = mount( NULL, 
		  "/dir01", 
		  "devfs", 
		  RTEMS_FILESYSTEM_READ_WRITE,
		  NULL );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EINVAL );

  puts( "Init - mount a new fs at /dir01 - OK" );
  status = mount( NULL, 
		  "/dir01", 
		  "devfs", 
		  RTEMS_FILESYSTEM_READ_WRITE,
		  &data );
  rtems_test_assert( status == 0 );

  puts( "Init - make file /dir01/dev -- expect ENOTSUP" );
  status = creat( "/dir01/dev", S_IRWXU );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == ENOTSUP );

  puts( "Init - unmount fs at /dir01 - OK" );
  status = unmount( "/dir01" );
  rtems_test_assert( status == 0 );

  status = rmdir( "/dir01" );
  rtems_test_assert( status == 0 );

  puts( "*** END OF TEST DEVFS03 ***" );

  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 4
#define CONFIGURE_MAXIMUM_TASKS             1
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM
#define CONFIGURE_FILESYSTEM_DEVFS

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
/* end of file */
