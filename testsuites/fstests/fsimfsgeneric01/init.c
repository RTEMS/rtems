/*
 * Copyright (c) 2012-2014 embedded brains GmbH.  All rights reserved.
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
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <rtems/imfs.h>
#include <rtems/malloc.h>
#include <rtems/libcsupport.h>

const char rtems_test_name[] = "FSIMFSGENERIC 1";

typedef enum {
  TEST_NEW,
  TEST_INITIALIZED,
  TEST_FSTAT_OPEN_0,
  TEST_FSTAT_OPEN_1,
  TEST_OPEN,
  TEST_READ,
  TEST_WRITE,
  TEST_IOCTL,
  TEST_LSEEK,
  TEST_FTRUNCATE,
  TEST_FSYNC,
  TEST_FDATASYNC,
  TEST_FCNTL,
  TEST_READV,
  TEST_WRITEV,
  TEST_CLOSED,
  TEST_FSTAT_UNLINK,
  TEST_REMOVED,
  TEST_DESTROYED
} test_state;

static int handler_open(
  rtems_libio_t *iop,
  const char *path,
  int oflag,
  mode_t mode
)
{
  test_state *state = IMFS_generic_get_context_by_iop(iop);

  rtems_test_assert(*state == TEST_FSTAT_OPEN_1);
  *state = TEST_OPEN;

  return 0;
}

static int handler_close(
  rtems_libio_t *iop
)
{
  test_state *state = IMFS_generic_get_context_by_iop(iop);

  rtems_test_assert(*state == TEST_WRITEV);
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
  ioctl_command_t request,
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
      *state = TEST_FSTAT_OPEN_0;
      break;
    case TEST_FSTAT_OPEN_0:
      *state = TEST_FSTAT_OPEN_1;
      break;
    case TEST_CLOSED:
      *state = TEST_FSTAT_UNLINK;
      break;
    default:
      rtems_test_assert(0);
      break;
  }

  buf->st_mode = S_IFCHR | S_IRWXU | S_IRWXG | S_IRWXO;

  return 0;
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

static ssize_t handler_readv(
  rtems_libio_t *iop,
  const struct iovec *iov,
  int iovcnt,
  ssize_t total
)
{
  test_state *state = IMFS_generic_get_context_by_iop(iop);

  rtems_test_assert(*state == TEST_FCNTL);
  *state = TEST_READV;

  return 0;
}

static ssize_t handler_writev(
  rtems_libio_t *iop,
  const struct iovec *iov,
  int iovcnt,
  ssize_t total
)
{
  test_state *state = IMFS_generic_get_context_by_iop(iop);

  rtems_test_assert(*state == TEST_READV);
  *state = TEST_WRITEV;

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
  .fcntl_h = handler_fcntl,
  .readv_h = handler_readv,
  .writev_h = handler_writev
};

static IMFS_jnode_t *node_initialize(
  IMFS_jnode_t *node,
  void *arg
)
{
  test_state *state = NULL;

  node = IMFS_node_initialize_generic(node, arg);
  state = IMFS_generic_get_context_by_node(node);

  rtems_test_assert(*state == TEST_NEW);
  *state = TEST_INITIALIZED;

  return node;
}

static IMFS_jnode_t *node_remove(IMFS_jnode_t *node)
{
  test_state *state = IMFS_generic_get_context_by_node(node);

  rtems_test_assert(*state == TEST_FSTAT_UNLINK);
  *state = TEST_REMOVED;

  return node;
}

static void node_destroy(IMFS_jnode_t *node)
{
  test_state *state = IMFS_generic_get_context_by_node(node);

  rtems_test_assert(*state == TEST_REMOVED);
  *state = TEST_DESTROYED;

  IMFS_node_destroy_default(node);
}

static const IMFS_node_control node_control = {
  .handlers = &node_handlers,
  .node_initialize = node_initialize,
  .node_remove = node_remove,
  .node_destroy = node_destroy
};

static void test_imfs_make_generic_node(void)
{
  test_state state = TEST_NEW;
  int rv = 0;
  int fd = 0;
  const char *path = "generic";
  char buf [1];
  ssize_t n = 0;
  off_t off = 0;
  struct iovec iov = {
    .iov_base = &buf [0],
    .iov_len = (int) sizeof(buf)
  };

  rv = IMFS_make_generic_node(
    path,
    S_IFCHR | S_IRWXU | S_IRWXG | S_IRWXO,
    &node_control,
    &state
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

  rv = readv(fd, &iov, 1);
  rtems_test_assert(rv == 0);

  rv = writev(fd, &iov, 1);
  rtems_test_assert(rv == 0);

  rv = close(fd);
  rtems_test_assert(rv == 0);

  rv = unlink(path);
  rtems_test_assert(rv == 0);

  rtems_test_assert(state == TEST_DESTROYED);
}

static IMFS_jnode_t *node_initialize_error(
  IMFS_jnode_t *node,
  void *arg
)
{
  errno = EIO;

  return NULL;
}

static IMFS_jnode_t *node_remove_inhibited(IMFS_jnode_t *node)
{
  rtems_test_assert(false);

  return node;
}

static void node_destroy_inhibited(IMFS_jnode_t *node)
{
  rtems_test_assert(false);
}

static const IMFS_node_control node_initialization_error_control = {
  .handlers = &node_handlers,
  .node_initialize = node_initialize_error,
  .node_remove = node_remove_inhibited,
  .node_destroy = node_destroy_inhibited
};

static const rtems_filesystem_operations_table *imfs_ops;

static int other_clone(rtems_filesystem_location_info_t *loc)
{
  return (*imfs_ops->clonenod_h)(loc);
}

static void test_imfs_make_generic_node_errors(void)
{
  int rv = 0;
  const char *path = "generic";
  rtems_chain_control *chain = &rtems_filesystem_mount_table;
  rtems_filesystem_mount_table_entry_t *mt_entry =
    (rtems_filesystem_mount_table_entry_t *) rtems_chain_first(chain);
  rtems_filesystem_operations_table other_ops;
  void *opaque = NULL;
  rtems_resource_snapshot before;

  rtems_resource_snapshot_take(&before);

  errno = 0;
  rv = IMFS_make_generic_node(
    path,
    S_IFDIR | S_IRWXU | S_IRWXG | S_IRWXO,
    &node_control,
    NULL
  );
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == EINVAL);
  rtems_test_assert(rtems_resource_snapshot_check(&before));

  errno = 0;
  imfs_ops = mt_entry->ops;
  other_ops = *imfs_ops;
  other_ops.clonenod_h = other_clone;
  mt_entry->ops = &other_ops;
  rv = IMFS_make_generic_node(
    path,
    S_IFCHR | S_IRWXU | S_IRWXG | S_IRWXO,
    &node_control,
    NULL
  );
  mt_entry->ops = imfs_ops;
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == ENOTSUP);
  rtems_test_assert(rtems_resource_snapshot_check(&before));

  errno = 0;
  opaque = rtems_heap_greedy_allocate(NULL, 0);
  rv = IMFS_make_generic_node(
    path,
    S_IFCHR | S_IRWXU | S_IRWXG | S_IRWXO,
    &node_control,
    NULL
  );
  rtems_heap_greedy_free(opaque);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == ENOMEM);
  rtems_test_assert(rtems_resource_snapshot_check(&before));

  errno = 0;
  rv = IMFS_make_generic_node(
    path,
    S_IFCHR | S_IRWXU | S_IRWXG | S_IRWXO,
    &node_initialization_error_control,
    NULL
  );
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == EIO);
  rtems_test_assert(rtems_resource_snapshot_check(&before));

  errno = 0;
  rv = IMFS_make_generic_node(
    "/nil/nada",
    S_IFCHR | S_IRWXU | S_IRWXG | S_IRWXO,
    &node_control,
    NULL
  );
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == ENOENT);
  rtems_test_assert(rtems_resource_snapshot_check(&before));
}

static void Init(rtems_task_argument arg)
{
  TEST_BEGIN();

  test_imfs_make_generic_node();
  test_imfs_make_generic_node_errors();

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 4

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
