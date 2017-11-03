/*
 * Copyright (c) 2012-2015 embedded brains GmbH.  All rights reserved.
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

#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <rtems/libio.h>
#include <rtems/rtems_bsdnet.h>

const char rtems_test_name[] = "SYSCALL 1";

static const char open_driver_path [] = "/dev/open_driver";

struct rtems_bsdnet_config rtems_bsdnet_config;

typedef struct {
  rtems_id main_task;
  rtems_id close_task;
  int fd;
} test_context;

static test_context test_instance;

static void test_sync(void)
{
  int rv;
  char buf [1];
  ssize_t n;
  int fd;

  fd = open(open_driver_path, O_RDWR);
  rtems_test_assert(fd >= 0);

  errno = 0;
  n = send(fd, buf, sizeof(buf), 0);
  rtems_test_assert(n == -1);
  rtems_test_assert(errno == ENOTSOCK);

  errno = 0;
  n = recv(fd, buf, sizeof(buf), 0);
  rtems_test_assert(n == -1);
  rtems_test_assert(errno == ENOTSOCK);

  rv = close(fd);
  rtems_test_assert(rv == 0);

  fd = socket(PF_INET, SOCK_DGRAM, 0);
  rtems_test_assert(fd >= 0);

  errno = 0;
  rv = fsync(fd);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == EINVAL);

  errno = 0;
  rv = fdatasync(fd);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == EINVAL);

  rv = close(fd);
  rtems_test_assert(rv == 0);
}

static void close_task(rtems_task_argument arg)
{
  test_context *ctx = (test_context *) arg;

  while (true) {
    rtems_status_code sc;
    int rv;

    sc = rtems_event_transient_receive(RTEMS_WAIT, RTEMS_NO_TIMEOUT);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    rv = close(ctx->fd);
    rtems_test_assert(rv == 0);

    sc = rtems_event_transient_send(ctx->main_task);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }
}

static void request_close(test_context *ctx)
{
  rtems_status_code sc;

  sc = rtems_event_transient_send(ctx->close_task);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void wait_for_close_task(void)
{
  rtems_status_code sc;

  sc = rtems_event_transient_receive(RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void test_accept_and_close(test_context *ctx)
{
  int rv;
  int fd;
  struct sockaddr_in addr;
  socklen_t addrlen = sizeof(addr);

  ctx->fd = socket(PF_INET, SOCK_STREAM, 0);
  rtems_test_assert(ctx->fd >= 0);

  rv = listen(ctx->fd, 1);
  rtems_test_assert(rv == 0);

  request_close(ctx);

  errno = 0;
  fd = accept(ctx->fd, (struct sockaddr *) &addr, &addrlen);
  rtems_test_assert(fd == -1);
  rtems_test_assert(errno == ENXIO);

  errno = 0;
  fd = accept(ctx->fd, (struct sockaddr *) &addr, &addrlen);
  rtems_test_assert(fd == -1);
  rtems_test_assert(errno == EBADF);

  wait_for_close_task();
}

static void test_connect_and_close(test_context *ctx)
{
  int rv;
  struct sockaddr_in addr;
  socklen_t addrlen = sizeof(addr);

  ctx->fd = socket(PF_INET, SOCK_STREAM, 0);
  rtems_test_assert(ctx->fd >= 0);

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(1234);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);

  request_close(ctx);

  errno = 0;
  rv = connect(ctx->fd, (struct sockaddr *) &addr, addrlen);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == ENXIO);

  errno = 0;
  rv = connect(ctx->fd, (struct sockaddr *) &addr, addrlen);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == EBADF);

  wait_for_close_task();
}

static void test_recv_and_close(test_context *ctx)
{
  int rv;
  struct sockaddr_in addr;
  socklen_t addrlen = sizeof(addr);
  char buf[1];
  ssize_t n;

  ctx->fd = socket(PF_INET, SOCK_DGRAM, 0);
  rtems_test_assert(ctx->fd >= 0);

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(1234);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);

  rv = bind(ctx->fd, (struct sockaddr *) &addr, addrlen);
  rtems_test_assert(rv == 0);

  request_close(ctx);

  errno = 0;
  n = recv(ctx->fd, &buf[0], sizeof(buf), 0);
  rtems_test_assert(n == -1);
  rtems_test_assert(errno == ENXIO);

  errno = 0;
  n = recv(ctx->fd, &buf[0], sizeof(buf), 0);
  rtems_test_assert(n == -1);
  rtems_test_assert(errno == EBADF);

  wait_for_close_task();
}

static void test_select_and_close(test_context *ctx)
{
  int rv;
  struct sockaddr_in addr;
  socklen_t addrlen = sizeof(addr);
  int nfds;
  struct fd_set set;

  ctx->fd = socket(PF_INET, SOCK_DGRAM, 0);
  rtems_test_assert(ctx->fd >= 0);

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(1234);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);

  rv = bind(ctx->fd, (struct sockaddr *) &addr, addrlen);
  rtems_test_assert(rv == 0);

  nfds = ctx->fd + 1;
  FD_ZERO(&set);
  FD_SET(ctx->fd, &set);

  request_close(ctx);

  errno = 0;
  rv = select(nfds, &set, NULL, NULL, NULL);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == EBADF);

  wait_for_close_task();
}

static void Init(rtems_task_argument arg)
{
  test_context *ctx = &test_instance;
  rtems_status_code sc;
  int rv;

  TEST_BEGIN();

  ctx->main_task = rtems_task_self();

  sc = rtems_task_create(
    rtems_build_name('C', 'L', 'O', 'S'),
    2,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx->close_task
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(
    ctx->close_task,
    close_task,
    (rtems_task_argument) ctx
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rv = rtems_bsdnet_initialize_network();
  rtems_test_assert(rv == 0);

  test_sync();
  test_accept_and_close(ctx);
  test_connect_and_close(ctx);
  test_recv_and_close(ctx);
  test_select_and_close(ctx);

  sc = rtems_task_delete(ctx->close_task);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  TEST_END();

  rtems_test_exit(0);
}

static rtems_device_driver open_driver_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  rtems_status_code sc = rtems_io_register_name(open_driver_path, major, 0);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  return RTEMS_SUCCESSFUL;
}

static rtems_device_driver open_driver_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  rtems_libio_open_close_args_t *oc = arg;

  oc->iop->data0 = 1;
  oc->iop->data1 = (void *) 1;

  return RTEMS_SUCCESSFUL;
}

#define OPEN_DRIVER { \
  .initialization_entry = open_driver_initialize, \
  .open_entry = open_driver_open \
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_EXTRA_DRIVERS OPEN_DRIVER

#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 4

#define CONFIGURE_MAXIMUM_TASKS 3

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
