/*  system.h
 *
 *  This include file contains information that is included in every
 *  function in the test set.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
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
#include <tmacros.h>

void *POSIX_Init(
  void *argument
);

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_MAXIMUM_POSIX_THREADS        1
#define CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES 1
#define CONFIGURE_MAXIMUM_TIMERS               1

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#include <rtems/confdefs.h>
/* end of include file */

