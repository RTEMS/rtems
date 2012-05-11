/*
 * Copyright (c) 2011 embedded brains GmbH.  All rights reserved.
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

#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include <rtems.h>
#include <tmacros.h>

#include <rtems/rtems_bsdnet.h>
#include <rtems/ftpd.h>
#include <rtems/ftpfs.h>

/* forward declarations to avoid warnings */
static rtems_task Init(rtems_task_argument argument);

struct rtems_bsdnet_config rtems_bsdnet_config;

#define FTP_WORKER_TASK_COUNT 2

#define FTP_WORKER_TASK_EXTRA_STACK (FTP_WORKER_TASK_COUNT * FTPD_STACKSIZE)

struct rtems_ftpd_configuration rtems_ftpd_configuration = {
  .priority = 90,
  .max_hook_filesize = 0,
  .port = 21,
  .hooks = NULL,
  .root = NULL,
  .tasks_count = FTP_WORKER_TASK_COUNT,
  .idle = 0,
  .access = 0
};

static const char content [] =
"                      LICENSE INFORMATION\n"
"\n"
"RTEMS is free software; you can redistribute it and/or modify it under\n"
"terms of the GNU General Public License as published by the\n"
"Free Software Foundation; either version 2, or (at your option) any\n"
"later version.  RTEMS is distributed in the hope that it will be useful,\n"
"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU\n"
"General Public License for more details. You should have received\n"
"a copy of the GNU General Public License along with RTEMS; see\n"
"file COPYING. If not, write to the Free Software Foundation, 675\n"
"Mass Ave, Cambridge, MA 02139, USA.\n"
"\n"
"As a special exception, including RTEMS header files in a file,\n"
"instantiating RTEMS generics or templates, or linking other files\n"
"with RTEMS objects to produce an executable application, does not\n"
"by itself cause the resulting executable application to be covered\n"
"by the GNU General Public License. This exception does not\n"
"however invalidate any other reasons why the executable file might be\n"
"covered by the GNU Public License.\n";

static void initialize_ftpfs(void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  int rv = 0;
  struct timeval to = {
    .tv_sec = 10,
    .tv_usec = 0
  };
  const char *target = RTEMS_FTPFS_MOUNT_POINT_DEFAULT;

  rv = mount_and_make_target_path(
    NULL,
    target,
    RTEMS_FILESYSTEM_TYPE_FTPFS,
    RTEMS_FILESYSTEM_READ_WRITE,
    NULL
  );
  rtems_test_assert(rv == 0);

  sc = rtems_ftpfs_set_verbose(target, true);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_ftpfs_set_timeout(target, &to);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void change_self_priority(void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_task_priority cur = 0;

  sc = rtems_task_set_priority(RTEMS_SUCCESSFUL, 110, &cur);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void create_file(const char *path)
{
  int rv = 0;
  int fd = open(path, O_WRONLY);
  ssize_t n = 0;

  rtems_test_assert(fd >= 0);

  n = write(fd, &content [0], sizeof(content));
  rtems_test_assert(n == (ssize_t) sizeof(content));

  rv = close(fd);
  rtems_test_assert(rv == 0);
}

static void copy_file(const char *src_path, const char *dest_path)
{
  int rv = 0;
  int in = open(src_path, O_RDONLY);
  int out = open(dest_path, O_WRONLY);
  ssize_t n_in = 0;
  char buf [64];

  rtems_test_assert(in >= 0);
  rtems_test_assert(out >= 0);

  while ((n_in = read(in, buf, sizeof(buf))) > 0) {
    ssize_t n_out = write(out, buf, (size_t) n_in);
    rtems_test_assert(n_out == n_in);
  }

  rv = close(out);
  rtems_test_assert(rv == 0);

  rv = close(in);
  rtems_test_assert(rv == 0);
}

static void check_file(const char *path)
{
  int rv = 0;
  int fd = open(path, O_RDONLY);
  ssize_t n = 0;
  char buf [64];
  const char *current = &content [0];
  size_t done = 0;

  rtems_test_assert(fd >= 0);

  while ((n = read(fd, buf, sizeof(buf))) > 0) {
    done += (size_t) n;
    rtems_test_assert(done <= sizeof(content));
    rtems_test_assert(memcmp(current, buf, (size_t) n) == 0);
    current += (size_t) n;
  }

  rtems_test_assert(done == sizeof(content));

  rv = close(fd);
  rtems_test_assert(rv == 0);
}

static void test(void)
{
  int rv = 0;
  const char file_a [] = "/FTP/127.0.0.1/a.txt";
  const char file_b [] = "/FTP/127.0.0.1/b.txt";

  rv = rtems_bsdnet_initialize_network();
  rtems_test_assert(rv == 0);

  rv = rtems_initialize_ftpd();
  rtems_test_assert(rv == 0);

  initialize_ftpfs();
  change_self_priority();
  create_file(file_a);
  copy_file(file_a, file_b);
  check_file(file_b);
}

static rtems_task Init(rtems_task_argument argument)
{
  puts("\n\n*** TEST FTP 1 ***");
  test();
  puts("*** END OF TEST FTP 1 ***");

  rtems_test_exit(0);
}

#define CONFIGURE_INIT

#define CONFIGURE_MICROSECONDS_PER_TICK 10000

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 14

#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM
#define CONFIGURE_FILESYSTEM_FTPFS

#define CONFIGURE_MAXIMUM_DRIVERS 2

#define CONFIGURE_MAXIMUM_TASKS (3 + FTP_WORKER_TASK_COUNT)
#define CONFIGURE_MAXIMUM_SEMAPHORES 2

#define CONFIGURE_EXTRA_TASK_STACKS FTP_WORKER_TASK_EXTRA_STACK

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#include <rtems/confdefs.h>
