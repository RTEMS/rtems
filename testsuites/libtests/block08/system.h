/*  system.h
 *
 *  This include file contains information that is included in every
 *  function in the test set.
 *
 *  Copyright (C) 2010 OKTET Labs, St.-Petersburg, Russia
 *  Author: Oleg Kravtsov <Oleg.Kravtsov@oktetlabs.ru>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems.h>
#include <rtems/blkdev.h>
#include <rtems/test.h>

#include <bsp.h>

rtems_task Init(
  rtems_task_argument argument
);

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 4

#define CONFIGURE_MAXIMUM_TASKS               28
#define CONFIGURE_MAXIMUM_TIMERS              10
#define CONFIGURE_MAXIMUM_SEMAPHORES          20
#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES      10
#define CONFIGURE_MAXIMUM_PARTITIONS          1
#define CONFIGURE_MAXIMUM_REGIONS             1
#define CONFIGURE_MAXIMUM_PERIODS             1
#define CONFIGURE_MAXIMUM_USER_EXTENSIONS     0
#define CONFIGURE_TICKS_PER_TIMESLICE       100

#define CONFIGURE_APPLICATION_NEEDS_LIBBLOCK
#define CONFIGURE_BDBUF_MAX_READ_AHEAD_BLOCKS 2
#define CONFIGURE_BDBUF_CACHE_MEMORY_SIZE   (512)
#define CONFIGURE_BDBUF_BUFFER_MIN_SIZE     (512)
#define CONFIGURE_BDBUF_BUFFER_MAX_SIZE     (512)

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_INIT_TASK_STACK_SIZE      (4 * RTEMS_MINIMUM_STACK_SIZE)

#define CONFIGURE_EXTRA_TASK_STACKS         (13 * RTEMS_MINIMUM_STACK_SIZE)

#include <rtems/confdefs.h>

/* end of include file */
