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
 * http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include "tmacros.h"

#include <rtems.h>

static void test(void)
{
  rtems_fatal_code error = 0;
  const char *desc_last = NULL;
  const char *desc;

  do {
    desc_last = desc;
    desc = rtems_internal_error_description( error );
    ++error;
    puts( desc );
  } while ( desc != desc_last );

  rtems_test_assert( error - 3 == INTERNAL_ERROR_NO_MEMORY_FOR_HEAP );
}

static void Init(rtems_task_argument arg)
{
  puts("\n\n*** TEST SPINTERNALERROR 2 ***");

  test();

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
