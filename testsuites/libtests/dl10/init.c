/*
 * Copyright (c) 2018 Chris Johns <chrisj@rtems.org>.  All rights reserved.
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

#include <rtems/rtl/dlfcn-shell.h>
#include <rtems/rtl/rtl.h>
#include <rtems/rtl/rtl-shell.h>
#include <rtems/shell.h>
#include <rtems/imfs.h>

#include "dl-load.h"

const char rtems_test_name[] = "libdl (RTL) 10";

/* forward declarations to avoid warnings */
static rtems_task Init(rtems_task_argument argument);

#include "dl10-tar.h"

#define TARFILE_START dl10_tar
#define TARFILE_SIZE  dl10_tar_size

static int test(void)
{
#if USE_SHELL_CMD
  int ret;
  ret = dl_load_test();
  if (ret)
    rtems_test_exit(ret);
#endif
  return 0;
}

static void Init(rtems_task_argument arg)
{
  int e;

  TEST_BEGIN();

  e = rtems_tarfs_load("/", (void *)TARFILE_START, (size_t)TARFILE_SIZE);
  if (e != 0)
  {
    printf ("error: untar failed: %d\n", e);
    rtems_test_exit (1);
    exit (1);
  }

  test();

  rtems_shell_init_environment ();

  printf ("RTL (libdl) commands: dl, rtl\n\n");

  if (rtems_shell_add_cmd ("rtl",
                           "rtl",
                           "rtl -l",
                           rtems_rtl_shell_command) == NULL)
  {
    printf("command add failed\n");
    rtems_test_exit(1);
    exit (1);
  }

  rtems_shell_init ("SHLL",
                    RTEMS_MINIMUM_STACK_SIZE * 4,
                    100,
                    "/dev/foobar",
                    false,
                    true,
                    NULL);

  TEST_END();

  rtems_test_exit(0);
}

#define CONFIGURE_SHELL_COMMANDS_INIT
#define CONFIGURE_SHELL_COMMANDS_ALL

/*
 * Remove the commands that pull in libblock.
 */
#define CONFIGURE_SHELL_NO_COMMAND_BLKSYNC
#define CONFIGURE_SHELL_NO_COMMAND_BLKSTATS
#define CONFIGURE_SHELL_NO_COMMAND_FDISK
#define CONFIGURE_SHELL_NO_COMMAND_MKRFS
#define CONFIGURE_SHELL_NO_COMMAND_DEBUGRFS

#include <rtems/shellconfig.h>

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 10

#define CONFIGURE_MAXIMUM_TASKS 4

#define CONFIGURE_MAXIMUM_SEMAPHORES 4

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_STACK_SIZE (8U * 1024U)

#define CONFIGURE_INIT_TASK_ATTRIBUTES   (RTEMS_DEFAULT_ATTRIBUTES | RTEMS_FLOATING_POINT)

#define CONFIGURE_UNIFIED_WORK_AREAS

#define CONFIGURE_UNLIMITED_OBJECTS

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
