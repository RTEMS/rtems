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
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <rtems/imfs.h>
#include <rtems/malloc.h>

typedef enum {
  TEST_NEW,
  TEST_INITIALIZED,
  TEST_FSTAT_OPEN,
  TEST_OPEN,
  TEST_READ,
  TEST_WRITE,
  TEST_IOCTL,
  TEST_LSEEK,
  TEST_FTRUNCATE,
  TEST_FSYNC,
  TEST_FDATASYNC,
  TEST_FCNTL,
  TEST_CLOSED,
  TEST_FSTAT_UNLINK,
  TEST_REMOVED,
  TEST_DESTROYED
} test_state;

static test_state global_state = TEST_NEW;

static int handler_open(
  rtems_libio_t *iop,
  const char *path,
  int oflag,
  mode_t mode
)
{
  test_state *state = IMFS_generic_get_context_by_iop(iop);

  rtems_test_assert(*state == TEST_FSTAT_OPEN);
  *state = TEST_OPEN;

  return 0;
}

static int handler_close(
  rtems_libio_t *iop
)
{
  test_state *state = IMFS_generic_get_context_by_iop(iop);

  rtems_test_assert(*state == TEST_FCNTL);
  *state = TEST_CLOSED;

  return 0;
}

static ssize_t handler_read(
  rtems_libio_t *iop,
  void *buffer,
  size_t count
)
{
  test_state *state = IMFS_generic_get_context_by_iop(iop);

  rtems_test_assert(*state == TEST_OPEN);
  *state = TEST_READ;

  return 0;
}

static ssize_t handler_write(
  rtems_libio_t *iop,
  const void *buffer,
  size_t count
)
{
  test_state *state = IMFS_generic_get_context_by_iop(iop);

  rtems_test_assert(*state == TEST_READ);
  *state = TEST_WRITE;

  return 0;
}

static int handler_ioctl(
  rtems_libio_t *iop,
  uint32_t request,
  void *buffer
)
{
  test_state *state = IMFS_generic_get_context_by_iop(iop);

  rtems_test_assert(*state == TEST_WRITE);
  *state = TEST_IOCTL;

  return 0;
}

static off_t handler_lseek(
  rtems_libio_t *iop,
  off_t length,
  int whence
)
{
  test_state *state = IMFS_generic_get_context_by_iop(iop);

  rtems_test_assert(*state == TEST_IOCTL);
  *state = TEST_LSEEK;

  return 0;
}

static int handler_fstat(
  const rtems_filesystem_location_info_t *loc,
  struct stat *buf
)
{
  test_state *state = IMFS_generic_get_context_by_location(loc);

  switch (*state) {
    case TEST_INITIALIZED:
      *state = TEST_FSTAT_OPEN;
      break;
    case TEST_CLOSED:
      *state = TEST_FSTAT_UNLINK;
      break;
    default:
      rtems_test_assert(0);
      break;
  }

  return rtems_filesystem_default_fstat(loc, buf);
}

static int handler_ftruncate(
  rtems_libio_t *iop,
  off_t length
)
{
  test_state *state = IMFS_generic_get_context_by_iop(iop);

  rtems_test_assert(*state == TEST_LSEEK);
  *state = TEST_FTRUNCATE;

  return 0;
}

static int handler_fsync(
  rtems_libio_t *iop
)
{
  test_state *state = IMFS_generic_get_context_by_iop(iop);

  rtems_test_assert(*state == TEST_FTRUNCATE);
  *state = TEST_FSYNC;

  return 0;
}

static int handler_fdatasync(
  rtems_libio_t *iop
)
{
  test_state *state = IMFS_generic_get_context_by_iop(iop);

  rtems_test_assert(*state == TEST_FSYNC);
  *state = TEST_FDATASYNC;

  return 0;
}

static int handler_fcntl(
  rtems_libio_t *iop,
  int cmd
)
{
  test_state *state = IMFS_generic_get_context_by_iop(iop);

  rtems_test_assert(*state == TEST_FDATASYNC);
  *state = TEST_FCNTL;

  return 0;
}

static const rtems_filesystem_file_handlers_r node_handlers = {
  .open_h = handler_open,
  .close_h = handler_close,
  .read_h = handler_read,
  .write_h = handler_write,
  .ioctl_h = handler_ioctl,
  .lseek_h = handler_lseek,
  .fstat_h = handler_fstat,
  .ftruncate_h = handler_ftruncate,
  .fsync_h = handler_fsync,
  .fdatasync_h = handler_fdatasync,
  .fcntl_h = handler_fcntl
};

static IMFS_jnode_t *node_initialize(
  IMFS_jnode_t *node,
  const IMFS_types_union *info
)
{
  test_state *state = NULL;

  node = IMFS_node_initialize_generic(node, info);
  state = IMFS_generic_get_context_by_node(node);

  rtems_test_assert(*state == TEST_NEW);
  *state = TEST_INITIALIZED;

  return node;
}

static IMFS_jnode_t *node_remove(
  IMFS_jnode_t *node,
  const IMFS_jnode_t *root_node
)
{
  test_state *state = IMFS_generic_get_context_by_node(node);

  rtems_test_assert(*state == TEST_FSTAT_UNLINK);
  *state = TEST_REMOVED;

  return node;
}

static IMFS_jnode_t *node_destroy(IMFS_jnode_t *node)
{
  test_state *state = IMFS_generic_get_context_by_node(node);

  rtems_test_assert(*state == TEST_REMOVED);
  *state = TEST_DESTROYED;

  return node;
}

static const IMFS_node_control node_control = {
  .imfs_type = IMFS_GENERIC,
  .handlers = &node_handlers,
  .node_initialize = node_initialize,
  .node_remove = node_remove,
  .node_destroy = node_destroy
};

static void test_imfs_make_generic_node(void)
{
  int rv = 0;
  int fd = 0;
  const char *path = "generic";
  char buf [1];
  ssize_t n = 0;
  off_t off = 0;

  rv = IMFS_make_generic_node(
    path,
    S_IFCHR | S_IRWXU | S_IRWXG | S_IRWXO,
    &node_control,
    &global_state
  );
  rtems_test_assert(rv == 0);

  fd = open(path, O_RDWR);
  rtems_test_assert(fd >= 0);

  n = read(fd, buf, sizeof(buf));
  rtems_test_assert(n == 0);

  n = write(fd, buf, sizeof(buf));
  rtems_test_assert(n == 0);

  rv = ioctl(fd, 0);
  rtems_test_assert(rv == 0);

  off = lseek(fd, off, SEEK_SET);
  rtems_test_assert(off == 0);

  rv = ftruncate(fd, 0);
  rtems_test_assert(rv == 0);

  rv = fsync(fd);
  rtems_test_assert(rv == 0);

  rv = fdatasync(fd);
  rtems_test_assert(rv == 0);

  rv = fcntl(fd, F_GETFD);
  rtems_test_assert(rv >= 0);

  rv = close(fd);
  rtems_test_assert(rv == 0);

  rv = unlink(path);
  rtems_test_assert(rv == 0);

  rtems_test_assert(global_state == TEST_DESTROYED);
}

static const IMFS_node_control node_invalid_control = {
  .imfs_type = IMFS_DIRECTORY,
  .handlers = &node_handlers,
  .node_initialize = node_initialize,
  .node_remove = node_remove,
  .node_destroy = node_destroy
};

static void test_imfs_make_generic_node_errors(void)
{
  int rv = 0;
  const char *path = "generic";
  rtems_chain_control *chain = &rtems_filesystem_mount_table;
  rtems_filesystem_mount_table_entry_t *mt_entry =
    (rtems_filesystem_mount_table_entry_t *) rtems_chain_first(chain);
  const char *type = mt_entry->type;
  void *opaque = NULL;

  errno = 0;
  rv = IMFS_make_generic_node(
    path,
    S_IFCHR | S_IRWXU | S_IRWXG | S_IRWXO,
    &node_invalid_control,
    NULL
  );
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == EINVAL);

  errno = 0;
  rv = IMFS_make_generic_node(
    path,
    S_IFDIR | S_IRWXU | S_IRWXG | S_IRWXO,
    &node_control,
    NULL
  );
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == EINVAL);

  mt_entry->type = "XXX";
  errno = 0;
  rv = IMFS_make_generic_node(
    path,
    S_IFCHR | S_IRWXU | S_IRWXG | S_IRWXO,
    &node_control,
    NULL
  );
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == ENOTSUP);
  mt_entry->type = type;

  opaque = rtems_heap_greedy_allocate(NULL, 0);
  errno = 0;
  rv = IMFS_make_generic_node(
    path,
    S_IFCHR | S_IRWXU | S_IRWXG | S_IRWXO,
    &node_control,
    NULL
  );
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == ENOMEM);
  rtems_heap_greedy_free(opaque);
}

static void Init(rtems_task_argument arg)
{
  printf("\n\n*** TEST FSIMFSGENERIC 1 ***\n");

  test_imfs_make_generic_node();
  test_imfs_make_generic_node_errors();

  printf("*** END OF TEST FSIMFSGENERIC 1 ***\n");

  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 4

#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
