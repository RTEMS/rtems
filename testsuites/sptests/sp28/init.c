#define TEST_INIT
#include <tmacros.h>

rtems_task Init(rtems_task_argument argument);

#define CONFIGURE_TEST_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_TEST_NEEDS_CLOCK_DRIVER
#define CONFIGURE_MAXIMUM_USER_EXTENSIONS    2

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_MICROSECONDS_PER_TICK       52429

#define CONFIGURE_MICROSECONDS_INIT

#include <confdefs.h>

#include <bsp.h>
#include <rtems/error.h>
#include <rtems/rtems/task_variable.h>
#include <stdio.h>

volatile int taskvar;

rtems_task
subtask (rtems_task_argument arg)
{
	int localvar = arg;
	int i;
	rtems_status_code sc;

	sc = rtems_task_variable_add (RTEMS_SELF, (void *) &taskvar);
	if (sc != RTEMS_SUCCESSFUL) {
		printf ("Can't add task variable: %s\n", rtems_status_text (sc));
		rtems_task_suspend (RTEMS_SELF);
	}
	taskvar = localvar;
	while (localvar < 1000) {
		localvar++;
		rtems_task_wake_after (0);
		taskvar++;
		rtems_task_wake_after (0);
		if (taskvar != localvar) {
			printf ("Task:%d taskvar:%d localvar:%d\n", arg, taskvar, localvar);
			rtems_task_suspend (RTEMS_SELF);
		}
	}
	sc = rtems_task_variable_delete (RTEMS_SELF, (void *) &taskvar);
	if (sc != RTEMS_SUCCESSFUL) {
		printf ("Can't delete task variable: %s\n", rtems_status_text (sc));
		rtems_task_suspend (RTEMS_SELF);
	}
	for (i = 0 ; ; i++) {
		taskvar = localvar = 100 * arg;
		rtems_task_wake_after (0);
		if (taskvar == localvar) {
			printf ("Task:%d taskvar:%d localvar:%d\n", arg, taskvar, localvar);
			rtems_task_suspend (RTEMS_SELF);
		}
		if ((arg == 3) && (i == 100)) {
			printf ("Task variables test succeeded.\n");
			exit (0);
		}
	}
}

void
starttask (int arg)
{
	rtems_id tid;
	rtems_status_code sc;

	sc = rtems_task_create (rtems_build_name ('S', 'R', 'V', arg + 'A'),
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

rtems_task
Init (rtems_task_argument ignored)
{
	starttask (1);
	starttask (2);
	starttask (3);
	rtems_task_suspend (RTEMS_SELF);
}
