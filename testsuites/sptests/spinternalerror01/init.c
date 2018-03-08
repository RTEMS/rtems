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

#include "tmacros.h"

#include <bsp.h>
#include <bsp/bootcard.h>

const char rtems_test_name[] = "SPINTERNALERROR 1";

#define FATAL_SOURCE 0xdeadbeef

#define FATAL_ERROR 0x600df00d

void boot_card( const char *cmdline )
{
  _Terminate( FATAL_SOURCE, FATAL_ERROR );
}

static void fatal_extension(
  Internal_errors_Source source,
  bool always_set_to_false,
  Internal_errors_t error
)
{
  TEST_BEGIN();

  if (
    source == FATAL_SOURCE
      && !always_set_to_false
      && error == FATAL_ERROR
  ) {
    TEST_END();
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

#define CONFIGURE_SCHEDULER

#define CONFIGURE_SCHEDULER_TABLE_ENTRIES { }

#define CONFIGURE_MEMORY_PER_TASK_FOR_SCHEDULER 0

#define CONFIGURE_TASK_STACK_ALLOCATOR NULL

#define CONFIGURE_TASK_STACK_DEALLOCATOR NULL

#define CONFIGURE_IDLE_TASK_INITIALIZES_APPLICATION

#define CONFIGURE_IDLE_TASK_BODY idle_body

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
