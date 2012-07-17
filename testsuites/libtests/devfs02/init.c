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

#include "test_support.h"

#include <tmacros.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <rtems/devfs.h>
#include <rtems/malloc.h>

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);

rtems_task Init(
  rtems_task_argument argument
)
{
  int status;
  rtems_filesystem_location_info_t *rootloc = &rtems_filesystem_root->location;
  const devFS_data *data = rootloc->mt_entry->immutable_fs_info;
  devFS_data zero_count_data = {
    .nodes = data->nodes,
    .count = 0
  };
  void *opaque;

  puts( "\n\n*** TEST DEVFS02 ***" );

  puts( "Init - attempt to create a fifo - expect ENOTSUP" );
  status = mkfifo( "/fifo01", 0 );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == ENOTSUP );

  /* Manipulate the device table size */
  puts( "Init - set device table size to zero" );
  rootloc->mt_entry->immutable_fs_info = &zero_count_data;

  puts( "Init - attempt to create a node - expect ENOSPC" );
  status = mknod( "/node", S_IFBLK, 0LL );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == ENOSPC );

  /* Now restore */
  puts( "Init - restore device table size" );
  rootloc->mt_entry->immutable_fs_info = data;

  opaque = rtems_heap_greedy_allocate( NULL, 0 );

  puts( "Init - attempt to create a node - expect ENOMEM" );
  status = mknod( "/node", S_IFBLK, 0LL );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == ENOMEM );

  rtems_heap_greedy_free( opaque );

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
