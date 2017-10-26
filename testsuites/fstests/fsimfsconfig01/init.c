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
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <utime.h>

#include <rtems/imfs.h>
#include <rtems/libio.h>

const char rtems_test_name[] = "FSIMFSCONFIG 1";

static const IMFS_node_control node_control = IMFS_GENERIC_INITIALIZER(
  &rtems_filesystem_handlers_default,
  IMFS_node_initialize_generic,
  IMFS_node_destroy_default
);

static void Init(rtems_task_argument arg)
{
  struct utimbuf times;
  const char *generic = "generic";
  const char *mnt = "mnt";
  const char *dev = "device";
  const char *file = "file";
  const char *fifo = "fifo";
  int rv;
  int fd;
  DIR *dirp;
  struct dirent *dire;
  struct stat st;

  TEST_BEGIN();

  rv = IMFS_make_generic_node(
    generic,
    S_IFCHR | S_IRWXU | S_IRWXG | S_IRWXO,
    &node_control,
    NULL
  );
  rtems_test_assert(rv == 0);

  errno = 0;
  rv = chown(generic, 0, 0);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == ENOTSUP);

  errno = 0;
  rv = chmod(generic, 0);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == ENOTSUP);

  errno = 0;
  rv = link(generic, "link");
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == ENOTSUP);

  rv = mkdir(mnt, S_IRWXU);
  rtems_test_assert(rv == 0);

  rv = mknod(dev, S_IFCHR | S_IRWXU, 0);
  rtems_test_assert(rv == 0);

  errno = 0;
  fd = creat(file, S_IRWXU);
  rtems_test_assert(fd == -1);
  rtems_test_assert(errno == ENOSYS);

  errno = 0;
  rv = mkfifo(fifo, S_IRWXU);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == ENOSYS);

  dirp = opendir(mnt);
  rtems_test_assert(dirp != NULL);

  errno = 0;
  dire = readdir(dirp);
  rtems_test_assert(dire == NULL);
  rtems_test_assert(errno == ENOTSUP);

  rv = closedir(dirp);
  rtems_test_assert(rv == 0);

  rv = stat(mnt, &st);
  rtems_test_assert(rv == 0);
  rtems_test_assert(st.st_size == 0);

  errno = 0;
  rv = mount(
    "",
    mnt,
    RTEMS_FILESYSTEM_TYPE_IMFS,
    RTEMS_FILESYSTEM_READ_ONLY,
    NULL
  );
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == ENOTSUP);

  errno = 0;
  rv = rename(generic, "new");
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == ENOTSUP);

  errno = 0;
  rv = symlink(generic, "link");
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == ENOTSUP);

  errno = 0;
  rv = utime(generic, &times);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == ENOTSUP);

  errno = 0;
  rv = unlink(generic);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == ENOTSUP);

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 4

#define CONFIGURE_FILESYSTEM_IMFS

#define CONFIGURE_IMFS_DISABLE_CHOWN
#define CONFIGURE_IMFS_DISABLE_CHMOD
#define CONFIGURE_IMFS_DISABLE_LINK

#if 0
/*
 * This would lead to a fatal error since rtems_filesystem_initialize() creates
 * a "/dev" directory.
 */
#define CONFIGURE_IMFS_DISABLE_MKNOD
#endif

#define CONFIGURE_IMFS_DISABLE_MKNOD_FILE
#define CONFIGURE_IMFS_DISABLE_MOUNT
#define CONFIGURE_IMFS_DISABLE_RENAME
#define CONFIGURE_IMFS_DISABLE_READDIR
#define CONFIGURE_IMFS_DISABLE_RMNOD
#define CONFIGURE_IMFS_DISABLE_SYMLINK
#define CONFIGURE_IMFS_DISABLE_UTIME

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
