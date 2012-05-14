/*  system.h
 *
 *  This include file contains information that is included in every
 *  function in the test set.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

/* functions */

#include <pmacros.h>
#include <unistd.h>
#include <errno.h>
#include <sched.h>

void *POSIX_Init (
  void *arg
);

void *task_a(
  void *arg
);

void *task_b(
  void *arg
);

void *task_c(
  void *arg
);


/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_MAXIMUM_POSIX_THREADS             5
#define CONFIGURE_MAXIMUM_POSIX_TIMERS              5
#define CONFIGURE_MAXIMUM_TIMERS                    4
#define CONFIGURE_MAXIMUM_POSIX_MUTEXES             2
#define CONFIGURE_MAXIMUM_POSIX_CONDITION_VARIABLES 2


#include <rtems/confdefs.h>

/* global variables */

#ifdef CONFIGURE_INIT
#define TEST_EXTERN
#else
#define TEST_EXTERN extern
#endif

TEST_EXTERN pthread_t        Init_id;
TEST_EXTERN pthread_t        Task_id;

/* end of include file */
