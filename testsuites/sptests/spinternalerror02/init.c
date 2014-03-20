/*
 * Copyright (c) 2012-2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Donierstr. 4
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

#include <rtems.h>

static void test_internal_error_text(void)
{
  rtems_fatal_code error = 0;
  const char *text = NULL;
  const char *text_last;

  do {
    text_last = text;
    text = rtems_internal_error_text( error );
    ++error;
    puts( text );
  } while ( text != text_last );

  rtems_test_assert( error - 3 == INTERNAL_ERROR_CPU_ISR_INSTALL_VECTOR );
}

static void test_fatal_source_text(void)
{
  rtems_fatal_source source = 0;
  const char *text = NULL;
  const char *text_last;

  do {
    text_last = text;
    text = rtems_fatal_source_text( source );
    ++source;
    puts( text );
  } while ( text != text_last );

  rtems_test_assert( source - 3 == RTEMS_FATAL_SOURCE_SMP );
}

static void test_status_text(void)
{
  rtems_status_code code = 0;
  const char *text = NULL;
  const char *text_last;

  do {
    text_last = text;
    text = rtems_status_text( code );
    ++code;
    puts( text );
  } while ( text != text_last );

  rtems_test_assert( code - 3 == RTEMS_PROXY_BLOCKING );
}

static void Init(rtems_task_argument arg)
{
  puts("\n\n*** TEST SPINTERNALERROR 2 ***");

  test_internal_error_text();
  test_fatal_source_text();
  test_status_text();

  puts("*** END OF TEST SPINTERNALERROR 2 ***");

  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
