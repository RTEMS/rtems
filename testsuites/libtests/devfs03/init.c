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
#include <rtems/score/heap.h>

rtems_task Init(
  rtems_task_argument argument
)
{
  int status;
  void *alloc_ptr = (void *)0;
  Heap_Information_block Info;

  puts( "\n\n*** TEST DEVFS03 ***" );

  puts( "Init - attempt to create /dir01 -- OK" );
  status = mkdir( "/dir01", S_IRWXU );
  rtems_test_assert( status == 0 );

  puts( "Init - allocating most of workspace memory" );
  status = rtems_workspace_get_information( &Info );
  rtems_test_assert( status == true );
  status = rtems_workspace_allocate( Info.Free.largest - 4, &alloc_ptr );
  rtems_test_assert( status == true );

  puts( "Init - mount a new fs at /dir01 - expect ENOMEM" );
  status = mount( NULL, 
		  "/dir01", 
		  "devfs", 
		  RTEMS_FILESYSTEM_READ_WRITE,
		  NULL );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == ENOMEM );

  puts( "Init - freeing the workspace memory" );
  status = rtems_workspace_free( alloc_ptr );
  rtems_test_assert( status == true );

  status = rmdir( "/dir01" );
  rtems_test_assert( status == 0 );

  puts( "*** END OF TEST DEVFS03 ***" );

  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS             1
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM
#define CONFIGURE_FILESYSTEM_DEVFS

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
/* end of file */
