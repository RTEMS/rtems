/*  config.h
 *
 *  This include file defines the Configuration Table for this test.
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */


/* configuration information */

#define CONFIGURE_APPLICATION

/* NOTICE: the clock driver is explicitly disabled */
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_MAXIMUM_POSIX_THREADS              10
#define CONFIGURE_MAXIMUM_POSIX_KEYS                 10

#define CONFIGURE_MAXIMUM_TASKS              1

#define ADA_TEST_NAME "ADA HELLO"

#include <rtems/confdefs.h>

/* end of include file */
