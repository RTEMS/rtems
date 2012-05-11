/**
 *
 *  This include file contains information that is included in every
 *  function in the test set.
 */

/*
 *  COPYRIGHT (c) 1989-2012.
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
void force_error(void);

void Put_Source(uint32_t source);

void Put_Error(
  uint32_t source,
  uint32_t error
);

void *POSIX_Init(
  void *argument
);

void Fatal_extension(
  uint32_t   source,
  bool    is_internal,
  uint32_t   error
);

/* need prototypes */

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
#ifndef SPFATAL_TEST_CASE_EXTRA_THREADS
#define SPFATAL_TEST_CASE_EXTRA_THREADS 0
#endif
#define CONFIGURE_MAXIMUM_POSIX_THREADS \
  (SPFATAL_TEST_CASE_EXTRA_THREADS + 1)

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#include <rtems/confdefs.h>

/* global variables */

/* end of include file */
