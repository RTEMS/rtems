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
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#define TESTS_USE_PRINTK
#include "tmacros.h"

#include <sys/stat.h>
#include <sys/statvfs.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <utime.h>

#include <rtems/libio_.h>

const char rtems_test_name[] = "FSNOFS 1";

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
  rtems_test_assert(null_loc->deferred_released_next == NULL);
  rtems_test_assert(null_loc->deferred_released_count == 0);
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

  rtems_filesystem_global_location_release(null_loc, false);

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

  rtems_filesystem_global_location_release(null_loc, false);

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

  rtems_libio_use_global_env();

  rtems_test_assert(node_count(loc_chain) == 1);
  rtems_test_assert(null_loc->reference_count == 4);
}

typedef struct {
  int flags;
  mode_t object_mode;
  uid_t object_uid;
  gid_t object_gid;
  bool expected_ok;
} check_access_case;

#define FR RTEMS_FS_PERMS_READ
#define FW RTEMS_FS_PERMS_WRITE
#define FX RTEMS_FS_PERMS_EXEC

#define UR S_IRUSR
#define UW S_IWUSR
#define UX S_IXUSR

#define GR S_IRGRP
#define GW S_IWGRP
#define GX S_IXGRP

#define OR S_IROTH
#define OW S_IWOTH
#define OX S_IXOTH

static const check_access_case check_access_euid_0_cases[] = {
  { 0,   0, 6, 7, true },
  { FR,  0, 6, 7, false },
  { FW,  0, 6, 7, false },
  { FX,  0, 6, 7, false },
  { FR, UR, 6, 7, true },
  { FW, UW, 6, 7, true },
  { FX, UX, 6, 7, true },
  { FR, GR, 6, 7, false },
  { FW, GW, 6, 7, false },
  { FX, GX, 6, 7, false },
  { FR, OR, 6, 7, false },
  { FW, OW, 6, 7, false },
  { FX, OX, 6, 7, false }
};

static const check_access_case check_access_egid_0_cases[] = {
  { 0,   0, 6, 7, true },
  { FR,  0, 6, 7, false },
  { FW,  0, 6, 7, false },
  { FX,  0, 6, 7, false },
  { FR, UR, 6, 7, false },
  { FW, UW, 6, 7, false },
  { FX, UX, 6, 7, false },
  { FR, GR, 6, 7, true },
  { FW, GW, 6, 7, true },
  { FX, GX, 6, 7, true },
  { FR, OR, 6, 7, false },
  { FW, OW, 6, 7, false },
  { FX, OX, 6, 7, false }
};

static const check_access_case check_access_other_cases[] = {
  { 0,   0, 3, 7, true },
  { FR,  0, 3, 7, false },
  { FW,  0, 3, 7, false },
  { FX,  0, 3, 7, false },
  { FR, UR, 3, 7, true },
  { FW, UW, 3, 7, true },
  { FX, UX, 3, 7, true },
  { FR, GR, 3, 7, false },
  { FW, GW, 3, 7, false },
  { FX, GX, 3, 7, false },
  { FR, OR, 3, 7, false },
  { FW, OW, 3, 7, false },
  { FX, OX, 3, 7, false },
  { 0,   0, 6, 4, true },
  { FR,  0, 6, 4, false },
  { FW,  0, 6, 4, false },
  { FX,  0, 6, 4, false },
  { FR, UR, 6, 4, false },
  { FW, UW, 6, 4, false },
  { FX, UX, 6, 4, false },
  { FR, GR, 6, 4, true },
  { FW, GW, 6, 4, true },
  { FX, GX, 6, 4, true },
  { FR, OR, 6, 4, false },
  { FW, OW, 6, 4, false },
  { FX, OX, 6, 4, false },
  { 0,   0, 6, 5, true },
  { FR,  0, 6, 5, false },
  { FW,  0, 6, 5, false },
  { FX,  0, 6, 5, false },
  { FR, UR, 6, 5, false },
  { FW, UW, 6, 5, false },
  { FX, UX, 6, 5, false },
  { FR, GR, 6, 5, true },
  { FW, GW, 6, 5, true },
  { FX, GX, 6, 5, true },
  { FR, OR, 6, 5, false },
  { FW, OW, 6, 5, false },
  { FX, OX, 6, 5, false },
  { 0,   0, 6, 7, true },
  { FR,  0, 6, 7, false },
  { FW,  0, 6, 7, false },
  { FX,  0, 6, 7, false },
  { FR, UR, 6, 7, false },
  { FW, UW, 6, 7, false },
  { FX, UX, 6, 7, false },
  { FR, GR, 6, 7, false },
  { FW, GW, 6, 7, false },
  { FX, GX, 6, 7, false },
  { FR, OR, 6, 7, true },
  { FW, OW, 6, 7, true },
  { FX, OX, 6, 7, true }
};

static void check_access(const check_access_case *table, size_t n)
{
  size_t i;

  for (i = 0; i < n; ++i) {
    const check_access_case *cac = &table[i];
    bool ok = rtems_filesystem_check_access(
      cac->flags,
      cac->object_mode,
      cac->object_uid,
      cac->object_gid
    );

    rtems_test_assert(ok == cac->expected_ok);
  }
}

static void test_check_access(void)
{
  rtems_user_env_t *uenv = rtems_current_user_env_get();

  rtems_test_assert(uenv->uid == 0);
  rtems_test_assert(uenv->gid == 0);
  rtems_test_assert(uenv->euid == 0);
  rtems_test_assert(uenv->egid == 0);
  rtems_test_assert(uenv->ngroups == 0);

  uenv->uid = 1;
  uenv->gid = 2;

  check_access(
    &check_access_euid_0_cases[0],
    RTEMS_ARRAY_SIZE(check_access_euid_0_cases)
  );

  uenv->euid = 3;

  check_access(
    &check_access_egid_0_cases[0],
    RTEMS_ARRAY_SIZE(check_access_egid_0_cases)
  );

  uenv->egid = 4;
  uenv->ngroups = 1;
  uenv->groups[0] = 5;

  check_access(
    &check_access_other_cases[0],
    RTEMS_ARRAY_SIZE(check_access_other_cases)
  );

  uenv->uid = 0;
  uenv->gid = 0;
  uenv->euid = 0;
  uenv->egid = 0;
  uenv->ngroups = 0;
}

static void Init(rtems_task_argument arg)
{
  TEST_BEGIN();

  test_initial_values();
  test_location_obtain();
  test_null_location_obtain();
  test_null_location_replace();
  test_null_location_get_and_replace();
  test_path_ops();
  test_user_env();
  test_check_access();

  TEST_END();
  exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_APPLICATION_DISABLE_FILESYSTEM

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
