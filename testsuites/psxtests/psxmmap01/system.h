/*
 * Copyright (c) 2017 Kevin Kirspel.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <pmacros.h>
#include <unistd.h>
#include <errno.h>

void *POSIX_Init(
  void *argument
);

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_ZERO_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_STUB_DRIVER

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_MAXIMUM_POSIX_THREADS     1
#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 10
#define CONFIGURE_MAXIMUM_POSIX_SHMS 1
#define CONFIGURE_MEMORY_OVERHEAD 10

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#include <rtems/confdefs.h>
