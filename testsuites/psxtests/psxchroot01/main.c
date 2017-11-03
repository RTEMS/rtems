/*
 *  Simple test program -- simplified version of sample test hello.
 */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <bsp.h>
#include <pmacros.h>

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
int test_main(void);

rtems_task Init(
  rtems_task_argument ignored
)
{
  test_main();
  rtems_test_exit( 0 );
}

/* configuration information */

/* NOTICE: the clock driver is explicitly disabled */
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 10

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>

/* end of file */
