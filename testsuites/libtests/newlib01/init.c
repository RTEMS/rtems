/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
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

#include <stdio.h>

#include <sys/reent.h>

#include <rtems.h>
#include <rtems/console.h>
#include <rtems/imfs.h>
#include <rtems/libcsupport.h>

#include "tmacros.h"

const char rtems_test_name[] = "NEWLIB 1";

static const char file_path[] = "/file";

typedef enum {
  INIT,
  OPEN,
  WRITTEN,
  CLOSED
} test_state;

typedef struct {
  rtems_id main_task_id;
  rtems_id worker_task_id;
  test_state current;
} test_context;

static test_context test_instance;

static void wake_up_main(const test_context *ctx)
{
  rtems_status_code sc;

  sc = rtems_event_transient_send(ctx->main_task_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void wait(void)
{
  rtems_status_code sc;

  sc = rtems_event_transient_receive(RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void worker_task(rtems_task_argument arg)
{
  test_context *ctx = &test_instance;
  struct _reent *reent = _REENT;
  FILE *output;
  char buf[1] = { 'x' };
  size_t n;

  rtems_test_assert(reent->__sdidinit == 0);

  output = stdout = fopen(&file_path[0], "r+");
  rtems_test_assert(stdout != NULL);

  /*
   * Check newlib's __sinit does not touch our assigned file pointer.
   */
  rtems_test_assert(reent->__sdidinit == 0);
  rtems_test_assert(fflush(stdout) == 0);
  rtems_test_assert(reent->__sdidinit != 0);
  rtems_test_assert(stdout == output);

  n = fwrite(&buf[0], sizeof(buf), 1, stdout);
  rtems_test_assert(n == 1);

  rtems_test_assert(ctx->current == OPEN);

  wake_up_main(ctx);

  rtems_test_assert(0);
}

static int handler_open(
  rtems_libio_t *iop,
  const char *path,
  int oflag,
  mode_t mode
)
{
  test_context *ctx = &test_instance;

  rtems_test_assert(ctx->current == INIT);
  ctx->current = OPEN;

  return 0;
}

static int handler_close(
  rtems_libio_t *iop
)
{
  test_context *ctx = &test_instance;

  rtems_test_assert(ctx->current == WRITTEN);
  ctx->current = CLOSED;

  return 0;
}

static ssize_t handler_read(
  rtems_libio_t *iop,
  void *buffer,
  size_t count
)
{
  rtems_test_assert(0);

  return 0;
}

static ssize_t handler_write(
  rtems_libio_t *iop,
  const void *buffer,
  size_t count
)
{
  test_context *ctx = &test_instance;

  rtems_test_assert(ctx->current == OPEN);
  ctx->current = WRITTEN;

  iop->offset += count;

  return (ssize_t) count;
}

static int handler_ioctl(
  rtems_libio_t *iop,
  ioctl_command_t request,
  void *buffer
)
{
  rtems_test_assert(0);

  return 0;
}

static off_t handler_lseek(
  rtems_libio_t *iop,
  off_t length,
  int whence
)
{
  rtems_test_assert(0);

  return 0;
}

static int handler_ftruncate(
  rtems_libio_t *iop,
  off_t length
)
{
  rtems_test_assert(0);

  return 0;
}

static int handler_fsync(
  rtems_libio_t *iop
)
{
  rtems_test_assert(0);

  return 0;
}

static int handler_fdatasync(
  rtems_libio_t *iop
)
{
  rtems_test_assert(0);

  return 0;
}

static int handler_fcntl(
  rtems_libio_t *iop,
  int cmd
)
{
  rtems_test_assert(0);

  return 0;
}

static ssize_t handler_readv(
  rtems_libio_t *iop,
  const struct iovec *iov,
  int iovcnt,
  ssize_t total
)
{
  rtems_test_assert(0);

  return 0;
}

static ssize_t handler_writev(
  rtems_libio_t *iop,
  const struct iovec *iov,
  int iovcnt,
  ssize_t total
)
{
  rtems_test_assert(0);

  return 0;
}

static const rtems_filesystem_file_handlers_r node_handlers = {
  .open_h = handler_open,
  .close_h = handler_close,
  .read_h = handler_read,
  .write_h = handler_write,
  .ioctl_h = handler_ioctl,
  .lseek_h = handler_lseek,
  .fstat_h = rtems_filesystem_default_fstat,
  .ftruncate_h = handler_ftruncate,
  .fsync_h = handler_fsync,
  .fdatasync_h = handler_fdatasync,
  .fcntl_h = handler_fcntl,
  .readv_h = handler_readv,
  .writev_h = handler_writev
};

static const IMFS_node_control node_control = IMFS_GENERIC_INITIALIZER(
  &node_handlers,
  IMFS_node_initialize_default,
  IMFS_node_destroy_default
);

static void test(void)
{
  test_context *ctx = &test_instance;
  rtems_status_code sc;
  int rv;
  rtems_resource_snapshot snapshot;
  FILE *file;

  ctx->main_task_id = rtems_task_self();

  /* Fill dynamic file pool in Newlib _GLOBAL_REENT */
  file = fopen(CONSOLE_DEVICE_NAME, "r+");
  rtems_test_assert(file != NULL);
  rv = fclose(file);
  rtems_test_assert(rv == 0);

  rtems_resource_snapshot_take(&snapshot);

  rv = IMFS_make_generic_node(
    &file_path[0],
    S_IFCHR | S_IRWXU | S_IRWXG | S_IRWXO,
    &node_control,
    NULL
  );
  rtems_test_assert(rv == 0);

  sc = rtems_task_create(
    rtems_build_name('W', 'O', 'R', 'K'),
    2,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx->worker_task_id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(ctx->worker_task_id, worker_task, 0);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  wait();

  sc = rtems_task_delete(ctx->worker_task_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rv = unlink(&file_path[0]);
  rtems_test_assert(rv == 0);

  rtems_test_assert(rtems_resource_snapshot_check(&snapshot));
}

static void Init(rtems_task_argument arg)
{
  TEST_BEGIN();

  test();

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 4

#define CONFIGURE_MAXIMUM_TASKS 2

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
