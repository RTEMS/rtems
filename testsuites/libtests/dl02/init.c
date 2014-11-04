/*
 * Copyright (c) 2014 Chris Johns <chrisj@rtems.org>.  All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include "tmacros.h"

#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

#include <rtems/rtl/rtl.h>
#include <rtems/untar.h>

#include "dl-load.h"

const char rtems_test_name[] = "libdl (RTL) 2";

/* forward declarations to avoid warnings */
static rtems_task Init(rtems_task_argument argument);

#include "dl-tar.h"

#define TARFILE_START dl_tar
#define TARFILE_SIZE  dl_tar_size

static int test(void)
{
  int ret;
  ret = dl_load_test();
  if (ret)
    rtems_test_exit(ret);
  return 0;
}

static void Init(rtems_task_argument arg)
{
  int te;

  TEST_BEGIN();

  te = Untar_FromMemory((void *)TARFILE_START, (size_t)TARFILE_SIZE);
  if (te != 0)
  {
    printf("untar failed: %d\n", te);
    rtems_test_exit(1);
    exit (1);
  }

  test();

  TEST_END();

  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 4

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_MINIMUM_TASK_STACK_SIZE (8U * 1024U)

#define CONFIGURE_EXTRA_TASK_STACKS (8 * 1024)

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>

