/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>
#include "test_support.h"

#include <rtems/imfs.h>
#include <rtems/untar.h>
#include <rtems/error.h>

const char rtems_test_name[] = "TAR 3";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
void test_tarfs_error(void);

void test_tarfs_error(void)
{
  int sc;

  puts("Loading tarfs image at non-root - ERROR");
  sc = rtems_tarfs_load("x",(void *)rtems_task_create, 72);
  if (sc != -1) {
    printf ("error: untar failed returned %d\n", sc);
    rtems_test_exit(1);
  }
}

rtems_task Init(
  rtems_task_argument argument
)
{
  TEST_BEGIN();

  test_tarfs_error();
  
  TEST_END();
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS             1
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
/* end of file */
