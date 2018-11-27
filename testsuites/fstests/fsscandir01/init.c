/*
 *  COPYRIGHT (c) 2015.
 *  On-Line Applications Research Corporation (OAR).
 *  
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <unistd.h>

#include <tmacros.h>

#include "fstest.h"
#include "fs_config.h"

const char rtems_test_name[] = "FSSCANDIR " FILESYSTEM;

#define FILE_NAME "aaa"

#define DIR_NAME "bbb"

void test(void)
{
  struct dirent **namelist;
  struct dirent *d;
  int rv;
  int n;
  int i;

  rtems_test_assert(MAXNAMLEN == NAME_MAX);

  rv = mknod(FILE_NAME, S_IFREG | S_IRWXU | S_IRWXG | S_IRWXO, 0);
  rtems_test_assert(rv == 0);

  rv = mkdir(DIR_NAME, S_IRWXU | S_IRWXG | S_IRWXO );
  rtems_test_assert(rv == 0);

  n = scandir(".", &namelist, NULL, alphasort);
  rtems_test_assert(2 <= n || n == 4);

  i = 0;
  d = namelist[i];

  if (n >= 3) {
    rtems_test_assert(strcmp(d->d_name, ".") == 0);
#ifdef DT_UNKNOWN
    rtems_test_assert(d->d_type == DT_DIR || d->d_type == DT_UNKNOWN);
#endif
    free(d);
    ++i;
    d = namelist[i];
  }

  if (n == 4) {
    rtems_test_assert(strcmp(d->d_name, "..") == 0);
#ifdef DT_UNKNOWN
    rtems_test_assert(d->d_type == DT_DIR || d->d_type == DT_UNKNOWN);
#endif
    free(d);
    ++i;
    d = namelist[i];
  }

  rtems_test_assert(strcmp(d->d_name, FILE_NAME) == 0);
#ifdef DT_UNKNOWN
  rtems_test_assert(d->d_type == DT_REG || d->d_type == DT_UNKNOWN);
#endif
  free(d);
  ++i;
  d = namelist[i];

  rtems_test_assert(strcmp(d->d_name, DIR_NAME) == 0);
#ifdef DT_UNKNOWN
  rtems_test_assert(d->d_type == DT_DIR || d->d_type == DT_UNKNOWN);
#endif
  free(d);

  free(namelist);
}
