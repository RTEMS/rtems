/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2014, 2022 embedded brains GmbH & Co. KG
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

#include <stdio.h>
#include <errno.h>

#include <sys/reent.h>
#include <sys/stat.h>

#include <rtems.h>
#include <rtems/console.h>
#include <rtems/imfs.h>
#include <rtems/libcsupport.h>
#include <rtems/sysinit.h>

#include "tmacros.h"

#ifndef _REENT_CLEANUP
#define _REENT_CLEANUP(ptr) ((ptr)->__cleanup)
#endif

const char rtems_test_name[] = "NEWLIB 1";

static const char stdio_file_path[] = "/stdio-file";

static const char non_stdio_file_path[] = "/non-stdio-file";

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
  FILE *non_stdio_file;
  int non_stdio_fd;
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

/*
 * Check that rand() is properly initialized and returns the expected sequence
 * for default seed values. A call to rand() without any previous call to
 * srand() generates the same sequence as when srand() is first called with a
 * seed value of 1.
 */
static void test_rand(void)
{
  int rv;

  rv = rand();
  rtems_test_assert(rv == 1481765933);
  rv = rand();
  rtems_test_assert(rv == 1085377743);
  rv = rand();
  rtems_test_assert(rv == 1270216262);

  srand(1);
  rv = rand();
  rtems_test_assert(rv == 1481765933);
  rv = rand();
  rtems_test_assert(rv == 1085377743);
  rv = rand();
  rtems_test_assert(rv == 1270216262);
}

/*
 * Check that lrand48() is properly initialized and returns the expected
 * sequence for default seed values. A call to lrand48() without any previous
 * call to srand48() uses default constant initializer values set in the _seed
 * member of struct _rand48.
 */
static void test_lrand48(void)
{
  long rv;

  rv = lrand48();
  rtems_test_assert(rv == 851401618);
  rv = lrand48();
  rtems_test_assert(rv == 1804928587);
  rv = lrand48();
  rtems_test_assert(rv == 758783491);
}

static void stdio_file_worker(rtems_task_argument arg)
{
  test_context *ctx = &test_instance;
  FILE *output;
  char buf[1] = { 'x' };
  size_t n;

  test_rand();
  test_lrand48();

  rtems_test_assert(_REENT_CLEANUP(_REENT) == NULL);

  output = stdout = fopen(&stdio_file_path[0], "r+");
  rtems_test_assert(stdout != NULL);

  /*
   * Check newlib's __sinit does not touch our assigned file pointer.
   */
  rtems_test_assert(_REENT_CLEANUP(_REENT) == NULL);
  rtems_test_assert(fflush(stdout) == 0);
  rtems_test_assert(_REENT_CLEANUP(_REENT) != NULL);
  rtems_test_assert(stdout == output);

  n = fwrite(&buf[0], sizeof(buf), 1, stdout);
  rtems_test_assert(n == 1);

  rtems_test_assert(ctx->current == OPEN);

  wake_up_main(ctx);

  rtems_test_assert(0);
}

static void non_stdio_file_worker(rtems_task_argument arg)
{
  test_context *ctx = &test_instance;
  FILE *fp;
  char buf[1] = { 'y' };
  size_t n;
  int fd;

  fp = ctx->non_stdio_file = fopen(&non_stdio_file_path[0], "w");
  rtems_test_assert(fp != NULL);

  /* Get file descriptor of new global file stream, store it in text context */
  fd = fileno(fp);
  rtems_test_assert(fd != -1);
  ctx->non_stdio_fd = fd;

  n = fwrite(&buf[0], sizeof(buf), 1, fp);
  rtems_test_assert(n == 1);

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

static void create_and_run_worker(test_context *ctx, rtems_task_entry entry)
{
  rtems_status_code sc;

  sc = rtems_task_create(
    rtems_build_name('W', 'O', 'R', 'K'),
    2,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx->worker_task_id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(ctx->worker_task_id, entry, 0);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  wait();

  sc = rtems_task_delete(ctx->worker_task_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

/*
 * Check that a FILE opened by a task and assigned to a stdio stream is closed
 * during thread termination. Ensure that resources are returned to the system.
 */
static void test_stdio_file(test_context *ctx)
{
  int rv;
  rtems_resource_snapshot snapshot;

  rtems_resource_snapshot_take(&snapshot);

  rv = IMFS_make_generic_node(
    &stdio_file_path[0],
    S_IFCHR | S_IRWXU | S_IRWXG | S_IRWXO,
    &node_control,
    NULL
  );
  rtems_test_assert(rv == 0);

  create_and_run_worker(ctx, stdio_file_worker);

  rv = unlink(&stdio_file_path[0]);
  rtems_test_assert(rv == 0);

  rtems_test_assert(rtems_resource_snapshot_check(&snapshot));
}

/*
 * Open a global FILE object from a task but do not assign it to a stdio
 * stream. The FILE is not closed upon thread termination.
 */
static void test_non_stdio_file(test_context *ctx)
{
  create_and_run_worker(ctx, non_stdio_file_worker);
}

/*
 * This exit handler will be called last among the functions registered with
 * atexit(). Check that stdio file descriptors are closed. The Newlib cleanup
 * handler has not yet run, so the stdio FILE objects themselves are still
 * open.
 */
static void check_after_libio_exit(void)
{
  test_context *ctx = &test_instance;
  struct stat unused;
  int rv;

  errno = 0;
  rv = fstat(0, &unused);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == EBADF);

  errno = 0;
  rv = fstat(1, &unused);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == EBADF);

  errno = 0;
  rv = fstat(2, &unused);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == EBADF);

  rtems_test_assert(stdin->_flags != 0);
  rtems_test_assert(stdout->_flags != 0);
  rtems_test_assert(stderr->_flags != 0);

  /*
   * The non-stdio file and its file descriptor should be still open at this
   * point.
   */
  rv = fstat(ctx->non_stdio_fd, &unused);
  rtems_test_assert(rv == 0);
  rtems_test_assert(ctx->non_stdio_file->_flags != 0);
}

static void register_exit_handler_before_libio_exit(void)
{
  int rv;

  rv = atexit(check_after_libio_exit);
  rtems_test_assert(rv == 0);
}

/*
 * Register the exit handler before rtems_libio_exit() so that
 * check_after_libio_exit() is called after rtems_libio_exit().  The exit()
 * handlers are in a LIFO list.
 */
RTEMS_SYSINIT_ITEM(register_exit_handler_before_libio_exit,
    RTEMS_SYSINIT_STD_FILE_DESCRIPTORS, RTEMS_SYSINIT_ORDER_FIRST);

/*
 * At this point, neither the functions registered with atexit() nor the Newlib
 * cleanup procedures have been called. Therefore, stdio file descriptors
 * should be open and stdio FILE object flags should be non-zero.
 */
static void test_exit_handling(test_context *ctx)
{
  struct stat unused;
  int rv;

  rv = fstat(0, &unused);
  rtems_test_assert(rv == 0);

  rv = fstat(1, &unused);
  rtems_test_assert(rv == 0);

  rv = fstat(2, &unused);
  rtems_test_assert(rv == 0);

  rtems_test_assert(stdin->_flags != 0);
  rtems_test_assert(stdout->_flags != 0);
  rtems_test_assert(stderr->_flags != 0);

  /*
   * The file descriptor of the non-stdio file should still be open; the FILE
   * object flags should still be non-zero.
   */
  rv = fstat(ctx->non_stdio_fd, &unused);
  rtems_test_assert(rv == 0);
  rtems_test_assert(ctx->non_stdio_file->_flags != 0);

  /* Run exit handlers and Newlib cleanup procedures */
  exit(0);
}

static void Init(rtems_task_argument arg)
{
  test_context *ctx = &test_instance;
  FILE *file;
  int rv;

  TEST_BEGIN();
  test_rand();
  test_lrand48();

  ctx->main_task_id = rtems_task_self();

  /* Fill dynamic file pool in Newlib */
  file = fopen(CONSOLE_DEVICE_NAME, "r+");
  rtems_test_assert(file != NULL);
  rv = fclose(file);
  rtems_test_assert(rv == 0);

  test_stdio_file(ctx);
  test_non_stdio_file(ctx);
  test_exit_handling(ctx);
}

static void fatal_extension(
  rtems_fatal_source source,
  bool always_set_to_false,
  rtems_fatal_code error
)
{
  if (
    source == RTEMS_FATAL_SOURCE_EXIT
      && !always_set_to_false
      && error == 0
  ) {
    /*
     * Final conditions check after exit handlers and Newlib cleanup procedures
     * have run. File descriptors and FILE objects themselves are closed.
     */
    struct stat unused;
    int rv;
    test_context *ctx = &test_instance;

    errno = 0;
    rv = fstat(0, &unused);
    rtems_test_assert(rv == -1);
    rtems_test_assert(errno == EBADF);

    errno = 0;
    rv = fstat(1, &unused);
    rtems_test_assert(rv == -1);
    rtems_test_assert(errno == EBADF);

    errno = 0;
    rv = fstat(2, &unused);
    rtems_test_assert(rv == -1);
    rtems_test_assert(errno == EBADF);

    rtems_test_assert(stdin->_flags == 0);
    rtems_test_assert(stdout->_flags == 0);
    rtems_test_assert(stderr->_flags == 0);

    /* The non-stdio file and its file descriptor should be closed */
    errno = 0;
    rv = fstat(ctx->non_stdio_fd, &unused);
    rtems_test_assert(rv == -1);
    rtems_test_assert(errno == EBADF);
    rtems_test_assert(ctx->non_stdio_file->_flags == 0);

    TEST_END();
  }
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 5

#define CONFIGURE_MAXIMUM_TASKS 2

#define CONFIGURE_INITIAL_EXTENSIONS \
  { .fatal = fatal_extension }, \
  RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
