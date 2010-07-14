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

rtems_task Init(
  rtems_task_argument argument
)
{
  int                               sc;
  int                               size;
  rtems_filesystem_location_info_t *temp_loc;
  rtems_device_name_t              *device_name_table;

  puts( "\n\n*** TEST DEVFS01 ***" );

  puts( "devFS_Show - OK" );
  sc = devFS_Show();
  rtems_test_assert( sc == 0 );

  /* save original node access information */
  temp_loc = &rtems_filesystem_root;
  device_name_table = (rtems_device_name_t *)temp_loc->node_access;
  temp_loc->node_access = NULL;

  puts( "devFS_Show - no device table - EFAULT" );
  sc = devFS_Show();
  rtems_test_assert( sc == -1 );
  rtems_test_assert( errno == EFAULT );

  /* restore node access information */
  temp_loc->node_access = device_name_table;

  /* save original device table size information */
  size = rtems_device_table_size;
  rtems_device_table_size = 0;
  puts( "devFS_Show - devices - OK" );
  sc = devFS_Show();
  rtems_test_assert( sc == 0 );

  /* restore original device table size information */
  rtems_device_table_size = size;
  
  puts( "*** END OF TEST DEVFS01 ***" );

  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS             1
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_USE_DEVFS_AS_BASE_FILESYSTEM

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
