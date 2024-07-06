/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
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
#include <limits.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "fstest.h"
#include "fs_config.h"
#include <tmacros.h>

const char rtems_test_name[] = "FSRENAMEPERMEXISTING " FILESYSTEM;
const RTEMS_TEST_STATE rtems_test_state = TEST_STATE;

static void rename_write_permission_test (void)
{
  int fd;
  int status;
  int rv;

  const char *name01 = "name01";
  const char *name02 = "name02";

  const char *dir01 = "dir01";
  const char *dir02 = "dir02";

  char path01[20];

  mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO;
  mode_t no_write_access = S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP
                         | S_IROTH | S_IXOTH;

  const char *wd = __func__;

  /*
   * Create a new directory and change the current directory to this
   */

  status = mkdir (wd, mode);
  rtems_test_assert (status == 0);
  status = chdir (wd);
  rtems_test_assert (status == 0);

  /*
   * The new argument points to a file and
   * the old argument points to another file,
   * both inside a directory with no write permission.
   */

  puts ("\nRename two files on a directory with no write permission \n");

  status = mkdir (dir01, mode);
  rtems_test_assert (status == 0);

  status = chdir (dir01);
  rtems_test_assert (status == 0);

  fd = creat (name01, mode);
  rtems_test_assert (fd >= 0);
  status = close (fd);
  rtems_test_assert (status == 0);

  fd = creat (name02, mode);
  rtems_test_assert (fd >= 0);
  status = close (fd);
  rtems_test_assert (status == 0);

  status = chmod (".", no_write_access);
  rtems_test_assert (status == 0);

  EXPECT_ERROR (EACCES, rename, name01 , name02);

  status = chdir ("..");
  rtems_test_assert (status == 0);

  /*
   * The new argument points to a file in a directory with no write access and
   * the old argument points to another file on a directory with write access.
   */

  puts ("\nRename file between two directories, with and without write access\n");

  status = mkdir (dir02, mode);
  rtems_test_assert (status == 0);

  status = chdir (dir02);
  rtems_test_assert (status == 0);

  fd = creat (name01, mode);
  rtems_test_assert (fd >= 0);
  status = close (fd);
  rtems_test_assert (status == 0);

  rv = snprintf (path01, sizeof(path01), "../%s/%s", dir01, name02);
  rtems_test_assert (rv < sizeof(path01));
  EXPECT_ERROR (EACCES, rename, name01, path01);

  /*
   * The new argument points to a file in a directory with write access and
   * the old argument points to another file on a directory without write access.
   */

  EXPECT_ERROR (EACCES, rename, path01, name01);

  /*
   * Clear directory
   */

  EXPECT_EQUAL (0, unlink, name01);

  rv = snprintf (path01, sizeof(path01), "../%s", dir01);
  rtems_test_assert (rv < sizeof(path01));
  status = chmod (path01, mode);
  rtems_test_assert (status == 0);

  rv = snprintf (path01, sizeof(path01), "../%s/%s", dir01, name01);
  rtems_test_assert (rv < sizeof(path01));
  EXPECT_EQUAL (0, unlink, path01);

  rv = snprintf (path01, sizeof(path01), "../%s/%s", dir01, name02);
  rtems_test_assert (rv < sizeof(path01));
  EXPECT_EQUAL (0, unlink, path01);

  status = chdir ("..");
  rtems_test_assert (status == 0);

  EXPECT_EQUAL (0, rmdir, dir01);
  EXPECT_EQUAL (0, rmdir, dir02);

  /*
   * Go back to parent directory
   */

  status = chdir ("..");
  rtems_test_assert (status == 0);

  /*
   * Remove test directory
   */

  status = rmdir (wd);
  rtems_test_assert (status == 0);
}

static void rename_search_permission_test (void)
{
  int fd;
  int status;
  int rv;

  const char *name01 = "name01";
  const char *name02 = "name02";

  const char *dir01 = "dir01";
  const char *dir02 = "dir02";

  char path01[20];
  char path02[20];

  mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO;
  mode_t no_execute_access = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP
                           | S_IROTH | S_IWOTH;

  const char *wd = __func__;

  /*
   * Create a new directory and change the current directory to this
   */

  status = mkdir (wd, mode);
  rtems_test_assert (status == 0);
  status = chdir (wd);
  rtems_test_assert (status == 0);

  /*
   * The new argument points to a file in a directory with no execute access and
   * the old argument points to another file on a directory with execute access.
   */

  puts ("\nRename file between two directories, with and without execute access\n");

  status = mkdir (dir01, mode);
  rtems_test_assert (status == 0);

  rv = snprintf (path01, sizeof(path01), "%s/%s", dir01, name01);
  rtems_test_assert (rv < sizeof(path01));
  fd = creat (path01, mode);
  rtems_test_assert (fd >= 0);
  status = close (fd);
  rtems_test_assert (status == 0);

  rv = snprintf (path02, sizeof(path02), "%s/%s", dir01, name02);
  rtems_test_assert (rv < sizeof(path02));
  fd = creat (path02, mode);
  rtems_test_assert (fd >= 0);
  status = close (fd);
  rtems_test_assert (status == 0);

  status = chmod (dir01, no_execute_access);
  rtems_test_assert (status == 0);

  status = mkdir (dir02, mode);
  rtems_test_assert (status == 0);

  status = chdir (dir02);
  rtems_test_assert (status == 0);

  fd = creat (name01, mode);
  rtems_test_assert (fd >= 0);
  status = close (fd);
  rtems_test_assert (status == 0);

  status = chdir ("..");
  rtems_test_assert (status == 0);

  rv = snprintf (path01, sizeof(path01), "%s/%s", dir02, name01);
  rtems_test_assert (rv < sizeof(path01));
  EXPECT_ERROR (EACCES, rename, path01, path02);

  /*
   * The new argument points to a file in a directory with execute access and
   * the old argument points to another file on a directory without execute access.
   */

  EXPECT_ERROR (EACCES, rename, path02, path01);

  /*
   * Clear directory
   */

  EXPECT_EQUAL (0, unlink, path01);

  status = chmod (dir01, mode);
  rtems_test_assert (status == 0);

  rv = snprintf (path01, sizeof(path01), "%s/%s", dir01, name01);
  rtems_test_assert (rv < sizeof(path01));
  EXPECT_EQUAL (0, unlink, path01);
  EXPECT_EQUAL (0, unlink, path02);
  EXPECT_EQUAL (0, rmdir, dir01);
  EXPECT_EQUAL (0, rmdir, dir02);

  /*
   * Go back to parent directory
   */

  status = chdir ("..");
  rtems_test_assert (status == 0);

  /*
   * Remove test directory
   */

  status = rmdir (wd);
  rtems_test_assert (status == 0);
}

static void rename_permission03 (void)
{
  int fd;
  int status;
  int rv;

  const char *name01 = "name01";
  const char *name02 = "name02";

  const char *dir01 = "dir01";

  char path01[30];

  mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO;
  const char *wd = __func__;

  /*
   * Create a new directory and change the current directory to this
   */

  status = mkdir (wd, mode);
  rtems_test_assert (status == 0);
  status = chdir (wd);
  rtems_test_assert (status == 0);

  /*
   * The new argument points to a file and
   * the old argument points to another file on a directory with S_ISVTX.
   */

  puts ("\nRename files within directories protected with S_ISVTX\n");

  status = mkdir (dir01, mode | S_ISVTX);
  rtems_test_assert (status == 0);

  rv = snprintf (path01, sizeof(path01), "%s/%s", dir01, name01);
  rtems_test_assert (rv < sizeof(path01));
  fd = creat (path01, mode);
  rtems_test_assert (fd >= 0);
  status = close (fd);
  rtems_test_assert (status == 0);

  fd = creat (name02, mode);
  rtems_test_assert (fd >= 0);
  status = close (fd);
  rtems_test_assert (status == 0);

  status = chown (path01, 65534, -1);
  rtems_test_assert (status == 0);

  status = chown (dir01, 65534, -1);
  rtems_test_assert (status == 0);

  EXPECT_EQUAL (-1, rename, path01, name02);

  puts("Testing errno for EPERM or EACCES");

  if (errno == EPERM || errno == EACCES) {
    FS_PASS ();
  } else {
    FS_FAIL ();
  }

  /*
   * Clear directory
   */

  EXPECT_EQUAL (0, unlink, path01);
  EXPECT_EQUAL (0, unlink, name02);
  EXPECT_EQUAL (0, rmdir, dir01);

  /*
   * The new argument points to a file on a directory with S_ISVTX and
   * the old argument points to a file outside that directory.
   */

  status = mkdir (dir01, mode | S_ISVTX);
  rtems_test_assert (status == 0);

  rv = snprintf (path01, sizeof(path01), "%s/%s", dir01, name01);
  rtems_test_assert (rv < sizeof(path01));
  fd = creat (path01, mode);
  rtems_test_assert (fd >= 0);
  status = close (fd);
  rtems_test_assert (status == 0);

  fd = creat (name02, mode);
  rtems_test_assert (fd >= 0);
  status = close (fd);
  rtems_test_assert (status == 0);

  status = chown (path01, 65534, -1);
  rtems_test_assert (status == 0);

  status = chown (dir01, 65534, -1);
  rtems_test_assert (status == 0);

  EXPECT_EQUAL (-1, rename, name02, path01);

  puts("Testing errno for EPERM or EACCES");

  if (errno == EPERM || errno == EACCES) {
    FS_PASS ();
  } else {
    FS_FAIL ();
  }

  /*
   * Clear directory
   */

  EXPECT_EQUAL (0, unlink, path01);
  EXPECT_EQUAL (0, unlink, name02);
  EXPECT_EQUAL (0, rmdir, dir01);

  /*
   * Go back to parent directory
   */

  status = chdir ("..");
  rtems_test_assert (status == 0);

  /*
   * Remove test directory
   */

  status = rmdir (wd);
  rtems_test_assert (status == 0);
}

void test (void)
{
  rename_write_permission_test ();
  rename_search_permission_test ();
  rename_permission03 ();
}
