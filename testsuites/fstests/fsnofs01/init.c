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

#include <sys/stat.h>
#include <sys/statvfs.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <utime.h>

#include <rtems/libio_.h>

static int node_count(const rtems_chain_control *chain)
{
  int count = 0;
  const rtems_chain_node *current = rtems_chain_immutable_first(chain);
  const rtems_chain_node *tail = rtems_chain_immutable_tail(chain);

  while (current != tail) {
    ++count;

    current = rtems_chain_immutable_next(current);
  }

  return count;
}

static void rtems_test_assert_equal_to_null_loc(
  const rtems_filesystem_location_info_t *local_loc
)
{
  rtems_filesystem_global_location_t *null_loc =
    &rtems_filesystem_global_location_null;

  rtems_test_assert(null_loc->location.node_access == local_loc->node_access);
  rtems_test_assert(null_loc->location.node_access_2 == local_loc->node_access_2);
  rtems_test_assert(null_loc->location.handlers == local_loc->handlers);
  rtems_test_assert(null_loc->location.mt_entry == local_loc->mt_entry);
}

static void test_initial_values(void)
{
  rtems_filesystem_global_location_t *null_loc =
    &rtems_filesystem_global_location_null;
  rtems_filesystem_mount_table_entry_t *null_mt = null_loc->location.mt_entry;
  rtems_chain_control *loc_chain = &null_mt->location_chain;
  rtems_chain_node *loc_node = &null_loc->location.mt_entry_node;

  rtems_test_assert(node_count(loc_chain) == 1);
  rtems_test_assert(rtems_chain_previous(loc_node) == rtems_chain_head(loc_chain));
  rtems_test_assert(rtems_chain_next(loc_node) == rtems_chain_tail(loc_chain));
  rtems_test_assert(null_mt->mt_point_node == null_loc);
  rtems_test_assert(null_mt->mt_fs_root == null_loc);
  rtems_test_assert(!null_mt->mounted);
  rtems_test_assert(!null_mt->writeable);
  rtems_test_assert(null_loc->reference_count == 4);
}

static void test_location_obtain(void)
{
  rtems_filesystem_global_location_t *global_loc = NULL;
  rtems_filesystem_global_location_t *null_loc =
    rtems_filesystem_global_location_obtain(&global_loc);
  rtems_filesystem_mount_table_entry_t *null_mt = null_loc->location.mt_entry;
  rtems_chain_control *loc_chain = &null_mt->location_chain;

  rtems_test_assert(node_count(loc_chain) == 1);
  rtems_test_assert(null_loc->reference_count == 5);

  rtems_filesystem_global_location_release(null_loc);

  rtems_test_assert(node_count(loc_chain) == 1);
  rtems_test_assert(null_loc->reference_count == 4);
}

static void test_null_location_obtain(void)
{
  rtems_filesystem_global_location_t *null_loc =
    rtems_filesystem_global_location_obtain_null();
  rtems_filesystem_mount_table_entry_t *null_mt = null_loc->location.mt_entry;
  rtems_chain_control *loc_chain = &null_mt->location_chain;

  rtems_test_assert(node_count(loc_chain) == 1);
  rtems_test_assert(null_loc->reference_count == 5);

  rtems_filesystem_global_location_release(null_loc);

  rtems_test_assert(node_count(loc_chain) == 1);
  rtems_test_assert(null_loc->reference_count == 4);
}

static void test_null_location_replace(void)
{
  rtems_filesystem_global_location_t *null_loc =
    &rtems_filesystem_global_location_null;
  rtems_filesystem_mount_table_entry_t *null_mt = null_loc->location.mt_entry;
  rtems_chain_control *loc_chain = &null_mt->location_chain;
  rtems_filesystem_location_info_t local_loc;

  rtems_test_assert(node_count(loc_chain) == 1);
  rtems_test_assert(null_loc->reference_count == 4);
  rtems_test_assert(rtems_filesystem_global_location_is_null(null_loc));

  rtems_filesystem_location_copy(&local_loc, &null_loc->location);

  rtems_test_assert(node_count(loc_chain) == 2);
  rtems_test_assert(null_loc->reference_count == 4);

  rtems_filesystem_location_detach(&local_loc);

  rtems_test_assert(node_count(loc_chain) == 2);
  rtems_test_assert(null_loc->reference_count == 4);
  rtems_test_assert(rtems_filesystem_location_is_null(&local_loc));
  rtems_test_assert_equal_to_null_loc(&local_loc);

  rtems_filesystem_location_free(&local_loc);

  rtems_test_assert(node_count(loc_chain) == 1);
  rtems_test_assert(null_loc->reference_count == 4);
}

static void test_null_location_get_and_replace(void)
{
  rtems_filesystem_global_location_t *null_loc =
    &rtems_filesystem_global_location_null;
  rtems_filesystem_mount_table_entry_t *null_mt = null_loc->location.mt_entry;
  rtems_chain_control *loc_chain = &null_mt->location_chain;
  rtems_filesystem_location_info_t local_loc_0;
  rtems_filesystem_location_info_t local_loc_1;

  rtems_test_assert(node_count(loc_chain) == 1);
  rtems_test_assert(null_loc->reference_count == 4);

  rtems_filesystem_location_copy(&local_loc_0, &null_loc->location);

  rtems_test_assert(node_count(loc_chain) == 2);
  rtems_test_assert(null_loc->reference_count == 4);
  rtems_test_assert_equal_to_null_loc(&local_loc_0);

  rtems_filesystem_location_copy_and_detach(&local_loc_1, &local_loc_0);

  rtems_test_assert(node_count(loc_chain) == 3);
  rtems_test_assert(null_loc->reference_count == 4);
  rtems_test_assert_equal_to_null_loc(&local_loc_0);
  rtems_test_assert_equal_to_null_loc(&local_loc_1);

  rtems_filesystem_location_free(&local_loc_0);

  rtems_test_assert(node_count(loc_chain) == 2);
  rtems_test_assert(null_loc->reference_count == 4);
  rtems_test_assert_equal_to_null_loc(&local_loc_1);

  rtems_filesystem_location_free(&local_loc_1);

  rtems_test_assert(node_count(loc_chain) == 1);
  rtems_test_assert(null_loc->reference_count == 4);
}

static void test_path_ops(void)
{
  int rv = 0;
  long lrv = 0;
  struct stat st;
  struct statvfs stvfs;
  char buf [32];
  ssize_t n = 0;
  const char *path = "/";
  const struct utimbuf times;

  errno = 0;
  rv = open(path, O_RDONLY);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == ENXIO);

  errno = 0;
  rv = chdir(path);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == ENXIO);

  errno = 0;
  rv = chroot(path);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == ENXIO);

  errno = 0;
  rv = mknod(path, S_IFREG, 0);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == ENXIO);

  errno = 0;
  rv = mkdir(path, 0);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == ENXIO);

  errno = 0;
  rv = mkfifo(path, 0);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == ENXIO);

  errno = 0;
  rv = stat(path, &st);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == ENXIO);

  errno = 0;
  rv = lstat(path, &st);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == ENXIO);

  errno = 0;
  rv = statvfs(path, &stvfs);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == ENXIO);

  errno = 0;
  n = readlink(path, buf, sizeof(buf));
  rtems_test_assert(n == -1);
  rtems_test_assert(errno == ENXIO);

  errno = 0;
  rv = chmod(path, 0);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == ENXIO);

  errno = 0;
  rv = chown(path, 0, 0);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == ENXIO);

  errno = 0;
  rv = lchown(path, 0, 0);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == ENXIO);

  errno = 0;
  rv = rmdir(path);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == ENXIO);

  errno = 0;
  rv = unlink(path);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == ENXIO);

  errno = 0;
  rv = truncate(path, 0);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == ENXIO);

  errno = 0;
  rv = access(path, 0);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == ENXIO);

  errno = 0;
  lrv = pathconf(path, _PC_LINK_MAX);
  rtems_test_assert(lrv == -1);
  rtems_test_assert(errno == ENXIO);

  errno = 0;
  rv = link(path, path);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == ENXIO);

  errno = 0;
  rv = symlink(path, path);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == ENXIO);

  errno = 0;
  rv = rename(path, path);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == ENXIO);

  errno = 0;
  rv = utime(path, &times);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == ENXIO);
}

static void test_user_env(void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_filesystem_global_location_t *null_loc =
    &rtems_filesystem_global_location_null;
  rtems_filesystem_mount_table_entry_t *null_mt = null_loc->location.mt_entry;
  rtems_chain_control *loc_chain = &null_mt->location_chain;

  rtems_test_assert(node_count(loc_chain) == 1);
  rtems_test_assert(null_loc->reference_count == 4);

  sc = rtems_libio_set_private_env();
  rtems_test_assert(sc == RTEMS_UNSATISFIED);

  rtems_test_assert(node_count(loc_chain) == 1);
  rtems_test_assert(null_loc->reference_count == 4);

  sc = rtems_libio_share_private_env(RTEMS_SELF);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(node_count(loc_chain) == 1);
  rtems_test_assert(null_loc->reference_count == 4);

  sc = rtems_libio_share_private_env(rtems_task_self());
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(node_count(loc_chain) == 1);
  rtems_test_assert(null_loc->reference_count == 4);

  rtems_libio_use_global_env();

  rtems_test_assert(node_count(loc_chain) == 1);
  rtems_test_assert(null_loc->reference_count == 4);
}

static void Init(rtems_task_argument arg)
{
  printk("\n\n*** TEST FSNOFS 1 ***\n");

  rtems_libio_init();

  test_initial_values();
  test_location_obtain();
  test_null_location_obtain();
  test_null_location_replace();
  test_null_location_get_and_replace();
  test_path_ops();
  test_user_env();

  printk("*** END OF TEST FSNOFS 1 ***\n");

  exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_APPLICATION_DISABLE_FILESYSTEM

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
