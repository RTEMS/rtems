/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2017 embedded brains GmbH & Co. KG
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

#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include <rtems/dosfs.h>
#include <rtems/ramdisk.h>

#include "tmacros.h"

const char rtems_test_name[] = "FSDOSFSNAME 2";

#define RAMDISK_PATH "/dev/rda"

#define MOUNT_PATH "/mnt"

static const char * const dir_paths[] = {
  MOUNT_PATH "/cpukit",
  MOUNT_PATH "/cpukit/or1k-exception-frame-print.c",
  MOUNT_PATH "/cpukit/preinstall.am",
  MOUNT_PATH "/cpukit/Makefile.in",
  MOUNT_PATH "/cpukit/Makefile.am",
  MOUNT_PATH "/cpukit/rtems",
  MOUNT_PATH "/cpukit/or1k-context-switch.S",
  MOUNT_PATH "/cpukit/or1k-exception-default.c",
  MOUNT_PATH "/cpukit/or1k-context-initialize.c",
  MOUNT_PATH "/cpukit/or1k-context-volatile-clobber.S",
  MOUNT_PATH "/cpukit/or1k-context-validate.S",
  MOUNT_PATH "/cpukit/or1k-exception-handler-low.S",
  MOUNT_PATH "/cpukit/cpu.c"
};

static void test(void)
{
  int rv;
  size_t i;

  rv = msdos_format(RAMDISK_PATH, NULL);
  rtems_test_assert(rv == 0);

  rv = mount_and_make_target_path(
    RAMDISK_PATH,
    MOUNT_PATH,
    RTEMS_FILESYSTEM_TYPE_DOSFS,
    RTEMS_FILESYSTEM_READ_WRITE,
    NULL
  );
  rtems_test_assert(rv == 0);

  for (i = 0; i < RTEMS_ARRAY_SIZE(dir_paths); ++i) {
    rv = mkdir(dir_paths[i], S_IRWXU | S_IRWXG | S_IRWXO);
    rtems_test_assert(rv == 0);
  }

  for (i = RTEMS_ARRAY_SIZE(dir_paths); i > 0; --i) {
    rv = unlink(dir_paths[i - 1]);
    rtems_test_assert(rv == 0);
  }
}

static void Init(rtems_task_argument arg)
{
  TEST_BEGIN();

  test();

  TEST_END();
  rtems_test_exit(0);
}

rtems_ramdisk_config rtems_ramdisk_configuration[] = {
  { .block_size = 512, .block_num = 64 }
};

size_t rtems_ramdisk_configuration_size = RTEMS_ARRAY_SIZE(rtems_ramdisk_configuration);

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_LIBBLOCK

#define CONFIGURE_APPLICATION_EXTRA_DRIVERS RAMDISK_DRIVER_TABLE_ENTRY

#define CONFIGURE_FILESYSTEM_DOSFS

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 32

#define CONFIGURE_BDBUF_CACHE_MEMORY_SIZE 512
#define CONFIGURE_BDBUF_BUFFER_MIN_SIZE 512
#define CONFIGURE_BDBUF_BUFFER_MAX_SIZE 512

#define CONFIGURE_MAXIMUM_TASKS 1
#define CONFIGURE_MAXIMUM_SEMAPHORES 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
