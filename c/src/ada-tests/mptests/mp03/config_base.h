/*  config_base.h
 *
 *  This include file defines all of the Configuration Table for this test
 *  EXCEPT the NODE NUMBER.
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */



#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_MAXIMUM_TASKS                       2

#define CONFIGURE_MAXIMUM_POSIX_THREADS              10
#define CONFIGURE_MAXIMUM_POSIX_KEYS                 10
#define CONFIGURE_MAXIMUM_POSIX_MUTEXES              20
#define CONFIGURE_MAXIMUM_POSIX_CONDITION_VARIABLES  10

#include <rtems/confdefs.h>

/* end of include file */
