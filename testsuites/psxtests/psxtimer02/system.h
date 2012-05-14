/*  system.h
 *
 *  This include file contains information that is included in every
 *  function in the test set.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

/* functions */

#include <pmacros.h>
#include <unistd.h>
#include <errno.h>
#include <sched.h>

void *POSIX_Init (
  void *arg
);

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_MAXIMUM_POSIX_THREADS             1
#define CONFIGURE_MAXIMUM_POSIX_TIMERS              1


#include <rtems/confdefs.h>

/* end of include file */
