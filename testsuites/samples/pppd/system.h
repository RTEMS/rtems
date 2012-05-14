#ifndef SYSTEM_H
#define SYSTEM_H

#include <rtems.h>
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
   { TTY1_DRIVER_TABLE_ENTRY, TTY2_DRIVER_TABLE_ENTRY }

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 8

#define CONFIGURE_EXECUTIVE_RAM_SIZE (512*1024)
#define CONFIGURE_MAXIMUM_SEMAPHORES 20
#define CONFIGURE_MAXIMUM_TASKS      20

#define CONFIGURE_MICROSECONDS_PER_TICK 10000

#define CONFIGURE_INIT_TASK_STACK_SIZE (10*1024)
#define CONFIGURE_INIT_TASK_PRIORITY   120
#define CONFIGURE_INIT_TASK_INITIAL_MODES (RTEMS_PREEMPT | \
                                           RTEMS_NO_TIMESLICE | \
                                           RTEMS_NO_ASR | \
                                           RTEMS_INTERRUPT_LEVEL(0))

#include <rtems/confdefs.h>

#endif
