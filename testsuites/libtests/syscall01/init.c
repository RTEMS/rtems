/*
 * Copyright (c) 2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
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

#include "tmacros.h"

#include <sys/socket.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <rtems/libio.h>
#include <rtems/rtems_bsdnet.h>

/* forward declarations to avoid warnings */
static rtems_task Init(rtems_task_argument argument);

static const char open_driver_path [] = "/dev/open_driver";

struct rtems_bsdnet_config rtems_bsdnet_config;

static void test(void)
{
  int rv;
  char buf [1];
  ssize_t n;
  int fd = open(open_driver_path, O_RDWR);
  rtems_test_assert(fd >= 0);

  n = send(fd, buf, sizeof(buf), 0);
  rtems_test_assert(n == -1);
  rtems_test_assert(errno == ENOTSOCK);

  n = recv(fd, buf, sizeof(buf), 0);
  rtems_test_assert(n == -1);
  rtems_test_assert(errno == ENOTSOCK);

  rv = close(fd);
  rtems_test_assert(rv == 0);
}

static void Init(rtems_task_argument arg)
{
  int rv;

  puts("\n\n*** TEST SYSCALL 1 ***");

  rv = rtems_bsdnet_initialize_network();
  rtems_test_assert(rv == 0);

  test();

  puts("*** END OF TEST SYSCALL 1 ***");

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
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_EXTRA_DRIVERS OPEN_DRIVER

#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM

#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 4

#define CONFIGURE_MAXIMUM_TASKS 2

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
