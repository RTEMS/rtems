/*
 *  report errors and panics to RTEMS' stderr.
 *  Currently just used by RTEMS monitor.
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
 *  RTEMS_ERROR_ERRNO       -- include errno text in output
 *  RTEMS_ERROR_PANIC       -- halts local system after output
 *  RTEMS_ERROR_ABORT       -- abort after output
 *
 * It can also include a rtems_status value which can be OR'd
 * with the above flags. *
 *
 * EXAMPLE
 *  #include <rtems.h>
 *  #include <rtems/error.h>
 *  rtems_error(0, "stray interrupt %d", intr);
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

/* This is always defined on RTEMS Scheduler Simulator and thus
 * we get a redefined warning if this is not present.
 */
#ifndef __RTEMS_VIOLATE_KERNEL_VISIBILITY__
  #define __RTEMS_VIOLATE_KERNEL_VISIBILITY__
#endif
#include <rtems.h>

#include <rtems/error.h>
#include <rtems/assoc.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>     /* _exit() */

int          rtems_panic_in_progress;

const rtems_assoc_t rtems_status_assoc[] = {
  { "successful completion",              RTEMS_SUCCESSFUL, 0 },
  { "returned from a thread",             RTEMS_TASK_EXITTED, 0 },
  { "multiprocessing not configured",     RTEMS_MP_NOT_CONFIGURED, 0 },
  { "invalid object name",                RTEMS_INVALID_NAME, 0 },
  { "invalid object id",                  RTEMS_INVALID_ID, 0 },
  { "too many",                           RTEMS_TOO_MANY, 0 },
  { "timed out waiting",                  RTEMS_TIMEOUT, 0 },
  { "object deleted while waiting",       RTEMS_OBJECT_WAS_DELETED, 0 },
  { "specified size was invalid",         RTEMS_INVALID_SIZE, 0 },
  { "address specified is invalid",       RTEMS_INVALID_ADDRESS, 0 },
  { "number was invalid",                 RTEMS_INVALID_NUMBER, 0 },
  { "item has not been initialized",      RTEMS_NOT_DEFINED, 0 },
  { "resources still outstanding",        RTEMS_RESOURCE_IN_USE, 0 },
  { "request not satisfied",              RTEMS_UNSATISFIED, 0 },
  { "thread is in wrong state",           RTEMS_INCORRECT_STATE, 0 },
  { "thread already in state",            RTEMS_ALREADY_SUSPENDED, 0 },
  { "illegal on calling thread",          RTEMS_ILLEGAL_ON_SELF, 0 },
  { "illegal for remote object",          RTEMS_ILLEGAL_ON_REMOTE_OBJECT, 0 },
  { "called from wrong environment",      RTEMS_CALLED_FROM_ISR, 0 },
  { "invalid thread priority",            RTEMS_INVALID_PRIORITY, 0 },
  { "invalid date/time",                  RTEMS_INVALID_CLOCK, 0 },
  { "invalid node id",                    RTEMS_INVALID_NODE, 0 },
  { "directive not configured",           RTEMS_NOT_CONFIGURED, 0 },
  { "not owner of resource",              RTEMS_NOT_OWNER_OF_RESOURCE , 0 },
  { "directive not implemented",          RTEMS_NOT_IMPLEMENTED, 0 },
  { "RTEMS inconsistency detected",       RTEMS_INTERNAL_ERROR, 0 },
  { "could not get enough memory",        RTEMS_NO_MEMORY, 0 },
  { "driver IO error",                    RTEMS_IO_ERROR, 0 },
  { "internal multiprocessing only",      THREAD_STATUS_PROXY_BLOCKING, 0 },
  { 0, 0, 0 },
};


const char *rtems_status_text(
  rtems_status_code status
)
{
  return rtems_assoc_name_by_local(rtems_status_assoc, status);
}


static int rtems_verror(
  rtems_error_code_t  error_flag,
  const char         *printf_format,
  va_list             arglist
)
{
  int               local_errno = 0;
  int               chars_written = 0;
  rtems_status_code status;

  if (error_flag & RTEMS_ERROR_PANIC) {
    if (rtems_panic_in_progress++)
      _Thread_Disable_dispatch();       /* disable task switches */

    /* don't aggravate things */
    if (rtems_panic_in_progress > 2)
      return 0;
  }

  (void) fflush(stdout);            /* in case stdout/stderr same */

  status = error_flag & ~RTEMS_ERROR_MASK;
  if (error_flag & RTEMS_ERROR_ERRNO)     /* include errno? */
    local_errno = errno;

  #if defined(RTEMS_MULTIPROCESSING)
    if (_System_state_Is_multiprocessing)
      fprintf(stderr, "[%" PRIu32 "] ", _Configuration_MP_table->node);
  #endif

  chars_written += vfprintf(stderr, printf_format, arglist);

  if (status)
    chars_written +=
      fprintf(stderr, " (status: %s)", rtems_status_text(status));

  if (local_errno) {
    if ((local_errno > 0) && *strerror(local_errno))
      chars_written += fprintf(stderr, " (errno: %s)", strerror(local_errno));
    else
      chars_written += fprintf(stderr, " (unknown errno=%d)", local_errno);
  }

  chars_written += fprintf(stderr, "\n");

  (void) fflush(stderr);

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
  rtems_error_code_t error_flag,
  const char *printf_format,
  ...
)
{
  va_list arglist;
  int chars_written;

  va_start(arglist, printf_format);
  chars_written = rtems_verror(error_flag, printf_format, arglist);
  va_end(arglist);

  if (error_flag & RTEMS_ERROR_PANIC) {
    rtems_error(0, "fatal error, exiting");
    _exit(errno);
  }
  if (error_flag & RTEMS_ERROR_ABORT) {
    rtems_error(0, "fatal error, aborting");
    abort();
  }

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

  rtems_error(0, "fatal error, exiting");
  _exit(errno);
}
