/*  system.h
 *
 *  This include file contains information that is included in every
 *  function in the test set.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

/* functions */

#include <bsp.h>
#include <pthread.h>
#include <assert.h>
#include <stdio.h>

void *Init(
  void *argument
);

void *Task_1_through_3(
  void *argument
);

/* configuration information */

#define CONFIGURE_SPTEST

#define CONFIGURE_TEST_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_TEST_NEEDS_CLOCK_DRIVER

#define CONFIGURE_HAS_OWN_INIT_TASK_TABLE
#define CONFIGURE_INIT_TASK_TABLE_SIZE 0
#define CONFIGURE_INIT_TASK_TABLE      NULL

#include <confdefs.h>

/* global variables */

/* end of include file */
