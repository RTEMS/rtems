/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>

#include <bsp.h>
#include <rtems/error.h>
#include <stdio.h>

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
rtems_task subtask(rtems_task_argument arg);
rtems_task Task_variable_deleter(rtems_task_argument ignored);
void starttask(int arg);
void test_errors(void);
void test_dtor(void *pointer);
void test_multiple_taskvars(void);
void test_out_of_memory(void);
rtems_task Other_Task(rtems_task_argument ignored);
void test_delete_from_other_task(void);
void test_delete_as_side_effect(void);

volatile void *taskvar;
volatile int nRunning;
volatile int nDeleted;

rtems_task
subtask (rtems_task_argument arg)
{
  uintptr_t         localvar = arg;
  int               i;
  rtems_status_code sc;

  nRunning++;
  while (nRunning != 3)
    rtems_task_wake_after (0);

  sc = rtems_task_variable_add(RTEMS_SELF, (void **)&taskvar, NULL);
  directive_failed( sc, "task variable add" );

  taskvar = (void *)localvar;
  while (localvar < 1000) {
    localvar++;
    rtems_task_wake_after (0);
    taskvar = (void *)((uintptr_t)taskvar + 1);
    rtems_task_wake_after (0);
    if ((uintptr_t)taskvar != localvar) {
      printf(
        "Task:%" PRIdrtems_task_argument " taskvar:%" PRIuPTR
          " localvar:%" PRIuPTR "\n",
        arg,
        (uintptr_t)taskvar,
        localvar
      );
      rtems_task_suspend (RTEMS_SELF);
    }
  }
  sc = rtems_task_variable_delete(RTEMS_SELF, (void **)&taskvar);
  nDeleted++;
  directive_failed( sc, "task variable delete" );

  if ((uintptr_t)taskvar == localvar) {
    printf(
      "Task:%" PRIdrtems_task_argument " deleted taskvar:%" PRIuPTR
        " localvar:%" PRIuPTR "\n",
      arg,
      (uintptr_t)taskvar,
      localvar
    );
    nRunning--;
    rtems_task_suspend (RTEMS_SELF);
  }
  while (nDeleted != 3)
    rtems_task_wake_after (0);
  for (i = 0 ; i < 1000 ; i++) {
    taskvar = (void *)(localvar = 100 * arg);
    rtems_task_wake_after(0);
    if (nRunning <= 1)
      break;
    if ((uintptr_t)taskvar == localvar) {
      printf(
        "Task:%" PRIdrtems_task_argument " taskvar:%" PRIuPTR
          " localvar:%" PRIuPTR "\n",
       arg,
       (uintptr_t)taskvar,
       localvar
      );
      nRunning--;
      rtems_task_suspend(RTEMS_SELF);
    }
  }
  nRunning--;
  while (nRunning)
    rtems_task_wake_after(0);

  puts("*** END OF TEST 28 ***" );
  rtems_test_exit(0);
}

void
starttask (int arg)
{
  rtems_id tid;
  rtems_status_code sc;

  sc = rtems_task_create(rtems_build_name ('S', 'R', 'V', arg + 'A'),
    RTEMS_MAXIMUM_PRIORITY - 1u,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_PREEMPT|RTEMS_NO_TIMESLICE|RTEMS_NO_ASR|RTEMS_INTERRUPT_LEVEL(0),
    RTEMS_NO_FLOATING_POINT|RTEMS_LOCAL,
    &tid
  );
  directive_failed( sc, "task create" );

  sc = rtems_task_start(tid, subtask, arg);
  directive_failed( sc, "task start" );
}

volatile void *taskvar1;
volatile void *taskvar2;
volatile void *taskvar3;

void test_errors(void)
{
  rtems_status_code  sc;
  void              *value;

  /*
   *  task variable add error status codes
   */
  puts( "task variable add - NULL pointer - RTEMS_INVALID_ADDRESS" );
  sc = rtems_task_variable_add(RTEMS_SELF, NULL, NULL );
  fatal_directive_status( sc, RTEMS_INVALID_ADDRESS, "add NULL pointer" );

  /*
   *  task variable get error status codes
   */
  puts( "task variable get - bad Id - RTEMS_INVALID_ID" );
  sc = rtems_task_variable_get(
    rtems_task_self() + 10,
    (void **)&taskvar1,
    &value
  );
  fatal_directive_status( sc, RTEMS_INVALID_ID, "bad Id" );

  puts( "task variable get - NULL pointer - RTEMS_INVALID_ADDRESS" );
  sc = rtems_task_variable_get(RTEMS_SELF, NULL, &value );
  fatal_directive_status( sc, RTEMS_INVALID_ADDRESS, "get NULL pointer" );

  puts( "task variable get - bad result - RTEMS_INVALID_ADDRESS" );
  sc = rtems_task_variable_get(RTEMS_SELF, (void **)&taskvar1, NULL);
  fatal_directive_status( sc, RTEMS_INVALID_ADDRESS, "get bad result" );

  puts( "task variable get - bad pointer - RTEMS_INVALID_ADDRESS" );
  sc = rtems_task_variable_get(RTEMS_SELF, (void **)&taskvar1, &value);
  fatal_directive_status( sc, RTEMS_INVALID_ADDRESS, "get bad pointer" );

  /*
   *  task variable delete error status codes
   */
  puts( "task variable delete - bad Id - RTEMS_INVALID_ID" );
  sc = rtems_task_variable_delete( rtems_task_self() + 10, (void **)&taskvar1 );
  fatal_directive_status( sc, RTEMS_INVALID_ID, "bad Id" );

  puts( "task variable delete - NULL pointer - RTEMS_INVALID_ADDRESS" );
  sc = rtems_task_variable_delete(RTEMS_SELF, NULL);
  fatal_directive_status( sc, RTEMS_INVALID_ADDRESS, "delete NULL pointer" );

  puts( "task variable delete - bad pointer - RTEMS_INVALID_ADDRESS" );
  sc = rtems_task_variable_delete(RTEMS_SELF, (void **)&taskvar1);
  fatal_directive_status( sc, RTEMS_INVALID_ADDRESS, "delete bad pointer" );

}

volatile uint32_t test_dtor_ran;

void test_dtor(void *pointer)
{
  test_dtor_ran++;
}


void test_multiple_taskvars(void)
{
  rtems_status_code  sc;
  void              *value;

  test_dtor_ran = 0;

  /*
   *  Add multiple task variables and add each twice to
   *  verify that behavior is OK
   */
  puts( "task variable add - bad Id - RTEMS_INVALID_ID" );
  sc = rtems_task_variable_add(
    rtems_task_self() + 10,
    (void **)&taskvar1,
    NULL
  );
  fatal_directive_status( sc, RTEMS_INVALID_ID, "bad Id" );

  puts( "Adding multiple task variables" );
  sc = rtems_task_variable_add(RTEMS_SELF, (void **)&taskvar1, NULL);
  directive_failed( sc, "add multiple #1" );

  sc = rtems_task_variable_add(RTEMS_SELF, (void **)&taskvar1, test_dtor);
  directive_failed( sc, "add multiple #2" );

  sc = rtems_task_variable_add(RTEMS_SELF, (void **)&taskvar2, test_dtor);
  directive_failed( sc, "add multiple #3" );

  sc = rtems_task_variable_add(RTEMS_SELF, (void **)&taskvar2, NULL);
  directive_failed( sc, "add multiple #4" );

  sc = rtems_task_variable_add(RTEMS_SELF, (void **)&taskvar3, NULL);
  directive_failed( sc, "add multiple #5" );

  sc = rtems_task_variable_add(RTEMS_SELF, (void **)&taskvar3, test_dtor);
  directive_failed( sc, "add multiple #6" );

  /*
   *  Obtain task variables in various spots on the chain
   */
  puts( "Obtaining multiple task variables" );
  sc = rtems_task_variable_get( RTEMS_SELF, (void **)&taskvar3, &value );
  directive_failed( sc, "get multiple #1" );
  sc = rtems_task_variable_get( RTEMS_SELF, (void **)&taskvar2, &value );
  directive_failed( sc, "get multiple #2" );
  sc = rtems_task_variable_get( RTEMS_SELF, (void **)&taskvar1, &value );
  directive_failed( sc, "get multiple #2" );

  /*
   *  Delete task variables in various spots on the chain
   */

  /* to trip the destructors */
  taskvar1 = (void *)1;
  taskvar2 = (void *)2;
  taskvar3 = (void *)3;

  puts( "Deleting multiple task variables" );
  sc = rtems_task_variable_delete(RTEMS_SELF, (void **)&taskvar2);
  directive_failed( sc, "delete multiple #1" );
  sc = rtems_task_variable_delete(RTEMS_SELF, (void **)&taskvar3);
  directive_failed( sc, "delete multiple #2" );
  sc = rtems_task_variable_delete(RTEMS_SELF, (void **)&taskvar1);
  directive_failed( sc, "delete multiple #3" );

  if ( test_dtor_ran != 2 ) {
    printf(
      "Test dtor ran %" PRIu32 " times not 2 times as expected\n",
       test_dtor_ran
    );
    rtems_test_exit(0);
  }
}

#define MAX_VARS 4096

void *Pointers[MAX_VARS];

void test_out_of_memory(void)
{
  int                 i;
  int                 max;
  rtems_status_code   sc;
  int                 ran_out = 0;
  void              **base;

  base = Pointers;

  for (i=0 ; i<MAX_VARS ; i++ ) {
    sc = rtems_task_variable_add(RTEMS_SELF, &base[i], NULL);
    if ( sc == RTEMS_NO_MEMORY ) {
      puts( "task_variable_add - returns NO_MEMORY" );
      max = i;
      ran_out = 1;
      break;
    }
    directive_failed( sc, "add loop until out of memory" );
  }

  if ( !ran_out ) {
    puts( "ERROR!!! did not run out of memory adding task variables!" );
    rtems_test_exit(0);
  }

  for (i=0 ; i<max ; i++ ) {
    sc = rtems_task_variable_delete(RTEMS_SELF, &base[i]);
    directive_failed( sc, "delete loop until out of memory" );
  }
}

rtems_id main_task;
rtems_id other_task;

rtems_task Other_Task(rtems_task_argument ignored)
{
  rtems_status_code sc;

  puts( "Deleting task variables in another task" );
  sc = rtems_task_variable_delete(main_task, (void **)&taskvar1);
  directive_failed( sc, "delete loop for other task" );

  (void) rtems_task_delete( RTEMS_SELF );
}

void test_delete_from_other_task(void)
{
  rtems_status_code sc;

  test_dtor_ran = 0;

  sc = rtems_task_ident( RTEMS_SELF, 0, &main_task );
  directive_failed( sc, "task ident" );

  sc = rtems_task_variable_add(RTEMS_SELF, (void **)&taskvar1, test_dtor);
  directive_failed( sc, "add for other task case" );

  sc = rtems_task_create(rtems_build_name ('O', 'T', 'H', 'R'),
    RTEMS_MAXIMUM_PRIORITY - 1u,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_PREEMPT|RTEMS_NO_TIMESLICE|RTEMS_NO_ASR|RTEMS_INTERRUPT_LEVEL(0),
    RTEMS_NO_FLOATING_POINT|RTEMS_LOCAL,
    &other_task
  );
  directive_failed( sc, "task create other" );

  sc = rtems_task_start(other_task, Other_Task, 0);
  directive_failed( sc, "task start other" );

  rtems_task_wake_after( 100 );

  if ( test_dtor_ran != 1 ) {
    printf(
      "Test dtor ran %" PRIu32 " times not 1 times as expected\n",
      test_dtor_ran
    );
    rtems_test_exit(0);
  }
}

/*
 *  Task which adds task variables just to delete them
 */
rtems_task Task_variable_deleter(rtems_task_argument ignored)
{
  rtems_status_code sc;

  puts( "Adding multiple task variables to delete implicitly" );
  sc = rtems_task_variable_add(RTEMS_SELF, (void **)&taskvar1, test_dtor);
  directive_failed( sc, "add multiple for delete #1" );

  sc = rtems_task_variable_add(RTEMS_SELF, (void **)&taskvar2, NULL);
  directive_failed( sc, "add multiple for delete #2" );

  sc = rtems_task_variable_add(RTEMS_SELF, (void **)&taskvar3, test_dtor);
  directive_failed( sc, "add multiple for delete #3" );

  (void) rtems_task_delete( RTEMS_SELF );
}

void test_delete_as_side_effect(void)
{
  rtems_status_code sc;
  rtems_id          deleter_task;

  test_dtor_ran = 0;

  sc = rtems_task_create(rtems_build_name ('O', 'T', 'H', 'R'),
    RTEMS_MAXIMUM_PRIORITY - 1u,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_PREEMPT|RTEMS_NO_TIMESLICE|RTEMS_NO_ASR|RTEMS_INTERRUPT_LEVEL(0),
    RTEMS_NO_FLOATING_POINT|RTEMS_LOCAL,
    &deleter_task
  );
  directive_failed( sc, "task create deleter" );

  sc = rtems_task_start(deleter_task, Task_variable_deleter, 0);
  directive_failed( sc, "task start deleter" );

  rtems_task_wake_after( 100 );

  if ( test_dtor_ran != 2 ) {
    printf(
      "Test dtor ran %" PRIu32 " times not 2 times as expected\n",
      test_dtor_ran
    );
    rtems_test_exit(0);
  }
}

rtems_task Init (rtems_task_argument ignored)
{
  puts("*** START OF TEST 28 ***" );

  test_errors();

  test_multiple_taskvars();

  test_delete_as_side_effect();

  test_delete_from_other_task();

  starttask (1);
  starttask (2);
  starttask (3);

  test_out_of_memory();

  rtems_task_suspend (RTEMS_SELF);
}

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_MAXIMUM_USER_EXTENSIONS    2

#define CONFIGURE_MAXIMUM_TASKS              4
#define CONFIGURE_MAXIMUM_TASK_VARIABLES     (4)
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_MICROSECONDS_PER_TICK      10000

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

