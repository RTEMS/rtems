/*  Minimum Size Application Initialization 
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <bsp.h>

rtems_task Init(
  rtems_task_argument ignored
)
{
}

/* configuration information */

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_MAXIMUM_TASKS 1

/* NOTICE: the clock driver is explicitly disabled */
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

/*
 *  This is the smallest IO subsystem RTEMS currently supports.
 */
#define CONFIGURE_USE_MINIIMFS_AS_BASE_FILESYSTEM

/*
 *  This disables reentrancy support in the C Library.  It is usually
 *  not something an application wants to do unless the development
 *  team is committed to using C Library routines that are KNOWN to
 *  be reentrant.  Caveat Emptor!!
 */
#define CONFIGURE_DISABLE_NEWLIB_REENTRANCY

/*
 *  This test does not need the console driver so there is no reason
 *  to configure termios.
 */
#define CONFIGURE_TERMIOS_DISABLED

/*
 *  This test does not use any stdio.
 */
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 0

/*
 *  This may prevent us from running on every architecture but it
 *  demonstrates that the user can specify how small of a minimum
 *  stack they want.
 */
#define CONFIGURE_MINIMUM_TASK_STACK_SIZE 512

#define CONFIGURE_INIT

#include <rtems/confdefs.h>

/* global variables */
