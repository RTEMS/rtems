/*
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>
#include <bsp.h>

rtems_task Init (rtems_task_argument argument);

#define CONFIGURE_EXTRA_TASK_STACKS RTEMS_MINIMUM_STACK_SIZE

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_MAXIMUM_TASKS             2
#define CONFIGURE_MAXIMUM_SEMAPHORES        2

#define CONFIGURE_INIT

#include <rtems/confdefs.h>

#include <rtems/error.h>
#include <stdio.h>
#include <stdlib.h>

#include "tmacros.h"

rtems_interval ticksPerSecond;
rtems_task subtask(rtems_task_argument arg);
void startTask(rtems_id arg);

rtems_task
subtask (rtems_task_argument arg)
{
  rtems_status_code sc;
  rtems_id          sem = (rtems_id)arg;

  for (;;) {
    rtems_task_wake_after (ticksPerSecond * 2);

    sc = rtems_semaphore_release (sem);
    if (sc != RTEMS_SUCCESSFUL) {
      printf(
        "%d: Can't release semaphore: %s\n", __LINE__,
        rtems_status_text (sc)
      );
    }
  }
}

void
startTask (rtems_id arg)
{
  rtems_id          tid;
  rtems_status_code sc;

  sc = rtems_task_create (rtems_build_name ('S', 'R', 'V', 'A'),
    RTEMS_MAXIMUM_PRIORITY - 1u,
    RTEMS_MINIMUM_STACK_SIZE * 2,
    RTEMS_PREEMPT|RTEMS_NO_TIMESLICE|RTEMS_NO_ASR|RTEMS_INTERRUPT_LEVEL(0),
    RTEMS_NO_FLOATING_POINT|RTEMS_LOCAL,
    &tid
  );
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

rtems_task Init (rtems_task_argument ignored)
{
  int i;
  rtems_id semrec, semnorec;
  rtems_status_code sc;
  rtems_interval then, now;

  puts( "*** SP29 - SIMPLE SEMAPHORE TEST ***" );
  puts( "This test only prints on errors." );

  ticksPerSecond = rtems_clock_get_ticks_per_second();
  if (ticksPerSecond <= 0) {
    printf(
      "Invalid ticks per second: %" PRIdrtems_interval "\n",
      ticksPerSecond
    );
    exit (1);
  }
  sc = rtems_semaphore_create (rtems_build_name ('S', 'M', 'r', 'c'),
    1,
    RTEMS_PRIORITY|RTEMS_BINARY_SEMAPHORE|RTEMS_INHERIT_PRIORITY| \
      RTEMS_NO_PRIORITY_CEILING|RTEMS_LOCAL,
    0,
    &semrec
  );
  directive_failed( sc, "create recursive lock" );

  sc = rtems_semaphore_create (rtems_build_name ('S', 'M', 'n', 'c'),
    1,
    RTEMS_PRIORITY|RTEMS_SIMPLE_BINARY_SEMAPHORE|RTEMS_NO_INHERIT_PRIORITY| \
      RTEMS_NO_PRIORITY_CEILING|RTEMS_LOCAL,
    0,
    &semnorec
  );
  directive_failed( sc, "create non-recursive lock" );

  sc = rtems_semaphore_obtain (semrec, RTEMS_NO_WAIT, 0);
  directive_failed( sc, "obtain recursive lock" );

  sc = rtems_semaphore_obtain (semrec, RTEMS_NO_WAIT, 0);
  directive_failed( sc, "reobtain recursive lock" );

  sc = rtems_semaphore_obtain (semnorec, RTEMS_NO_WAIT, 0);
  directive_failed( sc, "reobtain recursive lock" );

  sc = rtems_semaphore_obtain (semnorec, RTEMS_NO_WAIT, 0);
  if (sc == RTEMS_SUCCESSFUL) {
    printf(
      "%d: Reobtain non-recursive-lock semaphore -- and should not have.\n",
      __LINE__
    );
    rtems_test_exit(1);
  }

  sc = rtems_semaphore_release (semnorec);
  directive_failed( sc, "release non-recursive lock" );

  sc = rtems_semaphore_release (semnorec);
  directive_failed( sc, "re-release non-recursive lock" );

  sc = rtems_semaphore_obtain (semnorec, RTEMS_NO_WAIT, 0);
  directive_failed( sc, "obtain non-recursive lock" );

  sc = rtems_semaphore_obtain (semnorec, RTEMS_NO_WAIT, 0);
  if (sc == RTEMS_SUCCESSFUL) {
    printf(
      "%d: Reobtain non-recursive-lock semaphore -- and should not have.\n",
      __LINE__
    );
  } else if (sc != RTEMS_UNSATISFIED) {
    printf(
      "%d: Reobtain non-recursive-lock semaphore failed, but error is "
        "%d (%s), not RTEMS_UNSATISFIED.\n",
      __LINE__,
      sc,
      rtems_status_text (sc)
    );
  }

  sc = rtems_semaphore_release (semnorec);
  directive_failed( sc, "release non-recursive lock" );

  sc = rtems_semaphore_release (semnorec);
  directive_failed( sc, "rerelease non-recursive lock" );

  sc = rtems_semaphore_obtain (semnorec, RTEMS_NO_WAIT, 0);
  directive_failed( sc, "obtain non-recursive lock" );

  /*
   *  Since this task is holding this, this task will block and timeout.
   *  Then the timeout error will be returned.
   */
  then = rtems_clock_get_ticks_since_boot();
  sc = rtems_semaphore_obtain (semnorec, RTEMS_WAIT, 5);
  now = rtems_clock_get_ticks_since_boot();
  if (sc == RTEMS_SUCCESSFUL) {
    printf(
      "%d: Reobtain non-recursive-lock semaphore -- and should not have.\n",
      __LINE__
    );
  } else if (sc != RTEMS_TIMEOUT) {
    printf(
      "%d: Reobtain non-recursive-lock semaphore failed, but error is "
        "%d (%s), not RTEMS_TIMEOUT.\n",
      __LINE__,
      sc,
      rtems_status_text (sc)
    );
  }
  if ((then - now) < 4) {
    printf(
      "%d: Reobtain non-recursive-lock semaphore failed without timeout.\n",
      __LINE__
    );
  }

  startTask (semnorec);
  then = rtems_clock_get_ticks_since_boot();
  for (i = 0 ; i < 5 ; i++) {
    rtems_interval diff;

    sc = rtems_semaphore_obtain(
      semnorec,
      RTEMS_WAIT,
      RTEMS_NO_TIMEOUT
    );
    now = rtems_clock_get_ticks_since_boot();
    diff = (now - then);
    then = now;
    if (sc != RTEMS_SUCCESSFUL) {
      printf(
        "%d: Failed to obtain non-recursive-lock semaphore: %s\n",
        __LINE__,
        rtems_status_text (sc)
      );
    } else if (diff < (int) (2 * ticksPerSecond)) {
      printf(
        "%d: Obtained obtain non-recursive-lock semaphore too quickly -- %"
          PRIdrtems_interval " ticks not %" PRIdrtems_interval " ticks\n",
        __LINE__,
        diff,
        (2 * ticksPerSecond)
      );
    }
  }

  puts( "*** END OF TEST 29 ***" );
  rtems_test_exit (0);
}
