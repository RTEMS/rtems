/*
 * Copyright 2010, Alin Rus <alin.codejunkie@gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

/* functions */

#include <pmacros.h>
#include <pthread.h>
#include <errno.h>
#include <sched.h>

void *POSIX_Init (void *argument);

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM

#define CONFIGURE_MAXIMUM_TASKS             20
#define CONFIGURE_MAXIMUM_SEMAPHORES        20
#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES    20
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 20

#define CONFIGURE_MAXIMUM_POSIX_THREADS        10
#define CONFIGURE_MAXIMUM_POSIX_MUTEXES        10
#define CONFIGURE_MAXIMUM_POSIX_CONDITION_VARIABLES     10
#define CONFIGURE_MAXIMUM_POSIX_KEYS           10

#define CONFIGURE_POSIX_INIT_THREAD_TABLE
#define CONFIGURE_EXTRA_TASK_STACKS         (10 * RTEMS_MINIMUM_STACK_SIZE)
#define CONFIGURE_POSIX_INIT_THREAD_STACK_SIZE (10 * RTEMS_MINIMUM_STACK_SIZE)


#define CONFIGURE_MALLOC_STATISTICS


#include <rtems/confdefs.h>

/* global variables */
TEST_EXTERN pthread_t Init_id;

/* end of include file */
