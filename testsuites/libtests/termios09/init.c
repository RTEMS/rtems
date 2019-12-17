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

#define INPUT_BUFFER_SIZE 64

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
  size_t input_head;
  size_t input_tail;
  unsigned char input_buf[INPUT_BUFFER_SIZE];
  int callback_counter;
} device_context;

typedef struct {
  device_context devices[DEVICE_COUNT];
  int fds[DEVICE_COUNT];
  struct termios term[DEVICE_COUNT];
  int context_switch_counter;
  rtems_id flush_task_id;
} test_context;

static test_context test_instance = {
  .devices = {
    {
      .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER("Interrupt")
    }, {
      .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER("Polled")
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
  int c;

  if (dev->input_head != dev->input_tail) {
    c = dev->input_buf[dev->input_head];
    dev->input_head = (dev->input_head + 1) % INPUT_BUFFER_SIZE;
  } else {
    c = -1;
  }

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
  ctx->term[i].c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL | ECHOPRT
    | ECHOCTL | ECHOKE | ICANON | ISIG | IEXTEN);
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
  device_context *dev = &ctx->devices[i];

  switch (i) {
    case INTERRUPT:
      rtems_termios_enqueue_raw_characters(dev->tty, &c, sizeof(c));
      break;
    case POLLED:
      dev->input_buf[dev->input_tail] = (unsigned char) c;
      dev->input_tail = (dev->input_tail + 1) % INPUT_BUFFER_SIZE;
      rtems_test_assert(dev->input_head != dev->input_tail);
      break;
    default:
      rtems_test_assert(0);
  }
}

static void enable_non_blocking(test_context *ctx, size_t i, bool enable)
{
  int flags;
  int rv;

  flags = fcntl(ctx->fds[i], F_GETFL, 0);
  rtems_test_assert(flags >= 0);

  if (enable) {
    flags |= O_NONBLOCK;
  } else {
    flags &= ~O_NONBLOCK;
  }

  rv = fcntl(ctx->fds[i], F_SETFL, flags);
  rtems_test_assert(rv == 0);
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

static void clear_set_lflag(
  test_context *ctx,
  size_t i,
  tcflag_t clear,
  tcflag_t set
)
{
  ctx->term[i].c_lflag &= ~clear;
  ctx->term[i].c_lflag |= set;
  set_term(ctx, i);
}

static void clear_set_oflag(
  test_context *ctx,
  size_t i,
  tcflag_t clear,
  tcflag_t set
)
{
  ctx->term[i].c_oflag &= ~clear;
  ctx->term[i].c_oflag |= set;
  set_term(ctx, i);
}

static void set_vmin_vtime(
  test_context *ctx,
  size_t i,
  cc_t vmin,
  cc_t vtime
)
{
  ctx->term[i].c_cc[VMIN] = vmin;
  ctx->term[i].c_cc[VTIME] = vtime;
  set_term(ctx, i);
}

static void set_veol_veol2(
  test_context *ctx,
  size_t i,
  cc_t veol,
  cc_t veol2
)
{
  ctx->term[i].c_cc[VEOL] = veol;
  ctx->term[i].c_cc[VEOL2] = veol2;
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

static void test_istrip(test_context *ctx)
{
  size_t i;

  for (i = 0; i < DEVICE_COUNT; ++i) {
    ssize_t n;
    char c;

    c = 'x';

    clear_set_iflag(ctx, i, 0, ISTRIP);

    n = read(ctx->fds[i], &c, sizeof(c));
    rtems_test_assert(n == 0);
    rtems_test_assert(c == 'x');

    input(ctx, i, '\376');

    n = read(ctx->fds[i], &c, sizeof(c));
    rtems_test_assert(n == 1);
    rtems_test_assert(c == '~');

    clear_set_iflag(ctx, i, ISTRIP, 0);
    input(ctx, i, '\376');

    n = read(ctx->fds[i], &c, sizeof(c));
    rtems_test_assert(n == 1);
    rtems_test_assert(c == '\376');
  }
}

static void test_iuclc(test_context *ctx)
{
  size_t i;

  for (i = 0; i < DEVICE_COUNT; ++i) {
    ssize_t n;
    char c;

    c = 'x';

    clear_set_iflag(ctx, i, 0, IUCLC);

    n = read(ctx->fds[i], &c, sizeof(c));
    rtems_test_assert(n == 0);
    rtems_test_assert(c == 'x');

    input(ctx, i, 'A');

    n = read(ctx->fds[i], &c, sizeof(c));
    rtems_test_assert(n == 1);
    rtems_test_assert(c == 'a');

    clear_set_iflag(ctx, i, IUCLC, 0);
    input(ctx, i, 'A');

    n = read(ctx->fds[i], &c, sizeof(c));
    rtems_test_assert(n == 1);
    rtems_test_assert(c == 'A');
  }
}

static void test_icrnl(test_context *ctx)
{
  size_t i;

  for (i = 0; i < DEVICE_COUNT; ++i) {
    ssize_t n;
    char c;

    c = 'x';

    clear_set_iflag(ctx, i, 0, ICRNL);

    n = read(ctx->fds[i], &c, sizeof(c));
    rtems_test_assert(n == 0);
    rtems_test_assert(c == 'x');

    input(ctx, i, '\r');

    n = read(ctx->fds[i], &c, sizeof(c));
    rtems_test_assert(n == 1);
    rtems_test_assert(c == '\n');

    clear_set_iflag(ctx, i, ICRNL, 0);
    input(ctx, i, '\r');

    n = read(ctx->fds[i], &c, sizeof(c));
    rtems_test_assert(n == 1);
    rtems_test_assert(c == '\r');
  }
}

static void test_inlcr(test_context *ctx)
{
  size_t i;

  for (i = 0; i < DEVICE_COUNT; ++i) {
    ssize_t n;
    char c;

    c = 'x';

    clear_set_iflag(ctx, i, 0, INLCR);

    n = read(ctx->fds[i], &c, sizeof(c));
    rtems_test_assert(n == 0);
    rtems_test_assert(c == 'x');

    input(ctx, i, '\n');

    n = read(ctx->fds[i], &c, sizeof(c));
    rtems_test_assert(n == 1);
    rtems_test_assert(c == '\r');

    clear_set_iflag(ctx, i, INLCR, 0);
    input(ctx, i, '\n');

    n = read(ctx->fds[i], &c, sizeof(c));
    rtems_test_assert(n == 1);
    rtems_test_assert(c == '\n');
  }
}

static void callback(struct termios *tty, void *arg)
{
  device_context *ctx = arg;

  ++ctx->callback_counter;
}

static void test_rx_callback(test_context *ctx)
{
  size_t i = INTERRUPT;
  device_context *dev = &ctx->devices[i];
  ssize_t n;
  char buf[3];

  buf[0] = 'x';

  dev->callback_counter = 0;
  dev->tty->tty_rcv.sw_pfn = callback;
  dev->tty->tty_rcv.sw_arg = dev;
  clear_set_lflag(ctx, i, ICANON, 0);

  set_vmin_vtime(ctx, i, 0, 0);

  n = read(ctx->fds[i], buf, 1);
  rtems_test_assert(n == 0);
  rtems_test_assert(buf[0] == 'x');

  input(ctx, i, 'a');
  rtems_test_assert(dev->callback_counter == 1);

  input(ctx, i, 'b');
  rtems_test_assert(dev->callback_counter == 1);

  n = read(ctx->fds[i], buf, 2);
  rtems_test_assert(n == 2);
  rtems_test_assert(buf[0] == 'a');
  rtems_test_assert(buf[1] == 'b');

  set_vmin_vtime(ctx, i, 2, 0);

  input(ctx, i, 'd');
  rtems_test_assert(dev->callback_counter == 1);

  input(ctx, i, 'e');
  rtems_test_assert(dev->callback_counter == 2);

  input(ctx, i, 'f');
  rtems_test_assert(dev->callback_counter == 2);

  n = read(ctx->fds[i], buf, 3);
  rtems_test_assert(n == 3);
  rtems_test_assert(buf[0] == 'd');
  rtems_test_assert(buf[1] == 'e');
  rtems_test_assert(buf[2] == 'f');

  dev->tty->tty_rcv.sw_pfn = NULL;
  dev->tty->tty_rcv.sw_arg = NULL;
}

static void test_rx_callback_icanon(test_context *ctx)
{
  size_t i = INTERRUPT;
  device_context *dev = &ctx->devices[i];
  ssize_t n;
  char buf[255];
  size_t j;

  buf[0] = 'x';

  dev->callback_counter = 0;
  dev->tty->tty_rcv.sw_pfn = callback;
  dev->tty->tty_rcv.sw_arg = dev;

  set_vmin_vtime(ctx, i, 0, 0);

  n = read(ctx->fds[i], buf, 1);
  rtems_test_assert(n == 0);
  rtems_test_assert(buf[0] == 'x');

  clear_set_lflag(ctx, i, 0, ICANON);
  set_veol_veol2(ctx, i, '1', '2');

  input(ctx, i, '\n');
  rtems_test_assert(dev->callback_counter == 1);

  input(ctx, i, 'a');
  rtems_test_assert(dev->callback_counter == 1);

  input(ctx, i, '\n');
  rtems_test_assert(dev->callback_counter == 1);

  n = read(ctx->fds[i], buf, 2);
  rtems_test_assert(n == 1);
  rtems_test_assert(buf[0] == '\n');
  n = read(ctx->fds[i], buf, 3);
  rtems_test_assert(n == 2);
  rtems_test_assert(buf[0] == 'a');
  rtems_test_assert(buf[1] == '\n');

  input(ctx, i, '\4');
  rtems_test_assert(dev->callback_counter == 2);

  input(ctx, i, 'b');
  rtems_test_assert(dev->callback_counter == 2);

  input(ctx, i, '\n');
  rtems_test_assert(dev->callback_counter == 2);

  n = read(ctx->fds[i], buf, 1);
  rtems_test_assert(n == 0);

  n = read(ctx->fds[i], buf, 3);
  rtems_test_assert(n == 2);
  rtems_test_assert(buf[0] == 'b');
  rtems_test_assert(buf[1] == '\n');

  /* EOL */
  input(ctx, i, '1');
  rtems_test_assert(dev->callback_counter == 3);

  input(ctx, i, 'c');
  rtems_test_assert(dev->callback_counter == 3);

  input(ctx, i, '\n');
  rtems_test_assert(dev->callback_counter == 3);

  n = read(ctx->fds[i], buf, 2);
  rtems_test_assert(n == 1);
  rtems_test_assert(buf[0] == '1');
  n = read(ctx->fds[i], buf, 3);
  rtems_test_assert(n == 2);
  rtems_test_assert(buf[0] == 'c');
  rtems_test_assert(buf[1] == '\n');

  /* EOL2 */
  input(ctx, i, '2');
  rtems_test_assert(dev->callback_counter == 4);

  input(ctx, i, 'd');
  rtems_test_assert(dev->callback_counter == 4);

  input(ctx, i, '\n');
  rtems_test_assert(dev->callback_counter == 4);

  n = read(ctx->fds[i], buf, 2);
  rtems_test_assert(n == 1);
  rtems_test_assert(buf[0] == '2');
  n = read(ctx->fds[i], buf, 3);
  rtems_test_assert(n == 2);
  rtems_test_assert(buf[0] == 'd');
  rtems_test_assert(buf[1] == '\n');

  for (j = 0; j < 255; ++j) {
    input(ctx, i, 'e');
    rtems_test_assert(dev->callback_counter == 4);
  }

  /* Raw input buffer overflow */
  input(ctx, i, 'e');
  rtems_test_assert(dev->callback_counter == 5);

  n = read(ctx->fds[i], buf, 255);
  rtems_test_assert(n == 255);

  dev->tty->tty_rcv.sw_pfn = NULL;
  dev->tty->tty_rcv.sw_arg = NULL;
  set_veol_veol2(ctx, i, '\0', '\0');
  clear_set_lflag(ctx, i, ICANON, 0);
}

static void test_read_icanon(test_context *ctx, size_t i)
{
  ssize_t n;
  char buf[3];

  clear_set_lflag(ctx, i, 0, ICANON);

  input(ctx, i, 'a');
  input(ctx, i, '\n');
  input(ctx, i, 'b');
  input(ctx, i, '\n');
  input(ctx, i, 'c');
  input(ctx, i, '\n');

  n = read(ctx->fds[i], buf, 3);
  rtems_test_assert(n == 2);
  rtems_test_assert(buf[0] == 'a');
  rtems_test_assert(buf[1] == '\n');

  n = read(ctx->fds[i], buf, 3);
  rtems_test_assert(n == 2);
  rtems_test_assert(buf[0] == 'b');
  rtems_test_assert(buf[1] == '\n');

  n = read(ctx->fds[i], buf, 3);
  rtems_test_assert(n == 2);
  rtems_test_assert(buf[0] == 'c');
  rtems_test_assert(buf[1] == '\n');

  clear_set_lflag(ctx, i, ICANON, 0);
}

static void flush_output(test_context *ctx, size_t i)
{
  if (i == INTERRUPT) {
    device_context *dev = &ctx->devices[i];
    int left;

    do {
      left = rtems_termios_dequeue_characters(dev->tty, dev->output_pending);
    } while (left > 0);
  }
}

static void clear_output(test_context *ctx, size_t i)
{
  device_context *dev = &ctx->devices[i];

  flush_output(ctx, i);
  dev->output_count = 0;
  memset(&dev->output_buf, 0, OUTPUT_BUFFER_SIZE);
}

static void test_onlret(test_context *ctx)
{
  tcflag_t oflags = OPOST | ONLRET;
  size_t i;

  for (i = 0; i < DEVICE_COUNT; ++i) {
    device_context *dev = &ctx->devices[i];
    char c;
    ssize_t n;

    dev->tty->column = 0;
    clear_output(ctx, i);

    clear_set_oflag(ctx, i, 0, oflags);

    c = 'a';
    n = write(ctx->fds[i], &c, sizeof(c));
    rtems_test_assert(n == 1);
    rtems_test_assert(dev->tty->column == 1);
    flush_output(ctx, i);
    rtems_test_assert(dev->output_count == 1);
    rtems_test_assert(dev->output_buf[0] == 'a');

    c = '\n';
    n = write(ctx->fds[i], &c, sizeof(c));
    rtems_test_assert(n == 1);
    rtems_test_assert(dev->tty->column == 0);
    flush_output(ctx, i);
    rtems_test_assert(dev->output_count == 2);
    rtems_test_assert(dev->output_buf[1] == '\n');

    clear_set_oflag(ctx, i, oflags, 0);
  }
}

static void test_onlcr(test_context *ctx)
{
  tcflag_t oflags = OPOST | ONLCR;
  size_t i;

  for (i = 0; i < DEVICE_COUNT; ++i) {
    device_context *dev = &ctx->devices[i];
    char c;
    ssize_t n;

    dev->tty->column = 0;
    clear_output(ctx, i);

    clear_set_oflag(ctx, i, 0, oflags);

    c = 'a';
    n = write(ctx->fds[i], &c, sizeof(c));
    rtems_test_assert(n == 1);
    rtems_test_assert(dev->tty->column == 1);
    flush_output(ctx, i);
    rtems_test_assert(dev->output_count == 1);
    rtems_test_assert(dev->output_buf[0] == 'a');

    c = '\n';
    n = write(ctx->fds[i], &c, sizeof(c));
    rtems_test_assert(n == 1);
    rtems_test_assert(dev->tty->column == 0);
    flush_output(ctx, i);
    rtems_test_assert(dev->output_count == 3);
    rtems_test_assert(dev->output_buf[1] == '\r');
    rtems_test_assert(dev->output_buf[2] == '\n');

    clear_set_oflag(ctx, i, oflags, 0);
  }
}

static void test_onocr(test_context *ctx)
{
  tcflag_t oflags = OPOST | ONOCR;
  size_t i;

  for (i = 0; i < DEVICE_COUNT; ++i) {
    device_context *dev = &ctx->devices[i];
    char c;
    ssize_t n;

    dev->tty->column = 0;
    clear_output(ctx, i);

    clear_set_oflag(ctx, i, 0, oflags);

    c = '\r';
    n = write(ctx->fds[i], &c, sizeof(c));
    rtems_test_assert(n == 1);
    rtems_test_assert(dev->tty->column == 0);
    flush_output(ctx, i);
    rtems_test_assert(dev->output_count == 0);

    c = 'a';
    n = write(ctx->fds[i], &c, sizeof(c));
    rtems_test_assert(n == 1);
    rtems_test_assert(dev->tty->column == 1);
    flush_output(ctx, i);
    rtems_test_assert(dev->output_count == 1);
    rtems_test_assert(dev->output_buf[0] == 'a');

    c = '\r';
    n = write(ctx->fds[i], &c, sizeof(c));
    rtems_test_assert(n == 1);
    rtems_test_assert(dev->tty->column == 0);
    flush_output(ctx, i);
    rtems_test_assert(dev->output_count == 2);
    rtems_test_assert(dev->output_buf[1] == '\r');

    clear_set_oflag(ctx, i, oflags, 0);
  }
}

static void test_ocrnl(test_context *ctx)
{
  tcflag_t oflags = OPOST | OCRNL;
  size_t i;

  for (i = 0; i < DEVICE_COUNT; ++i) {
    device_context *dev = &ctx->devices[i];
    char c;
    ssize_t n;

    dev->tty->column = 0;
    clear_output(ctx, i);

    clear_set_oflag(ctx, i, 0, oflags);

    c = '\r';
    n = write(ctx->fds[i], &c, sizeof(c));
    rtems_test_assert(n == 1);
    rtems_test_assert(dev->tty->column == 0);
    flush_output(ctx, i);
    rtems_test_assert(dev->output_count == 1);
    rtems_test_assert(dev->output_buf[0] == '\n');

    clear_set_oflag(ctx, i, oflags, 0);
  }
}

static void test_ocrnl_onlret(test_context *ctx)
{
  tcflag_t oflags = OPOST | OCRNL | ONLRET;
  size_t i;

  for (i = 0; i < DEVICE_COUNT; ++i) {
    device_context *dev = &ctx->devices[i];
    char c;
    ssize_t n;

    dev->tty->column = 0;
    clear_output(ctx, i);

    clear_set_oflag(ctx, i, 0, oflags);

    c = 'a';
    n = write(ctx->fds[i], &c, sizeof(c));
    rtems_test_assert(n == 1);
    rtems_test_assert(dev->tty->column == 1);
    flush_output(ctx, i);
    rtems_test_assert(dev->output_count == 1);
    rtems_test_assert(dev->output_buf[0] == 'a');

    c = '\r';
    n = write(ctx->fds[i], &c, sizeof(c));
    rtems_test_assert(n == 1);
    rtems_test_assert(dev->tty->column == 0);
    flush_output(ctx, i);
    rtems_test_assert(dev->output_count == 2);
    rtems_test_assert(dev->output_buf[1] == '\n');

    clear_set_oflag(ctx, i, oflags, 0);
  }
}

static void test_opost(test_context *ctx)
{
  tcflag_t oflags = OPOST;
  size_t i;

  for (i = 0; i < DEVICE_COUNT; ++i) {
    device_context *dev = &ctx->devices[i];
    char c;
    ssize_t n;

    dev->tty->column = 0;
    clear_output(ctx, i);

    clear_set_oflag(ctx, i, 0, oflags);

    c = 'a';
    n = write(ctx->fds[i], &c, sizeof(c));
    rtems_test_assert(n == 1);
    rtems_test_assert(dev->tty->column == 1);
    flush_output(ctx, i);
    rtems_test_assert(dev->output_count == 1);
    rtems_test_assert(dev->output_buf[0] == 'a');

    c = '\33';
    n = write(ctx->fds[i], &c, sizeof(c));
    rtems_test_assert(n == 1);
    rtems_test_assert(dev->tty->column == 1);
    flush_output(ctx, i);
    rtems_test_assert(dev->output_count == 2);
    rtems_test_assert(dev->output_buf[1] == '\33');

    c = '\t';
    n = write(ctx->fds[i], &c, sizeof(c));
    rtems_test_assert(n == 1);
    rtems_test_assert(dev->tty->column == 8);
    flush_output(ctx, i);
    rtems_test_assert(dev->output_count == 3);
    rtems_test_assert(dev->output_buf[2] == '\t');

    c = '\b';
    n = write(ctx->fds[i], &c, sizeof(c));
    rtems_test_assert(n == 1);
    rtems_test_assert(dev->tty->column == 7);
    flush_output(ctx, i);
    rtems_test_assert(dev->output_count == 4);
    rtems_test_assert(dev->output_buf[3] == '\b');

    c = '\r';
    n = write(ctx->fds[i], &c, sizeof(c));
    rtems_test_assert(n == 1);
    rtems_test_assert(dev->tty->column == 0);
    flush_output(ctx, i);
    rtems_test_assert(dev->output_count == 5);
    rtems_test_assert(dev->output_buf[4] == '\r');

    clear_set_oflag(ctx, i, oflags, 0);
  }
}

static void test_xtabs(test_context *ctx)
{
  tcflag_t oflags = OPOST | OXTABS;
  size_t i;

  for (i = 0; i < DEVICE_COUNT; ++i) {
    device_context *dev = &ctx->devices[i];
    char c;
    ssize_t n;

    dev->tty->column = 0;
    clear_output(ctx, i);

    clear_set_oflag(ctx, i, 0, oflags);

    c = 'a';
    n = write(ctx->fds[i], &c, sizeof(c));
    rtems_test_assert(n == 1);
    rtems_test_assert(dev->tty->column == 1);
    flush_output(ctx, i);
    rtems_test_assert(dev->output_count == 1);
    rtems_test_assert(dev->output_buf[0] == 'a');

    c = '\t';
    n = write(ctx->fds[i], &c, sizeof(c));
    rtems_test_assert(n == 1);
    rtems_test_assert(dev->tty->column == 8);
    flush_output(ctx, i);
    rtems_test_assert(dev->output_count == 8);
    rtems_test_assert(dev->output_buf[1] == ' ');
    rtems_test_assert(dev->output_buf[2] == ' ');
    rtems_test_assert(dev->output_buf[3] == ' ');
    rtems_test_assert(dev->output_buf[4] == ' ');
    rtems_test_assert(dev->output_buf[5] == ' ');
    rtems_test_assert(dev->output_buf[6] == ' ');
    rtems_test_assert(dev->output_buf[7] == ' ');

    clear_set_oflag(ctx, i, oflags, 0);
  }
}

static void test_olcuc(test_context *ctx)
{
  tcflag_t oflags = OPOST | OLCUC;
  size_t i;

  for (i = 0; i < DEVICE_COUNT; ++i) {
    device_context *dev = &ctx->devices[i];
    char c;
    ssize_t n;

    dev->tty->column = 0;
    clear_output(ctx, i);

    clear_set_oflag(ctx, i, 0, oflags);

    c = 'a';
    n = write(ctx->fds[i], &c, sizeof(c));
    rtems_test_assert(n == 1);
    rtems_test_assert(dev->tty->column == 1);
    flush_output(ctx, i);
    rtems_test_assert(dev->output_count == 1);
    rtems_test_assert(dev->output_buf[0] == 'A');

    c = 'B';
    n = write(ctx->fds[i], &c, sizeof(c));
    rtems_test_assert(n == 1);
    rtems_test_assert(dev->tty->column == 2);
    flush_output(ctx, i);
    rtems_test_assert(dev->output_count == 2);
    rtems_test_assert(dev->output_buf[1] == 'B');

    c = '9';
    n = write(ctx->fds[i], &c, sizeof(c));
    rtems_test_assert(n == 1);
    rtems_test_assert(dev->tty->column == 3);
    flush_output(ctx, i);
    rtems_test_assert(dev->output_count == 3);
    rtems_test_assert(dev->output_buf[2] == '9');

    clear_set_oflag(ctx, i, oflags, 0);
  }
}

static void
set_self_prio(rtems_task_priority prio)
{
  rtems_status_code sc;

  sc = rtems_task_set_priority(RTEMS_SELF, prio, &prio);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void flush_task(rtems_task_argument arg)
{
  test_context *ctx = (test_context *) arg;

  while (true) {
    set_self_prio(1);
    flush_output(ctx, INTERRUPT);
    set_self_prio(2);
  }
}

static void test_write(test_context *ctx)
{
  tcflag_t oflags = OPOST | ONLCR | OXTABS;
  rtems_status_code sc;
  size_t i = INTERRUPT;
  device_context *dev = &ctx->devices[i];
  char buf[OUTPUT_BUFFER_SIZE];
  ssize_t n;

  ctx->context_switch_counter = 0;

  sc = rtems_task_create(
    rtems_build_name('F', 'L', 'S', 'H'),
    2,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx->flush_task_id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(
    ctx->flush_task_id,
    flush_task,
    (rtems_task_argument) ctx
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  clear_output(ctx, i);
  memset(buf, 'a', OUTPUT_BUFFER_SIZE);

  n = write(ctx->fds[i], &buf[0], OUTPUT_BUFFER_SIZE);
  rtems_test_assert(n == OUTPUT_BUFFER_SIZE - 1);

  rtems_test_assert(ctx->context_switch_counter == 0);

  enable_non_blocking(ctx, i, true);
  n = write(ctx->fds[i], &buf[OUTPUT_BUFFER_SIZE - 1], 1);
  rtems_test_assert(n == 0);

  enable_non_blocking(ctx, i, false);
  n = write(ctx->fds[i], &buf[OUTPUT_BUFFER_SIZE - 1], 1);
  rtems_test_assert(n == 1);

  rtems_test_assert(ctx->context_switch_counter == 2);
  rtems_test_assert(dev->output_count == OUTPUT_BUFFER_SIZE);
  rtems_test_assert(memcmp(dev->output_buf, buf, OUTPUT_BUFFER_SIZE) == 0);

  clear_set_oflag(ctx, i, 0, oflags);

  /* Ensure that ONLCR output expansion is taken into account */

  dev->tty->column = 0;
  clear_output(ctx, i);
  memset(buf, 'b', OUTPUT_BUFFER_SIZE - 1);
  buf[OUTPUT_BUFFER_SIZE - 2] = '\n';

  n = write(ctx->fds[i], &buf[0], OUTPUT_BUFFER_SIZE - 3);
  rtems_test_assert(n == OUTPUT_BUFFER_SIZE - 3);

  rtems_test_assert(ctx->context_switch_counter == 2);

  n = write(ctx->fds[i], &buf[OUTPUT_BUFFER_SIZE - 3], 2);
  rtems_test_assert(n == 1);

  rtems_test_assert(ctx->context_switch_counter == 2);

  enable_non_blocking(ctx, i, true);
  n = write(ctx->fds[i], &buf[OUTPUT_BUFFER_SIZE - 2], 1);
  rtems_test_assert(n == 0);

  enable_non_blocking(ctx, i, false);
  n = write(ctx->fds[i], &buf[OUTPUT_BUFFER_SIZE - 2], 1);
  rtems_test_assert(n == 1);

  rtems_test_assert(ctx->context_switch_counter == 4);
  rtems_test_assert(dev->output_count == OUTPUT_BUFFER_SIZE);
  buf[OUTPUT_BUFFER_SIZE - 2] = '\r';
  buf[OUTPUT_BUFFER_SIZE - 1] = '\n';
  rtems_test_assert(memcmp(dev->output_buf, buf, OUTPUT_BUFFER_SIZE) == 0);

  /* Ensure that OXTABS output expansion is taken into account */

  dev->tty->column = 0;
  clear_output(ctx, i);
  memset(buf, 'c', OUTPUT_BUFFER_SIZE - 8);
  buf[OUTPUT_BUFFER_SIZE - 8] = '\t';

  n = write(ctx->fds[i], &buf[0], OUTPUT_BUFFER_SIZE - 9);
  rtems_test_assert(n == OUTPUT_BUFFER_SIZE - 9);

  rtems_test_assert(ctx->context_switch_counter == 4);

  n = write(ctx->fds[i], &buf[OUTPUT_BUFFER_SIZE - 9], 2);
  rtems_test_assert(n == 1);

  rtems_test_assert(ctx->context_switch_counter == 4);

  enable_non_blocking(ctx, i, true);
  n = write(ctx->fds[i], &buf[OUTPUT_BUFFER_SIZE - 8], 1);
  rtems_test_assert(n == 0);

  enable_non_blocking(ctx, i, false);
  n = write(ctx->fds[i], &buf[OUTPUT_BUFFER_SIZE - 8], 1);
  rtems_test_assert(n == 1);

  rtems_test_assert(ctx->context_switch_counter == 6);
  rtems_test_assert(dev->output_count == OUTPUT_BUFFER_SIZE);
  memset(&buf[OUTPUT_BUFFER_SIZE - 8], ' ', 8);
  rtems_test_assert(memcmp(dev->output_buf, buf, OUTPUT_BUFFER_SIZE) == 0);

  clear_set_oflag(ctx, i, oflags, 0);

  sc = rtems_task_delete(ctx->flush_task_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void test_tx_callback(test_context *ctx)
{
  size_t i = INTERRUPT;
  device_context *dev = &ctx->devices[i];
  char c;
  ssize_t n;

  clear_output(ctx, i);
  dev->callback_counter = 0;
  dev->tty->tty_snd.sw_pfn = callback;
  dev->tty->tty_snd.sw_arg = dev;

  c = 'a';
  n = write(ctx->fds[i], &c, sizeof(c));
  rtems_test_assert(n == 1);
  rtems_test_assert(dev->callback_counter == 0);
  flush_output(ctx, i);
  rtems_test_assert(dev->callback_counter == 1);
  rtems_test_assert(dev->output_count == 1);
  rtems_test_assert(dev->output_buf[0] == 'a');

  dev->tty->tty_snd.sw_pfn = NULL;
  dev->tty->tty_snd.sw_arg = NULL;
}

static void Init(rtems_task_argument arg)
{
  test_context *ctx = &test_instance;

  TEST_BEGIN();

  setup(ctx);
  test_igncr(ctx);
  test_istrip(ctx);
  test_iuclc(ctx);
  test_icrnl(ctx);
  test_inlcr(ctx);
  test_rx_callback(ctx);
  test_rx_callback_icanon(ctx);
  test_read_icanon(ctx, INTERRUPT);
  test_read_icanon(ctx, POLLED);
  test_onlret(ctx);
  test_onlcr(ctx);
  test_onocr(ctx);
  test_ocrnl(ctx);
  test_ocrnl_onlret(ctx);
  test_opost(ctx);
  test_xtabs(ctx);
  test_olcuc(ctx);
  test_write(ctx);
  test_tx_callback(ctx);

  TEST_END();
  rtems_test_exit(0);
}

static void switch_extension(Thread_Control *executing, Thread_Control *heir)
{
  test_context *ctx = &test_instance;

  ++ctx->context_switch_counter;
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 5

#define CONFIGURE_MAXIMUM_TASKS 2

#define CONFIGURE_MAXIMUM_SEMAPHORES 7

#define CONFIGURE_INITIAL_EXTENSIONS \
  { .thread_switch = switch_extension }, \
  RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
