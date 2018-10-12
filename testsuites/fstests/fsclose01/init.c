/*
 * Copyright (c) 2012, 2018 embedded brains GmbH.  All rights reserved.
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
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <rtems/imfs.h>
#include <rtems/malloc.h>
#include <rtems/libcsupport.h>

#include <tmacros.h>

const char rtems_test_name[] = "FSCLOSE 1";

typedef enum {
  ACTION_ClOSE,
  ACTION_FCNTL,
  ACTION_FDATASYNC,
  ACTION_FCHDIR,
  ACTION_FCHMOD,
  ACTION_FCHOWN,
  /* ACTION_FPATHCONF, not easy to test */
  ACTION_FSTAT,
  ACTION_FSYNC,
  ACTION_FTRUNCATE,
  ACTION_IOCTL,
  ACTION_LSEEK,
  ACTION_READ,
  ACTION_READV,
  ACTION_WRITE,
  ACTION_WRITEV
} test_action;

typedef struct {
  rtems_id worker_id;
  int fd;
  test_action action;
  bool wait_in_close;
  bool wait_in_fstat;
  int close_count;
  int fcntl_count;
  int fdatasync_count;
  int fstat_count;
  int fsync_count;
  int ftruncate_count;
  int ioctl_count;
  int lseek_count;
  int open_count;
  int read_count;
  int readv_count;
  int write_count;
  int writev_count;
} test_context;

static test_context test_instance;

static void wait(void)
{
  rtems_status_code sc;

  sc = rtems_event_transient_receive(RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void wakeup_worker(const test_context *ctx)
{
  rtems_status_code sc;

  sc = rtems_event_transient_send(ctx->worker_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static int handler_open(
  rtems_libio_t *iop,
  const char *path,
  int oflag,
  mode_t mode
)
{
  test_context *ctx;

  ctx = IMFS_generic_get_context_by_iop(iop);
  ++ctx->open_count;

  return 0;
}

static int handler_close(
  rtems_libio_t *iop
)
{
  test_context *ctx;

  ctx = IMFS_generic_get_context_by_iop(iop);
  ++ctx->close_count;

  if (ctx->wait_in_close) {
    ctx->wait_in_close = false;
    wait();
  }

  return 0;
}

static ssize_t handler_read(
  rtems_libio_t *iop,
  void *buffer,
  size_t count
)
{
  test_context *ctx;

  ctx = IMFS_generic_get_context_by_iop(iop);
  ++ctx->read_count;

  wait();
  return 0;
}

static ssize_t handler_write(
  rtems_libio_t *iop,
  const void *buffer,
  size_t count
)
{
  test_context *ctx;

  ctx = IMFS_generic_get_context_by_iop(iop);
  ++ctx->write_count;

  wait();
  return 0;
}

static int handler_ioctl(
  rtems_libio_t *iop,
  ioctl_command_t request,
  void *buffer
)
{
  test_context *ctx;

  ctx = IMFS_generic_get_context_by_iop(iop);
  ++ctx->ioctl_count;

  wait();
  return 0;
}

static off_t handler_lseek(
  rtems_libio_t *iop,
  off_t length,
  int whence
)
{
  test_context *ctx;

  ctx = IMFS_generic_get_context_by_iop(iop);
  ++ctx->lseek_count;

  wait();
  return 0;
}

static int handler_fstat(
  const rtems_filesystem_location_info_t *loc,
  struct stat *buf
)
{
  test_context *ctx;

  buf->st_mode = S_IFCHR | S_IRWXU | S_IRWXG | S_IRWXO;
  ctx = IMFS_generic_get_context_by_location(loc);
  ++ctx->fstat_count;

  if (ctx->wait_in_fstat) {
    ctx->wait_in_fstat = false;
    wait();
  }

  return 0;
}

static int handler_ftruncate(
  rtems_libio_t *iop,
  off_t length
)
{
  test_context *ctx;

  ctx = IMFS_generic_get_context_by_iop(iop);
  ++ctx->ftruncate_count;

  wait();
  return 0;
}

static int handler_fsync(
  rtems_libio_t *iop
)
{
  test_context *ctx;

  ctx = IMFS_generic_get_context_by_iop(iop);
  ++ctx->fsync_count;

  wait();
  return 0;
}

static int handler_fdatasync(
  rtems_libio_t *iop
)
{
  test_context *ctx;

  ctx = IMFS_generic_get_context_by_iop(iop);
  ++ctx->fdatasync_count;

  wait();
  return 0;
}

static int handler_fcntl(
  rtems_libio_t *iop,
  int cmd
)
{
  test_context *ctx;

  ctx = IMFS_generic_get_context_by_iop(iop);
  ++ctx->fcntl_count;

  wait();
  return 0;
}

static ssize_t handler_readv(
  rtems_libio_t *iop,
  const struct iovec *iov,
  int iovcnt,
  ssize_t total
)
{
  test_context *ctx;

  ctx = IMFS_generic_get_context_by_iop(iop);
  ++ctx->readv_count;

  wait();
  return 0;
}

static ssize_t handler_writev(
  rtems_libio_t *iop,
  const struct iovec *iov,
  int iovcnt,
  ssize_t total
)
{
  test_context *ctx;

  ctx = IMFS_generic_get_context_by_iop(iop);
  ++ctx->writev_count;

  wait();
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

static const IMFS_node_control node_control = {
  .handlers = &node_handlers,
  .node_initialize = IMFS_node_initialize_generic,
  .node_remove = IMFS_node_remove_default,
  .node_destroy = IMFS_node_destroy_default
};

static void worker_task(rtems_task_argument arg)
{
  test_context *ctx;
  int rv;
  char buf[1];
  ssize_t n;
  off_t off;
  struct iovec iov = {
    .iov_base = &buf[0],
    .iov_len = sizeof(buf)
  };
  struct stat st;

  ctx = (test_context *) arg;

  while (true) {
    wait();

    switch (ctx->action) {
      case ACTION_ClOSE:
        ctx->wait_in_close = true;
        rv = close(ctx->fd);
        rtems_test_assert(rv == 0);
        break;
      case ACTION_FCNTL:
        rv = fcntl(ctx->fd, F_GETFD);
        rtems_test_assert(rv >= 0);
        break;
      case ACTION_FDATASYNC:
        rv = fdatasync(ctx->fd);
        rtems_test_assert(rv == 0);
        break;
      case ACTION_FCHDIR:
        ctx->wait_in_fstat = true;
        rv = fchdir(ctx->fd);
        rtems_test_assert(rv == -1);
        rtems_test_assert(errno == ENOTDIR);
        break;
      case ACTION_FCHMOD:
        rv = fstat(ctx->fd, &st);
        rtems_test_assert(rv == 0);
        ctx->wait_in_fstat = true;
        rv = fchmod(ctx->fd, st.st_mode);
        rtems_test_assert(rv == 0);
        break;
      case ACTION_FCHOWN:
        rv = fstat(ctx->fd, &st);
        rtems_test_assert(rv == 0);
        ctx->wait_in_fstat = true;
        rv = fchown(ctx->fd, st.st_uid, st.st_gid);
        rtems_test_assert(rv == 0);
        break;
      case ACTION_FSTAT:
        ctx->wait_in_fstat = true;
        rv = fstat(ctx->fd, &st);
        rtems_test_assert(rv == 0);
        break;
      case ACTION_FSYNC:
        rv = fsync(ctx->fd);
        rtems_test_assert(rv == 0);
        break;
      case ACTION_FTRUNCATE:
        rv = ftruncate(ctx->fd, 0);
        rtems_test_assert(rv == 0);
        break;
      case ACTION_IOCTL:
        rv = ioctl(ctx->fd, 0);
        rtems_test_assert(rv == 0);
        break;
      case ACTION_LSEEK:
        off = lseek(ctx->fd, 0, SEEK_SET);
        rtems_test_assert(off == 0);
        break;
      case ACTION_READ:
        n = read(ctx->fd, buf, sizeof(buf));
        rtems_test_assert(n == 0);
        break;
      case ACTION_READV:
        n = readv(ctx->fd, &iov, 1);
        rtems_test_assert(n == 0);
        break;
      case ACTION_WRITE:
        n = write(ctx->fd, buf, sizeof(buf));
        rtems_test_assert(n == 0);
        break;
      case ACTION_WRITEV:
        n = writev(ctx->fd, &iov, 1);
        rtems_test_assert(n == 0);
        break;
      default:
        rtems_test_assert(0);
        break;
    }
  }
}

static void test_fd_free_fifo(const char *path)
{
  int a;
  int b;
  int rv;

  a = open(path, O_RDWR);
  rtems_test_assert(a >= 0);

  rv = close(a);
  rtems_test_assert(rv == 0);

  b = open(path, O_RDWR);
  rtems_test_assert(b >= 0);

  rv = close(b);
  rtems_test_assert(rv == 0);

  rtems_test_assert(a != b);
}

static void test_close(test_context *ctx)
{
  const char *path = "generic";
  int rv;
  rtems_status_code sc;
  test_action ac;

  rv = IMFS_make_generic_node(
    path,
    S_IFCHR | S_IRWXU | S_IRWXG | S_IRWXO,
    &node_control,
    ctx
  );
  rtems_test_assert(rv == 0);

  test_fd_free_fifo(path);

  sc = rtems_task_create(
    rtems_build_name('W', 'O', 'R', 'K'),
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx->worker_id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(
    ctx->worker_id,
    worker_task,
    (rtems_task_argument) ctx
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  for (ac = ACTION_ClOSE; ac <= ACTION_WRITEV; ++ac) {
    ctx->action = ac;
    ctx->fd = open(path, O_RDWR);
    rtems_test_assert(ctx->fd >= 0);

    wakeup_worker(ctx);
    rv = close(ctx->fd);
    rtems_test_assert(rv == -1);

    if (ac == ACTION_ClOSE) {
      rtems_test_assert(errno == EBADF);
    } else {
      rtems_test_assert(errno == EBUSY);
    }

    wakeup_worker(ctx);
    rv = close(ctx->fd);

    if (ac == ACTION_ClOSE) {
      rtems_test_assert(rv == -1);
      rtems_test_assert(errno == EBADF);
    } else {
      rtems_test_assert(rv == 0);
    }
  }

  sc = rtems_task_delete(ctx->worker_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rv = unlink(path);
  rtems_test_assert(rv == 0);

  rtems_test_assert(ctx->close_count == 17);
  rtems_test_assert(ctx->fcntl_count == 1);
  rtems_test_assert(ctx->fdatasync_count == 1);
  rtems_test_assert(ctx->fstat_count == 42);
  rtems_test_assert(ctx->fsync_count == 1);
  rtems_test_assert(ctx->ftruncate_count == 1);
  rtems_test_assert(ctx->ioctl_count == 1);
  rtems_test_assert(ctx->lseek_count == 1);
  rtems_test_assert(ctx->open_count == 17);
  rtems_test_assert(ctx->read_count == 1);
  rtems_test_assert(ctx->readv_count == 1);
  rtems_test_assert(ctx->write_count == 1);
  rtems_test_assert(ctx->writev_count == 1);
}

static void test_tmpfile(test_context *ctx)
{
  rtems_resource_snapshot before;
  FILE *f;
  int rv;

  rv = mkdir("/tmp", S_IRWXU | S_IRWXG | S_IRWXO);
  rtems_test_assert(rv == 0);

  f = tmpfile();
  rtems_test_assert(f != NULL);
  rv = fclose(f);
  rtems_test_assert(rv == 0);

  rtems_resource_snapshot_take(&before);
  f = tmpfile();
  rtems_test_assert(f != NULL);
  rv = fclose(f);
  rtems_test_assert(rv == 0);
  rtems_test_assert(rtems_resource_snapshot_check(&before));
}

static void Init(rtems_task_argument arg)
{
  TEST_BEGIN();
  test_close(&test_instance);
  test_tmpfile(&test_instance);
  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 5

#define CONFIGURE_MAXIMUM_TASKS 2

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_PRIORITY 2
#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT
#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_DEFAULT_MODES

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
