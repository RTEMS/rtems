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

#include <limits.h>

#include <rtems.h>

const char rtems_test_name[] = "SPFATAL 26";

static void provoke_aligment_or_data_access_exception( void )
{
  uintptr_t one = 1;
  int i = sizeof(void *) * CHAR_BIT;
  uintptr_t n = 1;
  uintptr_t base = 0;
  uintptr_t inc;

  *(volatile uint64_t *) base;

  do {
    int j;

    --i;
    base = one << i;
    inc = base << 1;

    for (j = 0; j < n; ++j, base += inc) {
      *(volatile uint64_t *) base;
    }

    n <<= 1;
  } while (i > 0);
}

static void Init( rtems_task_argument arg )
{
  rtems_test_begink();

  provoke_aligment_or_data_access_exception();

  rtems_test_assert( 0 );
}

static void fatal_extension(
  rtems_fatal_source source,
  bool is_internal,
  rtems_fatal_code code
)
{
  rtems_test_assert( source == RTEMS_FATAL_SOURCE_EXCEPTION );
  rtems_test_assert( !is_internal );

  rtems_exception_frame_print( (const rtems_exception_frame *) code );

  rtems_test_endk();
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
