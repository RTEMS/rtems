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
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include <rtems/termiostypes.h>

#include "tmacros.h"

const char rtems_test_name[] = "TERMIOS 9";

#define INTERRUPT 0

#define POLLED 1

#define DEVICE_COUNT 2

#define OUTPUT_BUFFER_SIZE 64

static const char * const paths[DEVICE_COUNT] = {
  "/interrupt",
  "/polled"
};

typedef struct {
  rtems_termios_device_context base;
  rtems_termios_tty *tty;
  size_t output_pending;
  size_t output_count;
  char output_buf[OUTPUT_BUFFER_SIZE];
  int input_char;
} device_context;

typedef struct {
  device_context devices[DEVICE_COUNT];
  int fds[DEVICE_COUNT];
  struct termios term[DEVICE_COUNT];
} test_context;

static test_context test_instance = {
  .devices = {
    {
      .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER("Interrupt")
    }, {
      .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER("Polled"),
      .input_char = -1
    }
  }
};

static bool first_open(
  rtems_termios_tty *tty,
  rtems_termios_device_context *base,
  struct termios *term,
  rtems_libio_open_close_args_t *args
)
{
  device_context *dev = (device_context *) base;

  dev->tty = tty;

  return true;
}

static void write_polled(
  rtems_termios_device_context *base,
  const char *buf,
  size_t len
)
{
  device_context *dev = (device_context *) base;

  rtems_test_assert(dev->output_count + len <= OUTPUT_BUFFER_SIZE);
  memcpy(&dev->output_buf[dev->output_count], buf, len);
  dev->output_count += len;
}

static void write_interrupt(
  rtems_termios_device_context *base,
  const char *buf,
  size_t len
)
{
  device_context *dev = (device_context *) base;

  write_polled(base, buf, len);
  dev->output_pending = len;
}

static int read_polled(rtems_termios_device_context *base)
{
  device_context *dev = (device_context *) base;
  int c = dev->input_char;

  dev->input_char = -1;

  return c;
}

static const rtems_termios_device_handler handlers[DEVICE_COUNT] = {
  {
    .first_open = first_open,
    .write = write_interrupt,
    .mode = TERMIOS_IRQ_DRIVEN
  }, {
    .first_open = first_open,
    .write = write_polled,
    .poll_read = read_polled,
    .mode = TERMIOS_POLLED
  }
};

static void set_term(test_context *ctx, size_t i)
{
  int rv;

  rv = tcsetattr(ctx->fds[i], TCSANOW, &ctx->term[i]);
  rtems_test_assert(rv == 0);
}

static void init_term(test_context *ctx, size_t i)
{
  int rv;

  rv = tcgetattr(ctx->fds[i], &ctx->term[i]);
  rtems_test_assert(rv == 0);

  ctx->term[i].c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP
    | INLCR | IGNCR | ICRNL | IXON);
  ctx->term[i].c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
  ctx->term[i].c_cflag &= ~(CSIZE | PARENB);
  ctx->term[i].c_cflag |= CS8;
  ctx->term[i].c_oflag &= ~(OPOST | ONLRET | ONLCR | OCRNL | ONLRET
    | TABDLY | OLCUC);

  ctx->term[i].c_cc[VMIN] = 0;
  ctx->term[i].c_cc[VTIME] = 0;

  set_term(ctx, i);
}

static void setup(test_context *ctx)
{
  rtems_status_code sc;
  size_t i;

  rtems_termios_initialize();

  for (i = 0; i < DEVICE_COUNT; ++i) {
    sc = rtems_termios_device_install(
      paths[i],
      &handlers[i],
      NULL,
      &ctx->devices[i].base
    );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    ctx->fds[i] = open(paths[i], O_RDWR);
    rtems_test_assert(ctx->fds[i] >= 0);

    init_term(ctx, i);
  }
}

static void input(test_context *ctx, size_t i, char c)
{
  switch (i) {
    case INTERRUPT:
      rtems_termios_enqueue_raw_characters(ctx->devices[i].tty, &c, sizeof(c));
      break;
    case POLLED:
      ctx->devices[i].input_char = (unsigned char) c;
      break;
    default:
      rtems_test_assert(0);
  }
}

static void clear_set_iflag(
  test_context *ctx,
  size_t i,
  tcflag_t clear,
  tcflag_t set
)
{
  ctx->term[i].c_iflag &= ~clear;
  ctx->term[i].c_iflag |= set;
  set_term(ctx, i);
}

static void test_igncr(test_context *ctx)
{
  size_t i;

  for (i = 0; i < DEVICE_COUNT; ++i) {
    ssize_t n;
    char c;

    c = 'x';

    clear_set_iflag(ctx, i, 0, IGNCR);

    n = read(ctx->fds[i], &c, sizeof(c));
    rtems_test_assert(n == 0);
    rtems_test_assert(c == 'x');

    input(ctx, i, '\r');

    n = read(ctx->fds[i], &c, sizeof(c));
    rtems_test_assert(n == 0);
    rtems_test_assert(c == 'x');

    clear_set_iflag(ctx, i, IGNCR, 0);
    input(ctx, i, '\r');

    n = read(ctx->fds[i], &c, sizeof(c));
    rtems_test_assert(n == 1);
    rtems_test_assert(c == '\r');
  }
}

static void Init(rtems_task_argument arg)
{
  test_context *ctx = &test_instance;

  TEST_BEGIN();

  setup(ctx);
  test_igncr(ctx);

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 5

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_MAXIMUM_SEMAPHORES 7

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
