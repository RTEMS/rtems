/*
 *  $Id$
 */

#define TEST_INIT
#include <tmacros.h>

rtems_task Init(rtems_task_argument argument);

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_MAXIMUM_USER_EXTENSIONS    2

#define CONFIGURE_MAXIMUM_TASKS              4
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_MICROSECONDS_PER_TICK       52429

#define CONFIGURE_MICROSECONDS_INIT

#include <confdefs.h>

#include <bsp.h>
#include <rtems/error.h>
#include <stdio.h>

volatile void *taskvar;
volatile int nRunning;
volatile int nDeleted;

rtems_task
subtask (rtems_task_argument arg)
{
	int localvar = arg;
	int i;
	rtems_status_code sc;

	nRunning++;
	while (nRunning != 3)
		rtems_task_wake_after (0);
	sc = rtems_task_variable_add (RTEMS_SELF, &taskvar, NULL);
	if (sc != RTEMS_SUCCESSFUL) {
		printf ("Can't add task variable: %s\n", rtems_status_text (sc));
		rtems_task_suspend (RTEMS_SELF);
	}
	taskvar = (void *)localvar;
	while (localvar < 1000) {
		localvar++;
		rtems_task_wake_after (0);
		taskvar = (void *)((int)taskvar + 1);
		rtems_task_wake_after (0);
		if ((int)taskvar != localvar) {
			printf ("Task:%d taskvar:%d localvar:%d\n", arg, (int)taskvar, localvar);
			rtems_task_suspend (RTEMS_SELF);
		}
	}
	sc = rtems_task_variable_delete (RTEMS_SELF, &taskvar);
	nDeleted++;
	if (sc != RTEMS_SUCCESSFUL) {
		printf ("Can't delete task variable: %s\n", rtems_status_text (sc));
		nRunning--;
		rtems_task_suspend (RTEMS_SELF);
	}
	if ((int)taskvar == localvar) {
		printf ("Task:%d deleted taskvar:%d localvar:%d\n", arg, (int)taskvar, localvar);
		nRunning--;
		rtems_task_suspend (RTEMS_SELF);
	}
	while (nDeleted != 3)
		rtems_task_wake_after (0);
	for (i = 0 ; i < 1000 ; i++) {
		taskvar = (void *)(localvar = 100 * arg);
		rtems_task_wake_after (0);
		if (nRunning <= 1)
			break;
		if ((int)taskvar == localvar) {
			printf ("Task:%d taskvar:%d localvar:%d\n", arg, (int)taskvar, localvar);
			nRunning--;
			rtems_task_suspend (RTEMS_SELF);
		}
	}
	nRunning--;
	while (nRunning)
		rtems_task_wake_after (0);
	printf ("Task variables test complete.\n");
	puts ("*** END OF TEST SP28 ***" );
	exit (0);
}

void
starttask (int arg)
{
	rtems_id tid;
	rtems_status_code sc;

	sc = rtems_task_create (rtems_build_name ('S', 'R', 'V', arg + 'A'),
		100,
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

	puts ("*** START OF TEST SP28 ***" );
	puts ("Task variables test begins.  Any output between\n");
	puts ("this line and the `Task variables test complete' line indicates an error.\n");
	starttask (1);
	starttask (2);
	starttask (3);
	rtems_task_suspend (RTEMS_SELF);
}
