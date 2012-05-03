/*  system.h
 *
 *  This include file contains information that is included in every
 *  function in the test set.
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

/*
 *  Some of the fatal error cases require the ability to peek inside RTEMS
 */
#define __RTEMS_VIOLATE_KERNEL_VISIBILITY__
#include <rtems.h>
#include <tmacros.h>

/* functions */

rtems_task Init(
  rtems_task_argument argument
);

void Fatal_extension(
  uint32_t   source,
  bool       is_internal,
  uint32_t   error
);

void Put_Error(
  uint32_t source,
  uint32_t error
);

void Put_Source(
  uint32_t source
);

void force_error(void);

/* need some prototypes for test cases */

rtems_device_driver consume_semaphores_initialize(
  rtems_device_major_number major __attribute__((unused)),
  rtems_device_minor_number minor __attribute__((unused)),
  void *pargp __attribute__((unused))
);

#define CONSUME_SEMAPHORE_DRIVERS \
  { consume_semaphores_initialize, NULL, NULL, NULL, NULL, NULL }

#include "testcase.h"

/* configuration information */

extern rtems_extensions_table initial_extensions;

#define CONFIGURE_INITIAL_EXTENSIONS \
  { \
    NULL,                    /* create  */ \
    NULL,                    /* start   */ \
    NULL,                    /* restart */ \
    NULL,                    /* delete  */ \
    NULL,                    /* switch  */ \
    NULL,                    /* begin   */ \
    NULL,                    /* exitted */ \
    Fatal_extension          /* fatal   */ \
  }

/* extra parameters may be in testcase.h */
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

/* always need an Init task, some cases need more tasks */
#ifndef SPFATAL_TEST_CASE_EXTRA_TASKS
#define SPFATAL_TEST_CASE_EXTRA_TASKS 0
#endif
#define CONFIGURE_MAXIMUM_TASKS \
  (SPFATAL_TEST_CASE_EXTRA_TASKS + 1)

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#include <rtems/confdefs.h>

/* global variables */

/* end of include file */
