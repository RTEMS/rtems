/*
 *  Simple test program -- check out of the basic file system mounting 
 *  capabilities
 *  Attempt to mount the IMFS file system on a mount point in the base IMFS
 */

#define TEST_INIT

#include <bsp.h>

void test_main( void );

rtems_task Init(
  rtems_task_argument ignored
)
{
  test_main();
  exit( 0 );
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <confdefs.h>

/* end of file */
