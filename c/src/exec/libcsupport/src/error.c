/*
 *  report errors and panics to RTEMS' stderr.
 *  Currently just used by RTEMS monitor.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

/*
 * These routines provide general purpose error reporting.
 * rtems_error reports an error to stderr and allows use of
 * printf style formatting.  A newline is appended to all messages.
 * 
 * error_flag can be specified as any of the following:
 *
 *  	RTEMS_ERROR_ERRNO       -- include errno text in output
 *  	RTEMS_ERROR_PANIC       -- halts local system after output
 *  	RTEMS_ERROR_ABORT       -- abort after output
 *
 * It can also include a rtems_status value which can be OR'd
 * with the above flags. * 
 *
 * EXAMPLE
 *	#include <rtems.h>
 *	#include <rtems/error.h>
 *	rtems_error(0, "stray interrupt %d", intr);
 *
 * EXAMPLE
 *        if ((status = rtems_task_create(...)) != RTEMS_SUCCCESSFUL)
 *        {
 *            rtems_error(status | RTEMS_ERROR_ABORT,
 *                        "could not create task");
 *        }
 *
 * EXAMPLE
 *        if ((fd = open(pathname, O_RDNLY)) < 0)
 *        {
 *            rtems_error(RTEMS_ERROR_ERRNO, "open of '%s' failed", pathname);
 *            goto failed;
 *        }
 */

#define __RTEMS_VIOLATE_KERNEL_VISIBILITY__
#include <rtems.h>

#include <rtems/error.h>
#include <rtems/assoc.h>

#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>		/* _exit() */

/* bug in hpux <errno.h>: no prototypes unless you are C++ */
#ifdef hpux9
char *strerror(int);
#endif

extern char *rtems_progname;
int          rtems_panic_in_progress;

rtems_assoc_t rtems_status_assoc[] = {
    { "successful completion",              RTEMS_SUCCESSFUL, },
    { "returned from a thread",             RTEMS_TASK_EXITTED, },
    { "multiprocessing not configured",     RTEMS_MP_NOT_CONFIGURED, },
    { "invalid object name",                RTEMS_INVALID_NAME, },
    { "invalid object id",                  RTEMS_INVALID_ID, },
    { "too many",                           RTEMS_TOO_MANY, },
    { "timed out waiting",                  RTEMS_TIMEOUT, },
    { "object deleted while waiting",       RTEMS_OBJECT_WAS_DELETED, },
    { "specified size was invalid",         RTEMS_INVALID_SIZE, },
    { "address specified is invalid",       RTEMS_INVALID_ADDRESS, },
    { "number was invalid",                 RTEMS_INVALID_NUMBER, },
    { "item has not been initialized",      RTEMS_NOT_DEFINED, },
    { "resources still outstanding",        RTEMS_RESOURCE_IN_USE, },
    { "request not satisfied",              RTEMS_UNSATISFIED, },
    { "thread is in wrong state",           RTEMS_INCORRECT_STATE, },
    { "thread already in state",            RTEMS_ALREADY_SUSPENDED, },
    { "illegal on calling thread",          RTEMS_ILLEGAL_ON_SELF, },
    { "illegal for remote object",          RTEMS_ILLEGAL_ON_REMOTE_OBJECT, },
    { "called from wrong environment",      RTEMS_CALLED_FROM_ISR, },
    { "invalid thread priority",            RTEMS_INVALID_PRIORITY, },
    { "invalid date/time",                  RTEMS_INVALID_CLOCK, },
    { "invalid node id",                    RTEMS_INVALID_NODE, },
    { "directive not configured",           RTEMS_NOT_CONFIGURED, },
    { "not owner of resource",              RTEMS_NOT_OWNER_OF_RESOURCE , },
    { "directive not implemented",          RTEMS_NOT_IMPLEMENTED, },
    { "RTEMS inconsistency detected",       RTEMS_INTERNAL_ERROR, },
    { "could not get enough memory",        RTEMS_NO_MEMORY, },
    { "driver IO error",                    RTEMS_IO_ERROR, },
    { "internal multiprocessing only",      THREAD_STATUS_PROXY_BLOCKING, },
    { 0, 0, 0 },
};


const char *
rtems_status_text(
    rtems_status_code status
)
{
    return rtems_assoc_name_by_local(rtems_status_assoc, status);
}


static int rtems_verror(
    unsigned32   error_flag,
    const char   *printf_format,
    va_list      arglist
)
{
    int               local_errno = 0;
    int               chars_written = 0;
    rtems_status_code status;

    if (error_flag & RTEMS_ERROR_PANIC)
    {
        if (rtems_panic_in_progress++)
            _Thread_Disable_dispatch();       /* disable task switches */

        /* don't aggravate things */
        if (rtems_panic_in_progress > 2)
            return 0;
    }

    (void) fflush(stdout);  	    /* in case stdout/stderr same */

    status = error_flag & ~RTEMS_ERROR_MASK;
    if (error_flag & RTEMS_ERROR_ERRNO)     /* include errno? */
        local_errno = errno;

    if (_System_state_Is_multiprocessing)
        fprintf(stderr, "[%d] ", _Configuration_MP_table->node);
    
    if (rtems_progname && *rtems_progname)
        chars_written += fprintf(stderr, "%s: ", rtems_progname);
    chars_written += vfprintf(stderr, printf_format, arglist);
    
    if (status)
        chars_written += fprintf(stderr, " (status: %s)", rtems_status_text(status));

    if (local_errno)
    {
      if ((local_errno > 0) && *strerror(local_errno))
        chars_written += fprintf(stderr, " (errno: %s)", strerror(local_errno));
      else
        chars_written += fprintf(stderr, " (unknown errno=%d)", local_errno);
    }
    
    chars_written += fprintf(stderr, "\n");

    (void) fflush(stderr);

    if (error_flag & (RTEMS_ERROR_PANIC | RTEMS_ERROR_ABORT))
    {
        if (error_flag & RTEMS_ERROR_PANIC)
        {
            rtems_error(0, "fatal error, exiting");
            _exit(local_errno);
        }
        else
        {
            rtems_error(0, "fatal error, aborting");
            abort();
        }
    }
    return chars_written;
}


/*
 * Report an error.
 * error_flag is as above; printf_format is a normal
 * printf(3) format string, with its concommitant arguments.
 * 
 * Returns the number of characters written.
 */

int rtems_error(
    int   error_flag,
    const char *printf_format,
    ...
  )
{
    va_list arglist;
    int chars_written;

    va_start(arglist, printf_format);
    chars_written = rtems_verror(error_flag, printf_format, arglist);
    va_end(arglist);
    
    return chars_written;
}

/* 
 * rtems_panic is shorthand for rtems_error(RTEMS_ERROR_PANIC, ...)
 */

void rtems_panic(
    const char *printf_format,
    ...
  )
{
    va_list arglist;

    va_start(arglist, printf_format);
    (void) rtems_verror(RTEMS_ERROR_PANIC, printf_format, arglist);
    va_end(arglist);
}
