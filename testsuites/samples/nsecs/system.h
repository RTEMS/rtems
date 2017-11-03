#ifndef SYSTEM_H
#define SYSTEM_H

#include <rtems/test.h>

#include <bsp.h> /* for device driver prototypes */

extern void dummy_function_empty_body_to_force_call(void);

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MICROSECONDS_PER_TICK 1000
#define CONFIGURE_MAXIMUM_TASKS            1
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#include <rtems/confdefs.h>
#endif
