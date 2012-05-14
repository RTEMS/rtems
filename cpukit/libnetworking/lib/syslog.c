/*
 * RTEMS version of syslog and associated routines
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#include <unistd.h>

static int LogStatus = LOG_CONS;
static const char *LogTag = "syslog";
static int LogFacility = LOG_USER;
static int LogMask = 0xff;

static int LogFd = -1;
static rtems_id LogSemaphore;
extern struct in_addr rtems_bsdnet_log_host_address;

#define SYSLOG_PORT	514

void
syslog (int pri, const char *fmt, ...)
{
	va_list ap;

	va_start (ap, fmt);
	vsyslog (pri, fmt, ap);
	va_end (ap);
}

/*
 * FIXME: Should cbuf be static?  It could be if we put the mutex
 *        around the entire body of this routine.  Then we wouldn't
 *        have to worry about blowing stacks with a local variable
 *        that large.  Could make cbuf bigger, too.
 */
void
vsyslog (int pri, const char *fmt, va_list ap)
{
	int cnt;
	char *cp;
	char *msgp, cbuf[200];
	int sent;

	if (pri & ~(LOG_PRIMASK|LOG_FACMASK)) {
		syslog (LOG_ERR|LOG_CONS|LOG_PERROR|LOG_PID,
								"syslog: unknown facility/priority: %#x", pri);
		pri &= LOG_PRIMASK|LOG_FACMASK;
	}

	if (!(LOG_MASK(LOG_PRI(pri)) & LogMask))
		return;

	if ((pri & LOG_FACMASK) == 0)
		pri |= LogFacility;

	cnt = sprintf (cbuf, "<%d>", pri);
	cp = msgp = cbuf + cnt;
	if (LogTag) {
		const char *lp = LogTag;
		while ((*cp = *lp++) != '\0')
			cp++;
	}
	if (LogStatus & LOG_PID) {
		rtems_id tid;
		rtems_task_ident (RTEMS_SELF, 0, &tid);
		cnt = sprintf (cp, "[%#lx]", (unsigned long)tid);
		cp += cnt;
	}
	if (LogTag) {
		*cp++ = ':';
		*cp++ = ' ';
	}
	cnt = vsprintf (cp, fmt, ap);
	cnt += cp - cbuf;
	if (cbuf[cnt-1] == '\n')
		cbuf[--cnt] = '\0';

	if (LogStatus & LOG_PERROR)
		printf ("%s\n", cbuf);

	/*
	 * Grab the mutex
	 */
	sent = 0;
	if ((rtems_bsdnet_log_host_address.s_addr != INADDR_ANY)
	 && (LogFd >= 0)
	 && (rtems_semaphore_obtain (LogSemaphore, RTEMS_WAIT, RTEMS_NO_TIMEOUT) == RTEMS_SUCCESSFUL)) {
		/*
		 * Set the destination address/port
		 */
		struct sockaddr_in farAddress;
		farAddress.sin_family = AF_INET;
		farAddress.sin_port = htons (SYSLOG_PORT);
		farAddress.sin_addr = rtems_bsdnet_log_host_address;
		memset (farAddress.sin_zero, '\0', sizeof farAddress.sin_zero);

		/*
		 * Send the message
		 */
		if (sendto (LogFd, cbuf, cnt, 0, (struct sockaddr *)&farAddress, sizeof farAddress) >= 0)
			sent = 1;
		rtems_semaphore_release (LogSemaphore);
	}
	if (!sent && (LogStatus & LOG_CONS) && !(LogStatus & LOG_PERROR))
		printf ("%s\n", msgp);
}

void
openlog (const char *ident, int logstat, int logfac)
{
	rtems_status_code sc;
	struct sockaddr_in myAddress;

	if (ident != NULL)
		LogTag = ident;
	LogStatus = logstat;
	if (logfac != 0 && (logfac & ~LOG_FACMASK) == 0)
		LogFacility = logfac;

	/*
	 * Create the socket
	 */
	if ((LogFd = socket (AF_INET, SOCK_DGRAM, 0)) < 0) {
		printf ("Can't create syslog socket: %d\n", errno);
		return;
	}

	/*
	 * Bind socket to name
	 */
	myAddress.sin_family = AF_INET;
	myAddress.sin_addr.s_addr = INADDR_ANY;
	myAddress.sin_port = 0;
	memset (myAddress.sin_zero, '\0', sizeof myAddress.sin_zero);
	if (bind (LogFd, (struct sockaddr *)&myAddress, sizeof (myAddress)) < 0) {
		close (LogFd);
		LogFd = -1;
		printf ("Can't bind syslog socket: %d\n", errno);
		return;
	}

	/*
	 * Create the mutex
	 */
	sc = rtems_semaphore_create (rtems_build_name('s', 'L', 'o', 'g'),
					1,
					RTEMS_PRIORITY |
						RTEMS_BINARY_SEMAPHORE |
						RTEMS_INHERIT_PRIORITY |
						RTEMS_NO_PRIORITY_CEILING |
						RTEMS_LOCAL,
					0,
					&LogSemaphore);
	if (sc != RTEMS_SUCCESSFUL) {
		printf ("Can't create syslog semaphore: %d\n", sc);
		close (LogFd);
		LogFd = -1;
	}
}

void
closelog(void)
{
	if (LogFd >= 0) {
		close (LogFd);
		LogFd = -1;
		rtems_semaphore_delete (LogSemaphore);
	}
}

int
setlogmask (int pmask)
{
	int omask;

	omask = LogMask;
	if (pmask != 0)
		LogMask = pmask;
	return (omask);
}
