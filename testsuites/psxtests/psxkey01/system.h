/**
 *  @file
 *
 *  This include file contains information that is included in every
 *  function in the test set.
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

/* functions */

#include <pmacros.h>

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_MAXIMUM_TASKS          1
#define CONFIGURE_MAXIMUM_POSIX_KEYS     1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#include <rtems/confdefs.h>

/* global variables */

#ifdef CONFIGURE_INIT
#define TEST_EXTERN
#else
#define TEST_EXTERN extern
#endif

TEST_EXTERN pthread_t        Init_id;
TEST_EXTERN pthread_key_t    Key_id[CONFIGURE_MAXIMUM_POSIX_KEYS - 1];
TEST_EXTERN uint32_t         Data_array[ CONFIGURE_MAXIMUM_TASKS ];

#undef TEST_EXTERN
/* end of include file */
