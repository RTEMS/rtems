/*
 *  Simple test program -- simplified version of sample test hello.
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

#define CONFIGURE_TEST_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <confdefs.h>

/* end of file */
