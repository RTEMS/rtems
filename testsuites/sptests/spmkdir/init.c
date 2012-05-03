/**
 * @file
 *
 * @ingroup tests
 *
 * @brief rtems_mkdir() test.
 */

/*
 * Copyright (c) 2010
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>

#include <rtems/libio.h>

#include "tmacros.h"

static void test_mkdir(const char *path, mode_t omode, int expected_rv)
{
  struct stat st;
  int rv = 0;
  mode_t current_umask = umask(0);
  mode_t dirmode = S_IFDIR | (omode & ~current_umask);

  umask(current_umask);

  rv = rtems_mkdir(path, omode);
  rtems_test_assert(rv == expected_rv);

  if (rv == 0) {
    rv = stat(path, &st);
    rtems_test_assert(rv == 0 && st.st_mode == dirmode);
  }
}

static rtems_task Init(rtems_task_argument argument)
{
  mode_t omode = S_IRWXU | S_IRWXG | S_IRWXO;
  int rv = 0;

  puts("\n\n*** TEST SPMKDIR ***");

  puts( "rtems_mkdir a - OK" );
  test_mkdir("a", omode, 0);
  puts( "rtems_mkdir a/b - OK" );
  test_mkdir("a/b", omode, 0);
  puts( "rtems_mkdir a/b/c/d/e/f/g/h/i - OK" );
  test_mkdir("a/b/c/d/e/f/g/h/i", omode, 0);
  puts( "rtems_mkdir a/b/c - OK" );
  test_mkdir("a/b/c", omode, 0);
  puts( "rtems_mkdir a/b/c/1 - OK" );
  test_mkdir("a/b/c/1", 0, 0);
  puts( "rtems_mkdir a/b/c/2 - OK" );
  test_mkdir("a/b/c/2", S_IRWXU, 0);
  puts( "rtems_mkdir a/b/c/3 - OK" );
  test_mkdir("a/b/c/3", S_IRWXG, 0);
  puts( "rtems_mkdir a/b/c/4 - OK" );
  test_mkdir("a/b/c/4", S_IRWXO, 0);
  puts( "rtems_mkdir a/b - OK" );
  test_mkdir("a/b", omode, 0);
  puts( "rtems_mkdir a - OK" );
  test_mkdir("a", omode, 0);
  puts( "rtems_mkdir a/b/x - OK" );
  test_mkdir("a/b/x", S_IRUSR, 0);
  puts( "rtems_mkdir a/b/x/y - expect failure" );
  test_mkdir("a/b/x/y", S_IRUSR, -1);
  puts( "mknod regular file a/n - OK" );  
  rv = mknod("a/n", S_IRWXU | S_IFREG, 0LL);
  puts( "rtems_mkdir a/n/b - expect failure" );
  test_mkdir("a/n/b", S_IRUSR, -1);

  puts( "Create node b and open in RDONLY mode - OK" );
  rv = open ("b", O_CREAT | O_RDONLY, omode);
  rtems_test_assert(rv >= 0);

  puts( "Closing b - OK" );
  rv = close(rv);
  rtems_test_assert(rv == 0);

  puts( "rtems_mkdir b - expect failure" );
  test_mkdir("b", omode, -1);
  rtems_test_assert(errno == EEXIST);

  puts("*** END OF TEST SPMKDIR ***");

  exit(0);
}

#define CONFIGURE_INIT

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM

#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 4

#define CONFIGURE_MAXIMUM_TASKS 1
#define CONFIGURE_MAXIMUM_DRIVERS 1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#include <rtems/confdefs.h>
