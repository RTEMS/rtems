/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2017, 2020 embedded brains GmbH & Co. KG
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

#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include <rtems/imfs.h>
#include <rtems/libio_.h>
#include <rtems/test-info.h>
#include <rtems/test.h>

const char rtems_test_name[] = "SPINTRCRITICAL 24";

typedef struct {
  int fd;
  rtems_libio_t *iop;
  long early_count;
  long late_count;
  long potential_hit_count;
  long append_count;
  long no_append_count;
  volatile bool closed;
} test_context;

static const char path[] = "generic";

static int handler_close(rtems_libio_t *iop)
{
  test_context *ctx;

  ctx = IMFS_generic_get_context_by_iop(iop);
  ctx->closed = true;

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

static T_interrupt_test_state interrupt(void *arg)
{
  test_context *ctx;
  T_interrupt_test_state state;
  int rv;
  unsigned int flags;

  state = T_interrupt_test_get_state();

  if (state != T_INTERRUPT_TEST_ACTION) {
    return T_INTERRUPT_TEST_CONTINUE;
  }

  ctx = arg;
  flags = rtems_libio_iop_flags_set(ctx->iop, 0);

  if ((flags & LIBIO_FLAGS_OPEN) != 0) {
    ++ctx->early_count;
    state = T_INTERRUPT_TEST_EARLY;
  } else if (ctx->closed) {
    ++ctx->late_count;
    state = T_INTERRUPT_TEST_LATE;
  } else {
    ++ctx->potential_hit_count;

    if (ctx->potential_hit_count >= 13) {
      state = T_INTERRUPT_TEST_DONE;
    } else {
      state = T_INTERRUPT_TEST_CONTINUE;
    }
  }

  rv = fcntl(ctx->fd, F_SETFL, O_APPEND);

  if (rv != 0) {
    T_quiet_psx_error(rv, EBADF);
  }

  return state;
}

static void prepare(void *arg)
{
  test_context *ctx;

  ctx = arg;

  ctx->fd = open(path, O_RDWR);
  T_quiet_ge_int(ctx->fd, 0);

  ctx->closed = false;
  ctx->iop = rtems_libio_iop(ctx->fd);
}

static void action(void *arg)
{
  test_context *ctx;
  int rv;

  ctx = arg;

  rv = close(ctx->fd);
  T_quiet_psx_success(rv);

  T_interrupt_test_busy_wait_for_interrupt();
}

static const T_interrupt_test_config config = {
  .prepare = prepare,
  .action = action,
  .interrupt = interrupt,
  .max_iteration_count = 10000
};

T_TEST_CASE(CloseInterrupt)
{
  test_context ctx;
  const char *path = "generic";
  int rv;
  T_interrupt_test_state state;

  memset(&ctx, 0, sizeof(ctx));

  rv = IMFS_make_generic_node(
    path,
    S_IFCHR | S_IRWXU | S_IRWXG | S_IRWXO,
    &node_control,
    &ctx
  );
  T_psx_success(rv);

  state = T_interrupt_test(&config, &ctx);
  T_eq_int(state, T_INTERRUPT_TEST_DONE);

  T_log(T_NORMAL, "early count = %ld", ctx.early_count);
  T_log(T_NORMAL, "late count = %ld", ctx.late_count);
  T_log(T_NORMAL, "potential hit count = %ld", ctx.potential_hit_count);
  T_log(T_NORMAL, "append count = %ld", ctx.append_count);
  T_log(T_NORMAL, "no append count = %ld", ctx.no_append_count);

  /* There is no reliable indicator if the test case has been hit */
  T_gt_int(ctx.early_count, 0);
  T_gt_int(ctx.late_count, 0);
  T_gt_int(ctx.potential_hit_count, 0);
  T_gt_int(ctx.append_count, 0);
  T_gt_int(ctx.no_append_count, 0);

  rv = unlink(path);
  T_psx_success(rv);
}

static void Init(rtems_task_argument arg)
{
  rtems_test_run(arg, TEST_STATE);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MICROSECONDS_PER_TICK 1000

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 4

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
