/*  system.h
 *
 *  This include file contains information that is included in every
 *  function in the test set.
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems.h>
#include <rtems/test.h>

/* functions */

rtems_task Init(
  rtems_task_argument argument
);

rtems_task test_task(
  rtems_task_argument my_number
);

void
destory_all_tasks(
  const char *who
);

bool status_code_bad(
  rtems_status_code status_code
);

extern void setup_tasks_to_watch(void);

/* configuration information */

#include <bsp.h> /* for device driver prototypes */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT

#define CONFIGURE_UNIFIED_WORK_AREAS
#define CONFIGURE_UNLIMITED_OBJECTS

#define CONFIGURE_MAXIMUM_USER_EXTENSIONS (5)

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#include <rtems/confdefs.h>

/*
 * Keep track of the task id's created, use a large array.
 */

#define MAX_TASKS         (1000)
#define TASK_INDEX_OFFSET (1)

extern rtems_id task_id[MAX_TASKS];

/*
 * Increment the task name.
 */

#define NEXT_TASK_NAME(c1, c2, c3, c4)  \
                 if (c4 == '9') {       \
                   if (c3 == '9') {     \
                     if (c2 == 'z') {   \
                       if (c1 == 'z') { \
                         printf("not enough task letters for names !!!\n"); \
                         exit( 1 );     \
                       } else           \
                         c1++;          \
                       c2 = 'a';        \
                     } else             \
                      c2++;             \
                     c3 = '0';          \
                   } else               \
                     c3++;              \
                   c4 = '0';            \
                 }                      \
                 else                   \
                   c4++                 \


/* end of include file */
