/*  Test1
 *
 *  This test uses creates a number of tasks so the capture engine
 *  can show a trace.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "system.h"
#include <stdio.h>
#include <stdlib.h>

#include <rtems.h>
#include <rtems/monitor.h>

static volatile int capture_CT1a_deleted;
static volatile int capture_CT1b_deleted;
static volatile int capture_CT1c_deleted;

static void
capture_wait (uint32_t period)
{
  rtems_task_wake_after (RTEMS_MICROSECONDS_TO_TICKS (period * 1000));
}

/*
 * CT1a: Claim the mutex and then wait a while then wake
 *       up and release the mutex. While this task waits with
 *       the mutex another higher priority task is started that
 *       just loops using all the processing time. It is not until
 *       another even higher priority thread blocks on the mutex
 *       does this task get raised to that priority and so
 *       releases the mutex. This will allow us to capture the
 *       action of priority inversion.
 */
static void
capture_CT1a (rtems_task_argument arg)
{
  rtems_id mutex = (rtems_id) arg;
  rtems_status_code sc;

  sc = rtems_semaphore_obtain (mutex, RTEMS_WAIT, 0);

  if (sc != RTEMS_SUCCESSFUL)
    printf ("error: CT1a: mutex obtain: %s\n", rtems_status_text (sc));

  capture_wait (2500);

  sc = rtems_semaphore_release (mutex);

  if (sc != RTEMS_SUCCESSFUL)
    printf ("error: CT1a: mutex release: %s\n", rtems_status_text (sc));

  capture_CT1a_deleted = 1;

  rtems_task_exit();
}

static void
capture_CT1b (rtems_task_argument arg)
{
  volatile int i;

  while (!capture_CT1c_deleted)
    i++;

  capture_CT1b_deleted = 1;

  rtems_task_exit();
}

static void
capture_CT1c (rtems_task_argument arg)
{
  rtems_id          mutex = (rtems_id) arg;
  rtems_status_code sc;

  sc = rtems_semaphore_obtain (mutex, RTEMS_WAIT, 0);

  if (sc != RTEMS_SUCCESSFUL)
    printf ("error: CT1c: mutex obtain: %s\n", rtems_status_text (sc));

  capture_wait (500);

  sc = rtems_semaphore_release (mutex);

  if (sc != RTEMS_SUCCESSFUL)
    printf ("error: CT1c: mutex release: %s\n", rtems_status_text (sc));

  capture_CT1c_deleted = 1;

  rtems_task_exit();
}

static void
capture_test_1 (int                                argc,
                char**                             argv,
                const rtems_monitor_command_arg_t* command_arg,
                bool                               verbose)
{
  rtems_status_code sc;
  rtems_name        name;
  rtems_id          id[3];
  rtems_id          mutex;
  int               loops;

  capture_CT1a_deleted = 0;
  capture_CT1b_deleted = 0;
  capture_CT1c_deleted = 0;

  name = rtems_build_name('C', 'T', 'm', '1');

  sc = rtems_semaphore_create (name, 1,
                               RTEMS_PRIORITY | RTEMS_BINARY_SEMAPHORE |
                               RTEMS_INHERIT_PRIORITY,
                               0, &mutex);

  if (sc != RTEMS_SUCCESSFUL)
  {
    printf ("error: Test 1: cannot mutex: %s\n", rtems_status_text (sc));
    return;
  }

  name = rtems_build_name('C', 'T', '1', 'a');

  sc = rtems_task_create (name, 102, 2 * 1024,
                          RTEMS_NO_FLOATING_POINT | RTEMS_LOCAL,
                          RTEMS_PREEMPT | RTEMS_TIMESLICE | RTEMS_NO_ASR,
                          &id[0]);

  if (sc != RTEMS_SUCCESSFUL)
  {
    printf ("error: Test 1: cannot create CT1a: %s\n", rtems_status_text (sc));
    rtems_semaphore_delete (mutex);
    return;
  }

  sc = rtems_task_start (id[0], capture_CT1a, (rtems_task_argument) mutex);

  if (sc != RTEMS_SUCCESSFUL)
  {
    printf ("error: Test 1: cannot start CT1a: %s\n", rtems_status_text (sc));
    rtems_task_exit();
    rtems_semaphore_delete (mutex);
    return;
  }

  capture_wait (1000);

  name = rtems_build_name('C', 'T', '1', 'b');

  sc = rtems_task_create (name, 101, 2 * 1024,
                          RTEMS_NO_FLOATING_POINT | RTEMS_LOCAL,
                          RTEMS_PREEMPT | RTEMS_TIMESLICE | RTEMS_NO_ASR,
                          &id[1]);

  if (sc != RTEMS_SUCCESSFUL)
  {
    printf ("error: Test 1: cannot create CT1b: %s\n", rtems_status_text (sc));
    rtems_task_exit();
    rtems_semaphore_delete (mutex);
    return;
  }

  sc = rtems_task_start (id[1], capture_CT1b, 0);

  if (sc != RTEMS_SUCCESSFUL)
  {
    printf ("error: Test 1: cannot start CT1b: %s\n", rtems_status_text (sc));
    rtems_task_exit();
    rtems_task_exit();
    rtems_semaphore_delete (mutex);
    return;
  }

  capture_wait (1000);

  name = rtems_build_name('C', 'T', '1', 'c');

  sc = rtems_task_create (name, 100, 2 * 1024,
                          RTEMS_NO_FLOATING_POINT | RTEMS_LOCAL,
                          RTEMS_PREEMPT | RTEMS_TIMESLICE | RTEMS_NO_ASR,
                          &id[2]);

  if (sc != RTEMS_SUCCESSFUL)
  {
    printf ("error: Test 1: cannot create CT1c: %s\n", rtems_status_text (sc));
    rtems_task_exit();
    rtems_task_exit();
    rtems_semaphore_delete (mutex);
    return;
  }

  sc = rtems_task_start (id[2], capture_CT1c, (rtems_task_argument) mutex);

  if (sc != RTEMS_SUCCESSFUL)
  {
    printf ("error: Test 1: cannot start CT1c: %s\n", rtems_status_text (sc));
    rtems_task_exit();
    rtems_task_exit();
    rtems_task_exit();
    rtems_semaphore_delete (mutex);
    return;
  }

  loops = 15;

  while (!(capture_CT1a_deleted || capture_CT1b_deleted ||
           capture_CT1c_deleted) && loops)
  {
    loops--;
    capture_wait (1000);
  }

  if (!loops)
  {
    printf ("error: Test 1: test tasks did not delete\n");
    rtems_task_exit();
    rtems_task_exit();
    rtems_task_exit();
  }

  sc = rtems_semaphore_delete (mutex);
  if (sc != RTEMS_SUCCESSFUL)
    printf ("error: Test 1: deleting the mutex: %s\n", rtems_status_text (sc));
}

static rtems_monitor_command_entry_t capture_cmds[] =
{
  {
    "test1",
    "usage: \n",
    0,
    capture_test_1,
    { 0 },
    0
  }
};

void setup_tasks_to_watch (void)
{
  size_t cmd;
  for (cmd = 0;
       cmd < sizeof (capture_cmds) / sizeof (rtems_monitor_command_entry_t);
       cmd++)
      rtems_monitor_insert_cmd (&capture_cmds[cmd]);
}
