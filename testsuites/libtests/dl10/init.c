/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2018 Chris Johns <chrisj@rtems.org>.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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

static void notification(int fd, int seconds_remaining, void *arg)
{
  printf(
    "Press any key to enter shell (%is remaining)\n",
    seconds_remaining
  );
}

static void Init(rtems_task_argument arg)
{
  int e;
  rtems_status_code sc;

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


  sc = rtems_shell_wait_for_input (STDIN_FILENO,
                                   20,
                                   notification,
                                   NULL);
  if (sc == RTEMS_SUCCESSFUL) {
    rtems_shell_init ("SHLL",
                      RTEMS_MINIMUM_STACK_SIZE * 4,
                      100,
                      "/dev/foobar",
                      false,
                      true,
                      NULL);
  }

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

#define CONFIGURE_INIT_TASK_STACK_SIZE (CONFIGURE_MINIMUM_TASK_STACK_SIZE + (4U * 1024U))

#define CONFIGURE_INIT_TASK_ATTRIBUTES   (RTEMS_DEFAULT_ATTRIBUTES | RTEMS_FLOATING_POINT)

#define CONFIGURE_UNIFIED_WORK_AREAS

#define CONFIGURE_UNLIMITED_OBJECTS

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
