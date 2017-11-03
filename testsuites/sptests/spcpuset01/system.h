/*
 *  COPYRIGHT (c) 1989-2013.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include "tmacros.h"
#include "test_support.h"

#include <sys/cpuset.h>

/* global variables */
extern cpu_set_t set1;
extern cpu_set_t set2;
extern cpu_set_t set3;

/* enable/disable verbose output */
#ifndef DPRINT
  /* #define DPRINT(...) printf(__VA_ARGS__) */
  #define DPRINT(...)
#endif

/* functions */

rtems_task Init(
  rtems_task_argument argument
);

void cpuset_logic_test(void);

/* configuration information */

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS            1
#define CONFIGURE_MAXIMUM_SEMAPHORES       1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#include <rtems/confdefs.h>


/*
 *  Keep the names and IDs in global variables so another task can use them.
 */


/*
 *  Handy macros and static inline functions
 */

/* end of include file */
