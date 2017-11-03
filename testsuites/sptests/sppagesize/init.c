/*
 *  Copyright (c) 2013 Chirayu Desai <cdesai@cyanogenmod.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>
#include <rtems.h>
#include <limits.h>
#include <unistd.h>
#include <sys/param.h>

const char rtems_test_name[] = "SPPAGESIZE";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);

rtems_task Init(
  rtems_task_argument argument
)
{
    TEST_BEGIN();

    rtems_test_assert(PAGESIZE == PAGE_SIZE);
    rtems_test_assert(getpagesize() == PAGE_SIZE);

    TEST_END();

    rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
