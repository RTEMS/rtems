/*  system.h
 *
 *  This include file contains information that is included in every
 *  function in the test set.
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <tmacros.h>
#include <itron.h>

/* functions */

void ITRON_Init( void );
void Task_2_through_4();

/* configuration information */

#define CONFIGURE_TEST_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_TEST_NEEDS_CLOCK_DRIVER

#define CONFIGURE_ITRON_INIT_TASK_TABLE

#include <confdefs.h>

/* global variables */

/* end of include file */



