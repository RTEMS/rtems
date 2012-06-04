/*
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
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
#include <errno.h>
#include <rtems/libio_.h>
#include <rtems/libcsupport.h>

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
int fs_mount(
  rtems_filesystem_mount_table_entry_t *mt_entry,
  const void                           *data
);

int fs_mount(
  rtems_filesystem_mount_table_entry_t *mt_entry,
  const void                           *data
)
{
  return 0;
}

rtems_task Init(
  rtems_task_argument argument
)
{
  int status = 0;
  void *alloc_ptr = (void *)0;

  puts( "\n\n*** TEST MOUNT MANAGER ROUTINE - 01 ***" );

  puts( "Init - allocating most of heap -- OK" );
  alloc_ptr = malloc( malloc_free_space() - 4 );
  rtems_test_assert( alloc_ptr != NULL );

  puts( "Init - attempt to register filesystem fs - expect ENOMEM" );
  status = rtems_filesystem_register( "fs", fs_mount );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == ENOMEM );

  puts( "Init - freeing allocated memory -- OK" );
  free( alloc_ptr );

  puts( "Init - register filesystem fs -- OK" );
  status = rtems_filesystem_register( "fs", fs_mount );
  rtems_test_assert( status == 0 );

  puts( "Init - attempt to make target(NULL) and mount - expect EINVAL" );
  status = mount_and_make_target_path(
             NULL,
             NULL,
             "fs",
             0,
             NULL );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EINVAL );

  puts( "Init - attempt to make target and mount - expect EINVAL" );
  status = mount_and_make_target_path(
             NULL,
             "/tmp",
             "fs",
             2,
             NULL );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EINVAL );

  puts( "Init - register filesystem fs - expect EINVAL" );
  status = rtems_filesystem_register( "fs", fs_mount );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EINVAL );

  puts( "Init - register filesystem bfs -- OK" );
  status = rtems_filesystem_register( "bfs", fs_mount );
  rtems_test_assert( status == 0 );

  puts( "Init - register filesystem bfs - expect EINVAL" );
  status = rtems_filesystem_register( "bfs", fs_mount );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EINVAL );

  puts( "Init - attempt to unregister with bad args - expect EINVAL" );
  status = rtems_filesystem_unregister( NULL );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EINVAL );

  puts( "Init - attempt to unregister fs -- OK" );
  status = rtems_filesystem_unregister( "fs" );
  rtems_test_assert( status == 0 );

  puts( "Init - attempt to unregister fs again - expect ENOENT" );
  status = rtems_filesystem_unregister( "fs" );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == ENOENT );

  puts( "Init - attempt to unregister bfs -- OK" );
  status = rtems_filesystem_unregister( "bfs" );
  rtems_test_assert( status == 0 );

  puts( "Init - attempt to unregister bfs again - expect ENOENT" );
  status = rtems_filesystem_unregister( "bfs" );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == ENOENT );

  puts( "*** END OF TEST MOUNT MANAGER ROUTINE - 01 ***" );
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS             1
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
