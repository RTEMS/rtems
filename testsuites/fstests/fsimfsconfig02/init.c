/*
 * Copyright (c) 2015 embedded brains GmbH.  All rights reserved.
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

#include "tmacros.h"

#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>

#include <rtems/imfs.h>
#include <rtems/libio.h>

const char rtems_test_name[] = "FSIMFSCONFIG 2";

static void Init(rtems_task_argument arg)
{
  const char *mnt = "mnt";
  const char *link = "link";
  char buf[1];
  int rv;

  TEST_BEGIN();

  rv = mkdir(mnt, S_IRWXU);
  rtems_test_assert(rv == 0);

  rv = mount(
    "",
    mnt,
    RTEMS_FILESYSTEM_TYPE_IMFS,
    RTEMS_FILESYSTEM_READ_ONLY,
    NULL
  );
  rtems_test_assert(rv == 0);

  errno = 0;
  rv = unmount(mnt);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == ENOTSUP);

  rv = symlink(mnt, link);
  rtems_test_assert(rv == 0);

  errno = 0;
  rv = readlink(link, &buf[0], sizeof(buf));
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == ENOTSUP);

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_FILESYSTEM_IMFS

#define CONFIGURE_IMFS_DISABLE_READLINK
#define CONFIGURE_IMFS_DISABLE_UNMOUNT

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
