/* 
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include "tmacros.h"
#include "test_support.h"

/* functions */

rtems_task Init(
  rtems_task_argument argument
);

rtems_task Test_task(
  rtems_task_argument argument
);

/* configuration information */

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_SMP_APPLICATION
#define CONFIGURE_SMP_MAXIMUM_PROCESSORS   4 

#define CONFIGURE_MAXIMUM_TASKS            \
    (1 + CONFIGURE_SMP_MAXIMUM_PROCESSORS)

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_INIT_TASK_STACK_SIZE \
    (3 * CONFIGURE_MINIMUM_TASK_STACK_SIZE)

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_MAXIMUM_SEMAPHORES          1

#include <rtems/confdefs.h>


/* global variables */

/*
 *  Keep the names and IDs in global variables so another task can use them.
 */

typedef struct {
  bool     IsLocked;
  int      cpu_num;
  uint32_t task_index;
} Log_t;

#define           LOG_SIZE   20

TEST_EXTERN rtems_id   Semaphore;      /* synchronisation semaphore */ 
TEST_EXTERN Log_t      Log[LOG_SIZE];  /* A log of locks/unlocks */
TEST_EXTERN volatile uint32_t   Log_index;      /* Index into log */

/*
 *  Handy macros and static inline functions
 */

/*
 *  Macro to hide the ugliness of printing the time.
 */

/* end of include file */
