/*
 *  Test for rtems_semaphore_flush
 *
 *  $Id$
 */

#include <bsp.h>

rtems_task Init (rtems_task_argument argument);

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_TEST_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_TEST_NEEDS_CLOCK_DRIVER

#define CONFIGURE_INIT

#include <confdefs.h>

#include <rtems/error.h>
#include <stdio.h>

rtems_interval ticksPerSecond;

rtems_task
subtask (rtems_task_argument arg)
{
	rtems_status_code sc;
	rtems_id sem = (rtems_id)arg;

	for (;;) {
		rtems_task_wake_after (ticksPerSecond * 2);

		sc = rtems_semaphore_release (sem);
		if (sc != RTEMS_SUCCESSFUL)
			printf ("%d: Can't release semaphore: %s\n", __LINE__, rtems_status_text (sc));
	}
}

void
startTask (rtems_id arg)
{
	rtems_id tid;
	rtems_status_code sc;

	sc = rtems_task_create (rtems_build_name ('S', 'R', 'V', 'A'),
		100,
		10000,
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

rtems_task Init (rtems_task_argument ignored)
{
	int i;
	rtems_id semrec, semnorec;
	rtems_status_code sc;
	rtems_interval then, now;

	sc = rtems_clock_get (RTEMS_CLOCK_GET_TICKS_PER_SECOND, &ticksPerSecond);
	if (sc != RTEMS_SUCCESSFUL) {
		printf ("Can't get ticks per second: %s\n", rtems_status_text (sc));
		exit (1);
	}
	sc = rtems_semaphore_create (rtems_build_name ('S', 'M', 'r', 'c'),
		1,
		RTEMS_PRIORITY|RTEMS_BINARY_SEMAPHORE|RTEMS_INHERIT_PRIORITY |RTEMS_NO_PRIORITY_CEILING|RTEMS_LOCAL,
		0,
		&semrec);
	if (sc != RTEMS_SUCCESSFUL) {
		printf ("%d: Can't create recursive-lock semaphore: %s\n", __LINE__, rtems_status_text (sc));
		exit (1);
	}
	sc = rtems_semaphore_create (rtems_build_name ('S', 'M', 'n', 'c'),
		1,
		RTEMS_PRIORITY|RTEMS_BINARY_SEMAPHORE|RTEMS_NO_NESTING_ALLOWED|RTEMS_INHERIT_PRIORITY |RTEMS_NO_PRIORITY_CEILING|RTEMS_LOCAL,
		0,
		&semnorec);
	if (sc != RTEMS_SUCCESSFUL) {
		printf ("%d: Can't create non-recursive-lock semaphore: %s\n", __LINE__, rtems_status_text (sc));
		exit (1);
	}

	sc = rtems_semaphore_obtain (semrec, RTEMS_NO_WAIT, 0);
	if (sc != RTEMS_SUCCESSFUL) {
		printf ("%d: Can't obtain recursive-lock semaphore: %s\n", __LINE__, rtems_status_text (sc));
	}
	sc = rtems_semaphore_obtain (semrec, RTEMS_NO_WAIT, 0);
	if (sc != RTEMS_SUCCESSFUL) {
		printf ("%d: Can't reobtain recursive-lock semaphore: %s\n", __LINE__, rtems_status_text (sc));
	}

	sc = rtems_semaphore_obtain (semnorec, RTEMS_NO_WAIT, 0);
	if (sc != RTEMS_SUCCESSFUL) {
		printf ("%d: Can't obtain non-recursive-lock semaphore: %s\n", __LINE__, rtems_status_text (sc));
	}
	sc = rtems_semaphore_obtain (semnorec, RTEMS_NO_WAIT, 0);
	if (sc == RTEMS_SUCCESSFUL) {
		printf ("%d: Reobtain non-recursive-lock semaphore -- and should not have.\n", __LINE__);
	}

	sc = rtems_semaphore_release (semnorec);
	if (sc != RTEMS_SUCCESSFUL) {
		printf ("%d: Can't release non-recursive-lock semaphore: %s\n", __LINE__, rtems_status_text (sc));
	}
	sc = rtems_semaphore_release (semnorec);
	if (sc != RTEMS_SUCCESSFUL) {
		printf ("%d: Can't rerelease non-recursive-lock semaphore: %s\n", __LINE__, rtems_status_text (sc));
	}
	sc = rtems_semaphore_obtain (semnorec, RTEMS_NO_WAIT, 0);
	if (sc != RTEMS_SUCCESSFUL) {
		printf ("%d: Can't obtain non-recursive-lock semaphore: %s\n", __LINE__, rtems_status_text (sc));
	}
	sc = rtems_semaphore_obtain (semnorec, RTEMS_NO_WAIT, 0);
	if (sc == RTEMS_SUCCESSFUL) {
		printf ("%d: Reobtain non-recursive-lock semaphore -- and should not have.\n", __LINE__);
	}
	else if (sc != RTEMS_UNSATISFIED) {
		printf ("%d: Reobtain non-recursive-lock semaphore failed, but error is %d (%s), not RTEMS_UNSATISFIED.\n", __LINE__, sc, rtems_status_text (sc));
	}

	sc = rtems_semaphore_release (semnorec);
	if (sc != RTEMS_SUCCESSFUL) {
		printf ("%d: Can't release non-recursive-lock semaphore: %s\n", __LINE__, rtems_status_text (sc));
	}
	sc = rtems_semaphore_release (semnorec);
	if (sc != RTEMS_SUCCESSFUL) {
		printf ("%d: Can't rerelease non-recursive-lock semaphore: %s\n", __LINE__, rtems_status_text (sc));
	}
	sc = rtems_semaphore_obtain (semnorec, RTEMS_NO_WAIT, 0);
	if (sc != RTEMS_SUCCESSFUL) {
		printf ("%d: Can't obtain non-recursive-lock semaphore: %s\n", __LINE__, rtems_status_text (sc));
	}
        /*
         *  Since this task is holding this, there is no reason to block.
         *  It is obviously an error to reobtain it.
         */
	rtems_clock_get (RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &then);
	sc = rtems_semaphore_obtain (semnorec, RTEMS_WAIT, 5);
	rtems_clock_get (RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &now);
	if (sc == RTEMS_SUCCESSFUL) {
		printf ("%d: Reobtain non-recursive-lock semaphore -- and should not have.\n", __LINE__);
	}
	else if (sc != RTEMS_UNSATISFIED) {
		printf ("%d: Reobtain non-recursive-lock semaphore failed, but error is %d (%s), not RTEMS_UNSATISFIED.\n", __LINE__, sc, rtems_status_text (sc));
	}
	if ((then - now) < 4)
		printf ("%d: Reobtain non-recursive-lock semaphore failed without timeout.\n", __LINE__);

	startTask (semnorec);
	rtems_clock_get (RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &then);
	for (i = 0 ; i < 5 ; i++) {
		int diff;

		sc = rtems_semaphore_obtain (semnorec, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
		rtems_clock_get (RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &now);
		diff = now - then;
		then = now;
		if (sc != RTEMS_SUCCESSFUL)
			printf ("%d: Failed to obtain non-recursive-lock semaphore: %s\n", __LINE__, rtems_status_text (sc));
		else if (diff < (2 * ticksPerSecond + 1))
			printf ("%d: Obtained obtain non-recursive-lock semaphore too quickly -- %d ticks\n", __LINE__, diff);
	}
		
	exit (0);
}
