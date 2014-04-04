/*
 * Copyright (c) 2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <bsp.h>
#include <bsp/bootcard.h>

#include <rtems/test.h>

const char rtems_test_name[] = "SPINTERNALERROR 1";

#define FATAL_SOURCE 0xdeadbeef

#define FATAL_IS_INTERNAL false

#define FATAL_ERROR 0x600df00d

void boot_card( const char *cmdline )
{
  _Terminate( FATAL_SOURCE, FATAL_IS_INTERNAL, FATAL_ERROR );
}

static void fatal_extension(
  Internal_errors_Source source,
  bool is_internal,
  Internal_errors_t error
)
{
  rtems_test_begink();

  if (
    source == FATAL_SOURCE
      && is_internal == FATAL_IS_INTERNAL
      && error == FATAL_ERROR
  ) {
    rtems_test_endk();
  }
}

static void *idle_body(uintptr_t ignored)
{
  while ( true ) {
    /* Do nothing */
  }

  return NULL;
}

#define CONFIGURE_INITIAL_EXTENSIONS \
  { .fatal = fatal_extension }, \
  RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_APPLICATION_DISABLE_FILESYSTEM

#define CONFIGURE_DISABLE_NEWLIB_REENTRANCY

#define CONFIGURE_SCHEDULER_USER

#define CONFIGURE_SCHEDULER_CONTEXT

#define CONFIGURE_SCHEDULER_CONTROLS { }

#define CONFIGURE_MEMORY_PER_TASK_FOR_SCHEDULER 0

#define CONFIGURE_TASK_STACK_ALLOCATOR NULL

#define CONFIGURE_TASK_STACK_DEALLOCATOR NULL

#define CONFIGURE_IDLE_TASK_INITIALIZES_APPLICATION

#define CONFIGURE_IDLE_TASK_BODY idle_body

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
