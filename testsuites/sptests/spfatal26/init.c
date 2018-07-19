/*
 * Copyright (c) 2012, 2018 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
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

#include <rtems.h>
#include <rtems/score/cpuimpl.h>

#include <tmacros.h>

const char rtems_test_name[] = "SPFATAL 26";

static void Init( rtems_task_argument arg )
{
  TEST_BEGIN();
  _CPU_Instruction_illegal();
  rtems_test_assert( 0 );
}

static void fatal_extension(
  rtems_fatal_source source,
  bool always_set_to_false,
  rtems_fatal_code code
)
{
  rtems_test_assert( source == RTEMS_FATAL_SOURCE_EXCEPTION );
  rtems_test_assert( !always_set_to_false );

  rtems_exception_frame_print( (const rtems_exception_frame *) code );

  TEST_END();
}

#define CONFIGURE_INITIAL_EXTENSIONS \
  { .fatal = fatal_extension }, \
  RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_APPLICATION_DISABLE_FILESYSTEM

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
