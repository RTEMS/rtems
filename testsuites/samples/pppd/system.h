#ifndef SYSTEM_H
#define SYSTEM_H

#include <rtems.h>
#include <rtems/test.h>
#include <rtems/console.h>
#include <rtems/clockdrv.h>

/* functions */
extern rtems_task Init(rtems_task_argument argument);
extern int pppdapp_initialize(void);

#include <bsp.h>

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#ifndef TTY1_DRIVER_TABLE_ENTRY
  #define TTY1_DRIVER_TABLE_ENTRY NULL_DRIVER_TABLE_ENTRY
#endif
#ifndef TTY2_DRIVER_TABLE_ENTRY
  #define TTY2_DRIVER_TABLE_ENTRY NULL_DRIVER_TABLE_ENTRY
#endif

#define CONFIGURE_APPLICATION_EXTRA_DRIVERS \
  TTY1_DRIVER_TABLE_ENTRY, TTY2_DRIVER_TABLE_ENTRY

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 8

#define CONFIGURE_UNIFIED_WORK_AREAS

#define CONFIGURE_UNLIMITED_OBJECTS

#define CONFIGURE_MICROSECONDS_PER_TICK 10000

#define CONFIGURE_INIT_TASK_STACK_SIZE (10*1024)
#define CONFIGURE_INIT_TASK_PRIORITY   120
#define CONFIGURE_INIT_TASK_INITIAL_MODES (RTEMS_PREEMPT | \
                                           RTEMS_NO_TIMESLICE | \
                                           RTEMS_NO_ASR | \
                                           RTEMS_INTERRUPT_LEVEL(0))
#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#include <rtems/confdefs.h>

#endif
