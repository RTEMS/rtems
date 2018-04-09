/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <bsp.h> /* for device driver prototypes */
#include "tmacros.h"
#include <rtems/imfs.h>
#include <rtems/error.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "tar02_tar.h"

const char rtems_test_name[] = "TAR 2";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
void test_tarfs_load(void);

#define TARFILE_START tar02_tar
#define TARFILE_SIZE  tar02_tar_size

static const char file[] = "/home/test_file";

static const char content_0[] =
  "This is a test of loading an RTEMS filesystem from an\n"
  "initial tar image.\n";

static const char content_1[] =
  "This is a test of loading an RTEMS filesystem from an\n"
  "initial tar image.\n"
  "And some other stuff.\n";

static char buf[sizeof(content_1) - 1];

static void check_file(
  const char *file,
  const char *expected_content,
  char *buf,
  size_t size
)
{
  int fd;
  ssize_t n;
  int rv;

  fd = open(file, O_RDONLY);
  rtems_test_assert(fd >= 0);

  n = read(fd, buf, size);
  rtems_test_assert(n == (ssize_t) size);

  rtems_test_assert(memcmp(expected_content, buf, size) == 0);

  rv = close(fd);
  rtems_test_assert(rv == 0);
}

static void write_file(
  const char *file,
  const char *content,
  size_t size
)
{
  int fd;
  ssize_t n;
  int rv;

  fd = open(file, O_WRONLY);
  rtems_test_assert(fd >= 0);

  n = write(fd, content, size);
  rtems_test_assert(n == (ssize_t) size);

  rv = close(fd);
  rtems_test_assert(rv == 0);
}

/* FIXME */
void test_cat(
  const char *file,
  int         offset_arg,
  int         length
);

void test_tarfs_load(void)
{
  rtems_status_code sc;

  printf("Loading tarfs image ... ");
  sc = rtems_tarfs_load("/",(void *)TARFILE_START, TARFILE_SIZE);
  if (sc != RTEMS_SUCCESSFUL) {
    printf ("error: untar failed: %s\n", rtems_status_text (sc));
    exit(1);
  }
  printf ("successful\n");

  /******************/
  printf( "========= /home/test_file =========\n" );
  test_cat(&file[0], 0, 0);
  check_file(&file[0], &content_0[0], &buf[0], sizeof(content_0) - 1);
  write_file(&file[0], &content_1[0], sizeof(content_1) - 1);
  check_file(&file[0], &content_1[0], &buf[0], sizeof(content_1) - 1);

  /******************/
  printf( "========= /symlink =========\n" );
  test_cat( "/symlink", 0, 0 );
}

rtems_task Init(
  rtems_task_argument ignored
)
{
  TEST_BEGIN();

  test_tarfs_load();

  TEST_END();
  exit( 0 );
}


/* NOTICE: the clock driver is explicitly disabled */
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS            1
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 5

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
