/*
 * RTEMS configuration/initialization
 * 
 * This program may be distributed and used for any purpose.
 * I ask only that you:
 *	1. Leave this author information intact.
 *	2. Document any changes you make.
 *
 * W. Eric Norum
 * Saskatchewan Accelerator Laboratory
 * University of Saskatchewan
 * Saskatoon, Saskatchewan, CANADA
 * eric@skatter.usask.ca
 */

#include <bsp.h>

#define CONFIGURE_TEST_NEEDS_CLOCK_DRIVER
#define CONFIGURE_TEST_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_MAXIMUM_SEMAPHORES		20
#define CONFIGURE_MAXIMUM_TIMERS		5
#define CONFIGURE_MAXIMUM_PERIODS		1

#define CONFIGURE_MICROSECONDS_PER_TICK	        1000

#define CONFIGURE_INIT
rtems_task Init (rtems_task_argument argument);

#include <confdefs.h>

#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>
#include <string.h>

/*
 * Test raw (ICANON=0) input
 */
static void
testRawInput (int vmin, int vtime)
{
	int i;
	struct termios old, new;
	rtems_interval ticksPerSecond, then, now;
	unsigned int msec;
	unsigned long count;
	int nread;
	unsigned char cbuf[100];

	printf ("*** Raw input  VMIN=%d  VTIME=%d ***\n", vmin, vtime);
	rtems_clock_get (RTEMS_CLOCK_GET_TICKS_PER_SECOND, &ticksPerSecond);
	i = tcgetattr (fileno (stdin), &old);
	if (i < 0) {
		printf ("tcgetattr failed: %s\n", strerror (errno));
		return;
	}
	new = old;
	new.c_lflag &= ~(ICANON|ECHO|ECHONL|ECHOK|ECHOE|ECHOPRT|ECHOCTL);
	new.c_cc[VMIN] = vmin;
	new.c_cc[VTIME] = vtime;
	i = tcsetattr (fileno (stdin), TCSANOW, &new);
	if (i < 0) {
		printf ("tcsetattr failed: %s\n", strerror (errno));
		return;
	}
	do {
		rtems_clock_get (RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &then);
		count = 0;
		for (;;) {
			nread = read (fileno (stdin), cbuf, sizeof cbuf);
			if (nread < 0) {
				printf ("Read error: %s\n", strerror (errno));
				goto out;
			}
			count++;
			if (nread != 0)
				break;
		}
		rtems_clock_get (RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &now);
		msec = (now - then) * 1000 / ticksPerSecond;
		printf ("Count:%-10lu  Interval:%3u.%3.3d  Char:",
					count, msec / 1000, msec % 1000);
		for (i = 0 ; i < nread ; i++)
			printf (" %2.2x", cbuf[i]);
		printf ("\n");
	} while (cbuf[0] != 'q');
    out:
	i = tcsetattr (fileno (stdin), TCSANOW, &old);
	if (i < 0)
		printf ("tcsetattr failed: %s\n", strerror (errno));
	printf ("*** End of Raw input  VMIN=%d  VTIME=%d ***\n", vmin, vtime);
}

/*
 * RTEMS Startup Task
 */
rtems_task
Init (rtems_task_argument ignored)
{
	int i, j;

	printf( "\n\n*** HELLO WORLD TEST ***\n" );
	printf( "Hello World\n" );
	printf( "*** END OF HELLO WORLD TEST ***\n" );

        printf( "\n\ntype 'q' to exit raw input tests\n\n" );

	for (;;) {
		/*
		 * Test  blocking, line-oriented input
		 */
		do {
			printf (">>> ");
			fflush (stdout);
			i = scanf (" %d", &j);
			printf ("Return: %d Value: %d\n", i, j);
		} while (i != 0);

		/*
		 * Consume what scanf rejected
		 */
		while ((i = getchar ()) != '\n')
			if (i == EOF)
				break;

		/*
		 * Test character-oriented input
		 */
		testRawInput (0, 0);
		testRawInput (0, 20);
		testRawInput (5, 0);
		testRawInput (5, 20);
	}
	exit (1);
}
