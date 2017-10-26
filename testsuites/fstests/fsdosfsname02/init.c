/*
 * Copyright (c) 2017 embedded brains GmbH.  All rights reserved.
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
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_LIBBLOCK

#define CONFIGURE_APPLICATION_EXTRA_DRIVERS RAMDISK_DRIVER_TABLE_ENTRY

#define CONFIGURE_FILESYSTEM_DOSFS

#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 32

#define CONFIGURE_BDBUF_CACHE_MEMORY_SIZE 512
#define CONFIGURE_BDBUF_BUFFER_MIN_SIZE 512
#define CONFIGURE_BDBUF_BUFFER_MAX_SIZE 512

#define CONFIGURE_MAXIMUM_TASKS 1
#define CONFIGURE_MAXIMUM_SEMAPHORES 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
