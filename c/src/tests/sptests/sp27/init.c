/*
 *  Test for rtems_semaphore_flush
 *
 *  $Id$
 */

#include <bsp.h>

rtems_task Init (rtems_task_argument argument);

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS               6

#define CONFIGURE_MICROSECONDS_PER_TICK       52429

#define CONFIGURE_INIT

#include <confdefs.h>

#include <rtems/error.h>
#include <stdio.h>
#include <stdlib.h>

#define NTASK 4

rtems_id semaphore;
volatile int flags[NTASK];

rtems_task
subtask (rtems_task_argument arg)
{
  rtems_status_code sc;

  for (;;) {
    flags[arg]++;
    sc = rtems_semaphore_obtain (semaphore, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
    if (sc == RTEMS_SUCCESSFUL)
      puts ("Obtained semaphore -- and should not have done so!");
    else if (sc != RTEMS_UNSATISFIED)
      printf ("Can't get semaphore: %s\n", rtems_status_text (sc));
  }
}

void
starttask (int arg)
{
  rtems_id tid;
  rtems_status_code sc;
  rtems_task_priority priority;

  rtems_task_set_priority (RTEMS_SELF, RTEMS_CURRENT_PRIORITY, &priority);
  sc = rtems_task_create (rtems_build_name ('S', 'R', 'V', arg + 'A'),
    priority,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_PREEMPT|RTEMS_NO_TIMESLICE|RTEMS_NO_ASR|RTEMS_INTERRUPT_LEVEL(0),
    RTEMS_NO_FLOATING_POINT|RTEMS_LOCAL,
    &tid);
  if (sc != RTEMS_SUCCESSFUL) {
    printf ("Can't create task: %s\n", rtems_status_text (sc));
    rtems_task_suspend (RTEMS_SELF);
  }
  sc = rtems_task_start (tid, subtask, arg);
  if (sc != RTEMS_SUCCESSFUL) {
  	printf ("Can't start task: %s\n", rtems_status_text (sc));
    rtems_task_suspend (RTEMS_SELF);
  }
}

rtems_task
Init (rtems_task_argument ignored)
{
  int pass, i;
  rtems_status_code sc;

  puts("**** Semaphore flush test ****");
  sc = rtems_semaphore_create (
    rtems_build_name ('S', 'E', 'M', 'F'),
    0,
    RTEMS_LOCAL|RTEMS_BINARY_SEMAPHORE|RTEMS_NO_INHERIT_PRIORITY|RTEMS_NO_PRIORITY_CEILING|RTEMS_FIFO,
    0,
    &semaphore);
  if (sc != RTEMS_SUCCESSFUL) {
    printf ("Can't flush semaphore: %s\n", rtems_status_text (sc));
    exit (1);
  }
  for (i = 0 ; i < NTASK ; i++)
    starttask (i);
  for (pass = 1 ; pass < 10 ; pass++) {
    rtems_task_wake_after (1);
    for (i = 0 ; i < NTASK ; i++) {
      if (flags[i] != pass)
        printf ("flags[%d] = %d -- expected %d\n", i, flags[i], pass);
    }
    sc = rtems_semaphore_flush (semaphore);
    if (sc != RTEMS_SUCCESSFUL) {
      printf ("Can't flush semaphore: %s\n", rtems_status_text (sc));
      exit (1);
    }
  }
  puts ("**** Semaphore flush test succeeded ****");
  exit (1);
}
