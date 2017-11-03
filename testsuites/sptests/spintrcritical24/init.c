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
#include <unistd.h>
#include <errno.h>

#include <rtems/imfs.h>

#include <tmacros.h>
#include <intrcritical.h>

const char rtems_test_name[] = "SPINTRCRITICAL 24";

typedef struct {
  int fd;
  long append_count;
  long no_append_count;
} test_context;

static test_context test_instance;

static const char path[] = "generic";

static int handler_close(rtems_libio_t *iop)
{
  test_context *ctx;

  ctx = IMFS_generic_get_context_by_iop(iop);

  if (rtems_libio_iop_is_append(iop)) {
    ++ctx->append_count;
  } else {
    ++ctx->no_append_count;
  }

  return 0;
}

static const rtems_filesystem_file_handlers_r node_handlers = {
  .open_h = rtems_filesystem_default_open,
  .close_h = handler_close,
  .fstat_h = rtems_filesystem_default_fstat,
  .fcntl_h = rtems_filesystem_default_fcntl
};

static const IMFS_node_control node_control = {
  .handlers = &node_handlers,
  .node_initialize = IMFS_node_initialize_generic,
  .node_remove = IMFS_node_remove_default,
  .node_destroy = IMFS_node_destroy_default
};

static void do_fcntl(rtems_id timer, void *arg)
{
  /* The arg is NULL */
  test_context *ctx;
  int rv;

  ctx = &test_instance;

  rv = fcntl(ctx->fd, F_SETFL, O_APPEND);

  if (rv != 0) {
    rtems_test_assert(rv == -1);
    rtems_test_assert(errno == EBADF);
  }
}

static bool test_body(void *arg)
{
  test_context *ctx;
  int rv;

  ctx = arg;

  ctx->fd = open(path, O_RDWR);
  rtems_test_assert(ctx->fd >= 0);

  rv = close(ctx->fd);
  rtems_test_assert(rv == 0);

  return false;
}

static void test(test_context *ctx)
{
  const char *path = "generic";
  int rv;

  rv = IMFS_make_generic_node(
    path,
    S_IFCHR | S_IRWXU | S_IRWXG | S_IRWXO,
    &node_control,
    ctx
  );
  rtems_test_assert(rv == 0);

  interrupt_critical_section_test(test_body, ctx, do_fcntl);

  /* There is no reliable indicator if the test case has been hit */
  rtems_test_assert(ctx->append_count > 0);
  rtems_test_assert(ctx->no_append_count > 0);

  rv = unlink(path);
  rtems_test_assert(rv == 0);
}

static void Init(rtems_task_argument arg)
{
  TEST_BEGIN();
  test(&test_instance);
  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MICROSECONDS_PER_TICK 1000

#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 4

#define CONFIGURE_MAXIMUM_TASKS 1
#define CONFIGURE_MAXIMUM_TIMERS 1
#define CONFIGURE_MAXIMUM_USER_EXTENSIONS 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
