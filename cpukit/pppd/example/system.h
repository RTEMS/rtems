
#ifndef SYSTEM_H
#define SYSTEM_H

#include <rtems.h>
#include <tty_drv.h>

/* functions */
extern rtems_task Init(rtems_task_argument argument);

#include <bsp.h>

#define CONFIGURE_HAS_OWN_DEVICE_DRIVER_TABLE
#ifdef CONFIGURE_INIT
rtems_driver_address_table Device_drivers[5] = {
  CONSOLE_DRIVER_TABLE_ENTRY,
  CLOCK_DRIVER_TABLE_ENTRY,
  TTY1_DRIVER_TABLE_ENTRY,
  TTY2_DRIVER_TABLE_ENTRY,
  {NULL, NULL, NULL, NULL, NULL, NULL}
};
#endif

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
