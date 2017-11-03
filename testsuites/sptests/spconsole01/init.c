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
 * http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/param.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include <rtems.h>
#include <rtems/console.h>

#include "tmacros.h"

const char rtems_test_name[] = "SPCONSOLE 1";

#define LINE_LEN 79

static const char n_line[LINE_LEN + 1] =
"nNnNnNnNnNnNnNnNnNnNnNnNnNnNnNnNnNnNnNnNnNnNnNnNnNnNnNnNnNnNnNnNnNnNnNnNnNnNnNn";

static const char b_line[LINE_LEN + 1] =
"bBbBbBbBbBbBbBbBbBbBbBbBbBbBbBbBbBbBbBbBbBbBbBbBbBbBbBbBbBbBbBbBbBbBbBbBbBbBbBb";

static const char e_line[LINE_LEN + 1] =
"eEeEeEeEeEeEeEeEeEeEeEeEeEeEeEeEeEeEeEeEeEeEeEeEeEeEeEeEeEeEeEeEeEeEeEeEeEeEeEe";

typedef struct {
  speed_t speed;
  uint32_t value;
  const char *msg;
} speed_desc;

static const speed_desc speeds[] = {
  { .speed = B9600, .value = 9600, .msg = "9600 8N1" },
  { .speed = B19200, .value = 19200, .msg = "19200 8N1" },
  { .speed = B38400, .value = 38400, .msg = "38400 8N1" },
  { .speed = B57600, .value = 57600, .msg = "57600 8N1" },
  { .speed = B115200, .value = 115200, .msg = "115200 8N1" }
};

#define BITS_PER_CHAR 10

typedef struct {
  int fd;
  struct termios saved_term;
  struct termios term;
  char buf[9600 / BITS_PER_CHAR];
  rtems_id done;
} test_context;

static test_context test_instance;

static void drain(test_context *ctx)
{
  int rv;

  rv = tcdrain(ctx->fd);
  rtems_test_assert(rv == 0);

  /* Ensure that a hardware transmit FIFO of reasonable size is empty */
  rv = usleep(200000);
  rtems_test_assert(rv == 0);
}

static void do_begin(test_context *ctx, const char *msg)
{
  puts(n_line);
  puts(msg);
  puts(b_line);
  drain(ctx);
}

static void do_end(test_context *ctx)
{
  int rv;

  drain(ctx);

  rv = cfsetspeed(&ctx->term, cfgetospeed(&ctx->saved_term));
  rtems_test_assert(rv == 0);

  rv = tcsetattr(ctx->fd, TCSANOW, &ctx->term);
  rtems_test_assert(rv == 0);

  puts("");
  puts(e_line);
}

static void test_steps_one_task(test_context *ctx, char c)
{
  char buf[128];
  size_t i;

  for (i = 0; i < RTEMS_ARRAY_SIZE(buf); ++i) {
    size_t r;
    char *p;

    buf[i] = '\n';

    r = i;
    p = buf;
    while (r > 0) {
      ssize_t m;

      m = write(ctx->fd, p, r);
      rtems_test_assert(m > 0);

      r -= (size_t) m;
      p += (size_t) m;
    }

    if (i > 1) {
      buf[i - 2] = c;
    }

    if (i > 0) {
      buf[i - 1] = '\r';
    }
  }
}

static void test_speeds(test_context *ctx)
{
  size_t i;

  for (i = 0; i < RTEMS_ARRAY_SIZE(speeds); ++i) {
    uint64_t t0;
    uint64_t t1;
    size_t n;
    size_t r;
    int rv;

    do_begin(ctx, speeds[i].msg);

    rv = cfsetspeed(&ctx->term, speeds[i].speed);
    rtems_test_assert(rv == 0);

    rv = tcsetattr(ctx->fd, TCSANOW, &ctx->term);
    rtems_test_assert(rv == 0);

    n = speeds[i].value / BITS_PER_CHAR;
    r = n;
    t0 = rtems_clock_get_uptime_nanoseconds();

    while (r > 0) {
      ssize_t m;

      m = write(ctx->fd, ctx->buf, MIN(sizeof(ctx->buf), r));
      rtems_test_assert(m > 0);

      r -= (size_t) m;
    }

    rv = tcdrain(ctx->fd);
    rtems_test_assert(rv == 0);

    t1 = rtems_clock_get_uptime_nanoseconds();

    do_end(ctx);

    printf("transmitted chars: %zu\n", n);
    printf(
      "transmitted bits (1 start, 8 data, 1 stop): %zu\n",
      n * BITS_PER_CHAR
    );

    printf("actual transmit time:   %10" PRIu64 "ns\n", t1 - t0);
    printf("expected transmit time: 1000000000ns\n");
  }
}

static void task(test_context *ctx, char c)
{
  rtems_status_code sc;

  test_steps_one_task(ctx, c);

  sc = rtems_semaphore_release(ctx->done);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_task_suspend(RTEMS_SELF);
}

static void task_y(rtems_task_argument arg)
{
  test_context *ctx;

  ctx = (test_context *) arg;
  task(ctx, 'Y');
}

static void task_z(rtems_task_argument arg)
{
  test_context *ctx;

  ctx = (test_context *) arg;
  task(ctx, 'z');
}

static void test(test_context *ctx)
{
  int rv;
  rtems_status_code sc;
  rtems_id y;
  rtems_id z;

  sc = rtems_semaphore_create(
    rtems_build_name('D', 'O', 'N', 'E'),
    0,
    RTEMS_COUNTING_SEMAPHORE,
    0,
    &ctx->done
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  ctx->fd = open(CONSOLE_DEVICE_NAME, O_RDWR);
  rtems_test_assert(ctx->fd >= 0);

  rv = tcgetattr(ctx->fd, &ctx->saved_term);
  rtems_test_assert(rv == 0);

  ctx->term = ctx->saved_term;
  cfmakeraw(&ctx->term);

  rv = tcsetattr(ctx->fd, TCSANOW, &ctx->term);
  rtems_test_assert(rv == 0);

  memset(&ctx->buf, 'd', sizeof(ctx->buf));

  do_begin(ctx, "steps one task");
  test_steps_one_task(ctx, 'x');
  do_end(ctx);

  do_begin(ctx, "steps three tasks");

  sc = rtems_task_create(
    rtems_build_name(' ', ' ', ' ', 'Y'),
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &y
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(y, task_y, (rtems_task_argument) ctx);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_create(
    rtems_build_name(' ', ' ', ' ', 'Z'),
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &z
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(z, task_z, (rtems_task_argument) ctx);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  test_steps_one_task(ctx, 'x');

  sc = rtems_semaphore_obtain(ctx->done, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_semaphore_obtain(ctx->done, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  do_end(ctx);

  test_speeds(ctx);

  rv = close(ctx->fd);
  rtems_test_assert(rv == 0);

  sc = rtems_task_delete(y);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_delete(z);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_semaphore_delete(ctx->done);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void Init(rtems_task_argument arg)
{
  TEST_BEGIN();

  rtems_print_printer_fprintf_putc(&rtems_test_printer);
  test(&test_instance);

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 4

#define CONFIGURE_MAXIMUM_TASKS 3
#define CONFIGURE_MAXIMUM_SEMAPHORES 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_DEFAULT_MODES
#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
